
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

