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

#ifndef AIMS_REGISTRATION_TRANSFORM_OBJECTS_H
#define AIMS_REGISTRATION_TRANSFORM_OBJECTS_H

#include <cartobase/smart/rcptr.h>
#include <aims/mesh/surface.h>
#include <aims/bucket/bucketMap.h>
#include <aims/fibers/bundles.h>

class Graph;

namespace aims
{

  /** Apply a spatial transformation to a mesh (AimsTimeSurface)

      Each vertex of the mesh is transformed according to the supplied
      transformation. Normals are re-calculated from the new vertex positions.
   */
  template <int D>
  void transformMesh( AimsTimeSurface<D, Void> & mesh,
                      const soma::Transformation3d & direct_transformation );

  /** Apply a spatial transformation to a BucketMap

      Each voxel of the input bucket is transformed with
      \arg direct_transformation, and the closest voxel of the output bucket is set.
      The voxel size of the output bucket can optionally be specified in
      \arg vs. By default, the same voxel size as the input bucket is used.

      \warning{This method provides no guarantees of topology preservation; in
      fact it will create holes in the resulting bucket, particularly when
      upsampling. When possible, you should use resampleBucket() instead, which
      performs nearest-neighbour resampling.}
   */
  carto::rc_ptr<BucketMap<Void> >
  transformBucketDirect( const BucketMap<Void> & bck,
                         const soma::Transformation3d & direct_transformation,
                         Point3df vs = Point3df( 0., 0., 0. ) );

  /** Apply a spatial transformation to a BucketMap

      The bucket is transformed by resampling, using a pull-back method in the
      same way as nearest neighbour resampling of a Volume.

      Pure pull-back resampling does not behave well when downsampling (it
      introduces holes), so by default this function returns the union of
      voxels transformed using the pushforward and pullback methods. Set \arg
      also_pushforward to \c false to disable this behaviour and only perform
      pure pullback.

      The voxel size of the output bucket can optionally be specified in
      \arg vs. By default, the same voxel size as the input bucket is used.

      \warning{Alhtough this method is more reliable than
      transformBucketDirect, it still provides no guarantees of topology
      preservation.}
   */
  carto::rc_ptr<BucketMap<Void> >
  resampleBucket( const BucketMap<Void> & bck,
                  const soma::Transformation3d & direct_transformation,
                  const soma::Transformation3d & inverse_transformation,
                  Point3df vs = Point3df( 0., 0., 0. ),
                  bool also_pushforward = true);

  /** Apply a spatial transformation to all objects contained in a Graph.

      The graph is modified in-place.

      An inverse transformation is necessary for correctly transforming Buckets
      (see resampleBucket). If inverse_transformation is NULL, buckets will
      be transformed with the push-forward method only (transformBucketDirect).

      \warning{Volumes are not transformed, neither are graph attributes.
        Please run AimsFoldArgAtt to fix the values of basic attributes, or the
        CorticalFoldsGraphUpgradeFromOld BrainVisa process, which can be found
        under Morphologist/Sulci/graphmanipulation, for a complete update.}
   */
  void transformGraph( Graph & graph,
                       const soma::Transformation3d & direct_transformation,
                       const soma::Transformation3d * inverse_transformation,
                       Point3df vs = Point3df( 0., 0., 0. ) );

  /** Apply a spatial transformation to fiber bundles.

      Each point along the bundles is transformed according to the supplied
      transformation.

      This is a BundleListener / BundleProducer stream processing class
      which applies vector field deformation to bundle data. It can be
      typically connected to a BundleReader and a BundleWriter.
  */
  class BundleTransformer : public BundleListener, public BundleProducer
  {
  public:
    BundleTransformer( const carto::rc_ptr<soma::Transformation3d>& direct_transformation );
    virtual ~BundleTransformer();

    virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
    virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
    virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                               const FiberInfo & );
    virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                                  const FiberInfo & );
    virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                                const FiberInfo &, const FiberPoint & );
    virtual void noMoreBundle( const BundleProducer & );

  private:
    carto::rc_ptr<soma::Transformation3d> _transformation;
  };

} // namespace aims

#endif // !defined(AIMS_REGISTRATION_TRANSFORM_OBJECTS_H)
