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


#ifndef AIMS_RESAMPLING_SPLINERESAMPLER_D_H
#define AIMS_RESAMPLING_SPLINERESAMPLER_D_H

#include <aims/resampling/splineresampler.h>

#include <cmath>
#include <vector>
#include <iostream>

#include <aims/utility/converter_volume.h>

#define EPSILON   1.192092896e-7

template < class T >
SplineResampler< T >::SplineResampler()
  : Resampler<T>(), _lastvolume( 0 ), _lasttime( -1 )
{
}


template < class T >
void SplineResampler< T >::reset()
{

  _lasttime = -1;
  _lastvolume = 0;

}

template < class T >
AimsData<double> SplineResampler< T >::getSplineCoef( const AimsData< T >& inVolume,
						      int t,
						      bool verbose )
{
  updateParameters( inVolume, t, verbose);
  return _splineCoefficients;

}


template < class T >
void
SplineResampler< T >::doResample( const AimsData< T >& inVolume,
                                  const aims::Transformation3d& invTransform3d,
                                  const T& outBackground,
                                  const Point3df& outLocation,
                                  T& outValue, int ) const
{

  int order = this->getOrder();
  int half = order / 2;
  int width = order + 1;

  AimsData< double >::const_iterator s = &_splineCoefficients( 0, 0, 0 );
  AimsData< double >::const_iterator pi, pj;

  Point3df normalizedInLocation;
  normalizedInLocation = invTransform3d.transform( outLocation );

  normalizedInLocation[0] += 0.5;
  normalizedInLocation[1] += 0.5;
  normalizedInLocation[2] += 0.5;

  int x = ( int )normalizedInLocation[0];
  int y = ( int )normalizedInLocation[1];
  int z = ( int )normalizedInLocation[2];

  if ( ( normalizedInLocation[0] < 0.0 ) &&
       ( ( double )x != normalizedInLocation[0] ) )
    {

      -- x;

    }
  if ( ( normalizedInLocation[1] < 0.0 ) &&
       ( ( double )y != normalizedInLocation[1] ) )
    {

      -- y;

    }
  if ( ( normalizedInLocation[2] < 0.0 ) &&
       ( ( double )z != normalizedInLocation[2] ) )
    {

      -- z;

    }

  if ( ( x >= 0 ) && ( x < inVolume.dimX() ) &&
       ( y >= 0 ) && ( y < inVolume.dimY() ) &&
       ( z >= 0 ) && ( z < inVolume.dimZ() ) )
    {

      std::vector< double > weightX( width ), weightY( width ), weightZ( width);
      std::vector< int > foldX( width ), foldY( width ), foldZ( width );

      double intensity, qi, qj;
      int i, j, k;

      if ( inVolume.dimZ() == 1 )
        {

          weightZ[ 0 ] = 1.0;
          foldZ[ 0 ] = 0;

        }
      else
        {

          if ( order % 2 )
            {

              normalizedInLocation[2] -= 0.5;
              z = ( int )normalizedInLocation[2];
              if ( ( normalizedInLocation[2] < 0.0 ) &&
                   ( ( double )z != normalizedInLocation[2] ) )
                {

                  -- z;

                }
              z -= half;

            }
          else
            {

              z -= half;
              normalizedInLocation[2] -= 0.5;

            }
          for ( k = 0; k < width; k++ )
            {

              weightZ[ k ] = getBSplineWeight( z + k,
                                               normalizedInLocation[2] );
              foldZ[ k ] = getFold( z + k, inVolume.dimZ() ) *
                ( &_splineCoefficients( 0, 0, 1 )
                  - &_splineCoefficients( 0 ) );

            }

        }

      if ( order % 2 )
        {

          normalizedInLocation[1] -= 0.5;
          y = ( int )normalizedInLocation[1];
          if ( ( normalizedInLocation[1] < 0.0 ) &&
               ( ( double )y != normalizedInLocation[1] ) )
            {

              -- y;

            }
          y -= half;

        }
      else
        {

          y -= half;
          normalizedInLocation[1] -= 0.5;

        }
      for ( j = 0; j < width; j++ )
        {

          weightY[ j ] = getBSplineWeight( y + j, normalizedInLocation[1] );
          foldY[ j ] = getFold( y +j, inVolume.dimY() )
            * ( &_splineCoefficients( 0, 1 ) - &_splineCoefficients( 0 ) );

        }

      if ( order % 2 )
        {

          normalizedInLocation[0] -= 0.5;
          x = ( int )normalizedInLocation[0];
          if ( ( normalizedInLocation[0] < 0.0 ) &&
               ( ( double )x != normalizedInLocation[0] ) )
            {

              -- x;

            }
          x -= half;

        }
      else
        {

          x -= half;
          normalizedInLocation[0] -= 0.5;

        }
      for ( i = 0; i < width; i++ )
        {

          weightX[ i ] = getBSplineWeight( x + i, normalizedInLocation[0] );
          foldX[ i ] = getFold( x + i, inVolume.dimX() );

        }

      intensity = 0.0;
      for ( k = 0; k < ( ( inVolume.dimZ() == 1 ) ? 1 : width ); k++ )
        {

          pj = s + foldZ[ k ];
          qj = 0.0;
          for ( j = 0; j < width; j++ )
            {

              pi = pj + foldY[ j ];
              qi = 0.0;
              for ( i = 0; i < width; i++ )
                {

                  qi += weightX[ i ] * ( double )*( pi + foldX[ i ] );

                }
              qj += weightY[ j ] * qi;

            }
          intensity += weightZ[ k ] * qj;

        }
      outValue = ( T )intensity;

    }
  else
    {

      outValue = outBackground;

    }

}


template < class T >
void
SplineResampler< T >::updateParameters( const AimsData< T >& inVolume,
                                        int t, bool verbose )
{

  if ( ( &inVolume != _lastvolume || t != _lasttime )
       && this->getOrder() > 1 )
    {

      if ( verbose )
        {

          std::cout << "[ updating spline coefficients : " << std::flush;

        }

      int inSizeX = inVolume.dimX();
      int inSizeY = inVolume.dimY();
      int inSizeZ = inVolume.dimZ();

      carto::Converter< AimsData< T >, AimsData< double > > converter;
      _splineCoefficients = AimsData<double>( inSizeX, inSizeY, inSizeZ );
      _splineCoefficients.setSizeXYZT( inVolume.sizeX(), inVolume.sizeY(),
                                       inVolume.sizeZ(), inVolume.sizeT() );
      if( t > 0 )
        {
          AimsData<T> tmpvol( inSizeX, inSizeY, inSizeZ );
          T		*p = &tmpvol( 0 );
          const T	*q;
          int x, y, z;
          for ( z = 0; z < inSizeZ; z++ )
            for ( y = 0; y < inSizeY; y++ )
              {
                q = &inVolume( 0, y, z, t );
                for ( x = 0; x < inSizeX; ++x, ++p, ++q )
                  *p = *q;
              }
          converter.convert( tmpvol, _splineCoefficients );
        }
      else
        converter.convert( inVolume, _splineCoefficients );

      if ( inSizeX > 1 )
        {

          std::vector< double > data( inSizeX );
          int x, y, z;
          for ( z = 0; z < inSizeZ; z++ )
            {

              for ( y = 0; y < inSizeY; y++ )
                {

                  for ( x = 0; x < inSizeX; x++ )
                    {

                      data[ x ] = ( double )_splineCoefficients( x, y, z );

                    }
                  iirConvolveMirror( data );
                  for ( x = 0; x < inSizeX; x++ )
                    {

                      _splineCoefficients( x, y, z ) = data[ x ];

                    }

                }

            }

        }

      if ( inSizeY > 1 )
        {

          std::vector< double > data( inSizeY );
          int x, y, z;
          for ( z = 0; z < inSizeZ; z++ )
            {

              for ( x = 0; x < inSizeX; x++ )
                {

                  for ( y = 0; y < inSizeY; y++ )
                    {

                      data[ y ] = ( double )_splineCoefficients( x, y, z );

                    }
                  iirConvolveMirror( data );
                  for ( y = 0; y < inSizeY; y++ )
                    {

                      _splineCoefficients( x, y, z ) = data[ y ];

                    }

                }

            }

        }

      if ( inSizeZ > 1 )
        {

          std::vector< double > data( inSizeZ );
          int x, y, z;
          for ( y = 0; y < inSizeY; y++ )
            {

              for ( x = 0; x < inSizeX; x++ )
                {

                  for ( z = 0; z < inSizeZ; z++ )
                    {

                      data[ z ] = ( double )_splineCoefficients( x, y, z );

                    }
                  iirConvolveMirror( data );
                  for ( z = 0; z < inSizeZ; z++ )
                    {

                      _splineCoefficients( x, y, z ) = data[ z ];

                    }

                }

            }

        }

      _lastvolume = &inVolume;
      _lasttime = t;

      if ( verbose )
        {

          std::cout << "done ]" << std::flush;

        }

    }

}


template < class T >
void
SplineResampler< T >::iirConvolveMirror( std::vector< double >& data ) const
{

  double tolerance = std::log10( EPSILON );

  std::vector< double >::iterator d = data.begin(), de = data.end();
  while ( d != de )
  {

    // Note: Relatively to this article:
    // - Unser et al. IEEE Transactions on Signal Processing 1993
    // _gain = c0 * PROD(poles)
    *d *= _gain;
    ++ d;

  }

  int size = ( int )data.size();
  if ( size == 1 )
  {

    return;

  }

  int size2 = 2 * ( size - 1 );

  std::vector< double >::const_reverse_iterator p = _poles.rbegin(),
                                                pe = _poles.rend();
  double* dataBegin = &data[ 0 ];
  double* dataEnd = dataBegin + data.size();
  double* ptrD1;
  double* ptrD2;

  int j, k;
  double x0;
  while ( p != pe )
  {

    j = ( int )std::ceil( tolerance / std::log10( std::fabs( *p ) ) );
    k = j - size2 * ( j / size2 );
    j -= k;
    if ( k < size )
    {

      ptrD1 = dataBegin + k;
      x0 = *ptrD1;

    }
    else
    {

      k = size2 - k;
      ptrD1 = dataBegin + k;
      x0 = *ptrD1;
      while ( ++ptrD1 < dataEnd )
      {

        x0 = *p * x0 + *ptrD1;

      }
      -- ptrD1;

    }
    while ( --ptrD1 >= dataBegin )
    {

      x0 = *p * x0 + *ptrD1;

    }
    while ( j > 0 )
    {

      ++ ptrD1;
      while ( ++ptrD1 < dataEnd )
      {

        x0 = *p * x0 + *ptrD1;

      }
      -- ptrD1;
      while ( --ptrD1 >= dataBegin )
      {

        x0 = *p * x0 + *ptrD1;

      }
      j -= size2;

    }
    ptrD2 = ptrD1 ++;
    *ptrD1++ = x0;
    x0 = *( ptrD2++ + size );
    while ( ptrD1 < dataEnd )
    {

      *ptrD1++ += *ptrD2++ * *p;

    }
    *ptrD2 = ( 2.0 * *ptrD2 - x0 ) / ( 1.0 - *p * *p );
    -- ptrD1;
    while ( --ptrD2 >= dataBegin )
    {

      *ptrD2 += *ptrD1-- * *p;

    }

    ++ p ;

  }

}


template < class T >
int SplineResampler< T >::getFold( int i, int size ) const
{
  return aims::mirrorCoeff(i, size);
}


#undef EPSILON

#endif
