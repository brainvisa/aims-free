#include <aims/pyramid/splinesubsampler_d.h>

using namespace carto;

namespace aims {

//============================================================================
//
//                      CONVOLUTION SUBSAMPLER
//
//============================================================================

template class ConvolutionSubSampler<DiscreteBSpline>;

#define conv_instantiate_methods_2( IN, OUT ) \
  template VolumeRef<OUT> ConvolutionSubSampler<DiscreteBSpline>::execute( const VolumeRef<IN> & ) const; \
  template VolumeRef<OUT> ConvolutionSubSampler<DiscreteBSpline>::execute( const VolumeRef<IN> &, VolumeRef<OUT> & ) const;

#define conv_instantiate_methods( T ) \
  conv_instantiate_methods_2( T, uint8_t )  \
  conv_instantiate_methods_2( T, int8_t )   \
  conv_instantiate_methods_2( T, uint16_t ) \
  conv_instantiate_methods_2( T, int16_t )  \
  conv_instantiate_methods_2( T, uint32_t ) \
  conv_instantiate_methods_2( T, int32_t )  \
  conv_instantiate_methods_2( T, uint64_t ) \
  conv_instantiate_methods_2( T, int64_t )  \
  conv_instantiate_methods_2( T, float )    \
  conv_instantiate_methods_2( T, double )

conv_instantiate_methods( uint8_t )
conv_instantiate_methods( int8_t )
conv_instantiate_methods( uint16_t )
conv_instantiate_methods( int16_t )
conv_instantiate_methods( uint32_t )
conv_instantiate_methods( int32_t )
conv_instantiate_methods( uint64_t )
conv_instantiate_methods( int64_t )
conv_instantiate_methods( float )
conv_instantiate_methods( double )

//============================================================================
//
//                      SPLINE SUBSAMPLER
//
//============================================================================

class DirectBSplineSubSampler;

#define instantiate_methods_2( IN, OUT ) \
  template VolumeRef<OUT> DirectBSplineSubSampler::execute( const VolumeRef<IN> &, VolumeRef<OUT> & ) const;

#define instantiate_methods( IN ) \
  instantiate_methods_2( IN, int8_t )      \
  instantiate_methods_2( IN, uint8_t )     \
  instantiate_methods_2( IN, int16_t )     \
  instantiate_methods_2( IN, uint16_t )    \
  instantiate_methods_2( IN, int32_t )     \
  instantiate_methods_2( IN, uint32_t )    \
  instantiate_methods_2( IN, int64_t )     \
  instantiate_methods_2( IN, uint64_t )    \
  instantiate_methods_2( IN, float )       \
  instantiate_methods_2( IN, double )

instantiate_methods( int8_t )   \
instantiate_methods( uint8_t )  \
instantiate_methods( int16_t )  \
instantiate_methods( uint16_t ) \
instantiate_methods( int32_t )  \
instantiate_methods( uint32_t ) \
instantiate_methods( int64_t )  \
instantiate_methods( uint64_t ) \
instantiate_methods( float )    \
instantiate_methods( double )

}
