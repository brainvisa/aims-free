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
