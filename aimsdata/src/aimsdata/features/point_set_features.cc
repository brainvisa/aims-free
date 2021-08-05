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

// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/features/point_set_features.h>
#include <aims/io/process.h>


using namespace std;
using namespace carto;

namespace aims {


  //-------------------------//
 //  PointsInImageFeatures  //
//-------------------------//

//-----------------------------------------------------------------------------
PointsInImageFeatures::PointsInImageFeatures( const std::string &prefix,
					      const rc_ptr< Interpolator > &interpolator ) :
 _interpolator( interpolator )
{
  // Create features names
  ScalarSetFeatures scalarFeatures;
  if ( prefix.empty() ) {
    _scalarFeatureNames = scalarFeatures.scalarFeatureNames();
  } else {
    _scalarFeatureNames.reserve( scalarFeatures.scalarFeatureNames().size() );
    for( std::vector< std::string >::const_iterator it =
	   scalarFeatures.scalarFeatureNames().begin(); 
	 it != scalarFeatures.scalarFeatureNames().end(); ++it ) {
      _scalarFeatureNames.push_back( prefix + "_" + *it );
    }
  }
}



//-----------------------------------------------------------------------------
void PointsInImageFeatures::
setPoints( const std::vector< PointsInImageFeatures::Point_t > &
	   points )
{
  _points = points;
}


//-----------------------------------------------------------------------------
void PointsInImageFeatures::
scalarFeatureValues( std::vector< ScalarFeaturesProvider::Scalar_t > &result )
  const
{
  ScalarSetFeatures ssf;
  ssf.values().reserve( _points.size() );
  for( std::vector< Point_t >::const_iterator it = _points.begin();
       it != _points.end(); ++it ) {
    if ( _interpolator->isValid( *it ) ) {
      ssf.values().push_back( _interpolator->value( *it ) );
    }
  }
  ssf.scalarFeatureValues( result );
}




} // namespace aims
