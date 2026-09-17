
#include <cstdlib>
#include <aims/math/mathelem.h>
#include <aims/math/tqli.h>
#include <cartodata/volume/volume.h>

using namespace carto;

template < class T >
void DecompositionTQLI< T >::doit( VolumeRef< T > d,
                                   VolumeRef< T > e,
                                   VolumeRef< T > z )
{
  ASSERT( z.getSizeZ() == 1 && z.getSizeT() == 1 );
  ASSERT( d.getSizeY() == 1 && d.getSizeZ() == 1 && d.getSizeT() == 1 );
  ASSERT( e.getSizeY() == 1 && e.getSizeZ() == 1 && e.getSizeT() == 1 );

  int m,l,iter,i,k,n;
  T s,r,p,g,f,dd,c,b;

  n = z.getSizeX();

  ASSERT( n == z.getSizeY() );

  for ( i = 1; i < n; i++ )
    e( i - 1 ) = e( i );
  e( n - 1 ) = 0;
  for ( l = 0; l < n; l++ )
  {
    iter = 0;
    do
    {
      for ( m = l; m < n - 1; m++ )
      {
        dd = fabs( d( m ) ) + fabs( d( m + 1 ) );
        if ( fabs( e( m ) ) + dd == dd)
          break;
      }
      if ( m != l )
      {
        ASSERT( iter++ != 30 );
        g = ( d( l + 1 ) - d( l ) ) / ( 2.0 * e( l ) );
        r = pythagore( g, (T)1.0 );
        g = d( m ) - d( l ) + e( l ) /
            ( g + ( g >= 0 ? fabs( r ) : -fabs( r ) ) );
        s = c = 1.0;
        p = 0.0;
        for ( i = m - 1; i >= l; i-- )
        {
          f = s * e( i );
          b = c * e( i );
          e( i + 1 ) = ( r = pythagore( f, g ) );
          if ( r == 0.0 )
          {
            d( i + 1 ) -= p;
            e( m ) = 0.0;
            break;
          }
          s = f / r;
          c = g / r;          
          g = d( i + 1 ) - p;
          r = ( d( i ) - g ) * s + 2.0 * c * b;
          d( i + 1 ) = g + ( p = s * r );
          g = c * r - b;
          for ( k = 0; k < n; k++ )
          {
            f = z( k, i + 1 );
            z( k, i + 1 ) = s * z( k, i ) + c * f;
            z( k, i ) = c * z( k, i ) - s * f;
          }
        }
        if ( ( r == 0.0 ) && ( i >= l ) )
          continue;
        d( l ) -= p;
        e( l )  = g;
        e( m )  = 0.0;
      }
    } while ( m != l );
  }
}


template void 
DecompositionTQLI< float >::doit( VolumeRef< float > d,
                                  VolumeRef< float > e,
                                  VolumeRef< float > z );


template void 
DecompositionTQLI< double >::doit( VolumeRef< double > d,
                                   VolumeRef< double > e,
                                   VolumeRef< double > z );
