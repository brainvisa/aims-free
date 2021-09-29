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
