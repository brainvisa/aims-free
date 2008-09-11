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


#include <aims/distancemap/danielsson.h>
#include <aims/connectivity/connectivity.h>
#include <aims/data/data.h>
#include <aims/utility/clock.h>
#include <iostream>
#include <iomanip>

using namespace aims;
using namespace std;

AimsData<float> AimsDanielssonDistanceMap(const AimsData<int16_t>& vol, DanielssonSide side)
{
  ASSERT( vol.dimT() == 1 );

  int x, y, z, n;
  Point3d off( 0, 0, 0 ), black( 0, 0, 0 ), white( 32767, 32767, 32767 );
  AimsData< Point3d >* bck=NULL;
  int dimX = vol.dimX();
  int dimY = vol.dimY();
  int dimZ = vol.dimZ();

  switch ( side )
  {
    case AIMS_DANIELSSON_OUTSIDE: 
      cout << "\ndistance map outside object" << endl;
      bck = new AimsData<Point3d >( dimX, dimY, dimZ, 1, 1 );
      *bck = white;
      bck->fillBorder( white );
      for ( z = 0; z < dimZ; z++ )                              
        for ( y = 0; y < dimY; y++ )                              
          for ( x = 0; x < dimX; x++ )
            (*bck)( x, y, z ) = ( vol( x, y, z ) != 0 ? black : white );
      break;
    case AIMS_DANIELSSON_INSIDE: 
      cout << "\ndistance map inside object" << endl;
      bck = new AimsData<Point3d >( dimX, dimY, dimZ, 1, 1 );
      *bck = black;
      bck->fillBorder( white );
      for ( z = 0; z < dimZ; z++ )                              
        for ( y = 0; y < dimY; y++ )                              
          for ( x = 0; x < dimX; x++ )
            (*bck)( x, y, z ) = ( vol( x, y, z ) == 0 ? black : white );
      break;
  }

  Connectivity cnt9m( bck->oLine(), bck->oSlice(),
                          Connectivity::CONNECTIVITY_9_XZ_Yminus);
  Connectivity cnt9p( bck->oLine(), bck->oSlice(),
                          Connectivity::CONNECTIVITY_9_XZ_Yplus);
  Connectivity cnt5m( bck->oLine(), bck->oSlice(),
                          Connectivity::CONNECTIVITY_5_XminusZ);
  Connectivity cnt5p( bck->oLine(), bck->oSlice(),
                          Connectivity::CONNECTIVITY_5_XplusZ);

  Point3d  *pointer = NULL, tp;

  cout << "loop forward on slices\n" << flush;
  cout << "slice : " << setw(4) << 0 << flush;
  for ( z = 0; z < dimZ; z++ )
  {
    cout << "\b\b\b\b" << setw(4) << z << flush;
    for ( y = 1; y < dimY; y++ )
    {
      pointer = &( (*bck)( 0, y, z ) );
      for ( x = 0; x < dimX; x++ )
      {
        for ( n = 0; n < 9; n++ )
        {
          off.item( 0 ) = abs( cnt9m.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt9m.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt9m.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt9m.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer++;
      }

      pointer = &( (*bck)( 1, y, z ) );
      for ( x = 1; x < dimX; x++ )
      {
        for ( n = 0; n < 5; n++ )
        {
          off.item( 0 ) = abs( cnt5m.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt5m.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt5m.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt5m.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer++;
      }

      pointer = &( (*bck)( dimX - 2, y, z ) );
      for ( x = dimX - 1; x-- ; )
      {
        for ( n = 0; n < 5; n++)
        {
          off.item( 0 ) = abs( cnt5p.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt5p.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt5p.xyzOffset( n ).item( 2 ) );
          if ( norm2( ( tp = *( pointer + cnt5p.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer--;
      }
    }

    for ( y = dimY - 1; y--; )
    {
      pointer = &( (*bck)( 0, y, z ) );
      for ( x = 0; x < dimX; x++ )
      {
        for ( n = 0; n < 9; n++ )
        {
          off.item( 0 ) = abs( cnt9p.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt9p.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt9p.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt9p.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer++;
      }

      pointer = &( (*bck)( 1, y, z ) );
      for ( x = 1; x < dimX; x++ )
      {
        for ( n = 0; n < 5; n++ )
        {
          off.item( 0 ) = abs( cnt5m.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt5m.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt5m.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt5m.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer++;
      }

      pointer = &( (*bck)( dimX - 2, y, z ) );
      for ( x = dimX - 1; x--; )
      {
        for ( n = 0; n < 5; n++ )
        {
          off.item( 0 ) = abs( cnt5p.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt5p.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt5p.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt5p.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer--;
      }
    }
  }

  cout << "\nloop backward on slices\n" << flush;
  cout << "slice : " << setw(4) << dimZ - 1 << flush;
  for ( z = dimZ; z--; )
  { cout << "\b\b\b\b" << setw(4) << z << flush;
    for ( y = 1; y < dimY; y++ )
    {
      pointer = &( (*bck)( 0, y, z ) );
      for ( x = 0; x < dimX; x++ )
      {
        for ( n = 0; n < 9; n++ )
        {
          off.item( 0 ) = abs( cnt9m.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt9m.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt9m.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt9m.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer++;
      }

      pointer = &( (*bck)( 1, y, z ) );
      for ( x = 1; x < dimX; x++ )
      {
        for ( n = 0; n < 5; n++ )
        {
          off.item( 0 ) = abs( cnt5m.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt5m.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt5m.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt5m.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer++;
      }

      pointer = &( (*bck)( dimX - 2, y, z ) );
      for ( x = dimX - 1; x--; )
      {
        for ( n = 0; n < 5; n++ )
        {
          off.item( 0 ) = abs( cnt5p.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt5p.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt5p.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt5p.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer--;
      }
    }

    for ( y = dimY - 1; y--; )
    {
      pointer = &( (*bck)( 0, y, z ) );
      for ( x = 0; x < dimX; x++ )
      {
        for ( n = 0; n < 9; n++ )
        {
          off.item( 0 ) = abs( cnt9p.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt9p.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt9p.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt9p.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer++;
      }

      pointer = &( (*bck)( 1, y, z ) );
      for ( x = 1; x < dimX; x++ )
      {
        for ( n = 0; n < 5; n++ )
        {
          off.item( 0 ) = abs( cnt5m.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt5m.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt5m.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt5m.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer++;
      }

      pointer = &( (*bck)( dimX - 2, y, z ) );
      for ( x = dimX - 1; x--; )
      {
        for ( n = 0; n < 5; n++ )
        {
          off.item( 0 ) = abs( cnt5p.xyzOffset( n ).item( 0 ) );
          off.item( 1 ) = abs( cnt5p.xyzOffset( n ).item( 1 ) );
          off.item( 2 ) = abs( cnt5p.xyzOffset( n ).item( 2 ) );    
          if ( norm2( ( tp = *( pointer + cnt5p.offset( n ) ) + off ) ) < 
               norm2( *pointer ) ) 
            *pointer = tp;
        }
        pointer--;
      }
    }
  }
  cout << "\n";

  AimsData<float> distmap( dimX, dimY, dimZ, 1, 1 );
  distmap = 0.0;
  distmap.fillBorder( 0.0 );
  distmap.setSizeX( vol.sizeX() );
  distmap.setSizeY( vol.sizeY() );
  distmap.setSizeZ( vol.sizeZ() );
  for ( z = 0; z < dimZ; z++ )                              
    for ( y = 0; y < dimY; y++ )                              
      for ( x = 0; x < dimX; x++ )
        distmap( x, y, z ) = norm( (*bck)(x,y,z) );

  delete bck;

  return distmap;
}


AimsData<float> AimsSignedDanielssonDistanceMap(const AimsData<int16_t> &vol)
{
  ASSERT( vol.dimT()==1 );
  AimsData<float> 
  inside = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_INSIDE );
  AimsData<float>
  outside = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_OUTSIDE );

  AimsData<float> 
  res( vol.dimX(), vol.dimY(), vol.dimZ(), 1, vol.borderWidth() );

  for ( int z = 0; z < res.dimZ(); z++ )
    for ( int y = 0; y < res.dimY(); y++ )
      for ( int x = 0; x < res.dimX(); x++ )
        res( x, y, z ) = inside( x, y, z ) - outside( x, y, z );  

  return res;
}
