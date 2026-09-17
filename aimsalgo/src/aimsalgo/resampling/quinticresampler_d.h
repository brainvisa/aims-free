
#ifndef AIMS_RESAMPLING_QUINTICRESAMPLER_D_H
#define AIMS_RESAMPLING_QUINTICRESAMPLER_D_H

#include <aims/resampling/quinticresampler.h>

#include <cmath>

namespace aims
{

template < class T >
QuinticResampler< T >::QuinticResampler()
  : SplineResampler< T >()
{

  this->_poles.push_back( 0.5 * ( std::sqrt( 270.0 - std::sqrt( 70980.0 ) ) +
                          std::sqrt( 105.0 ) - 13.0 ) );
  this->_poles.push_back( 0.5 * ( std::sqrt( 270.0 + std::sqrt( 70980.0 ) ) -
                          std::sqrt( 105.0 ) - 13.0 ) );
  this->_gain = ( 1.0 - this->_poles[ 0 ] ) * ( 1.0 - this->_poles[ 1 ] );
  this->_gain *= this->_gain;

}


template < class T >
QuinticResampler< T >::~QuinticResampler()
{
}


template < class T >
int QuinticResampler< T >::getOrder() const
{

  return 5;

}


template < class T >
double QuinticResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = std::fabs( x - ( double )i );
  if ( x < 1.0 )
  {

    double f = x * x;
    return f * ( f * ( 0.25 - x * ( 1.0 / 12.0 ) ) - 0.5 ) + 0.55;

  }
  if ( x < 2.0 )
  {

    return x * ( x * ( x * ( x * ( x * ( 1.0 / 24.0 ) - 0.375 ) + 1.25 ) -
           1.75 ) + 0.625 ) + 0.425;

  }
  if ( x < 3.0 )
  {

    double f = 3.0 - x;
    x = f * f;
    return f * x * x * ( 1.0 / 120.0 );

  }
  return 0.0;

}

} // namespace aims

#endif
