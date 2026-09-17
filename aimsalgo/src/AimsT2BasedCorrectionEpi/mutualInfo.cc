
#include "mutualInfo.h"
#include <math.h>

float mutualInformation( const float* p1,
                         const float* p2,
                         float** p12,
                         int levels )
{

  float v1 = 0.0f;
  float v2 = 0.0f;
  float v12 = 0.0f;
  float mi=0.0;
  int l1, l2;
  for ( l2 = 0; l2 < levels; l2++ )
    for ( l1 = 0; l1 < levels; l1++ )
    {

      v1 = p1[ l1 ];
      v2 = p2[ l2 ];
      v12 = p12[ l1 ][ l2 ];
      if ( v2 && v12 && v1 )
        mi += v12 * log( v12 / ( v1 * v2 ) );

    }

  return mi;
}
