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


#ifndef AIMS_RESAMPLING_SIXTHORDERRESAMPLER_D_H
#define AIMS_RESAMPLING_SIXTHORDERRESAMPLER_D_H

#include <aims/resampling/sixthorderresampler.h>

#include <cmath>

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

#endif
