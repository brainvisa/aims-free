%ModuleHeaderCode
#include <aims/transform/transform_objects.h>
%End

/* for an unknown reason functions defined in aimsalgo in the aims namespace
   are not seen from the python module. Classes are, so are functions defined
   the same way in aimsdata. The only workaround found is to define them
   outside of their namespace.

   Even then, the behavior is incinsistant:
   transformBucketDirect and other functions don't need to be declared in
   namespace aims, or to be redefined to use the function in the namespace
   (where they are actually).
   transformMesh and transformGraph used to work the same way (by chance?)
   and, one day, did not compile any longer unless we define a MethodCode
   for them and call the functions in the namespace aims.
*/

  void transformMesh( AimsTimeSurface_2_VOID & mesh,
                      const soma::Transformation3d & direct_transformation);
%Docstring
transformMesh(mesh, direct_transformation)

Apply a spatial transformation to a segments mesh (AimsTimeSurface).

The mesh is transformed in-place and modified.

Each vertex of the mesh is transformed according to the supplied
transformation. Normals are re-calculated from the new vertex positions.
%End

%MethodCode
  aims::transformMesh( *a0, *a1 );
%End

  void transformMesh( AimsTimeSurface_3_VOID & mesh,
                      const soma::Transformation3d & direct_transformation );
%Docstring
transformMesh(mesh, direct_transformation)

Apply a spatial transformation to a triangles mesh (AimsTimeSurface)

The mesh is transformed in-place and modified.

Each vertex of the mesh is transformed according to the supplied
transformation. Normals are re-calculated from the new vertex positions.
%End

%MethodCode
  aims::transformMesh( *a0, *a1 );
%End

  void transformMesh( AimsTimeSurface_4_VOID & mesh,
                      const soma::Transformation3d & direct_transformation );
%Docstring
transformMesh(mesh, direct_transformation)

Apply a spatial transformation to a quads mesh (AimsTimeSurface)

The mesh is transformed in-place and modified.

Each vertex of the mesh is transformed according to the supplied
transformation. Normals are re-calculated from the new vertex positions.
%End

%MethodCode
  aims::transformMesh( *a0, *a1 );
%End

  rc_ptr_BucketMap_VOID
  transformBucketDirect( const BucketMap_VOID & bck,
                         const soma::Transformation3d & direct_transformation,
                         Point3df vs = Point3df( 0., 0., 0. ) );
%Docstring
transformBucketDirect(bck, direct_transformation, vs=aims.Point3df(0., 0., 0.))

Apply a spatial transformation to a BucketMap

Each voxel of the input bucket is transformed with direct_transformation, and
the closest voxel of the output bucket is set. The voxel size of the output
bucket can optionally be specified in vs. By default, the same voxel size as
the input bucket is used.

Warning
-------

This method provides no guarantees of topology preservation; in fact it will
create holes in the resulting bucket, particularly when upsampling. When
possible, you should use resampleBucket() instead, which performs
nearest-neighbour resampling.
%End

  rc_ptr_BucketMap_VOID
  resampleBucket( const BucketMap_VOID & bck,
                  const soma::Transformation3d & direct_transformation,
                  const soma::Transformation3d & inverse_transformation,
                  Point3df vs = Point3df( 0., 0., 0. ),
                  bool also_pushforward = true);
%Docstring
resampleBucket(bck, direct_transformation, inverse_transformation,
               vs=aims.Point3df(0., 0., 0.), also_pushforward=True)

Apply a spatial transformation to a BucketMap.

The bucket is transformed by resampling, using a pull-back method in the same
way as nearest neighbour resampling of a Volume.

Pure pull-back resampling does not behave well when downsampling (it introduces
holes), so by default this function returns the union of voxels transformed
using the pushforward and pullback methods. Set also_pushforward to
false to disable this behaviour and only perform pure pullback.

The voxel size of the output bucket can optionally be specified in
vs. By default, the same voxel size as the input bucket is used.

Warning
-------

Alhtough this method is more reliable than
transformBucketDirect, it still provides no guarantees of topology
preservation.
%End

  void transformGraph( Graph & graph,
                       const soma::Transformation3d & direct_transformation,
                       const soma::Transformation3d * inverse_transformation,
                       Point3df vs = Point3df( 0., 0., 0. ) );
%Docstring
transformGraph(graph, direct_transformation, inverse_transformation,
               vs=aims.Point3df(0., 0., 0.))

Apply a spatial transformation to all objects contained in a Graph.

The graph is modified in-place.

An inverse transformation is necessary for correctly transforming Buckets (see
resampleBucket). If inverse_transformation is NULL, buckets will be transformed
with the push-forward method only (transformBucketDirect).

Warnings
--------

Volumes are not transformed, neither are graph attributes. Please run
AimsFoldArgAtt to fix the values of basic attributes, or the
CorticalFoldsGraphUpgradeFromOld BrainVisa process, which can be found under
Morphologist/Sulci/graphmanipulation, for a complete update.
%End

%MethodCode
  aims::transformGraph( *a0, *a1, a2, *a3 );
%End

namespace aims
{

  class BundleTransformer : aims::BundleListener, aims::BundleProducer
  {
%TypeHeaderCode
#include <aims/transform/transform_objects.h>
%End

%Docstring
BundleTransformer.__init__(direct_transformation)

Apply a spatial transformation to fiber bundles.

Each point along the bundles is transformed according to the supplied
transformation.

This is a BundleListener / BundleProducer stream processing class which applies
vector field deformation to bundle data. It can be typically connected to a
BundleReader and a BundleWriter.
%End

  public:
    BundleTransformer( const rc_ptr_Transformation3d & direct_transformation );
    virtual ~BundleTransformer();
  };

}; // namespace aims
