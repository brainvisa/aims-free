
class vector_%Template1typecode%
{
%TypeHeaderCode
#include <vector>
#include <cartobase/object/object.h>
#include <pyaims/object/numconv.h>
#include <cartobase/type/string_conversion.h>
%Template1typeinclude%
%Template1sipinclude%
#ifndef PYAIMSSIP_VECTOR_%Template1typecode%_DEFINED
#define PYAIMSSIP_VECTOR_%Template1typecode%_DEFINED
typedef std::vector<%Template1% > vector_%Template1typecode%;
#endif
%End

%TypeCode
#include <pyaims/vector/numpyarrayfunc.h>
%End

%ConvertToTypeCode
  if (sipIsErr == NULL)
  {
    if( sipCanConvertToType( sipPy,
        sipType_vector_%Template1typecode%, SIP_NOT_NONE | SIP_NO_CONVERTORS ) )
      return 1;
    if( !PySequence_Check( sipPy ) )
      return 0;
    // test compatibility of each element of the sequence
    unsigned    i, n = PySequence_Size( sipPy );
    PyObject    *pyitem;
    bool ok = true;
    for( i=0; ok && i<n; ++i )
    {
      pyitem = PySequence_GetItem( sipPy, i );
      if( !pyitem )
      {
        ok = false;
        break;
      }
      if( !%Template1testPyType%( pyitem ) )
      {
        const sipTypeDef* st = sipFindType( "%Template1sipClass%" );
        if( !st || !sipCanConvertToType( pyitem, st, SIP_NOT_NONE ) )
          ok = false;
      }
      if( pyitem )
        Py_DECREF( pyitem );
    }
    if( ok )
      return 1;
    else
      return 0;
  }

  if( sipCanConvertToType( sipPy, sipType_vector_%Template1typecode%,
                           SIP_NO_CONVERTORS ) )
  {
    int state = 0;

    vector_%Template1typecode% * dat
      = (vector_%Template1typecode% *)
        sipConvertToType( sipPy,
          sipType_vector_%Template1typecode%,
          sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
    if( *sipIsErr && dat )
    {
      sipReleaseType( dat, sipType_vector_%Template1typecode%, state );
      dat = 0;
    }
    else if( dat )
    {
      *sipCppPtr = dat;
      return 0;
    }
  }

  if( PySequence_Check( sipPy ) )
  {
    // --- numpy impl ---

%%Template1defScalar%%
%%Template1defNumpyBindings%%
%#if defined( PYAIMS_SCALAR ) || defined( PYAIMS_NUMPY_BINDINGS )%
%%Template1defNumpyIsSubArray%%

    *sipCppPtr = new std::vector<%Template1% >;
    unsigned	n = PySequence_Size( sipPy );
    (*sipCppPtr)->resize( n );

    if( n != 0 )
    {
      std::vector<int> dims( 1, n );
      std::vector<int> added_dims = %Template1NumDims%;
      dims.insert( dims.end(), added_dims.begin(), added_dims.end() );

      PyObject *pyobj = sipConvertFromType(
        *sipCppPtr, sipType_vector_%Template1typecode%,  0 );
      PyObject *pyarr = PyObject_GetAttrString( pyobj, "np" );
      int res = PySequence_SetSlice( pyarr, 0, n, sipPy );

      // delete internal ref to the array which nobody else owns
      // PyObject_DelAttrString( (PyObject *) sipSelf, "_arrayref" );
      // dec reference to self that was manually incremented in arraydata()
      // when building the numpy array
      Py_DECREF( pyarr );
      Py_DECREF( pyobj );

      if( res < 0 )
      {
        *sipIsErr = 1;
        delete *sipCppPtr;
        *sipCppPtr = 0;
        return 0;
      }
    }

%#else%

    *sipCppPtr = new std::vector<%Template1% >;
    unsigned	i, n = PySequence_Size( sipPy );
    (*sipCppPtr)->reserve( n );
    PyObject	*pyitem;
    for( i=0; i<n; ++i )
    {
      pyitem = PySequence_GetItem( sipPy, i );
      bool ok = false;
      bool conv = false;
      if( pyitem )
      {
        if( %Template1testPyType%( pyitem ) )
          ok = true;
        else
        {
          const sipTypeDef* st = sipFindType( "%Template1sipClass%" );
          if( st )
          {
            if( sipCanConvertToType( pyitem, st, SIP_NOT_NONE ) )
            {
              ok = true;
              conv = true;
            }
          }
        }
      }
      if( !ok )
      {
        *sipIsErr = 1;
        delete *sipCppPtr;
        sipCppPtr = 0;
        std::ostringstream s;
        s << "wrong list item type, item " << i;
        PyErr_SetString( PyExc_TypeError, s.str().c_str() );
        if( pyitem )
          Py_DECREF( pyitem );
        return 0;
      }

      %Template1% %Template1deref% item = %Template1castFromSip% %Template1CFromPy%( pyitem );
      (*sipCppPtr)->push_back( %Template1deref%item );
%%Template1defScalar%%
%#ifndef PYAIMS_SCALAR%
      if( conv )
        delete & %Template1deref% item;
%#endif%
      Py_DECREF( pyitem );
    }

%#endif%

    return sipGetState( sipTransferObj );
  }
  *sipCppPtr =
     (vector_%Template1typecode% *) 
     sipConvertToType( sipPy, sipType_vector_%Template1typecode%,
                           0, SIP_NO_CONVERTORS, 0, sipIsErr );
  return 0;
%End

public:
  vector_%Template1typecode%();
  //vector_%Template1typecode%( const vector_%Template1typecode% & );

  vector_%Template1typecode%( SIP_PYOBJECT );
%MethodCode
  if( !PySequence_Check( a0 ) )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "wrong argument type" );
  }
  else
  {

    if( sipCanConvertToType( a0, sipType_vector_%Template1typecode%,
                             SIP_NO_CONVERTORS ) )
    {
      int state = 0;

      vector_%Template1typecode% * dat
        = (vector_%Template1typecode% *)
          sipConvertToType( a0,
            sipType_vector_%Template1typecode%,
            0, SIP_NO_CONVERTORS, &state, &sipIsErr );
      if( sipIsErr && dat )
      {
        sipReleaseType( dat, sipType_vector_%Template1typecode%, state );
        dat = 0;
      }
      else if( dat )
      {
        sipCpp = new vector_%Template1typecode%( *dat );
        sipReleaseType( dat, sipType_vector_%Template1typecode%, state );
      }
    }

    else
    {
      // not a vector

      unsigned	i, n = PySequence_Size( a0 );
      sipCpp = new vector_%Template1typecode%;

      // --- numpy impl ---

%%Template1defScalar%%
%%Template1defNumpyBindings%%
%#if defined( PYAIMS_SCALAR ) || defined( PYAIMS_NUMPY_BINDINGS )%
%%Template1defNumpyIsSubArray%%

      sipCpp->resize( n );

      if( n != 0 )
      {
        std::vector<int> dims( 1, n );
        std::vector<int> added_dims = %Template1NumDims%;
        dims.insert( dims.end(), added_dims.begin(), added_dims.end() );

        PyArray_Descr *descr = %Template1NumType_Descr%;
        if( !descr )
          descr = PyArray_DescrFromType( %Template1NumType% );
        PyObject *pyarr = aims::initNumpyArray( (PyObject *) sipSelf, descr,
                                                dims.size(),
                                                &dims[0],
                                                (char *) &(*sipCpp)[0] );
%#ifdef PYAIMS_NPY_IS_SUBARRAY%
        PyObject *sub_arr = PyMapping_GetItemString(
          pyarr, const_cast<char *>( "v" ) );
        if( sub_arr )
        {
          Py_DECREF( pyarr ); // we don't use the whole array
          pyarr = sub_arr;
        }
%#endif%
        int res = PySequence_SetSlice( pyarr, 0, n, a0 );

        // delete internal ref to the array which nobody else owns
        PyObject_DelAttrString( (PyObject *) sipSelf, "_arrayref" );
        // dec reference to self that was manually incremented in arraydata()
        // when building the numpy array
        Py_DECREF( (PyObject *) sipSelf );
        Py_DECREF( pyarr );

        if( res < 0 )
        {
          sipIsErr = 1;
          // Py_DECREF( pyarr );
          delete sipCpp;
          sipCpp = 0;
        }
      }

%#else%

      // ---

      sipCpp->reserve( n );
      PyObject	*pyitem;
      for( i=0; i<n; ++i )
      {
        pyitem = PySequence_GetItem(a0,i);
        bool ok = false;
        bool conv = false;
        if( pyitem )
        {
          if( %Template1testPyType%( pyitem ) )
            ok = true;
          else
          {
            const sipTypeDef* st = sipFindType( "%Template1sipClass%" );
            if( st )
            {
              if( sipCanConvertToType( pyitem, st, SIP_NOT_NONE ) )
              {
                ok = true;
                conv = true;
              }
            }
          }
        }
        if( !ok )
        {
          sipIsErr = 1;
          delete sipCpp;
          sipCpp = 0;
          std::ostringstream s;
          s << "wrong list item type, item " << i;
          PyErr_SetString( PyExc_TypeError, s.str().c_str() );
          if( pyitem )
            Py_DECREF( pyitem );
          break;
        }

        %Template1% %Template1deref% item = %Template1castFromSip% %Template1CFromPy%( pyitem );
        sipCpp->push_back( %Template1deref%item );
%%Template1defScalar%%
%#ifndef PYAIMS_SCALAR%
        if( conv )
          delete & %Template1deref% item;
        Py_DECREF( pyitem );
%#endif%
      }

%#endif%
    }
  }
%End


  ~vector_%Template1typecode%();

  // assignation operator
  void assign( SIP_PYOBJECT );
%MethodCode
  bool ok = false;

  if( sipCanConvertToType( a0, sipType_vector_%Template1typecode%,
                           SIP_NO_CONVERTORS ) )
  {
    int state = 0;

    vector_%Template1typecode% * dat
      = (vector_%Template1typecode% *)
        sipConvertToType( a0,
          sipType_vector_%Template1typecode%,
          0, SIP_NO_CONVERTORS, &state, &sipIsErr );
    if( sipIsErr && dat )
    {
      sipReleaseType( dat, sipType_vector_%Template1typecode%, state );
      dat = 0;
    }
    else if( dat )
    {
      *sipCpp = *dat;
      sipReleaseType( dat, sipType_vector_%Template1typecode%, state );
      ok = true;
    }
  }
  else if( PySequence_Check( a0 ) )
  {
    // --- numpy impl ---

%%Template1defScalar%%
%%Template1defNumpyBindings%%
%#if defined( PYAIMS_SCALAR ) || defined( PYAIMS_NUMPY_BINDINGS )%
%%Template1defNumpyIsSubArray%%

    unsigned	n = PySequence_Size( a0 );
    sipCpp->resize( n );

    if( n != 0 )
    {
      std::vector<int> dims( 1, n );
      std::vector<int> added_dims = %Template1NumDims%;
      dims.insert( dims.end(), added_dims.begin(), added_dims.end() );

      PyObject *pyarr = PyObject_GetAttrString( sipSelf, "np" );
      int res = PySequence_SetSlice( pyarr, 0, n, a0 );

      Py_DECREF( pyarr );

      if( res == 0 )
        ok = true;
    }
    else
      ok = true;

    // ---

%#else%

    if( sipCanConvertToType( a0, sipType_vector_%Template1typecode%, 0 ) )
    {

      int state = 0;
      vector_%Template1typecode% * dat
        = (vector_%Template1typecode% *)
          sipConvertToType( a0,
            sipType_vector_%Template1typecode%,
            0, 0, &state, &sipIsErr );
      if( sipIsErr && dat )
      {
        sipReleaseType( dat, sipType_vector_%Template1typecode%, state );
        dat = 0;
      }
      else if( dat )
      {
        *sipCpp = *dat;
        sipReleaseType( dat, sipType_vector_%Template1typecode%, state );
        ok = true;
      }
    }
%#endif%
  }

  if( ok )
  {
    std::vector<int> dims( 1, sipCpp->size() );
    std::vector<int> added_dims = %Template1NumDims%;
    dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
    aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                            (char *) &(*sipCpp)[0] );
  }
  else
  {
    sipIsErr = 1;
  }
%End


/* we return a SIP_PYOBJECT rather than a %Template1% because if %Template1% 
     is a MappedType (and not a binding), SIP considers it is a temporary 
     object, and deletes it right after its use, even if it's actually a 
     pointer to a C++ owned object.
  */
  SIP_PYOBJECT __getitem__( int ) const;
%MethodCode
  if( a0 < 0 )
    a0 = sipCpp->size() + a0;
  if( a0 < 0 || a0 >= (long)sipCpp->size() )
  {
    sipRes = 0;
    sipIsErr = 1;
    PyErr_SetString( PyExc_IndexError, "vector index out of range" );
  }
  else
    sipRes = %Template1pyFromC%( %Template1address%(*sipCpp)[ a0 ] );
%End


  void __setitem__( int, %Template1PyType%%Template1deref% );
%MethodCode
  if( a0 < 0 )
    a0 = sipCpp->size() + a0;
  if( a0 < 0 || a0 >= (long)sipCpp->size() )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_IndexError, "vector index out of range" );
  }
  else
    (*sipCpp)[ a0 ] = %Template1deref%a1;
%End


  size_t size() const;


  Py_ssize_t __len__() const;
%MethodCode
  sipRes = sipCpp->size();
%End

  int __contains__( %Template1PyType%%Template1deref% ) const;
%MethodCode
  std::vector<%Template1% >::const_iterator i, e = sipCpp->end();
  sipRes = 0;
  for( i=sipCpp->begin(); i!=e; ++i )
    if( %Template1pyaddress%*i == %Template1pyaddress% %Template1deref%a0 )
    {
      sipRes = 1;
      break;
    }
%End

  int index( %Template1PyType%%Template1deref% ) const;
%MethodCode
  std::vector<%Template1% >::const_iterator i, e = sipCpp->end();
  sipRes = -1;
  int n = 0;
  for( i=sipCpp->begin(); i!=e; ++i, ++n )
    if( %Template1pyaddress%*i == %Template1pyaddress% %Template1deref%a0 )
    {
      sipRes = n;
      break;
    }
  if( sipRes == -1 )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_ValueError, "element not in list" );
  }
%End

  void append( %Template1PyType%%Template1deref% );
%MethodCode
  sipCpp->push_back( %Template1deref%a0 );
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &(*sipCpp)[0] );
%End

  void resize( unsigned );
%MethodCode
  sipCpp->resize( a0 );
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &(*sipCpp)[0] );
%End

  void resize( unsigned, const %Template1PyType% & );
%MethodCode
  sipCpp->resize( a0,
    static_cast<const vector_%Template1typecode%::value_type &>(
      const_cast<%Template1PyType% &>( %Template1deref%a1 ) )  );
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &(*sipCpp)[0] );
%End

  void append( unsigned, const %Template1PyType% & );
%MethodCode
  sipCpp->insert( sipCpp->end(), a0,
    static_cast<const vector_%Template1typecode%::value_type &>(
      const_cast<%Template1PyType% &>( %Template1deref%a1 ) ) );
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &(*sipCpp)[0] );
%End

  void insert( int, %Template1PyType%%Template1deref% );
%MethodCode
  if( a0 < 0 )
    a0 += sipCpp->size();
  if( a0 >= sipCpp->size() )
    a0 = sipCpp->size() - 1;
  sipCpp->insert( sipCpp->begin() + a0, %Template1deref%a1 );
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &(*sipCpp)[0] );
%End

  bool operator == ( const vector_%Template1typecode% & ) const;
%MethodCode
  sipRes = ( (*sipCpp) == *a0 );
%End

  bool operator != ( const vector_%Template1typecode% & ) const;
%MethodCode
  sipRes = ( (*sipCpp) != *a0 );
%End


  vector_%Template1typecode% * operator + 
    ( const vector_%Template1typecode% & ) 
    /Factory/;
%MethodCode
  sipRes = new vector_%Template1typecode%;
  sipRes->reserve( sipCpp->size() + a0->size() );
  sipRes->insert( sipRes->begin(), sipCpp->begin(), sipCpp->end() );
  sipRes->insert( sipRes->end(), a0->begin(), a0->end() );
%End


  void operator += ( const vector_%Template1typecode% & );
%MethodCode
  sipCpp->insert( sipCpp->end(), a0->begin(), a0->end() );
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &(*sipCpp)[0] );
%End


  void remove( %Template1PyType%%Template1deref% );
%MethodCode
  std::vector<%Template1% >::iterator i, e = sipCpp->end();
  for( i=sipCpp->begin(); i!=e; ++i )
    if( %Template1compareElement%*i
      == %Template1compareElement%%Template1deref%a0 )
    {
      sipCpp->erase( i );
      std::vector<int> dims( 1, sipCpp->size() );
      std::vector<int> added_dims = %Template1NumDims%;
      dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
      aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                              (char *) &(*sipCpp)[0] );
      break;
    }
%End


  void __delitem__( int );
%MethodCode
  if( a0 < 0 )
    a0 = sipCpp->size() + a0;
  if( (unsigned) a0 < sipCpp->size() )
  {
    sipCpp->erase( sipCpp->begin() + a0 );
    std::vector<int> dims( 1, sipCpp->size() );
    std::vector<int> added_dims = %Template1NumDims%;
    dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
    aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                            (char *) &(*sipCpp)[0] );
  }
%End


  // __iter__ isn't called by python iter() function when defined in SIP
  vector_%Template1typecode%_iterator* __objiter__() /Factory/;
%MethodCode
  std::vector<%Template1% >::iterator i = sipCpp->begin();
  sipRes = new vector_%Template1typecode%_iterator( i, sipCpp, sipSelf );
%End


  SIP_PYLIST list();
%MethodCode
  // Convert to a Python list of %Template1%.

  // Create the list.
  if( (sipRes = PyList_New(sipCpp -> size())) == NULL )
    return NULL;

  // Get it.

  for( unsigned i = 0; i < sipCpp->size(); ++i )
    if( PyList_SetItem(sipRes,i,
        %Template1pyFromC%(%Template1address%(*sipCpp)[i])) 
        < 0 )
      {
        Py_DECREF(sipRes);
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_TypeError, "item conversion failed" );
        break;
      }
%End

  // printing

  SIP_PYOBJECT __str__();
%MethodCode
%%Template1defScalar%%
%#ifdef PYAIMS_SCALAR%
  std::ostringstream  ss;
  ss << "[";
  unsigned i, n = sipCpp->size();
  if( n > 200 )
    n = 200;
  for( i=0; i<n; ++i )
  {
    ss << (*sipCpp)[i];
    if( i < n-1 )
      ss << ", ";
  }
  if( n < sipCpp->size() )
    ss << ", ...";
  ss << "]";
  std::string	s = ss.str();
  const char	*s2 = s.c_str();
  if( s2 == 0 )
    s2 = "";
#if PY_VERSION_HEX >= 0x03000000
  sipRes = PyUnicode_FromString(s2);
#else
  sipRes = PyString_FromString(s2);
#endif
%#else%
  std::ostringstream  ss;
  ss << "[";
  unsigned i, n = sipCpp->size();
  PyObject *po, *str;
  if( n > 200 )
    n = 200;
  for( i=0; i<n; ++i )
  {
    po = %Template1pyFromC%(%Template1address%(*sipCpp)[i]);
    str = PyObject_Str( po );
    if( str )
    {
      if( !strcmp( "%Template1typecode%", "STRING" ) )
        ss << carto::quotedString( carto::PyString_AsStdString( str ) );
      else
        ss << carto::PyString_AsStdString( str );
    }
    else
      ss << "<object not printable>";
    if( i < n-1 )
      ss << ", ";
  }
  if( n < sipCpp->size() )
    ss << ", ...";
  ss << "]";
  std::string	s = ss.str();
  const char	*s2 = s.c_str();
  if( s2 == 0 )
    s2 = "";
#if PY_VERSION_HEX >= 0x03000000
  sipRes = PyUnicode_FromString(s2);
#else
  sipRes = PyString_FromString(s2);
#endif
%#endif%
%End

  // conversion from Object
  /*
  static rc_ptr_vector_%Template1typecode%
  rcFromObject( carto::GenericObject ) /Factory/;
%MethodCode
  try
    {
      carto::rc_ptr<vector_%Template1typecode%> & obj
        = a0->value<carto::rc_ptr<vector_%Template1typecode%> >();
      sipRes = new carto::rc_ptr<vector_%Template1typecode%>( obj );
    }
  catch( std::exception & e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
%End
  */

  static vector_%Template1typecode% *fromObject( carto::GenericObject );
%MethodCode
  try
  {
    vector_%Template1typecode% & obj
      = a0->value<vector_%Template1typecode%>();
    sipRes = &obj;
  }
  catch( std::exception & e )
  {
#define TEST_%Template1typecode%_TYPE
#ifdef TEST_STRING_TYPE
    try
    {
      vector_%Template1typecode% * pobj
        = a0->value<vector_%Template1typecode% *>();
      sipRes = pobj;
    }
    catch( std::exception & e )
#endif
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
#undef TEST_%Template1typecode%_TYPE
  }
%End

  carto::Object toObject();
%MethodCode
  sipRes = new carto::Object( carto::Object::value( *sipCpp ) );
%End

%%Template1defScalar%%
%%Template1defNumpyBindings%%
%#if defined( PYAIMS_SCALAR ) || defined( PYAIMS_NUMPY_BINDINGS )%
%%Template1defNumpyIsSubArray%%

  SIP_PYOBJECT arraydata() /Factory/;
%MethodCode
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );

  PyArray_Descr *descr = %Template1NumType_Descr%;
  if( !descr )
    descr = PyArray_DescrFromType( %Template1NumType% );
  sipRes = aims::initNumpyArray( sipSelf, descr, dims.size(),
                                 &dims[0],
                                 (char *) &(*sipCpp)[0] );
%#ifdef PYAIMS_NPY_IS_SUBARRAY%
  PyObject *sub_arr = PyMapping_GetItemString( sipRes,
                                               const_cast<char *>( "v" ) );
  if( sub_arr )
  {
    Py_DECREF( sipRes ); // we don't use the whole array
    sipRes = sub_arr;
  }
%#endif%
%End

  SIP_PYOBJECT __array__() /Factory/;
%MethodCode
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );

  PyArray_Descr *descr = %Template1NumType_Descr%;
  if( !descr )
    descr = PyArray_DescrFromType( %Template1NumType% );
  sipRes = aims::initNumpyArray( sipSelf, descr, dims.size(),
                                 &dims[0],
                                 (char *) &(*sipCpp)[0] );
%#ifdef PYAIMS_NPY_IS_SUBARRAY%
  PyObject *sub_arr = PyMapping_GetItemString( sipRes,
                                               const_cast<char *>( "v" ) );
  if( sub_arr )
  {
    Py_DECREF( sipRes ); // we don't use the whole array
    sipRes = sub_arr;
  }
%#endif%
%End


  SIP_PYOBJECT array_struct() /Factory/;
%Docstring
Return a numpy array of elements. For vectors of scalar types, this is the same as the other functions __array__() and arraydata(). But for vectors where the elements are "sub-arrays", ie objects that can be mapped to arrays, the returned array elements will be structures: for instance a Point3df will be mapped to numpy.dtype([('v', '3f4')])
%End

%MethodCode
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );

  PyArray_Descr *descr = %Template1NumType_Descr%;
  if( !descr )
    descr = PyArray_DescrFromType( %Template1NumType% );
  sipRes = aims::initNumpyArray( sipSelf, descr, dims.size(),
                                 &dims[0],
                                 (char *) &(*sipCpp)[0] );
%End


  void checkResize();
%MethodCode
  std::vector<int> dims( 1, sipCpp->size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &(*sipCpp)[0] );
%End

  void _arrayDestroyedCallback( SIP_PYOBJECT );
%MethodCode
  // remove weak reference in volume
  PyObject_DelAttrString( sipSelf, "_arrayref" );
  // dec reference to self that was manually incremented in arraydata()
  // when building the numpy array
  Py_DECREF( sipSelf );
  // the aditional ref to self will be deleted when the callback 
  // (method bound on self) is destroyed, just after now
  // so self can be destroyed safely
%End

%#endif%
};


class vector_%Template1typecode%_iterator
{
%TypeHeaderCode
#include <vector>
%Template1typeinclude%
%Template1sipinclude%
#ifndef PYAIMSSIP_VECTOR_%Template1typecode%_DEFINED
#define PYAIMSSIP_VECTOR_%Template1typecode%_DEFINED
typedef std::vector<%Template1% > vector_%Template1typecode%;
#endif
#ifndef PYAIMSSIP_VECTOR_%Template1typecode%_ITER_DEFINED
#define PYAIMSSIP_VECTOR_%Template1typecode%_ITER_DEFINED
class vector_%Template1typecode%_iterator 
{
public:
  vector_%Template1typecode%_iterator( vector_%Template1typecode%* v,
                                       PyObject* pyvec )
    : _vector( v ), _pyvec( pyvec )
  { Py_INCREF( pyvec ); }
  vector_%Template1typecode%_iterator( std::vector<%Template1% >::iterator & i,
                                       vector_%Template1typecode%* v,
                                       PyObject *pyvec  )
    : _iter( i ), _vector( v ), _pyvec( pyvec )
  { Py_INCREF( pyvec ); }
  ~vector_%Template1typecode%_iterator() { if( _pyvec ) { Py_DECREF( _pyvec ); } }

  vector_%Template1typecode%::iterator _iter;
  vector_%Template1typecode%	*_vector;
  PyObject *_pyvec;
};
#endif
%End

public:
  vector_%Template1typecode%_iterator(
    vector_%Template1typecode% * /GetWrapper/ ) /HoldGIL/;
%MethodCode
  sipCpp = new vector_%Template1typecode%_iterator( a0, a0Wrapper );
%End
  ~vector_%Template1typecode%_iterator() /HoldGIL/;

  SIP_PYOBJECT __objnext__();
%MethodCode
  if( sipCpp->_iter == sipCpp->_vector->end() )
  {
    sipRes = 0;
    sipIsErr = 1;
    PyErr_SetString( PyExc_StopIteration, "iterator outside bounds" );
  }
  else
  {
    sipRes = %Template1pyFromC%(%Template1address%(*sipCpp->_iter) );
    ++sipCpp->_iter;
  }
%End

  SIP_PYOBJECT __iter__();
%MethodCode
  sipRes = 
    sipConvertFromType( sipCpp,
                        sipType_vector_%Template1typecode%_iterator, 0 );
%End
};


