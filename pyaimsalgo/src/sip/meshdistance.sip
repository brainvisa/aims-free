
%ModuleHeaderCode
#include <aims/distancemap/meshdistance.h>
#include <aims/distancemap/meshvoronoi.h>
#include <aims/distancemap/meshmorphomat.h>
%End

namespace aims
{

  namespace meshdistance
  {

    TimeTexture_FLOAT
    MeshDistance( const AimsSurfaceTriangle & mesh,
                  const TimeTexture_S16 & inittex,
                  bool allowUnreached ) /ReleaseGIL/;
%Docstring
    dist_tex = MeshDistance(mesh, inittex, allowUnreached)

    inittex values have the following meaning:

    0: propagation domain
    n>0: seed (distance 0)
    -1: forbidden
%End
%MethodCode
  sipRes = new TimeTexture_FLOAT;
  (*sipRes)[0] = aims::meshdistance::MeshDistance( a0->begin()->second,
                  a1->begin()->second, a2 );
%End

    TimeTexture_S16
    MeshVoronoiStepbyStep( const AimsSurfaceTriangle & mesh, 
                           const TimeTexture_S16 & inittex,
                           float dist, bool connexity,
                           bool object ) /ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_S16(
    aims::meshdistance::MeshVoronoiStepbyStep( a0->begin()->second,
      a1->begin()->second, a2, a3, a4 ) );
%End

    TimeTexture_S16
    MeshVoronoi( const AimsSurfaceTriangle & mesh,
                 const TimeTexture_S16 & inittex,
                 float dist, bool connexity, bool object ) /ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_S16;
  (*sipRes)[0] = aims::meshdistance::MeshVoronoi( a0->begin()->second,
                  a1->begin()->second,
                  a2, a3, a4 );
%End

    TimeTexture_S16 MeshVoronoi( const AimsSurfaceTriangle & mesh,
                                 const TimeTexture_S16 & inittex,
                                 short background, short forbidden,
                                 float dist, bool connexity, bool object)
                                /ReleaseGIL/;
%Docstring
    MeshVoronoi( mesh, inittex, background, forbidden, dist, connexity, object)
    MeshVoronoi( mesh, inittex, dist, connexity, object )

    Compute a geodesic voronoi diagram (dist = MAX_FLOAT,object=true ) of
    objects defined in inittex.
    The background has the label Back and the objects have a positive
    label.
    The distance can be euclidean geodesic (connexity=false) or just the
    connexity of the triangulation(connexity=true)
    This function can as well be used for dilation(object=true) /
    erosion(object=false) using the  parameter dist as the size of the
    structuring element.

    Parameters
    ----------
    mesh: AimsSurfaceTriangle
    inittex: TimeTexture_S16
    background: short
    forbidden: short
    dist: float
    connexity: bool
    object: bool

    Returns
    -------
    voronoi_texture: TimeTexture_S16
%End
%MethodCode
  sipRes = new TimeTexture_S16;
  std::cout << "vornoi: " << a2 << ", " << a3 << ", " << a4 << ", " << a5 << ", " << a6 << std::endl;
  (*sipRes)[0] = aims::meshdistance::MeshVoronoiT( a0->begin()->second,
                  a1->begin()->second, a2, a3, a4, a5, a6 );
%End

    TimeTexture_FLOAT
    MeshDilation(AimsSurfaceTriangle & mesh,
                 const TimeTexture_FLOAT & inittex,
                 const float & Back, const float & For,
                 const float dist, bool connexity) /ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_FLOAT;
  (*sipRes)[0] = aims::meshdistance::MeshDilation(a0->begin()->second, a1->begin()->second,
		  							a2, a3, a4, a5);
%End

    TimeTexture_S16
    MeshDilation(AimsSurfaceTriangle & mesh,
                  const TimeTexture_S16 & inittex,
                  const short & Back, const short & For,
                  const float dist, bool connexity) /ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_S16;
  (*sipRes)[0] = aims::meshdistance::MeshDilation(a0->begin()->second, a1->begin()->second,
		  							a2, a3, a4, a5);
%End

    TimeTexture_FLOAT
    MeshErosion(AimsSurfaceTriangle & mesh,
                 const TimeTexture_FLOAT & inittex,
                 const float & Back, const float & For,
                 const float dist, bool connexity) /ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_FLOAT;
  (*sipRes)[0] = aims::meshdistance::MeshErosion(a0->begin()->second, a1->begin()->second,
		  							a2, a3, a4, a5);
%End

    TimeTexture_S16
    MeshErosion(AimsSurfaceTriangle & mesh,
                  const TimeTexture_S16 & inittex,
                  const short & Back, const short & For,
                  const float dist, bool connexity) /ReleaseGIL/;
%MethodCode
  sipRes = new TimeTexture_S16;
  (*sipRes)[0] = aims::meshdistance::MeshErosion(a0->begin()->second, a1->begin()->second,
		  							a2, a3, a4, a5);
%End


  };

};

