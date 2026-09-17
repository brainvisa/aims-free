// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <aims/roi/voxelSampling.h>
#include <sstream>
#include <stdlib.h>

using namespace std;
using namespace carto;

namespace aims
{

    ////////////////////
   //                //
  //  VoxelSampler  //
 //                //
////////////////////

//-----------------------------------------------------------------------------
VoxelSampler::~VoxelSampler() {}


    //////////////////////////
   //                      //
  //  SimpleVoxelSampler  //
 //                      //
//////////////////////////

//-----------------------------------------------------------------------------
SimpleVoxelSampler::~SimpleVoxelSampler() {}


//-----------------------------------------------------------------------------
SimpleVoxelSampler::SimpleVoxelSampler( int voxelSampling )
{
  if ( voxelSampling < 1 ) {
    throw runtime_error( "Less than one point per voxel requested" );
  }

  Point3d sampling;

  // Compute x, y and z sampling according to voxelSampling.
  // The real numer of points per voxel is not always exactly
  // voxelSampling.
  const int low = (int) floor( pow( voxelSampling, 1.0 / 3 ) );
  sampling[ 0 ] = sampling[ 1 ] = sampling[ 2 ] = low;
  int diff = abs( low * low * low - voxelSampling );
  if ( abs( low * low * ( low +1 )  - voxelSampling ) < diff ) {
    sampling[ 0 ] = low;
    sampling[ 1 ] = low;
    sampling[ 2 ] = low + 1;
    diff = abs( low * low * ( low + 1 ) - voxelSampling );
  }
  if ( abs( low * ( low + 1 ) * ( low + 1 )  - voxelSampling ) < diff ) {
    sampling[ 0 ] = low;
    sampling[ 1 ] = low + 1;
    sampling[ 2 ] = low + 1;
    diff = abs( low * ( low + 1 ) * ( low + 1 ) - voxelSampling );
  }
  if ( abs( ( low + 1 ) * ( low + 1 ) * ( low + 1 )  - voxelSampling ) 
       < diff ) {
    sampling[ 0 ] = low + 1;
    sampling[ 1 ] = low + 1;
    sampling[ 2 ] = low + 1;
  }
  
  offset[ 0 ] = 1.0 / sampling[ 0 ];
  offset[ 1 ] = 1.0 / sampling[ 1 ];
  offset[ 2 ] = 1.0 / sampling[ 2 ];
  restart();
}


//-----------------------------------------------------------------------------
void SimpleVoxelSampler::restart()
{
  point = offset;
  point /= 2.0f;
  point -= 0.5f;
}


//-----------------------------------------------------------------------------
bool SimpleVoxelSampler::isValid() const
{
  return point[ 2 ] < 0.5;
}


//-----------------------------------------------------------------------------
const Point3df &SimpleVoxelSampler::value() const
{
  return point;
}


//-----------------------------------------------------------------------------
void SimpleVoxelSampler::next()
{
  point[ 0 ] += offset[ 0 ];
  if ( point[ 0 ] >= 0.5 ) {
    point[ 0 ] = offset[ 0 ] / 2.0 - 0.5;
    point[ 1 ] += offset[ 1 ];
    if ( point[ 1 ] >= 0.5 ) {
      point[ 1 ] = offset[ 1 ] / 2.0 - 0.5;
      point[ 2 ] += offset[ 2 ];
    }
  }
}


//-----------------------------------------------------------------------------
string SimpleVoxelSampler::info() const
{
  const Point3d sampling( int32_t( 1.0 / offset[ 0 ] ),
                          int32_t( 1.0 / offset[ 1 ] ),
                          int32_t( 1.0 / offset[ 2 ] ) );
  ostringstream out;
  out << "Simple voxel sampler: " << sampling;
  string result = out.str();
  return result;
}


} // namespace aims
