

#ifndef AIMS_MESH_CONNECTIVITY_MESHCC_H
#define AIMS_MESH_CONNECTIVITY_MESHCC_H

#include <aims/config/aimsalgo_config.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <map>
#include <set>

///
AIMSALGO_API

/* threshold a texture and label resultiong connected components
   modes : 0 = above threshold, 1 = below threshold
*/
template<class T>
Texture<T> AimsMeshLabelConnectedComponent( const AimsSurface<3,Void> & mesh,
                             const Texture<T> & inittex,
                             T threshold, int mode=1, int verbose=0 );
template<class T>
unsigned AimsMeshLabelNbConnectedComponent( const AimsSurface<3,Void> & mesh,
                           const Texture<T> & inittex, const T lab );
template<class T>
Texture<T> AimsMeshLabelConnectedComponent2Texture(
  const AimsSurface<3,Void> & mesh, const Texture<T> & inittex,  T threshold );
/** Split label "label" into connected components, then filter smaller ones
    out.
    Original values are left unchanged, except for filtered out regions which
    will be given the label "background" (0 by default).

    Filtering can keep the "ncomp" largest components, and suppress regions
    with fewer than "min_npts" vertices, and regions under the surface area
    "min_surf". If any of these criterions is 0, then filtering for this
    criterion doesn't happen.
*/
template<class T>
Texture<T> AimsMeshFilterConnectedComponent(
  const AimsSurface<3,Void> & mesh, const Texture<T> & inittex, T label,
  const T & background = 0, unsigned long ncomp = 1,
  unsigned long min_npts = 0, float min_surf = 0. );

/// hum, should not be there as a global, non-namespaced variable...
const int FORBIDDEN = -1;

#endif

