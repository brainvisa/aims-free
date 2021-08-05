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
#include <aims/math/mathelem.h>
#include <aims/math/householder.h>
#include <cartodata/volume/volume.h>

using namespace carto;

template < class T >
void HouseholderTridiag< T >::doit( VolumeRef< T > a,
                                    VolumeRef< T > d,
                                    VolumeRef< T > e )
{
  ASSERT( a.getSizeZ() == 1 && a.getSizeT() == 1);
  ASSERT( d.getSizeY() == 1 && d.getSizeZ() == 1 && d.getSizeT() == 1 );
  ASSERT( e.getSizeY() == 1 && e.getSizeZ() == 1 && e.getSizeT() == 1 );

  int l,k,j,i,n;
  T scale,hh,h,g,f;

  n = a.getSizeX();

  for ( i = n - 1; i >= 1; i-- )
  {
    l = i - 1;
    h = 0;
    scale = 0;
    if ( l > 0 )
    {
      for ( k = 0; k <= l; k++ )
        scale += fabs( a( i, k ) );
      if ( scale == 0 ) e( i ) = a( i, l );
      else
      {
        for ( k = 0; k <= l; k++ )
        {
          a( i, k ) /= scale;
          h += square( a( i, k ) );
        }
        f = a( i, l );
        g = ( f >= 0 ) ? -sqrt( h ) : +sqrt( h );
        e( i ) = scale * g;
        h -= f * g;
        a( i, l )  = f - g;
        f = 0;
        for ( j = 0; j <= l; j++ )
        {
          a( j, i )  = a( i, j ) / h;
          g = 0;
          for ( k = 0; k <= j; k++ ) 
            g += a( j, k ) * a( i, k );
          for ( k = j + 1; k <= l; k++ )
            g += a( k, j ) * a( i, k );
          e( j ) = g / h;
          f += e( j ) * a( i, j );
        }
        hh = f / ( h + h );
        for ( j = 0; j <= l; j++ )
        {
          f = a( i, j );
          e( j ) = g = e( j ) - hh * f;
          for ( k = 0; k <= j; k++ )
            a( j, k ) -= f * e( k ) + g * a( i, k );
        }
      }
    }
    else
      e( i ) = a( i, l );
    d( i ) = h;
  }
  d( 0 ) = 0;
  e( 0 ) = 0;
  for ( i = 0; i < n; i++ )
  {
    l = i - 1;
    if ( d( i ) )
    {
      for ( j = 0; j <= l; j++ )
      {
        g = (T)0;
        for ( k = 0; k <= l; k++ )
          g += a( i, k ) * a( k, j );
        for ( k = 0; k <= l; k++ )
          a( k, j ) -= g * a( k, i );
      }
    }
    d( i ) = a( i, i );
    a( i, i ) = (T)1;
    for ( j = 0; j <= l; j++ )
      a( j, i ) = a( i, j ) = (T)0;
  }
}


template void 
HouseholderTridiag< float >::doit( VolumeRef< float > a,
                                   VolumeRef< float > d,
                                   VolumeRef< float > e );


template void 
HouseholderTridiag< double >::doit( VolumeRef< double > a,
                                    VolumeRef< double > d,
                                    VolumeRef< double > e );
