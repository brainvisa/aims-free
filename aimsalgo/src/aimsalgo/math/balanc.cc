
#include <cstdlib>
#include <cartodata/volume/volume.h>
#include <aims/math/balanc.h>

using namespace aims;
using namespace carto;

template < class T >
Balancing< T >::Balancing() : radix( (T)16.0 )
{
}

template < class T >
VolumeRef< T > Balancing< T >::doit( VolumeRef< T > mat, VolumeRef< T > *sc )
{
  ASSERT( mat.getSizeZ() == 1 && mat.getSizeT() == 1 );
  ASSERT( mat.getSizeX() == mat.getSizeY() );

  int i, j, last;
  T s, r, g, f, c, sqrdx;

  int n = mat.getSizeX();

  VolumeRef< T > scale( n, 1, 1, 1,
                        AllocatorContext(
                          &carto::MemoryAllocator::singleton() ) );

  sqrdx = radix * radix;
  last = 0;

  for ( i=0; i<n; i++ )  scale( i ) = (T)1;

  while( last == 0 )
    {
      last = 1;

      for ( i=0; i<n; i++ )
	{
	  r = c = (T)0;

	  for ( j=0; j<n; j++ )
	    if ( j != i )
	      {
		c += (T)fabs( mat( j, i ) );
		r += (T)fabs( mat( i, j ) );
	      }

	  if ( c && r )
	    {
	      g = r / radix;
	      f = (T)1;
	      s = c + r;

	      while( c < g )
		{
		  f *= radix;
		  c *= sqrdx;
		}

	      g = r * radix;

	      while( c > g )
		{
		  f /= radix;
		  g /= sqrdx;
		}

	      if ( ( c + r ) / f < (T)( 0.95 * s ) )
		{
		  last = 0;
		  g = (T)1 / f;
		  scale( i ) *= f;

		  for ( j=0; j<n; j++ )
		    mat( i, j ) *= g;

		  for ( j=0; j<n; j++ )
		    mat( j, i ) *= f;
		}
	    }
	}
    }

  if ( sc )  *sc = scale;

  return mat;
}


template
Balancing< float >::Balancing();

template VolumeRef< float >
Balancing< float >::doit( VolumeRef< float > mat, VolumeRef< float > *sc );


template
Balancing< double >::Balancing();

template VolumeRef< double >
Balancing< double >::doit( VolumeRef< double > mat, VolumeRef< double > *sc );
