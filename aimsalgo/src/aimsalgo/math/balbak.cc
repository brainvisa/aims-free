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


#include <aims/math/balbak.h>
#include <cartodata/volume/volume.h>

using namespace carto;

template < class T >
VolumeRef< T > BackBalancing< T >::doit( VolumeRef< T > mat,
                                        const VolumeRef< T >& wr,
                                        const VolumeRef< T >& wi,
                                        const VolumeRef< T > *sc )
{
  ASSERT( mat.getSizeZ() == 1 && mat.getSizeT() == 1 );
  ASSERT( mat.getSizeX() == mat.getSizeY() );
  ASSERT( wr.getSizeY() == 1 && wr.getSizeZ() == 1 && wr.getSizeT() == 1 );
  ASSERT( wi.getSizeY() == 1 && wi.getSizeZ() == 1 && wi.getSizeT() == 1 );
  ASSERT( wr.getSizeX() == wi.getSizeX() && wr.getSizeX() == mat.getSizeX() );

  int i, j;
  T s, norm;

  int n = mat.getSizeX();

  VolumeRef< T > scale( n, 1, 1, 1,
                        AllocatorContext(
                          &carto::MemoryAllocator::singleton() ) );

  if ( sc ) scale = *sc;
  else
    for ( i=0; i<n; i++ )  scale( i ) = (T)1;

  ASSERT( scale.getSizeX() == n );

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


template VolumeRef< float >
BackBalancing< float >::doit( VolumeRef< float > mat,
                              const VolumeRef< float >& wr,
                              const VolumeRef< float >& wi,
                              const VolumeRef< float > *sc );

template VolumeRef< double >
BackBalancing< double >::doit( VolumeRef< double > mat,
                               const VolumeRef< double >& wr,
                               const VolumeRef< double >& wi,
                               const VolumeRef< double > *sc );
