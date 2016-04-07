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

#include <aims/features/scalar_set_features.h>
#include <math.h>
#include <algorithm>
#include <stdexcept>

using namespace std;


namespace aims {


  //---------------------//
 //  ScalarSetFeatures  //
//---------------------//

//-----------------------------------------------------------------------------
void ScalarSetFeatures::_setScalarFeatureNames()
{
  _scalarFeatureNames.clear();
  _scalarFeatureNames.reserve( 5 );
  _scalarFeatureNames.push_back( "mean" );
  _scalarFeatureNames.push_back( "stddev" );
  _scalarFeatureNames.push_back( "min" );
  _scalarFeatureNames.push_back( "max" );
  _scalarFeatureNames.push_back( "median" );
}


//-----------------------------------------------------------------------------
ScalarSetFeatures::ScalarSetFeatures()
{
  _setScalarFeatureNames();
}


//-----------------------------------------------------------------------------
ScalarSetFeatures::ScalarSetFeatures( const std::vector< Scalar_t > &values )
{
  _setScalarFeatureNames();
  setValues( values );
}


//-----------------------------------------------------------------------------
void ScalarSetFeatures::setValues( const std::vector< Scalar_t > &values )
{
  _values = values;
}

//-----------------------------------------------------------------------------
void ScalarSetFeatures::setValues( const std::vector< Scalar_t > &values,
                                   const std::vector< Scalar_t > &weights
)
{
  _values = values;
  _weights = weights;
}

//-----------------------------------------------------------------------------
void ScalarSetFeatures::scalarFeatureValues( 
  vector< ScalarFeaturesProvider::Scalar_t > &result) const
{
  result.resize( 5 );
  Scalar_t &mean = result[ 0 ];
  Scalar_t &stddev = result[ 1 ];
  Scalar_t &minimum = result[ 2 ];
  Scalar_t &maximum = result[ 3 ];
  Scalar_t &median = result[ 4 ];

  double sumWeight = 0;
  double product = 0;
  
  if ( _values.empty() ) {
    mean = stddev = minimum = maximum = median = 0;
  } else if ( _values.size() == 1 ) {
    mean = minimum = maximum = median = _values[ 0 ];
    stddev = 0;
  } else {
    vector< Scalar_t >::iterator itValue = _values.begin();
    const bool has_weights = _weights.begin() != _weights.end();
    if ( has_weights ) {
      vector< Scalar_t >::const_iterator itWeight = _weights.begin();
      mean = minimum = maximum = ( *itWeight ) * ( *itValue );
      sumWeight = ( *itWeight );
      ++itWeight;
      ++itValue;
      for( itValue; itValue != _values.end(); ++itValue ) {
        if ( itWeight == _weights.end() ) {
          throw runtime_error( "Weight data don't have the same size as images" );
        }
        product = ( *itWeight ) * ( *itValue );
        mean += product;
        sumWeight += ( *itWeight );
        if ( product > maximum ) maximum = product;
        if ( product < minimum ) minimum = product;
        ++itWeight;
      }
      mean /= sumWeight;
      stddev = 0;
      itWeight = _weights.begin();
      for( itValue = _values.begin(); itValue != _values.end(); ++itValue ) {
        stddev += ( *itWeight ) * (( *itValue - mean ) * ( *itValue - mean ));
        *itValue *= *itWeight;
        ++itWeight;
      }
      stddev = sqrt( stddev / sumWeight );
    } else {
      mean = minimum = maximum = *itValue;
      for( ++itValue; itValue != _values.end(); ++itValue ) {
        mean += *itValue;
        if ( *itValue > maximum ) maximum = *itValue;
        if ( *itValue < minimum ) minimum = *itValue;
      }
      mean /= _values.size();
      stddev = 0;
      for( itValue = _values.begin(); itValue != _values.end(); ++itValue ) {
        stddev += ( *itValue - mean ) * ( *itValue - mean );
      }
      stddev = sqrt( stddev / _values.size() );
    }
    // Compute median
    sort( _values.begin(), _values.end() );
    if ( _values.size() & 1 ) {
      // Odd number of values
      median = _values[ (size_t) _values.size() / 2 ];
    } else {
      // Even number of values
      const size_t i = (size_t) _values.size() / 2;
      median = ( _values[ i ] + _values[ i + 1 ] ) / 2;
    }
  }
}


} // namespace aims
