
#include <aims/math/balbak.h>
#include <cartodata/volume/volume.h>

using namespace carto;

template < class T >
VolumeRef< T > BackBalancing< T >::doit( VolumeRef< T > mat,
                                        const VolumeRef< T >& wr,
                                        const VolumeRef< T >& wi,
                                        const VolumeRef< T > *sc )
{
  ASSERT( mat.getSizeZ() == 1 && mat.getSizeT() == 1 );
  ASSERT( mat.getSizeX() == mat.getSizeY() );
  ASSERT( wr.getSizeY() == 1 && wr.getSizeZ() == 1 && wr.getSizeT() == 1 );
  ASSERT( wi.getSizeY() == 1 && wi.getSizeZ() == 1 && wi.getSizeT() == 1 );
  ASSERT( wr.getSizeX() == wi.getSizeX() && wr.getSizeX() == mat.getSizeX() );

  int i, j;
  T s, norm;

  int n = mat.getSizeX();

  VolumeRef< T > scale( n, 1, 1, 1,
                        AllocatorContext(
                          &carto::MemoryAllocator::singleton() ) );

  if ( sc ) scale = *sc;
  else
    for ( i=0; i<n; i++ )  scale( i ) = (T)1;

  ASSERT( scale.getSizeX() == n );

  for ( i=0; i<n; i++ )
    {
      s = scale( i );

      for ( j=0; j<n; j++ )
        mat( i, j ) *= s;
    }

  for ( j=0; j<n; j++ )
  {
    norm = (T)0;

    if ( wi( j ) == (T)0 )
    {
      for ( i=0; i<n; i++ )
        norm += mat( i, j ) * mat( i, j );

      norm = (T)sqrt( norm );

      for ( i=0; i<n; i++ )
        mat( i, j ) /= norm;
    }
      else if ( wi( j ) > (T)0 )
    {
      for ( i=0; i<n; i++ )
        norm += mat( i, j ) * mat( i, j ) + mat( i, j+1 ) * mat( i, j+1 );

      norm = (T)sqrt( norm );

      for ( i=0; i<n; i++ )
        {
          mat( i, j ) /= norm;
          mat( i, j + 1 ) /= norm;
        }
    }
  }

  return mat;
}


template VolumeRef< float >
BackBalancing< float >::doit( VolumeRef< float > mat,
                              const VolumeRef< float >& wr,
                              const VolumeRef< float >& wi,
                              const VolumeRef< float > *sc );

template VolumeRef< double >
BackBalancing< double >::doit( VolumeRef< double > mat,
                               const VolumeRef< double >& wr,
                               const VolumeRef< double >& wi,
                               const VolumeRef< double > *sc );
