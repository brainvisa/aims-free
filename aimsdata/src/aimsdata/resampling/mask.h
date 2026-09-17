#ifndef AIMS_RESAMPLING_MASK_H
#define AIMS_RESAMPLING_MASK_H

#include <aims/bucket/bucket.h>
#include <cartodata/volume/volume.h>

namespace aims {

  /** mask src object with m.
    \param intersect if true, the result is the intersection between both
      buckets. If false, m is removed from src.
  */
  BucketMap<Void>* mask( const BucketMap<Void> & src,
                         const BucketMap<Void> & m, bool intersect = true );
  /** mask src object with m, avoiding value val in m if insideval is true,
      keeping only val otherwise
  */
  template <typename T>
  BucketMap<Void>* maskWithVolume( const BucketMap<Void> & src,
                                   const carto::rc_ptr<carto::Volume<T> >  m,
                                   const T & val = 0, bool insideval = false );

}

#endif
