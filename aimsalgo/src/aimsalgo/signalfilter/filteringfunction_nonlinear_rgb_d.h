/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_SIGNALFILTER_FILTERINGFUNCTION_NONLINEAR_RGB_D_H
#define AIMS_SIGNALFILTER_FILTERINGFUNCTION_NONLINEAR_RGB_D_H

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/filteringfunction_nonlinear_rgb.h>
#include <aims/signalfilter/filteringfunction_nonlinear.h>      // aims::NonLinFilterFunc
#include <aims/utility/channel.h>                           // ChannelSelector
#include <aims/connectivity/structuring_element.h> // aims::StructuringElement
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//----------------------------------------------------------------------------


namespace aims {

  /// Templated function to process filter on multichannel data
  /// (AimsRGB, AimsRGBA, ...)
  ///
  /// This function does not filter image but only apply filter on set of
  /// multichannel data, commonly the neighborhood of a voxel.
  /// \param f       function to use (must derive from \c NonLinFilterFunc)
  /// \param volume  data to evaluate filter on
  /// \return        result of the filter on data
  template <typename VoxelType>
  inline VoxelType multichannelfiltervalues_nonlin(
    NonLinFilterFunc<typename VoxelType::ChannelType> & f,
    const carto::VolumeRef<VoxelType> &volume
  )
  {
    ChannelSelector< carto::VolumeRef<VoxelType>,
                     carto::VolumeRef<typename VoxelType::ChannelType>
                   > selector;
    VoxelType r;
    int32_t samples = DataTypeInfo<VoxelType>::samples();

    // Split data and process filter on each channel
    for( int32_t s = 0; s < samples; s++ )
    {
      carto::VolumeRef<typename VoxelType::ChannelType> c = selector.select( volume, s );
      r[s] = f.execute( c );
    }

    return r;
  }

  /// Templated function to process filter on multichannel data
  /// (AimsRGB, AimsRGBA, ...)
  ///
  /// This function does not filter image but only apply filter on set of
  /// multichannel data, commonly the neighborhood of a voxel.
  /// \param f      function to use (must derive from \c NonLinFilterFunc)
  /// \param volume data to evaluate filter on
  /// \param se     structuring element to evaluate filter on
  /// \return result of the filter on data
  template <typename VoxelType>
  inline VoxelType multichannelfiltervalues_nonlin(
    NonLinFilterFunc<typename VoxelType::ChannelType> & f,
    const carto::VolumeRef<VoxelType> &volume,
    const StructuringElementRef & se
  )
  {
    ChannelSelector< carto::VolumeRef<VoxelType>,
                     carto::VolumeRef<typename VoxelType::ChannelType>
                   > selector;
    VoxelType r;
    int32_t samples = DataTypeInfo<VoxelType>::samples();

    // Split data and process filter on each channel
    for( int32_t s = 0; s < samples; s++ )
    {
      carto::VolumeRef<typename VoxelType::ChannelType> c = selector.select( volume, s );
      r[s] = f.execute( c, se );
    }

    return r;
  }

} // namespace aims

#endif

