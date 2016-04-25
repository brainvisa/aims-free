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

#ifndef AIMS_SIGNALFILTER_MOVINGAVERAGEFILTER_H
#define AIMS_SIGNALFILTER_MOVINGAVERAGEFILTER_H

//============================================================================
//
//                       MOVING AVERAGE KERNEL
//
//============================================================================

#include <aims/signalfilter/types.h>
#include <aims/vector/vector.h>
#include <vector>

namespace aims {

  /// 1D Moving Average Kernel
  ///
  /// This kernel can be used as basis function in ConvolutionFilter and
  /// ConvolutionSubSampler classes.
  ///
  /// The moving average transfer function is:
  /// - if Causal: H(z) = 1/l * sum[k=-l+1;k<=0]( z^{-k} )
  /// - if Anticausal: H(z) = 1/l * sum[k=0;k<=l-1]( z^{-k} )
  /// - if Symmetrical: H(z) = 1/l * sum[k=-l/2;k<=l/2]( z^{-k} )
  ///
  /// The symmetrical version needs an odd length.
  class MovingAverageKernel
  {
  public:

    //------------------------------------------------------------------------
    /// Constructor/Destructor/Copy
    //------------------------------------------------------------------------
    /// @{
    /// If symtype == Symmetrical, length should be odd.
    MovingAverageKernel( unsigned length = 3,
                         FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageKernel( const MovingAverageKernel & other );
    virtual ~MovingAverageKernel();
    MovingAverageKernel & operator= ( const MovingAverageKernel & other );
    /// @}

    //------------------------------------------------------------------------
    /// Get value
    //------------------------------------------------------------------------
    /// @{
    double operator() ( int x ) const;
    double at( int x ) const;
    const Point2di & support() const;
    /// @}

    //------------------------------------------------------------------------
    /// Set parameters
    //------------------------------------------------------------------------
    /// Be aware that a call to reset, etc.
    /// will trigger a reallocation of the array and a recomputing of
    /// the values. If several of these values need changing, it is thus
    /// avdised to use the reset() method.
    ///
    /// @{
    void reset( unsigned length = 3,
                FilterType::Direction symtype = FilterType::Symmetrical );
    void setLength( unsigned length );
    void setType( FilterType::Direction symtype );
    /// @}

    //------------------------------------------------------------------------
    /// Get parameters
    //------------------------------------------------------------------------
    /// @{
    unsigned                length()    const;
    FilterType::Direction   type()    const;
    /// @}

  protected:
    void setSupport( FilterType::Direction symtype );
    FilterType::Direction   _type;
    Point2di                _support;
    std::vector<double>     _values;
  };

} // namespace aims

//============================================================================
//
//                       MOVING AVERAGE CASCADE KERNEL
//
//============================================================================

#include <aims/vector/vector.h>
#include <vector>

namespace aims {

  /// 1D Cascade of Moving Average Kernel
  ///
  /// This kernel can be used as basis function in ConvolutionFilter and
  /// ConvolutionSubSampler classes.
  ///
  /// It is equivalent to a convolution of n Moving Average
  ///
  /// The symmetrical version needs n * (length - 1) to be even.
  class MovingAverageCascadeKernel
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor/Destructor/Copy
    //------------------------------------------------------------------------
    /// @{
    /// If symtype == Symmetrical, n * (length - 1) should be even.
    MovingAverageCascadeKernel( unsigned length = 3, unsigned n = 2,
                                FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeKernel( const MovingAverageCascadeKernel & other );
    virtual ~MovingAverageCascadeKernel();
    MovingAverageCascadeKernel & operator= ( const MovingAverageCascadeKernel & other );
    /// @}

    //------------------------------------------------------------------------
    /// Get value
    //------------------------------------------------------------------------
    /// @{
    double operator() ( int x ) const;
    double at( int x ) const;
    const Point2di & support() const;
    /// @}

    //------------------------------------------------------------------------
    /// Set parameters
    //------------------------------------------------------------------------
    /// Be aware that a call to reset, etc.
    /// will trigger a reallocation of the array and a recomputing of
    /// the values. If several of these values need changing, it is thus
    /// avdised to use the reset() method.
    ///
    /// @{
    /// (Re)set all parameters
    void reset( unsigned length = 3, unsigned n = 2,
                FilterType::Direction symtype = FilterType::Symmetrical );
    void setLength( unsigned length );
    void setCascade( unsigned n );
    void setType( FilterType::Direction symtype );
    /// @}

    //------------------------------------------------------------------------
    /// Get parameters
    //------------------------------------------------------------------------
    /// @{
    unsigned                length()    const;
    unsigned                cascade()   const;
    FilterType::Direction   type()      const;
    /// @}

  protected:
    void setValues( unsigned length, unsigned n, FilterType::Direction symtype );
    unsigned _n;
    Point2di _support;
    std::vector<double> _values;
  };

} // namespace aims


//============================================================================
//
//                       MOVING AVERAGE FILTER
//
//============================================================================

#include <aims/signalfilter/convolutionfilter.h>

namespace aims {

  /// Separable Moving Average Filter
  ///
  /// This filter convolves a volume with a Moving Average Kernel
  /// \see MovingAverageKernel
  class MovingAverageFilter: public ConvolutionFilter<MovingAverageKernel>
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    MovingAverageFilter( unsigned length = 3, FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageFilter( const Point4du & length, FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageFilter( const MovingAverageFilter & other );
    virtual ~MovingAverageFilter();
    MovingAverageFilter & operator=( const MovingAverageFilter & other );
    /// @}

    //------------------------------------------------------------------------
    /// Set parameters
    //------------------------------------------------------------------------
    /// @{
    void setLength( unsigned length );
    void setLength( const Point4du & length );
    void setType( FilterType::Direction symtype );
    /// @}

    //------------------------------------------------------------------------
    /// Get parameters
    //------------------------------------------------------------------------
    /// @{
    Point4du                length()    const;
    FilterType::Direction   type()      const;
    /// @}

  protected:
    // Make some inherited functions private
    using ConvolutionFilter<MovingAverageKernel>::setBasisFunction;
  };

} // namespace aims

//============================================================================
//
//                       MOVING AVERAGE CASCADE FILTER
//
//============================================================================

#include <aims/signalfilter/convolutionfilter.h>

namespace aims {

  /// Separable Cascade of Moving Average Filter
  ///
  /// This filter convolves a volume with a Cascade of Moving Average Kernel
  /// \see MovingAverageCascadeKernel
  class MovingAverageCascadeFilter: public ConvolutionFilter<MovingAverageCascadeKernel>
  {
  public:
    //------------------------------------------------------------------------
    /// Constructor / Copy
    //------------------------------------------------------------------------
    /// @{
    MovingAverageCascadeFilter( unsigned length = 3, unsigned n = 2,
                                FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeFilter( const Point4du & length, unsigned n = 2,
                                FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeFilter( const Point4du & length, const Point4du & n,
                                FilterType::Direction symtype = FilterType::Symmetrical );
    MovingAverageCascadeFilter( const MovingAverageCascadeFilter & other );
    virtual ~MovingAverageCascadeFilter();
    MovingAverageCascadeFilter & operator=( const MovingAverageCascadeFilter & other );
    /// @}

    //------------------------------------------------------------------------
    /// Set parameters
    //------------------------------------------------------------------------
    /// @{
    void setLength( unsigned length );
    void setLength( const Point4du & length );
    void setCascade( unsigned n );
    void setCascade( const Point4du & n );
    void setType( FilterType::Direction symtype );
    /// @}

    /// Get parameters
    /// @{
    Point4du              length()    const;
    Point4du              cascade()   const;
    FilterType::Direction type()      const;
    /// @}

  protected:
    // Make some inherited functions private
    using ConvolutionFilter<MovingAverageCascadeKernel>::setBasisFunction;
  };

} // namespace aims

#endif // AIMS_SIGNALFILTER_MOVINGAVERAGEFILTER_H
