// we don't want to issue a warning while compiling the obsolete classes
// themselves...
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/data/fastAllocationData.h>

namespace aims {

  template class AimsFastAllocationData<uint8_t>;
  template class AimsFastAllocationData<int8_t>;
  template class AimsFastAllocationData<uint16_t>;
  template class AimsFastAllocationData<int16_t>;
  template class AimsFastAllocationData<uint32_t>;
  template class AimsFastAllocationData<int32_t>;
  template class AimsFastAllocationData<float>;
  template class AimsFastAllocationData<double>;
  
} // namespace carto
