
#ifndef AIMS_RESAMPLING_MEANSUBSAMPLING_H
#define AIMS_RESAMPLING_MEANSUBSAMPLING_H

#include <aims/resampling/subsamplingimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <cartobase/type/datatypetraits.h>

namespace aims {

  template <typename T>
  class MeanSubSampling: public SubSamplingImageAlgorithm<T>
  {
    public:
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
      typedef MeanFilterFunc<ChannelType> FilterFuncType;

      MeanSubSampling( int sx = 3, int sy = 3, int sz = 1 ):
        SubSamplingImageAlgorithm<T>(sx, sy, sz, FilterFuncType())
      {}

      ~MeanSubSampling() {}
  };

}

#endif
