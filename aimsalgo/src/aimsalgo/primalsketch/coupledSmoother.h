
// REM about scale (t) and variance (s):
// When using a Gaussian filtering we  use the following convention: 
//       t=s^2.
// It corresponds to the heat equation: dI/dt=0.5*Laplacian(I)

#ifndef AIMS_PRIMALSKETCH_COUPLEDSMOOTHER_H
#define AIMS_PRIMALSKETCH_COUPLEDSMOOTHER_H

#include <aims/config/aimsalgo_config.h>

namespace aims {

  template<typename Geom, typename Text> class CoupledSmoother
  {

  protected:

	public:

	CoupledSmoother() {}
    
    virtual std::pair<Text, Text> doSmoothing(const std::pair<Text, Text > & ima, 
                                              const std::pair<Text, Text > & constraint,
                                              int maxiter, bool verbose=false) = 0;
    
	virtual float dt() {return 0.0;}                                                                 //smooooth operatooor...                                                        // virtuel pur
    virtual bool optimal() {return false;}   // optimal smoothing method:
    //     0 -> from original image (ex: convolution)
    //     1 -> from previous scale (ex: diffusion)
  };

}
#endif
