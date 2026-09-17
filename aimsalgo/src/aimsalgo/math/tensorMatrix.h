
#ifndef AIMS_MATH_TENSORMATRIX_H
#define AIMS_MATH_TENSORMATRIX_H

#include <aims/def/general.h>

namespace carto
{
  template <typename T> class VolumeRef;
}

class Trieder;


/** Reconstructing the matrix from eigen system.
*/
carto::VolumeRef<float>
AimsEigenToMatrix(const Trieder& trieder,
                  float lambda1,float lambda2,float lambda3);


#endif
