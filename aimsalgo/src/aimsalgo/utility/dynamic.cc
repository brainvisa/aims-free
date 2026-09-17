#include <aims/utility/dynamic_d.h>

namespace aims {

#define instantiate_method(T) \
  template carto::VolumeRef<T> clipDynamic( const carto::VolumeRef<T> &, bool, float ); \
  template carto::VolumeRef<T> clipDynamic( carto::VolumeRef<T> &, bool, float, bool ); \
  template carto::VolumeRef<T> clip( const carto::VolumeRef<T> &, const T&, const T& ); \
  template carto::VolumeRef<T> clip( carto::VolumeRef<T> &, const T&, const T& , bool );

  instantiate_method(int8_t)
  instantiate_method(uint8_t)
  instantiate_method(int16_t)
  instantiate_method(uint16_t)
  instantiate_method(int32_t)
  instantiate_method(uint32_t)
  instantiate_method(float)
  instantiate_method(double)

};
