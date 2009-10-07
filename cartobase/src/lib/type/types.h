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

#ifndef CARTOBASE_TYPE_TYPES_H
#define CARTOBASE_TYPE_TYPES_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/smart/rcptr.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <complex>
#include <typeinfo>


/* these types are out of any namespace for 2 reasons:
   - compatibility: they were in Aims without a namespace
   - simplicity: they are types used in many places, they have to be short

   BUT: they may conflict with system types
*/

/** Void does not contain anything.
    This class is useful for creating buckets or surfaces without
    any value associated to a point or a vertex. However, the size
    of Void is 1 byte.
*/
class Void
{
public:
  Void()    { }
  template<typename T> explicit Void( const T & ) { }

  template<typename T> Void& operator = ( const T& ) { return *this; };
};


inline bool operator == ( const Void&, const Void& )
{
  return true;
}

inline std::ostream& operator << ( std::ostream& os, const Void& )
{
  return os;
}

inline std::istream& operator >> ( std::istream& is, Void& )
{
  return is;
}

/// ...
typedef uint8_t byte;
/// ...
typedef unsigned short ushort;
/// ...
typedef uint32_t word;
/// ...
typedef unsigned int uint;
/// ...
typedef unsigned long ulong;
/// Complex 32 bits float
typedef std::complex<float> cfloat;
/// Complex 64 bits float
typedef std::complex<double> cdouble;


namespace carto
{

    //-------------------//
   //  DataTypeCode<T>  //
  //-------------------//

  /**	This class is just a hint to convert an actual data type to an 
	identifier string used in input/output stuff */
  template<class T> class DataTypeCode
  {
    // protected: // we'll make it protected later
  public:
    DataTypeCode() {};

  public:
    static inline std::string objectType();
    static inline std::string dataType();
    static inline std::string name();
  };


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

    //---------------------------//
   //  DataTypeCode<vector<T>>  //
  //---------------------------//

  template<class T> class DataTypeCode< std::vector<T> >
  {
  public:
    static inline std::string objectType()
    { return( "vector" ); }
    static inline std::string dataType()
    { return( DataTypeCode<T>().dataType() ); }
    static inline std::string name() 
    { 
      return std::string("vector of ") + DataTypeCode< T >::name(); 
    }
  };

    //-------------------------//
   //  DataTypeCode<list<T>>  //
  //-------------------------//

  template<class T> class DataTypeCode< std::list<T> >
  {
  public:
    static inline std::string objectType()
    { return( "list" ); }
    static inline std::string dataType()
    { return( DataTypeCode<T>().dataType() ); }
    static inline std::string name() 
    { 
      return std::string("list of ") + DataTypeCode< T >::name(); 
    }
  };


    //-------------------------//
   //  DataTypeCode<set<T>>  //
  //-------------------------//

  template<class T> class DataTypeCode< std::set<T> >
  {
  public:
    static inline std::string objectType()
    { return( "set" ); }
    static inline std::string dataType()
    { return( DataTypeCode<T>().dataType() ); }
    static inline std::string name() 
    { 
      return std::string("set of ") + DataTypeCode< T >::name(); 
    }
  };


    //-------------------------//
   //  DataTypeCode<map<T,U>>  //
  //-------------------------//

  template<typename T, typename U> class DataTypeCode< std::map<T,U> >
  {
  public:
    static inline std::string objectType()
    { return "map"; }
    static inline std::string dataType()
    { return DataTypeCode<U>::dataType(); }
    static inline std::string name() 
    { 
      return std::string("map of ") + DataTypeCode< T >::name() + "_" 
        + DataTypeCode<U>::name();
    }
  };


    //---------------------------//
   //  DataTypeCode<rc_ptr<T>>  //
  //---------------------------//

  template<typename T> class DataTypeCode< rc_ptr<T> >
  {
  public:
    static inline std::string objectType()
    { return "rc_ptr"; }
    static inline std::string dataType()
    { return DataTypeCode<T>::name(); }
    static inline std::string name() 
    { 
      return std::string("rc_ptr of ") + DataTypeCode< T >::name();
    }
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


    //-------------------//
   //  DataTypeCode<T>  //
  //-------------------//

  //---------------------------------------------------------------------------
  template<typename T>
  inline std::string  DataTypeCode<T>::objectType() 
  {
    return ""; 
  }

  //---------------------------------------------------------------------------
  template<typename T>
  inline std::string DataTypeCode<T>::dataType()
  {
    return typeid( T ).name();
    /*
    T::make_compilation_error( "this is only here to cause an error: "
                            "this code should NEVER be compiled. " 
                            "This function should always be specialized" );
    return "undefined type";
    */
  }

  //---------------------------------------------------------------------------
  template<typename T>
  inline std::string DataTypeCode<T>::name()
  { 
    return  DataTypeCode< T >::dataType();
  }


  //	specializations

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<int8_t>::dataType()
  {
    return "S8";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<uint8_t>::dataType()
  {
    return "U8";
  }

  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<char>::dataType()
  {
    return "S8";
  }
#endif

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<int16_t>::dataType()
  {
    return "S16";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<uint16_t>::dataType()
  {
    return "U16";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<int32_t>::dataType()
  {
    return "S32";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<uint32_t>::dataType()
  {
    return "U32";
  }

  // ### remove after everything has been moved to intN_t/uintN_t
  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<long>::dataType()
  {
    return "LONG";
  }

  // ### remove after everything has been moved to intN_t/uintN_t
  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<unsigned long>::dataType()
  {
    return "ULONG";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<float>::dataType()
  {
    return "FLOAT";
    // return "float";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<double>::dataType()
  {
    return "DOUBLE";
    // return "double";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<cfloat>::dataType()
  {
    return "CFLOAT";
    // return "cfloat";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<cdouble>::dataType()
  {
    return "CDOUBLE";
    // return "cdouble";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<Void>::dataType()
  {
    return "VOID";
    //return "void";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<bool>::dataType()
  {
    return "boolean";
  }

  //---------------------------------------------------------------------------
  template<> inline std::string DataTypeCode<std::string>::dataType()
  {
    return "string";
  }

} // namespace carto

#endif
