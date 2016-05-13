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

#ifndef AIMSALGO_MATH_BSPLINE_H
#define AIMSALGO_MATH_BSPLINE_H

#include <aims/vector/vector.h>         // Point2dd
#include <cartobase/exception/assert.h>
#include <cmath>    // std::abs
#include <limits>   // std::numeric_limits
#include <vector>   // std::vector

namespace aims {

  //==========================================================================
  //
  //     B-SPLINE : ORDER AS TEMPLATE ARGUMENT
  //
  //==========================================================================

  /// 1D B-Spline functions centered on 0, recursively evaluated at run time.
  template <unsigned Order>
  struct BSplineOrder
  {
    static double at( double x )
    {
      return spline(x);
    }

    static double spline( double x )
    {
      if( std::abs(x) > double(Order+1)/2 )
        return 0.;

      return ( dO()/2. + .5 - x ) / dO()
             * BSplineOrder<Order-1>::spline( x + .5 )
             + ( dO()/2. + .5 + x  ) / dO()
             * BSplineOrder<Order-1>::spline( x - .5 );
    }

    static double derivative( double x )
    {
      return BSplineOrder<Order-1>::spline( x + .5 )
             - BSplineOrder<Order-1>::spline( x - .5 );
    }

    static double derivative2( double x )
    {
      ASSERT( Order > 1 );
      return BSplineOrder<Order-1>::derivative( x + .5 )
             - BSplineOrder<Order-1>::derivative( x - .5 );
    }

    static unsigned order() { return Order; }

  private:
    static double dO()     { return (double)Order; }
  };

  /// Specialization for 0 order B-Spline
  template <>
  struct BSplineOrder<0>
  {
    static double spline( double value )
    {
      if( -.5 <= value && value < .5 )
        return 1.;
      else
        return 0.;
    }

    static unsigned order() { return 0; }
  };

  //==========================================================================
  //
  //     B-SPLINE : MULTI ORDER
  //
  //==========================================================================

  /// *Centered and *scaled B-Spline function, evaluated at run time.
  ///
  /// Centered B-splines of order \c n are recursively evaluated following the
  /// formula:
  /// \f[
  ///   \Beta^n(x) = \frac{
  ///     (\frac{n+1}{2} + x) \Beta^{n-1}( x + \frac{1}{2} ) +
  ///     (\frac{n+1}{2} - x) \Beta^{n-1}( x - \frac{1}{2} )
  ///   }{n}
  /// \f]
  ///
  /// Let \c m be the scaling factor, the scaled B-Spline is then equal to
  /// \f$b_m^n(x) = \Beta^n( \frac{x}{m} )\f$
  ///
  /// A shifted scaled B-Spline is equal to:
  /// \f$c_m^n(x) = \Beta^n( \frac{x}{m} + \frac{1}{2} )\f$
  ///
  /// See Unser, Aldroubi & Eden: "B-Spline Signal Processing: Part I -
  /// Theory" in IEEE Transactions on Signal Processing (February 1993).
  ///
  /// In this implementation, unshifted centered B-Spline are recursively
  /// evaluated at value x/m (+ .5).
  ///
  /// \note This is also a base class for particular spline implementations
  /// ( fast, tabular, ... ). Methods are virtual so this class can be used
  /// in polymorphic contexts
  class BSpline
  {
  public:
    BSpline( unsigned order = 3, float scale = 1., bool shifted = false );
    BSpline( const BSpline & other );
    virtual ~BSpline();
    BSpline & operator= ( const BSpline & other );

    /// Compute value
    /// @{
    /// \see spline()
    virtual
    double   operator() ( double x )                const;
    /// spline value at point x
    virtual
    double   at( double x )                     const;
    /// n-th derivative of the spline at point x
    virtual
    double   derivative( double x, unsigned n = 1 ) const;
    /// Get the inferior and superior boundaries of the function support
    /// (i.e. the interval on which it is not null).
    /// If the order is not zero, boundaries are not included in the support.
    /// Else, they are.
    virtual
    const Point2dd & support() const;
    /// @}

    /// Set parameters
    /// @ {
    /// (Re)set all parameters
    virtual
    void reset( unsigned order = 3, float scale = 1., bool shifted = false );
    /// order
    virtual
    void setOrder( unsigned order );
    /// scale
    virtual
    void setScale( float scale );
    /// shifted
    virtual
    void setShifted( bool shifted );
    /// centered
    virtual
    void setCentered( bool centered );
    /// @}

    /// Get parameters
    /// @ {
    /// spline order
    virtual
    unsigned order()    const;
    /// spline scale
    virtual
    float    scale()    const;
    /// is shifted ?
    virtual
    bool     shifted()  const;
    /// is centered ?
    virtual
    bool     centered() const;
    /// @}

  protected:
    double dO()     const;
    virtual void setSupport();
    unsigned _order;
    float    _scale;
    bool     _shift;
    Point2dd _support;
  };

  //==========================================================================
  //
  //     B-SPLINE : FAST
  //
  //==========================================================================

  /// *Centered and *scaled B-Spline function, evaluated at run time.
  ///
  /// Here, analytical expression are hard coded for orders 0 to 7.
  ///
  /// In this implementation, unshifted centered B-Spline are
  /// evaluated at value x/m (+ .5).
  class FastBSpline: public BSpline
  {
  public:
    FastBSpline( unsigned order = 3, float scale = 1., bool shifted = false );
    FastBSpline( const FastBSpline & other );
    virtual ~FastBSpline();
    FastBSpline & operator= ( const FastBSpline & other );

    /// Compute value
    /// @{
    /// spline value at point x
    ///
    /// Implemented for orders 0 to 7
    virtual
    double   at( double x )                     const;
    /// n-th derivative of the spline at point x
    ///
    /// Implemented for orders 0 to 3
    virtual
    double   derivative( double x, unsigned n = 1 ) const;
    /// @}

  };

  //==========================================================================
  //
  //     B-SPLINE : TABULATED VALUES
  //
  //==========================================================================

  /// Pre-computed B-Spline values
  /// In the "order 0" case, the array is not used (the analytical expression
  /// is too simple)
  ///
  /// See BSpline for information on values computation
  class TabulBSpline: public BSpline
  {
  public:
    TabulBSpline( unsigned order = 3,
                  unsigned nder = 1,
                  float scale = 1.,
                  bool shifted = false,
                  size_t length = std::numeric_limits<unsigned short>::max() - 1 );
    TabulBSpline( const TabulBSpline & other );
    virtual ~TabulBSpline();
    TabulBSpline & operator= ( const TabulBSpline & other );

    /// Get value
    /// @{
    /// spline value at point x
    virtual
    double   at( double x )                     const;
    /// n-th derivative of the spline at point x
    virtual
    double   derivative( double x, unsigned n = 1 ) const;
    /// @}

    /// Set parameters
    /// Be aware that a call to reset, setOrder, setNbDer and setLength
    /// will trigger a reallocation of the array and a recomputing of
    /// the values. If several of these values need changing, it is thus
    /// avdised to use the reset() method.
    ///
    /// @{
    /// (Re)set all parameters
    virtual
    void reset( unsigned order = 3,
                unsigned nder = 1,
                float scale = 1.,
                bool shifted = false,
                size_t length = std::numeric_limits<unsigned short>::max() - 1 );
    /// order
    virtual
    void setOrder( unsigned order );
    /// number of derivative stored
    virtual
    void setNbDer( unsigned nder );
    /// array length
    virtual
    void setLength( size_t length );
    /// @}

    /// Get parameters
    /// @{
    /// number of derivative stored
    virtual
    unsigned nder()     const;
    /// array length
    virtual
    size_t   length()   const;
    /// @}

  protected:
    size_t   index( double x )          const;
    bool     is_valid( size_t index ) const;
    void     setArray( unsigned nder, size_t length );
    std::vector<std::vector<double> > _values;
  };

  //==========================================================================
  //
  //     DISCRETE B-SPLINE
  //
  //==========================================================================

  /// Discrete B-Splines
  /// B-Spline pre sampled only on integral values.
  class DiscreteBSpline: public BSpline
  {
  public:
    /// Constructor/Destructor/Copy
    /// @{
    DiscreteBSpline( unsigned order = 3, float scale = 1., bool shifted = false );
    DiscreteBSpline( const DiscreteBSpline & other );
    virtual ~DiscreteBSpline();
    DiscreteBSpline & operator= ( const DiscreteBSpline & other );
    /// @}

    /// Get value
    /// @{
    virtual
    double operator() ( int x ) const;
    virtual
    double at( int x ) const;
    /// @}

    /// Set parameters
    /// Be aware that a call to reset, setOrder, setNbDer and setLength
    /// will trigger a reallocation of the array and a recomputing of
    /// the values. If several of these values need changing, it is thus
    /// avdised to use the reset() method.
    ///
    /// @{
    /// (Re)set all parameters
    virtual
    void reset( unsigned order = 3, float scale = 1., bool shifted = false );
    /// order
    virtual
    void setOrder( unsigned order );
    /// scale
    virtual
    void setScale( float scale );
    /// shifted
    virtual
    void setShifted( bool shifted = true );
    /// centered
    virtual
    void setCentered( bool centered = true );
    /// @}

  protected:
    void setArray();
    void setSupport();
    std::vector<double> _values;
  };

} // namespace aims

#endif // AIMSALGO_MATH_BSPLINE_H
