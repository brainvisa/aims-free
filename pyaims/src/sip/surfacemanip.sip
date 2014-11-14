namespace aims
{

class SurfaceManip
{
%TypeHeaderCode
#include <pyaims/vector/stdvector.h>
#include <aims/mesh/surfaceOperation.h>
#ifndef PYAIMS_AIMSTIMESURFACE_2_VOID_DEFINED
#define PYAIMS_AIMSTIMESURFACE_2_VOID_DEFINED
typedef AimsTimeSurface<2,Void> AimsTimeSurface_2;
#endif
#ifndef PYAIMS_TIMETEXTURE_S16_DEFINED
#define PYAIMS_TIMETEXTURE_S16_DEFINED
typedef TimeTexture<int16_t> TimeTexture_S16;
#endif
%End

public:
  static AimsSurfaceTriangle* meshPlanarPolygon( const Point4df &,
                                                 const AimsTimeSurface_2 & )
                                                /Factory/;
%MethodCode
  AimsSurfaceTriangle *pm = new AimsSurfaceTriangle;
  aims::SurfaceManip::meshPlanarPolygon( *a0, *a1, *pm );
  sipRes = pm;
%End

  static void invertSurfacePolygons( AimsTimeSurface_3 & surface );

  static void meshMerge( AimsTimeSurface_3 &, const AimsTimeSurface_3 & );
  static void meshTransform( AimsTimeSurface_3 &, const Motion & );
  static void meshTransform( AimsTimeSurface_2 &, const Motion & );

  static float meshArea( const AimsSurfaceTriangle & surf );
  static float meshVolume( const AimsSurfaceTriangle & surf );

  static AimsTimeSurface_3 *
      meshExtract( const AimsTimeSurface_3 & mesh,
                   const TimeTexture_S16 & tex, short value,
                   vector_SIZE_T ** overtex = 0 ) /Factory/;

  static AimsTimeSurface_2 *
      meshTextureBoundary( const AimsSurfaceTriangle & mesh,
        const TimeTexture_S16 & tex, short region ) /Factory/;
  static AimsTimeSurface_2 *
    meshTextureBoundary( const AimsSurfaceTriangle & mesh,
      const TimeTexture_S32 & tex, int region ) /Factory/;
  static AimsTimeSurface_2 *
    meshTextureBoundary( const AimsSurfaceTriangle & mesh,
      const TimeTexture_FLOAT & tex, float region ) /Factory/;

  static void cutMesh( const AimsSurfaceTriangle & insurf,
                       const Point4df & plane, AimsSurfaceTriangle & cut,
                       AimsTimeSurface_2 & borderline );

  static Point3df nearestPointToMesh( const Point3df & pos, 
                                      const AimsSurfaceTriangle & mesh, 
                                      unsigned nneighbours = 1, 
                                      unsigned* index = 0 );
  static vector_set_U32
    surfaceNeighbours( const AimsSurfaceTriangle & surf );
  static AimsSurfaceTriangle* refineMeshTri4(
    const AimsSurfaceTriangle & mesh,
    const vector_U32 & selectedPolygons = vector_U32() ) /Factory/;
  static TimeTexture_FLOAT* meshDensity( const AimsSurfaceTriangle & mesh,
    bool asDistance = false ) /Factory/;
  static TimeTexture_FLOAT* meshEdgeLengthRatioTexture(
    const AimsSurfaceTriangle & nummesh,
    const AimsSurfaceTriangle & denommesh ) /Factory/;

  static bool checkMeshIntersect( const AimsSurfaceTriangle &,
                                  const AimsSurfaceTriangle & );
  static vector_POINT3DF * lineDirections(
    const AimsTimeSurface_2 & ) /Factory/;
};

};

