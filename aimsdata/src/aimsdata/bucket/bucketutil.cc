
#include <aims/bucket/bucketutil_d.h>

using namespace aims;
using namespace carto;

template
VolumeRef<int16_t> BucketUtil::volumeFromBucket<Void, int16_t>(
  const BucketMap<Void> & bucket, int borderwidth, bool enable_shift,
  Point3d** pos );


