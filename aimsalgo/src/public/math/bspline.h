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

  /// B-Spline function centered on 0, evaluated at run time.
  class BSpline
  {
  public:
    BSpline( unsigned order = 3 );
    ~BSpline();

    /// \see spline()
    double   operator() ( double x )                const;
    /// spline value at point x
    double   spline( double x )                     const;
    /// n-th derivative of the spline at point x
    double   derivative( double x, unsigned n = 1 ) const;
    /// spline order
    unsigned order()                                const;

  private:
    double dO()     const;
    unsigned _order;
  };

  //==========================================================================
  //
  //     B-SPLINE : TABULATED VALUES
  //
  //==========================================================================

  /// Pre-computed B-Spline values
  /// In the "order 0" case, the array is not used (the analytical expression
  /// is too simple)
  class TabulBSpline
  {
  public:
    TabulBSpline( unsigned order = 3, unsigned nder = 1, unsigned length = 65537 );
    ~TabulBSpline();

    /// \see spline()
    double   operator() ( double x )                const;
    /// spline value at point x
    double   spline( double x )                     const;
    /// n-th derivative of the spline at point x
    double   derivative( double x, unsigned n = 1 ) const;
    /// spline order
    unsigned order()                                const;
    /// number of derivative stored
    unsigned nder()                                 const;
    /// length
    unsigned length()                               const;

  private:
    unsigned index( double x )          const;
    bool     is_valid( unsigned index ) const;
    unsigned _order;
    std::vector<std::vector<double> > _values;
  };

} // namespace aims

#endif // AIMSALGO_MATH_BSPLINE_H
