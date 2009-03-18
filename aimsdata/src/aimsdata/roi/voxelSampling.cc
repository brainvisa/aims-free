/* Copyright (c) 1995-2005 CEA
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
SimpleVoxelSampler::SimpleVoxelSampler( int voxelSampling = 1 )
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
