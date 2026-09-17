


#ifndef AIMS_IMAGECOUPLEDSMOOTHER_H
#define AIMS_IMAGECOUPLEDSMOOTHER_H

#include <aims/primalsketch/coupledSmoother.h>
#include <cartodata/volume/volume.h>

namespace aims
{

  // REM: surface smoothing is geometry dependent, therefore 
  // smoother contains the geometry it is specialised for
  // It can then smooth any texture based on that geometry (mesh)

  template<typename T>
  class CoupledSmoother<carto::VolumeRef<T>, carto::VolumeRef<T> >
  {

  protected:

  public:
    
    virtual std::pair<carto::VolumeRef<T>, carto::VolumeRef<T> >
    doSmoothing( const std::pair<carto::VolumeRef<T>, carto::VolumeRef<T> >
                    & ima,
                 const std::pair<carto::VolumeRef<T>, carto::VolumeRef<T> >
                    & constraint,
                 int maxiter, bool verbose=false )=0; // for coupled constrained diffusion

	virtual float dt() {return 0.0;}      
    virtual bool optimal() {return false;}   // optimal smoothing method:
    //     false -> from original image (ex: convolution)
    //     true -> from previous scale (ex: diffusion)
  };
}

#endif
