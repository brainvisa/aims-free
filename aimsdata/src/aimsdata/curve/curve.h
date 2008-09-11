/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef AIMS_CURVE_CURVE_H
#define AIMS_CURVE_CURVE_H

#include <aims/vector/vector.h>
#include <cartobase/smart/rcptr.h>
#include <vector>
#include <exception>

namespace aims
{

 
  //------------//
 //  Polyline  //
//------------//

class Polyline
{
public:

  typedef float Scalar_t;
  typedef AimsVector< Scalar_t, 3 > Point_t;
  typedef std::vector< Point_t >::const_iterator const_iterator;

  /** Return the number of points
   */
  inline size_t size() const;
  /** Add a point if it is different from the last point. Return true if
      the point has been added.
  */
  bool push_back( const Point_t & );
  /** Forward iterator on the first point
   */
  inline const_iterator begin() const;

  /** Forward iterator after the last point
   */
  inline const_iterator end() const;
  /** Return the point corresponding to an index
   */
  inline const Point_t &operator []( size_t ) const;
  /** Reserve space for the points (does not change the points in the line).
      There is no read/write (i.e. non const) version of this method because
      it is important to check that two identical points are not added in the
      polyline. It avoids possible crash when interpolating the polyline.
  */
  inline void reserve( size_t );
  /** Remove all points
   */
  inline void clear();

  /** Compute the line's length (sum of every segment's length)
   */
  Scalar_t length() const;

  /** Resample the Polyline and fills a vector with result.
   */
  void resample( size_t numberOfPoints, std::vector< Point_t > & );

private:

  std::vector< Point_t > _points;
};



  //----------------------------------//
 //  PolylinePolynomialInterpolator  //
//----------------------------------//

class PolylinePolynomialInterpolator
{
public:

  typedef Polyline::Point_t Tangent_t;

  inline PolylinePolynomialInterpolator( const Polyline & );
  inline PolylinePolynomialInterpolator( const Polyline &,
                                         const std::vector< Tangent_t > & );
  void reset( const Polyline & );
  void reset( const Polyline &, 
              const std::vector< Tangent_t > & );

  Polyline::Scalar_t length() const;

  class Sampler {
  public:
    bool isValid() const;
    void step( Polyline::Scalar_t );
    Polyline::Point_t point() const;
    PolylinePolynomialInterpolator::Tangent_t tangent() const;
    Polyline::Scalar_t curvature() const;
    PolylinePolynomialInterpolator::Tangent_t normal() const;

  private:
    friend class PolylinePolynomialInterpolator;

    Sampler( const PolylinePolynomialInterpolator &,
             Polyline::Scalar_t start );

    const PolylinePolynomialInterpolator &_interpolator;
    Polyline::Scalar_t _lengthFromStart;
    unsigned _splineIndex;
    Polyline::Scalar_t _lengthInSpline;

  };

  inline Sampler sampler( Polyline::Scalar_t start = 0 ) const;


private:

  struct Spline {
    Tangent_t a, b, c, d;
    Polyline::Scalar_t length;
  };

  friend Polyline::Scalar_t 
  splineArcLength( const Spline &si,
                   const Polyline::Scalar_t &from,
                   const Polyline::Scalar_t &to );
  friend class Sampler;

  std::vector< Spline > _splines;
};



  //------------//
 //  Polyline  //
//------------//

//-----------------------------------------------------------------------------
inline size_t Polyline::size() const
{
  return _points.size();
}


//-----------------------------------------------------------------------------
inline Polyline::const_iterator Polyline::begin() const
{
  return _points.begin();
}


//-----------------------------------------------------------------------------
inline Polyline::const_iterator Polyline::end() const
{
  return _points.end();
}


//-----------------------------------------------------------------------------
inline const Polyline::Point_t &Polyline::operator []( size_t index ) const
{
  return _points[ index ];
}


//-----------------------------------------------------------------------------
inline void Polyline::reserve( size_t space )
{
  return _points.reserve( space );
}


//-----------------------------------------------------------------------------
inline void Polyline::clear()
{
  return _points.clear();
}


  //----------------------------------//
 //  PolylinePolynomialInterpolator  //
//----------------------------------//

//-----------------------------------------------------------------------------
inline PolylinePolynomialInterpolator::PolylinePolynomialInterpolator
( const Polyline &line )
{
  reset( line );
}

//-----------------------------------------------------------------------------
inline PolylinePolynomialInterpolator::PolylinePolynomialInterpolator
( const Polyline &line,
  const std::vector< Tangent_t > &tangents )
{
  reset( line, tangents );
}


//-----------------------------------------------------------------------------
inline PolylinePolynomialInterpolator::Sampler
PolylinePolynomialInterpolator::sampler( Polyline::Scalar_t start ) const
{
  return Sampler( *this, start );
}



} // namespace aims

#endif // ifndef AIMS_CURVE_H
