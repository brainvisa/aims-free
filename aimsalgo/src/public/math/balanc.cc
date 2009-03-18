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
#include <aims/math/balanc.h>

using namespace aims;

template < class T >
Balancing< T >::Balancing() : radix( (T)16.0 )
{
}

template < class T >
AimsData< T > Balancing< T >::doit( AimsData< T >& mat, AimsData< T > *sc )
{
  ASSERT( mat.dimZ() == 1 && mat.dimT() == 1 );
  ASSERT( mat.dimX() == mat.dimY() );

  int i, j, last;
  T s, r, g, f, c, sqrdx;

  int n = mat.dimX();

  AimsFastAllocationData< T > scale( n );

  sqrdx = radix * radix;
  last = 0;

  for ( i=0; i<n; i++ )  scale( i ) = (T)1;

  while( last == 0 )
    {
      last = 1;

      for ( i=0; i<n; i++ )
	{
	  r = c = (T)0;

	  for ( j=0; j<n; j++ )
	    if ( j != i )
	      {
		c += (T)fabs( mat( j, i ) );
		r += (T)fabs( mat( i, j ) );
	      }

	  if ( c && r )
	    {
	      g = r / radix;
	      f = (T)1;
	      s = c + r;

	      while( c < g )
		{
		  f *= radix;
		  c *= sqrdx;
		}

	      g = r * radix;

	      while( c > g )
		{
		  f /= radix;
		  g /= sqrdx;
		}

	      if ( ( c + r ) / f < (T)( 0.95 * s ) )
		{
		  last = 0;
		  g = (T)1 / f;
		  scale( i ) *= f;

		  for ( j=0; j<n; j++ )
		    mat( i, j ) *= g;

		  for ( j=0; j<n; j++ )
		    mat( j, i ) *= f;
		}
	    }
	}
    }

  if ( sc )  *sc = scale;

  return mat;
}


template
Balancing< float >::Balancing();

template AimsData< float > 
Balancing< float >::doit( AimsData< float >& mat, AimsData< float > *sc );


template
Balancing< double >::Balancing();

template AimsData< double >
Balancing< double >::doit( AimsData< double >& mat, AimsData< double > *sc );
