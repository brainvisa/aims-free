/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


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
