
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
