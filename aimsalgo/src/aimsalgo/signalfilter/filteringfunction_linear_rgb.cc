#include <aims/signalfilter/filteringfunction_linear_rgb_d.h>
#include <aims/signalfilter/filteringfunction_linear_d.h>


#define AIMS_LIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE( NAME, VOXELTYPE )       \
  NAME< VOXELTYPE >::NAME( carto::Object options ):                          \
    LinearFilteringFunction<VOXELTYPE>(),                                    \
    _m(options) {}                                                           \
  NAME< VOXELTYPE >::NAME( const NAME< VOXELTYPE > & other ):                \
    LinearFilteringFunction<VOXELTYPE>(),                                    \
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
  const std::vector<int> & NAME< VOXELTYPE >::getAmplitude() const           \
  {                                                                          \
    return _m.getAmplitude();                                                \
  }                                                                          \
  VOXELTYPE NAME< VOXELTYPE >::execute(                                      \
    const carto::VolumeRef< VOXELTYPE > & volume                             \
  ) const                                                                    \
  {                                                                          \
    return multichannelfiltervalues_lin< VOXELTYPE >( _m, volume );          \
  }                                                                          \
  NAME< VOXELTYPE > * NAME< VOXELTYPE >::clone() const                       \
  {                                                                          \
    return new NAME< VOXELTYPE > (*this);                                    \
  }

namespace aims {

  template class LinearFilteringFunction<AimsRGB>;
  template class LinearFilteringFunction<AimsRGBA>;
  template class LinearFilteringFunctionFactory<AimsRGB>;
  template class LinearFilteringFunctionFactory<AimsRGBA>;

  AIMS_LIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(GaborFilterFunc, AimsRGB)
  AIMS_LIN_FILTERFUNC_MULTICHANNEL_SPECIALIZE(GaborFilterFunc, AimsRGBA)

} // namespace aims
