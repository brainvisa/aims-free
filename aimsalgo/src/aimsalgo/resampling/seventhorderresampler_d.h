 
#ifndef AIMS_RESAMPLING_SEVENTHORDERRESAMPLER_D_H
#define AIMS_RESAMPLING_SEVENTHORDERRESAMPLER_D_H

#include <aims/resampling/seventhorderresampler.h>

#include <cmath>


namespace aims
{

template < class T >
SeventhOrderResampler< T >::SeventhOrderResampler()
  : SplineResampler< T >()
{

  this->_poles.push_back(
                -0.5352804307964381655424037816816460718339231523426924148812 );
  this->_poles.push_back(
              -0.122554615192326690515272264359357343605486549427295558490763 );
  this->_poles.push_back(
             -0.0091486948096082769285930216516478534156925639545994482648003 );
  this->_gain = 3.0248282036441843886795463832305782146916878615537002580987;

}


template < class T >
SeventhOrderResampler< T >::~SeventhOrderResampler()
{
}


template < class T >
int SeventhOrderResampler< T >::getOrder() const
{

  return 7;

}


template < class T >
double SeventhOrderResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = std::fabs( x - ( double )i );
  if ( x < 1.0 )
  {

    double f = x * x;
    return f * ( f * ( f * ( x * ( 1.0 / 144.0 ) - 1.0 / 36.0 ) + 1.0 / 9.0 ) -
           1.0 / 3.0 ) + 151.0 / 315.0;

  }
  if ( x < 2.0 )
  {

    return x * ( x * ( x * ( x * ( x * ( x * ( 0.05 - x * ( 1.0 / 240.0 ) ) -
           7.0 / 30.0 ) + 0.5 ) - 7.0 / 18.0 ) - 0.1 ) - 7.0 / 90.0 ) +
           103.0 / 210.0;

  }
  if ( x < 3.0 )
  {

    return x * ( x * ( x * ( x * ( x * ( x * ( x * ( 1.0 / 720.0 ) -
           1.0 / 36.0 ) + 7.0 / 30.0 ) - 19.0 / 18.0 ) + 49.0 / 18.0 ) -
           23.0 / 6.0 ) + 217.0 / 90.0 ) - 139.0 / 630.0;

  }
  if ( x < 4.0 )
  {

    double f = 4.0 - x;
    x = f * f * f;
    return x * x * f * ( 1.0 / 5040.0 );

  }
  return 0.0;

}

} // namespace aims

#endif
