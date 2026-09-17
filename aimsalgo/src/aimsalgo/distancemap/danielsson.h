
#ifndef AIMS_DISTANCEMAP_DANIELSSON_H
#define AIMS_DISTANCEMAP_DANIELSSON_H

#include <aims/config/aimsalgo_config.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <typename T> class Volume;
  template <typename T> class VolumeRef;
}


enum DanielssonSide
{
  AIMS_DANIELSSON_OUTSIDE,
  AIMS_DANIELSSON_INSIDE
};

/**@name Danielsson distance maps.
    Two distance maps can be processed : the distance map of the outside
    of the object (give AIMS_DANIELSSON_OUTSIDE argument), and the distance map
    of the inside of the object (give the AIMS_DANIELSSON_INSIDE argument).
*/
//@{
/// Return float euclidean Danielsson distance map
carto::VolumeRef<float>
AimsDanielssonDistanceMap( const carto::rc_ptr<carto::Volume<int16_t> > &vol,
                           DanielssonSide side=AIMS_DANIELSSON_OUTSIDE );
/// Return float euclidean signed distance map (minus out, plus in object)
carto::VolumeRef<float>
AimsSignedDanielssonDistanceMap(
  const carto::rc_ptr<carto::Volume<int16_t> > &vol );
//@}

#endif
