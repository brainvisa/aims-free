
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <aims/distancemap/mask.h>
#include <aims/math/mathelem.h>
//debug
#include <cstdlib>
#include <iostream>
using namespace aims;
using namespace std;

void ChamferMask::erase()
{
  if ( _offset ) delete [] _offset;
  if ( _distance ) delete [] _distance;
}



void ChamferMask::set( int dimx, int dimy, int dimz,
                       float sizex, float sizey, float sizez,
                       float mult_factor )
{
  erase();

  _length = dimx * dimy * dimz - 1;
  _offset = new Point3d[ _length ];
  _distance = new int[ _length ];

  //
  // order vector to optimize front propagation memory
  //
  int x=0, y=0, z=0, k=0;
  int max_sum = dimx / 2 + dimy / 2 + dimz / 2;

  for( int m = 1; m <= max_sum; m++ )
    for ( z = - dimz / 2; z < dimz / 2 + 1; z++ )
      for ( y = - dimy / 2; y < dimy / 2 + 1; y++ )
        for ( x = - dimx / 2; x < dimx / 2 + 1; x++ )
          if ( abs( int( x ) ) + abs( int( y ) ) + abs( int( z ) ) == m )
          {
            _offset[ k ] = Point3d( x, y, z );
            _distance[ k++ ] = int( mult_factor * 
                                     sqrt( sqr( x * sizex ) +
                                           sqr( y * sizey ) +
                                           sqr( z * sizez )  ) + 0.5 );
          }
}


void ChamferMask::set( const vector<int> & dims, const vector<float> & vsize,
                       float mult_factor )
{
  set( dims[0], dims[1], dims[2], vsize[0], vsize[1], vsize[2], mult_factor );
}


void ChamferMask::set( int dimx, int dimy, int dimz,
                       const vector<float> & vsize, float mult_factor )
{
  set( dimx, dimy, dimz, vsize[0], vsize[1], vsize[2], mult_factor );
}

