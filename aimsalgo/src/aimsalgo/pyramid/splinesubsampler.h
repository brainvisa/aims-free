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

#ifndef AIMS_PYRAMID_SPLINESUBSAMPLER_H
#define AIMS_PYRAMID_SPLINESUBSAMPLER_H

//============================================================================
//
//                       DIRECT B-SPLINE SUBSAMPLER
//
//============================================================================

//--- aims -------------------------------------------------------------------
#include <aims/math/bspline.h>                        // aims::DiscreteBSpline
#include <aims/pyramid/convolutionsubsampler.h>     // aims::ConvolutionFilter
#include <aims/vector/vector.h>                                    // Point4df
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//----------------------------------------------------------------------------

namespace aims {

  /// Class excuting a spline-based subsampling
  ///
  /// The behaviour is equivalent to:
  /// 1. The image is convoluted with a b-spline of order N, expanded by a
  ///    factor R.
  /// 2. The image is decimated by a factor R
  /// However, for performances, the convolution is only computed in voxels
  /// that will be kept after decimation.
  ///
  /// Note that in the case of R = 1, the output image will differ from the
  /// input image. The behaviour is thus different from a classical
  /// "binning" subsampling.
  ///
  /// By default, the kernel is normalized (it sums to 1). Normalization
  /// can be deactivated. When using spline subsampling in the frame
  /// of least square approximation, normalization should be deactivated.
  /// (see LeastSquareSpline)
  class DirectBSplineSubSampler: public ConvolutionSubSampler<DiscreteBSpline>
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    DirectBSplineSubSampler( unsigned factor = 2,
                             bool normalize = false,
                             unsigned spline_order = 3 );
    DirectBSplineSubSampler( const Point4du & factor,
                             bool normalize = false,
                             unsigned spline_order = 3 );
    DirectBSplineSubSampler( const DirectBSplineSubSampler & other );
    virtual ~DirectBSplineSubSampler();
    DirectBSplineSubSampler & operator=( const DirectBSplineSubSampler & other );
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    /// Get spline order
    unsigned order() const;
    /// Get normalization mode
    bool normalize() const;
    /// Set spline order
    /// Updates underlying basis functions
    void setOrder( unsigned n );
    /// Override ConclutionSubSampler version to update the underlying basis
    /// functions
    void setFactor( unsigned r );
    /// Override ConclutionSubSampler version to update the underlying basis
    /// functions
    void setFactor( const Point4du & r );
    /// Set normalization mode
    void setNormalize( bool normalize = true );
    /// @{

    //------------------------------------------------------------------------
    /// Execution
    //------------------------------------------------------------------------
    /// Function-like operator that filters a given volume
    /// @{
    /// Write result in an existing volume.
    /// If the allocated size differs from the needed resulting size,
    /// the volume is reallocated.
    ///
    /// DirectBSplineSubSampler overrides the inherited version to add
    /// normalization.
    ///
    /// \note 1 copy
    /// \warning This call risks causing additional precision errors
    template <typename OUT, typename IN>
    carto::VolumeRef<OUT> execute( const carto::VolumeRef<IN> & in,
                                         carto::VolumeRef<OUT> & out ) const;
    using ConvolutionSubSampler<DiscreteBSpline>::execute;
    /// @}

  protected:
    bool     _normalize;
    // utility functions
    void setFunctions( const Point4du & r, unsigned n );
    // Make some inherited functions private
    using ConvolutionSubSampler<DiscreteBSpline>::setBasisFunction;
  };

} // namespace aims

#endif // AIMS_PYRAMID_SPLINESUBSAMPLER_H
