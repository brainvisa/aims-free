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

#include <aims/resampling/masklinresampler.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>

#include <aims/vector/vector.h>
#include <aims/transformation/affinetransformation3d.h>


namespace {
  template <class T>
  void _sliceResamp( const carto::Volume<T>& input_data,
                     carto::Volume<T> & resamp,
                     const T&background, T* out,
                     const Point3df& start,
                     int t, const carto::Volume<float>& Rinv );
} // anonymous namespace

namespace aims
{

template <class T>
void MaskLinearResampler<T>::
doResample( const carto::Volume<T> &input_data,
            const soma::Transformation3d &inverse_transform,
            const T &background,
            const Point3df& output_location,
            T &output_value, int t ) const
{
  const Point3df input_location = inverse_transform.transform(output_location);

  float xf = std::floor(input_location[0]);
  float yf = std::floor(input_location[1]);
  float zf = std::floor(input_location[2]);

  std::vector<int> dims = input_data.getSize();

  // The test is done using floating-point so that NaN values are excluded (the
  // background value is returned if the transformation yields NaN)
  if ( ( xf >= 0 ) && ( xf < dims[0] ) &&
       ( yf >= 0 ) && ( yf < dims[1] ) &&
       ( zf >= 0 ) && ( zf < dims[2] ) )
  {
    int x = static_cast<int>(xf);
    int y = static_cast<int>(yf);
    int z = static_cast<int>(zf);

    if (input_data.at(x,     y,     z, t) == -32768 ||
        input_data.at(x + 1, y,     z, t) == -32768 ||
        input_data.at(x,     y + 1, z, t) == -32768 ||
        input_data.at(x + 1, y + 1, z, t) == -32768 ||
        input_data.at(x,     y,     z + 1, t) == -32768 ||
        input_data.at(x + 1, y,     z + 1, t) == -32768 ||
        input_data.at(x,     y + 1, z + 1, t) == -32768 ||
        input_data.at(x + 1, y + 1, z + 1, t) == -32768)
    {
      output_value = -32768;
    } else {
      // Normal linear interpolation
      _linearresampler.resample_inv_to_vox(input_data, inverse_transform,
                                           background, output_location,
                                           output_value, t);
    }
  } else {
    output_value = background;
  }
}


template <class T>
void MaskLinearResampler<T>::resample( const carto::Volume< T >& input_data,
                                       const aims::AffineTransformation3d&
                                         motion,
                                       const T& background,
                                       carto::Volume< T > & thing,
                                       bool verbose ) const
{
  int dimt = thing.getSizeT();
  ASSERT( thing.getSizeT() == input_data.getSizeT()
          && thing.refVolume().isNull() );

  aims::AffineTransformation3d dirMotion = motion;
  std::unique_ptr<aims::AffineTransformation3d> invMotion = motion.inverse();
  
  // scale motion
  Point3df sizeFrom( input_data.getVoxelSize() );
  Point3df sizeTo( thing.getVoxelSize() );
  dirMotion.scale( sizeFrom, sizeTo );
  invMotion->scale( sizeTo, sizeFrom );

  //
  Point3df start;
  T* it = &thing.at( 0 );
  int dimz = thing.getSizeZ();

  if(verbose) {
    std::cout << "Resampling volume [  1] / slice [  1]" << std::flush;
  }

  for (int t = 1; t <= dimt; t++ )
  {
    start = invMotion->translation();
    for ( int s = 1; s <= dimz; s++ )
    {
      if(verbose) {
        std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
                  << std::setw( 3 ) << t << "] / slice ["
                  << std::setw( 3 ) << s << "]" << std::flush;
      }

      _sliceResamp( input_data, thing, background, it, start, t - 1,
                    invMotion->rotation() );
      it = &thing.at( 0, 0, s, t - 1 );
      start[ 0 ] += invMotion->rotation()( 0, 2 );
      start[ 1 ] += invMotion->rotation()( 1, 2 );
      start[ 2 ] += invMotion->rotation()( 2, 2 );
    }
  }
  if(verbose) {
    std::cout << std::endl;
  }
}

} // namespace aims


namespace {

template <class T>
void _sliceResamp( const carto::Volume<T>& input_data,
                   carto::Volume<T> & resamp,
                   const T& background, T* out,
                   const Point3df& start, int t,
                   const carto::Volume<float>& Rinv )
{
  static_assert(std::numeric_limits<int>::digits >= 31,
                "the optimizations need at least 32-bit int");

  const T* pOrig = &input_data.at( 0, 0, 0, t );

  int dimX1 = input_data.getSizeX();
  int dimX2 = resamp.getSizeX();
  int dimY2 = resamp.getSizeY();

  const int SIXTEEN = 16;
  const int TWO_THEN_SIXTEEN = 65536;

  int maxX = TWO_THEN_SIXTEEN * ( dimX1 - 1 );
  int maxY = TWO_THEN_SIXTEEN * ( input_data.getSizeY() - 1 );
  int maxZ = TWO_THEN_SIXTEEN * ( input_data.getSizeZ() - 1 );

  int xLinCurrent = ( int )( 65536.0 * start[ 0 ] );
  int yLinCurrent = ( int )( 65536.0 * start[ 1 ] );
  int zLinCurrent = ( int )( 65536.0 * start[ 2 ] );

  int xu = ( int )( 65536.0 * Rinv( 0, 0 ) );
  int yu = ( int )( 65536.0 * Rinv( 1, 0 ) );
  int zu = ( int )( 65536.0 * Rinv( 2, 0 ) );
  int xv = ( int )( 65536.0 * Rinv( 0, 1 ) );
  int yv = ( int )( 65536.0 * Rinv( 1, 1 ) );
  int zv = ( int )( 65536.0 * Rinv( 2, 1 ) );

  int xCurrent, yCurrent, zCurrent;
  int dx, dy, dz;
  const T* it;
  long   incr_vois[8] = {
    0,
    1,
    &input_data.at( 0, 1, 0 ) - &input_data.at( 0 ),
    &input_data.at( 0, 1, 0 ) - &input_data.at( 0 ),
    &input_data.at( 0, 0, 1 ) - &input_data.at( 0 ),
    &input_data.at( 1, 0, 1 ) - &input_data.at( 0 ),
    &input_data.at( 1, 1, 1 ) - &input_data.at( 0 ),
    &input_data.at( 0, 1, 1 ) - &input_data.at( 0 )
  };
float stock1, stock2, stock3;
  for ( int v = dimY2; v--; )
  {
    xCurrent = xLinCurrent;
    yCurrent = yLinCurrent;
    zCurrent = zLinCurrent;

    for ( int u = dimX2; u--; )
    {
      if ( xCurrent >= 0 && xCurrent < maxX &&
           yCurrent >= 0 && yCurrent < maxY &&
           zCurrent >= 0 && zCurrent < maxZ   )
      {
        dx = xCurrent & 0xffff;
        dy = yCurrent & 0xffff;
        dz = zCurrent & 0xffff;

        it = pOrig + ( zCurrent >> SIXTEEN ) * incr_vois[4]
                   + ( yCurrent >> SIXTEEN ) * dimX1
                   + ( xCurrent >> SIXTEEN );

	if (*(it + incr_vois[0]) == -32768 ||
	    *(it + incr_vois[1]) == -32768 ||
	    *(it + incr_vois[2]) == -32768 ||
	    *(it + incr_vois[3]) == -32768 ||
	    *(it + incr_vois[4]) == -32768 ||
	    *(it + incr_vois[5]) == -32768 ||
	    *(it + incr_vois[6]) == -32768 ||
	    *(it + incr_vois[7]) == -32768 )
	  {
	    *out++ = -32768;
	  }
	else
	  {

	  
	    stock1 = *it * ( TWO_THEN_SIXTEEN - dx );
	    stock1 += *(++it) * dx;
	    stock1 /= TWO_THEN_SIXTEEN;

	    it += dimX1;
	    stock2 = *it * dx;
	    stock2 += *(--it) * ( TWO_THEN_SIXTEEN - dx );
	    stock2 /= TWO_THEN_SIXTEEN;

	    stock1 *= ( TWO_THEN_SIXTEEN - dy );
	    stock1 += stock2 * dy;
	    stock1 /= TWO_THEN_SIXTEEN;

	    it += incr_vois[4] - dimX1;
	    stock2 = *it * ( TWO_THEN_SIXTEEN - dx );
	    stock2 += *(++it) * dx;
	    stock2 /= TWO_THEN_SIXTEEN;

	    it += dimX1;
	    stock3 = *it * dx;
	    stock3 += *(--it) * ( TWO_THEN_SIXTEEN - dx );
	    stock3 /= TWO_THEN_SIXTEEN;

	    stock2 *= TWO_THEN_SIXTEEN - dy;
	    stock2 += stock3 * dy;
	    stock2 /= TWO_THEN_SIXTEEN;

	    stock1 *= ( TWO_THEN_SIXTEEN - dz );
	    stock1 += stock2 * dz;

	    *out++ = static_cast<T>( stock1 / TWO_THEN_SIXTEEN );
	  }
      }
      else
        *out++ = background;

      xCurrent += xu;
      yCurrent += yu;
      zCurrent += zu;
    }
    xLinCurrent += xv;
    yLinCurrent += yv;
    zLinCurrent += zv;
  }
}

} // anonymous namespace
