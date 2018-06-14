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


#ifndef AIMS_RESAMPLING_CUBICRESAMPLER_D_H
#define AIMS_RESAMPLING_CUBICRESAMPLER_D_H

#include <cstdlib>
#include <aims/resampling/cubicresampler.h>
#include <aims/utility/channel.h>

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

// AimsRGB Specialization
AIMS_RESAMPLING_INSTANCIATE_MULTICHANNELRESAMPLER( CubicResampler, AimsRGB )
AIMS_RESAMPLING_INSTANCIATE_MULTICHANNELRESAMPLER( CubicResampler, AimsRGBA )

#endif
