/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


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
    Object features;
    _result->getProperty( roiIterator->regionName(), features );
    if ( features.isNull() ) {
      _result->setProperty( roiIterator->regionName(),
                            PropertySet() );
      features = _result->getProperty( roiIterator->regionName() );
    }

    vector< ScalarFeaturesProvider::Scalar_t > featureValues;
    
    rc_ptr< MaskIterator > maskIterator = roiIterator->maskIterator();
    size_t point_count = 0;
    while ( maskIterator->isValid() ) {
      ++point_count;
      maskIterator->next();
    }
    features->setProperty( "point_count", point_count );
    features->setProperty( "volume", point_count * voxelSize[ 0 ] *
                           voxelSize[ 1 ] * voxelSize[ 2 ] );
    if ( ! _interpolators.empty() ) {
      for( Interpolators_t::const_iterator it = _interpolators.begin();
           it != _interpolators.end(); ++it ) {
        vector< Interpolator::Scalar_t > interpolated;
        vector<  ScalarFeaturesProvider::Scalar_t > allValues;
        allValues.reserve( point_count );
        vector< vector<  ScalarFeaturesProvider::Scalar_t > > timesValues;
        for( maskIterator->restart(); maskIterator->isValid();
             maskIterator->next() ) {
          const Point3df &p = maskIterator->valueMillimeters();
          (*it->second)( p[ 0 ], p[ 1 ], p[ 2 ], interpolated );
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
            }
          } else {
            allValues.push_back( *interpolated.begin() );
          }
        }
      
        Object o;
        features->getProperty( it->first, o );
        if ( o.isNull() ) {
          features->setProperty( it->first, PropertySet() );
          o = features->getProperty( it->first );
        }
        _scalarSetFeatures.setValues( allValues );
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
addImageStatistics( const std::string &prefix, 
                    const rc_ptr< Interpolator > &interpolator )
{
  if ( _interpolators.find( prefix ) == _interpolators.end() ) {
    _interpolators[ prefix ] = interpolator;
  } else {
    throw runtime_error( string( "Cannot compute statistics because the label \"" ) + prefix + "\" is used for several images." );
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
      try {
        vector< ScalarFeaturesProvider::Scalar_t > *pVector =
          &v->value<vector< ScalarFeaturesProvider::Scalar_t > >();
        out << "[ ";
        for( size_t i = 0; i < pVector->size(); ++i ) {
          out << (*pVector)[ i ] << ", ";
        }
        out << "]," << endl;
      } catch( exception & ) {
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


//-----------------------------------------------------------------------------
void RoiFeatures::writeCSV( ostream &out ) const
{
  list< string > roiNames;
  list< string > csvHeader;
  map< string, map< string, double > > csvValues;

  for( Object itRoi = _result->objectIterator(); itRoi->isValid(); itRoi->next() ) {
    const string roiName( itRoi->key() );
    if ( roiName == "format" || roiName == "content_type" ) continue;
    Object features = itRoi->currentValue();
    if ( features->isDictionary() ) {
      roiNames.push_back( roiName );
      list< pair< string, Object > > stack;
      stack.push_back( pair< string, Object >( "", features ) );
      while( ! stack.empty() ) {
        const string key = stack.front().first;
        const Object feature = stack.front().second;
        stack.pop_front();
        if ( feature->isScalar() ) {
          if ( find( csvHeader.begin(), csvHeader.end(), key ) == csvHeader.end() ) {
            csvHeader.push_back( key );
          }
          const double value = feature->getScalar();
          map< string, double > &values = csvValues[ roiName ];
          if ( values.find( key ) == values.end() ) {
            values[ key ] = value;
          } else {
            throw runtime_error( string( "Cannot write CVS file because there is more than one value in column \"" ) + key + "\" for region \"" + roiName + "\"" );
          }
        } else if ( feature->isDictionary() ) {
          for( Object itFeature = feature->objectIterator(); itFeature->isValid(); itFeature->next() ) {
            const string key2( ( key.empty() ? string() : key + "." ) + itFeature->key() );
            stack.push_front( pair< string, Object >( key2, itFeature->currentValue() ) );
          }
        } else {
          cerr << "Warning: ignoring feature of type " << feature->type() << " for key " << key << endl;
        }
      }
    } else {
      cerr << "Cannot write features in CSV format for ROI " << roiName << " because it contains data of type " << features->type() << endl;
    }
  }

  out << "ROI_label";
  for( list< string >::iterator itH = csvHeader.begin(); itH != csvHeader.end(); ++itH ) {
    out << "\t" << *itH;
  }
  out << endl;
  for( list< string >::iterator itR = roiNames.begin(); itR != roiNames.end(); ++itR ) {
    out << *itR;
    for( list< string >::iterator itH = csvHeader.begin(); itH != csvHeader.end(); ++itH ) {
      const map< string, double > &values = csvValues[ *itR ];
      map< string, double >::const_iterator itV = values.find( *itH );
      if ( itV != values.end() ) {
        out << "\t" << itV->second;
      } else {
        out << "\t";
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
