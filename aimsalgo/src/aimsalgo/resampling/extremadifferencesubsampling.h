
#ifndef AIMS_RESAMPLING_EXTREMADIFFERENCESUBSAMPLING_H
#define AIMS_RESAMPLING_EXTREMADIFFERENCESUBSAMPLING_H

#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/resampling/subsamplingimagealgorithm.h>
#include <cartobase/type/datatypetraits.h>

namespace aims {

  template <typename T>
  class ExtremaDifferenceSubSampling: public SubSamplingImageAlgorithm<T>
  {
    public:
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
      typedef ExtremaDifferenceFilterFunc<ChannelType> FilterFuncType;

      ExtremaDifferenceSubSampling( int sx = 3, int sy = 3, int sz = 1 ):
        SubSamplingImageAlgorithm<T>(sx, sy, sz, FilterFuncType())
      {}

      ~ExtremaDifferenceSubSampling() {}
  };

}

#endif
