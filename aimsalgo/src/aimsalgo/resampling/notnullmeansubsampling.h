
#ifndef AIMS_RESAMPLING_NOTNULLMEANSUBSAMPLING_H
#define AIMS_RESAMPLING_NOTNULLMEANSUBSAMPLING_H

#include <aims/resampling/subsamplingimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <cartobase/type/datatypetraits.h>

namespace aims {

  template <typename T>
  class NotNullMeanSubSampling: public SubSamplingImageAlgorithm<T>
  {
    public:
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
      typedef NotNullMeanFilterFunc<ChannelType> FilterFuncType;

      NotNullMeanSubSampling( int sx = 3, int sy = 3, int sz = 1 ):
        SubSamplingImageAlgorithm<T>(sx, sy, sz, FilterFuncType())
      {}

      ~NotNullMeanSubSampling() {}
  };

}

#endif
