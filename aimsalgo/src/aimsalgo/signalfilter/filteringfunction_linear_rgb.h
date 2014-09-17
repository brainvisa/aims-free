/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_SIGNALFILTER_FILTERINGFUNCTION_LINEAR_RGB_H
#define AIMS_SIGNALFILTER_FILTERINGFUNCTION_LINEAR_RGB_H

//--- aims -------------------------------------------------------------------
#include <aims/rgb/rgb.h>
#include <aims/signalfilter/filteringfunction_linear.h>
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

#define AIMS_LIN_FILTERFUNC_MULTICHANNEL_DECLARE( NAME, T )                  \
  template <>                                                                \
  class NAME< T >: public LinFilterFunc< T >                                 \
  {                                                                          \
    public:                                                                  \
      NAME( carto::Object options = carto::none() );                         \
      virtual ~NAME();                                                       \
      virtual void setOptions( carto::Object );                              \
      virtual T execute( const carto::VolumeRef< T > & volume ) const ;      \
      virtual T execute( const carto::VolumeRef< T > & volume,               \
                         const StructuringElementRef & se ) const;           \
      virtual StructuringElementRef getStructuringElement(                   \
        const std::vector<double> & voxel_size = std::vector<double>(4,1.)   \
      ) const;                                                               \
      virtual NAME * clone() const;                                          \
    protected:                                                               \
      NAME< T::ChannelType > _m;                                             \
  }

namespace aims {
  AIMS_LIN_FILTERFUNC_MULTICHANNEL_DECLARE(GaborFilterFunc, AimsRGB);
  AIMS_LIN_FILTERFUNC_MULTICHANNEL_DECLARE(GaborFilterFunc, AimsRGBA);
} // namepsace aims

#endif
