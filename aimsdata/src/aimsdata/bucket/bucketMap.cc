#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cartobase/object/object_d.h>
#include <aims/bucket/bucketMap.h>

namespace carto {

INSTANTIATE_GENERIC_OBJECT_TYPE( aims::BucketMap<Void> )
INSTANTIATE_GENERIC_OBJECT_TYPE( aims::BucketMap<int16_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( aims::BucketMap<uint16_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( aims::BucketMap<int32_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( aims::BucketMap<uint32_t> )
INSTANTIATE_GENERIC_OBJECT_TYPE( aims::BucketMap<float> )
INSTANTIATE_GENERIC_OBJECT_TYPE( aims::BucketMap<double> )
INSTANTIATE_GENERIC_OBJECT_TYPE( aims::BucketMap<DtiTensor> )

INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<Void> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<int16_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<uint16_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<int32_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<uint32_t> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<float> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<double> > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< aims::BucketMap<DtiTensor> > )

} // namespace carto
