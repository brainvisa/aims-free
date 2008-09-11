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


#include <aims/math/mathelem.h>
#include <aims/math/tqli.h>
#include <aims/data/data.h>

template < class T >
void DecompositionTQLI< T >::doit( AimsData< T >& d,
				   AimsData< T >& e,
				   AimsData< T >& z )
{
  ASSERT( z.dimZ() == 1 && z.dimT() == 1 );
  ASSERT( d.dimY() == 1 && d.dimZ() == 1 && d.dimT() == 1 );
  ASSERT( e.dimY() == 1 && e.dimZ() == 1 && e.dimT() == 1 );

  int m,l,iter,i,k,n;
  T s,r,p,g,f,dd,c,b;

  n = z.dimX();

  ASSERT( n == z.dimY() );

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
DecompositionTQLI< float >::doit( AimsData< float >& d,
				  AimsData< float >& e,
				  AimsData< float >& z );


template void 
DecompositionTQLI< double >::doit( AimsData< double >& d,
				   AimsData< double >& e,
				   AimsData< double >& z );
