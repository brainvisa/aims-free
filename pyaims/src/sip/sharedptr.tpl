
// ---

class weak_shared_ptr_%Template1typecode%
{
%TypeHeaderCode
#include <cartobase/smart/sharedptr.h>
%Template1typeinclude%
#ifndef PYAIMS_WEAKSHAREDPTR_%Template1typecode%_DEFINED
#define PYAIMS_WEAKSHAREDPTR_%Template1typecode%_DEFINED
typedef carto::weak_shared_ptr<%Template1% >
  weak_shared_ptr_%Template1typecode%;
#endif
%End

%TypeCode
%Template1sipinclude%
#include <cartobase/smart/rcptrtrick.h>
#include <cartobase/type/conversion.h>
%End

%ConvertToTypeCode
  if (!sipIsErr)
  {
    // check type
    // return %Template1testPyType%( sipPy );
    return sipCanConvertToInstance( sipPy,
      sipClass_weak_shared_ptr_%Template1typecode%,
      SIP_NOT_NONE | SIP_NO_CONVERTORS )
      || sipCanConvertToInstance( sipPy, sipClass_%Template1sipClass%,
      SIP_NOT_NONE | SIP_NO_CONVERTORS )
      || sipCanConvertToInstance( sipPy, sipClass_%Template1sipClass%,
      SIP_NOT_NONE);
  }

  if( sipPy == Py_None )
  {
    *sipCppPtr = 0;
    return 0;
  }

  int state = 0;

  carto::weak_shared_ptr<%Template1% > * dat
    = (carto::weak_shared_ptr<%Template1% > *)
      sipConvertToInstance( sipPy,
        sipClass_weak_shared_ptr_%Template1typecode%,
        sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
  if( *sipIsErr && dat )
  {
    sipReleaseInstance( dat, sipClass_weak_shared_ptr_%Template1typecode%,
                        state );
    dat = 0;
  }
  else if( dat )
  {
    *sipCppPtr = new carto::weak_shared_ptr< %Template1% >( *dat );
    sipReleaseInstance( dat, sipClass_weak_shared_ptr_%Template1typecode%,
                        state );
    return sipGetState(sipTransferObj);
  }

  carto::rc_ptr<%Template1% > * dats
    = (carto::rc_ptr<%Template1% > *)
      sipConvertToInstance( sipPy,
        sipClass_rc_ptr_%Template1typecode%,
        sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
  if( *sipIsErr && dats )
  {
    sipReleaseInstance( dats, sipClass_rc_ptr_%Template1typecode%,
                        state );
    dats = 0;
  }
  else if( dats )
  {
    *sipCppPtr = new carto::weak_shared_ptr< %Template1% >( *dats );
    sipReleaseInstance( dats, sipClass_rc_ptr_%Template1typecode%,
                        state );
    return sipGetState(sipTransferObj);
  }

  // %Template1% %Template1deref% obj = %Template1CFromPy%( sipPy );
  %Template1% %Template1deref% obj
    = (%Template1% %Template1deref%)
      sipForceConvertToInstance( sipPy, sipClass_%Template1sipClass%,
        sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
  if( *sipIsErr && obj )
  {
    sipReleaseInstance( obj, sipClass_%Template1sipClass%, state );
    obj = 0;
  }
  if( !obj )
  {
    *sipIsErr = 0;
    obj = (%Template1% %Template1deref%)
      sipForceConvertToInstance( sipPy, sipClass_%Template1sipClass%,
        sipTransferObj, 0, &state, sipIsErr );
  }
  if( obj )
  {
    *sipCppPtr = new carto::weak_shared_ptr< %Template1% >( obj );
    // if there is no rc_ptr on this object yet, make it inactive
    if( carto::rc_ptr_trick::refCount( **sipCppPtr ) == 1 )
    {
      (*sipCppPtr)->release();
      delete *sipCppPtr;
      *sipCppPtr = new carto::weak_shared_ptr< %Template1% >( obj, true );
    }
    sipReleaseInstance( obj, sipClass_%Template1sipClass%, state );
    return sipGetState(sipTransferObj);
  }
  *sipIsErr = 1;
  return 0;
%End

public:
  weak_shared_ptr_%Template1typecode%( %Template1PyType% * /*dont Transfer*/
    = 0 );
%MethodCode
  sipCpp = new carto::weak_shared_ptr< %Template1% >( a0 );
  // if there is no rc_ptr on this object yet, make it inactive
  if( carto::rc_ptr_trick::refCount( *sipCpp ) == 1 )
  {
    sipCpp->release();
    delete sipCpp;
    sipCpp = new carto::weak_shared_ptr< %Template1% >( a0, true );
  }
%End
  weak_shared_ptr_%Template1typecode%( rc_ptr_%Template1typecode%
    & x );
  weak_shared_ptr_%Template1typecode%( weak_shared_ptr_%Template1typecode%
    & x );
  ~weak_shared_ptr_%Template1typecode%();

  void reset( %Template1PyType% * /*dont Transfer*/ );
  void release();
  bool isNull() const;
%MethodCode
  sipRes = !sipCpp->get();
%End

  %Template1PyType% * _get();
%MethodCode
  sipRes = sipCpp->get();
  if( !sipRes )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "Null rc_ptr used" );
  }
%End

  bool testDeletable();
%MethodCode
  sipRes = sipCpp->get() && (*sipCpp)->testDeletable();
%End

  bool tryDelete();
%MethodCode
  sipRes = sipCpp->get() && (*sipCpp)->tryDelete();
%End

  bool operator == ( const rc_ptr_%Template1typecode% & x ) const;
  bool operator == ( const weak_shared_ptr_%Template1typecode% & x ) const;
  bool operator == ( const weak_ptr_%Template1typecode% & x ) const;
  bool operator != ( const rc_ptr_%Template1typecode% & x ) const;
  bool operator != ( const weak_shared_ptr_%Template1typecode% & x ) const;
  bool operator != ( const weak_ptr_%Template1typecode% & x ) const;
  bool operator < ( const rc_ptr_%Template1typecode% & x ) const;
  bool operator < ( const weak_shared_ptr_%Template1typecode% & x ) const;
  bool operator < ( const weak_ptr_%Template1typecode% & x ) const;
  long __hash__() const;
%MethodCode
  sipRes = 
#if defined( _WIN64 )
  (long long int) 
#else
  (long) 
#endif
  sipCpp->get();
%End

/*
  T& operator * () const;
  T* operator -> () const;

  rc_ptr<T> & operator = ( const rc_ptr<T> & x );
  rc_ptr<T> & operator = ( const weak_shared_ptr<T> & x );
*/

};

// ---

class weak_ptr_%Template1typecode%
{
%TypeHeaderCode
#include <cartobase/smart/weakptr.h>
%Template1typeinclude%
#ifndef PYAIMS_WEAKPTR_%Template1typecode%_DEFINED
#define PYAIMS_WEAKPTR_%Template1typecode%_DEFINED
typedef carto::weak_ptr<%Template1% >
  weak_ptr_%Template1typecode%;
#endif
%End

%ConvertToTypeCode
  if (!sipIsErr)
  {
    // check type
    // return %Template1testPyType%( sipPy );
    return sipCanConvertToInstance( sipPy,
      sipClass_weak_ptr_%Template1typecode%,
      SIP_NOT_NONE | SIP_NO_CONVERTORS )
      || sipCanConvertToInstance( sipPy, sipClass_%Template1sipClass%,
      SIP_NOT_NONE | SIP_NO_CONVERTORS )
      || sipCanConvertToInstance( sipPy, sipClass_%Template1sipClass%,
      SIP_NOT_NONE);
  }

  if( sipPy == Py_None )
  {
    *sipCppPtr = 0;
    return 0;
  }

  int state = 0;

  carto::weak_ptr<%Template1% > * dat
    = (carto::weak_ptr<%Template1% > *)
      sipConvertToInstance( sipPy,
        sipClass_weak_ptr_%Template1typecode%,
        sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
  if( *sipIsErr && dat )
  {
    sipReleaseInstance( dat, sipClass_weak_ptr_%Template1typecode%,
                        state );
    dat = 0;
  }
  else if( dat )
  {
    *sipCppPtr = new carto::weak_ptr< %Template1% >( *dat );
    sipReleaseInstance( dat, sipClass_weak_ptr_%Template1typecode%,
                        state );
    return sipGetState(sipTransferObj);
  }

  // %Template1% %Template1deref% obj = %Template1CFromPy%( sipPy );
  %Template1% %Template1deref% obj
    = (%Template1% %Template1deref%)
      sipForceConvertToInstance( sipPy, sipClass_%Template1sipClass%,
        sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
  if( *sipIsErr && obj )
  {
    sipReleaseInstance( obj, sipClass_%Template1sipClass%, state );
    obj = 0;
  }
  if( !obj )
  {
    *sipIsErr = 0;
    obj = (%Template1% %Template1deref%)
      sipForceConvertToInstance( sipPy, sipClass_%Template1sipClass%,
        sipTransferObj, 0, &state, sipIsErr );
  }
  if( obj )
  {
    *sipCppPtr = new carto::weak_ptr< %Template1% >( obj );
    sipReleaseInstance( obj, sipClass_%Template1sipClass%, state );
    return sipGetState(sipTransferObj);
  }
  *sipIsErr = 1;
  return 0;
%End

public:
  weak_ptr_%Template1typecode%( %Template1PyType% * = 0 );
  ~weak_ptr_%Template1typecode%();

  void reset( %Template1PyType% * );
  %Template1PyType%* release();
  bool isNull() const;
%MethodCode
  sipRes = !sipCpp->get();
%End

  %Template1PyType% * _get();
%MethodCode
  sipRes = sipCpp->get();
  /* if( !sipRes )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "Null rc_ptr used" );
  } */
%End

  bool operator == ( const rc_ptr_%Template1typecode% & x ) const;
  bool operator == ( const weak_shared_ptr_%Template1typecode% & x ) const;
  bool operator == ( const weak_ptr_%Template1typecode% & x ) const;
  bool operator != ( const rc_ptr_%Template1typecode% & x ) const;
  bool operator != ( const weak_shared_ptr_%Template1typecode% & x ) const;
  bool operator != ( const weak_ptr_%Template1typecode% & x ) const;
  bool operator < ( const rc_ptr_%Template1typecode% & x ) const;
  bool operator < ( const weak_shared_ptr_%Template1typecode% & x ) const;
  bool operator < ( const weak_ptr_%Template1typecode% & x ) const;
  long __hash__() const;
%MethodCode
  sipRes = 
#if defined( _WIN64 )
  (long long int) 
#else
  (long) 
#endif
  sipCpp->get();
%End

/*
  T& operator * () const;
  T* operator -> () const;

  rc_ptr<T> & operator = ( const rc_ptr<T> & x );
  rc_ptr<T> & operator = ( const weak_ptr<T> & x );
*/

};

