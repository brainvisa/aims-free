/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef AIMS_RESAMPLING_QUINTICRESAMPLER_D_H
#define AIMS_RESAMPLING_QUINTICRESAMPLER_D_H

#include <aims/resampling/quinticresampler.h>

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

#endif

