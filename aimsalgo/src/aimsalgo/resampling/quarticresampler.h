
#ifndef AIMS_RESAMPLING_QUARTICRESAMPLER_H
#define AIMS_RESAMPLING_QUARTICRESAMPLER_H

#include <aims/resampling/splineresampler.h>

namespace aims
{

/** Volume resampler using quartic (order 4) interpolation.

    The resampling API is described in the base classes, Resampler and
    SplineResampler.
 */
template <class T>
class QuarticResampler : public SplineResampler< T >
{
public:

  QuarticResampler();
  ~QuarticResampler();

  int getOrder() const CARTO_OVERRIDE;

protected:

  double getBSplineWeight( int i, double x ) const CARTO_OVERRIDE;
};

} // namespace aims

#endif
