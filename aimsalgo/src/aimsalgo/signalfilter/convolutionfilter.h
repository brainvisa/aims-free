#ifndef AIMS_SIGNALFILTER_CONVOLUTIONFILTER_H
#define AIMS_SIGNALFILTER_CONVOLUTIONFILTER_H

#include <cartodata/volume/volume.h>
#include <vector>

namespace aims {

  //==========================================================================
  //   CONVOLUTION FILTER
  //==========================================================================
  /// Convolution Filter
  ///
  /// Interpolate a discrete signal with a separable kernel of
  /// finite support.
  /// The basis function must possess:
  /// - U operator() ( T )
  /// - V support() with:
  ///   * support()[0] the lower support boundary
  ///   * support()[1] the upper support boundary
  template <typename BasisFunction>
  class ConvolutionFilter
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Destructor / Copy
    //------------------------------------------------------------------------
    /// @{
    /// The same basis function is used in all directions
    ConvolutionFilter( const BasisFunction & bfunc );
    /// A different basis function is used in each direction
    ConvolutionFilter( const std::vector<BasisFunction> & bfunc = std::vector<BasisFunction>() );
    ConvolutionFilter( const ConvolutionFilter & other );
    virtual ~ConvolutionFilter();
    ConvolutionFilter & operator=( const ConvolutionFilter & other );
    /// @}

    //------------------------------------------------------------------------
    /// Execution
    //------------------------------------------------------------------------
    /// Function-like operator that filters a given volume
    /// @{
    /// \note 1 copy
    /// \warning This call risks causing additional precision errors
    template <typename OUT, typename IN>
    carto::VolumeRef<OUT> execute( const carto::VolumeRef<IN> & in ) const;
    /// Write result in an existing volume.
    /// If the allocated size differs from the needed resulting size,
    /// the volume is reallocated.
    /// \note 1 copy
    /// \warning This call risks causing additional precision errors
    template <typename OUT, typename IN>
    carto::VolumeRef<OUT> execute( const carto::VolumeRef<IN> & in, carto::VolumeRef<OUT> & out ) const;
    /// @}


    //------------------------------------------------------------------------
    /// Parameters
    //------------------------------------------------------------------------
    /// @{
    /// 4D vector giving directions along which the filter is applied.
    const std::vector<bool> & directions() const;
    /// Change the basis functions.
    void setBasisFunction( const BasisFunction & func );
    /// Change the basis functions.
    void setBasisFunction( const std::vector<BasisFunction> & func );
    /// Set the directions along which the filter is applied.
    void setDirections( const std::vector<bool> & dir );
    /// Set the directions along which the filter is applied.
    void setDirections( bool dirx, bool diry, bool dirz, bool dirt );
    /// Verbosity level
    void setVerbose( int verbose = 1 );
    /// Equivalent to setVerbose(0)
    void setQuiet();
    /// @}

  protected:
    std::vector<BasisFunction> _func;
    int                        _verbose;
    std::vector<bool>          _dir;
  };

} // namespace aims

#endif // AIMS_SIGNALFILTER_CONVOLUTIONFILTER_H
