namespace aims
{

class SurfaceManip
{
%Docstring
Surface Manipulation Object. All mehtods are static in this class, it is just a means of grouping functions.
%End

%TypeHeaderCode
#include <pyaims/vector/stdvector.h>
#include <aims/mesh/surfacemanip.h>
#ifndef PYAIMS_AIMSTIMESURFACE_2_VOID_DEFINED
#define PYAIMS_AIMSTIMESURFACE_2_VOID_DEFINED
typedef AimsTimeSurface<2,Void> AimsTimeSurface_2_VOID;
#endif
#ifndef PYAIMS_TIMETEXTURE_S16_DEFINED
#define PYAIMS_TIMETEXTURE_S16_DEFINED
typedef TimeTexture<int16_t> TimeTexture_S16;
#endif
%End

public:
  static AimsSurfaceTriangle* meshPlanarPolygon(
    const Point4df &, const AimsTimeSurface_2_VOID & ) /Factory/;
%Docstring
meshPlanarPolygon(plane, polygon)

Tesselate a planar polygon to fill it by a triangular mesh

Parameters
----------
plane: Point4df
    plane equation
polygon: AimsTimeSurface_2_VOID
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

  static void invertSurfacePolygons( AimsTimeSurface_3_VOID & surface );
%Docstring
invertSurfacePolygons(mesh)

Invert polygons order (flips 2 vertex indices in each triangle) to flip its interior / exterior notions (used by OpenGL rendering).
The input mesh is modified in-place.
%End

  static void meshMerge( AimsTimeSurface_2_VOID &,
                         const AimsTimeSurface_2_VOID & );
  static void meshMerge( AimsTimeSurface_3_VOID &,
                         const AimsTimeSurface_3_VOID & );
  static void meshMerge( AimsTimeSurface_4_VOID &,
                         const AimsTimeSurface_4_VOID & );
%Docstring
meshMerge(input_output_mesh, input_mesh)

Concatenates the second mesh to the first one. The first argument will be modified.

Parameters
----------
input_output_mesh: AimsTimeSurface_3_VOID
    first mesh. Will be modified to be the output.
input_mesh: AimsTimeSurface_3_VOID
    second mesh to be appended to the first one.

Returns
-------
None
%End

  static void meshTransform( AimsTimeSurface_3_VOID &, const Motion & );
%Docstring
meshTransform(mesh, transformation)

Apply linear coordinates transformation to a mesh. The input mesh will be modified in-place.

Parameters
----------
mesh: AimsTimeSurface_3_VOID or AimsTimeSurface_2_VOID
    mesh to be transformed. Will be modified.
transformation: AffineTransformation3d
    Linear transformation matrix

Returns
-------
None
%End

  static void meshTransform( AimsTimeSurface_2_VOID &, const Motion & );

  static float meshArea( const AimsSurfaceTriangle & surf );
%Docstring
Surface area of a triangular mesh, in mm^2
%End

  static std::map<int, float> meshArea( const AimsSurfaceTriangle & surf,
                                        const TimeTexture_S16 & tex );
%Docstring
Surface areas of regions of a triangular mesh, in mm^2. Regions are given as a texture.
%End

  static float meshVolume( const AimsSurfaceTriangle & surf );
%Docstring
meshVolume(mesh)

Volume inside a triangular mesh, in mm^3.
The mesh must enclode a closed volume, with no holes, otherwise the volume processing will "leak" and will be wrong.
%End

  static AimsTimeSurface_3_VOID *
      meshExtract( const AimsTimeSurface_3_VOID & mesh,
                   const TimeTexture_S16 & tex, short value,
                   vector_SIZE_T ** overtex = 0 ) /Factory/;
%Docstring
meshExtract(mesh, texture, label_value)

Extracts a sub-mesh defined by a texture label value.

Returns
-------
sub_mesh: AimsTimeSurface_3_VOID
    extracted sub-mesh
vertices: vector_ULONG
    indices of extracted vertices
%End

  static AimsTimeSurface_2_VOID *
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
boundary: AimsTimeSurface_2_VOID
    output segments mesh (filar mesh) for the boundary
%End

  static AimsTimeSurface_2_VOID *
    meshTextureBoundary( const AimsSurfaceTriangle & mesh,
      const TimeTexture_S32 & tex, int region ) /Factory/;
  static AimsTimeSurface_2_VOID *
    meshTextureBoundary( const AimsSurfaceTriangle & mesh,
      const TimeTexture_FLOAT & tex, float region ) /Factory/;

  static void cutMesh( const AimsSurfaceTriangle & insurf,
                       const Point4df & plane, AimsSurfaceTriangle & cut,
                       AimsTimeSurface_2_VOID & borderline );
%Docstring
cutMesh(in_mesh, plane, cut_mesh, border_line)

Cut a mesh by a plane and that's all.

Parameters
----------
in_mesh: AimsSurfaceTriangle
    Triangular mesh to be cut.
plane: Point4df
    cut plane equation (4 coefs)
cut_mesh: AimsSurfaceTriangle
    output cut mesh (the part satisfying the plane equation). Polygons crossing the plane will be cut into smaller ones.
border_line: AimsTimeSurface_2_VOID
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
    const AimsSurfaceTriangle & mesh,
    const vector_U32 & selectedPolygons = vector_U32() ) /Factory, ReleaseGIL/;
%Docstring
refineMeshTri4(mesh, selected_polygons=None)

Refine a mesh by subdivising every triangle into 4 smaller ones.

Parameters
----------
mesh: AimsSurfaceTriangle
    mesh to be refined
selected_polygons: vector_U32
    optional list of polygons indices to be selectively refined. Others will not be changed, unless they are at the border of a refined one, in which case they have to be split in some way.

Returns
-------
refined_mesh: AimsSurfaceTriangle
    refined mesh
%End

  static TimeTexture_FLOAT* meshEdgeLengthRatioTexture(
    const AimsSurfaceTriangle & nummesh,
    const AimsSurfaceTriangle & denommesh ) /Factory, ReleaseGIL/;
%Docstring
meshEdgeLengthRatioTexture(numerator_mesh, denominator_mesh)

Calculate an edge length ratio in edges of two meshes with the same topology. The max length ratios of edges is kept for each vertex.

Parameters
----------
numerator_mesh: AimsSurfaceTriangle
denominator_mesh: AimsSurfaceTriangle

Returns
-------
ratio_texture: TimeTexture_FLOAT
%End

  static TimeTexture_FLOAT* meshDensity( const AimsSurfaceTriangle & mesh,
    bool asDistance = false ) /Factory/;
%Docstring
meshDensity(mesh, as_distance=False)

Calculate a mesh density: inverse of the average edges distance.

Parameters
----------
mesh: AimsSurfaceTriangle
as_distance: bool (optional)
    if True, the average distance is not inverted, thus the output is an average distance map.

Returns
-------
density_texture: TimeTexture_FLOAT
%End

  static bool checkMeshIntersect( const AimsSurfaceTriangle &,
                                  const AimsSurfaceTriangle & );

  static vector_POINT3DF * lineDirections(
    const AimsTimeSurface_2_VOID & ) /Factory/;
%Docstring
lineDirections(segments_mesh)

calculate directions of a line mesh, for each vertex

Parameters
----------
segments_mesh: AimsTimeSurface_2_VOID

Returns
-------
directions: vector_POINT3DF
%End

  static void sortPolygonsAlongDirection(
    AimsTimeSurface_2_VOID & mesh, int timestep, const Point3df & direction );
%Docstring
sortPolygonsAlongDirection(mesh, timestep, direction)

Sort polygons along a given direction.
Polygons centers will be used for sorting. Only one timestep is performed (to be fast).

Parameters
----------
mesh: AimsTimeSurface_*
    The mesh will be modified.
timestep: int
direction: Point3df
%End


  static void sortPolygonsAlongDirection(
    AimsTimeSurface_3_VOID & mesh, int timestep, const Point3df & direction );
%Docstring
sortPolygonsAlongDirection(mesh, timestep, direction)

Sort polygons along a given direction.
Polygons centers will be used for sorting. Only one timestep is performed (to be fast).

Parameters
----------
mesh: AimstimeSurface_*
    The mesh will be modified.
timestep: int
direction: Point3df
%End

  static void sortPolygonsAlongDirection(
    AimsTimeSurface_4_VOID & mesh, int timestep, const Point3df & direction );
%Docstring
sortPolygonsAlongDirection(mesh, timestep, direction)

Sort polygons along a given direction.
Polygons centers will be used for sorting. Only one timestep is performed (to be fast).

Parameters
----------
mesh: AimstimeSurface_*
    The mesh will be modified.
timestep: int
direction: Point3df
%End


  static void rasterizeMeshWireframe( const AimsTimeSurface_2_VOID & mesh,
                                      rc_ptr_Volume_S16 & volume,
                                      int value = 1 ) /ReleaseGIL/;

%Docstring
rasterizeMeshWireframe(mesh, volume, value=1)

Rasterize polygons edges into a volume.

Parameters
----------
mesh: AimsTimeSurface_*
volume: Volume_S16
    Volume to write mesh imprint to
value: int
    label value used to write the mesh imprint in the volume
%End

  static void rasterizeMeshWireframe( const AimsTimeSurface_3_VOID & mesh,
                                      rc_ptr_Volume_S16 & volume,
                                      int value = 1 ) /ReleaseGIL/;

  static void rasterizeMeshWireframe( const AimsTimeSurface_4_VOID & mesh,
                                      rc_ptr_Volume_S16 & volume,
                                      int value = 1 ) /ReleaseGIL/;

  static void rasterizeMesh( const AimsTimeSurface_3_VOID & mesh,
                             rc_ptr_Volume_S16 & volume, int value = 2 )
                             /ReleaseGIL/;
%Docstring
rasterizeMesh(mesh, volume, value=2)

Rasterize polygons into a volume.

Parameters
----------
mesh: AimsTimeSurface_*
volume: Volume_S16
    Volume to write mesh imprint to
value: int
    label value used to write the mesh imprint in the volume
%End

};

};

