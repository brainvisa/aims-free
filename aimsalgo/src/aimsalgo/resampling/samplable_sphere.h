
#ifndef AIMS_RESAMPLING_SAMPLABLE_SPHERE_H
#define AIMS_RESAMPLING_SAMPLABLE_SPHERE_H

#include <aims/resampling/samplable.h>

namespace aims
{

  class SphereSamplable : public aimsalgo::Samplable<float, 3>
  {
  public:
    SphereSamplable( const Point3df & center, float radius );
    virtual ~SphereSamplable() {}
    virtual bool contains( AimsVector<float,3> & vector )
    {
      // std::cout << "SphereSamplable::contains " << vector << std::endl;
      return (vector - _center).norm2() <= _sqrradius;
    }

  private:
    Point3df _center;
    float    _sqrradius;
  };


  inline SphereSamplable::SphereSamplable( const Point3df & center,
                                           float radius )
  : aimsalgo::Samplable<float, 3>(), _center( center ),
    _sqrradius( radius*radius )
  {}

}

#endif

