/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_SIGNALFILTER_FILTERINGFUNCTION_NONLINEAR_RGB_H
#define AIMS_SIGNALFILTER_FILTERINGFUNCTION_NONLINEAR_RGB_H

//--- aims -------------------------------------------------------------------
#include <aims/rgb/rgb.h>
#include <aims/signalfilter/filteringfunction_nonlinear.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>
//--- forward declarations ---------------------------------------------------
namespace carto {
  template <typename T> class VolumeRef;
}
namespace aims {
  class StructuringElement;
}
//----------------------------------------------------------------------------

#define AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE( NAME, VOXELTYPE )       \
  template <>                                                                \
  class NAME< VOXELTYPE >: public NonLinFilterFunc< VOXELTYPE >              \
  {                                                                          \
    public:                                                                  \
      NAME( carto::Object options = carto::none() );                         \
      virtual ~NAME();                                                       \
      virtual void setOptions( carto::Object );                              \
      virtual VOXELTYPE execute( const carto::VolumeRef< VOXELTYPE > & volume ) const ; \
      virtual VOXELTYPE execute( const carto::VolumeRef< VOXELTYPE > & volume,          \
                         const StructuringElementRef & se ) const;           \
      virtual NAME * clone() const;                                          \
    protected:                                                               \
      NAME< VOXELTYPE::ChannelType > _m;                                     \
  }

namespace aims {
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MedianFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MedianFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMedianFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMedianFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MeanFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MeanFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMeanFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMeanFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MinFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MinFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MaxFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MaxFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MajorityFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(MajorityFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(ExtremaDifferenceFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(ExtremaDifferenceFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(SumFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(SumFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(VarFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(VarFilterFunc, AimsRGBA);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(StDevFilterFunc, AimsRGB);
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_DECLARE(StDevFilterFunc, AimsRGBA);
} // namepsace aims

#endif
