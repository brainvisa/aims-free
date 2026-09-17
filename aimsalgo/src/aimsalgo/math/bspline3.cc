
#include <cstdlib>
#include <aims/math/mathelem.h>

using namespace aims;

float betaSpline3( float u )
{
  float valabs = fabs( u );
  if ( valabs <= 1 )
    return 0.5 * cub( valabs ) - sqr( valabs ) + 2.0 / 3.0;
  else if ( valabs > 1 && valabs <= 2 )
    return cub( 2.0 - valabs ) / 6.0;
  return 0.0;
}
