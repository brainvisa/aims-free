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


#include <cstdlib>
#include <aims/foldgraph/foldgraphattributes.h>
#include <aims/utility/threshold.h>
#include <aims/utility/merge.h>
#include <aims/distancemap/front.h>
#include <aims/distancemap/fastmarching.h>
#include <aims/utility/boundingbox.h>
#include <aims/resampling/motion.h>
#include <aims/graph/graphmanip.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/data/fastAllocationData.h>
#include <aims/math/eigen.h>
#include <aims/utility/converter_bucket.h>
#include <aims/connectivity/component.h>
#include <aims/mesh/mesher.h>
#include <aims/moment/geomMoment.h>
#include <aims/math/gradient.h>
#include <aims/utility/converter_volume.h>
#include <aims/morphology/operatormorpho.h>
#include <graph/graph/graph.h>
#include <cartobase/thread/loopContext.h>
#include <cartobase/thread/threadedLoop.h>
#include <cartobase/thread/thread.h>
#include <cartobase/thread/cpu.h>
#include <cartobase/thread/mutex.h>
#include <cartobase/config/version.h>
#include <cartobase/config/verbose.h>

using namespace aims;
using namespace carto;
using namespace std;


FoldGraphAttributes::FoldGraphAttributes( const AimsData<int16_t> & skel, 
                                          Graph & graph, const Motion *motion, 
                                          int16_t inside, int16_t outside, 
                                          bool withmeshes,
                                          const vector<int> & gver )
  : _skel( skel ), _graph( graph ), _inside( inside ), _outside( outside ), 
    _motion( motion ), _domeshes( withmeshes ), _graphversion( gver ),
    _mesher()
{
  _mesher.setVerbose( false );
  _mesher.setDecimation( 100.0, 5, 3, 180.0 );
  _mesher.setMinFacetNumber( 50 );
}


void FoldGraphAttributes::cleanup()
{
  _depth = AimsData<int16_t>();
  _ndepth = AimsData<int16_t>();
}


AimsData<int16_t> &FoldGraphAttributes::getNDepth()
{
  return _ndepth;
}

const AimsData<int16_t> &FoldGraphAttributes::getNDepth() const
{
  return _ndepth;
}

AimsData<int16_t> &FoldGraphAttributes::getDepth()
{
  return _depth;
}

const AimsData<int16_t> &FoldGraphAttributes::getDepth() const
{
  return _depth;
}

AimsData<float> &FoldGraphAttributes::getBrainDepth()
{
  return _braindepthmap;
}

const AimsData<float> &FoldGraphAttributes::getBrainDepth() const
{
  return _braindepthmap;
}

AimsData<float> &FoldGraphAttributes::getBrainDepthGradX()
{
  return _braindepthmap_gradX;
}

const AimsData<float> &FoldGraphAttributes::getBrainDepthGradX() const
{
  return _braindepthmap_gradX;
}

AimsData<float> &FoldGraphAttributes::getBrainDepthGradY()
{
  return _braindepthmap_gradY;
}

const AimsData<float> &FoldGraphAttributes::getBrainDepthGradY() const
{
  return _braindepthmap_gradY;
}

AimsData<float> &FoldGraphAttributes::getBrainDepthGradZ()
{
  return _braindepthmap_gradZ;
}

const AimsData<float> &FoldGraphAttributes::getBrainDepthGradZ() const
{
  return _braindepthmap_gradZ;
}

AimsData<float> &FoldGraphAttributes::getDilatedDepth()
{
  return _dilated_depthmap;
}

const AimsData<float> &FoldGraphAttributes::getDilatedDepth() const
{
  return _dilated_depthmap;
}

AimsData<float> &FoldGraphAttributes::getDilatedDepthGradX()
{
  return _dilated_depthmap_gradX;
}

const AimsData<float> &FoldGraphAttributes::getDilatedDepthGradX() const
{
  return _dilated_depthmap_gradX;
}

AimsData<float> &FoldGraphAttributes::getDilatedDepthGradY()
{
  return _dilated_depthmap_gradY;
}

const AimsData<float> &FoldGraphAttributes::getDilatedDepthGradY() const
{
  return _dilated_depthmap_gradY;
}

AimsData<float> &FoldGraphAttributes::getDilatedDepthGradZ()
{
  return _dilated_depthmap_gradZ;
}

const AimsData<float> &FoldGraphAttributes::getDilatedDepthGradZ() const
{
  return _dilated_depthmap_gradZ;
}

namespace
{

  static const float	depthfactor = 50.F;


  float threadsByCpu( int maxth )
  {
    if( maxth == 0 )
      return 1;
    int ncpu = cpuCount();
    if( maxth > 0 )
      return ((float) maxth) / ncpu;
    if( ncpu <= -maxth )
      return 1;
    return ((float) -maxth) / ncpu;
  }


  class NodeContext : public LoopContext
  {
  public:
    typedef void (*DoVertex)( Graph &, Vertex *, NodeContext &, int, int );

    NodeContext( Graph & g, DoVertex func, Mesher & mesher )
      : LoopContext(), graph( g ), vertices( g.begin(), g.end() ), 
        vertexFunc( func ), mesher( mesher )
    {}

    virtual ~NodeContext() { }

    virtual void doIt( int startIndex, int count )
    {
      for ( int i = startIndex; i < startIndex + count; i++ )
        vertexFunc( graph, vertices[i], *this, i, graph.order() );
    }

    Graph		& graph;
    vector<Vertex *>	vertices;
    DoVertex		vertexFunc;
    Mesher              & mesher;
  };


  unsigned addPoints( const BucketMap<Void> & bck, Point3df & grav, 
                      const Motion* motion )
  {
    const BucketMap<Void>::Bucket		& bk = bck.begin()->second;
    BucketMap<Void>::Bucket::const_iterator	i, e = bk.end();
    Point3df					p;
    float	vx = bck.sizeX(), vy = bck.sizeY(), vz = bck.sizeZ();

    for( i=bk.begin(); i!=e; ++i )
      {
        if( motion )
          p = motion->transform( Point3df( i->first[0] * vx, i->first[1] * vy, 
                                           i->first[2] * vz ) );
        else
          p = Point3df( i->first[0], i->first[1], i->first[2] );
        grav += p;
      }
    return bk.size();
  }


#if 0
  void extrema( const BucketMap<Void> & bck, 
                const AimsData<int16_t> & depth, int16_t & dmin, 
                int16_t & dmax, bool reset )
  {
    const BucketMap<Void>::Bucket		& bk = bck.begin()->second;
    BucketMap<Void>::Bucket::const_iterator	i, e = bk.end();
    int16_t					x;

    for( i=bk.begin(); i!=e; ++i )
      if( reset )
        {
          dmin = dmax = depth( i->first );
          reset = false;
        }
      else
        {
          x = depth( i->first );
          if( x < dmin )
            dmin = x;
          if( x > dmax )
            dmax = x;
        }
  }
#endif


  void extremaLoc( const BucketMap<Void> & bck, 
                   const AimsData<int16_t> & depth, int16_t & dmin, 
                   int16_t & dmax, Point3d & pmin, Point3d & pmax, bool reset )
  {
    const BucketMap<Void>::Bucket		& bk = bck.begin()->second;
    BucketMap<Void>::Bucket::const_iterator	i, e = bk.end();
    int16_t					x;

    for( i=bk.begin(); i!=e; ++i )
      if( reset )
        {
          pmin = pmax = i->first;
          dmin = dmax = depth( pmin );
          reset = false;
        }
      else
        {
          x = depth( i->first );
          if( x >= 32501 )
          {
            cerr << "invalid point: " << i->first << endl;
          }
          else
          {
            if( x < dmin )
              {
                pmin = i->first;
                dmin = x;
              }
            if( x > dmax )
              {
                pmax = i->first;
                dmax = x;
              }
          }
        }
  }


  void addCovariance( const BucketMap<Void> & bck, AimsData<float> & covar, 
                      const Motion* motion )
  {
    if( bck.empty() )
      return;
    const BucketMap<Void>::Bucket		& bk = bck.begin()->second;
    BucketMap<Void>::Bucket::const_iterator	i, e = bk.end();
    Point3df					p;
    float	vx = bck.sizeX(), vy = bck.sizeY(), vz = bck.sizeZ();

    for( i=bk.begin(); i!=e; ++i )
      {
        if( motion )
          p = motion->transform( Point3df( i->first[0] * vx, i->first[1] * vy, 
                                           i->first[2] * vz ) );
        else
          p = Point3df( i->first[0] * vx, i->first[1] * vy, i->first[2] * vz );
        covar( 0, 0 ) += sqr( p[0] );
        covar( 1, 1 ) += sqr( p[1] );
        covar( 2, 2 ) += sqr( p[2] );
        covar( 0, 1 ) += p[0] * p[1];
        covar( 0, 2 ) += p[0] * p[2];
        covar( 1, 2 ) += p[1] * p[2];
      }
  }


  template <typename T, typename U>
  inline 
  U round( const T & x )
  {
    return x;
  }


  template <>
  inline 
  int round( const float & x )
  {
    return (int) rint( x );
  }


  template <typename T, typename U>
  inline 
  void point2Vec( const AimsVector<T,3> & p, vector<U> & v )
  {
    v[0] = round<T,U>( p[0] );
    v[1] = round<T,U>( p[1] );
    v[2] = round<T,U>( p[2] );
  }


  rc_ptr<AimsSurfaceTriangle> 
  meshBuckets( const list<rc_ptr<BucketMap<Void> > > & bks, Mesher & mesher )
  {
    list<rc_ptr<BucketMap<Void> > >::const_iterator	i, e = bks.end();

    // bounding box of all buckets
    BoundingBox	bb;
    for( i=bks.begin(); i!=e; ++i )
      bb.add( **i );
    Point3df	dmax = bb.maximum();
    float	vx = bks.front()->sizeX();
    float	vy = bks.front()->sizeY();
    float	vz = bks.front()->sizeZ();

    // fill in volume with all buckets
    AimsData<int16_t>	vol( (int) rint( dmax[0] / vx + 1 ), 
                             (int) rint( dmax[1] / vy + 1 ), 
                             (int) rint( dmax[2] / vz + 1 ), 1, 1 );
    vol.setSizeX( vx );
    vol.setSizeY( vy );
    vol.setSizeZ( vz );
    vol = 0;
    vol.fillBorder( -1 );
    RawConverter<BucketMap<Void>, AimsData<int16_t> >	conv;
    for( i=bks.begin(); i!=e; ++i )
      conv.printToVolume( **i, vol );

    // keep only biggest connected component
    // cout << "before AimsConnectedComponent, thread " << pthread_self() << endl;
    AimsConnectedComponent( vol, Connectivity::CONNECTIVITY_18_XYZ, int16_t(0),
                            true, 0, 0, 1, false );
    // cout << "after AimsConnectedComponent, thread " << pthread_self() << endl;

    // mesh
    vol.fillBorder( -1 );
    rc_ptr<AimsSurfaceTriangle> surface( new AimsSurfaceTriangle );
    mesher.getBrain( vol, *surface );

    return surface;
  }


  void meshVertex( Graph & graph, Vertex* v, NodeContext & context, 
                   int i, int n )
  {
    if( v->getSyntax() == "hull" )
      return;

    list<rc_ptr<BucketMap<Void> > >	bks;
    Vertex::iterator			ie, ee;
    rc_ptr<BucketMap<Void> >		bck;
    rc_ptr<AimsSurfaceTriangle>		mesh;
    Edge				*e;

    if( v->getProperty( "aims_ss", bck ) )
      bks.push_back( bck );
    if( v->getProperty( "aims_bottom", bck ) )
      bks.push_back( bck );
    if( v->getProperty( "aims_other", bck ) )
      bks.push_back( bck );
    for( ie=v->begin(), ee=v->end(); ie!=ee; ++ie )
      {
        e = *ie;
        string	synt = e->getSyntax();
        if( synt == "hull_junction" || synt == "junction" )
          {
            if( e->getProperty( "aims_junction", bck ) )
              bks.push_back( bck );
          }
        else if( synt == "plidepassage" 
                 && e->getProperty( "aims_plidepassage", bck ) )
          bks.push_back( bck );
      }

    if( !bks.empty() )
      {
        context.lock();
        cout << "meshing node " << i+1 << " / " << n << "..." << endl;
        context.unlock();
        mesh = meshBuckets( bks, context.mesher );
        // lock a mutex since storeAims() is not thread-safe
        context.lock();
        GraphManip::storeAims( graph, v, "aims_Tmtktri", mesh );
        context.unlock();
      }
  }


  float bucketLength( const BucketMap<Void> & jbk, bool normalized, 
                      float & ndmaxf, const Point3df & nvs )
  {
    float	dmaxf = 0;
    ndmaxf = 0;

    if( jbk.empty() )
      return 0;

    const BucketMap<Void>::Bucket	& bk = jbk.begin()->second;
    Point3d				pmax;

    if( !bk.empty() )
      {
        AimsBucket<Void>	cc;
        cc.setSizeXYZT( jbk.sizeX(), jbk.sizeY(), jbk.sizeZ(), jbk.sizeT() );
        AimsConnectedComponent( cc, jbk, 
                                Connectivity::CONNECTIVITY_26_XYZ, Void(), 
                                true );
        AimsBucket<Void>::iterator	icc, ecc = cc.end();
        list<AimsBucketItem<Void> >::iterator	iicc, eicc;
        // distance maps in each connected component
        for( icc=cc.begin(); icc!=ecc; ++icc )
          {
            float tdmaxf, tndmaxf = 0;
            BucketMap<float>		bdist;
            BucketMap<float>::Bucket	& bd = bdist[0];
            BucketMap<float>::Bucket::iterator	ibd, ebd = bd.end();

            bdist.setSizeXYZT( jbk.sizeX(), jbk.sizeY(), jbk.sizeZ(), 
                               jbk.sizeT() );
            for( iicc=icc->second.begin(), eicc=icc->second.end(); 
                 iicc!=eicc; ++iicc )
              bd[ iicc->location() ] = 1.F;
            bd.begin()->second = 0.F;
            /* 1st distance map from any point, to find one extremity
               (point of max distance from this arbitrary first one) */
            AimsDistanceFrontPropagation( bdist, 1.F, -1.F, 3, 3, 3, 50.F, 
                                          false );
            tdmaxf = -1.F;
            for( ibd=bd.begin(); ibd!=ebd; ++ibd )
              {
                if( ibd->second > tdmaxf )
                  {
                    tdmaxf = ibd->second;
                    pmax = ibd->first;
                  }
                ibd->second = 1.F;
              }
            bd[ pmax ] = 0.F;
            /* 2nd distance map to find the other extremity: point with the
               higher distance from the first one */
            AimsDistanceFrontPropagation( bdist, 1.F, -1.F, 3, 3, 3, 50.F, 
                                          false );
            tdmaxf = -1.F;
            for( ibd=bd.begin(); ibd!=ebd; ++ibd )
              if( ibd->second > tdmaxf )
                tdmaxf = ibd->second;
            if( normalized )
              {
                for( ibd=bd.begin(); ibd!=ebd; ++ibd )
                  ibd->second = 1.F;
                bd[ pmax ] = 0.F;
                bdist.setSizeXYZT( nvs[0], nvs[1], nvs[2], 1. );
                AimsDistanceFrontPropagation( bdist, 1.F, -1.F, 3, 3, 3, 
                                              50.F, false );
                tndmaxf = 0;
                for( ibd=bd.begin(); ibd!=ebd; ++ibd )
                  if( ibd->second > tndmaxf )
                    tndmaxf = ibd->second;
              }
            dmaxf += tdmaxf;
            ndmaxf += tndmaxf;
          }
        // (all distances are multipied by 50, distance map factor)
        dmaxf /= 50.F;
        ndmaxf /= 50.F;
      }

    return dmaxf;
  }

}

class FoldGraphAttributes::DistanceMapThreadContext : public LoopContext
{
public:
  DistanceMapThreadContext( FoldGraphAttributes &fa,
                            const AimsData<int16_t> & th )
    : LoopContext(), foldatt( fa ), thresh( th )
  {}

  virtual ~DistanceMapThreadContext() { }

  virtual void doIt( int startIndex, int count )
  {
    for ( int i = startIndex; i < startIndex + count; i++ )
      switch( i )
      {
        case 0:
          foldatt.prepareNativeDepthMap();
          break;
        case 1:
          foldatt.prepareNormalizedDepthMap( thresh );
          break;
        case 2:
          foldatt.prepareDilatedDepthMap( thresh );
          break;
        case 3:
          foldatt.prepareBrainDepthMap();
          break;
        case 4:
          foldatt.prepareGradientX();
          break;
        case 5:
          foldatt.prepareGradientY();
          break;
        case 6:
          foldatt.prepareGradientZ();
          break;
        case 7:
          foldatt.prepareBrainGradientX();
          break;
        case 8:
          foldatt.prepareBrainGradientY();
          break;
        case 9:
          foldatt.prepareBrainGradientZ();
          break;
        default:
          break;
      }
  }

  FoldGraphAttributes & foldatt;
  const AimsData<int16_t> & thresh;
};


float FoldGraphAttributes::getDepthfactor() const
{
  return depthfactor;
}


void FoldGraphAttributes::prepareNativeDepthMap()
{
  if( verbose )
    cout << "building depth map..." << endl;
  int16_t forbidden = -111;
  AimsDistanceFrontPropagation<int16_t>( _depth, forbidden, _inside, 3, 3, 3,
                                         depthfactor, false );
  if( verbose )
    cout << "depth map done." << endl;
}


void FoldGraphAttributes::prepareNormalizedDepthMap
( const AimsData<int16_t> & th )
{
  if( _motion )
  {
    if( verbose )
      cout << "building normalized depth map..." << endl;
    int16_t forbidden = -111;
    _ndepth = th.clone();
    Point3df  vs( _skel.sizeX(), _skel.sizeY(), _skel.sizeZ() );
    _ndepth.setSizeX
    ( ( _motion->transform( Point3df( vs[0], 0, 0 ) )
    - _motion->transform( Point3df( 0, 0, 0 ) ) ).norm() );
    _ndepth.setSizeY
    ( ( _motion->transform( Point3df( 0, vs[1], 0 ) )
    - _motion->transform( Point3df( 0, 0, 0 ) ) ).norm() );
    _ndepth.setSizeZ
    ( ( _motion->transform( Point3df( 0, 0, vs[2] ) )
    - _motion->transform( Point3df( 0, 0, 0 ) ) ).norm() );
    AimsDistanceFrontPropagation<int16_t>( _ndepth, forbidden, _inside,
                                          3, 3, 3, depthfactor, false );
    if( verbose )
      cout << "normalized depth map done." << endl;
  }
}


void FoldGraphAttributes::prepareDilatedDepthMap
( const AimsData<int16_t> & th )
{
  if( verbose )
    cout << "building dilated depth map..." << endl;
  AimsData<int16_t> thb;
  int16_t forbidden = -111;
  AimsThreshold<int16_t, int16_t>       thr2( AIMS_EQUAL_TO, -111, 0, 0);
  thb = thr2(th);
  thb = AimsData<int16_t>(thb, 1);
  thb.fillBorder(-1);
  thb = AimsMorphoDilation(thb, 1.);
  _dilated_depthmap = AimsData<float>( th.dimX(), th.dimY(),
                                       th.dimZ(), th.dimT() );
  _dilated_depthmap.setSizeX(th.sizeX());
  _dilated_depthmap.setSizeY(th.sizeY());
  _dilated_depthmap.setSizeZ(th.sizeZ());
  carto::Converter< AimsData<int16_t>, AimsData<float> > conv;
  conv.convert( th, _dilated_depthmap );
  int x, y, z, t;
  for (t = 0; t < thb.dimT(); t++)
    for (z = 0; z < thb.dimZ(); z++)
      for (y = 0; y < thb.dimY(); y++)
        for (x = 0; x < thb.dimX(); x++)
          if (thb(x, y, z, t) and th(x, y, z, t) == _inside)
            _dilated_depthmap(x, y, z, t) = forbidden;
  AimsDistanceFrontPropagation<float>( _dilated_depthmap, forbidden, _inside,
                                       3, 3, 3, 1., false );
  if( verbose )
    cout << "dilated depth map done." << endl;
}


void FoldGraphAttributes::prepareGradientX()
{
  AimsGradient<float>   grad;
  _dilated_depthmap_gradX = grad.X(_dilated_depthmap);
}


void FoldGraphAttributes::prepareGradientY()
{
  AimsGradient<float>   grad;
  _dilated_depthmap_gradY = grad.Y(_dilated_depthmap);
}


void FoldGraphAttributes::prepareGradientZ()
{
  AimsGradient<float>   grad;
  _dilated_depthmap_gradZ = grad.Z(_dilated_depthmap);
}


void FoldGraphAttributes::prepareBrainGradientX()
{
  AimsGradient<float>   grad;
  _braindepthmap_gradX = grad.X(_braindepthmap);
}


void FoldGraphAttributes::prepareBrainGradientY()
{
  AimsGradient<float>   grad;
  _braindepthmap_gradY = grad.Y(_braindepthmap);
}


void FoldGraphAttributes::prepareBrainGradientZ()
{
  AimsGradient<float>   grad;
  _braindepthmap_gradZ = grad.Z(_braindepthmap);
}


void FoldGraphAttributes::prepareDepthMap()
{
  if( _depth.dimX() == _skel.dimX() )
    return; // already done

  // geodesic depth on skeleton
  AimsData<int16_t> th;

  cout << "making various depth maps..." << endl;
  int16_t				forbidden = -111;
  AimsThreshold<int16_t, int16_t>	thr( AIMS_BETWEEN, _inside, _outside, 
                                             forbidden );
  th = thr( _skel );
  _depth = th.clone();

  cout << "running 4 distance maps..." << endl;
  int verb = verbose;
  if( maxThreads() != 1 )
    verbose = 0;
  DistanceMapThreadContext   dc( *this, th );
  float tpcpu = threadsByCpu( maxThreads() );
  ThreadedLoop  tl( &dc, 0, 4, 0, tpcpu );
  tl.launch();
  cout << "done." << endl;
  cout << "calculating gradients..." << endl;
  ThreadedLoop  tl2( &dc, 4, 6, 0, tpcpu );
  tl2.launch();
  cout << "done." << endl;
  verbose = verb;
}

void FoldGraphAttributes::prepareBrainDepthMap()
{
  carto::Converter< AimsData<int16_t>, AimsData<float> > conv;
  AimsData<int16_t> th;
  int16_t				forbidden = -111;

  if( verbose )
    cout << "making depth map (unconstrained)..." << endl;
  AimsThreshold<int16_t, int16_t>	thr3( AIMS_EQUAL_TO, _outside, 0,
                                             forbidden );
  th = thr3( _skel );
  _braindepthmap = AimsData<float>(th.dimX(), th.dimY(),
				th.dimZ(), th.dimT() );
  _braindepthmap.setSizeX(th.sizeX());
  _braindepthmap.setSizeY(th.sizeY());
  _braindepthmap.setSizeZ(th.sizeZ());
  conv.convert( th, _braindepthmap );

  AimsDistanceFrontPropagation<float>( _braindepthmap, forbidden, _inside,
                                       3, 3, 3, 1., false );
  if( verbose )
    cout << "done" << endl;
}


vector<int> FoldGraphAttributes::graphVersion() const
{
  vector<int> ver;
  if( !_graphversion.empty() )
  {
    unsigned i, n = _graphversion.size();
    for( i=0; i<n; ++i )
      ver.push_back( _graphversion[i] );
    for( ; i<2; ++i )
      ver.push_back( 0 );
  }
  else
  {
    ver.push_back( CARTOBASE_VERSION_MAJOR );
    ver.push_back( CARTOBASE_VERSION_MINOR );
  }
  return ver;
}


void FoldGraphAttributes::doAll()
{
  vector<int> ver = graphVersion();
  if( ver[0] >= 4 || ( ver[0] == 3 && ver[1] >= 2 ) )
    rebuildCorticalRelations();

  if( _domeshes )
    makeMeshes();
  makeGlobalAttributes();
  makeSimpleSurfaceAttributes();
  makeJunctionAttributes();
  makePliDePassageAttributes();
  makeCorticalRelationAttributes();
  GraphManip::completeGraph( _graph );
  makeSummaryGlobalAttributes();
}


void FoldGraphAttributes::makeMeshes()
{
  cout << "Making meshes...\n";

  NodeContext	nc( _graph, &meshVertex, _mesher );
  float tpcpu = threadsByCpu( maxThreads() );
  ThreadedLoop	tl( &nc, 0, _graph.order(), 0, tpcpu );
  tl.launch();

  cout << "\nDone: meshes\n";
}



void FoldGraphAttributes::makeGlobalAttributes()
{
  string ver;
  vector<int> iver = graphVersion();
  {
    unsigned i, j = iver.size();
    stringstream ss;
    for( i=0; i<j; ++i )
    {
      ss << iver[i];
      if( i < j-1 )
        ss << ".";
    }
    ver = ss.str();
  }
  _graph.setProperty( "CorticalFoldArg_VERSION", ver );
  _graph.setProperty( "datagraph_VERSION", ver );
  _graph.setProperty( "datagraph_compatibility_model_VERSION", ver );

  // Talairach transform
  if( !_motion )
    {
      // if none is given but one is already in the graph, use it
      Motion	m = GraphManip::talairach( _graph );
      if( !m.isIdentity() )
        _motion = new Motion( m );
    }
  else
    GraphManip::storeTalairach( _graph, *_motion, true );

  vector<float>	vs(3), nvs(3);
  vs[0] = _skel.sizeX();
  vs[1] = _skel.sizeY();
  vs[2] = _skel.sizeZ();
  _graph.setProperty( "voxel_size", vs );

  // TODO: AC, PC, IH

  // brain hull volume and mesh surface
  AimsThreshold<int16_t, int16_t>	thr( AIMS_DIFFER, _outside );
  AimsData<int16_t>			bhull = thr.bin( _skel );
  long		sum = 0;
  float		vol = bhull.sizeX() * bhull.sizeY() * bhull.sizeZ();
  int		x, y, z;

  ForEach3d( bhull, x, y, z )
    if( bhull( x, y, z ) != 0 )
      ++sum;
  _graph.setProperty( "brain_hull_volume", vol * sum );
  Mesher	mesher;
  mesher.setVerbose( false );
  mesher.setDecimation( 100.0, 5, 3, 180.0 );
  mesher.setMinFacetNumber( 50 );
  bhull.fillBorder( -1 );
  AimsSurfaceTriangle bhsurf;
  mesher.getBrain( bhull, bhsurf );
  _graph.setProperty( "brain_hull_area", SurfaceManip::meshArea( bhsurf ) );

  if( _motion )
    {
      float	nvvol;
      Point3df	p0( 0, 0, 0 );
      p0 = _motion->transform( p0 );
      nvs[0] = ( _motion->transform( Point3df( vs[0], 0, 0 ) ) - p0 ).norm();
      nvs[1] = ( _motion->transform( Point3df( 0, vs[1], 0 ) ) - p0 ).norm();
      nvs[2] = ( _motion->transform( Point3df( 0, 0, vs[2] ) ) - p0 ).norm();
      nvvol = nvs[0] * nvs[1] * nvs[2];
      _graph.setProperty( "refbrain_hull_volume", nvvol * sum );
      SurfaceManip::meshTransform( bhsurf, *_motion );
      _graph.setProperty( "refbrain_hull_area", 
                          SurfaceManip::meshArea( bhsurf ) );
    }
}

void FoldGraphAttributes::makeSimpleSurfaceAttributes()
{
  prepareDepthMap();
  // prepareBrainDepthMap();

  Graph::iterator	iv, ev = _graph.end();
  Vertex		*v;
  int			np, i;
  float			vvol, nvvol = 0, tmpf1;
  BoundingBox		gbb, gbbt;
  Point3df		grav, rgrav, norm, nvs, ptmp;
  vector<float>		point( 3 );
  int16_t		dmin, dmax;
  AimsFastAllocationData<float>	covar( 3, 3 ), normc( 3, 3 );
  vector<float>			talcovar( 9 );
  Motion			transmot;
  Point3d			pmin, pmax;
  BucketMap<Void>::Bucket::const_iterator	ib, eb;

  cout << "Making single surfaces attributes...\n";

  vector<float>	vs;
  if( !_graph.getProperty( "voxel_size", vs ) )
    vs = vector<float>( 3, 1. );
  vvol = vs[0] * vs[1] * vs[2];
  if( _motion )
    {
      Point3df	p0( 0, 0, 0 );
      p0 = _motion->transform( p0 );
      nvs[0] = ( _motion->transform( Point3df( vs[0], 0, 0 ) ) - p0 ).norm();
      nvs[1] = ( _motion->transform( Point3df( 0, vs[1], 0 ) ) - p0 ).norm();
      nvs[2] = ( _motion->transform( Point3df( 0, 0, vs[2] ) ) - p0 ).norm();
      nvvol = nvs[0] * nvs[1] * nvs[2];
    }

  for( iv=_graph.begin(); iv!=ev; ++iv )
    {
      v = *iv;
      if( !v->hasProperty( "name" ) )
        v->setProperty( "name", string( "unknown" ) );

      rc_ptr<BucketMap<Void> >	ssb, btb, otb;
      v->getProperty( "aims_bottom", btb );
      v->getProperty( "aims_other", otb );
      if( v->getProperty( "aims_ss", ssb ) && !ssb->empty() )
        {
          float	vx = ssb->sizeX();
          float	vy = ssb->sizeY();
          float	vz = ssb->sizeZ();

          BoundingBox	bb;
          BoundingBox	bbt( _motion );
          bb.add( *ssb );
          np = ssb->begin()->second.size();

          v->setProperty( "ss_point_number", np );

          grav = Point3df( 0. );
          addPoints( *ssb, grav, 0 );
          extremaLoc( *ssb, _depth, dmin, dmax, pmin, pmax, true );

          if( btb && !btb->empty() )
            {
              bb.add( *btb );
              np += btb->begin()->second.size();
              v->setProperty( "bottom_point_number", 
                              (int) btb->begin()->second.size() );
              addPoints( *btb, grav, 0 );
              extremaLoc( *btb, _depth, dmin, dmax, pmin, pmax, false );
              // mean depth
              tmpf1 = 0;
              for( ib=btb->begin()->second.begin(), 
                     eb=btb->begin()->second.end(); ib!=eb; ++ib )
                tmpf1 += _depth( ib->first );
              if( btb->begin()->second.size() != 0 )
                {
                  tmpf1 /= btb->begin()->second.size() * depthfactor;
                  v->setProperty( "mean_depth", tmpf1 );
                }
            }
          if( otb && !otb->empty() )
            {
              bb.add( *otb );
              np += otb->begin()->second.size();
              v->setProperty( "other_point_number", 
                              (int) otb->begin()->second.size() );
              addPoints( *otb, grav, 0 );
              extremaLoc( *otb, _depth, dmin, dmax, pmin, pmax, false );
            }
          grav /= np;

          //hull normal
          point[0] = point[1] = point[2] = 0.;
          for( ib=ssb->begin()->second.begin(),
               eb=ssb->begin()->second.end(); ib!=eb; ++ib )
          {
               float gX, gY, gZ;
               gX = _braindepthmap_gradX( ib->first );
               gY = _braindepthmap_gradY( ib->first );
               gZ = _braindepthmap_gradZ( ib->first );
               if (sqrt(gX * gX + gY * gY + gZ * gZ) > 10)
                  continue;
               point[0] += gX;
               point[1] += gY;
               point[2] += gZ;
          }
          float hn_norm = sqrt(point[0] * point[0] + point[1] * point[1] +
		point[2] * point[2]);
          point[0] /= hn_norm;
          point[1] /= hn_norm;
          point[2] /= hn_norm;
          v->setProperty( "hull_normal", point);
          v->setProperty( "hull_normal_weight", hn_norm);

          //depth direction
          point[0] = point[1] = point[2] = 0.;
          for( ib=ssb->begin()->second.begin(),
               eb=ssb->begin()->second.end(); ib!=eb; ++ib )
          {
               float gX, gY, gZ;
               gX = _dilated_depthmap_gradX( ib->first );
               gY = _dilated_depthmap_gradY( ib->first );
               gZ = _dilated_depthmap_gradZ( ib->first );
               if (sqrt(gX * gX + gY * gY + gZ * gZ) > 10)
                  continue;
               point[0] += gX;
               point[1] += gY;
               point[2] += gZ;
          }
          hn_norm = sqrt(point[0] * point[0] + point[1] * point[1] +
		point[2] * point[2]);
          point[0] /= hn_norm;
          point[1] /= hn_norm;
          point[2] /= hn_norm;
          v->setProperty( "depth_direction", point);
          v->setProperty( "depth_direction_weight", hn_norm);
          if( _motion )
            {
              // ref hull normal
	      v->getProperty( "hull_normal", point);
              norm = _motion->transformUnitNormal(point[0], point[1], point[2]);
              point[0] = norm[0];
              point[1] = norm[1];
              point[2] = norm[2];
              v->setProperty( "refhull_normal", point);
	      // ref depth direction
	      v->getProperty( "depth_direction", point);
              norm = _motion->transformUnitNormal(point[0], point[1], point[2]);
              point[0] = norm[0];
              point[1] = norm[1];
              point[2] = norm[2];
              v->setProperty( "refdepth_direction", point);
	      //
              rgrav = _motion->transform
                ( Point3df( grav[0] * vx, grav[1] * vy, grav[2] * vz ) );
              bbt.add( *ssb );
              covar = 0.;
              addCovariance( *ssb, covar, _motion );
              if( btb )
                {
                  bbt.add( *btb );
                  addCovariance( *btb, covar, _motion );
                  // mean depth
                  tmpf1 = 0;
                  for( ib=btb->begin()->second.begin(), 
                         eb=btb->begin()->second.end(); ib!=eb; ++ib )
                    tmpf1 += _ndepth( ib->first );
                  if( btb->begin()->second.size() != 0 )
                    {
                      tmpf1 /= btb->begin()->second.size() * depthfactor;
                      v->setProperty( "refmean_depth", tmpf1 );
                    }
                }
              if( otb )
                {
                  bbt.add( *otb );
                  addCovariance( *otb, covar, _motion );
                }
            }
          v->setProperty( "size", float( np ) * vvol );
          if( _motion )
            v->setProperty( "refsize", float( np ) * nvvol );

          vector<int>	pt(3);
          Point3df	p = bb.minimum();
          pt[0] = (int) rint( p[0] / vx );
          pt[1] = (int) rint( p[1] / vy );
          pt[2] = (int) rint( p[2] / vz );
          v->setProperty( "boundingbox_min", pt );
          p = bb.maximum();
          pt[0] = (int) rint( p[0] / vx );
          pt[1] = (int) rint( p[1] / vy );
          pt[2] = (int) rint( p[2] / vz );
          v->setProperty( "boundingbox_max", pt );
          if( _motion )
            {
              vector<float>	pf(3);
              p = bbt.minimum();
              pf[0] = p[0];
              pf[1] = p[1];
              pf[2] = p[2];
              v->setProperty( "Tal_boundingbox_min", pf );
              p = bbt.maximum();
              pf[0] = p[0];
              pf[1] = p[1];
              pf[2] = p[2];
              v->setProperty( "Tal_boundingbox_max", pf );
            }
          // global bounding boxes
          gbb.add( bb.minimum() );
          gbb.add( bb.maximum() );
          gbbt.add( bbt.minimum() );
          gbbt.add( bbt.maximum() );

          point[0] = grav[0];
          point[1] = grav[1];
          point[2] = grav[2];
          v->setProperty( "gravity_center", point );
          if( _motion )
            {
              point[0] = rgrav[0];
              point[1] = rgrav[1];
              point[2] = rgrav[2];
              v->setProperty( "refgravity_center", point );
            }

          // depth
          //   look in hull junction as well
          Vertex::iterator	ie, ee = v->end();
          for( ie=v->begin(); ie!=ee && (*ie)->getSyntax()!="hull_junction"; 
               ++ie ) {}
          if( ie != ee )
            {
              rc_ptr<BucketMap<Void> >	hjb;
              (*ie)->getProperty( "aims_junction", hjb );
              if( hjb && !hjb->empty() )
                extremaLoc( *hjb, _depth, dmin, dmax, pmin, pmax, false );
            }
          float	dmin2 = float( dmin ) / depthfactor, dmax2;
          if( dmin2 <= vx + 0.01 || dmin2 <= vy + 0.01 || dmin2 <= vz + 0.01 )
            dmin2 = 0;
          v->setProperty( "maxdepth", float( dmax ) / depthfactor );
          v->setProperty( "mindepth", dmin2 );
          if( _motion )
            {
              if( dmin2 != 0 )
                dmin2 = float( _ndepth( pmin ) ) / depthfactor;
              if( dmax != 0 )
                dmax2 = float( _ndepth( pmax ) ) / depthfactor;
              else
                dmax2 = 0;
              v->setProperty( "refmaxdepth", dmax2 );
              v->setProperty( "refmindepth", dmin2 );
            }

          // talcovar
          if( _motion )
            {
              talcovar[0] = covar( 0, 0 );
              talcovar[1] = covar( 0, 1 );
              talcovar[2] = covar( 0, 2 );
              talcovar[3] = covar( 0, 1 );
              talcovar[4] = covar( 1, 1 );
              talcovar[5] = covar( 1, 2 );
              talcovar[6] = covar( 0, 2 );
              talcovar[7] = covar( 1, 2 );
              talcovar[8] = covar( 2, 2 );
              v->setProperty( "talcovar", talcovar );
            }

          // normals

          if( np >= 10 )
            {
              normc = 0.;
              Point3df	mg = -Point3df( grav[0] * vx, grav[1] * vy, 
                                        grav[2] * vz );
              transmot.setTranslation( -Point3df( grav[0] * vx, grav[1] * vy,
                                                  grav[2] * vz ) );
              addCovariance( *ssb, normc, &transmot );
              if( btb )
                addCovariance( *btb, normc, &transmot );
              if( otb )
                addCovariance( *otb, normc, &transmot );
              int	skl = 0;
              v->getProperty( "skeleton_label", skl );
              normc( 1, 0 ) = normc( 0, 1 ); // fill the incomplete sym matrix
              normc( 2, 0 ) = normc( 0, 2 );
              normc( 2, 1 ) = normc( 1, 2 );
              AimsEigen<float>	eigen;
              eigen.setReturnType( AimsEigen<float>::VectorOfEigenValues );
              AimsData<float>	eigenval = eigen.doit( normc );
              unsigned		mineig = 0, /*maxeig = 0,*/ deptheig = 0;
	      unsigned          eig1 = 0, eig2 = 0;
	      float             dotmax = 0;
	      v->getProperty("depth_direction", point);
	      
	      for (unsigned int i = 0; i < 3; ++i)
	      {
                 float dot = abs(point[0] * normc(0, i) +
                                 point[1] * normc(1, i) +
                                 point[2] * normc(2, i));
                 if (dot > dotmax) {
                   dotmax = dot;
                   deptheig = i;
                 }
              }
              eig1 = (deptheig + 1) % 3;
              eig2 = (deptheig + 2) % 3;
              if( eigenval( eig1 ) < eigenval( eig2 ) ) {
                   mineig = eig1;
                   //maxeig = eig2;
	      } else {
                   mineig = eig2;
                   //maxeig = eig1;
	      }
              norm = Point3df( normc( 0, mineig ), normc( 1, mineig ), 
                               normc( 2, mineig ) );
              norm.normalize();
              point[0] = norm[0];
              point[1] = norm[1];
              point[2] = norm[2];
              v->setProperty( "normal", point );
              if( _motion )
                {
                  norm = _motion->transformUnitNormal(norm);
                  point[0] = norm[0];
                  point[1] = norm[1];
                  point[2] = norm[2];
                  v->setProperty( "refnormal", point );
                }
              vector<float> point2( 3 );
              v->getProperty( "normal", point2 );
	      v->getProperty("depth_direction", point);
              norm = vectProduct(Point3df(point[0], point[1], point[2]),
                                 Point3df(point2[0], point2[1], point2[2]));
              norm.normalize();
              point[0] = norm[0];
              point[1] = norm[1];
              point[2] = norm[2];
              v->setProperty( "direction", point );
              if( _motion )
                {
                  norm = _motion->transform( norm ) 
                          - _motion->transform( Point3df( 0. ) );
                  norm.normalize();
                  point[0] = norm[0];
                  point[1] = norm[1];
                  point[2] = norm[2];
                  v->setProperty( "refdirection", point );
                }
            }

          // moments

          GeometricMoment<int16_t>	mom( MomentBase::Normal );
          mom.doit( *ssb );
          if( btb && !btb->empty() )
            {
              GeometricMoment<int16_t> m2( MomentBase::Normal );
              m2.doit( *btb );
              mom += m2;
            }
          if( otb && !otb->empty() )
            {
              GeometricMoment<int16_t> m2( MomentBase::Normal );
              m2.doit( *otb );
              mom += m2;
            }
          vector<float>	vecmom;
          vecmom.reserve( 20 );
          vecmom.push_back( (float) mom.m0() );
          vecmom.push_back( (float) mom.m1()[0] );
          vecmom.push_back( (float) mom.m1()[1] );
          vecmom.push_back( (float) mom.m1()[2] );
          for( i=0; i<6; ++i )
            vecmom.push_back( (float) mom.m2()[i] );
          for( i=0; i<10; ++i )
            vecmom.push_back( (float) mom.m3()[i] );

          v->setProperty( "moments", vecmom );
          // should we provide moments in Talairach space ?
          // invariants should be the same
          // unless scale factors are not isotropic.....
          // so... ?
        }
    }

  // store global bounding boxes
  if( gbb.count() > 0 )
    {
      vector<int>	pt(3);
      Point3df	p = gbb.minimum();
      pt[0] = (int) rint( p[0] / vs[0] );
      pt[1] = (int) rint( p[1] / vs[1] );
      pt[2] = (int) rint( p[2] / vs[2] );
      _graph.setProperty( "boundingbox_min", pt );
      p = gbb.maximum();
      pt[0] = (int) rint( p[0] / vs[0] );
      pt[1] = (int) rint( p[1] / vs[1] );
      pt[2] = (int) rint( p[2] / vs[2] );
      _graph.setProperty( "boundingbox_max", pt );
    }
  if( gbbt.count() > 0 )
    {
      vector<float>	pf(3);
      point2Vec( gbbt.minimum(), pf );
      _graph.setProperty( "Tal_boundingbox_min", pf );
      point2Vec( gbbt.maximum(), pf );
      _graph.setProperty( "Tal_boundingbox_max", pf );
    }

  cout << "Done: single surfaces attributes.\n";
}


void FoldGraphAttributes::makeJunctionAttributes()
{
  prepareDepthMap();

  const set<Edge *>		& edges = ((const Graph &)_graph).edges();
  set<Edge *>::const_iterator	ie, ee = edges.end();
  rc_ptr<BucketMap<Void> >	jbk;
  Point3df			grav;
  unsigned			n;
  AimsFastAllocationData<float>	dirc( 3, 3 ), eigen;
  Motion			transmot;
  vector<float>			point(3);
  vector<int>			pointi(3);
  Point3df			dir;  AimsEigen<float>		eigensys;
  Edge				*e;
  int16_t			dmin, dmax;
  float				dmaxf, ndmaxf;
  Point3d			extr, pmin, pmax;
  BoundingBox			bb, bbt;
  Point3df			nvs;
  float				vvol, nvvol = 0, hjlen = 0, nhjlen = 0;

  cout << "Making junction attributes" << endl;

  eigensys.setReturnType( AimsEigen<float>::VectorOfEigenValues );

  vector<float>	vs;
  if( !_graph.getProperty( "voxel_size", vs ) )
    vs = vector<float>( 3, 1. );
  vvol = vs[0] * vs[1] * vs[2];

  if( _motion )
    {
      Point3df	p0( 0, 0, 0 );
      p0 = _motion->transform( p0 );
      nvs[0] = ( _motion->transform( Point3df( vs[0], 0, 0 ) ) - p0 ).norm();
      nvs[1] = ( _motion->transform( Point3df( 0, vs[1], 0 ) ) - p0 ).norm();
      nvs[2] = ( _motion->transform( Point3df( 0, 0, vs[2] ) ) - p0 ).norm();
      nvvol = nvs[0] * nvs[1] * nvs[2];
    }

  if( _graph.getProperty( "boundingbox_min", pointi ) )
    bb.add( Point3df( pointi[0] * vs[0], pointi[1] * vs[1], 
                      pointi[2] * vs[2] ) );
  if( _graph.getProperty( "boundingbox_max", pointi ) )
    bb.add( Point3df( pointi[0] * vs[0], pointi[1] * vs[1], 
                      pointi[2] * vs[2] ) );
  if( _motion )
    {
      if( _graph.getProperty( "Tal_boundingbox_min", point ) )
        bbt.add( Point3df( point[0], point[1], point[2] ) );
      if( _graph.getProperty( "Tal_boundingbox_max", point ) )
        bbt.add( Point3df( point[0], point[1], point[2] ) );
      bbt.setMotion( _motion );
    }

  for( ie=edges.begin(); ie!=ee; ++ie )
    if( ( (*ie)->getSyntax() == "junction" 
          || (*ie)->getSyntax() == "hull_junction" )
        && (*ie)->getProperty( "aims_junction", jbk ) )
      {
        e = *ie;
        float	vx = jbk->sizeX();
        float	vy = jbk->sizeY();
        float	vz = jbk->sizeZ();

        // bounding box
        bb.add( *jbk );
        if( _motion )
          bbt.add( *jbk );

        grav = 0.;
        n = addPoints( *jbk, grav, 0 );
        grav /= n;
        grav[0] *= vx;
        grav[1] *= vy;
        grav[2] *= vz;

        // direction
        if( n >= 10 )
          {
            dirc = 0.;
            transmot.setTranslation( -grav );
            addCovariance( *jbk, dirc, &transmot );
            dirc( 1, 0 ) = dirc( 0, 1 );	// sym
            dirc( 2, 0 ) = dirc( 0, 2 );
            dirc( 2, 1 ) = dirc( 1, 2 );
            eigen = eigensys.doit( dirc );
            unsigned		maxeig = 0;
            if( eigen( 1 ) > eigen( 0 ) )
              maxeig = 1;
            if( eigen( 2 ) > eigen( maxeig ) )
              maxeig = 2;
            dir = Point3df( dirc( 0, maxeig ), dirc( 1, maxeig ), 
                            dirc( 2, maxeig ) );
            dir.normalize();
            point[0] = dir[0];
            point[1] = dir[1];
            point[2] = dir[2];
            e->setProperty( "direction", point );
            if( _motion )
              {
                dir = _motion->transform( dir ) 
                  - _motion->transform( Point3df( 0. ) );
                dir.normalize();
                point[0] = dir[0];
                point[1] = dir[1];
                point[2] = dir[2];
                e->setProperty( "refdirection", point );
              }
          }

        // depth
        if( e->getSyntax() != "hull_junction" )
          {
            extremaLoc( *jbk, _depth, dmin, dmax, pmin, pmax, true );
            e->setProperty( "mindepth", float( dmin ) / depthfactor );
            e->setProperty( "maxdepth", float( dmax ) / depthfactor );
            if( _motion )
              {
                if( dmin != 0 )
                  e->setProperty( "refmindepth", 
                                  float( _ndepth( pmin ) ) / depthfactor );
                if( dmax != 0 )
                  e->setProperty( "refmaxdepth", 
                                  float( _ndepth( pmax ) ) / depthfactor );
              }
          }

        // extremities
        const BucketMap<Void>::Bucket		& bk = jbk->begin()->second;
        BucketMap<Void>::Bucket::const_iterator	i, j, ebk = bk.end();
        bool					e1e2 = false;
        float					dis;
        Point3d					e1, e2;

        dmaxf = -100.F;
        for( i=bk.begin(); i!=ebk; ++i )
          for( j=i; j!=ebk; ++j )
            {
              extr = j->first - i->first;
              dis = sqr( extr[0] * vx ) + sqr( extr[1] * vy ) 
                + sqr( extr[2] * vz );
              if( dis > dmaxf )
                {
                  dmaxf = dis;
                  e1e2 = true;
                  e1 = i->first;
                  e2 = j->first;
                }
            }
        if( e1e2 )
          {
            point2Vec( e1, pointi );
            e->setProperty( "extremity1", pointi );
            point2Vec( e2, pointi );
            e->setProperty( "extremity2", pointi );
            if( _motion )
              {
                Point3df	ee1 
                  = _motion->transform( Point3df( e1[0] * vx, e1[1] * vy, 
                                                  e1[2] * vz ) );
                point2Vec( ee1, point );
                e->setProperty( "refextremity1", point );
                ee1 = _motion->transform( Point3df( e2[0] * vx, e2[1] * vy, 
                                                    e2[2] * vz ) );
                point2Vec( ee1, point );
                e->setProperty( "refextremity2", point );
              }
          }

        // size
        e->setProperty( "point_number", (int) bk.size() );
        e->setProperty( "size", (float) bk.size() * vvol );
        if( _motion )
          e->setProperty( "refsize", (float) bk.size() * nvvol );

        // length
        dmaxf = bucketLength( *jbk, _motion, ndmaxf, nvs );
        e->setProperty( "length", dmaxf );
        if( _motion )
          e->setProperty( "reflength", ndmaxf );
        if( e->getSyntax() == "hull_junction" )
        {
          hjlen += dmaxf;
          if( _motion )
            nhjlen += ndmaxf;
        }
      }

  // bounding box
  if( bb.count() > 0 )
    {
      Point3df	p = bb.minimum();
      pointi[0] = (int) rint( p[0] / vs[0] );
      pointi[1] = (int) rint( p[1] / vs[1] );
      pointi[2] = (int) rint( p[2] / vs[2] );
      _graph.setProperty( "boundingbox_min", pointi );
      p = bb.maximum();
      pointi[0] = (int) rint( p[0] / vs[0] );
      pointi[1] = (int) rint( p[1] / vs[1] );
      pointi[2] = (int) rint( p[2] / vs[2] );
      _graph.setProperty( "boundingbox_max", pointi );
      if( _motion )
        {
          point2Vec( bbt.minimum(), point );
          _graph.setProperty( "Tal_boundingbox_min", point );
          point2Vec( bbt.maximum(), point );
          _graph.setProperty( "Tal_boundingbox_max", point );
        }
    }

  if( hjlen != 0 )
    _graph.setProperty( "total_sulci_length", hjlen );
  if( nhjlen != 0 )
    _graph.setProperty( "reftotal_sulci_length", nhjlen );

  cout << "Done: junction attributes" << endl;
}


void FoldGraphAttributes::makePliDePassageAttributes()
{
  prepareDepthMap();

  const set<Edge *>		& edges = ((const Graph &)_graph).edges();
  set<Edge *>::const_iterator	ie, ee = edges.end();
  rc_ptr<BucketMap<Void> >	bck;
  Edge				*e;
  int16_t			dmin, dmax;
  Point3d			pmin, pmax;
  vector<int>			pointi( 3 );
  Point3df			pmaxf, nvs;
  vector<float>			point( 3 );
  int				np;
  BoundingBox			bb, bbt;
  float				vvol, nvvol = 0;

  cout << "Making pli de passage attributes" << endl;

  vector<float>	vs;
  if( !_graph.getProperty( "voxel_size", vs ) )
    vs = vector<float>( 3, 1. );
  vvol = vs[0] * vs[1] * vs[2];
  if( _motion )
    {
      Point3df	p0( 0, 0, 0 );
      p0 = _motion->transform( p0 );
      nvs[0] = ( _motion->transform( Point3df( vs[0], 0, 0 ) ) - p0 ).norm();
      nvs[1] = ( _motion->transform( Point3df( 0, vs[1], 0 ) ) - p0 ).norm();
      nvs[2] = ( _motion->transform( Point3df( 0, 0, vs[2] ) ) - p0 ).norm();
      nvvol = nvs[0] * nvs[1] * nvs[2];
    }

  if( _graph.getProperty( "boundingbox_min", pointi ) )
    bb.add( Point3df( pointi[0] * vs[0], pointi[1] * vs[1], 
                      pointi[2] * vs[2] ) );
  if( _graph.getProperty( "boundingbox_max", pointi ) )
    bb.add( Point3df( pointi[0] * vs[0], pointi[1] * vs[1], 
                      pointi[2] * vs[2] ) );
  if( _motion )
    {
      if( _graph.getProperty( "Tal_boundingbox_min", point ) )
        bbt.add( Point3df( point[0], point[1], point[2] ) );
      if( _graph.getProperty( "Tal_boundingbox_max", point ) )
        bbt.add( Point3df( point[0], point[1], point[2] ) );
      bbt.setMotion( _motion );
    }

  float dmax2;
  for( ie=edges.begin(); ie!=ee; ++ie )
    if( (*ie)->getSyntax() == "plidepassage" 
        && (*ie)->getProperty( "aims_plidepassage", bck ) )
    {
      e = *ie;
      float	vx = bck->sizeX();
      float	vy = bck->sizeY();
      float	vz = bck->sizeZ();

      // bounding box
      bb.add( *bck );
      if( _motion )
        bbt.add( *bck );

      // size
      np = bck->begin()->second.size();
      e->setProperty( "point_number", np );
      e->setProperty( "size", float( np ) * vvol );
      if( _motion )
        e->setProperty( "refsize", float( np ) * nvvol );

      // depth and location
      extremaLoc( *bck, _depth, dmin, dmax, pmin, pmax, true );
      e->setProperty( "depth", float( dmax ) / depthfactor );
      point2Vec( pmax, pointi );
      e->setProperty( "location", pointi );
      if( _motion )
        {
          if( dmax != 0 )
            dmax2 = float( _ndepth( pmax ) ) / depthfactor;
          else
            dmax2 = 0;
          e->setProperty( "refdepth", dmax2 );
          pmaxf = _motion->transform( Point3df( pmax[0] * vx, pmax[1] * vy, 
                                                pmax[2] * vz ) );
          point2Vec( pmaxf, point );
          e->setProperty( "reflocation", point );
        }
    }

  // bounding box
  if( bb.count() > 0 )
    {
      Point3df	p = bb.minimum();
      pointi[0] = (int) rint( p[0] / vs[0] );
      pointi[1] = (int) rint( p[1] / vs[1] );
      pointi[2] = (int) rint( p[2] / vs[2] );
      _graph.setProperty( "boundingbox_min", pointi );
      p = bb.maximum();
      pointi[0] = (int) rint( p[0] / vs[0] );
      pointi[1] = (int) rint( p[1] / vs[1] );
      pointi[2] = (int) rint( p[2] / vs[2] );
      _graph.setProperty( "boundingbox_max", pointi );
      if( _motion )
        {
          point2Vec( bbt.minimum(), point );
          _graph.setProperty( "Tal_boundingbox_min", point );
          point2Vec( bbt.maximum(), point );
          _graph.setProperty( "Tal_boundingbox_max", point );
        }
    }

  cout << "Done: pli de passage attributes" << endl;
}


void FoldGraphAttributes::makeCorticalRelationAttributes()
{
  const set<Edge *>		& edges = ((const Graph &)_graph).edges();
  set<Edge *>::const_iterator	ie, ee = edges.end();
  rc_ptr<BucketMap<Void> >	bck, jbk1, jbk2;
  Edge				*e, *hj1, *hj2;
  Vertex			*ss1, *ss2;
  int				np;
  float				dmin, dist, ndist = 0, ndmin = 0;
  Point3d			point, ss1min, ss2min;
  Point3df			pointf, nvs;
  vector<int>			vpoint( 3 );
  vector<float>			vpointf( 3 );
  bool				dzero;
  BoundingBox			bb, bbt;
  float				vvol, nvvol = 0, dmaxf, ndmaxf;

  cout << "Making cortical relations attributes" << endl;

  rc_ptr<BucketMap<Void> >	nullbck( new BucketMap<Void> );
  (*nullbck)[0] = BucketMap<Void>::Bucket();

  vector<float>	vs;
  if( !_graph.getProperty( "voxel_size", vs ) )
    vs = vector<float>( 3, 1. );
  vvol = vs[0] * vs[1] * vs[2];
  if( _motion )
    {
      Point3df	p0( 0, 0, 0 );
      p0 = _motion->transform( p0 );
      nvs[0] = ( _motion->transform( Point3df( vs[0], 0, 0 ) ) - p0 ).norm();
      nvs[1] = ( _motion->transform( Point3df( 0, vs[1], 0 ) ) - p0 ).norm();
      nvs[2] = ( _motion->transform( Point3df( 0, 0, vs[2] ) ) - p0 ).norm();
      nvvol = nvs[0] * nvs[1] * nvs[2];
    }

  if( _graph.getProperty( "boundingbox_min", vpoint ) )
    bb.add( Point3df( vpoint[0] * vs[0], vpoint[1] * vs[1], 
                      vpoint[2] * vs[2] ) );
  if( _graph.getProperty( "boundingbox_max", vpoint ) )
    bb.add( Point3df( vpoint[0] * vs[0], vpoint[1] * vs[1], 
                      vpoint[2] * vs[2] ) );
  if( _motion )
    {
      if( _graph.getProperty( "Tal_boundingbox_min", vpointf ) )
        bbt.add( Point3df( vpointf[0], vpointf[1], vpointf[2] ) );
      if( _graph.getProperty( "Tal_boundingbox_max", vpointf ) )
        bbt.add( Point3df( vpointf[0], vpointf[1], vpointf[2] ) );
      bbt.setMotion( _motion );
    }

  Point3df	p0( 0, 0, 0 ), p1;
  if( _motion )
    p0 = _motion->transform( p0 );

  for( ie=edges.begin(); ie!=ee; ++ie )
    if( (*ie)->getSyntax() == "cortical" 
        && (*ie)->getProperty( "aims_cortical", bck ) )
    {
      e = *ie;
      float	vx = bck->sizeX();
      float	vy = bck->sizeY();
      float	vz = bck->sizeZ();

      const BucketMap<Void>::Bucket		& bk = bck->begin()->second;
      // size
      np = bk.size();
      e->setProperty( "point_number", np );
      e->setProperty( "size", float( np ) * vvol );
      if( _motion )
        e->setProperty( "refsize", float( np ) * nvvol );

      // length
      dmaxf = bucketLength( *bck, _motion, ndmaxf, nvs );
      e->setProperty( "length", dmaxf );
      if( _motion )
        e->setProperty( "reflength", ndmaxf );

      // bounding box
      bb.add( *bck );
      if( _motion )
        bbt.add( *bck );

      // retreive sulci and hull junctions
      Edge::iterator	i = e->begin();
      ss1 = *i;
      ++i;
      ss2 = *i;
      Vertex::iterator	ie2, ee2 = ss1->end();
      for( ie2=ss1->begin(); ie2!=ee2 && (*ie2)->getSyntax()!="hull_junction";
           ++ie2 ) {}
      if( ie2 != ee2 )
        hj1 = *ie2;
      else
        hj1 = 0;
      for( ie2=ss2->begin(), ee2=ss2->end(); 
           ie2!=ee2 && (*ie2)->getSyntax()!="hull_junction"; ++ie2 ) {}
      if( ie2 != ee2 )
        hj2 = *ie2;
      else
        hj2 = 0;
      if( !hj1 || !hj1->getProperty( "aims_junction", jbk1 ) )
        jbk1 = nullbck;
      if( !hj2 || !hj2->getProperty( "aims_junction", jbk2 ) )
        jbk2 = nullbck;
      const BucketMap<Void>::Bucket		& bk1 = jbk1->begin()->second;
      const BucketMap<Void>::Bucket		& bk2 = jbk2->begin()->second;

      // find min distance points
      BucketMap<Void>::Bucket::const_iterator	ib1, eb1 = bk1.end();
      BucketMap<Void>::Bucket::const_iterator	ib2, eb2 = bk2.end();

      // cout << "bk1: " << bk1.size() << ", bk2: " << bk2.size() << endl;
      dmin = numeric_limits<float>::max();
      dzero = false;
      for( ib1=bk1.begin(); ib1!=eb1; ++ib1 )
        for( ib2=bk2.begin(); ib2!=eb2; ++ib2 )
          {
            point = ib2->first - ib1->first;
            dist = sqr( point[0] * vx ) + sqr( point[1] * vy ) 
              + sqr( point[2] * vz );
            if( _motion )
              {
                p1 = _motion->transform( Point3df( point[0] * vx, 
                                                   point[1] * vy, 
                                                   point[2] * vz ) ) - p0;
                ndist = sqr( p1[0] ) + sqr( p1[1] ) + sqr( p1[2] );
              }
            if( dist < 0.0001 )
              dzero = true;
            else if( dist < dmin )
              {
                dmin = dist;
                ndmin = ndist;
                ss1min = ib1->first;
                ss2min = ib2->first;
              }
          }
      if( dmin != numeric_limits<float>::max() )
        {
          // cout << "nearest OK\n";
          point2Vec( ss1min, vpoint );
          e->setProperty( "SS1nearest", vpoint );
          point2Vec( ss2min, vpoint );
          e->setProperty( "SS2nearest", vpoint );
          if( _motion )
            {
              pointf = _motion->transform( ss1min[0] * vx, ss1min[1] * vy, 
                                           ss1min[2] * vz );
              point2Vec( pointf, vpointf );
              e->setProperty( "refSS1nearest", vpointf );
              pointf = _motion->transform( ss2min[0] * vx, ss2min[1] * vy, 
                                           ss2min[2] * vz );
              point2Vec( pointf, vpointf );
              e->setProperty( "refSS2nearest", vpointf );
            }
        }
      // else cout << "no nearest: " << dmin << endl;
      if( dzero )
        {
          dmin = 0;
          ndmin = 0;
        }
      e->setProperty( "dist", sqrt( dmin ) );
      if( _motion )
        e->setProperty( "refdist", sqrt( ndmin ) );
    }

  // bounding box
  if( bb.count() > 0 )
    {
      Point3df	p = bb.minimum();
      vpoint[0] = (int) rint( p[0] / vs[0] );
      vpoint[1] = (int) rint( p[1] / vs[1] );
      vpoint[2] = (int) rint( p[2] / vs[2] );
      _graph.setProperty( "boundingbox_min", vpoint );
      p = bb.maximum();
      vpoint[0] = (int) rint( p[0] / vs[0] );
      vpoint[1] = (int) rint( p[1] / vs[1] );
      vpoint[2] = (int) rint( p[2] / vs[2] );
      _graph.setProperty( "boundingbox_max", vpoint );
      if( _motion )
        {
          point2Vec( bbt.minimum(), vpointf );
          _graph.setProperty( "Tal_boundingbox_min", vpointf );
          point2Vec( bbt.maximum(), vpointf );
          _graph.setProperty( "Tal_boundingbox_max", vpointf );
        }
    }

  cout << "Done: cortical relations attributes" << endl;
}


void FoldGraphAttributes::makeSummaryGlobalAttributes()
{
  float	area = 0, narea = 0, x, csf = 0, grey = 0;
  Graph::iterator	iv, ev = _graph.end();
  for( iv=_graph.begin(); iv!=ev; ++iv )
  {
    if( (*iv)->getProperty( "surface_area", x ) )
      {
        area += x;
        if( (*iv)->getProperty( "refsurface_area", x ) )
          narea += x;
      }
    if( (*iv)->getProperty( "CSF_volume", x ) )
      csf += x;
    if( (*iv)->getProperty( "GM_volume", x ) )
      grey += x;
  }
  if( area != 0 )
  {
    _graph.setProperty( "folds_area", area );
    if( narea != 0 )
      _graph.setProperty( "reffolds_area", narea );
  }
  if( csf != 0 )
    _graph.setProperty( "CSF_volume", csf );
  if( grey != 0 )
    _graph.setProperty( "GM_volume", grey );
}


namespace
{

  void printBucket( AimsData<int16_t> vol,
                    const rc_ptr<BucketMap<Void> > bck, int16_t value )
  {
    const BucketMap<Void>::Bucket & bk = bck->begin()->second;
    BucketMap<Void>::Bucket::const_iterator i, e = bk.end();

    for( i=bk.begin(); i!=e; ++i )
    {
      const Point3d & p = i->first;
      vol( p[0], p[1], p[2] ) = value;
    }
  }

}


AimsData<int16_t> FoldGraphAttributes::rebuildCorticalRelations()
{
  AimsData<int16_t> seedvol( _skel.dimX(), _skel.dimY(), _skel.dimZ() );
  seedvol.setHeader( _skel.header()->cloneHeader() );
  int x, y, z, dx = seedvol.dimX(), dy = seedvol.dimY(), dz = seedvol.dimZ();
  Connectivity c( 0, 0, Connectivity::CONNECTIVITY_26_XYZ );
  Connectivity c6( 0, 0, Connectivity::CONNECTIVITY_6_XYZ );
  int i, j, n = c.nbNeighbors(), m = c6.nbNeighbors();

  // DEBUG
  unsigned long nwork = 0;

  // keep only hull
  seedvol = -1;
  for( z=0; z<dz; ++z )
    for( y=0; y<dy; ++y )
      for( x=0; x<dx; ++x )
        switch( _skel( x, y, z ) )
        {
        case 30:
        case 60:
        case 80:
          for( i=0; i<n; ++i )
          {
            Point3d p = c.xyzOffset(i) + Point3d( x, y, z );
            if( p[0] >= 0 && p[1] >= 0 && p[2] >= 0 && p[0] < dx && p[1] < dy
              && p[2] < dz && _skel( p[0], p[1], p[2] ) == _outside )
            {
              seedvol( x, y, z ) = -10;
              // dilate on 6 neighbourhood
              for( j=0; j<m; ++j )
              {
                Point3d q = c6.xyzOffset(j) + Point3d( x, y, z );
                if( q[0] >= 0 && q[1] >= 0 && q[2] >= 0 && q[0] < dx
                  && q[1] < dy && q[2] < dz )
                {
                  seedvol( q[0], q[1], q[2] ) = -10;
                  ++nwork;
                }
              }
            break;
            }
          }
          break;
        default:
          break;
        }

  cout << "work voxels: " << nwork << endl;
  // print hull_junctions seeds
  set<int16_t> seeds;
  set<Edge *>::iterator ie, ee = _graph.edges().end(), je;
  Edge::const_iterator iv;
  rc_ptr<BucketMap<Void> > bck;

  /* sort hull_junction edges by skeleton_label index.
     This is useful to ensure consistency in the order which buckets will be
     printed into the seeds volume. As they overlap somehow, if we do not do
     this, seeds are not the same from one run to another.
  */
  int index;
  map<int, Edge *> edges;
  for( ie=_graph.edges().begin(); ie!=ee; ++ie )
    if( (*ie)->getSyntax() == "hull_junction"
      && (*ie)->getProperty( "aims_junction", bck ) )
    {
      iv = (*ie)->begin();
      if( (*iv)->getSyntax() != "fold" )
        ++iv;
      if( (*iv)->getProperty( "skeleton_label", index ) )
        edges[index] = *ie;
    }

  map<int, Edge *>::iterator iie, eie = edges.end();

  for( iie=edges.begin(); iie!=eie; ++iie )
  {
    iie->second->getProperty( "aims_junction", bck );
    index = iie->first;
    printBucket( seedvol, bck, index );
    seeds.insert( index );
  }

  // voronoi
  set<int16_t> work;
  work.insert( -10 );
  cout << "seeds: " << seeds.size() << endl;
  FastMarching<Volume<int16_t> > fm( Connectivity::CONNECTIVITY_26_XYZ, true );
  fm.doit( seedvol.volume(), work, seeds );
  VolumeRef<int16_t> voronoi = fm.voronoiVol();

  cout << "clear cortical relations...\n";
  // clear existing cortical relations in graph
  ie=_graph.edges().begin();
  while( ie != ee )
  {
    if( (*ie)->getSyntax() == "cortical" )
    {
      je = ie;
      ++ie;
      _graph.removeEdge( *je );
    }
    else
      ++ie;
  }

  cout << "rebuild cortical relations...\n";
  // take interfaces as cortical relations
  vector<pair<int16_t,int16_t> > interf = fm.midInterfaceLabels();
  map<int16_t, Vertex *> vertices;
  Graph::iterator ivv, ev = _graph.end();
  for( ivv=_graph.begin(); ivv!=ev; ++ivv )
    if( (*ivv)->getSyntax() == "fold"
      && (*ivv)->getProperty( "skeleton_label", index ) )
      vertices[ index ] = *ivv;

  vector<float> vs;
  _graph.getProperty( "voxel_size", vs );
  while( vs.size() < 3 )
    vs.push_back( 1. );
  float voxvol = vs[0] * vs[1] * vs[2];

  vector<pair<int16_t,int16_t> >::const_iterator ii, ei = interf.end();
  Edge *cort;
  Converter<BucketMap<float>, BucketMap<Void> > cv;
  cout << "interfaces: " << interf.size() << endl;
  for( ii=interf.begin(); ii!=ei; ++ii )
  {
    const BucketMap<float> & cbck = fm.midInterface( ii->first, ii->second );
    cort = _graph.addEdge( vertices[ ii->first ], vertices[ ii->second ],
      "cortical" );
    GraphManip::storeAims( _graph, cort, "aims_cortical",
      rc_ptr<BucketMap<Void> >( cv( cbck ) ) );
    cort->setProperty( "point_number", (int) cbck.begin()->second.size() );
    cort->setProperty( "size", (float) cbck.begin()->second.size() * voxvol );
  }

  cout << "done.\n";
  return voronoi;
}


namespace
{
  struct ThicknessStat
  {
    ThicknessStat() : npoint( 0 ), mean( 0 ), var( 0 ) {}
    int npoint;
    float mean;
    float var;
  };
}


void FoldGraphAttributes::thickness( const BucketMap<float> & midInterface,
                                     const VolumeRef<int16_t> voronoi )
{
  Graph::iterator iv, ev = _graph.end();
  int skel;
  Vertex *v;
  const BucketMap<float>::Bucket & mid0 = midInterface.begin()->second;
  BucketMap<float>::Bucket::const_iterator ib, eb = mid0.end();
  map<int16_t, ThicknessStat> tstats;

  for( ib=mid0.begin(); ib!=eb; ++ib )
  {
    const Point3d & p = ib->first;
    skel = voronoi.at( p[0], p[1], p[2] );
    if( skel > 0 )
    {
      ThicknessStat & s = tstats[ skel ];
      ++s.npoint;
      s.mean += ib->second;
      s.var += ib->second * ib->second;
    }
  }

  for( iv=_graph.begin(); iv!=ev; ++iv )
  {
    v = *iv;
    if( v->getSyntax() != "fold" )
      continue;
    v->getProperty( "skeleton_label", skel );
    ThicknessStat & s = tstats[ skel ];
    v->setProperty( "mid_interface_voxels", s.npoint );
    if( s.npoint != 0 )
    {
      s.mean /= s.npoint;
      v->setProperty( "thickness_mean", s.mean );
      if( s.npoint >= 2 )
        v->setProperty( "thickness_std",
                        sqrt( ( s.var - s.npoint * s.mean * s.mean )
                        / (s.npoint - 1) ) );
      else
        v->setProperty( "thickness_std", 0.F ); // should we just omit it ?
    }
  }
}


void FoldGraphAttributes::greyAndCSFVolumes( const VolumeRef<int16_t> gw,
                                             const VolumeRef<int16_t> voronoi )
{
  Graph::iterator iv, ev = _graph.end();
  int skel, g;
  Vertex *v;
  int16_t CSF_label = 255;
  int16_t GM_label = 100;
  int x, y, z, dx = voronoi->getSizeX(), dy = voronoi->getSizeY(),
    dz = voronoi->getSizeZ();
  vector<int> grey( 65536, 0 );
  vector<int> csf( 65536, 0 );
  vector<float> vs;
  gw->header().getProperty( "voxel_size", vs );
  float vvol = 1;
  if( vs.size() >= 1 )
  {
    vvol *= vs[0];
    if( vs.size() >= 2 )
    {
      vvol *= vs[1];
      if( vs.size() >= 3 )
        vvol *= vs[2];
    }
  }

  for( z=0; z<dz; ++z )
    for( y=0; y<dy; ++y )
      for( x=0; x<dx; ++x )
      {
        skel = voronoi->at( x, y, z );
        if( skel > 0 )
        {
          g = gw->at( x, y, z );
          if( g == GM_label )
            ++grey[skel];
          else if( g == CSF_label )
            ++csf[skel];
        }
      }

  for( iv=_graph.begin(); iv!=ev; ++iv )
  {
    v = *iv;
    if( v->getSyntax() != "fold" )
      continue;
    v->getProperty( "skeleton_label", skel );
    v->setProperty( "GM_volume", grey[ skel ] * vvol );
    v->setProperty( "LCR_volume", csf[ skel ] * vvol ); // use CSF_volume !
    v->setProperty( "CSF_volume", csf[ skel ] * vvol );
  }
}


void FoldGraphAttributes::setMaxThreads( int mt )
{
  _maxthreads = mt;
}

