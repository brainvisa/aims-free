/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMSALGO_SIGNALFILTER_NONLINFILT_FUNC_RGB_H
#define AIMSALGO_SIGNALFILTER_NONLINFILT_FUNC_RGB_H

#include <aims/data/data_g.h>
#include <aims/signalfilter/nonlin_filt-func.h>

#define AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE( NONLINFILT_FUNC, VOXELTYPE ) \
template <> \
class NONLINFILT_FUNC< VOXELTYPE > : public NonLinFilterFunc< VOXELTYPE > \
{ \
  public: \
    NONLINFILT_FUNC(); \
    virtual ~NONLINFILT_FUNC(); \
    virtual VOXELTYPE execute( const carto::VolumeRef<VOXELTYPE>& volume ) const; \
}; \


#define AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_SPECIALIZE( NONLINFILT_FUNC, VOXELTYPE ) \
NONLINFILT_FUNC<VOXELTYPE>::NONLINFILT_FUNC(): NonLinFilterFunc<VOXELTYPE>() {} \
\
NONLINFILT_FUNC<VOXELTYPE>::~NONLINFILT_FUNC() {} \
\
VOXELTYPE NONLINFILT_FUNC<VOXELTYPE>::execute( const carto::VolumeRef<VOXELTYPE>& volume ) const \
{ \
  NONLINFILT_FUNC<VOXELTYPE::ChannelType> m; \
  return multichannelfiltervalues<VOXELTYPE>(m, volume); \
} \


/// Templated function to process filter on multichannel data (AimsRGB, AimsRGBA, ...)
/// This function does not filter image but only apply filter on set of multichannel data,
/// commonly the neighborhood of a voxel.
/// \param filter function to use (must derive from \c NonLinFilterFunc)
/// \param data data to evaluate filter on
/// \return result of the filter on data
template< typename VoxelType >
inline VoxelType multichannelfiltervalues( NonLinFilterFunc< typename VoxelType::ChannelType > & f, 
                                           const carto::VolumeRef< VoxelType > &volume );

AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MedianFilterFunc, AimsRGB)
AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MedianFilterFunc, AimsRGBA)

AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MeanFilterFunc, AimsRGB)
AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MeanFilterFunc, AimsRGBA)

AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(NotNullMeanFilterFunc, AimsRGB)
AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(NotNullMeanFilterFunc, AimsRGBA)

AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MinFilterFunc, AimsRGB)
AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MinFilterFunc, AimsRGBA)

AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MaxFilterFunc, AimsRGB)
AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MaxFilterFunc, AimsRGBA)

AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MajorityFilterFunc, AimsRGB)
AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(MajorityFilterFunc, AimsRGBA)

AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(ExtremaDifferenceFilterFunc, AimsRGB)
AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(ExtremaDifferenceFilterFunc, AimsRGBA)

AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(SumFilterFunc, AimsRGB)
AIMSALGO_NONLINFILT_FUNC_MULTICHANNEL_DECLARE(SumFilterFunc, AimsRGBA)

#endif

