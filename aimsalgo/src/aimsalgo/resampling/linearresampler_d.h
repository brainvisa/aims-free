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


#ifndef AIMS_RESAMPLING_LINEARRESAMPLER_D_H
#define AIMS_RESAMPLING_LINEARRESAMPLER_D_H

#include <aims/resampling/linearresampler.h>

#include <cartobase/type/converter.h>

#include <cmath>

namespace aims
{

template < class T >
LinearResampler< T >::LinearResampler()
  : SplineResampler< T >()
{
}


template < class T >
LinearResampler< T >::~LinearResampler()
{
}


template < class T >
int LinearResampler< T >::getOrder() const
{

  return 1;

}


template < class T >
void LinearResampler< T >::
doResampleChannel( const AimsData< ChannelType >& inVolume,
                   const aims::Transformation3d& invTransform3d,
                   const ChannelType& outBackground,
                   const Point3df& outLocation,
                   ChannelType& outValue, int t ) const
{

  const ChannelType *i = &inVolume( 0, 0, 0, t );
  const ChannelType *pi, *pj;

  Point3df normalizedInLocation;
  normalizedInLocation = invTransform3d.transform( outLocation );

  float xf = round(normalizedInLocation[0]);
  float yf = round(normalizedInLocation[1]);
  float zf = round(normalizedInLocation[2]);

  // The test is done using floating-point so that NaN values are excluded (the
  // background value is returned if the transformation yields NaN)
  if ( ( xf >= 0 ) && ( xf < inVolume.dimX() ) &&
       ( yf >= 0 ) && ( yf < inVolume.dimY() ) &&
       ( zf >= 0 ) && ( zf < inVolume.dimZ() ) )
  {

    double weightX0, weightY0, weightX1, weightY1;
    long foldX0, foldY0, foldX1, foldY1;
    double intensity, qi, qj;

    // first y contribution
    int y = static_cast<long>(floor(normalizedInLocation[1]));
    weightY0 = getBSplineWeight( y, normalizedInLocation[1] );
    foldY0 = (long)this->getFold( y, inVolume.dimY() ) * inVolume.dimX();

    // second y contribution
    ++ y;
    weightY1 = getBSplineWeight( y, normalizedInLocation[1] );
    foldY1 = (long)this->getFold( y, inVolume.dimY() ) * inVolume.dimX();

    // first x contribution
    int x = static_cast<long>(floor(normalizedInLocation[0]));
    weightX0 = getBSplineWeight( x, normalizedInLocation[0] );
    foldX0 = (long)this->getFold( x, inVolume.dimX() );

    // second x contribution
    ++ x;
    weightX1 = getBSplineWeight( x, normalizedInLocation[0] );
    foldX1 = (long)this->getFold( x, inVolume.dimX() );

    if ( inVolume.dimZ() == 1 )
    {

      //summing contributions
      pj = i;
      pi = pj + (size_t)(foldY0);
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + size_t(foldX1) );
      qj = weightY0 * qi;
      pi = pj + foldY1;
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      intensity = qj + weightY1 * qi;

    }
    else
    {

      // first z contribution
      int z = static_cast<long>(floor(normalizedInLocation[2]));
      pj = i + (size_t)(this->getFold( z, inVolume.dimZ() )) * inVolume.dimX() *
           inVolume.dimY();
      pi = pj + (size_t)(foldY0);
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      qj = weightY0 * qi;
      pi = pj + (size_t)foldY1;
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      qj += weightY1 * qi;
      intensity = getBSplineWeight( z, normalizedInLocation[2] ) * qj;

      // first z contribution
      ++ z;
      pj = i + (size_t)(this->getFold( z, inVolume.dimZ() )) * inVolume.dimX() *
           inVolume.dimY();
      pi = pj + (size_t)(foldY0);
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      qj = weightY0 * qi;
      pi = pj + (size_t)(foldY1);
      qi = weightX0 * ( double )*( pi + (size_t)(foldX0) );
      qi += weightX1 * ( double )*( pi + (size_t)(foldX1) );
      qj += weightY1 * qi;
      intensity += getBSplineWeight( z, normalizedInLocation[2] ) * qj;
    }

    carto::RawConverter<double, ChannelType>().convert(intensity, outValue);

  }
  else
  {

    outValue = outBackground;

  }

}

template < class T >
double LinearResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = fabs( x - ( double )i );
  return ( x > 1.0 ) ? 0.0 : 1.0 - x;

}

} // namespace aims

#endif
