#include <aims/pyramid/subsamplingpyramid_d.h>
#include <aims/pyramid/movingaveragesubsampler.h>

using namespace carto;
using namespace std;

namespace aims {

#define instantiate_methods( S, T ) \
  template vector<VolumeRef<T> > SubSamplingPyramidBuilder<S>::execute( const VolumeRef<T> & );

#define instantiate_class( S ) \
  template class SubSamplingPyramidBuilder<S>; \
  instantiate_methods( S, uint8_t ) \
  instantiate_methods( S, int8_t ) \
  instantiate_methods( S, uint16_t ) \
  instantiate_methods( S, int16_t ) \
  instantiate_methods( S, uint32_t ) \
  instantiate_methods( S, int32_t ) \
  instantiate_methods( S, uint64_t ) \
  instantiate_methods( S, int64_t ) \
  instantiate_methods( S, float ) \
  instantiate_methods( S, double )

instantiate_class( MovingAverageSubSampler )
instantiate_class( MovingAverageCascadeSubSampler )

}
