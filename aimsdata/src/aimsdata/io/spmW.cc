// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/spmW_d.h>

namespace aims
{

template class SpmWriter<int8_t> ;
template class SpmWriter<uint8_t> ;
template class SpmWriter<int16_t> ;
template class SpmWriter<uint16_t> ;
template class SpmWriter<int32_t> ;
template class SpmWriter<uint32_t> ;
template class SpmWriter<float> ;
template class SpmWriter<double> ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class SpmWriter<char> ;
#endif
template class SpmWriter<int64_t> ;
template class SpmWriter<uint64_t> ;

} // namespace aims
