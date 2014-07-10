/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMSALGO_SIGNALFILTER_NONLINFILT_FUNC_RGB_D_H
#define AIMSALGO_SIGNALFILTER_NONLINFILT_FUNC_RGB_D_H

#include <aims/math/mathelemrgb.h>
#include <aims/signalfilter/nonlin_filt-func_d.h>
#include <aims/signalfilter/nonlin_filt-func-rgb.h>
#include <aims/utility/channel.h>

template< typename VoxelType >
inline VoxelType multichannelfiltervalues( NonLinFilterFunc< typename VoxelType::ChannelType > & f, 
                                           const carto::VolumeRef< VoxelType > &volume )
{
  ChannelSelector< carto::VolumeRef<VoxelType>, 
                   carto::VolumeRef<typename VoxelType::ChannelType > > selector;
  VoxelType r;
  int32_t samples = DataTypeInfo<VoxelType>::samples();

  /* Split data and process filter on each channel */
  for ( int32_t s = 0; s < samples; s++ )
  {
    carto::VolumeRef<typename VoxelType::ChannelType> c = selector.select( volume, s );
    r[s] = f.doit( c );
  }

  return r;
}

#endif

