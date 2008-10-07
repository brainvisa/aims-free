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


#include <aims/distancemap/mask.h>
#include <aims/data/data.h>
#include <aims/bucket/bucket.h>
#include <aims/math/mathelem.h>
#include <cartobase/config/verbose.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;

#define AIMS_OUTSIDE 32501
#define AIMS_DOMAIN  32500

namespace
{

  template <class T>
  void AimsDistancePreparation( AimsData<T>& vol, T val_domain, T val_outside)
  {
    int x, y, z;

    ForEach3d( vol, x, y, z )
      if ( vol( x, y, z ) == val_outside )
        vol( x, y, z ) = AIMS_OUTSIDE;
      else if ( vol( x, y, z ) == val_domain )
        vol( x, y, z ) = AIMS_DOMAIN;
      else
        vol( x, y, z ) = 0;
  }


  template <typename T>
  void AimsDistancePreparation( BucketMap<T>& bck, T val_domain, T val_outside)
  {
    typename BucketMap<T>::iterator		i, e = bck.end();
    typename BucketMap<T>::Bucket::iterator	ib, eb;
    // debug
    unsigned n = 0;
    for( i=bck.begin(); i!=e; ++i )
      {
        typename BucketMap<T>::Bucket	& b = i->second;
        for( ib=b.begin(), eb=b.end(); ib!=eb; ++ib )
          if( ib->second == val_outside )
            ib->second = AIMS_OUTSIDE;
          else if( ib->second == val_domain )
            ib->second = AIMS_DOMAIN;
          else
            {
            ib->second = 0;
            ++n;
            }
      }
  }


  inline
  bool inside( Point3d& pt, int dimX, int dimY, int dimZ )
  {
    if ( pt.item(0) < 0     ||
         pt.item(0) >= dimX ||
         pt.item(1) < 0     ||
         pt.item(1) >= dimY ||
         pt.item(2) < 0     ||
         pt.item(2) >= dimZ   )
      return false;
    return true;
  }


  template <class T>
  void 
  FillDistanceBucket( AimsBucket<Void>& bucket, AimsData<T>& vol,
                      const ChamferMask& mask )
  {
    int x=0, y=0, z=0, n=0;
    int dimX = vol.dimX();
    int dimY = vol.dimY();
    int dimZ = vol.dimZ();
    Point3d neigh;
    AimsBucketItem<Void> item;
    bool back6neigh=false;
    int	l = mask.length();
    if( l > 26 )
      l = 26; // 1 voxel away is always enough

    for ( z = 0; z < dimZ; z++ )
      for ( y = 0; y < dimY; y++ )
        for ( x = 0; x < dimX; x++ )
          {
            if ( vol( x, y, z ) == 0 )
              {
                back6neigh = false;
                for ( n = 0; n < l; ++n )
                  {
                    neigh[0] = x + mask.offset( n ).item(0);
                    neigh[1] = y + mask.offset( n ).item(1);
                    neigh[2] = z + mask.offset( n ).item(2);
                    if ( inside( neigh, dimX, dimY, dimZ ) == true )
                      {
                        if ( vol( neigh ) && vol( neigh ) == AIMS_DOMAIN )
                          {
                            back6neigh = true;
                            break;
                          }
                      }
                  }
                if ( back6neigh )
                  {
                    item.location().item(0) = x;
                    item.location().item(1) = y;
                    item.location().item(2) = z;
                    bucket[ 0 ].push_back( item );
                  }
              }
          }
  }


  template <typename T>
  void 
  FillDistanceBucket( AimsBucket<Void> & bucket, 
                      map<Point3d, T, BucketMapLess> & bck, 
                      const ChamferMask& mask )
  {
    Point3d neigh;
    AimsBucketItem<Void> item;
    list<AimsBucketItem<Void> >	& obk = bucket[0];
    bool back6neigh=false;
    typename BucketMap<T>::Bucket::iterator	ib, eb = bck.end(), jb;
    int	n, l = mask.length();
    if( l > 26 )
      l = 26; // 1 voxel away is always enough

    for( ib=bck.begin(); ib!=eb; ++ib )
      if( ib->second == 0 )
        {
          back6neigh = false;
          for( n = 0; n < l; ++n )
            {
              neigh = ib->first + mask.offset( n );
              jb = bck.find( neigh );
              if( jb != eb && jb->second == AIMS_DOMAIN )
                {
                  back6neigh = true;
                  break;
                }
            }
          if( back6neigh )
            {
              item.location() = ib->first;
              obk.push_back( item );
            }
        }
  }


  template <class T>
  void 
  EmptyDistanceBucket( AimsBucket<Void>& bucket, AimsData<T>& vol,
                       const ChamferMask& mask, int limit, float mult_factor )
  {

    list< AimsBucketItem<Void> >::const_iterator it,ite;
    list< AimsBucketItem<Void> >* ptr=NULL;
    AimsBucketItem<Void> item;
    int dimX = vol.dimX();
    int dimY = vol.dimY();
    int dimZ = vol.dimZ();
    int dist,n,d;
    Point3d center,neigh;
    T new_val;
    int last_dist=0;

    AimsData< list< AimsBucketItem<Void> >* > lstPtr( limit );
    lstPtr = NULL;
    for ( AimsBucket<Void>::iterator lst  = bucket.begin();
          lst != bucket.end(); lst++ )
      lstPtr( (*lst).first ) = &( (*lst).second );
  

    if( verbose )
      cout << "distance : " << setw(10) << setprecision(8) << 0 << flush;
    for ( dist = 0; dist < limit; dist++ )
      {
        ptr = lstPtr( dist );
        if ( ptr )
          {
            last_dist = dist;
            if( verbose && dist % 100 == 0 )
              cout <<  "\b\b\b\b\b\b\b\b\b\b" << setw(10) << setprecision(8) 
                   << float( dist ) / mult_factor << flush;

            for ( it  = ptr->begin(),
                    ite = ptr->end(); it != ite; it++ )
              {
                center = it->location();
                if ( vol( center ) >= T( dist )  )
                  for ( n = 0; n < mask.length(); n++ )
                    {
                      neigh.item(0) = center.item(0) 
                        + mask.offset( n ).item(0);
                      neigh.item(1) = center.item(1) 
                        + mask.offset( n ).item(1);
                      neigh.item(2) = center.item(2) 
                        + mask.offset( n ).item(2);
                      if ( inside( neigh, dimX, dimY, dimZ ) && 
                           vol( neigh ) > dist               &&
                           vol( neigh ) != AIMS_OUTSIDE        )
                        {
                          new_val = vol( center ) + mask.distance( n );
                          if ( new_val < vol( neigh ) )
                            {
                              vol( neigh ) = new_val;
                              item.location() = neigh;
                              bucket[ ( d = dist + mask.distance( n ) ) ]
                                .push_back( item );
                              if ( lstPtr( d ) == NULL )
                                lstPtr( d ) = &bucket[ d ];
                            }
                        }
                    }
              }
          }
      }
    if( verbose )
      cout <<  "\b\b\b\b\b\b\b\b\b\b" << setw(10) << setprecision(8) 
           << float( last_dist ) / mult_factor << endl;
  }


  template <typename T>
  void 
  EmptyDistanceBucket( AimsBucket<Void>& bucket, 
                       map<Point3d, T, BucketMapLess> & bck,
                       const ChamferMask& mask, int limit, float mult_factor )
  {
    //bool verbose = 0;
    list< AimsBucketItem<Void> >::const_iterator it,ite;
    list< AimsBucketItem<Void> >* ptr=NULL;
    AimsBucketItem<Void> item;
    typename BucketMap<T>::Bucket::iterator	ib, jb, eb = bck.end();
    int dist,n,d;
    Point3d center,neigh;
    T new_val;
    int last_dist=0;

    vector< list< AimsBucketItem<Void> >* > lstPtr( limit );
    for ( AimsBucket<Void>::iterator lst  = bucket.begin();
          lst != bucket.end(); lst++ )
      lstPtr[ (*lst).first ] = &( (*lst).second );

    if( verbose )
      cout << "distance : " << setw(10) << setprecision(8) << 0 << flush;
    for ( dist = 0; dist < limit; dist++ )
      {
        ptr = lstPtr[ dist ];
        if( ptr )
          {
            last_dist = dist;
            if( verbose && dist % 100 == 0 )
              cout <<  "\b\b\b\b\b\b\b\b\b\b" << setw(10) << setprecision(8) 
                   << float( dist ) / mult_factor << flush;

            for( it  = ptr->begin(),
                   ite = ptr->end(); it != ite; ++it )
              {
                center = it->location();
                ib = bck.find( center );
                if( ib->second >= T( dist )  )
                  for ( n = 0; n < mask.length(); n++ )
                    {
                      neigh = center + mask.offset( n );
                      jb = bck.find( neigh );
                      if( jb != eb && jb->second > dist 
                          && jb->second != AIMS_OUTSIDE )
                        {
                          new_val = ib->second + mask.distance( n );
                          if( new_val < jb->second )
                            {
                              jb->second = new_val;
                              item.location() = neigh;
                              bucket[ ( d = dist + mask.distance( n ) ) ]
                                .push_back( item );
                              if( lstPtr[ d ] == NULL )
                                lstPtr[ d ] = &bucket[ d ];
                            }
                        }
                    }
              }
          }
      }
    if( verbose )
      cout <<  "\b\b\b\b\b\b\b\b\b\b" << setw(10) << setprecision(8) 
           << float( last_dist ) / mult_factor << endl;
  }

} // namespace {}


template <typename T>
void 
AimsDistanceFrontPropagation( AimsData<T>& vol, T val_domain, T val_outside,
                              int xm, int ym, int zm, float mult_factor,
                              bool divide )
{
  ChamferMask mask;
  AimsBucket<Void> bucket;
  int limit;

  limit = int( mult_factor * sqrt( sqr( vol.dimX() * vol.sizeX() ) +
                                    sqr( vol.dimY() * vol.sizeY() ) +
                                    sqr( vol.dimZ() * vol.sizeZ() )  ) + 0.5 );
  mask.set( xm, ym, zm, vol.sizeX(), vol.sizeY(), vol.sizeZ(), mult_factor );

  AimsDistancePreparation( vol, val_domain, val_outside );

  FillDistanceBucket( bucket, vol, mask );
  EmptyDistanceBucket( bucket, vol, mask, limit, mult_factor );

  if ( divide == true )
  {
    int x, y, z;
    ForEach3d( vol, x, y, z )
      vol( x, y, z ) /= mult_factor;
  }
}


template <typename T> void 
AimsDistanceFrontPropagation( BucketMap<T> & dist, T val_domain, 
                              T val_outside, 
                              int xm, int ym, int zm, float mult_factor,
                              bool divide )
{
  ChamferMask mask;
  AimsBucket<Void> bucket;
  int limit = 0;
  typename BucketMap<T>::Bucket			& dbk = dist.begin()->second;
  typename BucketMap<T>::Bucket::iterator	ib, eb = dbk.end();

  float	x;
  Point3df	vs( dist.sizeX(), dist.sizeY(), dist.sizeZ() );
  for( ib=dbk.begin(); ib!=eb; ++ib )
    {
      x = mult_factor * sqrt( sqr( ib->first[0] * vs[0] ) 
                              + sqr( ib->first[1] * vs[1] ) 
                              + sqr( ib->first[2] * vs[2] ) ) + 0.5;
      if( x > limit )
        limit = (int) x;
    }
  mask.set( xm, ym, zm, dist.sizeX(), dist.sizeY(), dist.sizeZ(), 
            mult_factor );

  AimsDistancePreparation( dist, val_domain, val_outside );

  FillDistanceBucket( bucket, dbk, mask );
  EmptyDistanceBucket( bucket, dbk, mask, limit, mult_factor );

  if ( divide == true )
  {
    typename BucketMap<T>::Bucket::iterator	ib, eb = dbk.end();
    for( ib=dbk.begin(); ib!=eb; ++ib )
      ib->second /= mult_factor;
  }
}




template 
void 
AimsDistanceFrontPropagation( AimsData<short>& vol, short val_domain,
                              short val_outside,
                              int xm, int ym, int zm, float mult_factor,
                              bool divide );

template 
void 
AimsDistanceFrontPropagation( AimsData<float>& vol, float val_domain,
                              float val_outside,
                              int xm, int ym, int zm, float mult_factor,
                              bool divide );

template 
void 
AimsDistanceFrontPropagation( BucketMap<short>& bck, short val_domain,
                              short val_outside,
                              int xm, int ym, int zm, float mult_factor,
                              bool divide );
template 
void 
AimsDistanceFrontPropagation( BucketMap<float>& bck, float val_domain,
                              float val_outside,
                              int xm, int ym, int zm, float mult_factor,
                              bool divide );

// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>

template class carto::Volume< list< AimsBucketItem<Void> >* >;

