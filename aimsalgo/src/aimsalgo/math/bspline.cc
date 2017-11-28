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
#include <cartobase/type/string_conversion.h>
#include <cmath>    // std::abs, std::min
#include <limits>   // std::numeric_limits
#include <vector>   // std::vector
#include <exception>

using namespace aims;
using namespace std;

//============================================================================
//
//     B-SPLINE : MULTI ORDER
//
//============================================================================

BSpline::BSpline( unsigned order, float scale, bool shifted ):
  _order(order),
  _scale(scale),
  _shift(shifted)
{
  setSupport();
}

BSpline::BSpline( const BSpline & other ):
  _order(other._order),
  _scale(other._scale),
  _shift(other._shift),
  _support(other._support)
{}

BSpline & BSpline::operator= ( const BSpline & other )
{
  if( this != &other )
  {
    _order = other._order;
    _scale = other._scale;
    _shift = other._shift;
    _support = other._support;
  }
  return *this;
}

BSpline::~BSpline()
{}

double BSpline::operator() ( double x ) const
{
  return this->at(x);
}

double BSpline::at( double x ) const
{
  x = ( _scale != 1. ? x/_scale : x ) + ( _shift ? 0.5 : 0. );
  if( std::abs(x) > double(order()+1)/2 )
    return 0.;
  if( order() == 0 )
  {
    if( x < 0.5 )
      return 1.;
    else
      return 0.;
  }

  BSpline bminus( order() - 1 ); // centered, unscaled
  return ( dO()/2. + .5 + x ) / dO()
         * bminus.at( x + .5 )
         + ( dO()/2. + .5 - x ) / dO()
         * bminus.at( x - .5 );
}

double BSpline::derivative( double x, unsigned n ) const
{
  if( n == 0 )
    return this->at(x);
  else if( n > this->order() )
    return 0.;
  else
  {
    x = ( this->scale() != 1. ? x/this->scale() : x ) + ( this->shifted() ? 0.5 : 0. );
    BSpline bminus( order() - 1 ); // centered, unscaled
    double result =  ( dO()/2. + .5 + x ) / dO()
           * bminus.derivative( x + .5, n )
           + ( dO()/2. + .5 - x ) / dO()
           * bminus.derivative( x - .5, n )
           + ( bminus.derivative( x + .5, n - 1 ) - bminus.derivative( x - .5 , n - 1 ) ) * double(n) / dO();
    return result;
  }
}

const Point2dd & BSpline::support() const
{
  return _support;
}

void BSpline::setSupport()
{
  _support[0] = - this->scale() * (float)(this->order() + 1.)/2. - ( this->shifted() ? .5 : 0. );
  _support[1] = this->scale() * (float)(this->order() + 1.)/2. - ( this->shifted() ? .5 : 0. );
}

void BSpline::reset( unsigned order, float scale, bool shifted )
{
  _order = order;
  _scale = scale;
  _shift = shifted;
  setSupport();
}

void BSpline::setOrder( unsigned order )
{
  _order = order;
  setSupport();
}

void BSpline::setScale( float scale )
{
  _scale = scale;
  setSupport();
}

void BSpline::setShifted( bool shifted )
{
  _shift = shifted;
  setSupport();
}

void BSpline::setCentered( bool centered )
{
  _shift = !centered;
  setSupport();
}

//============================================================================
//
//     B-SPLINE : FAST
//
//============================================================================

FastBSpline::FastBSpline( unsigned order, float scale, bool shifted ):
BSpline(order, scale, shifted)
{}

FastBSpline::~FastBSpline()
{}

FastBSpline::FastBSpline( const FastBSpline & other ):
  BSpline( other )
{}

FastBSpline & FastBSpline::operator= ( const FastBSpline & other )
{
  if( this != &other )
  {
    BSpline::operator=( other );
  }
  return *this;
}

double FastBSpline::at( double x ) const
{
  x = std::abs( ( this->scale() != 1. ? x / this->scale() : x ) +
                ( this->shifted() ? 0.5 : 0. ) );

  switch( this->order() )
  {
    case 0:
      if( x < 0.5 )
        return 1.0;
      return 0.0;

    case 1:
      if( x < 1.0 )
        return 1.0 - x;
      return 0.0;

    case 2:
      if ( x < 0.5 )
      {
        return 0.75 - x * x;
      }
      if ( x < 1.5 )
      {
        x = 1.5 - x;
        return 0.5 * x * x;
      }
      return 0.0;

    case 3:
      if ( x < 1.0 )
      {
        return ( x * x * ( x - 2.0 ) * 3.0 + 4.0 ) * ( 1.0 / 6.0 );
      }
      if ( x < 2.0 )
      {
        x = 2.0 - x;
        return x * x * x * ( 1.0 / 6.0 );
      }
      return 0.0;

    case 4:
      if ( x < 0.5 )
      {
        x *= x;
        return x * ( x * 0.25 - 0.625 ) + 115.0 / 192.0;
      }
      if ( x < 1.5 )
      {
        return x * ( x * ( x * ( 5.0 / 6.0 - x * ( 1.0 / 6.0 ) ) - 1.25 ) +
               5.0 / 24.0 ) + 55.0 / 96.0;
      }
      if ( x < 2.5 )
      {
        x -= 2.5;
        x *= x;
        return x * x * ( 1.0 / 24.0 );
      }
      return 0.0;

    case 5:
      if ( x < 1.0 )
      {
        double f = x * x;
        return f * ( f * ( 0.25 - x * ( 1.0 / 12.0 ) ) - 0.5 ) + 0.55;
      }
      if ( x < 2.0 )
      {
        return x * ( x * ( x * ( x * ( x * ( 1.0 / 24.0 ) - 0.375 ) + 1.25 ) -
               1.75 ) + 0.625 ) + 0.425;
      }
      if ( x < 3.0 )
      {
        double f = 3.0 - x;
        x = f * f;
        return f * x * x * ( 1.0 / 120.0 );
      }
      return 0.0;

    case 6:
      if ( x < 0.5 )
      {
        x *= x;
        return x * ( x * ( 7.0 / 48.0 - x * ( 1.0 / 36.0 ) ) - 77.0 / 192.0 ) +
               5887.0 / 11520.0;
      }
      if ( x < 1.5 )
      {
        return x * ( x * ( x * ( x * ( x * ( x * ( 1.0 / 48.0 ) - 7.0 / 48.0 ) +
               0.328125 ) - 35.0 / 288.0 ) - 91.0 / 256.0 ) - 7.0 / 768.0 ) +
               7861.0 / 15360.0;
      }
      if ( x < 2.5 )
      {
        return x * ( x * ( x * ( x * ( x * ( 7.0 / 60.0 - x * ( 1.0 / 120.0 ) ) -
               0.65625 ) + 133.0 / 72.0 ) - 2.5703125 ) + 1267.0 / 960.0 ) +
               1379.0 / 7680.0;
      }
      if ( x < 3.5 )
      {
        x -= 3.5;
        x *= x * x;
        return x * x * ( 1.0 / 720.0 );
      }
      return 0.0;

    case 7:
      if ( x < 1.0 )
      {
        double f = x * x;
        return f * ( f * ( f * ( x * ( 1.0 / 144.0 ) - 1.0 / 36.0 ) + 1.0 / 9.0 ) -
               1.0 / 3.0 ) + 151.0 / 315.0;
      }
      if ( x < 2.0 )
      {
        return x * ( x * ( x * ( x * ( x * ( x * ( 0.05 - x * ( 1.0 / 240.0 ) ) -
               7.0 / 30.0 ) + 0.5 ) - 7.0 / 18.0 ) - 0.1 ) - 7.0 / 90.0 ) +
               103.0 / 210.0;
      }
      if ( x < 3.0 )
      {
        return x * ( x * ( x * ( x * ( x * ( x * ( x * ( 1.0 / 720.0 ) -
               1.0 / 36.0 ) + 7.0 / 30.0 ) - 19.0 / 18.0 ) + 49.0 / 18.0 ) -
               23.0 / 6.0 ) + 217.0 / 90.0 ) - 139.0 / 630.0;
      }
      if ( x < 4.0 )
      {
        double f = 4.0 - x;
        x = f * f * f;
        return x * x * f * ( 1.0 / 5040.0 );
      }
      return 0.0;

    default:
      throw std::invalid_argument( "aims::FastBSpline: order " +
                                   carto::toString(order()) +
                                   " not implemented." );
  }
}

double FastBSpline::derivative( double x, unsigned n ) const
{
  if( n == 0 )
    return at(x);
  else
  {
    ASSERT( n <= this->order() );
    // throw std::logic_error( "aims::FastBSpline: derivative not implemented." );

    x = ( this->scale() != 1. ? x / this->scale() : x ) +
        ( this->shifted() ? 0.5 : 0. );
    double sign = ( x < 0 && n % 2 ? -1. /* antisym if odd */
                                   : 1.  /* sym if even */ );
    x = std::abs( x );
    double result = 0;
    bool ok = false;
    switch( this->order() )
    {
      case 1: result = ( x < 1.0 ? -1.0
                                 : 0.0 );
        ok = true;
        break;
      case 2:
        switch( n )
        {
          case 1: result = ( x < 0.5 ? -2.0 * x :
                             x < 1.5 ? x - 1.5
                                     : 0.0 );
            ok = true;
            break;
          case 2: result = ( x < 0.5 ? -2.0 :
                             x < 1.5 ? 1.0
                                     : 0.0 );
            ok = true;
            break;
        }
      case 3:
        switch( n )
        {
          case 1: result = ( x < 1.0 ? 1.5 * x * x - 2.0 * x :
                             x < 2.0 ? -0.5 * ( 2.0 - x ) * ( 2.0 - x )
                                     : 0.0 );
            ok = true;
            break;
          case 2: result = ( x < 1.0 ? 3.0 * x - 2.0 :
                             x < 2.0 ? 2.0 - x
                                     : 0.0 );
            ok = true;
            break;
          case 3: result = ( x < 1.0 ? 3.0 :
                             x < 2.0 ? -1.0
                                     : 0.0 );
            ok = true;
            break;
        }
    default:
      if( !ok )
        throw std::invalid_argument( "aims::FastBSpline: derivative " +
                                     carto::toString(n) +
                                     " of order " +
                                     carto::toString(order()) +
                                     " not implemented." );
    }
    return result * sign;
  }
}

//============================================================================
//
//     B-SPLINE : TABULATED VALUES
//
//============================================================================

TabulBSpline::TabulBSpline( unsigned order, unsigned nder, float scale,
                            bool shifted, size_t length ):
BSpline(order, scale, shifted),
_values( std::min(nder, order) + 1, vector<double>(order ? length + 1 : 0 ) )
{
  setArray(nder, length);
}

TabulBSpline::TabulBSpline( const TabulBSpline & other ):
  BSpline( other ),
  _values( other._values )
{}

TabulBSpline & TabulBSpline::operator= ( const TabulBSpline & other )
{
  if( this != &other )
  {
    BSpline::operator=( other );
    _values = other._values;
  }
  return *this;
}

TabulBSpline::~TabulBSpline()
{}

size_t TabulBSpline::index( double x ) const
{
//   x = ( this->scale() != 1. ? x / this->scale() : x ) +
//         ( this->shifted() ? 0.5 : 0. );
//   return unsigned( std::abs(x) * 2. * double(_values[0].size() - 1) / ( double( this->order() ) + 1 ) );

  return unsigned( std::abs( x * _tablescale + _tableoffset ) );
}

bool TabulBSpline::is_valid( size_t index ) const
{
  return ( /* index >= 0 && */ index < _values[0].size() );
}

void TabulBSpline::setArray( unsigned nder, size_t length )
{
  // Reallocate array if needed
  nder = std::min(nder, this->order());
  size_t old_length = ( _values.size() > 0 ? _values[0].size() : 0 );
  size_t new_length = ( this->order() ? length + 1 : 0 );
  if( new_length == old_length )
  {
    if( nder + 1 > _values.size() )
      _values.resize( nder + 1, vector<double>( new_length ) );
    else if( nder + 1 < _values.size() )
      _values.resize( nder + 1 );
  }
  else
    _values = std::vector<std::vector<double> >( nder + 1, vector<double>( new_length ) );

  // Fill the array with centered & unscaled values.
  if( this->order() )
  {
    double parameter;
    BSpline b(this->order());
    for( size_t i = 0; i <= length; ++i )
    {
      parameter = double(i) * double(this->order() + 1) / ( double(length) * 2 );
      for( unsigned n = 0; n <= nder; ++n )
        _values[n][i] = b.derivative( parameter, n );
    }
  }

  _tablescale
    = 2. * double(_values[0].size() - 1) / ( double( this->order() + 1 ) )
      / scale();
  _tableoffset = ( this->shifted() ? 0.5 : 0. ) / scale();
}

void TabulBSpline::reset( unsigned order, unsigned nder, float scale,
                          bool shifted, size_t length )
{
  BSpline::reset( order, scale, shifted );
  setArray( nder, length );
}

void TabulBSpline::setOrder( unsigned order )
{
  BSpline::setOrder( order );
  setArray( _values.size() - 1, ( _values.size() > 0 ? _values[0].size() - 1 : 0 ) );
}

void TabulBSpline::setNbDer( unsigned nder )
{
  setArray( nder, ( _values.size() > 0 ? _values[0].size() - 1 : 0 ) );
}

void TabulBSpline::setLength( size_t length )
{
  setArray( _values.size() - 1, length );
}

unsigned TabulBSpline::nder() const
{
  return _values.size() - 1;
}

size_t TabulBSpline::length() const
{
  return ( _values.size() > 0 ? _values[0].size() : 0 );
}


double TabulBSpline::at( double x ) const
{
  if( this->order() == 0 )
    return BSplineOrder<0>::spline(x);

  size_t i = index(x);
  if( !is_valid(i) )
    return 0.;
  else
    return _values[0][i];
}

double TabulBSpline::derivative( double x, unsigned n ) const
{
  if( n == 0 )
    return at(x);

  ASSERT( n <= this->order() && n < _values.size() );
  size_t i = index(x);

  x = ( this->scale() != 1. ? x / this->scale() : x ) +
      ( this->shifted() ? 0.5 : 0. ); // for symmetry test

  if( !is_valid(i) )
    return 0.;
  else
    return _values[n][i] * ( x < 0 && n % 2 ? -1. /* antisym if odd */
                                            : 1.  /* sym if even */ );
}

//============================================================================
//
//     DISCRETE B-SPLINE
//
//============================================================================

void DiscreteBSpline::setArray()
{
  // length
  _values.assign( (int)_support[1] - (int)_support[0] + 1, 0. );

  BSpline s( _order, _scale, _shift );
  for( int i = (int)_support[0], j = 0; i <= (int)_support[1]; ++i, ++j )
    _values[j] = s( (double)i );
}

DiscreteBSpline::DiscreteBSpline( unsigned order, float scale, bool shifted ):
  BSpline( order, scale, shifted ),
  _values()
{
  setArray();
}

DiscreteBSpline::DiscreteBSpline( const DiscreteBSpline & other ):
  BSpline( other ),
  _values( other._values )
{}

DiscreteBSpline & DiscreteBSpline::operator= ( const DiscreteBSpline & other )
{
  if( this != &other )
  {
    BSpline::operator=( other );
    _values = other._values;
  }
  return *this;
}

DiscreteBSpline::~DiscreteBSpline()
{}

double DiscreteBSpline::operator() ( int x ) const
{
  return at(x);
}

double DiscreteBSpline::at( int x ) const
{
  return _values[ x - (int)_support[0] ];
}

void DiscreteBSpline::reset( unsigned order, float scale, bool shifted )
{
  BSpline::reset( order, scale, shifted );
  setArray();
}

void DiscreteBSpline::setOrder( unsigned order )
{
  BSpline::setOrder( order );
  setArray();
}

void DiscreteBSpline::setScale( float scale )
{
  BSpline::setScale( scale );
  setArray();
}

void DiscreteBSpline::setShifted( bool shifted )
{
  BSpline::setShifted( shifted );
  setArray();
}

void DiscreteBSpline::setCentered( bool centered )
{
  BSpline::setCentered( centered );
  setArray();
}

void DiscreteBSpline::setSupport()
{
  float trueval, roundval;

  // borne inf
  trueval = - this->scale() * (float)(this->order() + 1) / 2. - ( this->shifted() ? .5 : 0. );
  roundval = std::ceil( trueval );
  _support[0] = (double)( trueval == roundval ? (int)roundval + 1 : (int)roundval );

  // borne sup
  trueval = this->scale() * (float)(this->order() + 1) / 2. - ( this->shifted() ? .5 : 0. );
  roundval = std::floor( trueval );
  _support[1] = (double)( trueval == roundval ? (int)roundval - 1 : (int)roundval );
}
