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


#include <cstdlib>
#include <cartodata/volume/volume.h>
#include <aims/math/elmhes.h>

using namespace carto;

template < class T >
VolumeRef< T > HessenbergReduction< T >::doit( VolumeRef< T > a,
                                               VolumeRef< short > *isc )
{
  ASSERT( a.getSizeZ() == 1 && a.getSizeT() == 1 );
  ASSERT( a.getSizeX() == a.getSizeY() );

  int i, j, m;
  T x, y, dum;

  int n = a.getSizeX();

  VolumeRef< short > iscale( n, 1, 1, 1,
                             AllocatorContext(
                                &carto::MemoryAllocator::singleton() ) );

  for ( m=1; m<n-1; m++ )
    {
      x = (T)0;
      i = m;

      for ( j=m; j<n; j++ )
	{
	  if ( fabs( a( j, m - 1 ) ) > fabs( x ) )
	    {
	      x = a( j, m-1 );
	      i = j;
	    }
	}

      iscale( m ) = i;

      if ( i != m )
      {
	for ( j=m-1; j<n; j++ )
	  {
	    dum = a( i, j );
	    a( i, j ) = a( m, j );
	    a( m, j ) = dum;
	  }

	for ( j=0; j<n; j++ )
	  {
	    dum = a( j, i );
	    a( j, i ) = a( j, m );
	    a( j, m ) = dum;
	  }
      }

      if ( x )
	{
	  for ( i=m+1; i<n; i++ )
	    {
	      if( ( y = a( i, m - 1 ) ) )
		{
		  y /= x;
		  a(i, m - 1 ) = y;

		  for ( j=m; j<n; j++ )
		    a( i, j ) -= y * a( m, j );

		  for ( j=0; j<n; j++ )
		    a( j, m ) += y * a( j, i );
		}
	    }
	}
    }

  if ( isc )  *isc = iscale;

  return a;
}


template VolumeRef< float >
HessenbergReduction< float >::doit( VolumeRef< float > a,
				    VolumeRef< short > *isc );


template VolumeRef< double >
HessenbergReduction< double >::doit( VolumeRef< double > a,
				     VolumeRef< short > *isc );
