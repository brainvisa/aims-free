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

#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

#include <cartobase/type/converter.h>
#include <aims/utility/channel.h>
#include <aims/utility/converter_volume.h>

#define EPSILON   1.192092896e-7


namespace aims
{

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
carto::VolumeRef<double> SplineResampler< T >::getSplineCoef(
  const carto::Volume< T >& inVolume,
  int t,
  bool verbose )
{
  updateParameters( inVolume, t, verbose);
  return _splineCoefficients;

}

template < class T >
void
SplineResampler< T >::doResampleChannel(
  const carto::Volume< ChannelType >& inVolume,
  const soma::Transformation3d& invTransform3d,
  const ChannelType& outBackground,
  const Point3df& outLocation,
  ChannelType& outValue, int t ) const
{
  assert(t == _lasttime);
  (void)( t ); // compilation warning...

  int order = this->getOrder();
  int half = order / 2;
  int width = order + 1;

  double *s = &_splineCoefficients( 0, 0, 0 );
  double *pi, *pj;

  Point3df normalizedInLocation;
  normalizedInLocation = invTransform3d.transform( outLocation );

  float xf = round(normalizedInLocation[0]);
  float yf = round(normalizedInLocation[1]);
  float zf = round(normalizedInLocation[2]);

  int dimx = inVolume.getSizeX();
  int dimy = inVolume.getSizeY();
  int dimz = inVolume.getSizeZ();

  // The test is done using floating-point so that NaN values are excluded (the
  // background value is returned if the transformation yields NaN)
  if ( ( xf >= 0 ) && ( xf < dimx ) &&
       ( yf >= 0 ) && ( yf < dimy ) &&
       ( zf >= 0 ) && ( zf < dimz ) )
    {
      int x = static_cast<int>(xf);
      int y = static_cast<int>(yf);
      int z = static_cast<int>(zf);

      std::vector< double > weightX( width ), weightY( width ), weightZ( width);
      std::vector< int > foldX( width ), foldY( width ), foldZ( width );

      double intensity, qi, qj;
      int i, j, k;

      if (dimz == 1 )
        {

          weightZ[ 0 ] = 1.0;
          foldZ[ 0 ] = 0;

        }
      else
        {

          if ( order % 2 )
            {

              z = static_cast<int>(floor(normalizedInLocation[2]));
              z -= half;

            }
          else
            {

              z -= half;

            }
          for ( k = 0; k < width; k++ )
            {

              weightZ[ k ] = getBSplineWeight( z + k,
                                               normalizedInLocation[2] );
              foldZ[ k ] = getFold( z + k, dimz ) *
                ( &_splineCoefficients( 0, 0, 1 )
                  - &_splineCoefficients( 0 ) );

            }

        }

      if ( order % 2 )
        {

          y = static_cast<int>(floor(normalizedInLocation[1]));
          y -= half;

        }
      else
        {

          y -= half;

        }
      for ( j = 0; j < width; j++ )
        {

          weightY[ j ] = getBSplineWeight( y + j, normalizedInLocation[1] );
          foldY[ j ] = getFold( y +j, dimy )
            * ( &_splineCoefficients( 0, 1 ) - &_splineCoefficients( 0 ) );

        }

      if ( order % 2 )
        {

          x = static_cast<int>(floor(normalizedInLocation[0]));
          x -= half;

        }
      else
        {

          x -= half;

        }
      for ( i = 0; i < width; i++ )
        {

          weightX[ i ] = getBSplineWeight( x + i, normalizedInLocation[0] );
          foldX[ i ] = getFold( x + i, dimx );

        }

      intensity = 0.0;
      for ( k = 0; k < ( ( dimz == 1 ) ? 1 : width ); k++ )
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

      // Clip the values to the output datatype: such out-of-range should be
      // rare, but they can occur due to overshoot with spline order >= 2.
      if(intensity < std::numeric_limits<ChannelType>::lowest())
        intensity = std::numeric_limits<ChannelType>::lowest();
      else if(intensity > std::numeric_limits<ChannelType>::max())
        intensity = std::numeric_limits<ChannelType>::max();
      else
        carto::RawConverter<double, ChannelType>().convert(intensity,
                                                           outValue);

    }
  else
    {

      outValue = outBackground;

    }

}


template < class T >
void
SplineResampler< T >::
updateParametersChannel( const carto::Volume< ChannelType >& inVolume,
                         int t, bool verbose ) const
{

  if ( ( &inVolume != _lastvolume || t != _lasttime )
       && this->getOrder() > 1 )
    {

      if ( verbose )
        {

          std::cout << "[ updating spline coefficients : " << std::flush;

        }

      std::vector<int> dims = inVolume.getSize();
      int inSizeX = dims[0];
      int inSizeY = dims[1];
      int inSizeZ = dims[2];

      carto::Converter< carto::VolumeRef< ChannelType >,
                        carto::VolumeRef< double > > converter;
      _splineCoefficients = carto::VolumeRef<double>( inSizeX, inSizeY,
                                                      inSizeZ );
      _splineCoefficients->header().setProperty( "voxel_size",
                                                 inVolume.getVoxelSize() );
      if( t > 0 )
        {
          carto::VolumeRef<ChannelType> tmpvol( inSizeX, inSizeY, inSizeZ );
          ChannelType          *p = &tmpvol( 0 );
          const ChannelType    *q;
          int x, y, z;
          for ( z = 0; z < inSizeZ; z++ )
            for ( y = 0; y < inSizeY; y++ )
              {
                q = &inVolume.at( 0, y, z, t );
                for ( x = 0; x < inSizeX; ++x, ++p, ++q )
                  *p = *q;
              }
          converter.convert( tmpvol, _splineCoefficients );
        }
      else
      {
        // converter needs a VolumeRefm inVolume is a Volume
        carto::rc_ptr<carto::Volume<ChannelType> > inref;
        inref.reset( const_cast<carto::Volume<ChannelType> *>( &inVolume ) );
        converter.convert( inref, _splineCoefficients );
        inref.release();
      }

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

  double tolerance = log10( EPSILON );

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

    j = ( int )ceil( tolerance / log10( std::fabs( *p ) ) );
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


template <typename T>
void SplineResampler<T>::
resample_channel_inv_to_vox(const carto::Volume< ChannelType >& inVolume,
                            const soma::Transformation3d&
                              inverse_transform_to_vox,
                            const ChannelType& outBackground,
                            carto::Volume< ChannelType > & outVolume,
                            bool verbose) const
{
  Point3df outResolution( outVolume.getVoxelSize() );

  int outSizeX = outVolume.getSizeX();
  int outSizeY = outVolume.getSizeY();
  int outSizeZ = outVolume.getSizeZ();
  int outSizeT = outVolume.getSizeT();
  if( outSizeT > inVolume.getSizeT() )
    outSizeT = inVolume.getSizeT();

  ChannelType* o;

  aims::Progression progress(0, static_cast<size_t>(outSizeX)
                             * outSizeY * outSizeZ * outSizeT);
  Point3df outLoc;
  int x, y, z, t;
  for ( t = 0; t < outSizeT; t++ )
    {
      updateParametersChannel( inVolume, t, verbose );
      outLoc = Point3df( 0.0, 0.0, 0.0 );

      for ( z = 0; z < outSizeZ; z++ )
        {

          for ( y = 0; y < outSizeY; y++ )
            {
              o = &outVolume.at( 0, y, z, t );

              for ( x = 0; x < outSizeX; x++ )
                {

                  doResampleChannel(
                    inVolume, inverse_transform_to_vox, outBackground,
                    outLoc, *o, t );
                  ++ o;
                  ++progress;
                  outLoc[0] += outResolution[0];

                }
              outLoc[1] += outResolution[1];
              outLoc[0] = 0.0;

              if(verbose) {
                progress.print();
              }
            }
          outLoc[2] += outResolution[2];
          outLoc[1] = 0.0;

        }

    }
}

// Single-channel version: just forwards calls to the spline resampler
template<typename T>
struct MultiChannelResamplerSwitch<false, T>
{

  static void doResample( const SplineResampler<T>* spline_resampler,
                          const carto::Volume< T > &input_data,
                          const soma::Transformation3d &inverse_transform,
                          const T &background,
                          const Point3df &output_location,
                          T &output_value,
                          int timestep )
  {
    spline_resampler->doResampleChannel(input_data,
                                        inverse_transform,
                                        background,
                                        output_location,
                                        output_value,
                                        timestep);
  }

  static void resample_inv_to_vox( const SplineResampler<T>* spline_resampler,
                                   const carto::Volume< T >& input_data,
                                   const soma::Transformation3d&
                                    inverse_transform_to_vox,
                                   const T& background,
                                   carto::Volume< T > & output_data,
                                   bool verbose = false )
  {
    // Call the base class version, which just calls doResample for every voxel
    spline_resampler->Resampler<T>::resample_inv_to_vox(
      input_data, inverse_transform_to_vox,
      background, output_data, verbose);
  }

  static void updateParameters( const SplineResampler<T>* spline_resampler,
                                const carto::Volume< T >& inVolume,
                                int t, bool verbose )
  {
    spline_resampler->updateParametersChannel(inVolume, t, verbose);
  }

};


// Multi-channel version: iterate over all channels
template<typename T>
struct MultiChannelResamplerSwitch<true, T>
{
  typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;

  static void doResample( const SplineResampler<T>* spline_resampler,
                          const carto::Volume< T > &input_data,
                          const soma::Transformation3d &inverse_transform,
                          const T &background,
                          const Point3df &output_location,
                          T &output_value,
                          int timestep )
  {
    ChannelSelector< carto::VolumeRef<T>, carto::VolumeRef<ChannelType> >
      selector;
    carto::VolumeRef<ChannelType> input_channel;

    for (unsigned int channel = 0;
         channel < carto::DataTypeTraits<T>::channelcount;
         channel++)
    {
      // build a temporary VolumeRef
      carto::VolumeRef<T> inref;
      inref.reset( const_cast<carto::Volume<T> *>( &input_data ) );
      input_channel = selector.select( inref, channel );
      inref.reset();

      spline_resampler->updateParametersChannel(input_channel,
                                                timestep, carto::verbose);
      return spline_resampler->doResampleChannel(input_channel,
                                                 inverse_transform,
                                                 background[channel],
                                                 output_location,
                                                 output_value[channel],
                                                 timestep);
    }
  }

  static void resample_inv_to_vox( const SplineResampler<T>* spline_resampler,
                                   const carto::Volume< T >& input_data,
                                   const soma::Transformation3d&
                                    inverse_transform_to_vox,
                                   const T& background,
                                   carto::Volume< T > & output_data,
                                   bool verbose = false )
  {
    ChannelSelector< carto::VolumeRef<T>, carto::VolumeRef<ChannelType> >
      selector;

    int dimX = output_data.getSizeX();
    int dimY = output_data.getSizeY();
    int dimZ = output_data.getSizeZ();
    int dimT = output_data.getSizeT();
    std::vector<float> vs = output_data.getVoxelSize();

    for (unsigned int channel = 0;
         channel < carto::DataTypeTraits<T>::channelcount;
         channel++)
    {
      carto::VolumeRef<ChannelType> input_channel;
      carto::VolumeRef<ChannelType> output_channel( dimX, dimY, dimZ, dimT );

      output_channel.header().setProperty( "voxel_size", vs );

      output_channel.copyHeaderFrom( output_data.header() );

      // build a temporary VolumeRef
      carto::VolumeRef<T> inref;
      inref.reset( const_cast<carto::Volume<T> *>( &input_data ) );
      /* We split the data and process resampling on each component */
      input_channel = selector.select( inref, channel );
      inref.release();

      spline_resampler->resample_channel_inv_to_vox(
        *input_channel, inverse_transform_to_vox, background[ channel ],
        *output_channel, verbose );

      // build a temporary VolumeRef
      carto::VolumeRef<T> outref;
      outref.reset( &output_data );
      selector.set( outref, channel, output_channel );
      outref.release();
    }

  }

  static void updateParameters( const SplineResampler<T>* /*spline_resampler*/,
                                const carto::Volume< T >& /*inVolume*/,
                                int /*t*/, bool /*verbose*/ )
  {
    // do nothing, updateParametersChannel is called as needed by the methods
    // above
  }

};


template < class T >
void
SplineResampler< T >::updateParameters( const carto::Volume< T >& inVolume,
                                        int t, bool verbose ) const
{
  typedef MultiChannelResamplerSwitch<
    carto::DataTypeTraits<T>::is_multichannel, T> Switch;

  Switch::updateParameters(this,
                           inVolume, t, verbose);
}

template < class T >
void SplineResampler<T>::
resample_inv_to_vox( const carto::Volume< T >& input_data,
                     const soma::Transformation3d& inverse_transform_to_vox,
                     const T& background,
                     carto::Volume< T > & output_data,
                     bool verbose ) const
{
  typedef MultiChannelResamplerSwitch<
    carto::DataTypeTraits<T>::is_multichannel, T> Switch;

  Switch::resample_inv_to_vox(this,
                              input_data, inverse_transform_to_vox,
                              background, output_data, verbose);
}


template < class T >
void SplineResampler<T>::
doResample( const carto::Volume< T > &input_data,
            const soma::Transformation3d &inverse_transform,
            const T &background,
            const Point3df &output_location,
            T &output_value,
            int timestep ) const
{
  typedef MultiChannelResamplerSwitch<
    carto::DataTypeTraits<T>::is_multichannel, T> Switch;

  Switch::doResample(this, input_data, inverse_transform, background,
                     output_location, output_value, timestep);
}

} // namespace aims

#endif // !defined( AIMS_RESAMPLING_SPLINERESAMPLER_D_H )
