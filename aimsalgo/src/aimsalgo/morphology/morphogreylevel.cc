// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif
#include <aims/morphology/morphogreylevel_d.h>

namespace aims {

template class MorphoGreyLevel<int8_t>;
template class MorphoGreyLevel<uint8_t>;
template class MorphoGreyLevel<int16_t>;
template class MorphoGreyLevel<uint16_t>;
template class MorphoGreyLevel<int32_t>;
template class MorphoGreyLevel<uint32_t>;
template class MorphoGreyLevel<float>;
template class MorphoGreyLevel<double>;

} // namespace aims
