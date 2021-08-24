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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <aims/math/mathelem.h>
#include <aims/math/svd.h>
#include <cartodata/volume/volume.h>

using namespace std;
using namespace carto;
using namespace aims;

template< class T >
VolumeRef< T > AimsSVD< T >::doit( VolumeRef< T >& u, VolumeRef< T > *v )
{
  ASSERT( u.getSizeZ() == 1 && u.getSizeT() == 1 );
  VolumeRef< T > res;

  int i, j, k, l=0, its, jj, nm=0, flag;
  T c, f, h, s, x, y, z, maxarg, ftemp;

  T g = (T)0;
  T scale = (T)0;
  T anorm = (T)0;

  int n = u.getSizeY();
  int m = u.getSizeX();

  VolumeRef< T > w( n, 1, 1, 1, AllocatorContext::fast() );
  VolumeRef< T > vv( n, n, 1, 1, AllocatorContext::fast() );

  VolumeRef< T > rv1( n, 1, 1, 1, AllocatorContext::fast() );

  for ( i=0; i<n; i++ )
    {
      l = i + 1;
      rv1( i ) = scale * g;
      g = s = scale = (T)0;

      if ( i < m )
	{
	  for ( k=i; k<m; k++ )
	    scale += (T)fabs( u( k, i ) );

	  if ( scale )
	    {
	      for ( k=i; k<m; k++ )
		{
		  u( k, i ) /= scale;
		  s += square( u( k, i ) );
		}

	      f = u( i, i );
	      g = ( f >= (T)0 ) ? -(T)sqrt( s ) : +(T)sqrt( s );
	      h = f * g - s;
	      u( i, i ) = f - g;

	      for ( j=l; j<n; j++ )
		{
		  for ( s=(T)0, k=i; k<m; k++ )
		    s += u( k, i ) * u( k, j );

		  f = s / h;

		  for ( k=i; k<m; k++ )
		    u( k, j ) += f * u( k, i );
		}

	      for ( k=i; k<m; k++ )
		u( k, i ) *= scale;
	    }
	}

      w( i ) = scale * g;
      g = s = scale = (T)0;

      if ( i < m && i != n-1 )
	{
	  for ( k=l; k<n; k++ )
	    scale += (T)fabs( u( i, k ) );

	  if ( scale )
	    {
	      for ( k=l; k<n; k++ )
		{
		  u( i, k ) /= scale;
		  s += square( u( i, k ) );
		}

	      f = u( i, l );
	      g = ( f >= (T)0 ) ? -sqrt( s ) : +sqrt( s );
	      h = f * g - s;
	      u( i, l ) = f - g;

	      for ( k=l; k<n; k++ )
		rv1( k ) = u( i, k ) / h;

	      for ( j=l; j<m; j++ )
		{
		  for ( s=(T)0, k=l; k<n; k++ )
		    s += u( j, k ) * u( i, k );

		  for ( k=l; k<n; k++ )
		    u( j, k ) += s * rv1( k );
		}

	      for ( k=l; k<n; k++ )
		u( i, k ) *= scale;
	    }
	}

      maxarg = fabs( w( i ) ) + fabs( rv1( i ) );
      anorm = ( anorm > maxarg ) ? anorm : maxarg;
    }

  for ( i=n-1; i>=0; i-- )
    {
      if ( i < n-1 )
	{
	  if ( g )
	    {
	      for ( j=l; j<n; j++ )
		vv( j, i ) = ( u( i, j ) / u( i, l ) ) / g;

	      for ( j=l; j<n; j++ )
		{
		  for ( s=(T)0, k=l; k<n; k++ )
		    s += u( i, k ) * vv( k, j );

		  for ( k=l; k<n; k++ )
		    vv( k, j ) += s * vv( k, i );
		}
	    }

	  for ( j=l; j<n; j++ )
	    vv( i, j ) = vv( j, i ) = (T)0;
	}

      vv( i, i ) = (T)1;
      g = rv1( i );
      l = i;
    }

  for ( i=( m < n ) ? m-1 : n-1; i>=0; i-- )
    {
      l = i + 1;
      g = w( i );

      for ( j=l; j<n; j++ )
	u( i, j ) = (T)0;

      if ( g )
	{
	  g = (T)1 / g;

	  for ( j=l; j<n; j++ )
	    {
	      for ( s=(T)0, k=l; k<m; k++ )
		s += u( k, i ) * u( k, j );

	      f = ( s / u( i, i ) ) * g;

	      for ( k=i; k<m; k++ )
		u( k, j ) += f * u( k, i );
	    }

	  for ( j=i; j<m; j++ )
	    u( j, i ) *= g;
	}
      else
	for ( j=i; j<m; j++ )
	  u( j, i ) = (T)0;

      ++u( i, i );
    }

  for ( k=n-1; k>=0; k-- )
    {
      for ( its=1; its<=30; its++ )
        {
          flag = 1;

          for ( l=k; l>=0; l-- )
            {
              nm = l - 1;
              if ( ( (T)fabs( rv1( l ) ) + anorm ) == anorm || l == 0 )
                {
                  flag = 0;
                  break;
                }
	      // TEST ON nm ADDED
              if ( nm < 0 )
		cout << "nm1 =  " << nm << endl ;
	      if( ( (T)fabs( w( nm ) ) + anorm ) == anorm )  break;
            }
	  if( l < 0 )
	    throw runtime_error( "Bad l value" ) ;
	  // TEST ON l ADDED
          if ( flag )
            {
	      if( l == -1 ||  nm == -1 ){
		cout << "l or nm == -1 and flag is still on" << endl ;
		throw runtime_error( "Bad l or nm value" ) ;
	      }
	      if( nm == -1 )
		cout << "nm == -1 and flag is still on" << endl ;
	      c = (T)0;
              s = (T)1;

              for ( i=l; i<=k; i++ )
                {
                  f = s * rv1( i );
                  rv1( i ) = c * rv1( i );

                  if ( ( (T)fabs( f ) + anorm ) == anorm )  break;

                  g = w( i );
                  h = pythagore( f, g );
                  w( i ) = h;
                  h = (T)1 / h;
                  c = g * h;
                  s = -f * h;

                  for ( j=0; j<m; j++ )
                    {
                      y = u( j, nm );
                      z = u( j, i );
                      u( j, nm ) = y * c + z * s;
                      u( j, i ) = z * c - y * s;
                    }
                }
            }

          z = w( k );

          if ( l == k )
            {
              if ( z < (T)0 )
                {
                  w( k ) = -z;

                  for ( j=0; j<n; j++ )
                    vv( j, k ) = - vv( j, k );
                }

              break;
            }

          ASSERT( its != 60 );

          x = w( l );
          nm = k - 1;
	  if( nm < 0 ){
	    cerr << "k = 0 && nm = -1" << endl ;
	    throw runtime_error("nm < 0" ) ;
	  }
          y = w( nm );
          g = rv1( nm );
          h = rv1( k );
          f = ( ( y - z ) * ( y + z ) + ( g - h ) * ( g + h ) ) / 
              ( (T)2 * h * y );
          g = pythagore( f, (T)1 );
          ftemp = ( f >= (T)0 ) ? +(T)fabs( g ) : -(T)fabs( g );
          f = ( ( x - z ) * ( x + z ) + 
              h * ( ( y / ( f + ftemp ) ) - h ) ) / x;
          c = s = (T)1;

          for ( j=l; j<=nm; j++ )
            {
              i = j + 1;
              g = rv1( i );
              y = w( i );
              h = s * g;
              g = c * g;
              z = pythagore( f, h );
              rv1( j ) = z;
              c = f / z;
              s = h / z;
              f = x * c + g * s;
              g = g * c - x * s;
              h = y * s;
              y *= c;

              for ( jj=0; jj<n; jj++ )
                {
                  x = vv( jj, j );
                  z = vv( jj, i );
                  vv( jj, j ) = x * c + z * s;
                  vv( jj, i ) = z * c - x * s;
                }

              z = pythagore( f, h );
              w( j ) = z;

              if ( z )
                {
                  z = (T)1 / z;
                  c = f * z;
                  s = h * z; 
                }

              f = c * g + s * y;
              x = c * y - s * g;

              for ( jj=0; jj<m; jj++ )
                {
                  y = u( jj, j );
                  z = u( jj, i );
                  u( jj, j ) = y * c + z * s;
                  u( jj, i ) = z * c - y * s;
                }
            }

          rv1( l ) = (T)0;
          rv1( k ) = f;
          w( k ) = x;
        }
    }

  if ( v )  *v = vv;

  if ( retType == MatrixOfSingularValues )
    res = diag( w );
  else
    res = w;

  return res;
}


template VolumeRef< float >
AimsSVD< float >::doit( VolumeRef< float >& u, VolumeRef< float > *v );


template VolumeRef< double >
AimsSVD< double >::doit( VolumeRef< double >& u, VolumeRef< double > *v );


template< class T >
void AimsSVD< T >::sort( VolumeRef< T >& u, VolumeRef< T >& w,
                         VolumeRef< T > *v )
{
  int i, j, k;
  T p;
  VolumeRef< T > ww;

  ASSERT( u.getSizeZ() == 1 && u.getSizeT() == 1 );
  ASSERT( w.getSizeZ() == 1 && w.getSizeT() == 1 );

  if ( v )
    ASSERT( v->getSizeZ() == 1 && v->getSizeT() == 1 );

  if ( w.getSizeY() > 1 )  ww = undiag( w );
  else ww = w;

  int n = w.getSizeX();
  int m = u.getSizeX();

  for ( i=0; i<n-1; i++ )
    {
      k = i;
      p = ww( k );

      for ( j=i+1; j<n; j++ )
	if ( ww( j ) >= p )
	  {
	    k = j;
	    p = ww( k );
	  }

      if ( k != i )
	{
	  ww( k ) = ww( i );
	  ww( i ) = p;

	  if ( v )
	    {
	      for ( j=0; j<n; j++ )
		{
		  p = (*v)( j, i );
		  (*v)( j, i ) = (*v)( j, k );
		  (*v)( j, k ) = p;
		}
	    }

	  for ( j=0; j<m; j++ )
	    {
	      p = u( j, i );
	      u( j, i ) = u( j, k );
	      u( j, k ) = p;
	    }
	}
    }

  if ( retType == MatrixOfSingularValues )
    w = diag( ww );
  else
    w = ww;
}

template void
AimsSVD< float >::sort( VolumeRef< float >& u, VolumeRef< float >&w,
                        VolumeRef< float > *v );
template void
AimsSVD< double >::sort( VolumeRef< double >& u, VolumeRef< double >&w,
                         VolumeRef< double > *v );

template< class T >
VolumeRef< T > AimsSVD< T >::backwardSubstitution( const VolumeRef< T >& u,
                                                   const VolumeRef< T >& w,
                                                   const VolumeRef< T >& v,
                                                   const VolumeRef< T >& y )
{

  int m = u.getSizeX();
  int n = u.getSizeY();

  ASSERT( y.getSizeX() == m );

  VolumeRef< T > x( n, 1, 1, 1, AllocatorContext::fast() );
  int i, j, jj;
  T s;

  VolumeRef< T > tmp( n, 1, 1, 1, AllocatorContext::fast() );

  if ( retType == MatrixOfSingularValues ) 
    for ( j = 0; j < n; j++ )
    {

      s = ( T )0.0;
      if ( w( j, j ) )
      {

        for ( i = 0; i < m; i++ )
          s += u( i, j ) * y( i );
        s /= w( j, j );

      }
      tmp( j ) = s;

    }
  else
    for ( j = 0; j < n; j++ )
    {

      s = ( T )0.0;
      if ( w( j ) )
      {

        for ( i = 0; i < m; i++ )
          s += u( i, j ) * y( i );
        s /= w( j );

      }
      tmp( j ) = s;

    }

  for ( j = 0; j < n; j++ )
  {

    s = 0.0;
    for ( jj = 0; jj < n; jj++ )
      s += v(  j, jj ) * tmp( jj );
    x( j ) = s;

  }

  return x;

}


template VolumeRef< float >
AimsSVD< float >::backwardSubstitution( const VolumeRef< float >& u,
                                        const VolumeRef< float >& w,
                                        const VolumeRef< float >& v,
                                        const VolumeRef< float >& y );


template VolumeRef< double >
AimsSVD< double >::backwardSubstitution( const VolumeRef< double >& u,
                                         const VolumeRef< double >& w,
                                         const VolumeRef< double >& v,
                                         const VolumeRef< double >& y );
