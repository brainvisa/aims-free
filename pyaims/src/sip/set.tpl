
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
    return ( PySequence_Check( sipPy ) && !PyString_Check( sipPy ) )
           || sipCanConvertToInstance( sipPy, 
                                       sipClass_set_%Template1typecode%,
                                       SIP_NOT_NONE | SIP_NO_CONVERTORS );

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

  if( PySequence_Check( sipPy ) )
  {
    *sipCppPtr = new std::set<%Template1% >;
    unsigned    i, n = PySequence_Size( sipPy );
    PyObject    *pyitem;
    for( i=0; i<n; ++i )
    {
      pyitem = PySequence_GetItem( sipPy, i );
      if( !pyitem || !%Template1testPyType%( pyitem ) )
      {
        *sipIsErr = 1;
        delete *sipCppPtr;
        sipCppPtr = 0;
        std::ostringstream s;
        s << "wrong list item type, item " << i;
        PyErr_SetString( PyExc_TypeError, s.str().c_str() );
        return 0;
      }

      (*sipCppPtr)->insert( %Template1pyderef% %Template1castFromSip%
                            %Template1CFromPy%( pyitem ) );
      Py_DECREF( pyitem );
    }
    return sipGetState( sipTransferObj );
  }
  *sipCppPtr = 
     (set_%Template1typecode% *)
#if SIP_VERSION >= 0x040400
  sipConvertToInstance( sipPy, sipClass_set_%Template1typecode%,
                           sipTransferObj, SIP_NO_CONVERTORS, 0, sipIsErr );
  return sipGetState( sipTransferObj );
#else
  sipConvertToCpp( sipPy, sipClass_set_%Template1typecode%,
                   sipIsErr );
  return 0;
#endif
%End

public:
  set_%Template1typecode%();
  //set_%Template1typecode%( const set_%Template1typecode% & );

  set_%Template1typecode%( SIP_PYOBJECT );
%MethodCode
  if( !PySequence_Check( a0 ) )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "wrong argument type" );
  }
  else
  {
    unsigned    i, n = PySequence_Size( a0 );
    sipCpp = new set_%Template1typecode%;
    PyObject    *pyitem;
    for( i=0; i<n; ++i )
    {
      pyitem = PySequence_GetItem(a0,i);
      if( !pyitem || !%Template1testPyType%( pyitem ) )
      {
        sipIsErr = 1;
        delete sipCpp;
        sipCpp = 0;
        std::ostringstream s;
        s << "wrong list item type, item " << i;
        PyErr_SetString( PyExc_TypeError, s.str().c_str() );
        break;
      }

      sipCpp->insert( %Template1pyderef% %Template1castFromSip%
                      %Template1CFromPy%( pyitem ) );
    }
  }
%End


  ~set_%Template1typecode%();

  size_t size() const;

  int __len__() const /AutoGen/;
%MethodCode
  sipRes = sipCpp->size();
%End

  int __contains__( %Template1PyType%%Template1deref% ) const /AutoGen/;
%MethodCode
  sipRes = sipCpp->find( %Template1deref% a0 ) != sipCpp->end();
%End

  void __delitem__( %Template1PyType%%Template1deref% ) const /AutoGen/;
%MethodCode
  sipCpp->erase( %Template1deref% a0 );
%End



  void insert( %Template1PyType% & );

  void add( %Template1PyType%%Template1deref% );
%MethodCode
  sipCpp->insert( %Template1deref%a0 );
%End

  bool operator == ( const set_%Template1typecode% & ) const;
%MethodCode
  sipRes = ( (*sipCpp) == *a0 );
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
  set_%Template1typecode%_iterator* __objiter__() const /Factory, AutoGen/;
%MethodCode
  std::set<%Template1% >::iterator i = sipCpp->begin();
  sipRes = new set_%Template1typecode%_iterator( i, sipCpp );
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
  sipRes = PyString_FromString(s2);
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
      ss << PyString_AsString( str );
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
  sipRes = PyString_FromString(s2);
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
  set_%Template1typecode%_iterator( set_%Template1typecode%* v )
    : _set( v )
  { }
  set_%Template1typecode%_iterator( std::set<%Template1% >::iterator & i,
                                       set_%Template1typecode%* v )
    : _iter( i ), _set( v )
  { }
  ~set_%Template1typecode%_iterator() {}

  set_%Template1typecode%::iterator _iter;
  set_%Template1typecode%    *_set;
};
#endif
%End

public:
  set_%Template1typecode%_iterator( set_%Template1typecode% * );
  ~set_%Template1typecode%_iterator();

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

