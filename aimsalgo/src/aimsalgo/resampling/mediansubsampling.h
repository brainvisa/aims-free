
#ifndef AIMS_RESAMPLING_MEDIANSUBSAMPLING_H
#define AIMS_RESAMPLING_MEDIANSUBSAMPLING_H

#include <aims/resampling/subsamplingimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <cartobase/type/datatypetraits.h>

namespace aims {

  template <typename T>
  class MedianSubSampling: public SubSamplingImageAlgorithm<T>
  {
    public:
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
      typedef MedianFilterFunc<ChannelType> FilterFuncType;

      MedianSubSampling( int sx = 3, int sy = 3, int sz = 1 ):
        SubSamplingImageAlgorithm<T>(sx, sy, sz, FilterFuncType())
      {}

      ~MedianSubSampling() {}
  };

}

#endif
