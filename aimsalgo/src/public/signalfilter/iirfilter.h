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

#ifndef AIMS_SIGNALFILTER_IIRFILTER_H
#define AIMS_SIGNALFILTER_IIRFILTER_H

#include <aims/signalfilter/types.h>
#include <cartodata/volume/volume.h>
#include <vector>

namespace aims {

  //==========================================================================
  //
  //   BASE CLASS FOR INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================
  /// Base class for separable infinite impulse response filters.
  ///
  /// This class implements all shared functionalities such as recursive calls
  /// to 1D filters, axis of filtering, border strategy, verbosity.
  /// Actual IIR implementations should derive from this class and implement
  /// the filter1d() method.
  ///
  /// \see CausalIIRFilter, CausalAllPoleIIRFilter, CausalSinglePoleIIRFilter,
  ///      AntiCausalWarperIIRFilter, SymAllPoleIIRFilter
  class IIRFilterBase
  {
  public:

    //------------------------------------------------------------------------
    /// Constructor / Destructor / Copy
    //------------------------------------------------------------------------
    /// @{
    IIRFilterBase();
    IIRFilterBase( const IIRFilterBase & other );
    virtual ~IIRFilterBase();
    IIRFilterBase & operator=( const IIRFilterBase & other );
    /// @}

    //------------------------------------------------------------------------
    /// Execution
    //------------------------------------------------------------------------
    /// Apply the filtering process to a volume.
    /// Under certain conditions, it can be done in place (the input volume
    /// will contain the result). Else an output volume will be created.
    /// @{
    /// \note 1 copy (+ 1 if the result is assigned to a different volume type)
    template <typename T>
    carto::VolumeRef<double> execute( const carto::VolumeRef<T> & in ) const;
    /// \note
    /// - if inplace : 0 copy
    /// - else       : 1 copy (+ 1 if the result is assigned to a different volume type)
    carto::VolumeRef<double> execute( carto::VolumeRef<double> & in, bool inplace = false ) const;
    /// Actual 1d filter that should be implemented by derived classes.
    virtual void filter1d( const carto::VolumeRef<double> in,
                           carto::VolumeRef<double> out, int dir = -1 ) const = 0;
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    /// Axis slong which the fitlering is performed. By default, filtering
    /// is performed along the X, Y and Z axes.
    virtual const std::vector<bool> & directions() const;
    virtual void setDirections( const std::vector<bool> & dir );
    virtual void setDirections( bool dirx, bool diry, bool dirz, bool dirt );
    virtual void setDirectionX( bool dirx ); ///< default: true
    virtual void setDirectionY( bool diry ); ///< default: true
    virtual void setDirectionZ( bool dirz ); ///< default: true
    virtual void setDirectionT( bool dirt ); ///< default: false
    /// Because the filter is infinite, the signal must be extended outisde
    /// the image domain. This extension can be by mirroring inside voxels
    /// about the borders, by using the nearest inside voxels or by using
    /// null values.
    virtual const FilterType::Boundary & boundary() const;
    virtual void setBoundary( const FilterType::Boundary & boundary );
    /// Verbosity level. Default value is carto::verbose that is usually
    /// set at the command level.
    virtual void setVerbose( int verbose = 1 );
    /// Set verbosity level to 0 -> Remove all possible traces.
    virtual void setQuiet();
    /// @}


  protected:
    //------------------------------------------------------------------------
    /// Helpers
    //------------------------------------------------------------------------
    /// @{
    /// Generic multidimensional filter. Calls filter1d successively
    /// in each direction.
    virtual void filter( carto::VolumeRef<double> vol ) const;
    /// redirection for in place fitlering
    virtual void filter1d( carto::VolumeRef<double> vol, int dir = -1,
                           carto::VolumeRef<double> tmp
                              = carto::VolumeRef<double>( (carto::Volume<double>*)0 ) ) const;
    /// Fast access : k must be in [0, size-1]
    virtual double & at( carto::VolumeRef<double> & vector, long k, int dir ) const;
    /// Fast access : k must be in [0, size-1]
    virtual const double & at( const carto::VolumeRef<double> & vector, long k, int dir ) const;
    /// Mirror access : k can be outside [0, size - 1].
    /// If borders are allocated, they will be used, else outside values will
    /// be infered according to the border strategy.
    /// However, for spline interpolation, note that border values should
    /// mirror inside values. It is the caller role to ensure that
    /// eventual borders are properly filled.
    virtual double at( const carto::VolumeRef<double> & vector, long k,
                       int dir, long fullsize, int binf ) const;
    /// If i is outside [0, size-1], it is mirrored to fall inside.
    virtual long mirrorCoeff( long i, long size ) const;
    /// If i is outside [binf, fullsize-1], it is mirrored to fall inside
    virtual long selectCoeff( long i, long fullsize, int binf ) const;
    /// compute K0 so that pole^K0 < precision
    virtual long computeK0( double pole, double tolerance = -1. ) const;
    /// @}

    int                   _verbose;
    std::vector<bool>     _dir;
    FilterType::Boundary  _boundary;
    FilterType::CopyType  _copy;
  };

// Yael - April 15, 2016
// I'm commenting out this class for now because the initialization
// strategy is currently false. I guess there must be a right way to do this
// but I don't have the time and do not need this class for now.
// I leave it in case it is of interest someday.
#if 0
  //==========================================================================
  //
  //   CAUSAL INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================
  /// H(z) = ( b0 + b1 * z^-1 + b2 * z^-2 ... ) / ( 1 - a1 * z^-1 - a2 * z^-2 ... )
  class CausalIIRFilter: public IIRFilterBase
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Destructor / Copy
    //------------------------------------------------------------------------
    /// @{
    /// Build an all-pole causal IIR filter.
    /// - If poles == false:
    ///   coeffs should contain the ordered polynomial coefficients a1, a2,...
    ///   of ( 1 - a1 * z^-1 - a2 * z^-2 - ... )
    /// - If poles == true:
    ///   coeffs should contain the poles zi of the polynome
    ///   ( 1 - z1 * z^-1 ) * ( 1 - z2 * z^-2 ) * ...
    CausalIIRFilter( const std::vector<double> & num_coeffs = std::vector<double>(),
                            const std::vector<double> & den_coeffs = std::vector<double>(),
                            double b0 = 1. );
    CausalIIRFilter( const CausalIIRFilter & other );
    virtual ~CausalIIRFilter();
    CausalIIRFilter & operator=( const CausalIIRFilter & other );
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
    const double & b0() const;
    const std::vector<double> & numCoeffs() const;
    const std::vector<double> & denCoeffs() const;
    void setB0( const double & gain );
    void setNumCoeff( const double & coeff );
    void setNumCoeffs( const std::vector<double> & coeff );
    void setDenCoeff( const double & coeff );
    void setDenCoeffs( const std::vector<double> & coeff );
    /// @}

  protected:
    //------------------------------------------------------------------------
    /// Helpers
    //------------------------------------------------------------------------
    /// @{
    double recursiveHelper( const carto::VolumeRef<double> in,
                            long k, int dir, long fullsize, int binf ) const;
    /// @}

    double _b0;
    std::vector<double> _num_coeff;
    std::vector<double> _den_coeff;
  };

  //==========================================================================
  //
  //   ALL POLE CAUSAL INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================
  /// H(z) = b0 / ( 1 - a1 * z^-1 - a2 * z^-2 ... )
  class CausalAllPoleIIRFilter: public IIRFilterBase
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Destructor / Copy
    //------------------------------------------------------------------------
    /// @{
    /// Build an all-pole causal IIR filter.
    /// - If poles == false:
    ///   coeffs should contain the ordered polynomial coefficients a1, a2,...
    ///   of ( 1 - a1 * z^-1 - a2 * z^-2 - ... )
    /// - If poles == true:
    ///   coeffs should contain the poles zi of the polynome
    ///   ( 1 - z1 * z^-1 ) * ( 1 - z2 * z^-2 ) * ...
    CausalAllPoleIIRFilter( const std::vector<double> & coeffs = std::vector<double>(),
                            double gain = 1., bool is_pole = false );
    /// Builds a single pole causal IIR filter
    /// (similar to CausalSinglePoleIIRFilter)
    CausalAllPoleIIRFilter( double coeff, double gain = 1., bool is_pole = false );
    CausalAllPoleIIRFilter( const CausalAllPoleIIRFilter & other );
    virtual ~CausalAllPoleIIRFilter();
    CausalAllPoleIIRFilter & operator=( const CausalAllPoleIIRFilter & other );
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
    const double & gain() const;
    const std::vector<double> & poles() const;      ///< only if is_pole
    const std::vector<double> & coeffs() const;     ///< only if not is_pole
    void setGain( const double & gain );
    void setPole( const double & pole, bool is_pole = true );
    void setPoles( const std::vector<double> & pole, bool is_pole = true );
    void setCoeff( const double & coeff, bool is_pole = false );
    void setCoeffs( const std::vector<double> & coeff, bool is_pole = false );
    /// @}

  protected:
    //------------------------------------------------------------------------
    /// Helpers
    //------------------------------------------------------------------------
    /// @{
    void computeK0();
    /// @}

    double _gain;
    std::vector<double> _coeff;
    std::vector<long>   _k0;
    bool   _is_pole;
  };
#endif

  //==========================================================================
  //
  //   SINGLE POLE CAUSAL INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================
  /// Special case of Infinite Impulse Response filter.
  ///
  /// Its transfer function is of the form:
  /// H(z) = b0 / ( 1 - a1 * z^-1 )
  class CausalSinglePoleIIRFilter: public IIRFilterBase
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Destructor / Copy
    //------------------------------------------------------------------------
    /// @{
    CausalSinglePoleIIRFilter( double pole = 0., double gain = 1. );
    CausalSinglePoleIIRFilter( const CausalSinglePoleIIRFilter & other );
    virtual ~CausalSinglePoleIIRFilter();
    CausalSinglePoleIIRFilter & operator=( const CausalSinglePoleIIRFilter & other );
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
    const double & gain() const;
    const double & pole() const;
    void setPole( const double & pole );
    void setGain( const double & gain );
    /// @}

  protected:
    //------------------------------------------------------------------------
    /// Helpers
    //------------------------------------------------------------------------
    /// @{
    void computeK0();
    /// @}

    double _gain;
    double _pole;
    long   _k0;
  };


// Yael - April 15, 2016
// Same as the CausalFilter class. I think this one is accurate, but it needs
// CausalFilter to work, so it will wait.
#if 0
  //==========================================================================
  //
  //   WARPER FOR ANTICAUSAL FILTERS
  //
  //==========================================================================
  /// Automatically create an anticausal filter from a causal filter
  /// by overriding the private volume accessors methods
  template <typename CausalFilter>
  class AntiCausalWarperIIRFilter: public CausalFilter
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Destructor / Copy
    //------------------------------------------------------------------------
    /// @{
    explicit AntiCausalWarperIIRFilter( const CausalFilter & other = CausalFilter() );
    AntiCausalWarperIIRFilter( const AntiCausalWarperIIRFilter & other );
    virtual ~AntiCausalWarperIIRFilter();
    AntiCausalWarperIIRFilter & operator=( const AntiCausalWarperIIRFilter & other );
    /// @}

  protected:
    //------------------------------------------------------------------------
    /// Helpers
    //------------------------------------------------------------------------
    /// @{
    virtual double & at( carto::VolumeRef<double> & vector, long k, int dir ) const;
    virtual const double & at( const carto::VolumeRef<double> & vector, long k, int dir ) const;
    virtual double at( const carto::VolumeRef<double> & vector, long k,
                       int dir, long fullsize, int binf ) const;
    /// @}
  };

  //--------------------------------------------------------------------------
  // Typedefs
  //--------------------------------------------------------------------------
  typedef AntiCausalWarperIIRFilter<CausalIIRFilter>           AntiCausalIIRFilter;
  typedef AntiCausalWarperIIRFilter<CausalAllPoleIIRFilter>    AntiCausalAllPoleIIRFilter;
  typedef AntiCausalWarperIIRFilter<CausalSinglePoleIIRFilter> AntiCausalSinglePoleIIRFilter;
#endif

  //==========================================================================
  //
  //   SYMMETRIC ALL-POLE INFINITE IMPULSE RESPONSE FILTER
  //
  //==========================================================================
  /// Symmetric, all-pole, infinite impulse response Filter
  ///
  /// Special case of Infinite Impulse Response filter:
  /// Separable filter whose transfer function is of the form
  /// H(z) = c / P(z)
  /// with the coefficients of P symmetric, i.e. p(k) = p(-k)
  ///
  /// This implementation follows that of
  /// Unser, Aldroubi & Eden, "B-Spline Signal Processing: Part II -
  /// Efficient Design and Applications" in IEEE Transactions on Signal
  /// Processing (1993)
  ///
  /// To diminish rounding errors, it is possible to give the gain value
  /// c * prod( -p(k) ) instead of c, and enable the precise mode with
  /// setModePreciseGain().
  ///
  /// \note If borders are allocated, they will be used, else inside values
  /// will be mirrored in the outside.
  /// However, for spline interpolation, note that border values should
  /// mirror inside values. It is the caller role to ensure that
  /// eventual borders are properly filled.
  class SymAllPoleIIRFilter: public IIRFilterBase
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Destructor / Copy
    //------------------------------------------------------------------------
    /// @{
    SymAllPoleIIRFilter( const double gain = 0.,
                         const std::vector<double> & poles = std::vector<double>() );
    SymAllPoleIIRFilter( const SymAllPoleIIRFilter & other );
    virtual ~SymAllPoleIIRFilter();
    SymAllPoleIIRFilter & operator=( const SymAllPoleIIRFilter & other );
    /// @}

    //------------------------------------------------------------------------
    /// Execution
    //------------------------------------------------------------------------
    /// @{
    virtual void filter1d( const carto::VolumeRef<double> in,
                           carto::VolumeRef<double>       out,
                           int                            dir = -1 ) const;
    /// @}

    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    const double & gain() const;
    const std::vector<double> & poles() const;
    void setPoles( const std::vector<double> & poles );
    void addPole( const double & pole );
    /// Relative to Unser & al, gain = c0
    void setGain( const double & gain );
    /// If this mode is activated, provided gain should be c * prod(-poles).
    /// This allows to avoid some floating points operations, which is
    /// especially useful at high orders.
    /// Default value at construction is false.
    void setModePreciseGain( bool precise = true );
    /// @}

  protected:
    //------------------------------------------------------------------------
    /// Helpers
    //------------------------------------------------------------------------
    /// @{
    void computeK0();
    void computePreciseGain();
    /// @}

    std::vector<double>   _poles;
    std::vector<long>     _k0;
    double                _gain;
    double                _precisegain;
    bool                  _precise;
  };

} // namespace aims

#endif // AIMS_SIGNALFILTER_IIRFILTER_H
