/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


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
