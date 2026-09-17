
#ifndef AIMS_RESAMPLING_QUARTICRESAMPLER_D_H
#define AIMS_RESAMPLING_QUARTICRESAMPLER_D_H

#include <aims/resampling/quarticresampler.h>

#include <cmath>


namespace aims
{

template < class T >
QuarticResampler< T >::QuarticResampler()
  : SplineResampler< T >()
{

  this->_poles.push_back( std::sqrt( 664.0 - std::sqrt( 438976.0 ) ) +
                          std::sqrt( 304.0 ) - 19.0 );
  this->_poles.push_back( std::sqrt( 664.0 + std::sqrt( 438976.0 ) ) -
                          std::sqrt( 304.0 ) - 19.0 );
  this->_gain = ( 1.0 - this->_poles[ 0 ] ) * ( 1.0 - this->_poles[ 1 ] );
  this->_gain *= this->_gain;

}


template < class T >
QuarticResampler< T >::~QuarticResampler()
{
}


template < class T >
int QuarticResampler< T >::getOrder() const
{

  return 4;

}

template < class T >
double QuarticResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = std::fabs( x - ( double )i );
  if ( x < 0.5 )
  {

    x *= x;
    return x * ( x * 0.25 - 0.625 ) + 115.0 / 192.0;

  }
  if ( x < 1.5 )
  {

    return x * ( x * ( x * ( 5.0 / 6.0 - x * ( 1.0 / 6.0 ) ) - 1.25 ) +
           5.0 / 24.0 ) + 55.0 / 96.0;

  }
  if ( x < 2.5 )
  {

    x -= 2.5;
    x *= x;
    return x * x * ( 1.0 / 24.0 );

  }
  return 0.0;

}

} // namespace aims

#endif
