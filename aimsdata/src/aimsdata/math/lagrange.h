/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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
