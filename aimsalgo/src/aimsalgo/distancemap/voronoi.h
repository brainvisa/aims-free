
#ifndef AIMS_DISTANCEMAP_VORONOI_H
#define AIMS_DISTANCEMAP_VORONOI_H

#include <aims/config/aimsalgo_config.h>

namespace carto
{
  template <typename T> class Volume;
  template <typename T> class VolumeRef;
}

template <class T>
carto::VolumeRef<T>
AimsVoronoiFrontPropagation( carto::rc_ptr<carto::Volume<T> > & vol,
                             T val_domain, T val_outside,
                             int xm, int ym, int zm, float mult_factor );

#endif
