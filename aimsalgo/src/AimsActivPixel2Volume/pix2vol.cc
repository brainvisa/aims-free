
#include "pix2vol.h"

using namespace std;

AimsData< short >
AimsActivPixel2Volume( AimsData< short >& vol, AimsActivPixel& pixels,
                       Point3df& trans, short label )
{
  ASSERT(vol.dimT()==1);
  AimsData< short > res( vol.dimX(), vol.dimY(), vol.dimZ() );
  res.setSizeXYZT( vol.sizeX(), vol.sizeY(), vol.sizeZ(), 1.0f );

  Point3df orig;
  Point3d pos;

  AimsVector< int, 3 > fact;

  orig[0] = res.dimX() * res.sizeX() / 2.0f - trans[0];
  orig[1] = res.dimY() * res.sizeY() / 2.0f - trans[1];
  orig[2] = res.dimZ() * res.sizeZ() / 2.0f - trans[2];

  fact[0] = (int)( pixels.voxelSize()[0] / res.sizeX() / 2.0f );
  fact[1] = (int)( pixels.voxelSize()[1] / res.sizeY() / 2.0f );
  fact[2] = (int)( pixels.voxelSize()[2] / res.sizeZ() / 2.0f );

  int i, j, k;
  vector< Point3df >::iterator it;
  for ( it=pixels.activPoints().begin(); it!=pixels.activPoints().end(); ++it )
    {
      pos[0] = (short)( ( orig[0] + (*it)[0] ) / res.sizeX() + 0.5f );
      pos[1] = (short)( ( orig[1] - (*it)[1] ) / res.sizeY() + 0.5f );
      pos[2] = (short)( ( orig[2] - (*it)[2] ) / res.sizeZ() + 0.5f );

      for ( k=-fact[2]; k<=fact[2]; k++ )
        for ( j=-fact[1]; j<=fact[1]; j++ )
          for ( i=-fact[0]; i<=fact[0]; i++ )
            res( pos[0]+i, pos[1]+j, pos[2]+k ) = label;
    }

  return res;
}
