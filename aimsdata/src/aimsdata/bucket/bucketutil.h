
#ifndef AIMS_BUCKET_BUCKETUTIL_H
#define AIMS_BUCKET_BUCKETUTIL_H

#include <cartodata/volume/volume.h>
#include <aims/bucket/bucket.h>


namespace aims
{

  /** Utility class related to buckets
  */
  class BucketUtil
  {
  public:
    /** Bucket to volume conversion.
        Compared to Converter<BucketMap, carto::Volume>, this function has
        options to handle min bounding box handling, and allows to have
        borders.
    */
    template <typename T, typename U=T>
    static carto::VolumeRef<U> volumeFromBucket(
      const BucketMap<T> & bucket, int borderwidth = 0,
      bool enable_shift = true, Point3d** pos = 0 );
    /** Bucket to volume conversion.
        Compared to Converter<BucketMap, carto::Volume>, this function has
        options to handle bounding box handling, and allows to have
        borders.
    */
    template <typename T, typename U=T>
    static carto::VolumeRef<U> volumeFromBucket(
      const BucketMap<T> & bucket, const Point3d & bbmin,
      const Point3d & bbmax, int borderwidth = 0 );
  };

}

#endif

