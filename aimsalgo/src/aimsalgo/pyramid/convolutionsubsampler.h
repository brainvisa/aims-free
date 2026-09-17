#ifndef AIMS_PYRAMID_CONVOLUTIONSUBSAMPLER_H
#define AIMS_PYRAMID_CONVOLUTIONSUBSAMPLER_H

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
  /// - V support() with V[0] and v[1]
  template <typename BasisFunction>
  class ConvolutionSubSampler
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Destructor / Copy
    //------------------------------------------------------------------------
    /// @{
    ConvolutionSubSampler( unsigned r = 2 );
    ConvolutionSubSampler( const Point4du & r );
    ConvolutionSubSampler( const BasisFunction & bfunc, unsigned r );
    ConvolutionSubSampler( const std::vector<BasisFunction> & bfunc = std::vector<BasisFunction>(),
                           const Point4du & r = Point4du( 2, 2, 2, 2 ) );
    ConvolutionSubSampler( const ConvolutionSubSampler & other );
    virtual ~ConvolutionSubSampler();
    ConvolutionSubSampler & operator=( const ConvolutionSubSampler & other );
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
    const std::vector<bool> & directions() const;
    const Point4du & factor() const;
    void setBasisFunction( const BasisFunction & func );
    void setBasisFunction( const std::vector<BasisFunction> & func );
    void setFactor( unsigned r );
    void setFactor( const Point4du & r );
    void setDirections( const std::vector<bool> & dir );
    void setDirections( bool dirx, bool diry, bool dirz, bool dirt );
    void setVerbose( int verbose = 1 );
    void setQuiet();
    /// @}

  protected:
    std::vector<BasisFunction> _func;
    int                        _verbose;
    std::vector<bool>          _dir;
    Point4du                   _factor;
  };

} // namespace aims

#endif // AIMS_PYRAMID_CONVOLUTIONSUBSAMPLER_H
