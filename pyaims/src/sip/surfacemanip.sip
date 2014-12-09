namespace aims
{

class SurfaceManip
{
%Docstring
Surface Manipulation Object. Available Methods are :

* meshMerge(AimsTimeSurface_3, AimsTimeSurface_3) : concatenate two 3D
  meshes into the first. Also the += operator of meshes

* meshTransform(aims.AimsTimeSurface_3 mesh,
  aims.AffineTransformation3d trans) : apply a
  transformation *in-place* to a mesh.

* meshPlanarPolygon

This doc might not be up-to-date, check sources for more info.
%End

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
%Docstring
meshMerge(input_output_mesh, input_mesh)

Concatenates the second mesh to the first one. The first argument will be modified.

Parameters
----------
input_output_mesh: AimsTimeSurface_3
    first mesh. Will be modified to be the output.
input_mesh: AimsTimeSurface_3
    second mesh to be appended to the first one.

Returns
-------
None
%End

  static void meshTransform( AimsTimeSurface_3 &, const Motion & );
%Docstring
meshTransform(mesh, transformation)

Apply linear coordinates transformation to a mesh. The input mesh will be modified in-place.

Parameters
----------
mesh: AimsTimeSurface_3 or AimsTimeSurface_2
    mesh to be transformed. Will be modified.
transformation: AffineTransformation3d
    Linear transformation matrix

Returns
-------
None
%End

  static void meshTransform( AimsTimeSurface_2 &, const Motion & );

  static float meshArea( const AimsSurfaceTriangle & surf );
%Docstring
Surface area of a triangular mesh, in mm^2
%End

  static float meshVolume( const AimsSurfaceTriangle & surf );
%Docstring
Volume inside a triangular mesh, in mm^3.
The mesh must enclode a closed volume, with no holes, otherwise the volume processing will "leak" and will be wrong.
%End

  static AimsTimeSurface_3 *
      meshExtract( const AimsTimeSurface_3 & mesh,
                   const TimeTexture_S16 & tex, short value,
                   vector_SIZE_T ** overtex = 0 ) /Factory/;
%Docstring
meshExtract(mesh, texture, label_value, out_vertices_map)

Extracts a sub-mesh defined by a texture label value.
%End

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
    const AimsSurfaceTriangle & mesh ) /Factory/;
  static TimeTexture_FLOAT* meshDensity( const AimsSurfaceTriangle & mesh )
    /Factory/;
  static bool checkMeshIntersect( const AimsSurfaceTriangle &,
                                  const AimsSurfaceTriangle & );
};

};

