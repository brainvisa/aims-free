/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_SIGNALFILTER_FILTERINGFUNCTION_ELEMENT_RGB_H
#define AIMS_SIGNALFILTER_FILTERINGFUNCTION_ELEMENT_RGB_H

//--- aims -------------------------------------------------------------------
#include <aims/rgb/rgb.h>
#include <aims/signalfilter/filteringfunction_element.h>
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

#define AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE( NAME, VOXELTYPE )      \
  template <>                                                                \
  class NAME< VOXELTYPE >: public ElementFilteringFunction< VOXELTYPE >      \
  {                                                                          \
    public:                                                                  \
      NAME( carto::Object options = carto::none() );                         \
      NAME( const NAME< VOXELTYPE > & other );                               \
      virtual ~NAME();                                                       \
      NAME< VOXELTYPE > & operator=( const NAME< VOXELTYPE > & other );      \
      virtual NAME * clone() const;                                          \
      virtual void setOptions( const carto::Object & options );              \
      virtual void updateOptions( const carto::Object & options );           \
      virtual VOXELTYPE execute( const carto::VolumeRef< VOXELTYPE > & in ) const;   \
      virtual VOXELTYPE execute( const carto::VolumeRef< VOXELTYPE > & in,           \
                         const carto::rc_ptr<StructuringElement> & se ) const; \
    protected:                                                               \
      NAME< VOXELTYPE::ChannelType > _m;                                     \
  }

namespace aims {
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MedianFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MedianFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMedianFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMedianFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MeanFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MeanFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMeanFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMeanFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MinFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MinFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MaxFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MaxFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MajorityFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(MajorityFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMajorityFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(NotNullMajorityFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(ExtremaDifferenceFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(ExtremaDifferenceFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(SumFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(SumFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(VarFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(VarFilterFunc, AimsRGBA);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(StDevFilterFunc, AimsRGB);
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_DECLARE(StDevFilterFunc, AimsRGBA);
} // namepsace aims

#endif
