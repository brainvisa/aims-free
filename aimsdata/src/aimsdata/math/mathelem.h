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
 *  Elementary math functions.
 */
#ifndef AIMS_MATH_MATHELEM_H
#define AIMS_MATH_MATHELEM_H

#include <cstdlib>
#include <aims/config/aimsdata_config.h>
#include <math.h>

namespace aims
{

  /**@name Square, cube, ...
   */
  //@{
  /// Get the square of x
  template <typename T> inline T sqr( const T & x)
  {
    return x * x;
  }

  /// Get the cube of x
  template <typename T> inline T cub( const T & x )
  {
    return x * x * x;
  }

  /// Get the sign of x
  template <typename T> inline float sgn( const T & x )
  {
    return x < 0 ? -1.0f : +1.0f;
  }
  //@}
}

/**@name Trigonometry*/
//@{
/// Get the argument of a complex re + i.im
template <class T>
double AimsArgument(T re,T im);
//@}

template <class T> inline
double AimsArgument(T re,T im)
{
  double re2=(double)re;
  double im2=(double)im;

  if ((!re2) && (!im2)) return(0);
  if (!re2)
  {
    if (im2>=0)        return M_PI/2;
    else               return -M_PI/2;
  }
  if (re2>0)           return atan(im2/re2);
  else if (im2>=0)     return atan(im2/re2)+M_PI;
  else                 return atan(im2/re2)-M_PI;
}


template <class T> inline
T square(const T& val)
{
  return val*val;
}


template <class T> inline
T cube(const T& val)
{
  return val*val*val;
}


template< class T > inline
T pythagore( const T& a, const T& b )
{
  T c, d;

  c = (T)fabs( a );
  d = (T)fabs( b );

  if ( c > d )  
    return c * (T)sqrt( (T)1 + square( d / c ) );
  else
    return ( d == (T)0 ) ? (T)0 : d * (T)sqrt( (T)1 + square( c / d ) );
}


/**@name Miscellaneous functions*/
//@{
/// Return the next power of two closest to an integer
AIMSDATA_API int AimsNextPowerOfTwo(int number);
/// Sigmoidal function (useful for neural networks)
AIMSDATA_API double AimsSigmoid(double x);
//@}

#endif
