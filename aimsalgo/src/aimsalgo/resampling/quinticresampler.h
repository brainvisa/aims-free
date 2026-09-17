
#ifndef AIMS_RESAMPLING_QUINTICRESAMPLER_H
#define AIMS_RESAMPLING_QUINTICRESAMPLER_H

#include <aims/resampling/splineresampler.h>

namespace aims
{

/** Volume resampler using quintic (order 5) interpolation.

    The resampling API is described in the base classes, Resampler and
    SplineResampler.
 */
template <class T>
class QuinticResampler : public SplineResampler< T >
{
public:

  QuinticResampler();
  ~QuinticResampler();

  int getOrder() const CARTO_OVERRIDE;

protected:

  double getBSplineWeight( int i, double x ) const CARTO_OVERRIDE;
};

} // namespace aims

#endif
