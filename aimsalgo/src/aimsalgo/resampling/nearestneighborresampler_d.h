
#ifndef AIMS_RESAMPLING_NEARESTNEIGHBORRESAMPLER_D_H
#define AIMS_RESAMPLING_NEARESTNEIGHBORRESAMPLER_D_H

#include <aims/resampling/nearestneighborresampler.h>

#include <cmath>

using namespace std;

namespace aims
{

template <class T>
void
NearestNeighborResampler<T>::doResample( const carto::Volume< T > &inVolume,
                                         const soma::Transformation3d &invTransform3d,
                                         const T &outBackground,
                                         const Point3df &outLocation,
                                         T &outValue, int t ) const
{

  Point3df inLocation = invTransform3d.transform( outLocation );

  doResample(inVolume, inLocation, outBackground, outValue, t);
}

template <class T>
void
NearestNeighborResampler<T>::doResample( const carto::Volume< T > &inVolume,
                                         const Point3df &inLocation,
                                         const T &outBackground,
                                         T &outValue, int t ) const
{
  float xf = round(inLocation[0]);
  float yf = round(inLocation[1]);
  float zf = round(inLocation[2]);

  std::vector<int> dims = inVolume.getSize();

  // The test is done using floating-point so that NaN values are excluded (the
  // background value is returned if the transformation yields NaN)
  if ( ( xf >= 0 ) && ( xf < dims[0] ) &&
       ( yf >= 0 ) && ( yf < dims[1] ) &&
       ( zf >= 0 ) && ( zf < dims[2] ) )
  {
    int x = static_cast<int>(xf);
    int y = static_cast<int>(yf);
    int z = static_cast<int>(zf);
    outValue = inVolume.at( x, y, z, t );
  }
  else
  {
    outValue = outBackground;
  }

}

} // namespace aims

#endif
