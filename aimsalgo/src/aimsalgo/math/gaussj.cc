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
#include <cartodata/volume/volume.h>
#include <aims/math/gaussj.h>


using namespace carto;


template < class T >
bool GaussJordan< T >::doit( rc_ptr<Volume< T > > & a,
                             rc_ptr<Volume< T > > & b, int mUtil )
{
  ASSERT( a->getSizeZ() == 1 && a->getSizeT() == 1 );
  ASSERT( a->getSizeX() == a->getSizeY() );
  ASSERT( b->getSizeZ() == 1 && b->getSizeT() == 1 );

  int n = a->getSizeX();
  int m = b->getSizeY();

  if ( mUtil == -1 ) ASSERT( a->getSizeY() == b->getSizeX() );
  else 
    {
      ASSERT( mUtil <= n && b->getSizeX() == mUtil );
      n = mUtil;
    }

  int i, icol = 0, irow = 0, j, k, l, ll;

  T big, dum, pivinv;

  VolumeRef< int > indxc( n, 1, 1, 1, AllocatorContext::fast() );
  VolumeRef< int > indxr( n, 1, 1, 1, AllocatorContext::fast() );
  VolumeRef< int > ipiv( n, 1, 1, 1, AllocatorContext::fast() );

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
		  if ( (T)fabs( a->at( j, k ) ) >= big )
		    {
		      big = (T)fabs( a->at( j, k ) );
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
	      dum = a->at( irow, l );
	      a->at( irow, l ) = a->at( icol, l );
	      a->at( icol, l ) = dum;
	    }

	  for ( l=0; l<m; l++ )
	    {
	      dum = b->at( irow, l );
	      b->at( irow, l ) = b->at( icol, l );
	      b->at( icol, l ) = dum;
	    }
	}

      indxr( i ) = irow;
      indxc( i ) = icol;

      if ( a->at( icol, icol ) == (T)0 )  return false;

      pivinv = (T)1 / a->at( icol, icol );
      a->at( icol, icol ) = (T)1;

      for ( l=0; l<n; l++ )  a->at( icol, l ) *= pivinv;
      for ( l=0; l<m; l++ )  b->at( icol, l ) *= pivinv;

      for ( ll=0; ll<n; ll++ )
	if ( ll != icol )
	  {
	    dum = a->at( ll, icol );
	    a->at( ll, icol ) = (T)0;

	    for ( l=0; l<n; l++ )  a->at( ll, l ) -= a->at( icol, l ) * dum;
	    for ( l=0; l<m; l++ )  b->at( ll, l ) -= b->at( icol, l ) * dum;
	  }
    }

  for ( l=n; l--; )
    if ( indxr( l ) != indxc( l ) )
      for ( k=0; k<n; k++ )
	{
	  dum = a->at( k, indxr( l ) );
	  a->at( k, indxr( l ) ) = a->at( k, indxc( l ) );
	  a->at( k, indxc( l ) ) = dum;
	}

  return true;
}


template bool
GaussJordan< float >::doit( rc_ptr<Volume< float > > & a,
                            rc_ptr<Volume< float > > & b, int mUtil );


template bool
GaussJordan< double >::doit( rc_ptr<Volume< double > > & a,
                             rc_ptr<Volume< double > > & b, int mUtil );

