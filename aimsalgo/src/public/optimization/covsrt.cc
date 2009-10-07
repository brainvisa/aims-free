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


#include <aims/data/data.h>
#include <aims/optimization/covsrt.h>


template < class T >
AimsData< T > CovarianceStorage< T >::doit( AimsData< T >& covar, 
					    AimsData< int > *ia, int mfit )
{
  ASSERT( covar.dimZ() == 1 && covar.dimT() == 1 );
  ASSERT( covar.dimX() == covar.dimY() );

  int i, j, k;

  T dum;

  int ma = covar.dimX();

  AimsData< int > iab( ma );

  if ( ia )
    {
      for ( i=mfit; i<ma; i++ )
	for ( j=0; j<=i; j++ )
	  covar( i, j ) = covar( j, i ) = (T)0;

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
	      dum = covar( i, k );
	      covar( i, k ) = covar( i, j );
	      covar( i, j ) = dum;
	    }

	  for ( i=0; i<ma; i++ )
	    {
	      dum = covar( k, i );
	      covar( k, i ) = covar( j, i );
	      covar( j, i ) = dum;
	    }

	  k--;
	}
    }

  return covar;
}


template AimsData< float >
CovarianceStorage< float >::doit( AimsData< float >& covar, 
				  AimsData< int > *ia, int mfit );


template AimsData< double >
CovarianceStorage< double >::doit( AimsData< double >& covar, 
				   AimsData< int > *ia, int mfit );
