
class list_%Template1typecode%
{
%TypeHeaderCode
#include <list>
#include <cartobase/object/object.h>
#include <pyaims/object/numconv.h>
#include <cartobase/type/string_conversion.h>
%Template1typeinclude%
%Template1sipinclude%
#ifndef PYAIMSSIP_LIST_%Template1typecode%_DEFINED
#define PYAIMSSIP_LIST_%Template1typecode%_DEFINED
typedef std::list<%Template1% > list_%Template1typecode%;
#endif
%End

%ConvertToTypeCode
  if (sipIsErr == NULL)
  {
    if( sipCanConvertToInstance( sipPy,
        sipClass_list_%Template1typecode%, SIP_NOT_NONE | SIP_NO_CONVERTORS ) )
      return 1;
    if( !PySequence_Check( sipPy ) )
      return 0;
    // test compatibility of each element of the sequence
    unsigned    i, n = PySequence_Size( sipPy );
    PyObject    *pyitem;
    bool ok = true;
    for( i=0; i<n; ++i )
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

  if( sipCanConvertToInstance( sipPy, sipClass_list_%Template1typecode%,
      SIP_NO_CONVERTORS ) )
  {
    int state = 0;

    list_%Template1typecode% * dat
      = (list_%Template1typecode% *)
        sipConvertToInstance( sipPy,
          sipClass_list_%Template1typecode%,
          sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
    if( *sipIsErr && dat )
    {
      sipReleaseInstance( dat, sipClass_list_%Template1typecode%, state );
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
    *sipCppPtr = new std::list<%Template1% >;
    unsigned	i, n = PySequence_Size( sipPy );
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
  *sipCppPtr = (list_%Template1typecode% *)
  sipConvertToInstance( sipPy, sipClass_list_%Template1typecode%,
                           0, SIP_NO_CONVERTORS, 0, sipIsErr );
  return 0;
%End

public:
  list_%Template1typecode%();
  //list_%Template1typecode%( const list_%Template1typecode% & );

  list_%Template1typecode%( SIP_PYOBJECT );
%MethodCode
  if( !PySequence_Check( a0 ) )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "wrong argument type" );
  }
  else
  {
    unsigned	i, n = PySequence_Size( a0 );
    sipCpp = new list_%Template1typecode%;
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


  ~list_%Template1typecode%();

  // assignation operator: uses copy operator
  void assign( const list_%Template1typecode% & );
%MethodCode
  *sipCpp = *a0;
%End


/* we return a SIP_PYOBJECT rather than a %Template1% because if %Template1%
     is a MappedType (and not a binding), SIP considers it is a temporary 
     object, and deletes it right after its use, even if it's actually a 
     pointer to a C++ owned object.
  */
  SIP_PYOBJECT __getitem__( int ) const;
%MethodCode
  std::list<%Template1% >::iterator i, e = sipCpp->end();
  if( a0 < 0 )
    a0 = sipCpp->size() + a0;
  if( a0 < 0 || a0 >= sipCpp->size() )
  {
    sipRes = 0;
    sipIsErr = 1;
    PyErr_SetString( PyExc_IndexError, "std::list index out of range" );
  }
  else
  {
    int n;
    for( i=sipCpp->begin(), n=0; n<a0 && i!=e; ++i, ++n ) {}
    if( i == e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_IndexError, "std::list index out of range" );
    }
    else
      sipRes = %Template1pyFromC%( %Template1address% *i );
  }
%End


  void __setitem__( int, %Template1PyType%%Template1deref% );
%MethodCode
  std::list<%Template1% >::iterator i, e = sipCpp->end();
  if( a0 < 0 )
    a0 = sipCpp->size() + a0;
  if( a0 < 0 || a0 >= sipCpp->size() )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_IndexError, "std::list index out of range" );
  }
  else
  {
    int n;
    for( i=sipCpp->begin(), n=0; n<a0 && i!=e; ++i, ++n ) {}
    if( i == e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_IndexError, "std::list index out of range" );
    }
    else
      *i = %Template1deref%a1;
  }
%End


  size_t size() const;


  int __len__() const /AutoGen/;
%MethodCode
  sipRes = sipCpp->size();
%End

  int __contains__( %Template1PyType%%Template1deref% ) const /AutoGen/;
%MethodCode
  std::list<%Template1% >::iterator i, e = sipCpp->end();
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
  std::list<%Template1% >::const_iterator i, e = sipCpp->end();
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
%End


  bool operator == ( const list_%Template1typecode% & ) const;
%MethodCode
  sipRes = ( (*sipCpp) == *a0 );
%End

  bool operator != ( const list_%Template1typecode% & ) const;
%MethodCode
  sipRes = ( (*sipCpp) != *a0 );
%End


  list_%Template1typecode% * operator +
    ( const list_%Template1typecode% & ) /Factory/;
%MethodCode
  sipRes = new list_%Template1typecode%;
  sipRes->insert( sipRes->begin(), sipCpp->begin(), sipCpp->end() );
  sipRes->insert( sipRes->end(), a0->begin(), a0->end() );
%End


  void operator += ( const list_%Template1typecode% & );
%MethodCode
  sipCpp->insert( sipCpp->end(), a0->begin(), a0->end() );
%End


  void remove( %Template1PyType%%Template1deref% );
%MethodCode
  std::list<%Template1% >::iterator i, e = sipCpp->end();
  for( i=sipCpp->begin(); i!=e; ++i )
    if( %Template1compareElement%*i
      == %Template1compareElement%%Template1deref%a0 )
    {
      sipCpp->erase( i );
      break;
    }
%End


  void __delitem__( int );
%MethodCode
  std::list<%Template1% >::iterator i, e = sipCpp->end();
  if( a0 < 0 )
    a0 = sipCpp->size() - a0;
  if( a0 < 0 )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_IndexError, "std::list index out of range" );
  }
  else
  {
    int n;
    for( i=sipCpp->begin(), n=0; n<a0 && i!=e; ++i, ++n ) {}
    if( i == e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_IndexError, "std::list index out of range" );
    }
    else
      sipCpp->erase( i );
  }
%End


  // __iter__ isn't called by python iter() function when defined in SIP
  list_%Template1typecode%_iterator* __objiter__() /Factory, AutoGen/;
%MethodCode
  std::list<%Template1% >::iterator i = sipCpp->begin();
  sipRes = new list_%Template1typecode%_iterator( i, sipCpp, sipSelf );
%End


  SIP_PYLIST list();
%MethodCode
  // Convert to a Python list of %Template1%.

  // Create the list.
  if( (sipRes = PyList_New(sipCpp -> size())) == NULL )
    return NULL;

  // Get it.

  std::list<%Template1% >::iterator il, e = sipCpp->end();
  unsigned i;
  for( i = 0, il=sipCpp->begin(); il!=e; ++i, ++il )
    if( PyList_SetItem(sipRes,i,
        %Template1pyFromC%(%Template1address% *il)) < 0 )
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
  unsigned i, n = sipCpp->size(), n0 = n;
  std::list<%Template1% >::iterator il, e = sipCpp->end();
  if( n > 200 )
    n = 200;
  for( i=0, il=sipCpp->begin(); i<n; ++i, ++il )
  {
    ss << *il;
    if( i < n-1 )
      ss << ", ";
  }
  if( n < n0 )
    ss << ", ...";
  ss << "]";
  std::string	s = ss.str();
  sipRes = carto::PyString_FromStdString(s);
%#else%
  std::ostringstream  ss;
  ss << "[";
  unsigned i, n = sipCpp->size(), n0 = n;
  std::list<%Template1% >::iterator il, e = sipCpp->end();
  PyObject *po, *str;
  if( n > 200 )
    n = 200;
  for( i=0, il=sipCpp->begin(); i<n; ++i, ++il )
  {
    po = %Template1pyFromC%(%Template1address% *il);
    str = PyObject_Str( po );
    if( str )
      if( !strcmp( "%Template1typecode%", "STRING" ) )
        ss << carto::quotedString( carto::PyString_AsStdString( str ) );
      else
        ss << carto::PyString_AsStdString( str );
    else
      ss << "<object not printable>";
    if( i < n-1 )
      ss << ", ";
  }
  if( n < n0 )
    ss << ", ...";
  ss << "]";
  std::string	s = ss.str();
  sipRes = carto::PyString_FromStdString(s);
%#endif%
%End

  // conversion from Object
  /*
  static rc_ptr_list_%Template1typecode%
  rcFromObject( carto::GenericObject ) /Factory/;
%MethodCode
  try
  {
      carto::rc_ptr<list_%Template1typecode%> & obj
        = a0->value<carto::rc_ptr<list_%Template1typecode%> >();
      sipRes = new carto::rc_ptr<list_%Template1typecode%>( obj );
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
  static list_%Template1typecode% *fromObject( carto::GenericObject );
%MethodCode
  try
  {
    list_%Template1typecode% & obj
      = a0->value<list_%Template1typecode%>();
    sipRes = &obj;
  }
  catch( std::exception & e )
  {
#define TEST_%Template1typecode%_TYPE
#if 0 // def TEST_STRING_TYPE
    try
    {
      list_%Template1typecode% * pobj
        = a0->value<list_%Template1typecode% *>();
      sipRes = pobj;
    }
    catch( std::exception & e )
#endif
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
  }
%End
*/

};


class list_%Template1typecode%_iterator
{
%TypeHeaderCode
#include <list>
%Template1typeinclude%
%Template1sipinclude%
#ifndef PYAIMSSIP_LIST_%Template1typecode%_DEFINED
#define PYAIMSSIP_LIST_%Template1typecode%_DEFINED
typedef std::list<%Template1% > list_%Template1typecode%;
#endif
#ifndef PYAIMSSIP_LIST_%Template1typecode%_ITER_DEFINED
#define PYAIMSSIP_LIST_%Template1typecode%_ITER_DEFINED
class list_%Template1typecode%_iterator
{
  public:
  list_%Template1typecode%_iterator( list_%Template1typecode%* v,
                                     PyObject* pylist )
  : _list( v ), _pylist( pylist )
  { Py_INCREF( pylist ); }
  list_%Template1typecode%_iterator( std::list<%Template1% >::iterator
                                     & i,
                                     list_%Template1typecode%* v,
                                     PyObject* pylist )
    : _iter( i ), _list( v ), _pylist( pylist )
  { Py_INCREF( pylist ); }
  ~list_%Template1typecode%_iterator() { if( _pylist ) { Py_DECREF( _pylist ); } }

  list_%Template1typecode%::iterator _iter;
  list_%Template1typecode%	*_list;
  PyObject *_pylist;
};
#endif
%End

  public:
  list_%Template1typecode%_iterator( list_%Template1typecode% * /GetWrapper/ )
  /HoldGIL/;
%MethodCode
  sipCpp = new list_%Template1typecode%_iterator( a0, a0Wrapper );
%End
  ~list_%Template1typecode%_iterator() /HoldGIL/;

  SIP_PYOBJECT __objnext__();
%MethodCode
  if( sipCpp->_iter == sipCpp->_list->end() )
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
                            sipClass_list_%Template1typecode%_iterator, 0 );
%End
};

