/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <limits>
#include <algorithm>
#include <cartobase/type/string_conversion.h>
#include <aims/features/roi_features.h>

using namespace std;
using namespace carto;

namespace aims {


  //---------------//
 //  RoiFeatures  //
//---------------//

//-----------------------------------------------------------------------------
void RoiFeatures::computeFeatures( const rc_ptr< RoiIterator > &roiIterator )
{
  const vector< string > &featureNames =
    _scalarSetFeatures.scalarFeatureNames();
  _result = Object::value( PropertySet() );
  while( roiIterator->isValid() ) {
    const Point3df &voxelSize = roiIterator->voxelSize();
    float voxelVolume = roiIterator->voxelVolume();
    Object features;
    _result->getProperty( roiIterator->regionName(), features );
    if ( features.isNull() ) {
      _result->setProperty( roiIterator->regionName(),
                            PropertySet() );
      features = _result->getProperty( roiIterator->regionName() );
    }

    rc_ptr< MaskIterator > maskIterator = roiIterator->maskIterator();
    size_t point_count = 0;
    while ( maskIterator->isValid() ) {
      ++point_count;
      maskIterator->next();
    }
    features->setProperty( "point_count", point_count );
    features->setProperty( "volume", (float)((double)point_count * (double)voxelVolume) );
    if ( ! _images.empty() ) {
      for( Images_t::const_iterator it = _images.begin();
           it != _images.end(); ++it ) {
        const rc_ptr< Interpolator > interpolator = getLinearInterpolator( it->second.first );
        rc_ptr< Interpolator > weight_interpolator( NULL );
        if ( ! it->second.second.empty() ) {
            weight_interpolator = getLinearInterpolator( it->second.second );
        }
        vector< Interpolator::Scalar_t > interpolated;
        Interpolator::Scalar_t weight;
        vector<  ScalarFeaturesProvider::Scalar_t > allValues;
        allValues.reserve( point_count );
        vector<  ScalarFeaturesProvider::Scalar_t > allWeights;
        allWeights.reserve( point_count );
        vector< vector<  ScalarFeaturesProvider::Scalar_t > > timesValues;
        for( maskIterator->restart(); maskIterator->isValid();
             maskIterator->next() ) {
          const Point3df &p = maskIterator->valueMillimeters();
          interpolator->values( p[ 0 ], p[ 1 ], p[ 2 ], interpolated );
          if ( ! weight_interpolator.isNull() ) {
              weight = weight_interpolator->value( p[ 0 ], p[ 1 ], p[ 2 ] );
          }
          if ( interpolated.size() > 1 ) {
            if ( timesValues.empty() ) {
              allValues.reserve( point_count * interpolated.size() );
              timesValues.resize( interpolated.size() );
              for( size_t i = 0; i < interpolated.size(); ++i ) {
                timesValues[ i ].reserve( point_count );
              }
            }
            for( size_t i = 0; i < interpolated.size(); ++i ) {
              timesValues[ i ].push_back( interpolated[ i ] );
              allValues.push_back( interpolated[ i ] );
              if ( ! weight_interpolator.isNull() ) {
                  allWeights.push_back( weight );
              }
            }
          } else {
            allValues.push_back( *interpolated.begin() );
            if ( ! weight_interpolator.isNull() ) {
                allWeights.push_back( weight );
            }
          }
        }

        Object o;
        features->getProperty( it->first, o );
        if ( o.isNull() ) {
          features->setProperty( it->first, PropertySet() );
          o = features->getProperty( it->first );
        }

        const PropertySet &header = interpolator->header();

        if ( weight_interpolator.isNull() ) {
          _scalarSetFeatures.setValues( allValues );
        } else {
          _scalarSetFeatures.setValues( allValues, allWeights );
        }
        _scalarSetFeatures.scalarFeatureValues( interpolated );
        for( size_t iName = 0; iName < featureNames.size(); ++iName ) {
          o->setProperty( featureNames[ iName ],
                          interpolated[ iName ] );
        }
        if ( ! timesValues.empty() ) {
          features->getProperty( "times", o );
          if ( o.isNull() ) {
            features->setProperty( "times", PropertySet() );
            o = features->getProperty( "times" );
          }
          Object o2;
          o->getProperty( it->first, o2 );
          if ( o2.isNull() ) {
            o->setProperty( it->first, PropertySet() );
            o2 = o->getProperty( it->first );
          }
          o2->setProperty( "abscissa",
                           vector< ScalarFeaturesProvider::Scalar_t >() );
          for( size_t iName = 0; iName < featureNames.size(); ++iName ) {
            o2->setProperty( featureNames[ iName ],
                             vector< ScalarFeaturesProvider::Scalar_t >() );
          }
          if ( header.hasProperty( "start_time" ) ) {
            o2->setProperty( "start_time", header.getProperty( "start_time" ) );
          }
          if ( header.hasProperty( "duration_time" ) ) {
            o2->setProperty( "duration_time", header.getProperty( "duration_time" ) );
          }
          for( size_t i = 0; i < timesValues.size(); ++i ) {
            o2->getProperty( "abscissa" )
              ->value< vector< ScalarFeaturesProvider::Scalar_t > >()
              .push_back( i );
            _scalarSetFeatures.setValues( timesValues[ i ] );
            _scalarSetFeatures.scalarFeatureValues( interpolated );
            for( size_t iName = 0; iName < featureNames.size(); ++iName ) {
              o2->getProperty( featureNames[ iName ] )
                ->value< vector< ScalarFeaturesProvider::Scalar_t > >()
                .push_back( interpolated[ iName ] );
            }
          }
        }
      }
    }
    roiIterator->next();
  }
}


//-----------------------------------------------------------------------------
void RoiFeatures::
addImageStatistics( const std::string &label,
                    const std::string &filename,
                    const std::string &weight_filename )
{
  if ( _images.find( label ) == _images.end() ) {
    _images[ label ] = std::pair<std::string, std::string>(filename, weight_filename);
  } else {
    throw runtime_error( string( "Cannot compute statistics because the label \"" ) + label + "\" is used for several images." );
  }
}

//-----------------------------------------------------------------------------
static void writeFeatures( const Object &features, ostream &out,
                           const string &indent )
{
  for( Object it = features->objectIterator(); it->isValid(); it->next() ) {
    out << indent << "'" << it->key() << "': ";
    Object v = it->currentValue();
    try {
      double d = v->getScalar();
      out << d  << "," << endl;
    } catch( exception & ) {
      if ( v->isArray() ) {
        out << "[ ";
        for( size_t i = 0; i < v->size(); ++i ) {
          out << v->getArrayItem( i )->getString() << ", ";
        }
        out << "]," << endl;
      } else {
        try {
          // Object it =  v->getInterface<DictionaryInterface>();
          out << "{" << endl;
          writeFeatures( v, out, indent + "  " );
          out << indent << "}," << endl;
        } catch( exception & ) {
          out << "'" << v->getString() << "'," << endl;
        }
      }
    }
  }
}


//-----------------------------------------------------------------------------
void RoiFeatures::writeMinf( ostream &out ) const
{
  out << "attributes = {" << endl
      << "  'format': 'features_1.0'," << endl
      << "  'content_type': 'roi_features'," << endl;
  writeFeatures( _result, out, "  " );
  out << "}" << endl;
}


class DoubleStringOrNone
{
public:
  inline DoubleStringOrNone() : type( 0 ) {}
  inline DoubleStringOrNone( double v ) : number( v ), type( 1 ) {}
  inline DoubleStringOrNone( const string &v ) : str( v ), type( 2 ) {}
  inline void write( ostream &out ) const {
    if ( type == 1 ) {
      out << number;
    } else if ( type == 2 ) {
      out << str;
    } else {
      out << "n/a";
    }
  }
  double number;
  string str;
  int type;
};


//-----------------------------------------------------------------------------
void RoiFeatures::writeCSV( ostream &out ) const
{
  list< string > csvHeader;
  list< map< string, DoubleStringOrNone > > csvValues;

  csvHeader.push_back( "ROI_label" );
  bool image_label = false;

  for( Object itRoi = _result->objectIterator(); itRoi->isValid(); itRoi->next() ) {
    const string roiName( itRoi->key() );
    if ( roiName == "format" || roiName == "content_type" ) continue;
    Object features = itRoi->currentValue();
    if ( features->isDictionary() ) {
      map< string, DoubleStringOrNone > roi_features;
      roi_features[ "ROI_label" ] = DoubleStringOrNone( roiName );
      for( Object itFeatures = features->objectIterator(); itFeatures->isValid(); itFeatures->next() ) {
        Object feature = itFeatures->currentValue();
        if ( feature->isScalar() ) {
          if ( find( csvHeader.begin(), csvHeader.end(), itFeatures->key() ) == csvHeader.end() ) {
            csvHeader.push_back( itFeatures->key() );
          }
          roi_features[ itFeatures->key() ] = DoubleStringOrNone( feature->getScalar() );
        }
      }
      bool add_roi_features = true;
      for( Object itFeatures = features->objectIterator(); itFeatures->isValid(); itFeatures->next() ) {
        Object feature = itFeatures->currentValue();
        if ( feature->isDictionary() ) {
          // Feature is an image
          map< string, DoubleStringOrNone > image_features = roi_features;
          image_features[ "image_label" ] = DoubleStringOrNone( itFeatures->key() );
          if ( ! image_label && ! itFeatures->key().empty() ) {
            csvHeader.push_back( "image_label" );
            image_label= true;
          }
          for( Object itImgFeatures = feature->objectIterator(); itImgFeatures->isValid(); itImgFeatures->next() ) {
            Object feature = itImgFeatures->currentValue();
            if ( feature->isScalar() ) {
              if ( find( csvHeader.begin(), csvHeader.end(), itImgFeatures->key() ) == csvHeader.end() ) {
                csvHeader.push_back( itImgFeatures->key() );
              }
              image_features[ itImgFeatures->key() ] = DoubleStringOrNone( feature->getScalar() );
            }
          }
          csvValues.push_front( image_features );
          add_roi_features = false;
          if ( feature->hasProperty( itFeatures->key() ) ) {
            Object time_features = feature->getProperty( itFeatures->key() ) ;
            if ( time_features->isDictionary() ) {
              list< string > time_feature_names;
              unsigned int time_steps = 0;
              for( Object itTimeFeatures = time_features->objectIterator(); itTimeFeatures->isValid(); itTimeFeatures->next() ) {
                if ( itTimeFeatures->currentValue()->isArray() ) {
                  bool ignore = true;
                  if ( time_steps == 0 ) {
                    time_steps = itTimeFeatures->currentValue()->size();
                    ignore = false;
                  } else if ( time_steps == itTimeFeatures->currentValue()->size() ) {
                    ignore = false;
                  }
                  if ( ignore ) {
                    cerr << "Warning: ignoring time feature " << itTimeFeatures->key() << " for image label " << itFeatures->key() << endl;
                  } else {
                    if ( find( csvHeader.begin(), csvHeader.end(), itTimeFeatures->key() ) == csvHeader.end() ) {
                      csvHeader.push_back( itTimeFeatures->key() );
                    }
                    time_feature_names.push_back( itTimeFeatures->key() );
                  }
                }
              }
              for( unsigned int time_step = 0; time_step < time_steps; ++time_step ) {
                map< string, DoubleStringOrNone > time_features_line = roi_features;
                time_features_line[ "image_label" ] = DoubleStringOrNone( itFeatures->key() );
                for( list< string >::const_iterator it = time_feature_names.begin(); it != time_feature_names.end(); ++it ) {
                  time_features_line[ *it ] = DoubleStringOrNone( time_features->getProperty( *it )->getArrayItem( time_step )->getScalar() );
                }
                csvValues.push_back( time_features_line );
                add_roi_features = false;
              }
            }
          }
        }
      }
      if ( add_roi_features ) csvValues.push_back( roi_features );
    } else {
      cerr << "Cannot write features in CSV format for ROI " << roiName << " because it contains data of type " << features->type() << endl;
    }
  }

  bool first = true;
  for( list< string >::const_iterator itH = csvHeader.begin(); itH != csvHeader.end(); ++itH ) {
    if ( first ) {
      first = false;
    } else {
      out << "\t" ;
    }
    out << *itH;
  }
  out << endl;
  for( list< map< string, DoubleStringOrNone > >::const_iterator itL = csvValues.begin(); itL != csvValues.end(); ++itL ) {
    first = true;
    for( list< string >::const_iterator itH = csvHeader.begin(); itH != csvHeader.end(); ++itH ) {
      if ( first ) {
        first = false;
      } else {
        out << "\t" ;
      }
      map< string, DoubleStringOrNone >::const_iterator itV = (*itL).find( *itH );
      if ( itV != itL->end() ) {
        const DoubleStringOrNone &v = itV->second;
        v.write( out );
      }
      else {
        out << "n/a";
      }
    }
    out << endl;
  }
}


//-----------------------------------------------------------------------------
void RoiFeatures::write( ostream &out, const string &format ) const
{
  if ( format == "csv" ) {
    writeCSV( out );
  } else {
    writeMinf( out );
  }
}


} // namespace aims
