
class set_%Template1typecode%
{
%TypeHeaderCode
#include <set>
#include <cartobase/object/object.h>
%Template1typeinclude%
%Template1sipinclude%
#ifndef PYAIMSSIP_SET_%Template1typecode%_DEFINED
#define PYAIMSSIP_SET_%Template1typecode%_DEFINED
typedef std::set<%Template1% > set_%Template1typecode%;
#endif
%End

%ConvertToTypeCode
  if (sipIsErr == NULL)
  {
    if( sipCanConvertToInstance( sipPy,
                                       sipClass_set_%Template1typecode%,
                                       SIP_NOT_NONE | SIP_NO_CONVERTORS ) )
      return true;

    PyObject *iter = PyObject_GetIter( sipPy );
    if( iter )
    {
      // FIXME: should check items types
      Py_DECREF( iter );
      return true;
    }
    return false;
  }

  if( sipCanConvertToInstance( sipPy, sipClass_set_%Template1typecode%,
      SIP_NO_CONVERTORS ) )
  {
    int state = 0;

    set_%Template1typecode% * dat
      = (set_%Template1typecode% *)
        sipConvertToInstance( sipPy,
          sipClass_set_%Template1typecode%,
          sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
    if( *sipIsErr && dat )
    {
      sipReleaseInstance( dat, sipClass_set_%Template1typecode%, state );
      dat = 0;
    }
    else if( dat )
    {
      *sipCppPtr = dat;
      return 0;
    }
  }

  PyObject *iter = PyObject_GetIter( sipPy );
  if( iter )
  {
    PyObject  *pyitem;
    *sipCppPtr = new set_%Template1typecode%;
    unsigned    i = 0;

    for( pyitem=PyIter_Next( iter ); pyitem; pyitem=PyIter_Next( iter ), ++i )
    {
      if( !%Template1testPyType%( pyitem ) )
      {
        *sipIsErr = 1;
        delete *sipCppPtr;
        sipCppPtr = 0;
        std::ostringstream s;
        s << "wrong list item type, item " << i;
        PyErr_SetString( PyExc_TypeError, s.str().c_str() );
        Py_DECREF( pyitem );
        Py_DECREF( iter );
        return 0;
      }

      (*sipCppPtr)->insert( %Template1pyderef% %Template1castFromSip%
                            %Template1CFromPy%( pyitem ) );
      Py_DECREF( pyitem );
    }
    Py_DECREF( iter );
    return sipGetState( sipTransferObj );
  }
  *sipCppPtr = 
     (set_%Template1typecode% *)
  sipConvertToInstance( sipPy, sipClass_set_%Template1typecode%,
                           sipTransferObj, SIP_NO_CONVERTORS, 0, sipIsErr );
  return sipGetState( sipTransferObj );
%End

public:
  set_%Template1typecode%();
  //set_%Template1typecode%( const set_%Template1typecode% & );

  set_%Template1typecode%( SIP_PYOBJECT );
%MethodCode
  PyObject *iter = PyObject_GetIter( a0 );
  if( !iter )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "wrong argument type" );
  }
  else
  {
    PyObject  *pyitem;
    sipCpp = new set_%Template1typecode%;
    unsigned    i = 0;

    for( pyitem=PyIter_Next( iter ); pyitem; pyitem=PyIter_Next( iter ), ++i )
    {
      if( !%Template1testPyType%( pyitem ) )
      {
        sipIsErr = 1;
        delete sipCpp;
        sipCpp = 0;
        std::ostringstream s;
        s << "wrong list item type, item " << i;
        PyErr_SetString( PyExc_TypeError, s.str().c_str() );
        Py_DECREF( pyitem );
        break;
      }

      sipCpp->insert( %Template1pyderef% %Template1castFromSip%
                      %Template1CFromPy%( pyitem ) );
      Py_DECREF( pyitem );
    }
    Py_DECREF( iter );
  }
%End


  ~set_%Template1typecode%();

  size_t size() const;

  int __len__() const;
%MethodCode
  sipRes = sipCpp->size();
%End

  int __contains__( %Template1PyType%%Template1deref% ) const;
%MethodCode
  sipRes = sipCpp->find( %Template1deref% a0 ) != sipCpp->end();
%End

  void __delitem__( %Template1PyType%%Template1deref% ) const;
%MethodCode
  sipCpp->erase( %Template1deref% a0 );
%End



  void insert( %Template1PyType%%Template1deref% );
%MethodCode
  sipCpp->insert( %Template1deref%a0 );
%End

  void add( %Template1PyType%%Template1deref% );
%MethodCode
  sipCpp->insert( %Template1deref%a0 );
%End

  bool operator == ( const set_%Template1typecode% & ) const;
%MethodCode
  sipRes = ( (*sipCpp) == *a0 );
%End

  bool operator != ( const set_%Template1typecode% & ) const;
%MethodCode
  sipRes = ( (*sipCpp) != *a0 );
%End

/*
  set_%Template1typecode% * operator +
    ( const set_%Template1typecode% & )
    /Factory/;
%MethodCode
  sipRes = new set_%Template1typecode%;
  sipRes->insert( sipCpp->begin(), sipCpp->end() );
  sipRes->insert( a0->begin(), a0->end() );
%End
*/

  void operator += ( const set_%Template1typecode% & );
%MethodCode
  sipCpp->insert( a0->begin(), a0->end() );
%End


  void remove( %Template1PyType%%Template1deref% );
%MethodCode
  sipCpp->erase( %Template1deref% a0 );
%End


  // __iter__ isn't called by python iter() function when defined in SIP
  set_%Template1typecode%_iterator* __objiter__() /Factory/;
%MethodCode
  std::set<%Template1% >::iterator i = sipCpp->begin();
  sipRes = new set_%Template1typecode%_iterator( i, sipCpp, sipSelf );
%End


  SIP_PYLIST list() const;
%MethodCode
  // Convert to a Python list of %Template1%.

  // Create the list.
  if( (sipRes = PyList_New(sipCpp -> size())) == NULL )
    return NULL;

  // Get it.

  unsigned  i = 0;
  std::set<%Template1% >::const_iterator is, es = sipCpp->end();
  for( is = sipCpp->begin(); is!=es; ++is, ++i )
    if( PyList_SetItem(sipRes,i,
        %Template1pyFromC%(%Template1address%(*is)))
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
  set_%Template1typecode%::const_iterator is;
  for( is=sipCpp->begin(), i=0; i<n; ++i, ++is )
  {
    ss << *is;
    if( i < n-1 )
      ss << ", ";
  }
  if( n < sipCpp->size() )
    ss << ", ...";
  ss << " ]";
  std::string   s = ss.str();
  const char    *s2 = s.c_str();
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
  set_%Template1typecode%::const_iterator is;
  PyObject *po, *str;
  if( n > 200 )
    n = 200;
  for( is=sipCpp->begin(), i=0; i<n; ++i, ++is )
  {
    po = %Template1pyFromC%(%Template1address%(*is));
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
  std::string   s = ss.str();
  const char    *s2 = s.c_str();
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
  static rc_ptr_set_%Template1typecode%
  rcFromObject( carto::GenericObject ) /Factory/;
%MethodCode
  try
    {
      carto::rc_ptr<set_%Template1typecode%> & obj
        = a0->value<carto::rc_ptr<set_%Template1typecode%> >();
      sipRes = new carto::rc_ptr<set_%Template1typecode%>( obj );
    }
  catch( std::exception & e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
%End
  */

/*
  static set_%Template1typecode% *fromObject( carto::GenericObject );
%MethodCode
  try
    {
      set_%Template1typecode% & obj
        = a0->value<set_%Template1typecode%>();
      sipRes = &obj;
    }
  catch( std::exception & e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
%End
*/
};


class set_%Template1typecode%_iterator
{
%TypeHeaderCode
#include <set>
%Template1typeinclude%
%Template1sipinclude%
#ifndef PYAIMSSIP_SET_%Template1typecode%_DEFINED
#define PYAIMSSIP_SET_%Template1typecode%_DEFINED
typedef std::set<%Template1% > set_%Template1typecode%;
#endif
#ifndef PYAIMSSIP_SET_%Template1typecode%_ITER_DEFINED
#define PYAIMSSIP_SET_%Template1typecode%_ITER_DEFINED
class set_%Template1typecode%_iterator
{
public:
  set_%Template1typecode%_iterator( set_%Template1typecode%* v, PyObject *pyset )
    : _set( v ), _pyset( pyset )
  { Py_INCREF( pyset ); /* prevent the set from dying */ }
  set_%Template1typecode%_iterator( std::set<%Template1% >::iterator & i,
                                    set_%Template1typecode%* v, PyObject* pyset )
    : _iter( i ), _set( v ), _pyset( pyset )
  { Py_INCREF( pyset ); }
  ~set_%Template1typecode%_iterator() { if( _pyset ) { Py_DECREF( _pyset );} }

  set_%Template1typecode%::iterator _iter;
  set_%Template1typecode%    *_set;
  PyObject *_pyset;
};
#endif
%End

public:
  set_%Template1typecode%_iterator( set_%Template1typecode% * /GetWrapper/ )
    /HoldGIL/;
%MethodCode
  sipCpp = new set_%Template1typecode%_iterator( a0, a0Wrapper );
%End
  ~set_%Template1typecode%_iterator() /HoldGIL/;

  SIP_PYOBJECT __objnext__();
%MethodCode
  if( sipCpp->_iter == sipCpp->_set->end() )
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
                            sipClass_set_%Template1typecode%_iterator, 0 );
%End
};

