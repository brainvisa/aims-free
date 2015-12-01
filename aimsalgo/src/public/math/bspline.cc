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

#include <aims/math/bspline.h>
#include <cartobase/exception/assert.h>
#include <cmath>    // std::abs, std::min
#include <limits>   // std::numeric_limits
#include <vector>   // std::vector

using namespace aims;
using namespace std;

//============================================================================
//
//     B-SPLINE : MULTI ORDER
//
//============================================================================

BSpline::BSpline( unsigned order ):
_order(order)
{}

BSpline::~BSpline()
{}

double BSpline::operator() ( double x ) const
{
  return spline(x);
}

double BSpline::spline( double x ) const
{
  if( std::abs(x) > double(order()+1)/2 )
    return 0.;
  if( order() == 0 )
    if( x < 0.5 )
      return 1.;
    else
      return 0.;

  BSpline bminus( order() - 1 );
  return ( dO()/2. + .5 + x ) / dO()
         * bminus.spline( x + .5 )
         + ( dO()/2. + .5 - x ) / dO()
         * bminus.spline( x - .5 );
}

double BSpline::derivative( double x, unsigned n ) const
{
  if( n == 0 )
    return spline(x);
  else
  {
    ASSERT( n <= order() );
    BSpline bminus( order() - 1 );
    return bminus.derivative( x + .5, n - 1 )
           - bminus.derivative( x - .5, n - 1 );
  }
}

unsigned BSpline::order() const
{
  return _order;
}

double BSpline::dO() const
{
  return (double)_order;
}

//============================================================================
//
//     B-SPLINE : TABULATED VALUES
//
//============================================================================

TabulBSpline::TabulBSpline( unsigned order, unsigned nder, unsigned length ):
_order( order ),
_values( std::min(nder, order) + 1, vector<double>(order ? length + 1 : 0 ) )
{
  nder = std::min(nder, order);
  if( order )
  {
    double parameter;
    BSpline b(order);
    for( unsigned i = 0; i <= length; ++i )
    {
      parameter = double(i) * double(order + 1) / ( double(length) * 2 );
      for( unsigned n = 0; n <= nder; ++n )
        _values[n][i] = b.derivative( parameter, (double)n );
    }
  }
}

TabulBSpline::~TabulBSpline()
{}

unsigned TabulBSpline::index( double x ) const
{
  return unsigned( std::abs(x) * 2. * double(_values[0].size() - 1) / ( double(_order) + 1 ) );
}

bool TabulBSpline::is_valid( unsigned index ) const
{
  return ( index >= 0 && index < _values[0].size() );
}

unsigned TabulBSpline::order() const
{
  return _order;
}

unsigned TabulBSpline::nder() const
{
  return _values.size() - 1;
}

unsigned TabulBSpline::length() const
{
  return _values[0].size();
}

double TabulBSpline::operator() ( double x ) const
{
  return spline(x);
}

double TabulBSpline::spline( double x ) const
{
  if( _order == 0 )
    return BSplineOrder<0>::spline(x);

  unsigned i = index(x);
  if( !is_valid(i) )
    return 0.;
  else
    return _values[0][i];
}

double TabulBSpline::derivative( double x, unsigned n ) const
{
  if( n == 0 )
    return spline(x);

  ASSERT( n <= _order && n < _values.size() );
  unsigned i = index(x);
  if( !is_valid(i) )
    return 0.;
  else
    return _values[n][i] * ( x < 0 && n % 2 ? -1. /* antisym if odd */
                                            : 1.  /* sym if even */ );
}
