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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/curve/curve.h>
#include <aims/math/integrator.h>
#include <aims/math/rombergitg.h>
#include <cartobase/config/verbose.h>
#include <cartobase/type/string_conversion.h>

using namespace std;
using namespace carto;

namespace aims {


  //------------//
 //  Polyline  //
//------------//

//-----------------------------------------------------------------------------
Polyline::Scalar_t Polyline::length() const
{
  Scalar_t result = 0;
  vector<Point_t>::const_iterator it = begin();
  if ( it != end() ) {
    const Point_t *pLastPoint = &(*it);
    while( ++it != end() ) {
      result += ( *it - *pLastPoint ).norm();
      pLastPoint = &(*it);
    }
  }
  return result;
}


//-----------------------------------------------------------------------------
bool Polyline::push_back( const Polyline::Point_t &point )
{
  if ( _points.empty() || point != *_points.rbegin() ) {
    _points.push_back( point );
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
void Polyline::resample( size_t numberOfPoints,
                         std::vector< Point_t > &result )
{
  result.clear();
  if ( size() > 1 ) {
    result.reserve( numberOfPoints );
    const Scalar_t totalLength = length();
    Polyline::Scalar_t segmentStart = 0;
    Polyline::const_iterator itCurve = begin();
    const Polyline::Point_t *previousPoint = &(*itCurve);
    ++itCurve;
    Point_t segmentVector = *itCurve - *previousPoint;
    Polyline::Scalar_t segmentLength = segmentVector.norm();
    for( size_t i = 0; i < numberOfPoints; ++i ) {
      const Scalar_t currentLength = Scalar_t( i ) / ( numberOfPoints - 1 ) * 
        totalLength;
      while( currentLength > segmentStart + segmentLength ) {
        segmentStart += segmentLength;
        previousPoint = &(*itCurve);
        ++itCurve;
        if ( itCurve == end() ) {
          result.push_back( *previousPoint );
          if ( result.size() != numberOfPoints ) {
            throw( runtime_error( string( "tried to sample a polyline over "
                                          "its length by " ) + 
                                  toString(  currentLength - segmentStart + 
                                             segmentLength ) + 
                                  " millimeters" ) );
          }
          return;
        }
        segmentVector = *itCurve - *previousPoint,
        segmentLength = segmentVector.norm();
      }
      result.push_back( *previousPoint + segmentVector * 
                        ( ( currentLength - segmentStart ) / segmentLength ) );
    }
  }
}




  //----------------------------------//
 //  PolylinePolynomialInterpolator  //
//----------------------------------//

//-----------------------------------------------------------------------------
/** This class is used with RombergIntegrator API in splineArcLength().
    It computes the curvilinear coordinate of a polynomial curve of third
    degree. This function is integrated to get the actual length of a
    polynomial curve segment.
*/
class SqrtPolynomial4 : public IntegrableOf<Polyline::Scalar_t>
{
public:
  Polyline::Scalar_t a, b, c, d, e;

  inline SqrtPolynomial4( const Polyline::Scalar_t &ca,
                          const Polyline::Scalar_t &cb,
                          const Polyline::Scalar_t &cc,
                          const Polyline::Scalar_t &cd,
                          const Polyline::Scalar_t &ce ) :
    a( ca ), b( cb ), c( cc ), d( cd ), e( ce )
  {}

  Polyline::Scalar_t valueAt( Polyline::Scalar_t x ) const;
};


//-----------------------------------------------------------------------------
Polyline::Scalar_t SqrtPolynomial4::valueAt( Polyline::Scalar_t x ) const
{
  Polyline::Scalar_t xPower = x;
  Polyline::Scalar_t polynomialValue = e + d * xPower;
  polynomialValue += c * (xPower *= x);
  polynomialValue += b * (xPower *= x);
  polynomialValue += a * (xPower *= x);
  return sqrt( polynomialValue );
}


//-----------------------------------------------------------------------------
Polyline::Scalar_t 
splineArcLength( const PolylinePolynomialInterpolator::Spline &si,
                 const Polyline::Scalar_t &from,
                 const Polyline::Scalar_t &to )
{
  SqrtPolynomial4 
    ca( 9 * ( si.a[0]*si.a[0] +  si.a[1]*si.a[1] + 
              si.a[2]*si.a[2] ),
        12 * ( si.a[0] * si.b[0] +  si.a[1] * si.b[1] +
               si.a[2] * si.b[2] ),
        6 * ( si.a[0] * si.c[0] +  si.a[1] * si.c[1] +
              si.a[2] * si.c[2] ) +
        4 * ( si.b[0]*si.b[0] +  si.b[1]*si.b[1] +
              si.b[2]*si.b[2] ),
        4 * ( si.b[0] * si.c[0] +  si.b[1] * si.c[1] +
              si.b[2] * si.c[2] ),
        si.c[0]*si.c[0] +  si.c[1]*si.c[1] +
        si.c[2]*si.c[2] );
  RombergIntegrator integrator( 1e-7, 20, 5 );
  try {
    return integrator.eval( ca, from, to );
  }
  catch( std::exception &e ) {
    return 0;
  }
}


//-----------------------------------------------------------------------------
void PolylinePolynomialInterpolator::reset( const Polyline &line )
{
  // Compute tangents
  vector< Tangent_t > tangents;
  tangents.reserve( line.size() );
  if ( line.size() >= 2 ) {
    tangents.push_back( ( line[ 1 ] - line[ 0 ] ) / Polyline::Scalar_t( 2 ) );
    unsigned i = 0;
    while( ++i < line.size()-1 ) {
      tangents.push_back( ( line[ i-1 ] - line[ i+1 ] ) /
                          Polyline::Scalar_t( 2 )  );
    }
    tangents.push_back( ( line[ i-1 ] - line[ i ] ) / Polyline::Scalar_t( 2 ));
  }
  // Compute splines
  reset( line, tangents );
}


//-----------------------------------------------------------------------------
void PolylinePolynomialInterpolator::reset( const Polyline &line,
                                            const vector<Tangent_t> &tangents )
{
  _splines.clear();
  if ( line.size() > 1 ) {
    // Check line and tangents sizes
    if ( line.size() != tangents.size() ) {
      throw runtime_error( "Error while creating a polyline interpolator: "
                           "line and tangent do not have the same number of "
                           "points" );
    }
    
    // Compute splines
    _splines.reserve( line.size() - 1 );
    for( unsigned i = 0; i < line.size()-1; ++i ) {
      const Polyline::Point_t &p1 = line[ i ];
      const Polyline::Point_t &p2 = line[ i+1 ];
      const Tangent_t &v1 = tangents[ i ];
      const Tangent_t &v2 = tangents[ i+1 ];
      
      _splines[ i ].d = p1;
      _splines[ i ].c = v1;
      _splines[ i ].b = Polyline::Scalar_t( 3 ) * ( p2 - p1 ) - 
        Polyline::Scalar_t( 2 ) * v1 - v2;
      _splines[ i ].a = v1 + v2 + Polyline::Scalar_t( 2 ) * ( p1 - p2 );
      _splines[ i ].length = splineArcLength( _splines[ i ],
                                              0, 1 );
    }
  }
}



//-----------------------------------------------------------------------------
Polyline::Scalar_t PolylinePolynomialInterpolator::length() const
{
  Polyline::Scalar_t result = 0;
  for( vector< Spline >::const_iterator it = _splines.begin();
       it != _splines.end(); ++it ) {
    result += it->length;
  }
  return result;
}


  //-------------------------------------------//
 //  PolylinePolynomialInterpolator::Sampler  //
//-------------------------------------------//

//-----------------------------------------------------------------------------
PolylinePolynomialInterpolator::Sampler::
Sampler( const PolylinePolynomialInterpolator &interpolator,
         Polyline::Scalar_t start ) :
  _interpolator( interpolator ),
  _lengthFromStart( 0 ),
  _splineIndex( 0 ),
  _lengthInSpline( 0 )
{
  if ( start && isValid() ) step( start );
}


//-----------------------------------------------------------------------------
bool PolylinePolynomialInterpolator::Sampler::
isValid() const
{
  return _splineIndex < _interpolator._splines.size();
}


//-----------------------------------------------------------------------------
void PolylinePolynomialInterpolator::Sampler::
step( Polyline::Scalar_t stepLength )
{
  if ( stepLength > 0 ) {
    _lengthFromStart += stepLength;
    _lengthInSpline += stepLength;
    if ( _lengthInSpline > _interpolator._splines[ _splineIndex ].length ) {
      _lengthInSpline -= _interpolator._splines[ _splineIndex ].length;
      ++_splineIndex;
    }
  } else if ( stepLength < 0 ) {
      throw std::logic_error( "negative step not implemented for "
                              "PolylinePolynomialInterpolator" );
  }
}


//-----------------------------------------------------------------------------
Polyline::Point_t PolylinePolynomialInterpolator::Sampler::
point() const
{
  const Spline &spline =  _interpolator._splines[ _splineIndex ];
  const Polyline::Scalar_t param = _lengthInSpline / spline.length;
  return 
    spline.d + 
    spline.c * param + 
    spline.b * param * param +
    spline.a * param * param * param;
}


//-----------------------------------------------------------------------------
PolylinePolynomialInterpolator::Tangent_t
PolylinePolynomialInterpolator::Sampler::
tangent() const
{
  const Spline &spline =  _interpolator._splines[ _splineIndex ];
  const Polyline::Scalar_t param = _lengthInSpline / spline.length;
  return 
    spline.c + 
    Polyline::Scalar_t( 2 ) * spline.b * param +
    Polyline::Scalar_t( 3 ) * spline.a * param * param;
}


//-----------------------------------------------------------------------------
Polyline::Scalar_t  PolylinePolynomialInterpolator::Sampler::
curvature() const
{
  return normal().norm();
}


//-----------------------------------------------------------------------------
PolylinePolynomialInterpolator::Tangent_t 
PolylinePolynomialInterpolator::Sampler::
normal() const
{
  const PolylinePolynomialInterpolator::Tangent_t t = tangent();
  if ( t[ 0 ] ) {
    return crossed( t, PolylinePolynomialInterpolator::Tangent_t( 0, 1, 0 ) );
  }
  return crossed( t, PolylinePolynomialInterpolator::Tangent_t( 1, 0, 0 ) );
}

} // namespace aims
