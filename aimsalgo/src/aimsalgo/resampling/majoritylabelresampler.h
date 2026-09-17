
#ifndef AIMS_RESAMPLING_MAJORITYLABELRESAMPLER_H
#define AIMS_RESAMPLING_MAJORITYLABELRESAMPLER_H

#include <aims/resampling/splineresampler.h>

namespace aims
{

/** Volume resampler using linear (order 1) interpolation, then selects the majority
    label in the neighbouring values.

    The resampling API is described in the base classes, Resampler and
    SplineResampler.
 */
template <class T>
class MajorityLabelResampler : public SplineResampler< T >
{
public:

  MajorityLabelResampler();
  ~MajorityLabelResampler();

  int getOrder() const CARTO_OVERRIDE;

protected:
  typedef typename SplineResampler<T>::ChannelType ChannelType;

  void doResampleChannel( const carto::Volume< ChannelType >& inVolume,
                          const soma::Transformation3d& transform3d,
                          const ChannelType& outBackground,
                          const Point3df& outLocation,
                          ChannelType& outValue, int t ) const CARTO_OVERRIDE;
  double getBSplineWeight( int i, double x ) const CARTO_OVERRIDE;
};

} // namespace aims

#endif
