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


#include "interpolator.h"
#include <aims/data/data_g.h>
#include <aims/vector/vector.h>
#include <aims/math/math_g.h>

void SplineInterpolator::doit( const AimsData<short>& orig,
                               AimsData<short>& dest,
                               const AimsVector<float,3>& param ) const
{
  int dimX = orig.dimX();
  int dimY = orig.dimY();
  float sizeX = orig.sizeX();
  float sizeY = orig.sizeY();
  float centerX = dimX * sizeX / 2.0;
  float centerY = dimY * sizeY / 2.0;
  float value, translation=0.0, y=0.0;

  float scale = param[0];
  float T0 = param[1];
  float T1 = param[2];

  AimsData<float> column( dimY );
  AimsData<float> filtered( dimY );
  AimsData<float> r( 2 * dimY - 1 );
  r = 0.0;
  r( dimY - 1 ) = 4.0;
  r( dimY - 2 ) = r( dimY ) = 1.0;

  for ( int i = 0; i < dimX; i++ )
  {
    int j;
    for ( j = 0; j < dimY; j++ )
      column( j ) = 6.0 * orig( i, j );

    AimsToeplitz( r, column, (carto::VolumeRef<float> &) filtered );

    for ( j = 0; j < dimY; j++ )
    {
      translation = T0 + T1 * ( centerX - ( i + 0.5 ) * sizeX );
      y = centerY - ( centerY - ( ( j + 0.5 ) * sizeY ) ) * scale + translation;

      value = 0.0;
      for ( int k = 0; k < dimY; k++ )
        value += filtered( k ) * betaSpline3( y / sizeY - 0.5 - k );

      dest( i, j ) = short( value ); 
    }
  }
}
