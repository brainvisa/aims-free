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


#include <aims/data/fastAllocationData.h>
#include <aims/math/eltran.h>

using namespace aims;

template < class T >
AimsData< T > HessenbergAccumulation< T >::doit( AimsData< T >& a, 
						 AimsData< short > *isc )
{
  ASSERT( a.dimZ() == 1 && a.dimT() == 1 );
  ASSERT( a.dimX() == a.dimY() );

  int i, j, mp, mp1;

  int n = a.dimX();

  AimsFastAllocationData< T > zev( n, n );
  AimsFastAllocationData< short > iscale( n );

  if ( isc )  iscale = *isc;
  else
    for ( i=0; i<n; i++ )  iscale( i ) = i;

  ASSERT( iscale.dimX() == n );

  for ( i=0; i<n; i++ )
    for ( j=0; j<n; j++ )
      {
	if ( i == j )  zev( i, j ) = (T)1;
	else zev( i, j ) = (T)0;
      }

  for ( mp=n-2; mp>0; mp-- )
    {
      mp1 = mp + 1;

      for ( i=mp1; i<n; i++ )
	zev( i, mp ) = a( i, mp - 1 );

      i = iscale( mp );

      if ( i == mp )  continue;

      for ( j=mp; j<n; j++ )
	{
	  zev( mp, j ) = zev( i, j );
	  zev( i, j ) = (T)0;
	}

      zev( i, mp ) = (T)1;
    }

  return zev;
}


template AimsData< float >
HessenbergAccumulation< float >::doit( AimsData< float >& a, 
				       AimsData< short > *isc );


template AimsData< double >
HessenbergAccumulation< double >::doit( AimsData< double >& a,
					AimsData< short > *isc );
