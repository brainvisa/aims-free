#include <aims/signalfilter/elementfilterfactory_d.h>
#include <aims/rgb/rgb.h>

#define AIMS_INSTANTIATE_FILTERS( NAME )                                     \
  template class NAME<int8_t>;                                               \
  template class NAME<uint8_t>;                                              \
  template class NAME<int16_t>;                                              \
  template class NAME<uint16_t>;                                             \
  template class NAME<int32_t>;                                              \
  template class NAME<uint32_t>;                                             \
  template class NAME<int64_t>;                                              \
  template class NAME<uint64_t>;                                             \
  template class NAME<float>;                                                \
  template class NAME<double>;                                               \
  template class NAME<AimsRGB>;                                              \
  template class NAME<AimsRGBA>

namespace aims{
  AIMS_INSTANTIATE_FILTERS( ElementFilterFactory );
  AIMS_INSTANTIATE_FILTERS( MinFilter );
  AIMS_INSTANTIATE_FILTERS( MaxFilter );
  AIMS_INSTANTIATE_FILTERS( MedianFilter );
  AIMS_INSTANTIATE_FILTERS( NotNullMedianFilter );
  AIMS_INSTANTIATE_FILTERS( MeanFilter );
  AIMS_INSTANTIATE_FILTERS( NotNullMeanFilter );
  AIMS_INSTANTIATE_FILTERS( MajorityFilter );
  AIMS_INSTANTIATE_FILTERS( ExtremaDifferenceFilter );
  AIMS_INSTANTIATE_FILTERS( SumFilter );
  AIMS_INSTANTIATE_FILTERS( VarFilter );
  AIMS_INSTANTIATE_FILTERS( StDevFilter );
}
