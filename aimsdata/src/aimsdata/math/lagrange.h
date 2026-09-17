/*
 *  Lagrange's interpolation.
 */
#ifndef AIMS_MATH_LAGRANGE_H
#define AIMS_MATH_LAGRANGE_H

#include <cstdlib>
#include <aims/config/aimsdata_config.h>
#include <cartobase/exception/assert.h>
#include <vector>
#include <math.h>

namespace aims
{

/**@name Lagrange's polynomial interpolation.
   This implementation uses Neville's algorithm that is a recursive
    way.
*/
//@{
/// Returns the interpolation of a function defined at (xa,ya) points at x (dy is the error)
template <class REAL>
AIMSDATA_API REAL AimsLagrangeInterpolation( const std::vector<REAL> &xa,
                                             const std::vector<REAL> &ya,
                                             REAL x, REAL *dy )

//@}
{
  int  i,m,ns=0,n=std::min( xa.size(), ya.size() );
  REAL den,dif,dift,ho,hp,w;
  REAL *c, *d, y;

  dif = fabs(x-xa[0]);
  c = new float[n];
  d = new float[n];

  for (i=0;i<n;i++) { 
    if ((dift = fabs(x-xa[i])) < dif)
    {
      ns  = i;
      dif = dift;
    }
    c[i] = ya[i];
    d[i] = ya[i];
  }
  
  y = ya[ns--];
  for (m=1;m<n;m++){ 
    for (i=0;i<n-m;i++){ 
      ho = xa[i]   - x;
      hp = xa[i+m] - x;
      w  = c[i+1]  - d[i];
      ASSERT( (den = ho - hp) != 0.0 );
      den = w / den;
      d[i] = hp * den;
      c[i] = ho * den;
    }
    y += (*dy=(2*ns+2 < (n-m) ? c[ns+1] : d[ns--]));
  }
  delete [] c;
  delete [] d;
  return y;
}

} // namespace aims

#endif
