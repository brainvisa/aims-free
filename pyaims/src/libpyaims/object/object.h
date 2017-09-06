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

#ifndef CARTO_PYAIMS_OBJECT_OBJECT_H
#define CARTO_PYAIMS_OBJECT_OBJECT_H

#include <Python.h>
#include <cartobase/object/object.h>
#include <cartobase/object/syntax.h>

// ValueObject implementations for PyObject

namespace carto
{

  class PythonWriter;

  /// only needed in bindings of GenericObject and subclasses
  void _objectStrCatchFunction( PythonWriter &, std::exception &,
                                const carto::Object & );

  template<> inline
  std::string DataTypeCode<PyObject *>::objectType()
  {
    return "PyObject";
  }

  template<> inline
  std::string DataTypeCode<PyObject *>::dataType()
  {
    return "PyObject";
  }

  template<> inline
  std::string DataTypeCode<PyObject *>::name()
  {
    return "PyObject";
  }

  template<> bool TypedObject<PyObject *>::isDictionary() const;
  template <> bool TypedObject<PyObject *>::isIterable() const;
  template <> bool TypedObject<PyObject *>::isScalar() const;
  template <> bool TypedObject<PyObject *>::isString() const;
  template <> bool TypedObject<PyObject *>::isIterator() const;
  template <> bool TypedObject<PyObject *>::isDictionaryIterator() const;
  template <> bool TypedObject<PyObject *>::isArray() const;
  template <> bool TypedObject<PyObject *>::isDynArray() const;
  template <> bool TypedObject<PyObject *>::isNone() const;
  template <> bool TypedObject<PyObject *>::isIntKeyIterator() const;

  extern template class TypedObject<PyObject *>;

  // ValueObject
  //---------------------------------------------------------------------------

  template<>
  inline
  ValueObject<PyObject *>::ValueObject()
  : _value( 0 )
  {
  }

  template<>
  inline
  ValueObject<PyObject *>::ValueObject( PyObject* const & o )
  : _value( o )
  {
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    Py_INCREF( o );
    PyGILState_Release(gstate);
  }

  template <>
  inline
  ValueObject<PyObject *>::~ValueObject()
  {
    if( _value )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      Py_DECREF( _value );
      PyGILState_Release(gstate);
    }
  }


  namespace interface_internal
  {

    template<typename T> inline 
    PyObject* PyObject_FromValue( const T & )
    {
      return 0;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const double & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      PyObject *y = PyFloat_FromDouble( x );
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const float & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      PyObject *y = PyFloat_FromDouble( x );
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const long & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      PyObject *y = PyLong_FromLong( x );
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const unsigned long & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      PyObject *y = PyLong_FromUnsignedLong( x );
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const int & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
#if PY_VERSION_HEX >= 0x03000000
      PyObject *y = PyLong_FromLong( x );
#else
      PyObject *y = PyInt_FromLong( x );
#endif
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const unsigned int & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      PyObject *y = PyLong_FromUnsignedLong( x );
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const short & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
#if PY_VERSION_HEX >= 0x03000000
      PyObject *y = PyLong_FromLong( x );
#else
      PyObject *y = PyInt_FromLong( x );
#endif
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const unsigned short & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
#if PY_VERSION_HEX >= 0x03000000
      PyObject *y = PyLong_FromLong( x );
#else
      PyObject *y = PyInt_FromLong( x );
#endif
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const char & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
#if PY_VERSION_HEX >= 0x03000000
      PyObject *y = PyLong_FromLong( x );
#else
      PyObject *y = PyInt_FromLong( x );
#endif
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( const unsigned char & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
#if PY_VERSION_HEX >= 0x03000000
      PyObject *y = PyLong_FromLong( x );
#else
      PyObject *y = PyInt_FromLong( x );
#endif
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline
    PyObject* PyObject_FromValue( const bool & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      PyObject *y = PyBool_FromLong( x );
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline
    PyObject* PyObject_FromValue( const std::string & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
#if PY_VERSION_HEX >= 0x03000000
      PyObject *y = PyUnicode_FromString( x.c_str() );
#else
      PyObject *y = PyString_FromString( x.c_str() );
#endif
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( char * const & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
#if PY_VERSION_HEX >= 0x03000000
      PyObject *y = PyUnicode_FromString( x );
#else
      PyObject *y = PyString_FromString( x );
#endif
      PyGILState_Release(gstate);
      return y;
    }

    template<> inline 
    PyObject* PyObject_FromValue( PyObject * const & x )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      Py_INCREF( x );
      PyGILState_Release(gstate);
      return x;
    }

  }	// namespace interface_interal


  class PySyntaxSet : public SyntaxSet
  {
  public:
    PySyntaxSet() : SyntaxSet() {}
    virtual ~PySyntaxSet();
  };


  /// a helper class converting to specific numeric types understood by python
  class NumericGenericObjectConverter
  {
  public:
    static bool asBool( const GenericObject & x ) { return x.value<bool>(); }

    static long int asInt( const GenericObject & x )
    {
      try
      {
        return x.value<int>();
      }
      catch( ... )
      {
        try
        {
          return x.value<unsigned>();
        }
        catch( ... )
        {
          try
          {
            return x.value<short>();
          }
          catch( ... )
          {
            try
            {
              return x.value<unsigned short>();
            }
            catch( ... )
            {
              try
              {
                return x.value<char>();
              }
              catch( ... )
              {
                return x.value<unsigned char>();
              }
            }
          }
        }
      }
    }

    static long int asLong( const GenericObject & x )
    {
      try
      {
        return x.value<long>();
      }
      catch( ... )
      {
        return x.value<unsigned long>();
      }
    }
  };


  DECLARE_GENERIC_OBJECT_TYPE( PyObject * )

}

#endif

