namespace aims
{

class SurfaceManip
{
%Docstring
Surface Manipulation Object. All mehtods are static in this class, it is just a means of grouping functions.

Methods
-------
* meshMerge
    concatenate two 3D meshes into the first. Also the += operator of meshes
* meshTransform
    apply a transformation *in-place* to a mesh.
* cutMesh
* meshPlanarPolygon
* meshArea
* meshVolume
* meshExtract
    Extract a sub-mesh
* meshTextureBoundary
* invertSurfacePolygons
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
%Docstring
meshPlanarPolygon(plane, polygon)

Tesselate a planar polygon to fill it by a triangular mesh

Parameters
----------
plane: Point4df
    plane equation
polygon: AimsTimeSurface_2
    planar polygon to tesselate. Typically, the output of cutMesh()

Returns
-------
planar_mesh: AimsSurfaceTriangle
    the mesh filling the polygon
%End

%MethodCode
  AimsSurfaceTriangle *pm = new AimsSurfaceTriangle;
  aims::SurfaceManip::meshPlanarPolygon( *a0, *a1, *pm );
  sipRes = pm;
%End

  static void invertSurfacePolygons( AimsTimeSurface_3 & surface );
%Docstring
invertSurfacePolygons(mesh)

Invert polygons order (flips 2 vertex indices in each triangle) to flip its interior / exterior notions (used by OpenGL rendering).
The input mesh is modified in-place.
%End

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
meshVolume(mesh)

Volume inside a triangular mesh, in mm^3.
The mesh must enclode a closed volume, with no holes, otherwise the volume processing will "leak" and will be wrong.
%End

  static AimsTimeSurface_3 *
      meshExtract( const AimsTimeSurface_3 & mesh,
                   const TimeTexture_S16 & tex, short value,
                   vector_SIZE_T ** overtex = 0 ) /Factory/;
%Docstring
meshExtract(mesh, texture, label_value)

Extracts a sub-mesh defined by a texture label value.

Returns
-------
sub_mesh: AimsTimeSurface_3
    extracted sub-mesh
vertices: vector_ULONG
    indices of extracted vertices
%End

  static AimsTimeSurface_2 *
      meshTextureBoundary( const AimsSurfaceTriangle & mesh,
        const TimeTexture_S16 & tex, short region ) /Factory/;
%Docstring
meshTextureBoundary(mesh, label_texture, region_value)

Extracts the boundary of region of value <region_value> of the input texture, on the mesh.

If region_value is negative, take boundaries of all regions. The input texture is a label texture.

Parameters
----------
mesh: AimsSurfaceTriangle
    input mesh to extract boundary on
label_texture: TimeTexture_S16, TimeTexture_S32, or TimeTexture_FLOAT
    label texture defining regions on the mesh
region_value: int16, int32 or float
    label of the desired region

Returns
-------
boundary: AimsTimeSurface_2
    output segments mesh (filar mesh) for the boundary
%End

  static AimsTimeSurface_2 *
    meshTextureBoundary( const AimsSurfaceTriangle & mesh,
      const TimeTexture_S32 & tex, int region ) /Factory/;
  static AimsTimeSurface_2 *
    meshTextureBoundary( const AimsSurfaceTriangle & mesh,
      const TimeTexture_FLOAT & tex, float region ) /Factory/;

  static void cutMesh( const AimsSurfaceTriangle & insurf,
                       const Point4df & plane, AimsSurfaceTriangle & cut,
                       AimsTimeSurface_2 & borderline );
%Docstring
cutMesh(in_mesh, plane, cut_mesh, border_line)

Cut a mesh by a plane

Parameters
----------
in_mesh: AimsSurfaceTriangle
    Triangular mesh to be cut.
plane: Point4df
    cut plane equation (4 coefs)
cut_mesh: AimsSurfaceTriangle
    output cut mesh (the part satisfying the plane equation). Polygons crossing the plane will be cut into smaller ones.
border_line: AimsTimeSurface_2
    output cut section polygon

Returns
-------
None
%End

  static Point3df nearestPointToMesh( const Point3df & pos, 
                                      const AimsSurfaceTriangle & mesh, 
                                      unsigned nneighbours = 1, 
                                      unsigned* index = 0 );
  static vector_set_U32
    surfaceNeighbours( const AimsSurfaceTriangle & surf );

  static AimsSurfaceTriangle* refineMeshTri4(
    const AimsSurfaceTriangle & mesh ) /Factory/;
%Docstring
refineMeshTri4(mesh)

Refine a mesh by subdivising every triangle into 4 smaller ones.
%End

  static TimeTexture_FLOAT* meshDensity( const AimsSurfaceTriangle & mesh )
    /Factory/;
%Docstring
meshDensity(mesh)

Calculate a mesh density: inverse of the average edges distance.

Returns
-------
density_texture: TimeTexture_FLOAT
%End

  static bool checkMeshIntersect( const AimsSurfaceTriangle &,
                                  const AimsSurfaceTriangle & );
};

};

