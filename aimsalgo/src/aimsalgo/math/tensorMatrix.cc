
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/math/tensorMatrix.h>
#include <aims/math/mathelem.h>
#include <cartodata/volume/volume.h>
#include <aims/vector/vector.h>
#include <aims/math/trieder.h>

using namespace carto;

VolumeRef<float>
AimsEigenToMatrix(const Trieder& trieder,
                  float lambda1,float lambda2,float lambda3)
{
  VolumeRef<float> tensor( 3, 3, 1, 1, AllocatorContext::fast() );

  tensor(0,0) = lambda1 * square(trieder.dirX().item(0)) +
                lambda2 * square(trieder.dirY().item(0)) +
                lambda3 * square(trieder.dirZ().item(0)) ;
  tensor(1,1) = lambda1 * square(trieder.dirX().item(1)) +
                lambda2 * square(trieder.dirY().item(1)) +
                lambda3 * square(trieder.dirZ().item(1)) ;
  tensor(2,2) = lambda1 * square(trieder.dirX().item(2)) +
                lambda2 * square(trieder.dirY().item(2)) +
                lambda3 * square(trieder.dirZ().item(2)) ;
  tensor(0,1) = lambda1 * trieder.dirX().item(0) * trieder.dirX().item(1) +
                lambda2 * trieder.dirY().item(0) * trieder.dirY().item(1) +
                lambda3 * trieder.dirZ().item(0) * trieder.dirZ().item(1) ;
  tensor(0,2) = lambda1 * trieder.dirX().item(0) * trieder.dirX().item(2) +
                lambda2 * trieder.dirY().item(0) * trieder.dirY().item(2) +
                lambda3 * trieder.dirZ().item(0) * trieder.dirZ().item(2) ;
  tensor(1,2) = lambda1 * trieder.dirX().item(1) * trieder.dirX().item(2) +
                lambda2 * trieder.dirY().item(1) * trieder.dirY().item(2) +
                lambda3 * trieder.dirZ().item(1) * trieder.dirZ().item(2) ;

  tensor(1,0) = tensor(0,1);
  tensor(2,0) = tensor(0,2);
  tensor(2,1) = tensor(1,2);

  return(tensor);
}
