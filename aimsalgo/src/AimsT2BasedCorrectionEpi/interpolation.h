

#ifndef AIMS_AIMST2BASEDCORRECTIONEPI_INTERPOLATION_H
#define AIMS_AIMST2BASEDCORRECTIONEPI_INTERPOLATION_H


#include <aims/vector/vector.h>


// in interpolation.cxx

void linearInterpolation( const short* orig,
                          short* dest,
                          int size,
                          float res,
                          const AimsVector< float, 3 >& param );

void splineInterpolation( const short* orig,
                          short* dest,
                          int size,
                          float res,
                          const AimsVector< float, 3 >& param );


#endif
