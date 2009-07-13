/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

/*
 *  Lagrange's interpolation.
 */
#ifndef AIMS_MATH_LAGRANGE_H
#define AIMS_MATH_LAGRANGE_H

#include <cstdlib>
#include <aims/config/aimsdata_config.h>
#include <aims/data/data.h>
#include <math.h>

namespace aims {


/**@name Lagrange's polynomial interpolation.
   This implementation uses Neville's algorithm that is a recursive
    way.
*/
//@{
/// Returns the interpolation of a function defined at (xa,ya) points at x (dy is the error)
template <class REAL>
AIMSDATA_API REAL AimsLagrangeInterpolationOf( const AimsData<REAL> &xa,
                                               const AimsData<REAL> &ya,
                                               REAL x, REAL *dy )

//@}
{
  int  i,m,ns=0,n=std::min(xa.dimX(),ya.dimX());
  REAL den,dif,dift,ho,hp,w;
  REAL *c, *d, y;

  dif = fabs(x-xa(0));
  c = new float[n];
  d = new float[n];

  for (i=0;i<n;i++) { 
    if ((dift = fabs(x-xa(i))) < dif) { 
      ns  = i;
      dif = dift;
    }
    c[i] = ya(i);
    d[i] = ya(i);
  }
  
  y = ya(ns--);
  for (m=1;m<n;m++){ 
    for (i=0;i<n-m;i++){ 
      ho = xa(i)   - x;
      hp = xa(i+m) - x;
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

// for backward compatibility
inline  float AimsLagrangeInterpolation( const AimsData<float> &xa,
                                         const AimsData<float> &ya,
                                         float x, float *dy )
{
  return AimsLagrangeInterpolationOf<float>( xa, ya, x, dy );
}

} // namespace aims

#endif
