#include <aims/signalfilter/filteringfunction_element_d.h>

#define AIMS_TEMPLATE_NONLIN_FILTERFUNC( NAME )                              \
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
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(ElementFilteringFunction);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(ElementFilteringFunctionFactory);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MedianFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MeanFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MinFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MaxFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(MajorityFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(ExtremaDifferenceFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(SumFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(VarFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(StDevFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(NotNullMeanFilterFunc);
  AIMS_TEMPLATE_NONLIN_FILTERFUNC(NotNullMedianFilterFunc);
}
