
// REM about scale (t) and variance (s):
// When using a Gaussian filtering we  use the following convention: 
//       t=s^2.
// It corresponds to the heat equation: dI/dt=0.5*Laplacian(I)

#ifndef AIMS_PRIMALSKETCH_SMOOTHER_H
#define AIMS_PRIMALSKETCH_SMOOTHER_H

#include <aims/config/aimsalgo_config.h>

namespace aims
{

  template<typename Geom, typename Text> class Smoother
  {

  protected:

	public:

	Smoother() {}
        virtual ~Smoother() {}
	virtual Text doSmoothing(const Text & ima, int maxiter, bool verbose=false)=0; // smooth operator...
	virtual float dt() {return 0.0;}                                                                 //smooooth operatooor...                                                        // virtuel pur
  virtual bool optimal() {return false;}   // optimal smoothing method:
    //     0 -> from original image (ex: convolution)
    //     1 -> from previous scale (ex: diffusion)
  };

}
#endif
