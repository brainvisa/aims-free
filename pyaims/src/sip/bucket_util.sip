

namespace aims
{

  class BucketUtil
  {
%Docstring
  Utility class related to buckets
%End

%TypeHeaderCode
#include <aims/bucket/bucketutil.h>
%End

  public:
    static rc_ptr_Volume_S16 volumeFromBucket(
      const BucketMap_VOID bucket, int borderwidth = 0,
      bool enable_shift = true, Point3d** pos = 0 );
%Docstring
    volume, offser = BucketUtil.volumeFromBucket(bucket, borderwidth=0,
                                                 enable_shift=True)

    Bucket to volume conversion.
    Compared to Converter_BucketMap_Volume, this function has
    options to handle min bounding box, and allows to have borders.

    If enable_shift is False, the volume will start at coordinates
    (0, 0, 0), without an offset (thus the volume may be much lager than
    needed, or may not contain the whole bucket if it has negative
    coordinates). In such case, pos is not used (thus passing 0 is OK).

%End
%MethodCode
  if( a2 )
  {
    Point3d p( 0, 0, 0 );
    sipRes = new rc_ptr_Volume_S16(
      aims::BucketUtil::volumeFromBucket<Void, int16_t>( *a0, a1, &p ) );
    a3 = new Point3d( p );
  }
  else
  {
    sipRes = new rc_ptr_Volume_S16(
      aims::BucketUtil::volumeFromBucket<Void, int16_t>( *a0, a1, 0 ) );
    a3 = 0;
  }
%End

    static rc_ptr_Volume_S16 volumeFromBucket(
      const BucketMap_VOID bucket, const Point3d & bbmin,
      const Point3d & bbmax, int borderwidth = 0 );
%Docstring
    volume = BucketUtil.volumeFromBucket(bucket, bbmin, bmax, borderwidth=0)

    Bucket to volume conversion.
    Compared to Converter_BucketMap_Volume, this function has
    options to handle bounding box, and allows to have borders.
%End
%MethodCode
  sipRes = new rc_ptr_Volume_S16(
    aims::BucketUtil::volumeFromBucket<Void, int16_t>( *a0, *a1, *a2, a3 ) );
%End

  };

};


