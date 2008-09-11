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
