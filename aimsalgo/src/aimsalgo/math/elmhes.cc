
#include <cstdlib>
#include <cartodata/volume/volume.h>
#include <aims/math/elmhes.h>

using namespace carto;

template < class T >
VolumeRef< T > HessenbergReduction< T >::doit( VolumeRef< T > a,
                                               VolumeRef< short > *isc )
{
  ASSERT( a.getSizeZ() == 1 && a.getSizeT() == 1 );
  ASSERT( a.getSizeX() == a.getSizeY() );

  int i, j, m;
  T x, y, dum;

  int n = a.getSizeX();

  VolumeRef< short > iscale( n, 1, 1, 1,
                             AllocatorContext(
                                &carto::MemoryAllocator::singleton() ) );

  for ( m=1; m<n-1; m++ )
    {
      x = (T)0;
      i = m;

      for ( j=m; j<n; j++ )
	{
	  if ( fabs( a( j, m - 1 ) ) > fabs( x ) )
	    {
	      x = a( j, m-1 );
	      i = j;
	    }
	}

      iscale( m ) = i;

      if ( i != m )
      {
	for ( j=m-1; j<n; j++ )
	  {
	    dum = a( i, j );
	    a( i, j ) = a( m, j );
	    a( m, j ) = dum;
	  }

	for ( j=0; j<n; j++ )
	  {
	    dum = a( j, i );
	    a( j, i ) = a( j, m );
	    a( j, m ) = dum;
	  }
      }

      if ( x )
	{
	  for ( i=m+1; i<n; i++ )
	    {
	      if( ( y = a( i, m - 1 ) ) )
		{
		  y /= x;
		  a(i, m - 1 ) = y;

		  for ( j=m; j<n; j++ )
		    a( i, j ) -= y * a( m, j );

		  for ( j=0; j<n; j++ )
		    a( j, m ) += y * a( j, i );
		}
	    }
	}
    }

  if ( isc )  *isc = iscale;

  return a;
}


template VolumeRef< float >
HessenbergReduction< float >::doit( VolumeRef< float > a,
				    VolumeRef< short > *isc );


template VolumeRef< double >
HessenbergReduction< double >::doit( VolumeRef< double > a,
				     VolumeRef< short > *isc );
