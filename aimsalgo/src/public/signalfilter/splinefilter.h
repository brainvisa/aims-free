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

#ifndef AIMS_SIGNALFILTER_SPLINEFILTER_H
#define AIMS_SIGNALFILTER_SPLINEFILTER_H

//============================================================================
//
//                       INVERSE B-SPLINE FILTER
//
//============================================================================

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/iirfilter.h>          // aims::SymAllPoleIIRFilter
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//----------------------------------------------------------------------------

namespace aims {

  /// This filter uses an inverse B-Spline convolution function to transform
  /// a discrete signal to its spline representation. This is the underlying
  /// process used for spline interpolation.
  ///
  /// It actually performs a direct spline transformation
  /// (image space -> spline space)
  ///
  /// See:
  /// Unser, Aldroubi & Eden, "B-Spline Signal Processing"
  /// in IEEE Transactions on Signal Processing (1993)
  class InverseBSplineFilter: public SymAllPoleIIRFilter
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    /// Even though the spline order is templated, it can be overriden at
    /// run time.
    InverseBSplineFilter( unsigned order = 3 );
    /// The internal order of the copied filter is kept, meaning that the
    /// built filter's order might differ from N and ORDER
    InverseBSplineFilter( const InverseBSplineFilter & other );
    virtual ~InverseBSplineFilter();
    /// The internal order of the copied filter is kept, meaning that the
    /// built filter's order might differ from N and ORDER
    InverseBSplineFilter & operator=( const InverseBSplineFilter & other );
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    /// Get spline order
    const unsigned & order() const;
    /// Set spline order
    /// The gain and poles of the underlying IIR filter will be updated
    /// accordingly.
    void setOrder( const unsigned & order );
    /// @}

  protected:
    //------------------------------------------------------------------------
    /// Helpers
    //------------------------------------------------------------------------
    /// @{
    /// Set the poles of the underlying IIR filter that corresponds to the
    /// chosen order.
    void setPoles();
    /// @}

    unsigned _order;
  };

} // namespace aims

//============================================================================
//
//                       DIRECT B-SPLINE FILTER
//
//============================================================================

//--- aims -------------------------------------------------------------------
#include <aims/math/bspline.h>                        // aims::DiscreteBSpline
#include <aims/signalfilter/convolutionfilter.h>    // aims::ConvolutionFilter
#include <aims/vector/vector.h>                                    // Point4df
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//----------------------------------------------------------------------------

namespace aims {

  /// This filter uses a convolution with Spline basis functions. It can be
  /// used to interpolate a signal from its spline representation
  ///
  /// See:
  /// Unser, Aldroubi & Eden, "B-Spline Signal Processing"
  /// in IEEE Transactions on Signal Processing (1993)
  class DirectBSplineFilter: public ConvolutionFilter<DiscreteBSpline>
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    DirectBSplineFilter( float scale = 1., bool normalize = false,
                         unsigned order = 3 );
    DirectBSplineFilter( const Point4df & scale, bool normalize = false,
                         unsigned order = 3 );
    DirectBSplineFilter( const DirectBSplineFilter & other );
    virtual ~DirectBSplineFilter();
    DirectBSplineFilter & operator=( const DirectBSplineFilter & other );
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    /// Get spline order
    unsigned order() const;
    /// Get spline scaling coefficient
    Point4df scale() const;
    /// Get normalization mode
    bool normalize() const;
    /// Set spline order
    /// Updates underlyong basis functions
    void setOrder( const unsigned & order );
    /// Set scaling coefficient
    /// Updates underlyong basis functions
    void setScale( const float & scale );
    /// Set scaling coefficient
    /// Updates underlyong basis functions
    void setScale( const Point4df & scale );
    /// Set normalization mode
    void setNormalize( const bool & normalize = true );
    /// @}

    //------------------------------------------------------------------------
    /// Execution
    //------------------------------------------------------------------------
    /// Function-like operator that filters a given volume
    /// @{
    /// Write result in an existing volume.
    /// If the allocated size differs from the needed resulting size,
    /// the volume is reallocated.
    ///
    /// DirectBSplineFilter overrides the inherited version to add
    /// normalization.
    ///
    /// \note 1 copy
    /// \warning This call risks causing additional precision errors
    template <typename OUTP, typename INP>
    carto::VolumeRef<OUTP> execute( const carto::VolumeRef<INP> & in,
                                          carto::VolumeRef<OUTP> & out ) const;
    using ConvolutionFilter<DiscreteBSpline>::execute;
    /// @}

  protected:
    bool     _normalize;
    // Make some inherited functions private
    using ConvolutionFilter<DiscreteBSpline>::setBasisFunction;
  };

} // namespace aims

// Yael - April 15th, 2016
// I'm commenting out this class for now because its implementation is false.
// Because poles depend on the smoothing parameter, the stability of the
// filter cannot be known a priori. Several implementations are possible
// (using computations in the complex space, separating the filter into
// a sum of causal and anticausal filters, separating it into a
// convolution of causal and anticausal filters, ...? )
// but I don't have the time and do not need this class for now.
// I leave it in case it is of interest someday.
#if 0
//============================================================================
//
//                     INVERSE SMOOTHING B-SPLINE FILTER
//
//============================================================================

//--- aims -------------------------------------------------------------------
#include <aims/signalfilter/iirfilter.h>          // aims::SymAllPoleIIRFilter
//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volume.h>                       // carto::VolumeRef
//----------------------------------------------------------------------------

namespace aims {

  /// This filter uses an inverse B-Spline convolution function to transform
  /// a discrete signal to its spline representation. This is the underlying
  /// process used for smoothing spline approximation.
  ///
  /// Additionnaly, a parameter lambda forces the resulting spline to be
  /// smooth enough by weighting their r deriviative in a minimization scheme.
  /// (when the spline order n = 2*r-1)
  ///
  /// It actually performs a direct spline transformation
  /// (image space -(S)-> spline space)
  ///
  /// See:
  /// Unser, Aldroubi & Eden, "B-Spline Signal Processing"
  /// in IEEE Transactions on Signal Processing (1993)
  class InverseSmoothingSplineFilter: public IIRFilterBase
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    /// Even though the spline order is templated, it can be overriden at
    /// run time.
    InverseSmoothingSplineFilter( float lambda, unsigned n = N );
    InverseSmoothingSplineFilter( const InverseSmoothingSplineFilter & other );
    virtual ~InverseSmoothingSplineFilter();
    InverseSmoothingSplineFilter & operator=( const InverseSmoothingSplineFilter & other );
    /// @}

    //------------------------------------------------------------------------
    /// Execution
    //------------------------------------------------------------------------
    /// @{
    virtual void filter1d( const carto::VolumeRef<double> in,
                           carto::VolumeRef<double> out, int dir = -1 ) const;
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    /// Get spline order
    const unsigned & order() const;
    /// Get smoothin coefficient
    const float & lambda() const;
    /// Set spline order
    /// The gain and poles of the underlying IIR filter will be updated
    /// accordingly.
    void setOrder( unsigned n );
    /// Set smoothing coefficient
    /// The gain and poles of the underlying IIR filter will be updated
    /// accordingly.
    void setLambda( float l );
    /// @}

  protected:
    //------------------------------------------------------------------------
    /// Helpers
    //------------------------------------------------------------------------
    /// @{
    void setFilters();
    /// @}

    enum FilterType { Interpolation, SymRealPole, SymComplexPole };

    unsigned                    _order;
    float                       _lambda;
    FilterType                  _ftype;
    CausalAllPoleIIRFilter      _causal;
    AntiCausalAllPoleIIRFilter  _anticausal;
    SymAllPoleIIRFilter         _symmetric;
    InverseBSplineFilter        _interpolation;

    // friend class
    template <unsigned O> friend class InverseSmoothingSplineFilter;
  };

} // namespace aims
#endif

#endif // AIMS_SIGNALFILTER_SPLINEFILTER_H
