
#ifndef AIMS_RESAMPLING_QUADRATICRESAMPLER_D_H
#define AIMS_RESAMPLING_QUADRATICRESAMPLER_D_H

#include <aims/resampling/quadraticresampler.h>

#include <cmath>

namespace aims
{

template < class T >
QuadraticResampler< T >::QuadraticResampler()
  : SplineResampler< T >()
{

  this->_poles.push_back( std::sqrt( 8.0 ) - 3.0 );
  this->_gain = 24.0 - std::sqrt( 512.0 );

}


template < class T >
QuadraticResampler< T >::~QuadraticResampler()
{
}


template < class T >
int QuadraticResampler< T >::getOrder() const
{

  return 2;

}

template < class T >
double QuadraticResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = std::fabs( x - ( double )i );
  if ( x < 0.5 )
  {

    return 0.75 - x * x;

  }
  if ( x < 1.5 )
  {

    x = 1.5 - x;
    return 0.5 * x * x;

  }
  return 0.0;

}

} // namespace aims

#endif
