


#ifndef AIMS_IMAGESMOOTHER_H
#define AIMS_IMAGESMOOTHER_H

#include <aims/primalsketch/smoother.h>
#include <cartodata/volume/volume.h>

namespace aims
{

  // REM: surface smoothing is geometry dependent, therefore 
  // smoother contains the geometry it is specialised for
  // It can then smooth any texture based on that geometry (mesh)

  template<typename T>
  class Smoother<carto::VolumeRef<T>, carto::VolumeRef<T> >
  {

  public:
    virtual ~Smoother() {}
    virtual carto::VolumeRef<T> doSmoothing(
      const carto::VolumeRef<T> & ima, int maxiter, bool verbose=false )=0;

    virtual float dt() {return 0.0;}      //smooooth operatooor...
                                                           // virtuel pur
                                                           // Virtuel pur
    virtual bool optimal() {return false;}   // optimal smoothing method:
    //     false -> from original image (ex: convolution)
    //     true -> from previous scale (ex: diffusion)
  };
}

#endif
