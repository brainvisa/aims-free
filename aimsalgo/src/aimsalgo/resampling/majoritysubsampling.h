
#ifndef AIMS_RESAMPLING_MAJORITYSUBSAMPLING_H
#define AIMS_RESAMPLING_MAJORITYSUBSAMPLING_H

#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/resampling/subsamplingimagealgorithm.h>
#include <cartobase/type/datatypetraits.h>

namespace aims {

  template <typename T>
  class MajoritySubSampling: public SubSamplingImageAlgorithm<T>
  {
    public:
      typedef T VoxelType;
      typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
      typedef MajorityFilterFunc<ChannelType> FilterFuncType;

      MajoritySubSampling( int sx = 3, int sy = 3, int sz = 1 ):
        SubSamplingImageAlgorithm<T>(sx, sy, sz, FilterFuncType())
      {}

      ~MajoritySubSampling() {}
  };

}

#endif
