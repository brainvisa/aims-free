
#include <cartodata/volume/volume.h>
#include <aims/math/eltran.h>

using namespace carto;

template < class T >
VolumeRef< T > HessenbergAccumulation< T >::doit(
  const VolumeRef< T > & a, const VolumeRef< short > *isc )
{
  ASSERT( a.getSizeZ() == 1 && a.getSizeT() == 1 );
  ASSERT( a.getSizeX() == a.getSizeY() );

  int i, j, mp, mp1;

  int n = a.getSizeX();

  VolumeRef< T > zev( n, n, 1, 1,
                      AllocatorContext(
                        &carto::MemoryAllocator::singleton() ) );
  VolumeRef< short > iscale( n, 1, 1, 1,
                             AllocatorContext(
                                &carto::MemoryAllocator::singleton() ) );

  if ( isc )  iscale = *isc;
  else
    for ( i=0; i<n; i++ )  iscale( i ) = i;

  ASSERT( iscale.getSizeX() == n );

  for ( i=0; i<n; i++ )
    for ( j=0; j<n; j++ )
      {
	if ( i == j )  zev( i, j ) = (T)1;
	else zev( i, j ) = (T)0;
      }

  for ( mp=n-2; mp>0; mp-- )
    {
      mp1 = mp + 1;

      for ( i=mp1; i<n; i++ )
	zev( i, mp ) = a( i, mp - 1 );

      i = iscale( mp );

      if ( i == mp )  continue;

      for ( j=mp; j<n; j++ )
	{
	  zev( mp, j ) = zev( i, j );
	  zev( i, j ) = (T)0;
	}

      zev( i, mp ) = (T)1;
    }

  return zev;
}


template VolumeRef< float >
HessenbergAccumulation< float >::doit( const VolumeRef< float > & a,
                                       const VolumeRef< short > *isc );


template VolumeRef< double >
HessenbergAccumulation< double >::doit( const VolumeRef< double > & a,
                                        const VolumeRef< short > *isc );
