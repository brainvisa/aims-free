#include <aims/signalfilter/filteringfunction_element_rgb_d.h>
#include <aims/signalfilter/filteringfunction_element_d.h>


#define AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE( NAME, VOXELTYPE )   \
  NAME< VOXELTYPE >::NAME( carto::Object options ):                          \
    ElementFilteringFunction< VOXELTYPE >(),                                 \
    _m(options) {}                                                           \
  NAME< VOXELTYPE >::NAME( const NAME< VOXELTYPE > & other ):                \
    ElementFilteringFunction< VOXELTYPE >(),                                 \
    _m(other._m) {}                                                          \
  NAME< VOXELTYPE >::~NAME() {}                                              \
  NAME< VOXELTYPE > & NAME< VOXELTYPE >::operator=( const NAME< VOXELTYPE > & other )  \
  {                                                                          \
    if(this != &other) {                                                     \
      _m = other._m;                                                         \
    }                                                                        \
    return *this;                                                            \
  }                                                                          \
  void NAME< VOXELTYPE >::setOptions( const carto::Object & options )        \
  {                                                                          \
    _m.setOptions( options );                                                \
  }                                                                          \
  void NAME< VOXELTYPE >::updateOptions( const carto::Object & options )     \
  {                                                                          \
    _m.updateOptions( options );                                             \
  }                                                                          \
  VOXELTYPE NAME< VOXELTYPE >::execute(                                      \
    const carto::VolumeRef< VOXELTYPE > & volume                             \
  ) const                                                                    \
  {                                                                          \
    return multichannelfiltervalues_nonlin< VOXELTYPE >( _m, volume );       \
  }                                                                          \
  VOXELTYPE NAME< VOXELTYPE >::execute(                                      \
    const carto::VolumeRef< VOXELTYPE > & volume,                            \
    const carto::rc_ptr<StructuringElement> & se                             \
  ) const                                                                    \
  {                                                                          \
    return multichannelfiltervalues_nonlin< VOXELTYPE >( _m, volume, se );   \
  }                                                                          \
  NAME< VOXELTYPE > * NAME< VOXELTYPE >::clone() const                       \
  {                                                                          \
    return new NAME< VOXELTYPE >(*this);                                     \
  }

namespace aims {

  template class ElementFilteringFunction<AimsRGB>;
  template class ElementFilteringFunction<AimsRGBA>;
  template class ElementFilteringFunctionFactory<AimsRGB>;
  template class ElementFilteringFunctionFactory<AimsRGBA>;

  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MedianFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MedianFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMedianFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMedianFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MeanFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MeanFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMeanFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMeanFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MinFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MinFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MaxFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MaxFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MajorityFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MajorityFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMajorityFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMajorityFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(ExtremaDifferenceFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(ExtremaDifferenceFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(SumFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(SumFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(VarFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(VarFilterFunc, AimsRGBA)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(StDevFilterFunc, AimsRGB)
  AIMS_ELEMENT_FILTERFUNC_MULTICHANNEL_SPECIALIZE(StDevFilterFunc, AimsRGBA)

} // namespace aims
