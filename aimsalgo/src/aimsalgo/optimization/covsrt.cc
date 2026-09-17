
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cartodata/volume/volume.h>
#include <aims/optimization/covsrt.h>


using namespace carto;

template < class T >
VolumeRef< T > CovarianceStorage< T >::doit( rc_ptr<Volume< T > >& covar,
                                             rc_ptr<Volume< int > > *ia,
                                             int mfit )
{
  ASSERT( covar->getSizeZ() == 1 && covar->getSizeT() == 1 );
  ASSERT( covar->getSizeX() == covar->getSizeY() );

  int i, j, k;

  T dum;

  int ma = covar->getSizeX();

  VolumeRef< int > iab( ma );

  if ( ia )
    {
      for ( i=mfit; i<ma; i++ )
        for ( j=0; j<=i; j++ )
          covar->at( i, j ) = covar->at( j, i ) = (T)0;

      iab = *ia;
    }
  else  
    {
      for ( i=0; i<ma; i++ )  iab( i ) = 1;
      mfit = ma;
    }

  k = mfit - 1;

  for ( j=ma; j--; )
    {
      if ( iab( j ) )
	{
	  for ( i=0; i<ma; i++ )
	    {
	      dum = covar->at( i, k );
	      covar->at( i, k ) = covar->at( i, j );
	      covar->at( i, j ) = dum;
	    }

	  for ( i=0; i<ma; i++ )
	    {
	      dum = covar->at( k, i );
	      covar->at( k, i ) = covar->at( j, i );
	      covar->at( j, i ) = dum;
	    }

	  k--;
	}
    }

  return covar;
}


template VolumeRef< float >
CovarianceStorage< float >::doit( rc_ptr<Volume< float > >& covar,
				  rc_ptr<Volume< int > > *ia, int mfit );


template VolumeRef< double >
CovarianceStorage< double >::doit( rc_ptr<Volume< double > >& covar,
				   rc_ptr<Volume< int > > *ia, int mfit );
