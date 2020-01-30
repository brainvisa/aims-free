
class vector_%Template1typecode%
{
%TypeHeaderCode
#include <vector>
#include <cartobase/object/object.h>
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
    if( sipCanConvertToInstance( sipPy,
        sipClass_vector_%Template1typecode%, SIP_NOT_NONE | SIP_NO_CONVERTORS ) )
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

  if( sipCanConvertToInstance( sipPy, sipClass_vector_%Template1typecode%,
      SIP_NO_CONVERTORS ) )
  {
    int state = 0;

    vector_%Template1typecode% * dat
      = (vector_%Template1typecode% *)
        sipConvertToInstance( sipPy,
          sipClass_vector_%Template1typecode%,
          sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
    if( *sipIsErr && dat )
    {
      sipReleaseInstance( dat, sipClass_vector_%Template1typecode%, state );
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
      if( conv )
        delete & %Template1deref% item;
      Py_DECREF( pyitem );
    }
    return sipGetState( sipTransferObj );
  }
  *sipCppPtr = 
     (vector_%Template1typecode% *) 
     sipConvertToInstance( sipPy, sipClass_vector_%Template1typecode%,
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
    unsigned	i, n = PySequence_Size( a0 );
    sipCpp = new vector_%Template1typecode%;
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
      if( conv )
        delete & %Template1deref% item;
      Py_DECREF( pyitem );
    }
  }
%End


  ~vector_%Template1typecode%();

  // assignation operator: uses copy operator
  void assign( const vector_%Template1typecode% & );
%MethodCode
  *sipCpp = *a0;
  int dims = sipCpp->size();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
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
  if( a0 < 0 || a0 >= sipCpp->size() )
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
  if( a0 < 0 || a0 >= sipCpp->size() )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_IndexError, "vector index out of range" );
  }
  else
    (*sipCpp)[ a0 ] = %Template1deref%a1;
%End


  size_t size() const;


  int __len__() const /AutoGen/;
%MethodCode
  sipRes = sipCpp->size();
%End

  int __contains__( %Template1PyType%%Template1deref% ) const /AutoGen/;
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
  int dims = sipCpp->size();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
%End

  void resize( unsigned );
%MethodCode
  sipCpp->resize( a0 );
  int dims = sipCpp->size();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
%End

  void resize( unsigned, const %Template1PyType% & );
%MethodCode
  sipCpp->resize( a0,
    static_cast<const vector_%Template1typecode%::value_type &>(
      const_cast<%Template1PyType% &>( %Template1deref%a1 ) )  );
  int dims = sipCpp->size();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
%End

  void append( unsigned, const %Template1PyType% & );
%MethodCode
  sipCpp->insert( sipCpp->end(), a0,
    static_cast<const vector_%Template1typecode%::value_type &>(
      const_cast<%Template1PyType% &>( %Template1deref%a1 ) ) );
  int dims = sipCpp->size();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
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
  int dims = sipCpp->size();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
%End


  void remove( %Template1PyType%%Template1deref% );
%MethodCode
  std::vector<%Template1% >::iterator i, e = sipCpp->end();
  for( i=sipCpp->begin(); i!=e; ++i )
    if( %Template1compareElement%*i
      == %Template1compareElement%%Template1deref%a0 )
    {
      sipCpp->erase( i );
      int dims = sipCpp->size();
      aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
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
    int dims = (int) sipCpp->size();
    aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
  }
%End


  // __iter__ isn't called by python iter() function when defined in SIP
  vector_%Template1typecode%_iterator* __objiter__() /Factory, AutoGen/;
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
  ss << "[ ";
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
  ss << " ]";
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
  ss << "[ ";
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
#if PY_VERSION_HEX >= 0x03000000
      PyObject *enc = PyUnicode_EncodeLocale( str, 0 );
      ss << PyBytes_AsString( enc );
      Py_DECREF( enc );
#else
      ss << PyString_AsString( str );
#endif
    }
    else
      ss << "<object not printable>";
    if( i < n-1 )
      ss << ", ";
  }
  if( n < sipCpp->size() )
    ss << ", ...";
  ss << " ]";
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

  SIP_PYOBJECT arraydata() /Factory/;
%MethodCode
  int dims = sipCpp->size();
  sipRes = aims::initNumpyArray( sipSelf, %Template1NumType%, 1, &dims,
                                 (char *) &(*sipCpp)[0] );
%End

  SIP_PYOBJECT __array__() /Factory/;
%MethodCode
  int dims = sipCpp->size();
  sipRes = aims::initNumpyArray( sipSelf, %Template1NumType%, 1, &dims,
                                 (char *) &(*sipCpp)[0] );
%End

  void checkResize();
%MethodCode
  int dims = sipCpp->size();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
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
    sipConvertFromInstance( sipCpp, 
                            sipClass_vector_%Template1typecode%_iterator, 0 );
%End
};


