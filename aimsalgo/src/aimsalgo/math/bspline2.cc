
#include <cstdlib>
#include <aims/math/mathelem.h>

using namespace aims;

float betaSpline2( float u )
{
  float valabs = fabs( u );
  if ( valabs <= 0.5 )
    return 0.75 - sqr( valabs ) ;
  else if ( valabs <= 1.5 )
    return 0.125 * ( 9 - 12 * valabs +4 * sqr( valabs ) ) ;
  return 0.0;
}
