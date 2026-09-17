
#ifndef AIMS_PRIMALSKETCH_DERICHESMOOTHER_H
#define AIMS_PRIMALSKETCH_DERICHESMOOTHER_H

#include <aims/signalfilter/g3dsmooth.h>
#include <aims/primalsketch/imageSmoother.h>


namespace aims
{

  template<class T>
  class DericheSmoother
    : public Smoother<carto::VolumeRef<T>, carto::VolumeRef<T> >
  {

  public:

    carto::VolumeRef<T> doSmoothing( const carto::VolumeRef<T> & ima,
                                     int maxiter,
                                     bool verbose=false);

    float dt() {return 0.0;}
    bool optimal() {return false;}

  };

}

#endif
