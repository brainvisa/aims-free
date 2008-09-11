
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

