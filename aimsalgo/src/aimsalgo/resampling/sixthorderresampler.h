
#ifndef AIMS_RESAMPLING_SIXTHORDERRESAMPLER_H
#define AIMS_RESAMPLING_SIXTHORDERRESAMPLER_H

#include <aims/resampling/splineresampler.h>

namespace aims
{

/** Volume resampler using sixth-order interpolation.


    The resampling API is described in the base classes, Resampler and
    SplineResampler.
 */
template <class T>
class SixthOrderResampler : public SplineResampler< T >
{
public:

  SixthOrderResampler();
  ~SixthOrderResampler();

  int getOrder() const CARTO_OVERRIDE;

protected:

  double getBSplineWeight( int i, double x ) const CARTO_OVERRIDE;
};

} // namespace aims

#endif
