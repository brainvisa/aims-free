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
