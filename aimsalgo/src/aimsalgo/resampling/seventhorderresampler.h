
#ifndef AIMS_RESAMPLING_SEVENTHORDERRESAMPLER_H
#define AIMS_RESAMPLING_SEVENTHORDERRESAMPLER_H

#include <aims/resampling/splineresampler.h>

namespace aims
{

/** Volume resampler using seventh-order interpolation.


    The resampling API is described in the base classes, Resampler and
    SplineResampler.
 */
template <class T>
class SeventhOrderResampler : public SplineResampler< T >
{
public:

  SeventhOrderResampler();
  ~SeventhOrderResampler();

  int getOrder() const CARTO_OVERRIDE;

protected:

  double getBSplineWeight( int i, double x ) const CARTO_OVERRIDE;
};

} // namespace aims

#endif
