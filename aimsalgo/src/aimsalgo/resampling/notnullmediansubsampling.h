
#ifndef AIMS_RESAMPLING_NOTNULLMEDIANSUBSAMPLING_H
#define AIMS_RESAMPLING_NOTNULLMEDIANSUBSAMPLING_H

#include <aims/resampling/subsamplingimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <cartobase/type/datatypetraits.h>

namespace aims {

  template <typename T>
  class NotNullMedianSubSampling: public SubSamplingImageAlgorithm<T>
  {
    public:
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
      typedef NotNullMedianFilterFunc<ChannelType> FilterFuncType;

      NotNullMedianSubSampling( int sx = 3, int sy = 3, int sz = 1 ):
        SubSamplingImageAlgorithm<T>(sx, sy, sz, FilterFuncType())
      {}

      ~NotNullMedianSubSampling() {}
  };

}

#endif
