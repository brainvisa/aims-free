// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
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
