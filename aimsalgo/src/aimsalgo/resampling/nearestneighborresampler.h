
#ifndef AIMS_RESAMPLING_NEARESTNEIGHBORRESAMPLER_H
#define AIMS_RESAMPLING_NEARESTNEIGHBORRESAMPLER_H

#include <aims/resampling/resampler.h>

namespace aims
{

/** Volume resampler using nearest-neighbour interpolation.

    The resampling API is described in the base class, Resampler.
 */
template <class T>
class NearestNeighborResampler : public Resampler< T >
{
  public:

    NearestNeighborResampler() : Resampler< T >() { };
    ~NearestNeighborResampler() { }

  protected:

    void
    doResample( const carto::Volume< T > &inVolume,
                const Point3df &inLocation,
                const T &outBackground,
                T &outValue, int t ) const;

    void
    doResample( const carto::Volume< T > &inVolume,
                const soma::Transformation3d &transform3d,
                const T &outBackground, const Point3df &outLocation,
                T &outValue, int t ) const CARTO_OVERRIDE;
};

} // namespace aims

#endif
