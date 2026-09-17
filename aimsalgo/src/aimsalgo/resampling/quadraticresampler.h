
#ifndef AIMS_RESAMPLING_QUADRATICRESAMPLER_H
#define AIMS_RESAMPLING_QUADRATICRESAMPLER_H

#include <aims/resampling/splineresampler.h>

namespace aims
{

/** Volume resampler using quadratic (order 2) interpolation.

    The resampling API is described in the base classes, Resampler and
    SplineResampler.
 */
template <class T>
class QuadraticResampler : public SplineResampler< T >
{
public:

  QuadraticResampler();
  ~QuadraticResampler();

  int getOrder() const CARTO_OVERRIDE;

protected:

  double getBSplineWeight( int i, double x ) const CARTO_OVERRIDE;
};

} // namespace aims

#endif
