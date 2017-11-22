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
 *  AIMS vector class
 */
#ifndef AIMS_VECTOR_VECTOR_H
#define AIMS_VECTOR_VECTOR_H

#include <aims/def/assert.h>
#include <aims/def/general.h>
#include <soma-io/utilities/asciidatasourcetraits.h>
#include <cartobase/type/types.h>
#include <cartobase/type/datatypeinfo.h>
#include <fstream>
#include <math.h>
#include <cartobase/object/object.h>
#include <vector>

template <class T,int D> class AimsVector;


template <class T,int D>
bool operator == (const AimsVector<T,D>& v1,
		  const AimsVector<T,D>& v2);

template <class T,int D>
bool operator == (const AimsVector<T,D>& v1,
		  const T& val);

/// should be defined implicitely, but fails with some compilers (Intel)
template <class T,int D>
bool operator != (const AimsVector<T,D>& v1,
		  const AimsVector<T,D>& v2)
{
  return !( v1 == v2 );
}

/// should be defined implicitely, but fails with some compilers (Intel)
template <class T,int D>
bool operator != (const AimsVector<T,D>& v1,
		  const T& val)
{
  return !( v1 == val );
}

template <class T,int D>
AimsVector<T,D> operator + (const AimsVector<T,D>& v1,
                            const AimsVector<T,D>& v2);

template <class T,int D>
AimsVector<T,D> operator - (const AimsVector<T,D>& v1,
                            const AimsVector<T,D>& v2);

template <class T,int D>
AimsVector<T,D> operator * (const AimsVector<T,D>& v1,
                            double value );

template <class T,int D>
AimsVector<T,D> operator * (double value,
                            const AimsVector<T,D>& v1);

template <class T,int D>
AimsVector<T,D> operator / (const AimsVector<T,D>& v1,
                            double value);

template <class T,int D>
AimsVector<T,D> operator + (const AimsVector<T,D>& v1);

template <class T,int D>
AimsVector<T,D> operator - (const AimsVector<T,D>& v1);

template <class T,int D>
AimsVector<T,3> crossed(const AimsVector<T,D>& v1,
                        const AimsVector<T,D>& v2);

template <class T,int D>
float norm(const AimsVector<T,D>& v1);

template <class T,int D>
float norm2(const AimsVector<T,D>& v1);

template <class T,int D>
double dnorm(const AimsVector<T,D>& v1);

template <class T,int D>
double dnorm2(const AimsVector<T,D>& v1);

template <class T,int D>
std::ostream& operator << (std::ostream& out,
			   const AimsVector<T,D>& thing);

template <class T,int D>
std::istream& operator >> (std::istream& in,
			   AimsVector<T,D>& thing);


/** The template class to implement basic vectors.
    The first template argument provides the type of stored items and
    the second template argument provides the size of the vector.
*/
template <class T,int D>
class AimsVector
{
  public:
    /**@name Constructors and Destructor*/
    //@{
    /// The constructor allocates an appropriate amount of memory
    AimsVector();
    explicit AimsVector( const std::vector<T> & value );
    /// The constructor fills the D items of vector with value
    AimsVector(const T& value);
    /// The constructor copy the D item of value[]
    explicit AimsVector(const T value[]);
    /// Constructor for D=2
    AimsVector(const T& x,const T& y);
    /// Constructor for D=3
    AimsVector(const T& x,const T& y,const T& z);
    /// Constructor for D=4
    AimsVector(const T& x,const T& y,const T& z,const T& t);
    /// Copy constructor
    AimsVector(const AimsVector<T,D>& other);
    /// Type conversion
    template<class U>
    explicit AimsVector(const AimsVector<U,D>& other);

    /// The destructor deletes the allocated memory space
    ~AimsVector();
    //@}

    /**@name Manipulators and methods*/
    //@{
    AimsVector<T,D>& operator = (const AimsVector<T,D>& other);
    AimsVector<T,D>& operator += (const AimsVector<T,D>& other);
    AimsVector<T,D>& operator -= (const AimsVector<T,D>& other);
    AimsVector<T,D>& operator *= ( double val );
    AimsVector<T,D>& operator /= ( double val );
    AimsVector<T,D>& normalize();

    /// Get a const reference to the dth item
    const T& item(int d) const;
    /// Get a const reference to the dth item
    const T& operator[](int d) const;
    /// Get a non const reference to the dth item
    T& item(int d);
    /// Get a const reference to the dth item
    T& operator[](int d);

    int size() const;
    /// Test if the vector is the null vector
    bool isNull() const;
    /// Return the magnitude of the vector
    float norm() const;
    /// Return the square magnitude of the vector
    float norm2() const;
    /// Return the magnitude of the vector
    double dnorm() const;
    /// Return the square magnitude of the vector
    double dnorm2() const;
    /// Get the dot product between 2 vectors
    T dot(const AimsVector<T,D>& other) const;

    //@}

    /// @name Iterators
    /// @{
    typedef T *        iterator;
    typedef const T *  const_iterator;
    iterator begin() { return _value; }
    const_iterator begin() const { return _value; }
    iterator end() { return _value + D; }
    const_iterator end() const { return _value + D; }
    /// @}

    std::vector<T> toStdVector() const;

  protected:
    /// Memory space allocated
    T _value[D];
};


/**@name Useful type definitions*/
//@{
// Keep those for compability
typedef AimsVector<int16_t,2> Point2d;
typedef AimsVector<int16_t,3> Point3d;
typedef AimsVector<int16_t,4> Point4d;
typedef AimsVector<uint32_t,2> Point2du;
typedef AimsVector<uint32_t,3> Point3du;
typedef AimsVector<uint32_t,4> Point4du;
// 2016: l now means long (64bit), i int (32bit), s short (16bit)
typedef AimsVector<int16_t,2> Point2ds;
typedef AimsVector<int16_t,3> Point3ds;
typedef AimsVector<int16_t,4> Point4ds;
typedef AimsVector<int32_t,2> Point2di;
typedef AimsVector<int32_t,3> Point3di;
typedef AimsVector<int32_t,4> Point4di;
typedef AimsVector<int64_t,2> Point2dl;
typedef AimsVector<int64_t,3> Point3dl;
typedef AimsVector<int64_t,4> Point4dl;
typedef AimsVector<uint16_t,2> Point2dus;
typedef AimsVector<uint16_t,3> Point3dus;
typedef AimsVector<uint16_t,4> Point4dus;
typedef AimsVector<uint32_t,2> Point2dui;
typedef AimsVector<uint32_t,3> Point3dui;
typedef AimsVector<uint32_t,4> Point4dui;
typedef AimsVector<uint64_t,2> Point2dul;
typedef AimsVector<uint64_t,3> Point3dul;
typedef AimsVector<uint64_t,4> Point4dul;
typedef AimsVector<float,2> Point2df;
typedef AimsVector<float,3> Point3df;
typedef AimsVector<float,4> Point4df;
typedef AimsVector<double,2> Point2dd;
typedef AimsVector<double,3> Point3dd;
typedef AimsVector<double,4> Point4dd;
//@}


extern template class AimsVector<int16_t,2>;
extern template class AimsVector<int16_t,3>;
extern template class AimsVector<int16_t,4>;
extern template class AimsVector<int32_t,2>;
extern template class AimsVector<int32_t,3>;
extern template class AimsVector<int32_t,4>;
extern template class AimsVector<int64_t,2>;
extern template class AimsVector<int64_t,3>;
extern template class AimsVector<int64_t,4>;
extern template class AimsVector<uint16_t,2>;
extern template class AimsVector<uint16_t,3>;
extern template class AimsVector<uint16_t,4>;
extern template class AimsVector<uint32_t,2>;
extern template class AimsVector<uint32_t,3>;
extern template class AimsVector<uint32_t,4>;
extern template class AimsVector<uint64_t,2>;
extern template class AimsVector<uint64_t,3>;
extern template class AimsVector<uint64_t,4>;
extern template class AimsVector<float,2>;
extern template class AimsVector<float,3>;
extern template class AimsVector<float,4>;
extern template class AimsVector<double,2>;
extern template class AimsVector<double,3>;
extern template class AimsVector<double,4>;

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{
DECLARE_GENERIC_OBJECT_TYPE( Point2ds )
DECLARE_GENERIC_OBJECT_TYPE( Point3ds )
DECLARE_GENERIC_OBJECT_TYPE( Point4ds )
DECLARE_GENERIC_OBJECT_TYPE( Point2di )
DECLARE_GENERIC_OBJECT_TYPE( Point3di )
DECLARE_GENERIC_OBJECT_TYPE( Point4di )
DECLARE_GENERIC_OBJECT_TYPE( Point2dl )
DECLARE_GENERIC_OBJECT_TYPE( Point3dl )
DECLARE_GENERIC_OBJECT_TYPE( Point4dl )
DECLARE_GENERIC_OBJECT_TYPE( Point2dus )
DECLARE_GENERIC_OBJECT_TYPE( Point3dus )
DECLARE_GENERIC_OBJECT_TYPE( Point4dus )
DECLARE_GENERIC_OBJECT_TYPE( Point2dui )
DECLARE_GENERIC_OBJECT_TYPE( Point3dui )
DECLARE_GENERIC_OBJECT_TYPE( Point4dui )
DECLARE_GENERIC_OBJECT_TYPE( Point2dul )
DECLARE_GENERIC_OBJECT_TYPE( Point3dul )
DECLARE_GENERIC_OBJECT_TYPE( Point4dul )
DECLARE_GENERIC_OBJECT_TYPE( Point2df )
DECLARE_GENERIC_OBJECT_TYPE( Point3df )
DECLARE_GENERIC_OBJECT_TYPE( Point4df )
DECLARE_GENERIC_OBJECT_TYPE( Point2dd )
DECLARE_GENERIC_OBJECT_TYPE( Point3dd )
DECLARE_GENERIC_OBJECT_TYPE( Point4dd )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point2ds> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point3ds> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point4ds> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point2di> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point3di> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point4di> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point2dl> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point3dl> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point4dl> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point2dus> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point3dus> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point4dus> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point2dui> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point3dui> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point4dui> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point2dul> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point3dul> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point4dul> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point2df> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point3df> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point4df> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point2dd> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point3dd> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<Point4dd> )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point2ds> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point3ds> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point4ds> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point2di> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point3di> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point4di> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point2dl> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point3dl> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point4dl> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point2dus> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point3dus> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point4dus> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point2dui> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point3dui> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point4dui> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point2dul> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point3dul> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point4dul> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point2df> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point3df> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point4df> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point2dd> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point3dd> > )
DECLARE_GENERIC_OBJECT_TYPE( std::vector<std::vector<Point4dd> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point2ds> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point3ds> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point4ds> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point2di> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point3di> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point4di> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point2dl> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point3dl> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point4dl> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point2dus> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point3dus> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point4dus> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point2dui> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point3dui> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point4dui> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point2dul> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point3dul> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point4dul> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point2df> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point3df> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point4df> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point2dd> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point3dd> > )
DECLARE_GENERIC_OBJECT_TYPE( std::list<std::vector<Point4dd> > )

  // partial specializations

  template<class T, int D> class DataTypeCode<AimsVector<T,D> >
  {
  public:
    static std::string objectType()
    { return( "Vector" ); }
    static std::string dataType()
    {
      char	num[10];
      sprintf( num, "%d", (int) D );
      return( std::string( "VECTOR_OF_" ) + num + "_" +
              DataTypeCode<T>::dataType() );
    }
    static std::string name()
    {
      return DataTypeCode<AimsVector<T,D> >::dataType();
    }
  };

  template<> inline std::string DataTypeCode<Point2df>::dataType()
  {
    return "POINT2DF";
  }

  template<> inline std::string DataTypeCode<Point3df>::dataType()
  {
    return "POINT3DF";
  }

}

// DataTypeInfo

template<typename T, int D>
class DataTypeInfo<AimsVector<T,D> >
{
  public :

    static inline const uint8_t samples() { return D; }
    static inline const uint8_t depth()
    { return sizeof(T); }
    static inline const std::string channelType()
    { return carto::DataTypeCode<AimsVector<T,D> >::dataType(); }
};

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


//
// Members of class AimsVector<T,D>
//
template <class T,int D>
inline
AimsVector<T,D>::AimsVector()
{
}


template <class T,int D>
inline
AimsVector<T,D>::AimsVector(const T& value)
{
  for (int d = 0; d < D; d++)
    _value[d] = value;
}


template <class T,int D>
inline
AimsVector<T,D>::AimsVector(const T value[])
{
  for (int d = 0; d < D; d++)
    _value[d] = value[d];
}

template <class T, int D>
inline
AimsVector<T,D>::AimsVector( const std::vector<T> & value )
{
  int d, N = std::min( typename std::vector<T>::size_type( D ), value.size() );
  for( d = 0; d < N; ++d )
    _value[d] = value[d];
  for( ; d<D; ++d )
    _value[d] = T(0);
}

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES
namespace internal
{
  template <typename T, int D>
  class fill_aimsvector2
  {
  public:
    inline static void doit( T _value[D], const T& x, const T& y )
    {
      _value[0] = x;
      _value[1] = y;
      for (int d = 2; d < D; d++)
        _value[d] = 0;
    }
  };

  template <typename T, int D>
  class fill_aimsvector3
  {
  public:
    inline static void doit( T _value[D], const T& x, const T& y, const T& z )
    {
      _value[0] = x;
      _value[1] = y;
      _value[2] = z;
      for (int d = 3; d < D; d++)
        _value[d] = 0;
    }
  };

  template <typename T, int D>
  class fill_aimsvector4
  {
  public:
    inline static void doit( T _value[D], const T& x, const T& y, const T& z,
                             const T& t )
    {
      _value[0] = x;
      _value[1] = y;
      _value[2] = z;
      _value[3] = t;
      for (int d = 4; d < D; d++)
        _value[d] = 0;
    }
  };

  template <typename T>
  class fill_aimsvector2<T, 1>
  {
  public:
    inline static void doit( T _value[1], const T& x, const T& )
    {
      _value[0] = x;
    }
  };

  template <typename T>
  class fill_aimsvector3<T, 1>
  {
  public:
    inline static void doit( T _value[1], const T& x, const T&, const T& )
    {
      _value[0] = x;
    }
  };

  template <typename T>
  class fill_aimsvector4<T, 1>
  {
  public:
    inline static void doit( T _value[1], const T& x, const T&, const T&,
                             const T& )
    {
      _value[0] = x;
    }
  };

  template <typename T>
  class fill_aimsvector3<T, 2>
  {
  public:
    inline static void doit( T _value[2], const T& x, const T& y, const T& )
    {
      _value[0] = x;
      _value[1] = y;
    }
  };

  template <typename T>
  class fill_aimsvector4<T, 2>
  {
  public:
    inline static void doit( T _value[2], const T& x, const T& y, const T&,
                             const T& )
    {
      _value[0] = x;
      _value[1] = y;
    }
  };

  template <typename T>
  class fill_aimsvector4<T, 3>
  {
  public:
    inline static void doit( T _value[3], const T& x, const T& y, const T& z,
                             const T& )
    {
      _value[0] = x;
      _value[1] = y;
      _value[2] = z;
    }
  };

}
#endif

template <class T,int D>
inline
AimsVector<T,D>::AimsVector(const T& x, const T& y)
{
  ::internal::fill_aimsvector2<T,D>::doit( _value, x, y );
}


template <class T,int D>
inline
AimsVector<T,D>::AimsVector(const T& x, const T& y, const T& z)
{
  ::internal::fill_aimsvector3<T,D>::doit( _value, x, y, z );
}


template <class T,int D>
inline
AimsVector<T,D>::AimsVector(const T& x, const T& y, const T& z, const T& t)
{
  ::internal::fill_aimsvector4<T,D>::doit( _value, x, y, z, t );
}


template <class T,int D>
inline
AimsVector<T,D>::AimsVector(const AimsVector<T,D>& other)
{
  for (int d = 0; d < D; d++)
    _value[d] = other._value[d];
}

template <class T,int D> template<class U>
inline
AimsVector<T,D>::AimsVector(const AimsVector<U,D>& other)
{
  for (int d = 0; d < D; d++)
    _value[d] = other._value[d];
}


template <class T,int D>
inline
AimsVector<T,D>& AimsVector<T,D>::operator = (const AimsVector<T,D>& other)
{
  if (this == &other)
    return *this;
  for (int d = 0; d < D; d++)
    _value[d] = other._value[d];
  return *this;
}


template <class T,int D>
inline
AimsVector<T,D>::~AimsVector()
{
}


template <class T,int D>
inline
AimsVector<T,D>& AimsVector<T,D>::operator += (const AimsVector<T,D>& other)
{
  for (int d = 0;d < D; d++)
    _value[d] += other._value[d];
  return *this;
}


template <class T,int D>
inline
AimsVector<T,D>& AimsVector<T,D>::operator -= (const AimsVector<T,D>& other)
{
  for (int d = 0; d < D; d++)
    _value[d] -= other._value[d];
  return *this;
}


template <class T,int D>
inline
AimsVector<T,D>& AimsVector<T,D>::operator *= ( double val )
{
  for (int d = 0; d < D; d++)
    _value[d] = (T) ( _value[d] * val );
  return *this;
}


template <class T,int D>
inline
AimsVector<T,D>& AimsVector<T,D>::operator /= ( double val )
{
  for (int d = 0; d < D; d++)
    _value[d] = (T) ( _value[d] / val );
  return *this;
}


template <class T,int D>
inline
AimsVector<T,D>& AimsVector<T,D>::normalize()
{
  return operator /=( norm() );
}


template <class T,int D>
inline
const T& AimsVector<T,D>::item(int d) const
{
  ASSERT(d < D);
  return _value[d];
}


template <class T,int D>
inline
const T& AimsVector<T,D>::operator[](int d) const
{
  ASSERT(d < D);
  return _value[d];
}


template <class T,int D>
inline
T& AimsVector<T,D>::item(int d)
{
  ASSERT(d < D);
  return _value[d];
}


template <class T,int D>
inline
T& AimsVector<T,D>::operator[](int d)
{
  ASSERT(d < D);
  return _value[d];
}

template <class T,int D>
inline
int AimsVector<T,D>::size() const
{
  return D;
}

template <class T,int D>
inline
bool AimsVector<T,D>::isNull() const
{
  for (int d = 0; d < D; ++d) {
    if ( _value[ d ] ) return false;
  }
  return true;
}

template <class T,int D>
inline
float AimsVector<T,D>::norm() const
{
  float result = 0;
  for (int d = 0; d < D; ++d)
    result += float(item(d)) * float(item(d));
  return sqrt(result);
}

template <class T,int D>
inline
float AimsVector<T,D>::norm2() const
{
  float result = 0;
  for (int d = 0; d < D; ++d)
    result += float(item(d)) * float(item(d));
  return result;
}

template <class T,int D>
inline
double AimsVector<T,D>::dnorm() const
{
  double result = 0;
  for (int d = 0; d < D; ++d)
    result += double(item(d)) * double(item(d));
  return sqrt(result);
}

template <class T,int D>
inline
double AimsVector<T,D>::dnorm2() const
{
  double result = 0;
  for (int d = 0; d < D; ++d)
    result += double(item(d)) * double(item(d));
  return result;
}


template <class T,int D>
inline
T AimsVector<T,D>::dot(const AimsVector<T,D>& other) const
{
  T result = (T)0;
  for (int d = 0; d < D; ++d)
    result += item(d) * other.item(d);
  return result;
}


template <class T,int D>
inline
std::vector<T> AimsVector<T, D>::toStdVector() const
{
  std::vector<T> vec( D );
  int i;
  for( i=0; i<D; ++i )
    vec[i] = item(i);
  return vec;
}


//
//  Friend functions
//

template <class T,int D>
inline
bool operator== (const AimsVector<T,D>& v1,
                 const AimsVector<T,D>& v2)
{
  for (int d = 0; d < D; ++d)
    if (v1.item(d) != v2.item(d))
      return false;
  return true;
}


template <class T,int D>
inline
bool operator== (const AimsVector<T,D>& v1,
                 const T& value)
{
  for (int d = 0; d < D; ++d)
    if (v1[d] != value)
      return false;
  return true;
}


template <class T,int D>
inline
AimsVector<T,D> operator+ (const AimsVector<T,D> &v1,
                            const AimsVector<T,D> &v2)
{
  AimsVector<T,D> result;
  for (int d = 0; d < D; ++d)
    result[d] = v1[d] + v2[d];
  return result;
}


template <class T,int D>
inline
AimsVector<T,D> operator- (const AimsVector<T,D>& v1,
                            const AimsVector<T,D>& v2)
{
  AimsVector<T,D> result;
  for (int d = 0; d < D; ++d)
    result[d] = v1[d] - v2[d];
  return result;
}


template <class T,int D>
inline
AimsVector<T,D> operator * ( const AimsVector<T,D>& v1,
                             double value )
{
  AimsVector<T,D> result;
  for (int d = 0; d < D; ++d)
    result[d] = (T) ( v1[d] * value );
  return result;
}


template <class T,int D>
inline
AimsVector<T,D> operator * ( double value,
                             const AimsVector<T,D>& v1 )
{
  AimsVector<T,D> result;
  for (int d = 0; d < D; ++d)
    result[d] = (T) ( v1[d] * value );
  return result;
}


template <class T,int D>
inline
AimsVector<T,D> operator / ( const AimsVector<T,D>& v1,
                             double value )
{
  AimsVector<T,D> result;
  for (int d = 0; d < D; ++d)
    result[d] = (T) ( v1[d] / value );
  return result;
}


template <class T,int D>
inline
AimsVector<T,D> operator+ (const AimsVector<T,D> &v1)
{
  return v1;
}


template <class T,int D>
inline
AimsVector<T,D> operator- (const AimsVector<T,D>& v1)
{
  AimsVector<T,D> result;
  for (int d = 0; d < D; ++d)
    result[d] = -v1[d];
  return result;
}


template <class T,int D>
inline
AimsVector<T,3> crossed(const AimsVector<T,D>& v1,
                        const AimsVector<T,D>& v2)
{
  AimsVector<T,3> result;
  result[0] = v1[1] * v2[2] - v1[2] * v2[1];
  result[1] = v1[2] * v2[0] - v1[0] * v2[2];
  result[2] = v1[0] * v2[1] - v1[1] * v2[0];
  return result;
}


template <class T,int D>
inline
float norm(const AimsVector<T,D>& v1)
{
  return v1.norm();
}


template <class T,int D>
inline
float norm2(const AimsVector<T,D>& v1)
{
  return v1.norm2();
}

template <class T,int D>
inline
double dnorm(const AimsVector<T,D>& v1)
{
  return v1.dnorm();
}


template <class T,int D>
inline
double dnorm2(const AimsVector<T,D>& v1)
{
  return v1.dnorm2();
}


template <class T,int D>
inline
std::ostream& operator << ( std::ostream& out,const AimsVector<T,D>& thing)
{
  out << '(';
  for (int d = 0; d < D-1 ; ++d)
    out << thing.item(d) << " ,";
  out << thing.item(D-1) << ')';
  return(out);
}


template <int D>
inline
std::ostream& operator << ( std::ostream& out,const AimsVector<char,D>& thing)
{
  out << '(';
  for (int d = 0; d < D-1 ; ++d)
    out << (int)thing.item(d) << " ,";
  out << (int)thing.item(D-1) << ')';
  return(out);
}


template <int D>
inline
std::ostream& operator << ( std::ostream& out,const AimsVector<byte,D>& thing)
{
  out << '(';
  for (int d = 0; d < D-1 ; ++d)
    out << (int)thing.item(d) << " ,";
  out << (int)thing.item(D-1) << ')';
  return(out);
}


template <class T,int D>
inline
std::istream& operator >> (std::istream& is, AimsVector<T,D>& thing)
{
  char ch = 0;

  //std::istream::sentry s( is ); // to use in the future...
  carto::StreamUtil::skip( is );
  if( !is )
    return is;
  if (is.peek() == '(')
    {
      is >> ch;
      carto::StreamUtil::skip( is );
      is >> thing.item(0);
      for (int i = 1; i < D; ++i)
        {
          carto::StreamUtil::skip( is );
          is >> ch;
          if (ch != ',')
            break;
          carto::StreamUtil::skip( is );
          is >> thing.item(i);
        }
      if( is )
        {
          carto::StreamUtil::skip( is );
          is >> ch;
        }
    }

  if( ch != ')' )
    std::cout << "vector read ends not with )\n";
    //is.setstate (std::ios::failbit);

  return is;
}


template <int D>
inline
std::istream& operator >> (std::istream& is, AimsVector<char,D>& thing)
{
  char ch = 0;
  int tmp;

  //std::istream::sentry s( is ); // to use in the future...
  carto::StreamUtil::skip( is );
  if( !is )
    return is;
  if (is.peek() == '(')
    {
      is >> ch;
      carto::StreamUtil::skip( is );
      is >> tmp;
      thing.item(0) = (char)tmp;
      for (int i = 1; i < D; ++i)
        {
          carto::StreamUtil::skip( is );
          is >> ch;
          if (ch != ',')
            break;
          carto::StreamUtil::skip( is );
          is >> tmp;
          thing.item(i) = (char)tmp;
        }
      if( is )
        {
          carto::StreamUtil::skip( is );
          is >> ch;
        }
    }

  if( ch != ')' )
    is.setstate (std::ios::failbit);

  return is;
}


template <int D>
inline
std::istream& operator >> (std::istream& is, AimsVector<byte,D>& thing)
{
  char ch = 0;
  int tmp;

  //std::istream::sentry s( is ); // to use in the future...
  carto::StreamUtil::skip( is );
  if( !is )
    return is;
  if (is.peek() == '(')
    {
      is >> ch;
      carto::StreamUtil::skip( is );
      is >> tmp;
      thing.item(0) = (byte)tmp;
      for (int i = 1; i < D; ++i)
        {
          carto::StreamUtil::skip( is );
          is >> ch;
          if (ch != ',')
            break;
          carto::StreamUtil::skip( is );
          is >> tmp;
          thing.item(i) = (byte)tmp;
        }
      if( is )
        {
          carto::StreamUtil::skip( is );
          is >> ch;
        }
    }

  if( ch != ')' )
    is.setstate (std::ios::failbit);

  return is;
}



template <typename T>
inline
AimsVector<T,3> vectProduct( const AimsVector<T,3> & v1,
                             const AimsVector<T,3> & v2 )
{
  return AimsVector<T,3>( v1[1] * v2[2] - v1[2] * v2[1],
        	   v1[2] * v2[0] - v1[0] * v2[2],
        	   v1[0] * v2[1] - v1[1] * v2[0] );
}


#endif
