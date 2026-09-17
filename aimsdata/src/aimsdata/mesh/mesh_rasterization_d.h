#ifndef AIMS_SURFACE_MESHRASTERIZATION_D_H
#define AIMS_SURFACE_MESHRASTERIZATION_D_H

#include <aims/mesh/surfaceOperation.h>
#include <cartodata/volume/volume.h>
#include <aims/bucket/bucket.h>


namespace aims
{

  template <int D, typename T>
  void SurfaceManip::rasterizeMeshWireframe(
    const AimsTimeSurface<D,T> & mesh,
    carto::rc_ptr<carto::Volume<int16_t> > & volume,
    int value )
  {
    const std::vector<Point3df> & vert = mesh.vertex();
    const std::vector<AimsVector<uint, D> > & polygons = mesh.polygon();
    std::vector<float> vs = volume->getVoxelSize();
    typename std::vector<AimsVector<uint, D> >::const_iterator
      ip, ep = polygons.end();
    Point3df v1, v2, v1v, v2v, direction;
    float lmax;
    unsigned i;
    carto::Volume<int16_t> & vol = *volume;

    for( ip=polygons.begin(); ip!=ep; ++ip )
    {
      const AimsVector<uint, D> & poly = *ip;
      for( i=0; i<D; ++i )
      {
        v1 = vert[poly[i]];
        v2 = vert[poly[(i+1) % D]];
        v1v = Point3df( v1[0] / vs[0], v1[1] / vs[1], v1[2] / vs[2]);
        v2v = Point3df( v2[0] / vs[0], v2[1] / vs[1], v2[2] / vs[2]);
        direction = v2v - v1v;
        lmax = direction.norm();
        direction.normalize();
        rasterizeLine( v1v, direction, lmax, vol, value );
      }
    }
  }

}

#endif



