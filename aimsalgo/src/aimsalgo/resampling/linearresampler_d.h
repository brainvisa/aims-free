
#ifndef AIMS_RESAMPLING_LINEARRESAMPLER_D_H
#define AIMS_RESAMPLING_LINEARRESAMPLER_D_H

#include <aims/resampling/linearresampler.h>

#include <cartobase/type/converter.h>

#include <cmath>

namespace aims
{

template < class T >
LinearResampler< T >::LinearResampler()
  : SplineResampler< T >()
{
}


template < class T >
LinearResampler< T >::~LinearResampler()
{
}


template < class T >
int LinearResampler< T >::getOrder() const
{

  return 1;

}


template < class T >
void LinearResampler< T >::
doResampleChannel( const carto::Volume< ChannelType >& inVolume,
                   const soma::Transformation3d& invTransform3d,
                   const ChannelType& outBackground,
                   const Point3df& outLocation,
                   ChannelType& outValue, int t ) const
{

  const ChannelType *i = &inVolume.at( 0, 0, 0, t );
  const ChannelType *pi, *pj;

  Point3df normalizedInLocation;
  normalizedInLocation = invTransform3d.transform( outLocation );

  float xf = round(normalizedInLocation[0]);
  float yf = round(normalizedInLocation[1]);
  float zf = round(normalizedInLocation[2]);

  std::vector<int> dims = inVolume.getSize();
  int dimx = dims[0], dimy = dims[1], dimz = dims[2];

  // The test is done using floating-point so that NaN values are excluded (the
  // background value is returned if the transformation yields NaN)
  if ( ( xf >= 0 ) && ( xf < dimx ) &&
       ( yf >= 0 ) && ( yf < dimy ) &&
       ( zf >= 0 ) && ( zf < dimz ) )
  {

    double weightX0, weightY0, weightX1, weightY1;
    long foldX0, foldY0, foldX1, foldY1;
    double intensity, qi, qj;

    // first y contribution
    int y = static_cast<long>(floor(normalizedInLocation[1]));
    weightY0 = getBSplineWeight( y, normalizedInLocation[1] );
    foldY0 = (long)this->getFold( y, dims[1] ) * dims[0];

    // second y contribution
    ++ y;
    weightY1 = getBSplineWeight( y, normalizedInLocation[1] );
    foldY1 = (long)this->getFold( y, dimy ) * dimx;

    // first x contribution
    int x = static_cast<long>(floor(normalizedInLocation[0]));
    weightX0 = getBSplineWeight( x, normalizedInLocation[0] );
    foldX0 = (long)this->getFold( x, dimx );

    // second x contribution
    ++ x;
    weightX1 = getBSplineWeight( x, normalizedInLocation[0] );
    foldX1 = (long)this->getFold( x, dimx );

    if ( dimz == 1 )
    {

      //summing contributions
      pj = i;
      pi = pj + (size_t)(foldY0);
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + size_t(foldX1) );
      qj = weightY0 * qi;
      pi = pj + foldY1;
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      intensity = qj + weightY1 * qi;

    }
    else
    {

      // first z contribution
      int z = static_cast<long>(floor(normalizedInLocation[2]));
      pj = i + (size_t)(this->getFold( z, dimz )) * dimx *
           dimy;
      pi = pj + (size_t)(foldY0);
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      qj = weightY0 * qi;
      pi = pj + (size_t)foldY1;
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      qj += weightY1 * qi;
      intensity = getBSplineWeight( z, normalizedInLocation[2] ) * qj;

      // first z contribution
      ++ z;
      pj = i + (size_t)(this->getFold( z, dimz )) * dimx *
           dimy;
      pi = pj + (size_t)(foldY0);
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      qj = weightY0 * qi;
      pi = pj + (size_t)(foldY1);
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      qj += weightY1 * qi;
      intensity += getBSplineWeight( z, normalizedInLocation[2] ) * qj;
    }

    carto::RawConverter<double, ChannelType>().convert(intensity, outValue);

  }
  else
  {

    outValue = outBackground;

  }

}

template < class T >
double LinearResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = fabs( x - ( double )i );
  return ( x > 1.0 ) ? 0.0 : 1.0 - x;

}

} // namespace aims

#endif
