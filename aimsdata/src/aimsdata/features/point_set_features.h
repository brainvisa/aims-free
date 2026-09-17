#ifndef AIMS_POINT_SET_FEATURES_H
#define AIMS_POINT_SET_FEATURES_H


#include <aims/features/scalar_set_features.h>
#include <aims/vector/vector.h>
#include <aims/resampling/linearInterpolator.h>


namespace aims {

  //--------------------//
 //  PointSetFeatures  //
//--------------------//

class PointSetFeatures : public ScalarFeaturesProvider
{
public:
  typedef Interpolator::Point_t Point_t;

  virtual void setPoints( const std::vector< Point_t > & ) = 0;
};


  //-------------------------//
 //  PointsInImageFeatures  //
//-------------------------//

class PointsInImageFeatures : public PointSetFeatures
{
public:
  PointsInImageFeatures( const std::string &prefix,
                         const carto::rc_ptr< Interpolator > &  );
  
  inline const std::vector< Point_t > &
  points() const;
  inline std::vector< Point_t > &points();
  void setPoints( const std::vector< Point_t > & );

  void scalarFeatureValues( 
    std::vector< ScalarFeaturesProvider::Scalar_t > &result ) const;

private:

  carto::rc_ptr< Interpolator > _interpolator;  
  std::vector< Point_t > _points;
};



  //-------------------------//
 //  PointsInImageFeatures  //
//-------------------------//

//-----------------------------------------------------------------------------
inline const std::vector< PointsInImageFeatures::Point_t > &
PointsInImageFeatures::points() const
{
  return _points;
}


//-----------------------------------------------------------------------------
inline std::vector< PointsInImageFeatures::Point_t > &
PointsInImageFeatures::points()
{
  return _points;
}




} // namesapce aims

#endif // ifndef AIMS_POINT_SET_FEATURES_H
