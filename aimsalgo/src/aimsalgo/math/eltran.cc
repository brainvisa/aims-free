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


#include <cartodata/volume/volume.h>
#include <aims/math/eltran.h>

using namespace carto;

template < class T >
VolumeRef< T > HessenbergAccumulation< T >::doit(
  const VolumeRef< T > & a, const VolumeRef< short > *isc )
{
  ASSERT( a.getSizeZ() == 1 && a.getSizeT() == 1 );
  ASSERT( a.getSizeX() == a.getSizeY() );

  int i, j, mp, mp1;

  int n = a.getSizeX();

  VolumeRef< T > zev( n, n, 1, 1,
                      AllocatorContext(
                        &carto::MemoryAllocator::singleton() ) );
  VolumeRef< short > iscale( n, 1, 1, 1,
                             AllocatorContext(
                                &carto::MemoryAllocator::singleton() ) );

  if ( isc )  iscale = *isc;
  else
    for ( i=0; i<n; i++ )  iscale( i ) = i;

  ASSERT( iscale.getSizeX() == n );

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


template VolumeRef< float >
HessenbergAccumulation< float >::doit( const VolumeRef< float > & a,
                                       const VolumeRef< short > *isc );


template VolumeRef< double >
HessenbergAccumulation< double >::doit( const VolumeRef< double > & a,
                                        const VolumeRef< short > *isc );
