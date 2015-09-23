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

/*
 *  Elementary math functions.
 */
#ifndef AIMS_MATH_MATHELEM_H
#define AIMS_MATH_MATHELEM_H

#include <cstdlib>
#include <aims/config/aimsdata_config.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>
#include <limits>

//============================================================================
//   BASIC OPERATIONS
//============================================================================
namespace aims
{

  /**@name Square, cube, ...
   */
  //@{
  /// Get the square of x
  template <typename T> inline T sqr( const T & x)
  {
    return x * x;
  }

  /// Get the cube of x
  template <typename T> inline T cub( const T & x )
  {
    return x * x * x;
  }

  /// Get the sign of x
  template <typename T> inline float sgn( const T & x )
  {
    return x < 0 ? -1.0f : +1.0f;
  }

  /// Get the absolute difference between 2 values
  /// without having to take care of used type
  /// this is particularly useful for unsigned types
  template <typename T>
  inline T absdiff( const T& x, const T& y )
  {
    return std::abs(static_cast<double>(y) - static_cast<double>(x));
  }

  //@}
}

//============================================================================
//   OPERATIONS ON ITERATED VALUES
//============================================================================
namespace aims {

  // helper function in anonymous namespace
  namespace {
    template <typename T>
    bool is_min( T a, T b )
    {
      return a < b;
    }
  }

  /// Defines basic math functions that run on iterators.
  /// They can be used with any iterators pointing to numeric values
  /// \code
  /// #include <aims/math/matheleme.h>
  /// #include <cartodata/volume/volume.h>
  /// using namespace aims;
  /// using namespace carto;
  ///
  /// int main()
  /// {
  ///   Volume<int16_t> image;
  ///   int16_t med = MathUtil<int16_t>::median( image.begin(), image.end() );
  /// }
  /// \endcode
  template <typename T>
  class MathUtil
  {
  public:
    template<typename Iterator>
    static T max( Iterator b, Iterator e, T default_value = std::numeric_limits<T>::min() )
    {
      Iterator m;
      m = std::max_element( b, e, is_min<T> );
      if( m == e )
        return default_value;
      else
        return T(*m);
    }

    template <typename Iterator>
    static T min( Iterator b, Iterator e, T default_value = std::numeric_limits<T>::max() )
    {
      Iterator m;
      m = std::min_element( b, e, is_min<T> );
      if( m == e )
        return default_value;
      else
        return T(*m);
    }

    template <typename Iterator>
    static T median( Iterator b, Iterator e, T default_value = (T)0 )
    {
      Iterator i;
      std::vector<T> values;
      for( i=b; i!=e; ++i )
        values.push_back( (T) *i );

      if( values.size() == 0 )
        return default_value;
      std::sort( values.begin(), values.end() );
      return values[ values.size() / 2 ];
    }

    template<typename Iterator>
    static T notnull_median( Iterator b, Iterator e, T default_value = (T)0 )
    {
      Iterator i;
      std::vector<T> values;
      for( i=b; i!=e; ++i )
        if( *i != 0 )
          values.push_back( *i );

      if( values.size() == 0 )
        return default_value;
      std::sort( values.begin(), values.end() );
      return values[ values.size() / 2 ];
    }

    template <typename Iterator>
    static T mean( Iterator b, Iterator e )
    {
      Iterator i = b;
      double sum = 0.0;
      uint32_t count = 0;

      // Sum values and count values
      for( ; i != e; ++i) {
        sum += (double)(*i);
        ++count;
      }
      return (count != 0 ? (T)( sum / count ) : (T)0);
    }

    template <typename Iterator>
    static T notnull_mean( Iterator b, Iterator e )
    {
      Iterator i;
      double sum = 0.0;
      uint32_t count = 0;

      // Sum values and count values
      for( i=b; i!=e; ++i )
        if( *i != 0 ) {
          sum += (double)(*i);
          ++count;
        }
      return (count != 0 ? (T)( sum / count ) : (T)0);
    }

    template <typename Iterator>
    static T majority( Iterator b, Iterator e, T default_value = (T)0 )
    {
      Iterator i;
      T currentclass, majorityclass = default_value;
      uint32_t currentclasscases = 0, majoritycases = 0;
      std::map<T, uint32_t> classcases;

      // Goes through the data and count number of values for each class
      for( i=b; i!=e; ++i )
      {
        currentclass = (*i);

        if ( !classcases[ currentclass ] )
        {
            classcases[ currentclass ] = 1;
            currentclasscases = 1;
        }
        else
        {
            currentclasscases = classcases[ currentclass ] + 1;
            classcases[ currentclass ] = currentclasscases;
        }

        if (currentclasscases > majoritycases)
        {
            // Set the new majority cases and class for which it occurs
            majorityclass = currentclass;
            majoritycases = currentclasscases;
        }
      }

      return majorityclass;
    }

    template <typename Iterator>
    static T extrema_difference( Iterator b, Iterator e )
    {
      T mi, mx;
      mi = min( b, e );
      mx = max( b, e );
      return absdiff<T>( mx, mi );
    }

    template <typename Iterator>
    static T sum( Iterator b, Iterator e )
    {
      Iterator i;
      double accumulate = 0.0;

      // Goes through the data and sum values
      for( i=b; i!=e; ++i )
        accumulate += (double)(*i);
      return (T)accumulate;
    }

    template <typename Iterator>
    static T variance( Iterator b, Iterator e )
    {
      Iterator i;
      double accumulate = 0.0;
      double accumulateSq = 0.0;
      uint32_t count = 0;

      for( i=b; i!=e; ++i ) {
        accumulate += (double)(*i);
        accumulateSq += ( (double)(*i) * (double)(*i) );
        ++count;
      }

      double dcount = (double) count;
      return (T)  ( accumulateSq / dcount -
                    ( accumulate / dcount ) *
                    ( accumulate / dcount ) );
    }

    template <typename Iterator>
    static T stdev( Iterator b, Iterator e )
    {
      Iterator i;
      double accumulate = 0.0;
      double accumulateSq = 0.0;
      uint32_t count = 0;

      for( i=b; i!=e; ++i ) {
        accumulate += (double)(*i);
        accumulateSq += ( (double)(*i) * (double)(*i) );
        ++count;
      }

      double dcount = (double) count;
      return (T) std::sqrt( accumulateSq / dcount -
                            ( accumulate / dcount ) *
                            ( accumulate / dcount ) );
    }

  }; // MathUtil

} // namespace aims

//============================================================================
//   TRIGONOMETRY
//============================================================================
/**@name Trigonometry*/
//@{
/// Get the argument of a complex re + i.im
template <class T>
double AimsArgument(T re,T im);
//@}

template <class T> inline
double AimsArgument(T re,T im)
{
  double re2=(double)re;
  double im2=(double)im;

  if ((!re2) && (!im2)) return(0);
  if (!re2)
  {
    if (im2>=0)        return M_PI/2;
    else               return -M_PI/2;
  }
  if (re2>0)           return std::atan(im2/re2);
  else if (im2>=0)     return std::atan(im2/re2)+M_PI;
  else                 return std::atan(im2/re2)-M_PI;
}


//============================================================================
//   BASIC OPERATIONS
//============================================================================
template <class T> inline
T square(const T& val)
{
  return val*val;
}


template <class T> inline
T cube(const T& val)
{
  return val*val*val;
}


template< class T > inline
T pythagore( const T& a, const T& b )
{
  T c, d;

  c = (T)std::fabs( a );
  d = (T)std::fabs( b );

  if ( c > d )
    return c * (T)std::sqrt( (T)1 + square( d / c ) );
  else
    return ( d == (T)0 ) ? (T)0 : d * (T)std::sqrt( (T)1 + square( c / d ) );
}


/**@name Miscellaneous functions*/
//@{
/// Return the next power of two closest to an integer
AIMSDATA_API int AimsNextPowerOfTwo(int number);
/// Sigmoidal function (useful for neural networks)
AIMSDATA_API double AimsSigmoid(double x);
//@}

#endif
