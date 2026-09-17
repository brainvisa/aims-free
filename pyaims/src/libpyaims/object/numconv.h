#ifndef PYAIMS_VECTOR_NUMCONV_H
#define PYAIMS_VECTOR_NUMCONV_H

#include <string>
#include <stdint.h>
#include <Python.h>
#include <pyaims/numpyarray.h>
#include <numpy/arrayscalars.h>

namespace carto
{

  uint32_t uint32_FromPy( PyObject *pyobj );


  inline PyObject* PyaimsInt_FromLong( long x )
  {
#if PY_VERSION_HEX >= 0x03000000
    return PyLong_FromLong( x );
#else
    return PyInt_FromLong( x );
#endif
  }


  inline long PyaimsLong_AsLong( PyObject *x )
  {
#if PY_VERSION_HEX >= 0x03000000
    return PyLong_AsLong( x );
#else
    if( PyInt_Check( x ) )
      return PyInt_AsLong( x );
    else
      return PyLong_AsLong( x );
#endif
  }


  inline int PyaimsInt_AsLong( PyObject *x )
  {
    return int( PyaimsLong_AsLong( x ) );
  }


  inline bool PyaimsInt_Check( PyObject *x )
  {
#if PY_VERSION_HEX >= 0x03000000
    if( PyLong_Check( x ) )
      return true;
//     long y = PyLong_AsLong( x );
#else
    if( PyLong_Check( x ) || PyInt_Check( x ) )
      return true;
#endif
    // check numpy types
    PyArray_Descr* dtype;
    if( !PyArray_DescrConverter( x, &dtype ) )
      return false;
    int typeNum = dtype->type_num;
    Py_DECREF( dtype );
    if( typeNum == NPY_BOOL || typeNum == NPY_INT8 || typeNum == NPY_UINT8
        || typeNum == NPY_INT16 || typeNum == NPY_UINT16
        || typeNum == NPY_INT32 || typeNum == NPY_UINT32
        || typeNum == NPY_INT64 || typeNum == NPY_UINT64 )
      return true;
    return false;
  }


  inline double PyaimsFloat_AsDouble( PyObject *x )
  {
    PyTypeObject* ptype = Py_TYPE( x ); //->ob_type;
    if( ptype == &PyFloat32ArrType_Type )
      return reinterpret_cast<PyFloatScalarObject *>( x )->obval;
    double z = PyFloat_AsDouble( x );
    if( z != -1. || !PyErr_Occurred() )
      return z;
//     if( PyFloat_Check( x ) )
//       return PyFloat_AsDouble( x );
#if PY_VERSION_HEX <= 0x03000000
    long y;
    y = PyInt_AsLong( x );
    if( y != -1 || !PyErr_Occurred() )
      return double( y );
    else
      PyErr_Clear();
#endif
    return double( PyLong_AsLong( x ) );
  }


  inline bool PyaimsNumber_Check( PyObject *x )
  {
#if PY_VERSION_HEX >= 0x03000000
    if( PyFloat_Check( x ) || PyLong_Check( x ) )
      return true;
#else
    if( PyFloat_Check( x ) || PyLong_Check( x ) || PyInt_Check( x ) )
      return true;
#endif
    double y = PyFloat_AsDouble( x );
    if( y == -1. && PyErr_Occurred() )
    {
      PyErr_Clear();
      return false;
    }
    return true;
  }


  inline PyObject*
  PyString_FromStdString( const std::string * x )
  {
#if PY_VERSION_HEX >= 0x03000000
    return PyUnicode_FromString( x->c_str() );
#else
    return PyString_FromString( x->c_str() );
#endif
  }


  inline PyObject*
  PyString_FromStdString( const std::string & x )
  {
#if PY_VERSION_HEX >= 0x03000000
    return PyUnicode_FromString( x.c_str() );
#else
    return PyString_FromString( x.c_str() );
#endif
  }


  inline std::string *
  PyString_ToStdString( PyObject * x )
  {
#if PY_VERSION_HEX >= 0x03000000
    PyObject *enc = PyUnicode_EncodeLocale( x, 0 );
    std::string *s = new std::string( PyBytes_AsString( enc ) );
    Py_DECREF( enc );
    return s;
#else
    if( PyString_Check( x ) )
      return new std::string( PyString_AsString(x) );
    else
    {
      PyObject *enc = PyUnicode_AsEncodedString( x, "utf-8", 0 );
      std::string *s = new std::string( PyString_AsString( enc ) );
      Py_DECREF( enc );
      return s;
    }
#endif
  }


  inline std::string
  PyString_AsStdString( PyObject * x )
  {
#if PY_VERSION_HEX >= 0x03000000
    PyObject *enc = PyUnicode_EncodeLocale( x, 0 );
    std::string s = std::string( PyBytes_AsString( enc ) );
    Py_DECREF( enc );
    return s;
#else
    if( PyString_Check( x ) )
      return std::string( PyString_AsString(x) );
    else
    {
      PyObject *enc = PyUnicode_AsEncodedString( x, "utf-8", 0 );
      std::string s = std::string( PyString_AsString( enc ) );
      Py_DECREF( enc );
      return s;
    }
#endif
  }


  inline bool PyStdString_Check( PyObject *x )
  {
  #if PY_VERSION_HEX >= 0x03000000
    return PyUnicode_Check( x );
  #else
    return PyString_Check( x ) || PyUnicode_Check( x );
  #endif
  }

}

#endif

