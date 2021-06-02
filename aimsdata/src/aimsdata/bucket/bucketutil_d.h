#ifndef AIMS_BUCKET_BUCKETUTIL_D_H
#define AIMS_BUCKET_BUCKETUTIL_D_H

#include <aims/bucket/bucketutil.h>
#include <aims/utility/boundingbox.h>
#include <aims/utility/converter_bucket.h>

namespace aims
{

  template <typename T, typename U>
  carto::VolumeRef<U>
  BucketUtil::volumeFromBucket(
    const BucketMap<T> & bucket, int borderwidth, Point3d* pos )
  {
    Point3d bmin( 0, 0, 0 );

    // bounding box of all buckets
    BoundingBox	bb;
    bb.add( bucket );
    Point3df dmax = bb.maximum();
    float vx = bucket.sizeX();
    float vy = bucket.sizeY();
    float vz = bucket.sizeZ();
    Point3d bmax = Point3d( int16_t( round( dmax[0] / vx ) ),
                            int16_t( round( dmax[1] / vy ) ),
                            int16_t( round( dmax[2] / vz ) ) );
    if( pos )
    {
      Point3df dmin = bb.minimum();
      bmin = Point3d( int16_t( round( dmin[0] / vx ) ),
                      int16_t( round( dmin[1] / vy ) ),
                      int16_t( round( dmin[2] / vz ) ) );
      *pos = bmin;
    }

    carto::VolumeRef<U> res = volumeFromBucket<T, U>( bucket, bmin, bmax,
                                                      borderwidth );

    return res;
  }


  template <typename T, typename U>
  carto::VolumeRef<U>
  BucketUtil::volumeFromBucket(
    const BucketMap<T> & bucket, const Point3d & bbmin, const Point3d & bbmax,
    int borderwidth )
  {
    Point3d size = bbmax - bbmin + Point3d( 1, 1, 1 );

    // fill in volume with all buckets
    carto::VolumeRef<U>	vol( size[0], size[1], size[2], 1, 1 );
    std::vector<float> vs;
    vs.push_back( bucket.sizeX() );
    vs.push_back( bucket.sizeY() );
    vs.push_back( bucket.sizeZ() );
    vs.push_back( bucket.sizeT() );
    vol->header().setProperty( "voxel_size", vs );
    vol->fill( 0 );
    vol->fillBorder( -1 );
    carto::RawConverter<BucketMap<T>, carto::VolumeRef<U> >	conv;
    conv.printToVolume( bucket, vol, -bbmin );

    return vol;
  }

}

#endif

