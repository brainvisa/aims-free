
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/math/distspheric.h>
#include <aims/vector/vector.h>
#include <cartodata/volume/volume.h>
#include <aims/math/mathelem.h>
#include <aims/math/random.h>
#include <math.h>

using namespace aims;
using namespace carto;

AimsVector<float,3> AimsPointInSolidAngle(const AimsVector<float,3> &vec,
                                          float theta_max)
{ AimsVector<float,3> nvec(vec);
  float cos_theta1,sin_theta1,cos_theta2,sin_theta2,phi,theta;

  nvec /= norm(nvec);

  if ( (square(nvec.item(0))+square(nvec.item(1))) != 0 )
  { cos_theta1 = nvec.item(1) / sqrt(square(nvec.item(0))+square(nvec.item(1)));
    sin_theta1 = nvec.item(0) / sqrt(square(nvec.item(0))+square(nvec.item(1)));

    cos_theta2 = nvec.item(2) / sqrt(square(nvec.item(0)) +
                                     square(nvec.item(1)) +
                                     square(nvec.item(2)) );
    sin_theta2 = sqrt(square(nvec.item(0)) + square(nvec.item(1))) / 
                 sqrt(square(nvec.item(0)) +
                      square(nvec.item(1)) +
                      square(nvec.item(2)) );
    VolumeRef<float> kernel( 3, 3, 1, 1, AllocatorContext::fast() );
    kernel(0,0) = cos_theta1;
    kernel(0,1) = cos_theta2 * sin_theta1;
    kernel(0,2) = sin_theta2 * sin_theta1;
    kernel(1,0) = - sin_theta1;
    kernel(1,1) = cos_theta2 * cos_theta1;
    kernel(1,2) = sin_theta2 * cos_theta1;
    kernel(2,0) = 0;
    kernel(2,1) = - sin_theta2;
    kernel(2,2) = cos_theta2;

#if defined(__GNUC__) && defined (__sun__)
    theta = ::asin(sin(theta_max) * (2 * UniformRandom() - 1));
#else
    theta = asin(sin(theta_max) * (2 * UniformRandom() - 1));
#endif
    phi   = UniformRandom() * 2 * M_PI;

    VolumeRef<float> ran( 3, 1, 1, 1, AllocatorContext::fast() );
    ran(0) = cos(phi) * sin(theta);
    ran(1) = sin(phi) * sin(theta);
    ran(2) = cos(theta);
    VolumeRef<float> res = matrix_product( kernel, ran );
    return(AimsVector<float,3>(res(0),res(1),res(2))*norm(nvec));
  }
  else
  {
#if defined(__GNUC__) && defined (__sun__)
    theta = ::asin(sin(theta_max) * (2 * UniformRandom() - 1));
#else
    theta = asin(sin(theta_max) * (2 * UniformRandom() - 1));
#endif
    phi   = UniformRandom() * 2 * M_PI;
    AimsVector<float,3> ran;
    ran.item(0) = cos(phi) * sin(theta);
    ran.item(1) = sin(phi) * sin(theta);
    ran.item(2) = cos(theta);
    return(ran*norm(nvec));
  }
  return(AimsVector<float,3>(0,0,0));
}
