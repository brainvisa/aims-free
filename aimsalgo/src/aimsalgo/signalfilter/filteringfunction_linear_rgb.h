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
  class NAME< T >: public LinearFilteringFunction< T >                       \
  {                                                                          \
    public:                                                                  \
      NAME( carto::Object options = carto::none() );                         \
      NAME( const NAME< T > & other );                                       \
      virtual ~NAME();                                                       \
      NAME<T> & operator=( const NAME< T > & other );                        \
      virtual NAME * clone() const;                                          \
      virtual void setOptions( const carto::Object & options );              \
      virtual void updateOptions( const carto::Object & options );           \
      const std::vector<int> & getAmplitude() const;                         \
      virtual T execute( const carto::VolumeRef<T> & in ) const;             \
    protected:                                                               \
      NAME< T::ChannelType > _m;                                             \
  }

namespace aims {
  AIMS_LIN_FILTERFUNC_MULTICHANNEL_DECLARE(GaborFilterFunc, AimsRGB);
  AIMS_LIN_FILTERFUNC_MULTICHANNEL_DECLARE(GaborFilterFunc, AimsRGBA);
} // namespace aims

#endif
