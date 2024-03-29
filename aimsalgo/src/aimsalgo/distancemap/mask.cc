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

