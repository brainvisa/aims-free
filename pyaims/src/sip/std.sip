
%MappedType std::string
{
%TypeHeaderCode
#include <pyaims/object/numconv.h>
%End

%ConvertToTypeCode
  // Allow a Python string whenever a std::string is expected.

  if (sipIsErr == NULL)
#if PY_VERSION_HEX >= 0x03000000
    return PyUnicode_Check( sipPy );
#else
    return( PyString_Check( sipPy ) || PyUnicode_Check( sipPy ) );
#endif

  if( sipPy == Py_None )
  {
    *sipCppPtr = new std::string;

    return 1;
  }

#if PY_VERSION_HEX < 0x03000000
  if( PyString_Check( sipPy ) )
  {
    *sipCppPtr = new std::string(PyString_AS_STRING(sipPy));

    return 1;
  }
#endif
  if( PyUnicode_Check( sipPy ) )
  {
    int res = 1;
    *sipCppPtr = carto::PyString_ToStdString( sipPy );

/*
    PyObject *sysmod = 0;
    PyObject *codmod = 0;
    PyObject *cod = 0;
    PyObject *defencmeth = 0;
    PyObject *encmeth = 0;
    PyObject *enc = 0;
    PyObject *strtuple = 0;
    PyObject *str = 0;
    int res = 1;
    try
    {
      sysmod = PyImport_AddModule( "sys" );
      codmod = PyImport_AddModule( "codecs" );
      defencmeth = PyObject_GetAttrString( sysmod, "getdefaultencoding" );
      if( !defencmeth )
      {
        std::cerr << "cannot retreive default encoding function\n";
        throw std::runtime_error( "cannot retreive default encoding" );
      }
      cod = PyObject_CallFunctionObjArgs( defencmeth, NULL );
      if( !cod )
      {
        std::cerr << "cannot retreive default encoding\n";
        throw std::runtime_error( "cannot retreive default encoding" );
      }
      encmeth = PyObject_GetAttrString( codmod, "getencoder" );
      if( !encmeth )
      {
        std::cerr << "cannot retreive codecs.getencoder\n";
        throw std::runtime_error( "cannot retreive codecs.getencoder" );
      }
      enc = PyObject_CallFunctionObjArgs( encmeth, cod, NULL );
      if( !enc )
      {
        std::cerr << "cannot retreive encoder\n";
        throw std::runtime_error( "cannot retreive encoder" );
      }
      strtuple = PyObject_CallFunctionObjArgs( enc, sipPy, NULL );
      if( !strtuple )
      {
        std::cerr << "cannot use encoder\n";
        throw std::runtime_error( "cannot use encoder" );
      }
      str = PyTuple_GetItem( strtuple, 0 );
      if( !str )
      {
        std::cerr << "cannot get encoded string\n";
        throw std::runtime_error( "cannot get encoded string" );
      }

      *sipCppPtr = new std::string( PyString_AS_STRING( str ) );
    }
    catch( std::exception & e )
    {
      res = 0;
      *sipIsErr = 1;
      // std::cerr << "exception in unicode->string conversion\n";
      PyErr_SetString( PyExc_RuntimeError, e.what() );
    }
    if( strtuple )
    {
      Py_DECREF( strtuple );
    }
    if( enc )
    {
      Py_DECREF( enc );
    }
    if( encmeth )
    {
      Py_DECREF( encmeth );
    }
    if( cod )
    {
      Py_DECREF( cod );
    }
    if( defencmeth )
    {
      Py_DECREF( defencmeth );
    }
*/

    return res;
  }

  return 0;
%End


%ConvertFromTypeCode
  // std::string -> PyObject
  if( !sipCpp )
  {
    std::string s;
    return carto::PyString_FromStdString( &s );
  }

  return carto::PyString_FromStdString( sipCpp );
%End
};

