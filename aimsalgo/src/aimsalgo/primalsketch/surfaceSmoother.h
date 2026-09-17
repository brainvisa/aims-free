
#ifndef AIMS_PRIMALSKETCH_IMAGESMOOTHER_H
#define AIMS_PRIMALSKETCH_IMAGESMOOTHER_H

#include <aims/primalsketch/smoother.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>

namespace aims
{

  // REM: surface smoothing is geometry dependent, therefore 
  // smoother contains the geometry it is specialised for
  // It can then smooth any texture based on that geometry (mesh)

  template<int D, typename T> class Smoother<AimsSurface<D, Void>, Texture<T> >
  {
  protected:

    AimsSurface<D, Void> *_mesh;
    
  public:
    Smoother() {}
    Smoother(AimsSurface<D> *mesh) {_mesh=mesh;}
    virtual ~Smoother() {}
		virtual float dt() {return 0.0;}                                                                 //smooooth operatooor...                                                        // virtuel pur

    virtual Texture<T> doSmoothing(const Texture<T> & ima, int maxiter, bool verbose=false)=0;
    // Virtuel pur

    virtual bool optimal() {return false;}   // optimal smoothing method:
    //     false -> from original image (ex: convolution)
    //     true -> from previous scale (ex: diffusion)
  };

}

#endif
