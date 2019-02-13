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

#include <aims/transform/transform_objects.h>

#include <cassert>
#include <iostream>
#include <vector>
#include <utility>

#include <aims/bucket/sparsevolume.h>
#include <aims/bucket/bucketMap.h>
#include <graph/graph/graph.h>
#include <aims/fibers/bundles.h>
#include <aims/resampling/nearestneighborresampler.h>
#include <aims/mesh/surface.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/transformation/transformation_chain.h>

using namespace std;
using namespace carto;
using namespace aims;

namespace
{

  template <class T>
  carto::const_ref<T> external_ref(const T& object)
  {
    return carto::const_ref<T>(&object, true);
  }

  // Functions with attributes are declared separately from their definition,
  // because "attributes are not allowed on a function-definition" (GCC 4.9).
  void
  transformBucketComponentDirect(
    const BucketMap<Void>::Bucket & in_bck,
    BucketMap<Void>::Bucket * out_bck,
    const Transformation3d & direct_transformation,
    Point3df in_vs,
    Point3df out_vs ) __attribute__((__nonnull__(2)));
  void update_bounding_box_bucket(
    const BucketMap<Void>::Bucket &bck,
    vector<int> *bbmin,
    vector<int> *bbmax) __attribute__((__nonnull__(2, 3)));
  void transformGraphObject( GraphObject *go,
                             const Transformation3d & direct_transformation,
                             const Transformation3d * inverse_transformation,
                             const Point3df & vso,
                             vector<int> *bbmin,
                             vector<int> *bbmax,
                             bool *inverse_transformation_missing = NULL)
    __attribute__((__nonnull__(1, 5, 6)));
  void update_bounding_box_bucket(
    const BucketMap<Void> &bck,
    vector<int> *bbmin,
    vector<int> *bbmax) __attribute__((__nonnull__(2, 3)));


  void update_bounding_box_bucket(
    const BucketMap<Void>::Bucket &bck,
    vector<int> *bbmin,
    vector<int> *bbmax)
  {
    BucketMap<Void>::Bucket::const_iterator
      ip, ep = bck.end();
    for( ip=bck.begin(); ip!=ep; ++ip )
    {
      const Point3d & p = ip->first;
      if( bbmin->empty() )
      {
        bbmin->resize( 3 );
        (*bbmin)[0] = p[0];
        (*bbmin)[1] = p[1];
        (*bbmin)[2] = p[2];
        *bbmax = *bbmin;
      }
      else
      {
        if( p[0] < (*bbmin)[0] )
          (*bbmin)[0] = p[0];
        if( p[1] < (*bbmin)[1] )
          (*bbmin)[1] = p[1];
        if( p[2] < (*bbmin)[2] )
          (*bbmin)[2] = p[2];
        if( p[0] > (*bbmax)[0] )
          (*bbmax)[0] = p[0];
        if( p[1] > (*bbmax)[1] )
          (*bbmax)[1] = p[1];
        if( p[2] > (*bbmax)[2] )
          (*bbmax)[2] = p[2];
      }
    }
  }

  void update_bounding_box_bucket(
    const BucketMap<Void> &bck,
    vector<int> *bbmin,
    vector<int> *bbmax)
  {
    BucketMap<Void>::const_iterator ip, ep = bck.end();
    for( ip=bck.begin(); ip!=ep; ++ip ) {
      update_bounding_box_bucket(ip->second, bbmin, bbmax);
    }
  }

  void transformGraphObject( GraphObject *go,
                             const soma::Transformation3d
                                & direct_transformation,
                             const soma::Transformation3d
                                * inverse_transformation,
                             const Point3df & vso,
                             vector<int> *bbmin,
                             vector<int> *bbmax,
                             bool *inverse_transformation_missing)
  {
    assert(bbmin);
    assert(bbmax);
    for( Object iter = go->objectIterator(); iter->isValid(); iter->next() )
    {
      {
        rc_ptr<AimsSurfaceTriangle> mesh;
        try {
          mesh = iter->currentValue()->value<rc_ptr<AimsSurfaceTriangle> >();
        } catch( ... ) {}
        if(!mesh.isNull()) {
          transformMesh( *mesh, direct_transformation );
          continue;
        }
      }

      {
        rc_ptr<BucketMap<Void> > bck;
        try {
          bck = iter->currentValue()->value<rc_ptr<BucketMap<Void> > >();
        } catch( ... ) {}
        if(!bck.isNull()) {
          rc_ptr<BucketMap<Void> > obk;
          if(inverse_transformation) {
            obk = resampleBucket( *bck, direct_transformation,
                                  *inverse_transformation, vso );
          } else {
            if(inverse_transformation_missing) {
              *inverse_transformation_missing = true;
            }
            obk = transformBucketDirect( *bck, direct_transformation, vso );
          }
          go->setProperty( iter->key(), obk );
          update_bounding_box_bucket(*obk, bbmin, bbmax);
          continue;
        }
      }

      {
        rc_ptr<AimsTimeSurface<2, Void> > mesh2;
        try {
          mesh2 = iter->currentValue()
            ->value<rc_ptr<AimsTimeSurface<2, Void> > >();
        } catch( ... ) {}
        if(!mesh2.isNull()) {
          transformMesh( *mesh2, direct_transformation );
          continue;
        }
      }

      {
        rc_ptr<AimsTimeSurface<4, Void> > mesh4;
        try {
          mesh4 = iter->currentValue()
            ->value<rc_ptr<AimsTimeSurface<4, Void> > >();
        } catch( ... ) {}
        if(!mesh4.isNull()) {
          transformMesh( *mesh4, direct_transformation );
          continue;
        }
      }

      // TODO Volume?

      if(false) {  // debug
        std::clog << "Graph component " << iter->key()
                  << " of type " << iter->currentValue()->type()
                  << " could not be transformed" << std::endl;
      }
    }
  }

  void
  transformBucketComponentDirect(
    const BucketMap<Void>::Bucket & in_bck,
    BucketMap<Void>::Bucket * out_bck,
    const soma::Transformation3d & direct_transformation,
    Point3df in_vs,
    Point3df out_vs )
  {
    BucketMap<Void>::Bucket::const_iterator ip, ep = in_bck.end();
    for( ip=in_bck.begin(); ip!=ep; ++ip )
    {
      Point3df p( ip->first[0] * in_vs[0],
                  ip->first[1] * in_vs[1],
                  ip->first[2] * in_vs[2] );
      p = direct_transformation.transform( p );
      out_bck->insert(std::make_pair(
        Point3d( int( rint( p[0] / out_vs[0] ) ),
                 int( rint( p[1] / out_vs[1] ) ),
                 int( rint( p[2] / out_vs[2] ) ) ),
        Void()
      ));
    }
  }
} // end of anonymous namespace

namespace aims
{

rc_ptr<BucketMap<Void> >
transformBucketDirect( const BucketMap<Void> & bck,
                       const soma::Transformation3d & direct_transformation,
                       Point3df vvs )
{
  if( vvs == Point3df( 0., 0., 0. ) )
  {
    vvs[0] = bck.sizeX();
    vvs[1] = bck.sizeY();
    vvs[2] = bck.sizeZ();
  }
  BucketMap<Void>::const_iterator ib, eb = bck.end();
  rc_ptr<BucketMap<Void> > out( new BucketMap<Void> );
  out->setSizeX( vvs[0] );
  out->setSizeY( vvs[1] );
  out->setSizeZ( vvs[2] );
  Point3df vs( bck.sizeX(), bck.sizeY(), bck.sizeZ() );

  for( ib=bck.begin(); ib!=eb; ++ib )
  {
    BucketMap<Void>::Bucket & obk = (*out)[ ib->first ];
    transformBucketComponentDirect(ib->second, &obk,
                                   direct_transformation,
                                   vs, vvs);
  }

  return out;
}

rc_ptr<BucketMap<Void> >
resampleBucket( const BucketMap<Void> & bck,
                const soma::Transformation3d & direct_transformation,
                const soma::Transformation3d & inverse_transformation,
                Point3df out_vs,
                bool also_pushforward)
{
  if( out_vs == Point3df( 0., 0., 0. ) )
  {
    out_vs[0] = bck.sizeX();
    out_vs[1] = bck.sizeY();
    out_vs[2] = bck.sizeZ();
  }
  rc_ptr<BucketMap<Void> > out( new BucketMap<Void> );
  out->setSizeX( out_vs[0] );
  out->setSizeY( out_vs[1] );
  out->setSizeZ( out_vs[2] );

  const Point3df in_vs( bck.sizeX(), bck.sizeY(), bck.sizeZ() );

  BucketMap<Void>::const_iterator ib, eb = bck.end();
  for( ib=bck.begin(); ib!=eb; ++ib )
  {
    BucketMap<Void>::Bucket & obk = (*out)[ ib->first ];
    if(ib->second.empty()) {
      // We are done, an empty output bucket is already allocated above.
      continue;
    }

    // 1. Compute the bounding box of the Bucket to allocate a volume of the
    // correct size
    std::vector<int> in_bbmin, in_bbmax;
    update_bounding_box_bucket(ib->second, &in_bbmin, &in_bbmax);
    assert(!in_bbmin.empty());
    const Point3d in_bbmin_point(in_bbmin[0], in_bbmin[1], in_bbmin[2]);
    Volume<uint8_t> in_vol(in_bbmax[0] - in_bbmin[0] + 1,
                           in_bbmax[1] - in_bbmin[1] + 1,
                           in_bbmax[2] - in_bbmin[2] + 1);
    in_vol.fill(0);

    // 2. Convert the input bucket to a volume shifted by in_bbmin
    BucketMap<Void>::Bucket::const_iterator ip, ep = ib->second.end();
    for( ip=ib->second.begin(); ip!=ep; ++ip ) {
      in_vol.at(ip->first - in_bbmin_point) = 1;
    }

    // 3. Convert the bucket using the direct method to estimate the new FoV
    std::vector<int> out_bbmin, out_bbmax;
    {
      BucketMap<Void>::Bucket *direct_bck;
      std::unique_ptr<BucketMap<Void>::Bucket > throwaway_bck;
      if(also_pushforward) {
        direct_bck = &obk;
      } else {
        throwaway_bck.reset(new BucketMap<Void>::Bucket);
        direct_bck = throwaway_bck.get();
      }
      transformBucketComponentDirect(ib->second, direct_bck,
                                     direct_transformation, in_vs, out_vs);
      update_bounding_box_bucket(*direct_bck, &out_bbmin, &out_bbmax);
    }

    // 4. Prepare a composed transformation that compensates for the offsets
    // in_bbmin and out_bbmin
    aims::TransformationChain3d transform_chain;
    {
      transform_chain.push_back(external_ref(inverse_transformation));
      rc_ptr<aims::AffineTransformation3d> translation_and_scaling_to_voxel(
        new aims::AffineTransformation3d);
      translation_and_scaling_to_voxel->translation()[0] = -in_bbmin[0];
      translation_and_scaling_to_voxel->translation()[1] = -in_bbmin[1];
      translation_and_scaling_to_voxel->translation()[2] = -in_bbmin[2];
      translation_and_scaling_to_voxel->rotation()(0, 0) = 1.f / in_vs[0];
      translation_and_scaling_to_voxel->rotation()(1, 1) = 1.f / in_vs[1];
      translation_and_scaling_to_voxel->rotation()(2, 2) = 1.f / in_vs[2];
      transform_chain.push_back(translation_and_scaling_to_voxel);
    }

    // 5. Resample
    //
    // FIXME: we need to add a border to the coordinates, otherwise upsampling
    // a bucket will miss some voxels on the edges.
    NearestNeighborResampler<uint8_t> resampler;
    Point3df out_pos;
    for(int z = out_bbmin[2]; z <= out_bbmax[2]; ++z) {
      out_pos[2] = z * out_vs[2];
      for(int y = out_bbmin[1]; y <= out_bbmax[1]; ++y) {
        out_pos[1] = y * out_vs[1];
        for(int x = out_bbmin[0]; x <= out_bbmax[0]; ++x) {
          out_pos[0] = x * out_vs[0];
          uint8_t resampled_value;
          resampler.resample_inv_to_vox(in_vol, transform_chain, 0,
                                        out_pos, resampled_value, 0);
          if(resampled_value != 0) {
            obk.insert(std::make_pair(Point3d(x, y, z), Void()));
          }
        }
      }
    }
  }

  return out;
}

void transformGraph( Graph & graph,
                     const soma::Transformation3d & direct_transformation,
                     const soma::Transformation3d * inverse_transformation,
                     Point3df out_vs )
{
  vector<float> vs;
  Object ovs = graph.getProperty( "voxel_size" );
  Object ivs = ovs->objectIterator();
  for( ; ivs->isValid(); ivs->next() )
    vs.push_back( float( ivs->currentValue()->getScalar() ) );
  while( vs.size() < 3 )
    vs.push_back( 1.f );

  if( out_vs == Point3df( 0., 0., 0. ) )
  {
    out_vs[0] = vs[0];
    out_vs[1] = vs[1];
    out_vs[2] = vs[2];
  }
  else
    graph.setProperty( "voxel_size", out_vs.toStdVector() );

  vector<int> bbmin, bbmax;
  bool inverse_transformation_missing = false;

  Graph::iterator iv, ev = graph.end();
  Object iter;
  for( iv=graph.begin(); iv!=ev; ++iv )
    transformGraphObject( *iv, direct_transformation, inverse_transformation,
                          out_vs, &bbmin, &bbmax,
                          &inverse_transformation_missing );

  const set<Edge *> & edges = graph.edges();
  set<Edge *>::const_iterator ie, ee = edges.end();
  for( ie=edges.begin(); ie!=ee; ++ie )
    transformGraphObject( *ie, direct_transformation, inverse_transformation,
                          out_vs, &bbmin, &bbmax,
                          &inverse_transformation_missing );

  graph.setProperty( "boundingbox_min", bbmin );
  graph.setProperty( "boundingbox_max", bbmax );

  if(inverse_transformation_missing) {
    std::cout << "Warning: no inverse transformation provided, buckets were "
      "transformed using a push-forward transformation only (this should work "
      "fine when downsampling, but will create holes when upsampling)."
      << std::endl;
  }

  vector<int> pos;
  vector<string> poskeys(3);
  poskeys[0] = "anterior_commissure";
  poskeys[1] = "posterior_commissure";
  poskeys[2] = "interhemi_point";
  vector<string>::iterator is, es = poskeys.end();
  for( is=poskeys.begin(); is!=es; ++is )
    if( graph.getProperty( *is, pos ) )
    {
      Point3df p( pos[0] * vs[0], pos[1] * vs[1], pos[2] * vs[2] );
      p = direct_transformation.transform( p );
      pos[0] = int( rint( p[0] / out_vs[0] ) );
      pos[1] = int( rint( p[1] / out_vs[1] ) );
      pos[2] = int( rint( p[2] / out_vs[2] ) );
      graph.setProperty( *is, pos );
    }

  // FIXME: We should remove attributes whose values are obsolete, but we
  // cannot remove them altogether because some of these attributes are
  // mandatory.
  std::cout << "Warning: Graph attributes (volume, area, thickness...) "
    "have not been updated according to the transformation. "
    "Please run AimsFoldArgAtt to fix the values of basic attributes, or the "
    "CorticalFoldsGraphUpgradeFromOld BrainVisa process, which can be found "
    "under Morphologist/Sulci/graphmanipulation, for a complete update."
    << std::endl;
}


// Bundles

BundleTransformer::
BundleTransformer(const const_ref<soma::Transformation3d>& direct_transformation)
  : BundleListener(), BundleProducer(),
    _transformation( direct_transformation )
{
}


BundleTransformer::~BundleTransformer()
{
}


void BundleTransformer::bundleStarted( const BundleProducer &,
                                       const BundleInfo &bi )
{
  startBundle( bi );
}


void BundleTransformer::bundleTerminated( const BundleProducer &,
                                          const BundleInfo & bi )
{
  terminateBundle( bi );
}


void BundleTransformer::fiberStarted( const BundleProducer &,
                                      const BundleInfo & bi,
                                      const FiberInfo & fi )
{
  startFiber( bi, fi );
}


void BundleTransformer::fiberTerminated( const BundleProducer &,
                                         const BundleInfo & bi,
                                         const FiberInfo & fi )
{
  terminateFiber( bi, fi );
}


void BundleTransformer::newFiberPoint( const BundleProducer &,
                                       const BundleInfo & bi,
                                       const FiberInfo & fi,
                                       const FiberPoint & /*pt*/ )
{
  addFiberPoint( bi, fi, _transformation->transform( pt ) );
}


void BundleTransformer::noMoreBundle( const BundleProducer & )
{
  BundleProducer::noMoreBundle();
}

} // namespace aims

// template instantiations
#include <aims/transform/transform_objects_d.h>
namespace aims
{
template void transformMesh( AimsTimeSurface<2, Void> &,
                             const soma::Transformation3d & );
template void transformMesh( AimsTimeSurface<3, Void> &,
                             const soma::Transformation3d & );
template void transformMesh( AimsTimeSurface<4, Void> &,
                             const soma::Transformation3d & );
} // namespace aims
