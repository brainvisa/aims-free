#include <aims/signalfilter/linearfilterfactory_d.h>
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


namespace aims {
  AIMS_INSTANTIATE_FILTERS( LinearFilterFactory );
  AIMS_INSTANTIATE_FILTERS( GaborFilter );
}
