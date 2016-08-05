
class AimsVector_%Template1typecode%_%Template2typecode%
{
%TypeHeaderCode
#include <aims/vector/vector.h>
#ifndef PYAIMS_AIMSVECTOR_%Template1typecode%_%Template2typecode%_DEFINED
#define PYAIMS_AIMSVECTOR_%Template1typecode%_%Template2typecode%_DEFINED
typedef AimsVector<%Template1%, %Template2%> 
  AimsVector_%Template1typecode%_%Template2typecode%;
#endif
%End

%TypeCode
#include <pyaims/vector/numpyarrayfunc.h>
%End

%ConvertToTypeCode
  if (sipIsErr == NULL)
  {
    if( sipCanConvertToInstance( sipPy,
         sipClass_AimsVector_%Template1typecode%_%Template2typecode%,
         SIP_NOT_NONE | SIP_NO_CONVERTORS ) )
      return 1;
    if( PySequence_Check( sipPy ) && PySequence_Size( sipPy ) == %Template2% )
    {
      // test compatibility of each element of the sequence
      unsigned    i;
      PyObject    *pyitem;
      for( i=0; i<%Template2%; ++i )
      {
        pyitem = PySequence_GetItem( sipPy, i );
        if( !pyitem || !%Template1testPyType%( pyitem ) )
        {
          if( pyitem )
            Py_DECREF( pyitem );
          return 0;
        }
        Py_DECREF( pyitem );
      }
      return 1;
    }
    return 0;
  }

  if( PyObject_IsInstance( sipPy, (PyObject *) 
      sipClass_AimsVector_%Template1typecode%_%Template2typecode% ) )
    {
      *sipCppPtr =
        (AimsVector_%Template1typecode%_%Template2typecode% *)
        sipConvertToInstance( sipPy,
          sipClass_AimsVector_%Template1typecode%_%Template2typecode%,
          0, SIP_NO_CONVERTORS, 0, sipIsErr );
      return 0;
    }
  if( PySequence_Check( sipPy ) && PySequence_Size( sipPy ) == %Template2% )
  {
    *sipCppPtr = new AimsVector_%Template1typecode%_%Template2typecode%;
    PyObject	*pyitem;
    unsigned	i;
    for( i=0; i<%Template2%; ++i )
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
        if( pyitem )
          Py_DECREF( pyitem );
        return 0;
      }

      (**sipCppPtr)[ i ] = %Template1pyderef% %Template1castFromSip% 
                           %Template1CFromPy%( pyitem );
      Py_DECREF(pyitem);
    }
    return sipGetState( sipTransferObj );
  }
  *sipCppPtr = 
    (AimsVector_%Template1typecode%_%Template2typecode% *) 
    sipConvertToInstance( sipPy,
      sipClass_AimsVector_%Template1typecode%_%Template2typecode%, 
      0, SIP_NO_CONVERTORS, 0, sipIsErr );
  return 0;
%End

public:
%%Template1defScalar%%
%%Template1defNumpyBindings%%

  AimsVector_%Template1typecode%_%Template2typecode%();
  AimsVector_%Template1typecode%_%Template2typecode%
    ( const %Template1% %Template1deref% );
  AimsVector_%Template1typecode%_%Template2typecode%
    ( const %Template1% %Template1deref%, const %Template1% %Template1deref%, 
      const %Template1% %Template1deref% );
  AimsVector_%Template1typecode%_%Template2typecode%( const 
    AimsVector_%Template1typecode%_%Template2typecode% & );
  /* sequence */

  AimsVector_%Template1typecode%_%Template2typecode%( SIP_PYOBJECT );
%MethodCode
  bool done = false;
%#if defined( PYAIMS_SCALAR ) || defined( PYAIMS_NUMPY_BINDINGS )%
  if( PyArray_Check( a0 ) )
  {
    PyArrayObject *arr = 0;
    arr = (PyArrayObject *) a0;
    if( arr->nd < 0 || arr->nd >1 )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, 
                       "Array dimensions are not compatible with "
                       "AimsVector_%Template1typecode%_%Template2typecode%" );
      done = true; 
    }
    else if( arr->descr->type_num != %Template1NumType% )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_TypeError, "wrong array data type" );
    }
    if( !sipIsErr )
    {
      // retreive dimensions
      int dims = arr->dimensions[0], i;
      if( dims != %Template2% )
      {
        sipIsErr = 1;
        PyErr_SetString( PyExc_RuntimeError, 
                         "Array size is not compatible with "
                         "AimsVector_%Template1typecode%_%Template2typecode%"
                       );
        done = true; 
      }
      else
      {
        sipCpp = new AimsVector_%Template1typecode%_%Template2typecode%;
        for( i=0; i<dims; ++i )
          (*sipCpp)[i] = ((%Template1% *)( PyArray_DATA(arr) ))[i];
        done = true;
      }
    }
  }
%#endif%

  if( !done )
  {
    if( !PySequence_Check( a0 ) )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_TypeError, "wrong argument type" );
    }
    else
    {
      unsigned  i, n = PySequence_Size( a0 );
      if( n != %Template2% )
      {
        sipIsErr = 1;
        PyErr_SetString( PyExc_TypeError,
                         "sequence size is not %Template2typecode%" );
      }
      else
      {
        sipCpp = new AimsVector_%Template1typecode%_%Template2typecode%;
        PyObject  *pyitem;
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
            if( !sipIsErr )
              PyErr_SetString( PyExc_TypeError, s.str().c_str() );
            if( pyitem )
              Py_DECREF( pyitem );
            break;
          }

          (*sipCpp)[i] = %Template1pyderef% %Template1castFromSip%
                        %Template1CFromPy%( pyitem );
          Py_DECREF( pyitem );
        }
        sipIsErr = 0;
      }
    }
  }
%End

 ~AimsVector_%Template1typecode%_%Template2typecode%();

 AimsVector_%Template1typecode%_%Template2typecode%* __deepcopy__( SIP_PYOBJECT ) /Factory/;
%MethodCode
  sipRes = new AimsVector_%Template1typecode%_%Template2typecode%( *sipCpp );
%End

 bool operator == 
   ( const AimsVector_%Template1typecode%_%Template2typecode% & ) const;
%MethodCode
  sipRes = ( (*sipCpp) == *a0 );
%End

 AimsVector_%Template1typecode%_%Template2typecode%
   operator+ ( AimsVector_%Template1typecode%_%Template2typecode% & );
 AimsVector_%Template1typecode%_%Template2typecode% 
   operator- ( AimsVector_%Template1typecode%_%Template2typecode% & );
 AimsVector_%Template1typecode%_%Template2typecode% operator* ( double );
 AimsVector_%Template1typecode%_%Template2typecode% operator/ ( double );
 bool isNull() const;
 float norm() const;
 float norm2() const;
 double dnorm() const;
 double dnorm2() const;

 %Template1% dot( AimsVector_%Template1typecode%_%Template2typecode% & );

 AimsVector_%Template1typecode%_3 *
   crossed( const AimsVector_%Template1typecode%_%Template2typecode% & )
   /Factory/;
%MethodCode
  sipRes = new AimsVector_%Template1typecode%_3(crossed(*sipCpp, *a0));
%End

 %Template1% item(int) const;
 SIP_PYOBJECT items( int = %Template2% ) const;
%MethodCode
  if( a0 > %Template2% )
    a0 = %Template2%;
  PyObject *t = PyTuple_New(a0);
  int i;
  for (i = 0; i < a0; i++)
    PyTuple_SET_ITEM(t, i, %Template1pyFromC%(%Template1address%(*sipCpp)[i]));
  sipRes = t;
%End

 void setItems( const AimsVector_%Template1typecode%_%Template2typecode%%Template1address% );
%MethodCode
	sipCpp->operator= (*a0);
%End

  SIP_PYOBJECT __getitem__( int ) const throw();
%MethodCode
  if( a0 < 0 )
    a0 = %Template2% + a0;
  if( a0 >= %Template2% )
    throw std::runtime_error( "Index out of range" );
  sipRes = %Template1pyFromC%( %Template1address%(*sipCpp)[ a0 ] );
%End

  void __setitem__( int, %Template1%%Template1deref% ) throw();
%MethodCode
  if( a0 < 0 )
    a0 = %Template2% + a0;
  if( a0 >= %Template2% )
    throw std::runtime_error( "Index out of range" );
  (*sipCpp)[ a0 ] = %Template1deref%a1;
%End

  int __len__();
%MethodCode
  sipRes = %Template2%;
%End

  SIP_PYLIST list() const;
%MethodCode
  // Convert to a Python list of %Template1%.

  // Create the list.
  if( (sipRes = PyList_New( %Template2% )) == NULL )
    return NULL;

  // Get it.

  for( unsigned i = 0; i < %Template2%; ++i )
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


  // assignation operator: uses copy operator
  void assign( const AimsVector_%Template1typecode%_%Template2typecode%* );
%MethodCode
  *sipCpp = *a0;
%End

  int __contains__( %Template1%%Template1deref% ) const /AutoGen/;
%MethodCode
  unsigned i;
  sipRes = 0;
  for( i=0; i!=%Template2%; ++i )
    if( (*sipCpp)[i] == %Template1pyaddress% %Template1deref%a0 )
    {
      sipRes = 1;
      break;
    }
%End

  // conversion from Object
  static AimsVector_%Template1typecode%_%Template2typecode% *
    fromObject( carto::GenericObject );
%MethodCode
  try
    {
      AimsVector_%Template1typecode%_%Template2typecode% & obj
        = a0->value<AimsVector_%Template1typecode%_%Template2typecode%>();
      sipRes = &obj;
    }
  catch( std::exception & e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
%End

  AimsVector_%Template1typecode%_%Template2typecode% &
    operator += ( const AimsVector_%Template1typecode%_%Template2typecode% & );

 AimsVector_%Template1typecode%_%Template2typecode% & operator += ( double );
%MethodCode
  *sipCpp += AimsVector_%Template1typecode%_%Template2typecode%( (%Template1%)a0 );
%End

  AimsVector_%Template1typecode%_%Template2typecode% &
    operator -= ( const AimsVector_%Template1typecode%_%Template2typecode% & );

 AimsVector_%Template1typecode%_%Template2typecode% & operator -= ( double );
%MethodCode
  *sipCpp -= AimsVector_%Template1typecode%_%Template2typecode%( (%Template1%)a0 );
%End

  AimsVector_%Template1typecode%_%Template2typecode% &
    operator *= ( double );

  AimsVector_%Template1typecode%_%Template2typecode% &
    operator /= ( double );

  AimsVector_%Template1typecode%_%Template2typecode% & normalize();

  AimsVector_%Template1typecode%_%Template2typecode% * __neg__() const;
%MethodCode
  sipRes = new AimsVector_%Template1typecode%_%Template2typecode%( -*sipCpp );
%End

%#if defined( PYAIMS_SCALAR ) || defined( PYAIMS_NUMPY_BINDINGS )%

  SIP_PYOBJECT arraydata() /Factory/;
%MethodCode
  int dims = %Template2%;
  sipRes = aims::initNumpyArray( sipSelf, %Template1NumType%, 1, &dims,
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


AimsVector_%Template1typecode%_%Template2typecode% operator + ( AimsVector_%Template1typecode%_%Template2typecode% &, %Template1% );
%MethodCode
  sipRes = new AimsVector_%Template1typecode%_%Template2typecode%( *a0 + AimsVector_%Template1typecode%_%Template2typecode%( a1 ) );
%End

AimsVector_%Template1typecode%_%Template2typecode% operator + ( %Template1%, AimsVector_%Template1typecode%_%Template2typecode% & );
%MethodCode
  sipRes = new AimsVector_%Template1typecode%_%Template2typecode%( *a1 + AimsVector_%Template1typecode%_%Template2typecode%( a0 ) );
%End

AimsVector_%Template1typecode%_%Template2typecode% operator - ( AimsVector_%Template1typecode%_%Template2typecode% &, %Template1% );
%MethodCode
  sipRes = new AimsVector_%Template1typecode%_%Template2typecode%( *a0 - AimsVector_%Template1typecode%_%Template2typecode%( a1 ) );
%End

AimsVector_%Template1typecode%_%Template2typecode% operator - ( %Template1%, AimsVector_%Template1typecode%_%Template2typecode% & );
%MethodCode
  sipRes = new AimsVector_%Template1typecode%_%Template2typecode%( -*a1 + AimsVector_%Template1typecode%_%Template2typecode%( a0 ) );
%End

AimsVector_%Template1typecode%_%Template2typecode% operator * ( double, AimsVector_%Template1typecode%_%Template2typecode% & );
%MethodCode
  sipRes = new AimsVector_%Template1typecode%_%Template2typecode%( *a1 * a0 );
%End

AimsVector_%Template1typecode%_%Template2typecode% operator * ( long, AimsVector_%Template1typecode%_%Template2typecode% & );
%MethodCode
  sipRes = new AimsVector_%Template1typecode%_%Template2typecode%( *a1 * a0 );
%End

