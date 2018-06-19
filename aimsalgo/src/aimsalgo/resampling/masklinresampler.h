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

template <class T>
class MaskLinearResampler : public Resampler< T >
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
    void resample( const AimsData< T >& input_data,
                   const aims::AffineTransformation3d& transform,
                   const T& background,
                   AimsData< T >& output_data,
                   bool verbose = false ) const CARTO_OVERRIDE;

  protected:

    static_assert(static_cast<T>(-32768) == -32768,
                  "type T cannot represent the mask value (-32768)");

    void _sliceResamp( const AimsData<T>& input_data, AimsData<T>& resamp, T* out,
                       const Point3df& start,
                       int t, const AimsData<float>& Rinv ) const;

    void
    doResample( const AimsData< T > &, const aims::Transformation3d &, const T &,
                const Point3df &, T &, int ) const CARTO_OVERRIDE;

    LinearResampler<T> _linearresampler;

};

#endif // !defined(AIMS_RESAMPLING_MASKLINRESAMPLER_H)
