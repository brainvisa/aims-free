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

#include <pyaims/object/object.h>
#include <cartobase/object/object_d.h>
#include <cartobase/io/asciidatasourcetraits.h>
#include <cartobase/object/pythonwriter.h>

using namespace std;
using namespace carto;
using namespace carto::interface_internal;

namespace carto
{

  void _objectStrCatchFunction( carto::PythonWriter & pw, std::exception & e,
                                const carto::Object & )
  {
    AsciiDataSourceTraits<std::string>::write( *pw.dataSource(),
      string( "<" ) + e.what() + ">" );
  }

  template <>
  //inline
  bool TypedObject<PyObject *>::isDictionary() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    bool x = PyMapping_Check( _value );
    if( x ) // it seems PyMapping_Check is not sufficient...
    {
      if( PyObject_HasAttrString( _value, "isDictionary" ) )
      {
        // the PyObject contains an Object: delegate to it.
        PyObject *res = PyObject_CallMethod(
          _value, const_cast<char *>( "isDictionary" ), 0 );
        if( res )
        {
          x = ( res == Py_True );
          Py_DECREF( res );
        }
        else
          x = false;
      }
      // this at least avoids problems for numpy.float32 objects...
      else if( !PyObject_HasAttrString( _value, "keys" ) )
        x = false;
    }
    PyGILState_Release(gstate);
    return x;
  }


  template <>
  //inline
  bool TypedObject<PyObject *>::isIterable() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    bool x = PySequence_Check( _value );
    if( x ) // it seems PySequence_Check is not sufficient...
    {
      if( PyObject_HasAttrString( _value, "isIterable" ) )
      {
        // the PyObject contains an Object: delegate to it.
        PyObject *res = PyObject_CallMethod(
          _value, const_cast<char *>( "isIterable" ), 0 );
        if( res )
        {
          x = ( res == Py_True );
          Py_DECREF( res );
        }
        else
          x = false;
      }
      // this at least avoids problems for numpy.float32 objects...
      else if( !PyObject_HasAttrString( _value, "__contains__" ) )
        x = false;
    }
    if( !x )
      x = isDictionary();
    PyGILState_Release(gstate);
    return x;
  }


  template <>
  //inline
  bool TypedObject<PyObject *>::isScalar() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    bool x = PyNumber_Check( _value );
    if( !x && PyObject_HasAttrString( _value, "isScalar" ) )
    {
      PyObject *res = PyObject_CallMethod(
        _value, const_cast<char *>( "isScalar" ), 0 );
      if( res )
      {
        x = ( res == Py_True );
        Py_DECREF( res );
      }
    }
    PyGILState_Release(gstate);
    return x;
  }


  template <>
  //inline
  bool TypedObject<PyObject *>::isString() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    bool x;
#if PY_VERSION_HEX < 0x03000000
    x = PyString_Check( _value );
    if( !x )
#endif
      x = PyUnicode_Check( _value );
    if( !x && PyObject_HasAttrString( _value, "isString" ) )
    {
      PyObject *res = PyObject_CallMethod(
        _value, const_cast<char *>( "isString" ), 0 );
      if( res )
      {
        x = ( res == Py_True );
        Py_DECREF( res );
      }
    }
    PyGILState_Release(gstate);
    return x;
  }


  template <>
  //inline
  bool TypedObject<PyObject *>::isIterator() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    bool x = PyIter_Check( _value );
    if( !x && PyObject_HasAttrString( _value, "isIterator" ) )
    {
      PyObject *res = PyObject_CallMethod(
        _value, const_cast<char *>( "isIteraror" ), 0 );
      if( res )
      {
        x = ( res == Py_True );
        Py_DECREF( res );
      }
    }
    PyGILState_Release(gstate);
    return x;
  }


  template <>
  //inline
  bool TypedObject<PyObject *>::isDictionaryIterator() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    bool x = PyIter_Check( _value ); // && ?;
    if( !x && PyObject_HasAttrString( _value, "isDictionaryIterator" ) )
    {
      PyObject *res = PyObject_CallMethod(
        _value, const_cast<char *>( "isDictionaryIterator" ), 0 );
      if( res )
      {
        x = ( res == Py_True );
        Py_DECREF( res );
      }
    }
    PyGILState_Release(gstate);
    return x;
  }


  template <>
  bool TypedObject<PyObject *>::isIntKeyIterator() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    bool x = PyIter_Check( _value ); // && ?;
    if( !x && PyObject_HasAttrString( _value, "isIntKeyIterator" ) )
    {
      PyObject *res = PyObject_CallMethod(
        _value, const_cast<char *>( "isIntKeyIterator" ), 0 );
      if( res )
      {
        x = ( res == Py_True );
        Py_DECREF( res );
      }
    }
    PyGILState_Release(gstate);
    return x;
  }


  template <>
  //inline
  bool TypedObject<PyObject *>::isArray() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    bool x = PySequence_Check( _value );
    if( !x && PyObject_HasAttrString( _value, "isArray" ) )
    {
      PyObject *res = PyObject_CallMethod(
        _value, const_cast<char *>( "isArray" ), 0 );
      if( res )
      {
        x = ( res == Py_True );
        Py_DECREF( res );
      }
    }
    PyGILState_Release(gstate);
    return x;
  }


  template <>
  //inline
  bool TypedObject<PyObject *>::isDynArray() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    bool x = PySequence_Check( _value );
    if( !x && PyObject_HasAttrString( _value, "isDynArray" ) )
    {
      PyObject *res = PyObject_CallMethod(
        _value, const_cast<char *>( "isDynArray" ), 0 );
      if( res )
      {
        x = ( res == Py_True );
        Py_DECREF( res );
      }
    }
    PyGILState_Release(gstate);
    return x;
  }


   template <>
  //inline
  bool TypedObject<PyObject *>::isNone() const
  {
    PyObject    *_value = getValue();
    if( !_value )
      return true;
    if( _value == Py_None )
      return true;
    bool x = false;
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    if( PyObject_HasAttrString( _value, "isNone" ) )
    {
      PyObject *res = PyObject_CallMethod(
        _value, const_cast<char *>( "isNone" ), 0 );
      if( res )
      {
        x = ( res == Py_True );
        Py_DECREF( res );
      }
    }
    PyGILState_Release(gstate);
    return x;
  }


 namespace interface_internal
  {

    // ScalarInterface
    //-------------------------------------------------------------------------
    template <>
    class ScalarImpl< PyObject *, false >
    {
    public:
      static inline bool isScalar( const TypedObject<PyObject *> & to )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        bool x = (bool) PyNumber_Check( to.getValue() );
        if( !x && PyObject_HasAttrString( to.getValue(), "isScalar" ) )
        {
          PyObject *res = PyObject_CallMethod(
            to.getValue(), const_cast<char *>( "isScalar" ), 0 );
          if( res )
          {
            x = ( res == Py_True );
            Py_DECREF( res );
          }
        }
        PyGILState_Release(gstate);
        return x;
      }

      static inline double getScalar( const TypedObject<PyObject *> &to )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        if( PyFloat_CheckExact( to.getValue() ) )
        {
          double x = PyFloat_AsDouble( to.getValue() );
          PyGILState_Release(gstate);
          return x;
        }
        PyObject	*po = PyNumber_Float( to.getValue() );
        if( po )
        {
          double x = PyFloat_AsDouble( po );
          Py_DECREF( po );
          PyGILState_Release(gstate);
          return x;
        }
        if( PyObject_HasAttrString( to.getValue(), "getScalar" ) )
        {
          PyObject *res = PyObject_CallMethod(
            to.getValue(), const_cast<char *>( "getScalar" ), 0 );
          if( res )
          {
            double x = PyFloat_AsDouble( res );
            Py_DECREF( res );
            PyGILState_Release(gstate);
            return x;
          }
        }
        PyErr_Clear();
        PyGILState_Release(gstate);
        throw std::runtime_error( "Cannot convert python object  to scalar" );
      }

      static inline void setScalar( TypedObject<PyObject *> &to, double value )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        PyObject	*po = PyFloat_FromDouble( value );
        if( !po )
        {
          PyErr_Clear();
          PyGILState_Release(gstate);
          throw std::runtime_error( "" );
        }
        if( to.getValue() )
        {
          Py_DECREF( to.getValue() );
        }
        PyGILState_Release(gstate);
        to.getValue() = po;
      }

      static bool equals( const TypedObject<PyObject *> & o1,
                          const GenericObject & o2 )
      {
        return o1.isScalar() && o2.isScalar()
          && o1.getScalar() == o2.getScalar();
      }

    };


    // StringInterface
    //-------------------------------------------------------------------------
    template <>
    class StringImpl< PyObject *, false >
    {
    public:
      static inline bool isString( const TypedObject<PyObject *> & po )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        bool x;
#if PY_VERSION_HEX < 0x03000000
        x = PyString_Check( po.getValue() );
        if( !x )
#endif
          x = PyUnicode_Check( po.getValue() );
        if( !x && PyObject_HasAttrString( po.getValue(), "isString" ) )
        {
          PyObject *res = PyObject_CallMethod(
            po.getValue(), const_cast<char *>( "isString" ), 0 );
          if( res )
          {
            x = ( res == Py_True );
            Py_DECREF( res );
          }
        }
        PyGILState_Release(gstate);
        return x;
      }

      static inline std::string getString( const TypedObject<PyObject *> &to )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        if( PyUnicode_Check( to.getValue() ) )
        {
#if PY_VERSION_HEX >= 0x03000000
          PyObject *enc = PyUnicode_EncodeLocale( to.getValue(), 0 );
          if( enc )
          {
              char *s = PyBytes_AsString( enc );
              Py_DECREF( enc );
              if( s )
              {
                PyGILState_Release( gstate );
                return s;
              }
          }
#else
          PyObject *enc = PyString_AsEncodedObject( to.getValue(), "utf-8",
                                                    0 );
          if( enc )
          {
              char *s = PyString_AsString( enc );
              Py_DECREF( enc );
              if( s )
              {
                PyGILState_Release( gstate );
                return s;
              }
          }
#endif
        }

#if PY_VERSION_HEX < 0x03000000
        char *s = PyString_AsString( to.getValue() );
        if( s )
        {
          PyGILState_Release(gstate);
          return s;
        }
#endif
        PyErr_Clear();
        PyObject	*po = PyNumber_Float( to.getValue() );
        if( po )
        {
          double	x = PyFloat_AsDouble( po );
          Py_DECREF( po );
          PyGILState_Release(gstate);
          return toString( x );
        }
        if( PyObject_HasAttrString( to.getValue(), "getString" ) )
        {
          PyObject *res = PyObject_CallMethod(
            to.getValue(), const_cast<char *>( "getString" ), 0 );
          if( res )
          {
#if PY_VERSION_HEX >= 0x03000000
            PyObject *enc = PyUnicode_EncodeLocale( res, 0 );
            const char *x = PyBytes_AsString( enc );
            Py_DECREF( enc );
#else
            const char *x = PyString_AsString( res );
#endif
            Py_DECREF( res );
            PyGILState_Release(gstate);
            return x;
          }
        }
        PyErr_Clear();
        PyGILState_Release(gstate);
        throw std::runtime_error( "Cannot convert python object to string" );
      }

      static inline void setString( TypedObject<PyObject *> &to,
                                    const std::string &value )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
#if PY_VERSION_HEX >= 0x03000000
        PyObject *po = PyUnicode_FromString( value.c_str() );
#else
        PyObject *po = PyString_FromString( value.c_str() );
#endif
        if( to.getValue() )
        {
          Py_DECREF( to.getValue() );
        }
        PyGILState_Release(gstate);
        to.getValue() = po;
      }

      static bool equals( const TypedObject<PyObject *> & o1,
                          const GenericObject & o2 )
      {
        return o1.isString() && o2.isString()
          && o1.getString() == o2.getString();
      }

    };


    // SizeInterface
    //-------------------------------------------------------------------------
    template <>
    inline 
    size_t 
    SizeImpl<PyObject *,false>::size( const TypedObject<PyObject *> &object )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      int s = PySequence_Size( object.getValue() );
      if( s < 0 )
        s = PyMapping_Size( object.getValue() );
      if( s < 0 )
      {
        if( PyObject_HasAttrString( object.getValue(), "size" ) )
        {
          PyObject *res = PyObject_CallMethod(
            object.getValue(), const_cast<char *>( "size" ), 0 );
          if( res )
          {
#if PY_VERSION_HEX >= 0x03000000
            long x = PyLong_AsLong( res );
#else
            long x = PyInt_AsLong( res );
#endif
            Py_DECREF( res );
            PyGILState_Release(gstate);
            return size_t(x);
          }
        }
        PyErr_Clear();
        PyGILState_Release(gstate);
        throw std::runtime_error( std::string( "Cannot convert object of "
                                               "type " ) 
                                  + DataTypeCode<PyObject *>::name() 
                                  + " to dictionary" );
      }
      PyGILState_Release(gstate);
      return s;
    }


    // ArrayInterface
    //-------------------------------------------------------------------------
    template <>
    class ArrayImpl< PyObject *, false >
    {
    public:

      static inline bool isArray( const TypedObject< PyObject * > & to )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        bool x = PySequence_Check( to.getValue() );
        if( !x && PyObject_HasAttrString( to.getValue(), "isArray" ) )
        {
          PyObject *res = PyObject_CallMethod(
            to.getValue(), const_cast<char *>( "isArray" ), 0 );
          if( res )
          {
            x = ( res == Py_True );
            Py_DECREF( res );
          }
        }
        PyGILState_Release(gstate);
        return x;
      }

      static inline Object 
      getArrayItem( const TypedObject<PyObject *> & to, int index )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        PyObject	*o = PySequence_GetItem( to.getValue(), index );
        if( !o )
        {
          PyErr_Clear();
          PyGILState_Release(gstate);
          throw std::runtime_error( std::string( "Array item not found in " )
                                    + DataTypeCode<PyObject *>::name()
                                    + " at index " + toString( index ) );
        }
        Object	obj = Object::value( o );
        Py_DECREF( o );
        PyGILState_Release(gstate);
        return obj;
      }

      static inline void setArrayItem( TypedObject<PyObject *> & to, 
                                       int index, Object value )
      {
        PyObject	*po = value->value<PyObject *>();
        if( !po )
          throw std::runtime_error
            ( std::string( "Cannot (²?) set non-python element " )
              + value->type() + " in python sequence" );
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        if( PySequence_SetItem( to.getValue(), index, po ) < 0 )
        {
          PyErr_Clear();
          PyGILState_Release(gstate);
          throw std::runtime_error( std::string( "Array item not found in " )
                                    + DataTypeCode<PyObject *>::name() 
                                    + " at index " + toString( index ) );
        }
        PyGILState_Release(gstate);
      }

    };


    // DynArrayInterface
    //-------------------------------------------------------------------------
    template <>
    class DynArrayImpl< PyObject *, false >
    {
    public:
      static inline bool isDynArray( const TypedObject< PyObject * > & to )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        bool x = PyList_Check( to.getValue() );
        if( !x && PyObject_HasAttrString( to.getValue(), "isDynArray" ) )
        {
          PyObject *res = PyObject_CallMethod(
            to.getValue(), const_cast<char *>( "isDynArray" ), 0 );
          if( res )
          {
            x = ( res == Py_True );
            Py_DECREF( res );
          }
        }
        PyGILState_Release(gstate);
        return x;
      }

      static inline void 
      reserveArray( TypedObject< PyObject * > &,
                    size_t )
      {
      }

      static inline void
      resizeArray( TypedObject< PyObject * > &,
                   size_t )
      {
        // TODO
      }

      static inline void 
      removeArrayItem( TypedObject< PyObject * > & to,
                       int index )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        if( !PySequence_Check( to.getValue() ) )
        {
          PyGILState_Release(gstate);
          throw std::runtime_error( "Cannot delete array item on non-array "
                                    "object" );
        }
        if( PySequence_DelItem( to.getValue(), index ) < 0 )
        {
          PyErr_Clear();
          PyGILState_Release(gstate);
          throw std::runtime_error
            ( std::string( "Cannot del item at position" ) 
              + toString( index ) );
        }
        PyGILState_Release(gstate);
      }


      static inline void 
      insertArrayItem( TypedObject< PyObject * > & to,
                       int index, Object value )
      {
        PyObject	*po = value->value<PyObject *>();
        if( !po )
          throw std::runtime_error
            ( std::string( "Cannot (yet?) insert non-python element " )
              + value->type() + " in python sequence" );
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        if( PyList_Insert( to.getValue(), index, po ) < 0 )
        {
          PyErr_Clear();
          PyGILState_Release(gstate);
          throw std::runtime_error( std::string( "Cannot insert element at "
                                                 "position " )
                                    + toString( index ) );
        }
        PyGILState_Release(gstate);
      }
    };


    // DictionaryInterface
    //-------------------------------------------------------------------------
    template <>
    class DictionaryImpl<PyObject *, false>
    {
    public:

      static inline bool isDictionary( const TypedObject< PyObject * > & to )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        bool x = PyMapping_Check( to.getValue() );
        if( x ) // it seems PyMapping_Check is not sufficient...
        {
          if( PyObject_HasAttrString( to.getValue(), "isDictionary" ) )
          {
            // the PyObject contains an Object: delegate to it.
            PyObject *res = PyObject_CallMethod(
              to.getValue(), const_cast<char *>( "isDictionary" ), 0 );
            if( res )
            {
              x = ( res == Py_True );
              Py_DECREF( res );
            }
            else
              x = false;
          }
          // this at least avoids problems for numpy.float32 objects...
          else if( !PyObject_HasAttrString( to.getValue(), "keys" ) )
            x = false;
        }
        PyGILState_Release(gstate);
        return x;
      }

      static inline bool 
      getProperty( const TypedObject<PyObject * > & to, 
                   const std::string & key, Object & result )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        PyErr_Clear();
        PyObject	*po =
            PyMapping_GetItemString( to.getValue(),
                                     const_cast<char *>( key.c_str() ) );
        if( !po )
        {
          PyErr_Clear();
          PyGILState_Release(gstate);
          return false;
        }
        // TODO: convert to standard types
        result.reset( new ValueObject<PyObject *>( po ) );
        Py_DECREF( po );
        PyGILState_Release(gstate);
        return true;
      }

      static inline void 
      setProperty( TypedObject<PyObject *> & to,
                   const std::string & key, Object value )
      {
        PyObject	*po = 0;
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        try
        {
          po = value->value<PyObject *>();
        }
        catch( ... )
        {
          try
          {
            int x = value->value<int>();
#if PY_VERSION_HEX >= 0x03000000
            po = PyLong_FromLong( x );
#else
            po = PyInt_FromLong( x );
#endif
          }
          catch( ... )
          {
            try
            {
              long x = value->value<long>();
              po = PyLong_FromLong( x );
            }
            catch( ... )
            {
              try
              {
                double x = value->getScalar();
                po = PyFloat_FromDouble( x );
              }
              catch( ... )
              {
                try
                {
                  string x = value->getString();
#if PY_VERSION_HEX >= 0x03000000
                  po = PyUnicode_FromString( x.c_str() );
#else
                  po = PyString_FromString( x.c_str() );
#endif
                }
                catch( ... )
                {
                }
              }
            }
          }
        }
        if( !po )
        {
          //sipConvertFromInstance( &value, sipClass_carto_Object, 0 );
          /* The following does not work because sipFindType() is only defined
           * in sip-generated modules (sipAPIaimssip.h) which are not built
           * yet and need this library. It could be done via a small
           * registration mechanism which would be completed by the sipaims
           * part.
          Object *obj2 = new Object( value );
          cout << "type: " << sipFindType( "carto_Object" ) << endl;
          po = sipConvertFromType( obj2, sipFindType( "carto_Object" ),
                                   Py_None );
          */

          PyErr_Clear();
          PyGILState_Release(gstate);
          throw std::runtime_error
            ( std::string( "Cannot (yet?) insert non-python element " )
              + value->type() + " in python dictionary" );
        }
        if( PyMapping_SetItemString( to.getValue(), 
                                     const_cast<char *>( key.c_str() ), po ) 
            < 0 )
        {
          PyErr_Clear();
          PyGILState_Release(gstate);
          throw std::runtime_error( std::string( "Cannot insert element with "
                                                 "key " )
                                    + key );
        }
        PyGILState_Release(gstate);
      }

      static inline bool 
      removeProperty( TypedObject<PyObject *> & to, 
                      const std::string & key )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        if( PyMapping_DelItemString( to.getValue(),
                                     const_cast<char *>( key.c_str() ) ) < 0 )
        {
          PyErr_Clear();
          PyGILState_Release(gstate);
          return false;
        }
        PyGILState_Release(gstate);
        return true;
      }

      static inline void 
      clearProperties( TypedObject<PyObject *> & to )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        if( !PyMapping_Check( to.getValue() ) )
        {
          PyGILState_Release(gstate);
          return;
        }
        // WARNING should not replace the object
        if( to.getValue() )
        {
          Py_DECREF( to.getValue() );
        }
        to.getValue() = PyDict_New();
        PyGILState_Release(gstate);
      }

      static inline 
      bool hasProperty( const TypedObject<PyObject *> & to, 
                        const std::string & key )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        bool x = PyMapping_HasKeyString( to.getValue(),
                                       const_cast<char *>( key.c_str() ) );
        PyGILState_Release(gstate);
        return x;
      }

      static bool equals( const TypedObject<PyObject *> & o1,
                          const GenericObject & o2 )
      {
        if( !o1.isDictionary() || !o2.isDictionary() )
          return false;
        if( o1.size() != o2.size() )
          return false;
        Object it, other_value;
        for( it=o1.objectIterator(); it->isValid(); it->next() )
        {
          try
          {
            other_value = o2.getProperty( it->key() );
            if( it->currentValue() != other_value )
            {
              return false;
            }
          }
          catch( ... )
          {
            return false;
          }
        }
        return true;
      }

    };


    // IterableInterface
    //-------------------------------------------------------------------------
    template <>
    class IterableImpl<PyObject *, false>
    {
    public:

      static Object 
      objectIterator( const TypedObject<PyObject *> & object );

      class PyObjectIterator : public virtual IteratorInterface
      {
      public:
        PyObjectIterator();
        PyObjectIterator( const PyObjectIterator & );
        virtual ~PyObjectIterator();
        virtual bool isValid() const;
        Object currentValue() const;
        void next();
        PyObjectIterator & operator = ( const PyObjectIterator & );

      protected:
        friend Object 
        IterableImpl<PyObject *, false>::objectIterator
        ( const TypedObject<PyObject *> & );
        PyObjectIterator( PyObject * pyobj );
        PyObject *_pyiter;
        PyObject *_pycur;
      };

      class PyMappingIterator :  public virtual DictionaryIteratorInterface, 
                                 public virtual IntKeyIteratorInterface,
                                 public virtual PyObjectIterator
      {
      public:
        PyMappingIterator();
        PyMappingIterator( const PyMappingIterator & );
        virtual ~PyMappingIterator();
        Object currentValue() const;
        void next() { PyObjectIterator::next(); }
        std::string key() const;
        long intKey() const;
        PyMappingIterator & operator = ( const PyMappingIterator & );

      private:
        friend Object 
        IterableImpl<PyObject *, false>::objectIterator
        ( const TypedObject<PyObject *> & );
        PyMappingIterator( PyObject * pyobj );
        PyObject	*_dict;
      };

      static inline bool isIterable( const TypedObject< PyObject * > & to )
      {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        bool x = PyIter_Check( to.getValue() );
        PyGILState_Release(gstate);
        return x;
      }

      static inline bool equals( const TypedObject<PyObject *> & o1,
                                 const GenericObject & o2 )
      {
        if( !o1.isIterable() || !o2.isIterable() )
          return false;
        if( o1.size() != o2.size() )
          return false;
        Object it, it2;
        for( it=o1.objectIterator(), it2=o2.objectIterator();
            it->isValid(); it->next(), it2->next() )
        {
          if( !it2->isValid() || it->currentValue() != it2->currentValue() )
            return false;
        }
        return true;
      }

    };


    // NoneInterface
    //-------------------------------------------------------------------------
    template <>
    class NoneImpl< PyObject *, false >
    {
    public:
      static inline bool isNone( const TypedObject<PyObject *> & to )
      {
        PyObject * o = to.getValue();
        if( !o || o == Py_None )
          return true;

        bool x = false;
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        if( PyObject_HasAttrString( to.getValue(), "isNone" ) )
        {
          PyObject *res = PyObject_CallMethod(
            to.getValue(), const_cast<char *>( "isNone" ), 0 );
          if( res )
          {
            x = ( res == Py_True );
            Py_DECREF( res );
          }
        }
        PyGILState_Release(gstate);
        return x;
      }
    };

  }

#define PyMapIter interface_internal::IterableImpl<PyObject *, false>::PyMappingIterator
#define PyObjIter interface_internal::IterableImpl<PyObject *, false>::PyObjectIterator
  DECLARE_GENERIC_OBJECT_TYPE( PyMapIter )
  DECLARE_GENERIC_OBJECT_TYPE( PyObjIter )

}

IterableImpl<PyObject *, false>::PyObjectIterator::PyObjectIterator()
  : IteratorInterface(), _pyiter( 0 ), _pycur( 0 )
{
}


IterableImpl<PyObject *, false>::PyObjectIterator::PyObjectIterator
( PyObject* pyobj )
  : IteratorInterface(), 
    _pyiter( PyObject_GetIter( pyobj ) ), 
    _pycur( _pyiter ? PyIter_Next( _pyiter ) : 0 )
{
}


IterableImpl<PyObject *, false>::PyObjectIterator::PyObjectIterator
( const PyObjectIterator & x )
  : Interface(), IteratorInterface(), 
    _pyiter( x._pyiter ), 
    _pycur( x._pycur )
{
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  if( _pyiter )
    Py_INCREF( _pyiter );
  if( _pycur )
    Py_INCREF( _pycur );
  PyGILState_Release(gstate);
}


IterableImpl<PyObject *, false>::PyObjectIterator::~PyObjectIterator()
{
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  if( _pycur )
  {
    Py_DECREF( _pycur );
  }
  if( _pyiter )
  {
    Py_DECREF( _pyiter );
  }
  PyGILState_Release(gstate);
}


IterableImpl<PyObject *, false>::PyObjectIterator & 
IterableImpl<PyObject *, false>::PyObjectIterator::operator = 
          ( const PyObjectIterator & x )
{
  if( &x == this )
    return *this;
  _pyiter = x._pyiter;
  _pycur = x._pycur;
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  if( _pyiter )
    Py_INCREF( _pyiter );
  if( _pycur )
    Py_INCREF( _pycur );
  PyGILState_Release(gstate);
  return *this;
}


bool 
IterableImpl<PyObject *, false>::PyObjectIterator::isValid() const
{
  return _pycur != 0;
}


Object 
IterableImpl<PyObject *, false>::PyObjectIterator::currentValue() const
{
  if( !_pycur )
    throw std::runtime_error( "no current iterator value" );
  return Object::value( _pycur );
}


void IterableImpl<PyObject *, false>::PyObjectIterator::next()
{
  if( _pycur )
    {
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      Py_DECREF( _pycur );
      _pycur = PyIter_Next( _pyiter );
      PyGILState_Release(gstate);
    }
}


IterableImpl<PyObject *, false>::PyMappingIterator::PyMappingIterator()
  :  DictionaryIteratorInterface(), IntKeyIteratorInterface(),
     PyObjectIterator(), _dict( 0 )
{
}


IterableImpl<PyObject *, false>::PyMappingIterator::PyMappingIterator
( PyObject* pyobj )
  :  DictionaryIteratorInterface(), IntKeyIteratorInterface(),
     PyObjectIterator( pyobj ), _dict( pyobj )
{
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  if( pyobj )
    Py_INCREF( pyobj );
  PyGILState_Release(gstate);
}


IterableImpl<PyObject *, false>::PyMappingIterator::PyMappingIterator
( const PyMappingIterator & x )
  : Interface(), IteratorInterface(), IntKeyIteratorInterface(),
    DictionaryIteratorInterface(), PyObjectIterator( x ),
    _dict( x._dict )
{
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  if( _dict )
    Py_INCREF( _dict );
  PyGILState_Release(gstate);
}


IterableImpl<PyObject *, false>::PyMappingIterator::~PyMappingIterator()
{
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  if( _dict )
  {
    Py_DECREF( _dict );
  }
  PyGILState_Release(gstate);
}


IterableImpl<PyObject *, false>::PyMappingIterator & 
IterableImpl<PyObject *, false>::PyMappingIterator::operator = 
          ( const PyMappingIterator & x )
{
  if( &x == this )
    return *this;
  PyObjectIterator::operator = ( x );
  _dict = x._dict;
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  if( _dict )
    Py_INCREF( _dict );
  PyGILState_Release(gstate);
  return *this;
}


std::string IterableImpl<PyObject *, false>::PyMappingIterator::key() const
{
  if( !_pycur )
    throw std::runtime_error( "no current iterator key" );
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  char *c = 0;
  if( PyUnicode_Check( _pycur ) )
  {
#if PY_VERSION_HEX >= 0x03000000
    PyObject *enc = PyUnicode_EncodeLocale( _pycur, 0 );
    c = PyBytes_AsString( enc );
#else
    PyObject *enc = PyString_AsEncodedObject( _pycur, "utf-8", 0 );
    if( enc )
      c = PyString_AsString( enc );
#endif
    Py_DECREF( enc );
  }
#if PY_VERSION_HEX < 0x03000000
  else if( PyString_Check( _pycur ) )
    c = PyString_AsString( _pycur );
#endif

  PyGILState_Release(gstate);
  if( !c )
    throw std::runtime_error( "current iterator key is not a string" );

  std::string	key = c;
  return key;
}


long IterableImpl<PyObject *, false>::PyMappingIterator::intKey() const
{
  if( !_pycur )
    throw std::runtime_error( "no current iterator key" );
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  long key = 0;
  bool fail = false;
  if( PyLong_Check( _pycur ) )
    key = PyLong_AsLong( _pycur );
#if PY_VERSION_HEX < 0x03000000
  else if( PyInt_Check( _pycur ) )
    key = PyInt_AsLong( _pycur );
#endif
  else
    fail = true;

  PyGILState_Release(gstate);
  if( fail)
    throw std::runtime_error( "current iterator key is not a number" );

  return key;
}


Object 
IterableImpl<PyObject *, false>::PyMappingIterator::currentValue() const
{
  try
  {
    std::string k = key();
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    PyObject		*pyo
      = PyMapping_GetItemString( _dict, const_cast<char *>( k.c_str() ) );
    if( pyo )
    {
      Object	o = Object::value( pyo );
      Py_DECREF( pyo );
      PyGILState_Release(gstate);
      return o;
    }
    PyErr_Clear();
    PyGILState_Release(gstate);
  }
  catch( std::runtime_error & )
  {
  }

  // try int dict
  long k = intKey();
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  PyObject *key = PyLong_FromLong( k );
  PyObject *pyo = PyObject_GetItem( _dict, key );
  Py_DECREF( key );
  if( pyo )
  {
    Object    o = Object::value( pyo );
    Py_DECREF( pyo );
    PyGILState_Release(gstate);
    return o;
  }
  PyErr_Clear();
  PyGILState_Release(gstate);
  throw std::runtime_error(
    "no current iterator value (not a dictionary?)" );
}


Object 
IterableImpl<PyObject *, false>::objectIterator
( const TypedObject<PyObject *> & to )
{
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  if( PyDict_Check( to.getValue() ) )
  {
    Object x = Object::value( PyMappingIterator( to.getValue() ) );
    PyGILState_Release(gstate);
    return x;
  }
  Object x = Object::value( PyObjectIterator( to.getValue() ) );
  PyGILState_Release(gstate);
  return x;
}


carto::PySyntaxSet::~PySyntaxSet()
{
}


INSTANTIATE_GENERIC_OBJECT_TYPE( PyObject * )
INSTANTIATE_GENERIC_OBJECT_TYPE( PyMapIter )
INSTANTIATE_GENERIC_OBJECT_TYPE( PyObjIter )
#undef PyMapIter

