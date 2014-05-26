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
#ifndef AIMS_MATH_MATHELEM_RGB_H
#define AIMS_MATH_MATHELEM_RGB_H

#include <cstdlib>
#include <math.h>
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
