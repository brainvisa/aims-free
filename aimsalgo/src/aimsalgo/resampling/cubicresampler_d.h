
#ifndef AIMS_RESAMPLING_CUBICRESAMPLER_D_H
#define AIMS_RESAMPLING_CUBICRESAMPLER_D_H

#include <aims/resampling/cubicresampler.h>

#include <cmath>

namespace aims
{

template < class T >
CubicResampler< T >::CubicResampler()
  : SplineResampler< T >()
{

  this->_poles.push_back( std::sqrt( 3.0 ) - 2.0 );
  this->_gain = 12.0 - std::sqrt( 108.0 );

}


template < class T >
CubicResampler< T >::~CubicResampler()
{
}


template < class T >
int CubicResampler< T >::getOrder() const
{

  return 3;

}


template < class T >
double CubicResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = std::fabs( x - ( double )i );
  if ( x < 1.0 )
  {

    return ( x * x * ( x - 2.0 ) * 3.0 + 4.0 ) * ( 1.0 / 6.0 );

  }
  if ( x < 2.0 )
  {

    x = 2.0 - x;
    return x * x * x * ( 1.0 / 6.0 );

  }
  return 0.0;

}

} // namespace aims

#endif
