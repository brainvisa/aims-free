
#ifndef AIMS_PRIMALSKETCH_DERICHESMOOTHER_D_H
#define AIMS_PRIMALSKETCH_DERICHESMOOTHER_D_H

#include <aims/primalsketch/dericheSmoother.h>
#include <iostream>

namespace aims
{

  template<class T> carto::VolumeRef<T> DericheSmoother<T>::doSmoothing(
    const carto::VolumeRef<T> & ima, int maxiter, bool /*verbose*/)
  {

    if (maxiter >= 0)
    {
      if( (ima.getVoxelSize()[0]==ima.getVoxelSize()[1])
          && (ima.getVoxelSize()[1]==ima.getVoxelSize()[2]) )
      {
        carto::VolumeRef<T> ima2=ima.copy(); // this is to avoid problem caused by
        // copy constructor that just makes a reference
        float sigma=sqrt(maxiter*ima.getVoxelSize()[0]*ima.getVoxelSize()[0]);
        std::cout << "sigma=" << sigma << std::endl;
        Gaussian3DSmoothing<T> gaussSmooth(sigma, sigma, sigma);
        return gaussSmooth.doit(ima2);
      }
      else
      {
        std::cerr << "Anisotropic images not supported (yet) for scale-space applications..." << std::endl;
        exit( EXIT_FAILURE );
      }
    }
    else
    {
      std::cerr << "DericheRecursiveConvolution Smoother: must have tIn < tOut"
        << std::endl;
      exit( EXIT_FAILURE );
    }
  }

}

#endif
