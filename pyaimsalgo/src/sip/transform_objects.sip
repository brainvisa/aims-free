%ModuleHeaderCode
#include <aims/transform/transform_objects.h>
%End

/* for an unknown reason functions defined in aimsalgo in the aims namespace
   are not seen from the python module. Classes are, so are functions defined
   the same way in aimsdata. The only workaround found is to define them
   outside of their namespace
*/

  void transformMesh( AimsTimeSurface_2_VOID & mesh,
                      const aims::Transformation3d & direct_transformation);
%Docstring
Apply a spatial transformation to a mesh (AimsTimeSurface)

Each vertex of the mesh is transformed according to the supplied
transformation. Normals are re-calculated from the new vertex positions.
%End
%MethodCode
  aims::transformMesh( *a0, *a1 );
%End

  void transformMesh( AimsTimeSurface_3_VOID & mesh,
                      const aims::Transformation3d & direct_transformation );
%Docstring
Apply a spatial transformation to a mesh (AimsTimeSurface)

Each vertex of the mesh is transformed according to the supplied
transformation. Normals are re-calculated from the new vertex positions.
%End
%MethodCode
  aims::transformMesh( *a0, *a1 );
%End

  void transformMesh( AimsTimeSurface_4_VOID & mesh,
                      const aims::Transformation3d & direct_transformation );
%Docstring
Apply a spatial transformation to a mesh (AimsTimeSurface)

Each vertex of the mesh is transformed according to the supplied
transformation. Normals are re-calculated from the new vertex positions.
%End
%MethodCode
  aims::transformMesh( *a0, *a1 );
%End

  rc_ptr_BucketMap_VOID
  transformBucketDirect( const BucketMap_VOID & bck,
                         const aims::Transformation3d & direct_transformation,
                         Point3df vs = Point3df( 0., 0., 0. ) );
%Docstring
Apply a spatial transformation to a BucketMap

Each voxel of the input bucket is transformed with direct_transformation, and
the closest voxel of the output bucket is set. The voxel size of the output
bucket can optionally be specified in vs. By default, the same voxel size as
the input bucket is used.

Warning
-------

This method will not preserve the topology of the input bucket, in fact it will
create many holes for anything but a simple translation. The correct way of
resampling a bucket is to do nearest-neighbour resampling using the inverse
transformation. (TODO: implement bucket resampling)
%End
%MethodCode
  sipRes
    = new carto::rc_ptr<aims::BucketMap<Void> >( aims::transformBucketDirect(
      *a0, *a1, *a2 ) );
%End

  void transformGraph( Graph & graph,
                       const aims::Transformation3d & direct_transformation,
                       Point3df vs = Point3df( 0., 0., 0. ) );
%Docstring
Apply a spatial transformation to all objects contained in a Graph.

The graph is modified in-place.

Warnings
--------

Buckets are transformed with transformBucketDirect(), the same caveats apply.
Volumes are not transformed at the moment.

The interface of this function will change to allow specifying an inverse
transformation, so that buckets and volumes can be resampled properly.
%End
%MethodCode
  aims::transformGraph( *a0, *a1, *a2 );
%End

namespace aims
{

  class BundleTransformer : aims::BundleListener, aims::BundleProducer
  {
%TypeHeaderCode
#include <aims/transform/transform_objects.h>
%End

%Docstring
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
