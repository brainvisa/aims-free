#include <aims/signalfilter/filteringfunction_nonlinear_rgb_d.h>
#include <aims/signalfilter/filteringfunction_nonlinear_d.h>


#define AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE( NAME, VOXELTYPE )    \
  NAME< VOXELTYPE > ::NAME( carto::Object options ): _m(options) {}          \
  NAME< VOXELTYPE >::~NAME() {}                                              \
  void NAME< VOXELTYPE >::setOptions( carto::Object options )                \
  {                                                                          \
    _m.setOptions( options );                                                \
  }                                                                          \
  VOXELTYPE NAME< VOXELTYPE >::execute(                                      \
    const carto::VolumeRef< VOXELTYPE > & volume                             \
  )                                                                          \
  {                                                                          \
    return multichannelfiltervalues_nonlin< VOXELTYPE >( _m, volume );       \
  }                                                                          \
  VOXELTYPE NAME< VOXELTYPE >::execute(                                      \
    const carto::VolumeRef< VOXELTYPE > & volume,                            \
    const StructuringElementRef & se                                         \
  )                                                                          \
  {                                                                          \
    return multichannelfiltervalues_nonlin< VOXELTYPE >( _m, volume, se );   \
  }                                                                          \
  NAME< VOXELTYPE > * NAME< VOXELTYPE >::clone() const                       \
  {                                                                          \
    return new NAME< VOXELTYPE > ();                                         \
  }

namespace aims {

  template class NonLinFilterFunc<AimsRGB>;
  template class NonLinFilterFunc<AimsRGBA>;
  template class NonLinFilterFuncFactory<AimsRGB>;
  template class NonLinFilterFuncFactory<AimsRGBA>;

  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MedianFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MedianFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMedianFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMedianFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MeanFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MeanFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMeanFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMeanFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MinFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MinFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MaxFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MaxFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MajorityFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(MajorityFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMajorityFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(NotNullMajorityFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(ExtremaDifferenceFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(ExtremaDifferenceFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(SumFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(SumFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(VarFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(VarFilterFunc, AimsRGBA)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(StDevFilterFunc, AimsRGB)
  AIMS_NONLIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(StDevFilterFunc, AimsRGBA)

} // namespace aims
