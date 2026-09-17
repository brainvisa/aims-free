
#include <cstdlib>
#include <aims/math/mathelem.h>
#include <aims/math/householder.h>
#include <cartodata/volume/volume.h>

using namespace carto;

template < class T >
void HouseholderTridiag< T >::doit( VolumeRef< T > a,
                                    VolumeRef< T > d,
                                    VolumeRef< T > e )
{
  ASSERT( a.getSizeZ() == 1 && a.getSizeT() == 1);
  ASSERT( d.getSizeY() == 1 && d.getSizeZ() == 1 && d.getSizeT() == 1 );
  ASSERT( e.getSizeY() == 1 && e.getSizeZ() == 1 && e.getSizeT() == 1 );

  int l,k,j,i,n;
  T scale,hh,h,g,f;

  n = a.getSizeX();

  for ( i = n - 1; i >= 1; i-- )
  {
    l = i - 1;
    h = 0;
    scale = 0;
    if ( l > 0 )
    {
      for ( k = 0; k <= l; k++ )
        scale += fabs( a( i, k ) );
      if ( scale == 0 ) e( i ) = a( i, l );
      else
      {
        for ( k = 0; k <= l; k++ )
        {
          a( i, k ) /= scale;
          h += square( a( i, k ) );
        }
        f = a( i, l );
        g = ( f >= 0 ) ? -sqrt( h ) : +sqrt( h );
        e( i ) = scale * g;
        h -= f * g;
        a( i, l )  = f - g;
        f = 0;
        for ( j = 0; j <= l; j++ )
        {
          a( j, i )  = a( i, j ) / h;
          g = 0;
          for ( k = 0; k <= j; k++ ) 
            g += a( j, k ) * a( i, k );
          for ( k = j + 1; k <= l; k++ )
            g += a( k, j ) * a( i, k );
          e( j ) = g / h;
          f += e( j ) * a( i, j );
        }
        hh = f / ( h + h );
        for ( j = 0; j <= l; j++ )
        {
          f = a( i, j );
          e( j ) = g = e( j ) - hh * f;
          for ( k = 0; k <= j; k++ )
            a( j, k ) -= f * e( k ) + g * a( i, k );
        }
      }
    }
    else
      e( i ) = a( i, l );
    d( i ) = h;
  }
  d( 0 ) = 0;
  e( 0 ) = 0;
  for ( i = 0; i < n; i++ )
  {
    l = i - 1;
    if ( d( i ) )
    {
      for ( j = 0; j <= l; j++ )
      {
        g = (T)0;
        for ( k = 0; k <= l; k++ )
          g += a( i, k ) * a( k, j );
        for ( k = 0; k <= l; k++ )
          a( k, j ) -= g * a( k, i );
      }
    }
    d( i ) = a( i, i );
    a( i, i ) = (T)1;
    for ( j = 0; j <= l; j++ )
      a( j, i ) = a( i, j ) = (T)0;
  }
}


template void 
HouseholderTridiag< float >::doit( VolumeRef< float > a,
                                   VolumeRef< float > d,
                                   VolumeRef< float > e );


template void 
HouseholderTridiag< double >::doit( VolumeRef< double > a,
                                    VolumeRef< double > d,
                                    VolumeRef< double > e );
