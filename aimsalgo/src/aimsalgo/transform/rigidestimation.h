#ifndef AIMS_TRANSFORM_RIGIDESTIMATION_H
#define AIMS_TRANSFORM_RIGIDESTIMATION_H

#include <aims/vector/vector.h>
#include <cartodata/volume/volume.h>
#include <vector>


namespace aims
{
  class AffineTransformation3d ;

  class RigidTransformEstimation
  {
  public:
    RigidTransformEstimation() ;
    RigidTransformEstimation( const std::vector<Point3df> & from,
                              const std::vector<Point3df> & to,
                              bool looseCondition = true ) ;
    
    ~RigidTransformEstimation() ;
    
    void setAppariatedPoints( const std::vector<Point3df> & from,
                              const std::vector<Point3df> & to ) ;
    void setLooseCondition( bool looseCondition ) ;
    
    bool motion(AffineTransformation3d&) ;
    
    static Point3df cross( const Point3df& u, const Point3df& v ) ;
    void rotationEstimation( const Point3df& u1, const Point3df& u2,
                             const Point3df& n1, const Point3df& n2,
                             Point3df& axis, float& ang ) ;

    static carto::VolumeRef<float> criterionItem(
      const Point3df& p1, const Point3df& p2,
      const Point3df& gc1, const Point3df& gc2, float weight ) ;
  private:
    std::vector<Point3df> _pointsFrom ;
    std::vector<Point3df> _pointsTo ;
    bool _looseCondition ;
    
    
    AffineTransformation3d * _motion ;
    bool _motionCalculated ;
    bool _is2D ;
    
    void looseEstimation() ;
    void pointToPointEstimation() ;
  } ;
}

#endif
