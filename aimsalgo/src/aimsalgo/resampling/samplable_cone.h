
#ifndef AIMS_RESAMPLING_SAMPLABLE_CONE_H
#define AIMS_RESAMPLING_SAMPLABLE_CONE_H

#include <aims/resampling/samplable.h>

namespace aims
{

  class ConeSamplable : public aimsalgo::Samplable<float, 3>
  {
  public:
    ConeSamplable( const Point3df & arrow, const Point3df & base, 
                   float radius );
    virtual ~ConeSamplable() {}
    virtual bool contains( AimsVector<float,3> & vector )
    {
      // std::cout << "ConeSamplable::contains " << vector << std::endl;
      float pos = vector.dot( _direction );
      if( pos >= _proj0 and pos <= _proj1 )
      {
        float rmax = (pos - _proj0) / (_proj1 - _proj0) * _radius;
        float r = ( vector - _cax0 - _direction * pos ).norm();
        if( r <= rmax )
          return true;
      }
      return false;
    }

  private:
    Point3df _arrow;
    Point3df _base;
    float    _radius;

    Point3df _direction;
    float    _proj0;
    float    _proj1;
    Point3df _cax0;
  };


  inline ConeSamplable::ConeSamplable( const Point3df & arrow,
                                       const Point3df & base, float radius )
  : aimsalgo::Samplable<float, 3>(), _arrow( arrow ), _base( base ), 
    _radius( radius )
  {
    _direction = base - arrow;
    _direction /= _direction.norm();
    _proj0 = _direction.dot( arrow );
    _proj1 = _direction.dot( base );
    _cax0 = _arrow - _direction * _proj0;
  }

}

#endif

