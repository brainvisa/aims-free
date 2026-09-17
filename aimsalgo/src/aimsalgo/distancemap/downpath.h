
#ifndef AIMS_DISTANCEMAP_DOWNPATH_H
#define AIMS_DISTANCEMAP_DOWNPATH_H

#include <aims/bucket/bucket.h>

namespace aims
{

  /* Follows the path from point pos, going down on the steepest path on an
     elevation map
  */
  template <typename T>
  BucketMap<Void>*
  downPath( const BucketMap<T> & region, const Point3d & pos );

  extern template
  BucketMap<Void>*
  downPath( const BucketMap<float> & region, const Point3d & pos );

}

#endif

