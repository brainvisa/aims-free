
#ifndef AIMS_RESAMPLING_MASKLINRESAMPLER_H
#define AIMS_RESAMPLING_MASKLINRESAMPLER_H

#include <aims/resampling/resampler.h>
#include <aims/resampling/linearresampler.h>

namespace aims
{

/** Volume resampler using linear (order 1) interpolation.

    \deprecated{This resampler shows unreliable behaviour: depending of the
    platform it does not always resample the last element along each axis
    correctly. Also, it uses some clever optimizations that do not check for
    overflow. If you need such a masked resampler, please consider contributing
    a fixed version.}

    This resampler will consider input voxels that are equal to -32768
    (hard-coded) as masked. The mask value (-32768) will always be returned for
    any interpolation involving a masked voxel.

    The default background value for this resampler is -32768 (same as the mask
    value).

    The resampling API is described in the base class, Resampler.
 */
template <class T>
class
#ifndef AIMS_MASKLINEARRESAMPLER_NO_DEPREC_WARNING
  __attribute__((__deprecated__))
#endif
  MaskLinearResampler : public Resampler< T >
{

  public:

    using Resampler<T>::defaultValue;
    using Resampler<T>::setDefaultValue;

    MaskLinearResampler() : Resampler< T >() {
      // Set background to the same value as the mask (which is hard-coded in
      // the resampler)
      setDefaultValue(-32768);
    };
    ~MaskLinearResampler() { }

    using Resampler<T>::resample;
    void resample( const carto::Volume< T >& input_data,
                   const aims::AffineTransformation3d& transform,
                   const T& background,
                   carto::Volume< T > & output_data,
                   bool verbose = false ) const CARTO_OVERRIDE;

  protected:

    static_assert(static_cast<T>(-32768) == -32768,
                  "type T cannot represent the mask value (-32768)");

    void
    doResample( const carto::Volume< T > &,
                const soma::Transformation3d &, const T &,
                const Point3df &, T &, int ) const CARTO_OVERRIDE;

    LinearResampler<T> _linearresampler;

};

} // namespace aims

#endif // !defined(AIMS_RESAMPLING_MASKLINRESAMPLER_H)
