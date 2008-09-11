
namespace aims
{

class ConeSamplable : aimsalgo::Samplable_FLOAT_3
{
%TypeHeaderCode
#include <aims/resampling/samplable_cone.h>
namespace aimsalgo
{
#ifndef PYAIMSALGOSIP_SAMPLABLE_FLOAT_3_DEFINED
#define PYAIMSALGOSIP_SAMPLABLE_FLOAT_3_DEFINED
 typedef aimsalgo::Samplable<float,3> Samplable_FLOAT_3;
#endif
}
%End

  public:
  ConeSamplable( const Point3df & arrow, const Point3df & base, float radius );
  virtual ~ConeSamplable();
  virtual bool contains( Point3df & vector );
};


class SphereSamplable : aimsalgo::Samplable_FLOAT_3
{
%TypeHeaderCode
#include <aims/resampling/samplable_sphere.h>
namespace aimsalgo
{
#ifndef PYAIMSALGOSIP_SAMPLABLE_FLOAT_3_DEFINED
#define PYAIMSALGOSIP_SAMPLABLE_FLOAT_3_DEFINED
 typedef aimsalgo::Samplable<float,3> Samplable_FLOAT_3;
#endif
}
%End

  public:
  SphereSamplable( const Point3df & center, float radius );
  virtual ~SphereSamplable();
  virtual bool contains( Point3df & vector );
};

};

