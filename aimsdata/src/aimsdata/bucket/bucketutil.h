
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
        options to handle min bounding box, and allows to have borders.

        If pos is non null, it should point to an (allocated) Point3d where the
        bounding box min position will be copied.

        If pos is null, the volume will start at coordinates
        (0, 0, 0), without an offset (thus the volume may be much lager than
        needed, or may not contain the whole bucket if it has negative
        coordinates). In such case, pos is not used (thus passing 0 is OK).
    */
    template <typename T, typename U=T>
    static carto::VolumeRef<U> volumeFromBucket(
      const BucketMap<T> & bucket, int borderwidth = 0, Point3d* pos = 0 );
    /** Bucket to volume conversion.
        Compared to Converter<BucketMap, carto::Volume>, this function has
        options to handle bounding box, and allows to have borders.
    */
    template <typename T, typename U=T>
    static carto::VolumeRef<U> volumeFromBucket(
      const BucketMap<T> & bucket, const Point3d & bbmin,
      const Point3d & bbmax, int borderwidth = 0 );
  };

}

#endif

