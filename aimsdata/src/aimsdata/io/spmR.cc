// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/spmR_d.h>

namespace aims
{

template class SpmReader<int8_t> ;
template class SpmReader<uint8_t> ;
template class SpmReader<int16_t> ;
template class SpmReader<uint16_t> ;
template class SpmReader<int32_t> ;
template class SpmReader<uint32_t> ;
template class SpmReader<float> ;
template class SpmReader<double> ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class SpmReader<char> ;
#endif
template class SpmReader<int64_t> ;
template class SpmReader<uint64_t> ;

} // namespace aims
