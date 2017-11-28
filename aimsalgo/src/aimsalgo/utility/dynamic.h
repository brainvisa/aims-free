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
#ifndef AIMSALGO_REGISTRATION_PREPROCESSING_H
#define AIMSALGO_REGISTRATION_PREPROCESSING_H

#include <cartodata/volume/volume.h>
#include <vector>

namespace aims {

  /// \function clipDynamic
  ///
  /// This method allows preprocessing of images to avoid squeezed histograms
  /// due to mask or outlier values.
  ///
  /// - If \c flatten_mask is set, the minimum value is replaced by the
  /// second minimum minus 1 (or epsilon for floats). This way, if the
  /// image was masked, the mask value is closer to the actual data range,
  /// else, the data is almost unchanged.
  ///
  /// - If \c pct_kept differs from 1, it is used to find a threshold under
  /// which at least that percentage of the histogram is kept. All values
  /// above are thresholded out and clipped.
  ///
  /// - Finally, if \c in place is true (when the input volume is non const),
  /// the result is returned in the input volume, else a new volume is
  /// allocated.

  template <typename T>
  carto::VolumeRef<T> clipDynamic( const carto::VolumeRef<T> & in,
                                   bool flatten_mask = true,
                                   float pct_kept = 1. );

  template <typename T>
  carto::VolumeRef<T> clipDynamic( carto::VolumeRef<T> & in,
                                   bool  flatten_mask = true,
                                   float pct_kept = 1.,
                                   bool  in_place = true );

#if 0 // TODO

  /// \function percentile
  ///
  /// - This method computes percentiles of the volume histogram.
  /// - Percentiles should be values in [0,100].
  /// - If histo_bons is fixed, percentiles are computed from the histogram,
  /// which is less precise but less memory demanding. Else, a sorted copy of
  /// the volume is used.
  /// - When the percentile does not fit exactly with a rank, the interpolated
  /// value is returned (or the nearest if interpolate = false)

  template <typename T>
  std::vector<T> percentile( const carto::VolumeRef<T> & in,
                             const std::vector<float> & pct,
                             bool interpolate = true,
                             int histo_bins = 0 );

  T percentile( const carto::VolumeRef<T> & in,
                const float & pct,
                bool interpolate = true,
                int histo_bins = 0 );

#endif

  /// \function clip
  ///
  /// Clip values outside [min, max]

  template <typename T>
  carto::VolumeRef<T> clip( const carto::VolumeRef<T> & in,
                            const T & min, const T & max );

  template <typename T>
  carto::VolumeRef<T> clip( carto::VolumeRef<T> & in,
                            const T & min, const T & max,
                            bool in_place = false );

} // namespace aims

#endif // AIMSALGO_REGISTRATION_PREPROCESSING_H
