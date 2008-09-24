/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/distancemap/fastmarching.h>
#include <aims/math/mathelem.h>
#include <map>
#include <aims/data/data.h>
#include <aims/utility/converter_bucket.h>

using namespace aims;
using namespace carto;
using namespace std;
using namespace aims::internal::fastmarching;

template <typename T>
FastMarching<T>::FastMarching( Connectivity::Type c, bool mid_interface)
  : _connectivity( c ), _mid_interface_option( mid_interface),
    _verbose( false )
{
}


template <typename T>
FastMarching<T>::~FastMarching()
{
}


namespace
{

  template <typename T>
  BucketMap<Void>* labelBucket( const SparseVolume<T> & labels,
                                const set<int16_t> & worklabels )
  {
    BucketMap<Void> * workbucket = new BucketMap<Void>;
    BucketMap<Void>::Bucket & bck = (*workbucket)[0];
    set<int16_t>::const_iterator end = worklabels.end();

    typename SparseVolume<T>::const_iterator i, e = labels.end();
    typename SparseVolume<T>::const_LowLevelStorage::const_iterator ib, eb;
    for( i=labels.begin(); i!=e; ++i )
      for( ib=i->second.begin(), eb=i->second.end(); ib!=eb; ++ib )
        if( worklabels.find( labels.at( ib ) ) != end )
          bck[ labels.position3d( i, ib ) ];

    return workbucket;
  }


  template <>
  BucketMap<Void>* labelBucket( const SparseVolume<BucketMap<Void> > & labels,
                                const set<int16_t> & worklabels )
  {
    BucketMap<Void> * workbucket = new BucketMap<Void>;
    BucketMap<Void>::Bucket & bck = (*workbucket)[0];
    set<int16_t>::const_iterator il, end = worklabels.end();
    BucketMap<Void>::const_iterator ib, eb = labels.data()->end();

    for( il=worklabels.begin(); il!=end; ++il )
    {
      ib = labels.data()->find( *il );
      if( ib != eb )
        bck.insert( ib->second.begin(), ib->second.end() );
    }

    return workbucket;
  }


  template <typename T>
  BucketMap<Void>* labelInterface( const SparseVolume<T> & labels,
    const set<int16_t> & worklabels, int16_t label, const Connectivity & c )
  {
    BucketMap<Void>* workbucket = new BucketMap<Void>;
    BucketMap<Void>::Bucket & bck = (*workbucket)[0];
    set<int16_t>::const_iterator end = worklabels.end();
    BucketMap<Void>::Bucket::const_iterator wend = bck.end();
    int i, n = c.nbNeighbors();

    typename SparseVolume<T>::const_iterator b, e = labels.end();
    typename SparseVolume<T>::const_LowLevelStorage::const_iterator ib, eb;
    typename StorageTrait<T>::VoxelType bg = labels.background();
    for( b=labels.begin(); b!=e; ++b )
      for( ib=b->second.begin(), eb=b->second.end(); ib!=eb; ++ib )
      {
        typename StorageTrait<T>::VoxelType l0 = labels.at( ib );
        if( l0 == label )
        {
          Point3d p0( labels.position3d( b, ib ) );
          for( i=0; i<n; ++i )
          {
            Point3d p( p0 + c.xyzOffset( i ) );
            typename StorageTrait<T>::VoxelType l = labels.checkedAt( p );
            if( l != labels.background() && worklabels.find( l ) != end )
            {
              bck[ p0 ] = Void();
              break;
            }
          }
        }
      }

    return workbucket;
  }


  // ---

  template <typename T>
  struct FastMarchingPrivateStruct
  {
    FastMarchingPrivateStruct( map<pair<int16_t, int16_t>,BucketMap<float> >
        & mid_interface ) : mid_interface_map( mid_interface ) {}
    SparseVolume<typename FastMarching<T>::FloatType> dist;
    SparseVolume<T> seed;
    SparseVolume<typename FastMarching<T>::FloatType> inv_speed;
    vector<float> voxel_size;
    int xm;
    int ym;
    int zm;
    multimap<float, Point3d> front;
    SparseVolume<T> status;
    bool mid_interface_option;
    map<pair<int16_t, int16_t>,BucketMap<float> > & mid_interface_map;
    bool verbose;
  };


  float solveQuadratic( float inv_speed, float u, float delta_ux,
                        float delta_uy, float delta_uz, float hx, float hy,
                        float hz )
  {
    float gx, gy, gz, u_tmp, a, b, c, delta;
    gx = delta_ux + hx * inv_speed;
    gy = delta_uy + hy * inv_speed;
    gz = delta_uz + hz * inv_speed;
    u_tmp = min(min(gx, gy), gz);
    a = 0;
    b = 0;
    c = -inv_speed*inv_speed;
    if(delta_ux < u)
    {
      a = (a + (1/(hx*hx)));
      b = (b + (delta_ux/(hx*hx)));
      c = (c + sqr(delta_ux/hx));
    }
    if(delta_uy < u)
    {
      a = (a + (1/(hy*hy)));
      b = (b + (delta_uy/(hy*hy)));
      c = (c + sqr(delta_uy/hy));
    }
    if(delta_uz < u)
    {
      a = (a + (1/(hz*hz)));
      b = (b + (delta_uz/(hz*hz)));
      c = (c + sqr(delta_uz/hz));
    }
    if(a > 0)
    {
      delta = (b*b - a*c);
      if(delta >= 0)
        u_tmp = (b + sqrt(delta))/a;
    }
    return u_tmp;
  }


  template <typename T>
  float update( int x, int y, int z, int seed_front,
                const FastMarchingPrivateStruct<T> & fps )
  {
    vector<float> delta_ux(2);
    vector<float> delta_uy(2);
    vector<float> delta_uz(2);
    int i, j, k;
    Point3d p( x, y, z );
    float u = fps.dist.at(p);
    float u_tmp;
    float inv_speed = fps.inv_speed.at( p );
    float hx = fps.voxel_size[0], hy= fps.voxel_size[1],
      hz = fps.voxel_size[2];
    if (x > 0 && fps.seed.at( x-1, y, z ) == seed_front)
      delta_ux[0] = fps.dist.at( x - 1, y, z );
    else delta_ux[0] = FLT_MAX;
    if (y > 0 && fps.seed.at( x, y - 1, z ) == seed_front)
      delta_uy[0] = fps.dist.at( x , y - 1, z );
    else delta_uy[0] = FLT_MAX;
    if (z > 0 && fps.seed.at( x, y , z- 1 ) == seed_front)
      delta_uz[0] = fps.dist.at( x , y , z- 1 );
    else delta_uz[0] = FLT_MAX;
    if (x + 1 < fps.xm  && fps.seed.at( x + 1, y , z ) == seed_front)
      delta_ux[1] = fps.dist.at( x + 1, y , z );
    else delta_ux[1] = FLT_MAX;
    if (y + 1 < fps.ym && fps.seed.at( x, y + 1, z ) == seed_front)
      delta_uy[1] = fps.dist.at( x , y + 1, z );
    else delta_uy[1] = FLT_MAX;
    if (z + 1 < fps.zm && fps.seed.at( x, y , z + 1 ) == seed_front)
      delta_uz[1] = fps.dist.at( x , y , z + 1 );
    else delta_uz[1] = FLT_MAX;
    for(i=0;i<2;++i)
      for(j=0;j<2;++j)
        for(k=0;k<2;++k)
          {
            u_tmp = solveQuadratic( inv_speed, u, delta_ux[i], delta_uy[j],
                                    delta_uz[k], hx, hy, hz );
            if (u_tmp < u)
              u = u_tmp;
          }
    return u;
  }
}


template <typename T>
typename FastMarching<T>::RCFloatType
FastMarching<T>::doit( const RCType & labels,
  int16_t worklabel, int16_t inlabel, int16_t outlabel )
{
  set<int16_t> worklabels;
  worklabels.insert(worklabel);
  set<int16_t> seeds;
  seeds.insert( inlabel );
  seeds.insert( outlabel );
  return doit( labels, worklabels, seeds );
}


namespace
{

  template <typename T>
  class fillInitialVoronoi
  {
  public:
    fillInitialVoronoi( const SparseVolume<T> & labels,
                        const set<int16_t> & seedLabels,
                        SparseVolume<T> & voronoi );
  };


  template <typename T>
  class fillInitialVoronoi<Volume<T> >
  {
  public:
    fillInitialVoronoi( const SparseVolume<Volume<T> > & labels,
                        const set<int16_t> & seedLabels,
                        SparseVolume<Volume<T> > & voronoi )
    {
      vector<int> sz = labels.getSize();
      int x, y, z, xm = sz[0], ym = sz[1], zm = sz[2];
      set<int16_t>::const_iterator end = seedLabels.end();
      int16_t label_tmp;
      for( z=0; z<zm; ++z )
        for( y=0; y<ym; ++y )
          for( x=0; x<xm; ++x )
          {
            label_tmp = labels.at( x, y, z );
            if( seedLabels.find(label_tmp) != end )
              voronoi.setValue( label_tmp, x, y, z );
          }
    }
  };



  template <typename T>
  class fillInitialVoronoi<BucketMap<T> >
  {
  public:
    fillInitialVoronoi( const SparseVolume<BucketMap<T> > & labels,
                        const set<int16_t> & seedLabels,
                        SparseVolume<BucketMap<T> > & voronoi )
    {
      set<int16_t>::const_iterator end = seedLabels.end();
      typename BucketMap<T>::Bucket & lb = labels.data()->begin()->second;
      typename BucketMap<T>::Bucket::const_iterator ip, ep = lb.end();
      typename BucketMap<T>::Bucket & vb = (*voronoi.data())[ 0 ];
      int16_t label_tmp;
      for( ip=lb.begin(); ip!=ep; ++ip )
      {
        label_tmp = ip->second;
        if( seedLabels.find(label_tmp) != end )
          vb[ ip->first ] = label_tmp;
      }
    }
  };


  template<>
  fillInitialVoronoi<BucketMap<Void> >::fillInitialVoronoi
  ( const SparseVolume<BucketMap<Void> > & labels,
    const set<int16_t> & seedLabels,
    SparseVolume<BucketMap<Void> > & voronoi )
  {
    set<int16_t>::const_iterator i, end = seedLabels.end();
    BucketMap<Void>::const_iterator ib, eb = labels.data()->end();
    BucketMap<Void>::Bucket::const_iterator ip, ep;
    int16_t label_tmp;
    for( i=seedLabels.begin(); i!=end; ++i )
    {
      label_tmp = *i;
      ib = labels.data()->find( label_tmp );
      if( ib != eb )
      {
        const BucketMap<Void>::Bucket & lb = ib->second;
        BucketMap<Void>::Bucket & vb = (*voronoi.data())[ label_tmp ];
        for( ip=lb.begin(), ep=lb.end(); ip!=ep; ++ip )
          vb[ ip->first ] = label_tmp;
      }
    }
  }

}


namespace
{
  template <typename T> typename FastMarching<T>::RCFloatType
  doit_private( FastMarchingPrivateStruct<T> & fps );
}


template <typename T>
typename FastMarching<T>::RCFloatType
FastMarching<T>::doit( const RCType & labels,
                       const set<int16_t> & worklabels,
                       const set<int16_t> & seeds )
{
  if( _verbose )
    cout << "FastMarching...\n";
  FastMarchingPrivateStruct<T> fps( _mid_interface_map );
  SparseVolume<T> slabels( labels );
  vector<int> sz = slabels.getSize();
  slabels.setBackground( -1 );
  fps.xm = sz[0];
  fps.ym = sz[1];
  fps.zm = sz[2];
  fps.voxel_size = slabels.voxelSize();
  fps.mid_interface_option = _mid_interface_option;
  int default_seed = -1;
  fps.verbose = _verbose;

  fps.dist = SparseVolume<FloatType>::alloc( slabels );
  SparseVolume<T> status( SparseVolume<T>::alloc( slabels ) );
  fps.seed = SparseVolume<T>::alloc( slabels );
  fps.inv_speed = SparseVolume<FloatType>::alloc( slabels );
  multimap<float, Point3d> & front = fps.front;
  fps.status = status;

  enum Status{
    far,
    near,
    active
  };

  status.fill( far );
  fps.dist.fill( FLT_MAX );
  fps.seed.fill( default_seed );
  fps.inv_speed.fill( FLT_MAX );
  typename SparseVolume<T>::iterator ist, est = slabels.end();
  typename SparseVolume<T>::LowLevelStorage::iterator iv, ev;
  set<int16_t>::const_iterator is, es = seeds.end(), iw, ew = worklabels.end();
  Connectivity c( 0, 0, _connectivity );
  int i, n = c.nbNeighbors();
  long long nwork = 0, ninter = 0;

  for( ist=slabels.begin(); ist!=est; ++ist )
    for( iv=ist->second.begin(), ev=ist->second.end(); iv!=ev; ++iv )
    {
      int16_t val = slabels.at( iv );
      Point3d pos = slabels.position3d( ist, iv );

      if( worklabels.find( val ) != ew )
      {
        fps.status.setValue( far, pos );
        fps.seed.setValue( default_seed, pos );
        fps.dist.setValue( FLT_MAX, pos );
        fps.inv_speed.setValue( 1, pos );
        ++nwork;
      }
      else
      {
        if( seeds.find( val ) != es )
        {
          bool inter = false;
          for( i=0; i<n; ++i )
          {
            Point3d p( pos + c.xyzOffset( i ) );
            typename StorageTrait<T>::VoxelType l = slabels.checkedAt( p );
            if( l != slabels.background() && worklabels.find( l ) != ew )
            {
              inter = true;
              break;
            }
          }
          if( inter ) // interface
          {
            fps.dist.setValue( 0, pos );
            fps.seed.setValue( val, pos );
            fps.status.setValue( near, pos );
            fps.inv_speed.setValue( 1, pos );
            front.insert( pair<float,Point3d>( 0, pos ) );
            ++ninter;
          }
          /* else // inside seed
          {
            fps.status.setValue( far, pos );
            fps.inv_speed.setValue( FLT_MAX, pos );
          } */
        }
      }
    }

  if( _verbose )
    cout << "work voxels: " << nwork << ", interface voxels: " << ninter
      << endl;

  doit_private( fps );
  _voronoi = fps.seed;
  fillInitialVoronoi<T>( slabels, seeds, _voronoi );
  return fps.dist.data();
}


namespace
{

  template <typename T> typename FastMarching<T>::RCFloatType
  doit_private( FastMarchingPrivateStruct<T> & fps )
  {
    enum Status{
      far,
      near,
      active
    };

  multimap<float, Point3d> & front = fps.front;
  SparseVolume<T> & status = fps.status;
  Connectivity c( 0, 0, Connectivity::CONNECTIVITY_6_XYZ );
  int n = c.nbNeighbors(),i;
  int x, y, z;
  long long iter = 0;
  while(!front.empty())
  {
    multimap<float,Point3d>::iterator itmap = front.begin();
    float dist_min = itmap->first;
    Point3d pos_min = itmap->second;
    if( fps.verbose && iter % 10000 == 0 )
    {
      cout << "iter: " << iter << ", front size: " << front.size()
          << ", point: " << pos_min << ", dist: " << dist_min << endl;
    }
    ++iter;
    float u;
    int16_t seed_front = fps.seed.at( pos_min );
    front.erase(itmap);
    status.setValue( active, pos_min );

    bool mid = false;
    float dist_neighbour = FLT_MAX;
    int16_t seed_neighbour = 0;
    float dist_tmp;

    for(i = 0;i < n; ++i)
    {
      Point3d offset = c.xyzOffset( i );
      Point3d p = pos_min + offset;
      x = p[0];
      y = p[1];
      z = p[2];
      if( fps.inv_speed.checkedAt( p ) != FLT_MAX )
      {
        if( status.at( p ) != active)
        {
          u = update(x, y, z, seed_front, fps);
          float dist = fps.dist.at( p );
          if ( u < dist )
          {
            pair<multimap<float,Point3d>::iterator,
              multimap<float,Point3d>::iterator> itpair
              = front.equal_range( dist );
            fps.dist.setValue( u, p );
            fps.seed.setValue( seed_front, p );
            for(itmap =itpair.first;itmap != itpair.second; ++itmap)
              if(itmap->second == p)
              {
                front.erase(itmap);
                break;
              }
            front.insert( pair<float,Point3d>(u,p) );
          }
        }
        else
        {
          typename StorageTrait<T>::VoxelType seed = fps.seed.at( p );
          if( fps.mid_interface_option && seed != seed_front )
          {
            dist_tmp = sqrt(sqr(offset[0]*fps.voxel_size[0])
              +sqr(offset[1]*fps.voxel_size[1])
              +sqr(offset[2]*fps.voxel_size[2])) + fps.dist.at( p );
            if (dist_tmp < dist_neighbour)
            {
              dist_neighbour = dist_tmp;
              seed_neighbour = seed;
              mid = true;
            }
          }
        }
      }
    }
    if (mid)
    {
      pair<int16_t,int16_t> mid_key( min(seed_front,seed_neighbour),
                                     max(seed_front,seed_neighbour) );
      fps.mid_interface_map[mid_key][0][pos_min] = dist_min + dist_neighbour;
    }
  }
  map<pair<int16_t,int16_t>,BucketMap<float> >::iterator
      im, em = fps.mid_interface_map.end();
  for( im=fps.mid_interface_map.begin(); im!=em; ++im )
    im->second.setSizeXYZT( fps.voxel_size[0], fps.voxel_size[1],
                            fps.voxel_size[2], 1. );
  return fps.dist.data();
}

}


template <typename T>
const BucketMap<float> & FastMarching<T>::midInterface (int16_t label1, int16_t label2) const
{
  pair<int16_t,int16_t> mid_key (min(label1,label2),max(label1,label2));
  map<pair<int16_t,int16_t>,BucketMap<float> >::const_iterator
      mid_interface_it = _mid_interface_map.find(mid_key);
  if (mid_interface_it == _mid_interface_map.end())
    throw runtime_error("interface does not exist");
  return mid_interface_it->second;
}


template <typename T>
VolumeRef<float>
FastMarching<T>::midInterfaceVol (int16_t label1, int16_t label2) const
{
  vector<int> sz = _voronoi.getSize();
  VolumeRef<float> mid_interface_vol( sz[0], sz[1], sz[2] );
  mid_interface_vol->fill( -1 );
  const BucketMap<float> & mid_interface_buck = midInterface(label1, label2);
  Converter<BucketMap<float>, VolumeRef<float> > conv;
  conv.convert(mid_interface_buck, mid_interface_vol);
  vector<float> voxel_size = _voronoi.voxelSize();
  mid_interface_vol->header().setProperty("voxel_size",voxel_size );
  return mid_interface_vol;
}

template <typename T>
vector<pair<int16_t,int16_t> > FastMarching<T>::midInterfaceLabels () const
{
  vector<pair<int16_t,int16_t> > mid_interface_labels;
  map<pair<int16_t,int16_t>,BucketMap<float> >::const_iterator
    mid_interface_it, mid_interface_et;
  for (mid_interface_it = _mid_interface_map.begin(),
    mid_interface_et = _mid_interface_map.end();
    mid_interface_it != mid_interface_et; ++ mid_interface_it)
  {
    mid_interface_labels.push_back(mid_interface_it->first);
  }
  return mid_interface_labels;
}

template <typename T>
typename FastMarching<T>::RCType FastMarching<T>::voronoiVol() const
{
  return _voronoi.data();
}


template class FastMarching<Volume<int16_t> >;
template class FastMarching<BucketMap<int16_t> >;
template class FastMarching<BucketMap<float> >;
