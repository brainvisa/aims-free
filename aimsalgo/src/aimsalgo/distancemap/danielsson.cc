
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <aims/distancemap/danielsson.h>
#include <aims/connectivity/connectivity.h>
#include <cartodata/volume/volume.h>
#include <aims/utility/clock.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;

VolumeRef<float> AimsDanielssonDistanceMap(
  const rc_ptr<Volume<int16_t> > & vol, DanielssonSide side)
{
  ASSERT( vol->getSizeT() == 1 );

  int x, y, z, n;
  Point3d off( 0, 0, 0 ), black( 0, 0, 0 ), white( 32767, 32767, 32767 );
  int dimX = vol->getSizeX();
  int dimY = vol->getSizeY();
  int dimZ = vol->getSizeZ();
  VolumeRef< Point3d > bck( dimX, dimY, dimZ, 1, 1 );

  switch ( side )
  {
    case AIMS_DANIELSSON_OUTSIDE: 
      cout << "\ndistance map outside object" << endl;
      *bck = white;
      bck->fillBorder( white );
      for ( z = 0; z < dimZ; z++ )                              
        for ( y = 0; y < dimY; y++ )                              
          for ( x = 0; x < dimX; x++ )
            (*bck)( x, y, z ) = ( vol->at( x, y, z ) != 0 ? black : white );
      break;
    case AIMS_DANIELSSON_INSIDE: 
      cout << "\ndistance map inside object" << endl;
      *bck = black;
      bck->fillBorder( white );
      for ( z = 0; z < dimZ; z++ )                              
        for ( y = 0; y < dimY; y++ )                              
          for ( x = 0; x < dimX; x++ )
            (*bck)( x, y, z ) = ( vol->at( x, y, z ) == 0 ? black : white );
      break;
  }

  Connectivity cnt9m( bck->getStrides()[1], bck->getStrides()[2],
                      Connectivity::CONNECTIVITY_9_XZ_Yminus);
  Connectivity cnt9p( bck->getStrides()[1], bck->getStrides()[2],
                      Connectivity::CONNECTIVITY_9_XZ_Yplus);
  Connectivity cnt5m( bck->getStrides()[1], bck->getStrides()[2],
                      Connectivity::CONNECTIVITY_5_XminusZ);
  Connectivity cnt5p( bck->getStrides()[1], bck->getStrides()[2],
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

  VolumeRef<float> distmap( dimX, dimY, dimZ, 1, 1 );
  distmap = 0.0;
  distmap.fillBorder( 0.0 );
  distmap.setVoxelSize( vol->getVoxelSize() );
  for ( z = 0; z < dimZ; z++ )                              
    for ( y = 0; y < dimY; y++ )                              
      for ( x = 0; x < dimX; x++ )
        distmap( x, y, z ) = norm( (*bck)(x,y,z) );

  return distmap;
}


VolumeRef<float> AimsSignedDanielssonDistanceMap(
  const rc_ptr<Volume<int16_t> > &vol )
{
  ASSERT( vol->getSizeT()==1 );
  VolumeRef<float>
  inside = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_INSIDE );
  VolumeRef<float>
  outside = AimsDanielssonDistanceMap( vol, AIMS_DANIELSSON_OUTSIDE );

  VolumeRef<float> res( vol->getSize(), vol->getBorders() );

  res = 0;
  res += inside;
  res -= outside;

  return res;
}
