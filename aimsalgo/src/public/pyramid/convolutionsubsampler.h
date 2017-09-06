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
