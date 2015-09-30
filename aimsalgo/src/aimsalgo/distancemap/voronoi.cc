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


#include <iomanip>
#include <limits>
#include <cartobase/config/verbose.h>
#include <aims/data/data.h>
#include <aims/bucket/bucket.h>
#include <aims/distancemap/mask.h>
#include <aims/math/mathelem.h>

using namespace aims;
using namespace std;

#define AIMS_OUTSIDE(T) std::numeric_limits<T>::max()
#define AIMS_DOMAIN(T) std::numeric_limits<T>::max() - 1

template <class T>
void AimsVoronoiPreparation( AimsData<T>& vol,
                             T val_domain, T val_outside)
{
  int x, y, z;

  ForEach3d( vol, x, y, z )
    if ( vol( x, y, z ) == val_outside )
      vol( x, y, z ) = AIMS_OUTSIDE(T);
    else if ( vol( x, y, z ) == val_domain )
      vol( x, y, z ) = AIMS_DOMAIN(T);
    else
      vol( x, y, z ) = 0;
}

template <class T>
void AimsRestoreVol( AimsData<T>& vol, T val_domain, T val_outside)
{
  int x, y, z;

  ForEach3d( vol, x, y, z )
    if ( vol( x, y, z ) == AIMS_OUTSIDE(T) )
      vol( x, y, z ) = val_outside;
    else if ( vol( x, y, z ) == AIMS_DOMAIN(T) )
      vol( x, y, z ) = val_domain;
    else
      vol( x, y, z ) = 0;
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
FillVoronoiBucket( AimsBucket<Void>& bucket, AimsData<T>& vol,
                   AimsData<T>& label,
                   const ChamferMask& mask )
{
  int x=0, y=0, z=0, n=0;
  int dimX = vol.dimX();
  int dimY = vol.dimY();
  int dimZ = vol.dimZ();
  Point3d neigh;
  AimsBucketItem<Void> item;
  T new_val;

  for ( z = 0; z < dimZ; z++ )
    for ( y = 0; y < dimY; y++ )
      for ( x = 0; x < dimX; x++ )
      {
        if ( vol( x, y, z ) == 0 )
        {
          for ( n = 0; n < mask.length(); n++ )
          {
            neigh[0] = x + mask.offset( n ).item(0);
            neigh[1] = y + mask.offset( n ).item(1);
            neigh[2] = z + mask.offset( n ).item(2);
            if ( inside( neigh, dimX, dimY, dimZ ) == true )
            {
              if ( vol( neigh ) && vol( neigh ) != AIMS_OUTSIDE(T) )
              {
                new_val = mask.distance( n );
                if ( new_val < vol( neigh ) )
                {
                  item.location() = neigh;
                  vol( neigh ) = new_val;
                  label( neigh ) = label( x, y, z );
                  bucket[ new_val ].push_back( item );
                }
              }
            }
          }
        }
      }
}

template <class T>
void
EmptyVoronoiBucket( AimsBucket<Void>& bucket, AimsData<T>& vol,
                    AimsData<T>& label,
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


  cout << "distance : " << setw(12) << setprecision(8) << 0 << flush;
  for ( dist = 1; dist < limit; dist++ )
  {
    ptr = lstPtr( dist );
    if ( ptr )
    {
      last_dist = dist;
      if ( dist % 100 == 0 )
        cout <<  "\b\b\b\b\b\b\b\b\b\b\b\b" << setw(12) << setprecision(8)
             << float( dist ) / mult_factor << flush;
      for ( it  = ptr->begin(),
            ite = ptr->end(); it != ite; it++ )
      {
        center = it->location();

        // Make this warning non-fatal as it is probably harmless and the fix
        // is not obvious
        #pragma GCC diagnostic push
        #pragma GCC diagnostic warning "-Wsign-compare"
        if ( vol( center ) >= dist )
          for ( n = 0; n < mask.length(); n++ )
          {
            neigh.item(0) = center.item(0) + mask.offset( n ).item(0);
            neigh.item(1) = center.item(1) + mask.offset( n ).item(1);
            neigh.item(2) = center.item(2) + mask.offset( n ).item(2);
            if ( inside( neigh, dimX, dimY, dimZ ) &&
                 vol( neigh ) > dist               &&
                 vol( neigh ) != AIMS_OUTSIDE(T)         )
            {
              new_val = vol( center ) + mask.distance( n );
              if ( new_val < vol( neigh ) )
              {
                vol( neigh ) = new_val;
                label( neigh ) = label( center );
                item.location() = neigh;
                bucket[ ( d = dist + mask.distance( n ) ) ].push_back( item );
                if ( lstPtr( d ) == NULL )
                  lstPtr( d ) = &bucket[ d ];

              }
            }
          }
        #pragma GCC diagnostic pop
      }
    }
  }
  cout <<  "\b\b\b\b\b\b\b\b\b\b\b\b" << setw(12) << setprecision(8)
       << float( last_dist ) / mult_factor << endl;
}


template <class T>
AimsData<T>
AimsVoronoiFrontPropagation( AimsData<T>& vol,
                             T val_domain, T val_outside,
                             int xm, int ym, int zm, float mult_factor )
{
  ChamferMask mask;
  AimsBucket<Void> bucket;
  int limit;

  limit = int( mult_factor * sqrt( sqr( vol.dimX() * vol.sizeX() ) +
                                   sqr( vol.dimY() * vol.sizeY() ) +
                                   sqr( vol.dimZ() * vol.sizeZ() )  ) + 0.5 );

  if (carto::verbose)
    std::cout << "limited distance to process voronoi is: "
              << carto::toString(limit) << std::endl;

  mask.set( xm, ym, zm, vol.sizeX(), vol.sizeY(), vol.sizeZ(), mult_factor );

  AimsData<T> label = vol.clone();

  AimsVoronoiPreparation( vol, val_domain, val_outside );

  FillVoronoiBucket( bucket, vol, label, mask );

  EmptyVoronoiBucket( bucket, vol, label, mask, limit,
                      mult_factor );

  AimsRestoreVol( vol, val_domain, val_outside );

  return label;
}


// Instanciations
template
AimsData<uint8_t>
AimsVoronoiFrontPropagation<uint8_t>( AimsData<uint8_t>& vol,
                                      uint8_t val_domain, uint8_t val_outside,
                                       int xm, int ym, int zm, float mult_factor );
template
AimsData<int8_t>
AimsVoronoiFrontPropagation<int8_t>( AimsData<int8_t>& vol,
                                     int8_t val_domain, int8_t val_outside,
                                     int xm, int ym, int zm, float mult_factor );

template
AimsData<uint16_t>
AimsVoronoiFrontPropagation<uint16_t>( AimsData<uint16_t>& vol,
                                       uint16_t val_domain, uint16_t val_outside,
                                       int xm, int ym, int zm, float mult_factor );
template
AimsData<int16_t>
AimsVoronoiFrontPropagation<int16_t>( AimsData<int16_t>& vol,
                                      int16_t val_domain, int16_t val_outside,
                                      int xm, int ym, int zm, float mult_factor );

template
AimsData<uint32_t>
AimsVoronoiFrontPropagation<uint32_t>( AimsData<uint32_t>& vol,
                                       uint32_t val_domain, uint32_t val_outside,
                                       int xm, int ym, int zm, float mult_factor );

template
AimsData<int32_t>
AimsVoronoiFrontPropagation<int32_t>( AimsData<int32_t>& vol,
                                      int32_t val_domain, int32_t val_outside,
                                      int xm, int ym, int zm, float mult_factor );
