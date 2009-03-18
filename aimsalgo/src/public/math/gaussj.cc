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


#include <cstdlib>
#include <aims/data/fastAllocationData.h>
#include <aims/math/gaussj.h>


using namespace aims;


template < class T >
bool GaussJordan< T >::doit( AimsData< T >& a, AimsData< T >& b, int mUtil )
{
  ASSERT( a.dimZ() == 1 && a.dimT() == 1 );
  ASSERT( a.dimX() == a.dimY() );
  ASSERT( b.dimZ() == 1 && b.dimT() == 1 );

  int n = a.dimX();
  int m = b.dimY();

  if ( mUtil == -1 ) ASSERT( a.dimY() == b.dimX() );
  else 
    {
      ASSERT( mUtil <= n && b.dimX() == mUtil );
      n = mUtil;
    }

  int i, icol = 0, irow = 0, j, k, l, ll;

  T big, dum, pivinv;

  AimsFastAllocationData< int > indxc( n );
  AimsFastAllocationData< int > indxr( n );
  AimsFastAllocationData< int > ipiv( n );

  for ( j=0; j<n; j++ )  ipiv( j ) = 0;

  for ( i=0; i<n; i++ )
    {
      big = (T)0;

      for ( j=0; j<n; j++ )
	if ( ipiv( j ) != 1 )
	  for ( k=0; k<n; k++ )
	    {
	      if ( ipiv( k ) == 0 )
		{
		  if ( (T)fabs( a( j, k ) ) >= big )
		    {
		      big = (T)fabs( a( j, k ) );
		      irow = j;
		      icol = k;
		    }
		}
	      else if ( ipiv( k ) > 1 )  return false;
	    }

      ++( ipiv( icol ) );

      if ( irow != icol )
	{
	  for ( l=0; l<n; l++ )
	    {
	      dum = a( irow, l );
	      a( irow, l ) = a( icol, l );
	      a( icol, l ) = dum;
	    }

	  for ( l=0; l<m; l++ )
	    {
	      dum = b( irow, l );
	      b( irow, l ) = b( icol, l );
	      b( icol, l ) = dum;
	    }
	}

      indxr( i ) = irow;
      indxc( i ) = icol;

      if ( a( icol, icol ) == (T)0 )  return false;

      pivinv = (T)1 / a( icol, icol );
      a( icol, icol ) = (T)1;

      for ( l=0; l<n; l++ )  a( icol, l ) *= pivinv;
      for ( l=0; l<m; l++ )  b( icol, l ) *= pivinv;

      for ( ll=0; ll<n; ll++ )
	if ( ll != icol )
	  {
	    dum = a( ll, icol );
	    a( ll, icol ) = (T)0;

	    for ( l=0; l<n; l++ )  a( ll, l ) -= a( icol, l ) * dum;
	    for ( l=0; l<m; l++ )  b( ll, l ) -= b( icol, l ) * dum;
	  }
    }

  for ( l=n; l--; )
    if ( indxr( l ) != indxc( l ) )
      for ( k=0; k<n; k++ )
	{
	  dum = a( k, indxr( l ) );
	  a( k, indxr( l ) ) = a( k, indxc( l ) );
	  a( k, indxc( l ) ) = dum;
	}

  return true;
}


template bool
GaussJordan< float >::doit( AimsData< float >& a, AimsData< float >& b,
			    int mUtil );


template bool
GaussJordan< double >::doit( AimsData< double >& a, AimsData< double >& b,
			     int mUtil );
