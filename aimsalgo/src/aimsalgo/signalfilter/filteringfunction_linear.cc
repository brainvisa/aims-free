#include <aims/signalfilter/filteringfunction_linear_d.h>

#define AIMS_TEMPLATE_LIN_FILTERFUNC( NAME )                                 \
  template class NAME<int8_t>;                                               \
  template class NAME<uint8_t>;                                              \
  template class NAME<int16_t>;                                              \
  template class NAME<uint16_t>;                                             \
  template class NAME<int32_t>;                                              \
  template class NAME<uint32_t>;                                             \
  template class NAME<int64_t>;                                              \
  template class NAME<uint64_t>;                                             \
  template class NAME<float>;                                                \
  template class NAME<double>

namespace aims {
  AIMS_TEMPLATE_LIN_FILTERFUNC(LinearFilteringFunction);
  AIMS_TEMPLATE_LIN_FILTERFUNC(LinearFilteringFunctionFactory);
  AIMS_TEMPLATE_LIN_FILTERFUNC(GaborFilterFunc);
}
