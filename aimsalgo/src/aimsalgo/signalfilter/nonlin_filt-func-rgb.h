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
#include <aims/math/mathelemrgb.h>
#include <aims/signalfilter/nonlin_filt-func.h>
#include <aims/utility/channel.h>
  
/// Templated function to process filter on multichannel data (AimsRGB, AimsRGBA, ...)
/// This function does not filter image but only apply filter on set of multichannel data,
/// commonly the neighborhood of a voxel.
/// \param filter function to use (must derive from \c NonLinFilterFunc)
/// \param data data to evaluate filter on
/// \return result of the filter on data
template< typename VoxelType >
inline VoxelType multichannelfiltervalues( NonLinFilterFunc< typename VoxelType::ChannelType > & f, 
                                            AimsData< VoxelType > &data )
{
  ChannelSelector< AimsData<VoxelType>, AimsData<typename VoxelType::ChannelType > > selector;
  VoxelType r;
  int32_t samples = DataTypeInfo<VoxelType>::samples();

  /* Split data and process filter on each channel */
  for ( int32_t s = 0; s < samples; s++ )
  {
    AimsData<typename VoxelType::ChannelType> c = selector.select( data, s );
    r[s] = f.doit( c );
  }

  return r;
}

// Specialization RGB for median filtering
template <>
class MedianFilterFunc< AimsRGB > : public NonLinFilterFunc< AimsRGB >
{
  public:
    MedianFilterFunc () { }
    virtual ~MedianFilterFunc () { }

    inline AimsRGB doit( AimsData<AimsRGB>& data ) const
    {
      MedianFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGB>(m, data);
    }
  
};

// Specialization RGBA for median filtering
template <>
class MedianFilterFunc< AimsRGBA > : public NonLinFilterFunc< AimsRGBA >
{
  public:
    MedianFilterFunc () { }
    virtual ~MedianFilterFunc () { }

    inline AimsRGBA doit( AimsData<AimsRGBA>& data ) const
    {
      MedianFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGBA>(m, data);
    }
};

// Specialization RGB for mean filtering
template <>
class MeanFilterFunc< AimsRGB > : public NonLinFilterFunc< AimsRGB >
{
  public:
    MeanFilterFunc () { }
    virtual ~MeanFilterFunc () { }

    inline AimsRGB doit( AimsData<AimsRGB>& data ) const
    {
      MeanFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGB>(m, data);
    }
  
};

// Specialization RGBA for mean filtering
template <>
class MeanFilterFunc< AimsRGBA > : public NonLinFilterFunc< AimsRGBA >
{
  public:
    MeanFilterFunc () { }
    virtual ~MeanFilterFunc () { }

    inline AimsRGBA doit( AimsData<AimsRGBA>& data ) const
    {
      MeanFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGBA>(m, data);
    }
  
};

// Specialization RGB for not null mean filtering
template <>
class NotNullMeanFilterFunc< AimsRGB > : public NonLinFilterFunc< AimsRGB >
{
  public:
    NotNullMeanFilterFunc () { }
    virtual ~NotNullMeanFilterFunc () { }

    inline AimsRGB doit( AimsData<AimsRGB>& data ) const
    {
      NotNullMeanFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGB>(m, data);
    }
};

// Specialization RGBA for not null mean filtering
template <>
class NotNullMeanFilterFunc< AimsRGBA > : public NonLinFilterFunc< AimsRGBA >
{
  public:
    NotNullMeanFilterFunc () { }
    virtual ~NotNullMeanFilterFunc () { }

    inline AimsRGBA doit( AimsData<AimsRGBA>& data ) const
    {
      NotNullMeanFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGBA>(m, data);
    }
};

// Specialization RGB for min filtering
template <>
class MinFilterFunc< AimsRGB > : public NonLinFilterFunc< AimsRGB >
{
  public:
    MinFilterFunc () { }
    virtual ~MinFilterFunc () { }

    inline AimsRGB doit( AimsData<AimsRGB>& data ) const
    {
      MinFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGB>(m, data);
    }
  
};

// Specialization RGBA for min filtering
template <>
class MinFilterFunc< AimsRGBA > : public NonLinFilterFunc< AimsRGBA >
{
  public:
    MinFilterFunc () { }
    virtual ~MinFilterFunc () { }

    inline AimsRGBA doit( AimsData<AimsRGBA>& data ) const
    {
      MinFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGBA>(m, data);
    }
};

// Specialization RGB for max filtering
template <>
class MaxFilterFunc< AimsRGB > : public NonLinFilterFunc< AimsRGB >
{
  public:
    MaxFilterFunc () { }
    virtual ~MaxFilterFunc () { }

    inline AimsRGB doit( AimsData<AimsRGB>& data ) const
    {
      MaxFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGB>(m, data);
    }
  
};

// Specialization RGBA for max filtering
template <>
class MaxFilterFunc< AimsRGBA > : public NonLinFilterFunc< AimsRGBA >
{
  public:
    MaxFilterFunc () { }
    virtual ~MaxFilterFunc () { }

    inline AimsRGBA doit( AimsData<AimsRGBA>& data ) const
    {
      MaxFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGBA>(m, data);
    }
};

// Specialization RGB for majority filtering
template <>
class MajorityFilterFunc< AimsRGB > : public NonLinFilterFunc< AimsRGB >
{
  public:
    MajorityFilterFunc () { }
    virtual ~MajorityFilterFunc () { }

    inline AimsRGB doit( AimsData<AimsRGB>& data ) const
    {
      MajorityFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGB>(m, data);
    }
  
};

// Specialization RGBA for majority filtering
template <>
class MajorityFilterFunc< AimsRGBA > : public NonLinFilterFunc< AimsRGBA >
{
  public:
    MajorityFilterFunc () { }
    virtual ~MajorityFilterFunc () { }

    inline AimsRGBA doit( AimsData<AimsRGBA>& data ) const
    {
      MajorityFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGBA>(m, data);
    }
};

// Specialization RGB for extrema difference filtering
template <>
class ExtremaDifferenceFilterFunc< AimsRGB > : public NonLinFilterFunc< AimsRGB >
{
  public:
    ExtremaDifferenceFilterFunc () { }
    virtual ~ExtremaDifferenceFilterFunc () { }

    inline AimsRGB doit( AimsData<AimsRGB>& data ) const
    {
      ExtremaDifferenceFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGB>(m, data);
    }
  
};

// Specialization RGBA for extrema difference filtering
template <>
class ExtremaDifferenceFilterFunc< AimsRGBA > : public NonLinFilterFunc< AimsRGBA >
{
  public:
    ExtremaDifferenceFilterFunc () { }
    virtual ~ExtremaDifferenceFilterFunc () { }

    inline AimsRGBA doit( AimsData<AimsRGBA>& data ) const
    {
      ExtremaDifferenceFilterFunc<uint8_t> m;
      return multichannelfiltervalues<AimsRGBA>(m, data);
    }
};

#endif

