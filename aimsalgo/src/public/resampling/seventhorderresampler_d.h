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


#ifndef AIMS_RESAMPLING_SEVENTHORDERRESAMPLER_D_H
#define AIMS_RESAMPLING_SEVENTHORDERRESAMPLER_D_H

#include <cstdlib>
#include <aims/resampling/seventhorderresampler.h>

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

#endif

