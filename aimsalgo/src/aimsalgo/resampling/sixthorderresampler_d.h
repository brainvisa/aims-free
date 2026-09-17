
#ifndef AIMS_RESAMPLING_SIXTHORDERRESAMPLER_D_H
#define AIMS_RESAMPLING_SIXTHORDERRESAMPLER_D_H

#include <aims/resampling/sixthorderresampler.h>

#include <cmath>

namespace aims
{

template < class T >
SixthOrderResampler< T >::SixthOrderResampler()
  : SplineResampler< T >()
{

  this->_poles.push_back(
                 -0.488294589303044755130118038883789062112279161239377608394 );
  this->_poles.push_back(
              -0.081679271076237512597937765737059080653379610398148178525368 );
  this->_poles.push_back(
            -0.00141415180832581775108724397655859252786416905534669851652709 );
  this->_gain = 2.598975999348577818390170516255374207847876853191217652822;

}


template < class T >
SixthOrderResampler< T >::~SixthOrderResampler()
{
}


template < class T >
int SixthOrderResampler< T >::getOrder() const
{

  return 6;

}


template < class T >
double SixthOrderResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = std::fabs( x - ( double )i );
  if ( x < 0.5 )
  {

    x *= x;
    return x * ( x * ( 7.0 / 48.0 - x * ( 1.0 / 36.0 ) ) - 77.0 / 192.0 ) +
           5887.0 / 11520.0;

  }
  if ( x < 1.5 )
  {

    return x * ( x * ( x * ( x * ( x * ( x * ( 1.0 / 48.0 ) - 7.0 / 48.0 ) +
           0.328125 ) - 35.0 / 288.0 ) - 91.0 / 256.0 ) - 7.0 / 768.0 ) +
           7861.0 / 15360.0;

  }
  if ( x < 2.5 )
  {

    return x * ( x * ( x * ( x * ( x * ( 7.0 / 60.0 - x * ( 1.0 / 120.0 ) ) -
           0.65625 ) + 133.0 / 72.0 ) - 2.5703125 ) + 1267.0 / 960.0 ) +
           1379.0 / 7680.0;

  }
  if ( x < 3.5 )
  {

    x -= 3.5;
    x *= x * x;
    return x * x * ( 1.0 / 720.0 );

  }
  return 0.0;

}

} // namespace aims

#endif
