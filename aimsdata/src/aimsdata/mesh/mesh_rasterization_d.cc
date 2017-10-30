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

#include <aims/mesh/mesh_rasterization_d.h>

using namespace aims;
using namespace carto;
using namespace std;


namespace
{

  template <typename T>
  void setPoint( T & vol, const Point3d & pos, int value );

  template <>
  inline
  void setPoint( Volume<int16_t> & vol, const Point3d & pos, int value )
  {
    vol.at( pos ) = value;
  }

  template <>
  inline
  void setPoint( BucketMap<Void>::Bucket & vol, const Point3d & pos, int )
  {
    vol[pos] = Void();
  }

}

namespace aims
{

  template <typename T>
  void SurfaceManip::rasterizeLine(
    const Point3df & p0, const Point3df & direction, float lmax,
    T & volume, int value )
  {
    float dx, dy, dz, dx2, dy2, dz2;
    float ax, ay, az, lx, ly, lz, l;
    Point3d pint;

    dx = direction[0] == 0.F ? 0 : ( direction[0] > 0.F ? 1.F : -1.F );
    dy = direction[1] == 0.F ? 0 : ( direction[1] > 0.F ? 1.F : -1.F );
    dz = direction[2] == 0.F ? 0 : ( direction[2] > 0.F ? 1.F : -1.F );
    dx2 = dx / 2;
    dy2 = dy / 2;
    dz2 = dz / 2;

    if( dx == 0.F )
    {
      ax = 0.F;
      lx = lmax + 1.;
    }
    else
    {
      if( p0[0] == floor( p0[0] - dx2 ) + dx2 )
        ax = - ( p0[0] + dx );
      else
        ax = - ( floor( p0[0] + 0.5 ) + dx2 );
    }
    if( dy == 0.F )
    {
      ay = 0.F;
      ly = lmax + 1.;
    }
    else
    {
      if( p0[1] == floor( p0[1] - dy2 ) + dy2 )
        ay = - ( p0[1] + dy );
      else
        ay = - ( floor( p0[1] + 0.5 ) + dy2 );
    }
    if( dz == 0.F )
    {
      az = 0.F;
      lz = lmax + 1.;
    }
    else
    {
      if( p0[2] == floor( p0[2] - dz2 ) + dz2 )
        az = - ( p0[2] + dz );
      else
        az = - ( floor( p0[2] + 0.5 ) + dz2 );
    }

    l = 0.F;
    pint = Point3d( floor( p0[0] + 0.5 ), floor( p0[1] + 0.5 ),
                    floor( p0[2] + 0.5 ) );
    setPoint( volume, pint, value );

    while( l < lmax )
    {
      if( dx != 0.F )
        lx = - ( ax + p0[0] ) / direction[0];
      if( dy != 0.F )
        ly = - ( ay + p0[1] ) / direction[1];
      if( dz != 0.F )
        lz = - ( az + p0[2] ) / direction[2];
      l = ( lx < ly  && lx != l ) ? lx : ( ly != l ? ly : l );
      l = ( lz < l && lz != l ) ? lz : l;
      if( dx != 0.F && l == lx )
      {
        ax -= dx;
        pint[0] += dx;
        setPoint( volume, pint, value );
      }
      if( dy != 0.F && l == ly )
      {
        ay -= dy;
        pint[1] += dy;
        setPoint( volume, pint, value );
      }
      if( dz != 0.F && l == lz )
      {
        az -= dz;
        pint[2] += dz;
        setPoint( volume, pint, value );
      }
    }
  }


  void SurfaceManip::rasterizeMesh( const AimsTimeSurface<3, Void> & mesh,
    carto::rc_ptr<carto::Volume<int16_t> > & volume, int value )
  {
    vector<int> dims = volume->getSize();
    unsigned dim;
    int z;
    vector<float> vs = volume->getVoxelSize();
    AimsTimeSurface<2, Void> borderline;
    Point4df plane;
    float vsz;

    for( dim=0; dim<3; ++dim )
    {
      vsz = vs[dim];
      plane = Point4df( 0, 0, 0, 0 );
      plane[dim] = -1.F;
//       cout << "dim: " << dim << endl;

      for( z=0; z<dims[dim]; ++z )
      {
        plane[3] = vsz * z;
        borderline.clear();
        cutMesh( mesh, plane, borderline, 0 );
        rasterizeMeshWireframe( borderline, volume, value );
      }
    }

    // ensure each mesh vertex is actually in the rasterized volume.
    // as the mesh is sliced across volume voxels slice positions, the
    // exact position of vertices may be missed in the above algorithm
    const vector<Point3df> & vert = mesh.vertex();
    vector<Point3df>::const_iterator ip, ep = vert.end();
    for( ip=vert.begin(); ip!=ep; ++ip )
      volume->at( int( rint( (*ip)[0] / vs[0] ) ),
                  int( rint( (*ip)[1] / vs[1] ) ),
                  int( rint( (*ip)[2] / vs[2] ) ) ) = value;
  }



  template void SurfaceManip::rasterizeMeshWireframe(
    const AimsTimeSurface<2,Void> & mesh,
    carto::rc_ptr<carto::Volume<int16_t> > & volume,
    int value );
  template void SurfaceManip::rasterizeMeshWireframe(
    const AimsTimeSurface<3,Void> & mesh,
    carto::rc_ptr<carto::Volume<int16_t> > & volume,
    int value );
  template void SurfaceManip::rasterizeMeshWireframe(
    const AimsTimeSurface<4,Void> & mesh,
    carto::rc_ptr<carto::Volume<int16_t> > & volume,
    int value );

  template void SurfaceManip::rasterizeLine(
    const Point3df & p0, const Point3df & direction, float lmax,
    Volume<int16_t> & volume, int value );
  template void SurfaceManip::rasterizeLine(
    const Point3df & p0, const Point3df & direction, float lmax,
    BucketMap<Void>::Bucket & volume, int value );

}

