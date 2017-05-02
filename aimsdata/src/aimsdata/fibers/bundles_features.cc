/* Copyright (c) 1995-2017 CEA
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


#include <aims/fibers/bundles_features.h>
#include <cartobase/object/pythonwriter.h>

using namespace std;
using namespace carto;

namespace aims {


  //-------------------//
 //  BundlesFeatures  //
//-------------------//


//-----------------------------------------------------------------------------
BundlesFeatures::BundlesFeatures()
{
  _result = Object::value( PropertySet() );
}

//-----------------------------------------------------------------------------
void BundlesFeatures::
addImageStatistics( const std::string &prefix, 
                    const rc_ptr< Interpolator > &interpolator )
{
  _interpolators[ prefix ] = interpolator;
}


//-----------------------------------------------------------------------------
void BundlesFeatures::bundleStarted( const BundleProducer &, 
                                     const BundleInfo & )
{
  _currentBundleFeaturesValues.clear();
  _fiberCount = 0;
}


//-----------------------------------------------------------------------------
void BundlesFeatures::bundleTerminated( const BundleProducer &,
                                        const BundleInfo &bundle )
{
  Object bundleFeatures;
  if( !_result->getProperty( bundle.name(), bundleFeatures ) )
  {
    bundleFeatures = Object::value( PropertySet() );
    _result->setProperty( bundle.name(), bundleFeatures );
  }
  bundleFeatures->setProperty( "numberOfFascicles", _fiberCount );
  for( String_ScalarSet_t::const_iterator it = 
         _currentBundleFeaturesValues.begin();
       it != _currentBundleFeaturesValues.end(); ++it ) {
    _scalarSetFeatures.setValues( it->second );
    const vector< string > &featureNames = 
      _scalarSetFeatures.scalarFeatureNames();;
    vector< ScalarFeaturesProvider::Scalar_t > featureValues;
    _scalarSetFeatures.scalarFeatureValues( featureValues );
    
    Object values;
    bundleFeatures->getProperty( it->first, values );
    if ( values.isNull() ) {
      values = Object::value( PropertySet() );
      bundleFeatures->setProperty( it->first, values );
    }
    for( size_t i = 0; i < featureNames.size(); ++i ) {
      values->setProperty( featureNames[ i ], featureValues[ i ] );
    }
  }
}



//-----------------------------------------------------------------------------
void BundlesFeatures::fiberStarted( const BundleProducer &, 
                                    const BundleInfo &,
                                    const FiberInfo & )
{
  ++_fiberCount;
  // Initialise current curve length
  _currentLength = 0;
  _firstPoint = true;
}



//-----------------------------------------------------------------------------
void BundlesFeatures::fiberTerminated( const BundleProducer &, 
                                        const BundleInfo &,
                                        const FiberInfo & )
{
  // Adds the current curve length to the set of length. This
  // set will be used to obtain features (via ScalarSetFeatures)
  // when bundle is terminated.
  _currentBundleFeaturesValues[ "length" ].push_back( _currentLength );
}


//-----------------------------------------------------------------------------
void BundlesFeatures::newFiberPoint( const BundleProducer &, 
                                      const BundleInfo &,
                                      const FiberInfo &, 
                                      const FiberPoint &point )
{
  // Update current curve's length
  if ( _firstPoint ) {
    _lastPoint = point;
    _firstPoint = false;
  } else {
    _currentLength += ( point - _lastPoint ).norm();
    _lastPoint = point;
  }

  // Interpolate point in each interpolator and adds the value into the
  // corresponding set of values. This set of values will be used to
  // obtain features (via ScalarSetFeatures) when bundle is terminated.
  for( Interpolators_t::const_iterator it = _interpolators.begin();
       it != _interpolators.end(); ++it ) {
    _currentBundleFeaturesValues[ it->first ]
      .push_back( (*it->second).value( point ) );
  }
}


//-----------------------------------------------------------------------------
void BundlesFeatures::noMoreBundle( const BundleProducer & )
{
}







//-----------------------------------------------------------------------------
void  BundlesFeatures::startBundleSlicing( BundlesSlicer &,
                                           const BundleInfo & )
{
}

//-----------------------------------------------------------------------------
void BundlesFeatures::terminateBundleSlicing( BundlesSlicer &,
                                              const BundleInfo & )
{
}


//-----------------------------------------------------------------------------
void  BundlesFeatures::
newBundleSlice( BundlesSlicer &,
                const BundleInfo &bundle, float abscissa,
                const std::vector< FiberPoint > &points )
{
  vector< ScalarFeaturesProvider::Scalar_t > slicePointValues( points.size() );
  vector< ScalarFeaturesProvider::Scalar_t > featureValues;
  
  if ( ! _result->hasProperty( bundle.name() )  ) {
    _result->setProperty( bundle.name(), PropertySet() );
  }
  Object features = _result->getProperty( bundle.name() );
  for( Interpolators_t::const_iterator itInterp = _interpolators.begin();
       itInterp != _interpolators.end(); ++itInterp ) {
    Object o;

    features->getProperty( itInterp->first, o );
    if ( o.isNull() ) {
      features->setProperty( itInterp->first, PropertySet() );
      o = features->getProperty( itInterp->first );
    }

    if ( ! o->hasProperty( "_vectors" ) ) {
      o->setProperty( "_vectors", PropertySet() );
    }
    o = o->getProperty( "_vectors" );

    if ( ! o->hasProperty( "abscissa" ) ) {
      o->setProperty( "abscissa", 
              Object::value( vector< ScalarFeaturesProvider::Scalar_t >() ) );
    }
    o->getProperty( "abscissa" )->
      value< vector< ScalarFeaturesProvider::Scalar_t > >()
      .push_back( abscissa );

    for( size_t iPoint = 0; iPoint < points.size(); ++iPoint ) {
      slicePointValues[ iPoint ] = itInterp->second->value( points[ iPoint ] );
    } 
    _scalarSetFeatures.setValues( slicePointValues );
    const vector< string > &featureNames = 
      _scalarSetFeatures.scalarFeatureNames();
    _scalarSetFeatures.scalarFeatureValues( featureValues );
    for( size_t iName = 0; iName < featureNames.size(); ++iName ) {
      if ( ! o->hasProperty( featureNames[ iName ] ) ) {
        o->setProperty( featureNames[ iName ], 
              Object::value( vector< ScalarFeaturesProvider::Scalar_t >() ) );
    }
    o->getProperty( featureNames[ iName ] )->
      value< vector< ScalarFeaturesProvider::Scalar_t > >()
      .push_back( featureValues[ iName ] );
    }
  }
}

//-----------------------------------------------------------------------------
void  BundlesFeatures::noMoreBundleSlice( BundlesSlicer & )
{
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
void BundlesFeatures::write( ostream &out ) const
{
  out << "attributes = {" << endl
      << "  'format': 'features_1.0'," << endl
      << "  'content_type': 'bundles_features'," << endl;
  writeFeatures( _result, out, "  " );
  out << "}" << endl;
}


//-----------------------------------------------------------------------------
void BundlesFeatures::write( const string &fileName ) const
{
  ofstream out( fileName.c_str() );
  write( out );
}


} // namespace aims
