
#ifndef AIMS_RESAMPLING_RESAMPLERFACTORY_H
#define AIMS_RESAMPLING_RESAMPLERFACTORY_H

#include <memory>

#include <aims/resampling/resampler.h>

namespace aims
{

  template <typename T>
  class ResamplerFactory
  {
  public:
    enum ResamplerType {
      NearestNeighbor = 0,
      Linear = 1,
      Quadratic = 2,
      Cubic = 3,
      Quartic = 4,
      Quintic = 5,
      SixthOrder = 6,
      SeventhOrder = 7,
      MajorityLabel = 101,
      Median = 201,
    };

    /** Instantiate a Resampler of the given order

    \param order order of interpolation: 0 is nearest neighbour (no
                 interpolation), 1 is linear, 3 is cubic, etc. up to 7th order.
    \return a new instance of the selected resampler type
    */
    static std::unique_ptr<Resampler<T> > getResampler( int order );

  };

} // namespace aims

#endif // !defined(AIMS_RESAMPLING_RESAMPLERFACTORY_H)
