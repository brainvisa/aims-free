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


#ifndef AIMS_RESAMPLING_MAJORITYLABELRESAMPLER_D_H
#define AIMS_RESAMPLING_MAJORITYLABELRESAMPLER_D_H

#include <aims/resampling/majoritylabelresampler.h>

#include <cartobase/type/converter.h>

#include <cmath>

namespace aims
{

template < class T >
MajorityLabelResampler< T >::MajorityLabelResampler()
  : SplineResampler< T >()
{
}


template < class T >
MajorityLabelResampler< T >::~MajorityLabelResampler()
{
}


template < class T >
int MajorityLabelResampler< T >::getOrder() const
{

  return 101;

}


template < class T >
void MajorityLabelResampler< T >::
doResampleChannel( const carto::Volume< ChannelType >& inVolume,
                   const soma::Transformation3d& invTransform3d,
                   const ChannelType& outBackground,
                   const Point3df& outLocation,
                   ChannelType& outValue, int t ) const
{

  const ChannelType *i = &inVolume.at( 0, 0, 0, t );
  const ChannelType *pi, *pj;

  Point3df normalizedInLocation;
  normalizedInLocation = invTransform3d.transform( outLocation );

  float xf = round(normalizedInLocation[0]);
  float yf = round(normalizedInLocation[1]);
  float zf = round(normalizedInLocation[2]);

  std::vector<int> dims = inVolume.getSize();
  int dimx = dims[0], dimy = dims[1], dimz = dims[2];

  // The test is done using floating-point so that NaN values are excluded (the
  // background value is returned if the transformation yields NaN)
  if ( ( xf >= 0 ) && ( xf < dimx ) &&
       ( yf >= 0 ) && ( yf < dimy ) &&
       ( zf >= 0 ) && ( zf < dimz ) )
  {

    double weightX0, weightY0, weightX1, weightY1;
    long foldX0, foldY0, foldX1, foldY1;
    double intensity, qi, qj;

    // first y contribution
    int y = static_cast<long>(floor(normalizedInLocation[1]));
    weightY0 = getBSplineWeight( y, normalizedInLocation[1] );
    foldY0 = (long)this->getFold( y, dims[1] ) * dims[0];

    // second y contribution
    ++ y;
    weightY1 = getBSplineWeight( y, normalizedInLocation[1] );
    foldY1 = (long)this->getFold( y, dimy ) * dimx;

    // first x contribution
    int x = static_cast<long>(floor(normalizedInLocation[0]));
    weightX0 = getBSplineWeight( x, normalizedInLocation[0] );
    foldX0 = (long)this->getFold( x, dimx );

    // second x contribution
    ++ x;
    weightX1 = getBSplineWeight( x, normalizedInLocation[0] );
    foldX1 = (long)this->getFold( x, dimx );

    std::map<ChannelType, unsigned long> values;

    if ( dimz == 1 )
    {

      //counting contributions
      pj = i;
      pi = pj + (size_t)(foldY0);
      if( weightY0 != 0 )
      {
        if( weightX0 != 0 )
          values[*( pi + (size_t)(foldX0) )]++;
        if( weightX1 != 0 )
          values[*( pi + size_t(foldX1) )]++;
      }
      if( weightY1 != 0 )
      {
        pi = pj + foldY1;
        if( weightX0 != 0 && weightY1 != 0 )
          values[*( pi + (size_t)(foldX0) )]++;
        if( weightX1 != 0 && weightY1 != 0 )
          values[*( pi + (size_t)(foldX1) )]++;
      }

    }
    else
    {

      // first z contribution
      int z = static_cast<long>(floor(normalizedInLocation[2]));
      if( getBSplineWeight( z, normalizedInLocation[2] ) != 0 )
      {
        pj = i + (size_t)(this->getFold( z, dimz )) * dimx * dimy;
        pi = pj + (size_t)(foldY0);

        if( weightY0 != 0 )
        {
          if( weightX0 != 0 )
            values[*( pi + (size_t)(foldX0) )]++;
          if( weightX1 != 0 )
            values[*( pi + size_t(foldX1) )]++;
        }
        if( weightY1 != 0 )
        {
          pi = pj + foldY1;
          if( weightX0 != 0 && weightY1 != 0 )
            values[*( pi + (size_t)(foldX0) )]++;
          if( weightX1 != 0 && weightY1 != 0 )
            values[*( pi + (size_t)(foldX1) )]++;
        }

      }
      // second z contribution
      ++ z;
      if( getBSplineWeight( z, normalizedInLocation[2] ) != 0 )
      {
        pj = i + (size_t)(this->getFold( z, dimz )) * dimx *
            dimy;
        pi = pj + (size_t)(foldY0);

        if( weightY0 != 0 )
        {
          if( weightX0 != 0 )
            values[*( pi + (size_t)(foldX0) )]++;
          if( weightX1 != 0 )
            values[*( pi + size_t(foldX1) )]++;
        }
        if( weightY1 != 0 )
        {
          pi = pj + foldY1;
          if( weightX0 != 0 && weightY1 != 0 )
            values[*( pi + (size_t)(foldX0) )]++;
          if( weightX1 != 0 && weightY1 != 0 )
            values[*( pi + (size_t)(foldX1) )]++;
        }
      }
    }

    unsigned long c = 0;
    ChannelType maxv = 0;
    typename std::map<ChannelType, unsigned long>::const_iterator
      iv, ev = values.end();

    for( iv=values.begin(); iv!=ev; ++iv )
      if( iv->second > c )
      {
        maxv = iv->first;
        c = iv->second;
      }
    if( c != 0 )
      intensity = maxv;
    else
      intensity = outBackground;

    carto::RawConverter<double, ChannelType>().convert(intensity, outValue);

  }
  else
  {

    outValue = outBackground;

  }

}

template < class T >
double MajorityLabelResampler< T >::getBSplineWeight( int i, double x ) const
{

  x = fabs( x - ( double )i );
  return ( x > 1.0 ) ? 0.0 : 1.0 - x;

}

} // namespace aims

#endif
