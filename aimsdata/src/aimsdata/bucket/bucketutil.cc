
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/bucket/bucketutil_d.h>

using namespace aims;
using namespace carto;

template
VolumeRef<int16_t> BucketUtil::volumeFromBucket<Void, int16_t>(
  const BucketMap<Void> & bucket, int borderwidth, Point3d* pos );

template
VolumeRef<int16_t> BucketUtil::volumeFromBucket<Void, int16_t>(
      const BucketMap<Void> & bucket, const Point3d & bbmin,
      const Point3d & bbmax, int borderwidth );

