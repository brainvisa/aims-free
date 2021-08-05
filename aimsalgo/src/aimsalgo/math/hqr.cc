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


#include <cstdlib>
#include <aims/math/mathelem.h>
#include <aims/math/hqr.h>
#include <cartodata/volume/volume.h>
#include <math.h>
#include <complex>

using namespace std;
using namespace carto;
using namespace aims;

template < class T >
VolumeRef< T > HessenbergQR< T >::doit( VolumeRef< T > a, VolumeRef< T > imag,
                                        VolumeRef< T > *zz )
{
  ASSERT( a.getSizeZ() == 1 && a.getSizeT() == 1 );
  ASSERT( a.getSizeX() == a.getSizeY() );

  bool eigenVal = false;
  int nn, m, l, k, j, its, i, mmin;
  T z = 0, y, x, w, v, u, t, s = 0, r = 0, q = 0, p = 0, anorm, snorm, ra, 
    sa, vr, vi;

  int n = a.getSizeX();

  VolumeRef< T > wr( n, 1, 1, 1,
                     AllocatorContext(
                        &carto::MemoryAllocator::singleton() ) );
  VolumeRef< T > wi( n, 1, 1, 1,
                     AllocatorContext(
                        &carto::MemoryAllocator::singleton() ) );

  if ( zz )  eigenVal = true;

  anorm = (T)fabs( a( 0, 0 ) );

  for ( i=1; i<n; i++ )
    for ( j=i-1; j<n; j++ )
      anorm += (T)fabs( a( i, j ) );

  nn = n - 1;
  t = (T)0;

  while( nn >= 0 )
    {
      its = 0;

      do
	{
	  for ( l=nn; l>=1; l-- )
	    {
	      s = (T)( fabs( a( l - 1, l - 1 ) ) + fabs( a( l, l ) ) );

	      if ( s == (T)0 )  s = anorm;

	      if ( (T)fabs( a( l, l - 1 ) ) <= (T)( 1.0e-9 * s ) )  break;
	    }

	  x = a( nn, nn );

	  if ( l == nn )
	    {
	      wr( nn ) = x + t;
	      wi( nn-- ) = (T)0;
	    }
	  else
	    {
	      y = a( nn - 1, nn - 1 );
	      w = a( nn, nn - 1 ) * a( nn - 1, nn );

	      if ( l == ( nn - 1 ) )
		{
		  p = (T)( 0.5 * ( y - x ) );
		  q = p * p + w;
		  z = (T)sqrt( fabs( q ) );
		  x += t;

		  if ( q >= (T)0 )
		    {
		      z = p + ( ( p >= (T)0 ) ? z : -z );
		      wr( nn - 1 ) = wr( nn ) = x + z;

		      if ( z )  wr( nn ) = x - w / z;

		      wi( nn - 1 ) = wi( nn ) = (T)0;

		      if ( eigenVal )
			{
			  x = a( nn, nn - 1 );
			  s = (T)( fabs( x ) + fabs( z ) );
			  p = x / s;
			  q = z / s;
			  r = (T)sqrt( p * p + q * q );
			  p /= r;
			  q /= r;

			  for ( j=nn-1; j<n; j++ )
			    {
			      z = a( nn - 1, j );
			      a( nn - 1, j ) = q * z + p * a( nn, j );
			      a( nn, j ) = q * a( nn, j ) - p * z;
			    }

			  for ( i=0; i<=nn; i++ )
			    {
			      z = a( i, nn - 1 );
			      a( i, nn - 1 ) = q * z + p * a( i, nn );
			      a( i, nn ) = q * a( i, nn ) - p * z;
			    }

			  for ( i=0; i<n; i++ )
			    {
			      z = (*zz)( i, nn - 1);
			      (*zz)( i, nn - 1 ) = q * z + p * (*zz)( i, nn );
			      (*zz)( i, nn ) = q * (*zz)( i, nn ) - p * z;
			    }
			}
		    }
		  else
		    {
		      wr( nn - 1 ) = wr( nn ) = x + p;
		      wi( nn ) = -( wi( nn - 1 ) = z );
		    }

		  nn -= 2;
		}
	      else
		{
		  ASSERT( its != 30 );

		  if ( its == 10 || its == 20 )
		    {
		      t += x;

		      for ( i=0; i<=nn; i++ )  a( i, i ) -= x;

		      s = (T)( fabs( a( nn, nn - 1 ) ) + 
			       fabs( a( nn - 1, nn - 2 ) ) );
		      y = x = (T)( 0.75 * s );
		      w = (T)( -0.4375 * s * s );
		    }

		  ++its;

		  for ( m=(nn-2); m>=l; m-- )
		    {
		      z = a( m, m );
		      r = x - z;
		      s = y - z;
		      p = ( r * s - w ) / a( m + 1, m ) + a( m, m + 1 );
		      q = a( m + 1, m + 1 ) - z - r - s;
		      r = a( m + 2, m + 1 );
		      s = (T)( fabs( p ) + fabs( q ) + fabs( r ) );
		      p /= s;
		      q /= s;
		      r /= s;

		      if ( m == l )  break;

		      u = (T)( fabs( a( m, m - 1 ) ) * 
			       ( fabs( q ) + fabs( r ) ) );
		      v = (T)( fabs( p ) * ( fabs( a( m - 1, m - 1 ) ) +
					     fabs( z ) +
					     fabs( a( m + 1, m + 1 ) ) ) );

		      if ( u <= (T)( 1.0e-9 * v ) )  break;
		    }

		  for ( i=m+2; i<=nn; i++ )
		    {
		      a( i, i - 2 ) = (T)0;

		      if ( i != ( m + 2 ) )  a( i, i - 3 ) = (T)0;
		    }

		  for ( k=m; k<=nn-1; k++ )
		    {
		      if ( k != m )
			{
			  p = a( k, k - 1 );
			  q = a( k + 1, k - 1 );
			  r = (T)0;

			  if ( k != ( nn - 1 ) )  r = a( k + 2, k - 1 );

			  x = (T)( fabs( p ) + fabs( q ) + fabs( r ) );

			  if ( x )
			    {
			      p /= x;
			      q /= x;
			      r /= x;
			    }
			}

		      snorm = (T)sqrt( p * p + q * q + r * r );
		      s = ( p >= (T)0 ) ? snorm : -snorm;

		      if ( s )
			{
			  if ( k == m )
			    {
			      if ( l != m )  a( k, k - 1 ) = -a( k, k - 1 );
			    }
			  else
			    a( k, k - 1 ) = -s * x;

			  p += s;
			  x = p / s;
			  y = q / s;
			  z = r / s;
			  q /= p;
			  r /= p;

			  for ( j=k; j<=nn; j++ )
			    {
			      p = a( k, j ) + q * a( k + 1, j );

			      if ( k != ( nn - 1 ) )
				{
				  p += r * a( k + 2, j );
				  a( k + 2, j ) -= p * z;
				}

			      a( k + 1, j ) -= p * y;
			      a( k, j ) -= p * x;
			    }

			  mmin = ( nn < k + 3 ) ? nn : k + 3;

			  for ( i=l; i<=mmin; i++ ) 
			    {
			      p = x * a( i, k ) + y * a( i, k + 1 );

			      if ( k != ( nn - 1 ) ) 
				{
				  p += z * a( i, k + 2 );
				  a( i, k + 2 ) -= p * r;
				}

			      a( i, k + 1 ) -= p * q;
			      a( i, k ) -= p;
			    }

			  if ( eigenVal )
			    {
			      for ( i=0; i<n; i++ )
				{
				  p = x * (*zz)( i, k ) + y * (*zz)( i, k+1 );

				  if ( k != ( nn - 1 ) )
				    {
				      p += z * (*zz)( i, k + 2 );
				      (*zz)( i, k + 2 ) -= p * r;
				    }

				  (*zz)( i, k + 1 ) -= p * q;
				  (*zz)( i, k ) -= p;
				}
			    }
			}
		    }
		}
	    }
	}
      while( l < nn - 1 );
    }

  if ( eigenVal )
    {
      if ( (T)fabs( anorm ) > 1.0e-9 )
	{
	  complex< T > z1;
	  complex< T > z2;

	  for ( nn=n-1; nn>=0; nn-- )
	    {
	      p = wr( nn );
	      q = wi( nn );

	      if ( q > (T)0 )  continue;

	      if ( q == (T)0 )
		{
		  m = nn;
		  a( nn, nn ) = (T)1;

		  for ( i=nn-1; i>=0; i-- )
		    {
		      w = a( i, i  ) - p;
		      r = a( i, nn );

		      for ( j=m; j<=nn-1; j++ )
			r += a( i, j ) * a( j, nn );

		      if ( wi( i ) < (T)0 )
			{
			  z = w;
			  s = r;
			}
		      else
			{
			  m = i;

			  if ( wi( i ) == (T)0 )
			    {
			      t = w;

			      if ( w == (T)0 )  t = (T)( 1.0e-9 * anorm );

			      a( i, nn ) = -r / t;
			    }
			  else
			    {
			      x = a( i, i + 1 );
			      y = a( i + 1, i );
			      q = square( wr( i ) - p ) +  square( wi( i ) );
			      t = ( x * s - z * r ) / q;
			      a( i, nn ) = t;

			      if ( (T)fabs( x ) > (T)fabs( z ) )
				a( i + 1, nn ) = ( -r - w * t ) / x;
			      else
				a( i + 1, nn ) = ( -s - y * t ) / z;
			    }
			}
		    }
		}
	      else
		{
		  m = nn - 1;

		  if ( (T)fabs( a( nn, nn-1 ) ) > (T)fabs( a( nn-1, nn ) ) )
		    {
		      a( nn - 1, nn - 1 ) = q / a( nn, nn - 1 );
		      a( nn - 1, nn ) = -( a( nn, nn ) - p ) / a( nn, nn - 1 );
		    }
		  else
		    {
		      z1 = complex< T >( (T)0, -a( nn - 1, nn ) );
		      z2 = complex< T >( a( nn - 1, nn - 1 ) - p, q );
		      z1 /= z2;
		      a( nn - 1, nn - 1 ) = z1.real();
		      a( nn - 1, nn ) = z1.imag();
		    }

		  a( nn, nn -1 ) = (T)0;
		  a( nn, nn ) = (T)1;

		  for ( i=nn-2; i>=0; i++ )
		    {
		      w = a( i, i ) - p;
		      ra = (T)0;
		      sa = a( i, nn );

		      for ( j=m; j<=nn-1; j++ )
			{
			  ra += a( i, j ) * a( j, nn - 1 );
			  sa += a( i, j ) * a( j, nn );
			}

		      if ( wi( i ) < (T)0 )
			{
			  z = w;
			  r = ra;
			  s = sa;
			}
		      else
			{
			  m = i;

			  if ( wi( i ) == (T)0 )
			    {
			      z1 = complex< T >( -ra, -sa );
			      z2 = complex< T >( w, q );
			      z1 /= z2;
			      a( i, nn - 1 ) = z1.real();
			      a( i, nn ) = z1.imag();
			    }
			  else
			    {
			      x = a( i, i + 1 );
			      y = a( i + 1, i );
			      vr = square( wr(i) - p ) + square( wi (i) ) - 
				q * q;
			      vi = (T)2 * ( wr( i ) - p ) * q;

			      if ( vr == (T)0 && vi == (T)0 )
				vr = (T)( 1.0e-9*anorm*( fabs(w) + fabs(q) +
							 fabs(x) + fabs(y) +
							 fabs(z) ) );

			      z1 = complex< T >( x * r - z * ra + q * sa, 
						 x * s - z * sa - q * ra );
			      z2 = complex< T >( vr, vi );
			      z1 /= z2;
			      a( i, nn -1 ) = z1.real();
			      a( i, nn ) = z1.imag();

			      if ( (T)fabs( x ) > (T)( fabs(z) + fabs(q) ) )
				{
				  a( i+1, nn-1 ) = ( -ra - 
						     w * a( i , nn - 1 ) +
						     q * a( i, nn ) ) / x;
				  a( i+1, nn ) = ( -sa - w * a( i, nn ) +
						   q * a( i, nn - 1 ) ) / x;
				}
			      else
				{
				  z1 = complex< T >( -r - y * a( i, nn - 1 ),
						     -s - y * a( i, nn ) );
				  z2 = complex< T >( z, q );
				  z1 /= z2;
				  a( i + 1, nn - 1 ) = z1.real();
				  a( i + 1, nn ) = z1.imag();
				}
			    }
			}
		    }
		}
	    }
	}

      for ( j=n-1; j>=0; j-- )
	for ( i=0; i<n; i++ )
	  {
	    z = (T)0;

	    for ( k=0; k<=j; k++ )
	      z += (*zz)( i, k ) * a( k, j );

	    (*zz)( i, j ) = z;
	  }
    }

  imag = wi;

  return wr;
}


template VolumeRef< float >
HessenbergQR< float >::doit( VolumeRef< float > a, VolumeRef< float > imag,
                             VolumeRef< float > *zz );


template VolumeRef< double >
HessenbergQR< double >::doit( VolumeRef< double > a,
                              VolumeRef< double > imag,
                              VolumeRef< double > *zz );
