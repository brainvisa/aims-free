/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

#ifndef AIMS_SURFACE_MESHRASTERIZATION_D_H
#define AIMS_SURFACE_MESHRASTERIZATION_D_H

#include <aims/mesh/surfaceOperation.h>
#include <cartodata/volume/volume.h>

namespace aims
{

  namespace internal
  {
    static void rasterize_line(
      const Point3df & p0, const Point3df & direction, float lmax,
      carto::rc_ptr<carto::Volume<int16_t> > & volume, int value );
  }


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
        internal::rasterize_line( v1v, direction, lmax, volume, value );
      }
    }
  }

}

#endif



