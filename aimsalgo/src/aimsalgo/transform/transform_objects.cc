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

#include <aims/bucket/bucketMap.h>
#include <aims/fibers/bundles.h>
#include <aims/mesh/surface.h>

using namespace std;
using namespace carto;
using namespace aims;

namespace
{

  void transformGraphObject( GraphObject *go,
                             const Transformation3d & direct_transformation,
                             const Point3df & vso,
                             vector<int> &bbmin, vector<int> & bbmax )
  {
    Object iter;
    for( iter=go->objectIterator(); iter->isValid(); iter->next() )
    {
      rc_ptr<AimsSurfaceTriangle> mesh;
      try
      {
        mesh = iter->currentValue()->value<rc_ptr<AimsSurfaceTriangle> >();
        transformMesh( *mesh, direct_transformation );
        continue;
      }
      catch( ... )
      {
      }

      rc_ptr<BucketMap<Void> > bck;
      try
      {
        bck = iter->currentValue()->value<rc_ptr<BucketMap<Void> > >();
        rc_ptr<BucketMap<Void> > obk
          = transformBucketDirect( *bck, direct_transformation, vso );
        go->setProperty( iter->key(), obk );

        // bounding box
        BucketMap<Void>::Bucket::const_iterator
          ip, ep = obk->begin()->second.end();
        for( ip=obk->begin()->second.begin(); ip!=ep; ++ip )
        {
          const Point3d & p = ip->first;
          if( bbmin.empty() )
          {
            bbmin.resize( 3 );
            bbmin[0] = p[0];
            bbmin[1] = p[1];
            bbmin[2] = p[2];
            bbmax = bbmin;
          }
          else
          {
            if( p[0] < bbmin[0] )
              bbmin[0] = p[0];
            if( p[1] < bbmin[1] )
              bbmin[1] = p[1];
            if( p[1] < bbmin[1] )
              bbmin[1] = p[1];
            if( p[0] > bbmax[0] )
              bbmax[0] = p[0];
            if( p[1] > bbmax[1] )
              bbmax[1] = p[1];
            if( p[2] > bbmax[2] )
              bbmax[2] = p[2];
          }
        }

        continue;
      }
      catch( ... )
      {
      }

      rc_ptr<AimsTimeSurface<2, Void> > mesh2;
      try
      {
        mesh2
          = iter->currentValue()->value<rc_ptr<AimsTimeSurface<2, Void> > >();
        transformMesh( *mesh2, direct_transformation );
        continue;
      }
      catch( ... )
      {
      }

      rc_ptr<AimsTimeSurface<4, Void> > mesh4;
      try
      {
        mesh4
          = iter->currentValue()->value<rc_ptr<AimsTimeSurface<4, Void> > >();
        transformMesh( *mesh4, direct_transformation );
        continue;
      }
      catch( ... )
      {
      }
    }
  }

} // anonymous namespace

namespace aims
{

rc_ptr<BucketMap<Void> >
transformBucketDirect( const BucketMap<Void> & bck,
                       const Transformation3d & direct_transformation,
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
    BucketMap<Void>::Bucket::const_iterator ip, ep = ib->second.end();
    for( ip=ib->second.begin(); ip!=ep; ++ip )
    {
      Point3df p( ip->first[0] * vs[0], ip->first[1] * vs[1],
                  ip->first[2] * vs[2] );
      p = direct_transformation.transform( p );
      obk[ Point3d( int( rint( p[0] / vvs[0] ) ),
                    int( rint( p[1] / vvs[1] ) ),
                    int( rint( p[2] / vvs[2] ) ) ) ] = Void();
    }
  }

  return out;
}

void transformGraph( Graph & graph,
                     const Transformation3d & direct_transformation,
                     Point3df vvs )
{
  vector<float> vs;
  Object ovs = graph.getProperty( "voxel_size" );
  Object ivs = ovs->objectIterator();
  for( ; ivs->isValid(); ivs->next() )
    vs.push_back( float( ivs->currentValue()->getScalar() ) );
  while( vs.size() < 3 )
    vs.push_back( 1.f );

  if( vvs == Point3df( 0., 0., 0. ) )
  {
    vvs[0] = vs[0];
    vvs[1] = vs[1];
    vvs[2] = vs[2];
  }
  else
    graph.setProperty( "voxel_size", vvs.toStdVector() );

  vector<int> bbmin, bbmax;

  Graph::iterator iv, ev = graph.end();
  Object iter;
  for( iv=graph.begin(); iv!=ev; ++iv )
    transformGraphObject( *iv, direct_transformation, vvs, bbmin, bbmax );

  const set<Edge *> & edges = graph.edges();
  set<Edge *>::const_iterator ie, ee = edges.end();
  for( ie=edges.begin(); ie!=ee; ++ie )
    transformGraphObject( *ie, direct_transformation, vvs, bbmin, bbmax );

  graph.setProperty( "boundingbox_min", bbmin );
  graph.setProperty( "boundingbox_max", bbmax );

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
      pos[0] = int( rint( p[0] / vvs[0] ) );
      pos[1] = int( rint( p[1] / vvs[1] ) );
      pos[2] = int( rint( p[2] / vvs[2] ) );
      graph.setProperty( *is, pos );
    }
}


// Bundles

BundleTransformer::
BundleTransformer(const rc_ptr<Transformation3d>& direct_transformation)
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
  FiberPoint opt;
  opt = _transformation->transform( opt );
  addFiberPoint( bi, fi, opt );
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
template void transformMesh( AimsTimeSurface<2, Void> &, const Transformation3d & );
template void transformMesh( AimsTimeSurface<3, Void> &, const Transformation3d & );
template void transformMesh( AimsTimeSurface<4, Void> &, const Transformation3d & );
} // namespace aims
