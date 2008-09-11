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


#include <aims/math/balbak.h>
#include <aims/data/fastAllocationData.h>

using namespace aims;

template < class T >
AimsData< T > BackBalancing< T >::doit( AimsData< T >& mat, AimsData< T >& wr, 
					AimsData< T >& wi, AimsData< T > *sc )
{
  ASSERT( mat.dimZ() == 1 && mat.dimT() == 1 );
  ASSERT( mat.dimX() == mat.dimY() );
  ASSERT( wr.dimY() == 1 && wr.dimZ() == 1 && wr.dimT() == 1 );
  ASSERT( wi.dimY() == 1 && wi.dimZ() == 1 && wi.dimT() == 1 );
  ASSERT( wr.dimX() == wi.dimX() && wr.dimX() == mat.dimX() );

  int i, j;
  T s, norm;

  int n = mat.dimX();

  AimsFastAllocationData< T > scale( n );

  if ( sc ) scale = *sc;
  else
    for ( i=0; i<n; i++ )  scale( i ) = (T)1;

  ASSERT( scale.dimX() == n );

  for ( i=0; i<n; i++ )
    {
      s = scale( i );

      for ( j=0; j<n; j++ )
	mat( i, j ) *= s;
    }

  for ( j=0; j<n; j++ )
    {
      norm = (T)0;

      if ( wi( j ) == (T)0 )
	{
	  for ( i=0; i<n; i++ )
	    norm += mat( i, j ) * mat( i, j );

	  norm = (T)sqrt( norm );

	  for ( i=0; i<n; i++ )
	    mat( i, j ) /= norm;
	}
      else if ( wi( j ) > (T)0 )
	{
	  for ( i=0; i<n; i++ )
	    norm += mat( i, j ) * mat( i, j ) + mat( i, j+1 ) * mat( i, j+1 );

	  norm = (T)sqrt( norm );

	  for ( i=0; i<n; i++ )
	    {
	      mat( i, j ) /= norm;
	      mat( i, j + 1 ) /= norm;
	    }
	}
    }

  return mat;
}


template AimsData< float > 
BackBalancing< float >::doit( AimsData< float >& mat, AimsData< float >& wr,
			      AimsData< float >& wi, AimsData< float > *sc );

template AimsData< double >
BackBalancing< double >::doit( AimsData< double >& mat, 
			       AimsData< double >& wr,
			       AimsData< double >& wi, 
			       AimsData< double > *sc );
