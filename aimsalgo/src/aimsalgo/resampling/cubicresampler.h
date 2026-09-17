
#ifndef AIMS_RESAMPLING_CUBICRESAMPLER_H
#define AIMS_RESAMPLING_CUBICRESAMPLER_H

#include <aims/resampling/splineresampler.h>

namespace aims
{

/** Volume resampler using cubic interpolation.

    The resampling API is described in the base classes, Resampler and
    SplineResampler.
 */
template <class T>
class CubicResampler : public SplineResampler< T >
{
public:

  CubicResampler();
  ~CubicResampler();

  int getOrder() const CARTO_OVERRIDE;

protected:

  double getBSplineWeight( int i, double x ) const CARTO_OVERRIDE;
};

} // namespace aims

#endif
