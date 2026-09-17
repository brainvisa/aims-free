/*
 *  Elementary math functions.
 */
#ifndef AIMS_MATH_MATHELEM_RGB_H
#define AIMS_MATH_MATHELEM_RGB_H

#include <cstdlib>
#include <aims/math/mathelem.h>
#include <aims/rgb/rgb.h>
#include <cartobase/type/datatypeinfo.h>

namespace aims
{

  /**@name Useful math functions for RGB, RGBA types
   */
  //@{

  /// Get the absolute difference between 2 \c AimsRGB
  /// ie AimsRGB(abs(Rx - Ry),
  ///            abs(Gx - Gy),
  ///            abs(Bx - By))
  inline AimsRGB absdiff( const AimsRGB& x,
                          const AimsRGB& y ) {
    AimsRGB r;

    for ( unsigned int i = 0; i < DataTypeInfo<AimsRGB>::samples(); ++i )
      r[i] = absdiff( x[i], y[i] );

    return r;
  }

  /// Get the absolute difference between 2 \c AimsRGBA
  /// ie AimsRGBA(abs(Rx - Ry),
  ///             abs(Gx - Gy),
  ///             abs(Bx - By),
  ///             abs(Ax - Ay))
  inline AimsRGBA absdiff( const AimsRGBA& x,
                           const AimsRGBA& y ) {
    AimsRGBA r;

    for ( unsigned int i = 0; i < DataTypeInfo<AimsRGBA>::samples(); ++i )
      r[i] = absdiff( x[i], y[i] );

    return r;
  }

  //@}
}

#endif
