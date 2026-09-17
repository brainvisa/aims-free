
#ifndef AIMS_DISTANCEMAP_FRONT_H
#define AIMS_DISTANCEMAP_FRONT_H

#include <aims/config/aimsalgo_config.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <typename T> class Volume;
  template <typename T> class VolumeRef;
}

namespace aims
{
  template <typename T> class BucketMap;
}


template <typename T> void 
AimsDistanceFrontPropagation( carto::rc_ptr<carto::Volume<T> > & vol,
                              T val_domain, T val_outside,
                              int xm, int ym, int zm, float mult_factor,
                              bool divide );

/** Chamfer front propagation distance map on a bucket
\param vol both input and output
\param val_domain value where the distance can propagate in the input bucket
\param val_outside voxels of this value are ignored, the front cannot 
  propagate on this value. In buckets it is generally unused, so provide an 
  unused value (negative for instance). Input voxels of other values are 
  considered fixed at their given distance. Use at least one voxel with a 
  given distance (0 typically) to start the propagation.
\param xm size of the kernel on x (in voxels)
\param ym size of the kernel on y (in voxels)
\param zm size of the kernel on z (in voxels)
\param mult_factor all distances are internally multiplied by this factor to 
  overcome precision issues (typically when using int values)
\param divide tells whether the multiplied distances should be re-divided to 
  millimeters values at the end of the operation
*/
template <typename T> void 
AimsDistanceFrontPropagation( aims::BucketMap<T>& vol, T val_domain, 
                              T val_outside,
                              int xm, int ym, int zm, float mult_factor,
                              bool divide );

#endif
