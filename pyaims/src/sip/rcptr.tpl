
class rc_ptr_%Template1typecode%
{
%TypeHeaderCode
#include <cartobase/smart/rcptr.h>
%Template1typeinclude%
#ifndef PYAIMS_RCPTR_%Template1typecode%_DEFINED
#define PYAIMS_RCPTR_%Template1typecode%_DEFINED
typedef carto::rc_ptr<%Template1% > rc_ptr_%Template1typecode%;
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
    int x = sipCanConvertToInstance( sipPy,
      sipClass_rc_ptr_%Template1typecode%,
      SIP_NOT_NONE | SIP_NO_CONVERTORS );
    if( x )
      return x;
    if( SUPERSUBCLASS( carto::RCObject, %Template1PyType% ) )
      return sipCanConvertToInstance( sipPy, sipClass_%Template1sipClass%,
      SIP_NOT_NONE | SIP_NO_CONVERTORS )
      || sipCanConvertToInstance( sipPy, sipClass_%Template1sipClass%,
      SIP_NOT_NONE);
    return 0;
  }

  if( sipPy == Py_None )
  {
    *sipCppPtr = 0;
    return 0;
  }

  int state = 0;

  carto::rc_ptr<%Template1% > * dat
    = (carto::rc_ptr<%Template1% > *)
      sipForceConvertToInstance( sipPy,
        sipClass_rc_ptr_%Template1typecode%,
        sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
  if( *sipIsErr && dat )
  {
    sipReleaseInstance( dat, sipClass_rc_ptr_%Template1typecode%,
                        state );
    dat = 0;
    *sipIsErr = 0;
  }
  else if( dat )
  {
    *sipCppPtr = new carto::rc_ptr< %Template1% >( *dat );
    sipReleaseInstance( dat, sipClass_rc_ptr_%Template1typecode%,
                        state );
    return sipGetState(sipTransferObj);
  }

  if( SUPERSUBCLASS( carto::RCObject, %Template1PyType% ) )
  {
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
      *sipCppPtr = new carto::rc_ptr< %Template1% >( obj );
      /*std::cout << "convert " << sipPy << " to rc_ptr_%Template1typecode%: "
        << carto::rc_ptr_trick::refCount( **sipCppPtr ) << std::endl;*/
      // if there is no rc_ptr on this object yet, make it inactive
      if( carto::rc_ptr_trick::refCount( **sipCppPtr ) == 1 )
      {
        (*sipCppPtr)->release();
        delete *sipCppPtr;
        *sipCppPtr = new carto::rc_ptr< %Template1% >( obj, true );
      }
      sipReleaseInstance( obj, sipClass_%Template1sipClass%, state );
      PyErr_Clear();
      return sipGetState(sipTransferObj);
    }
    *sipIsErr = 1;
    return 0;
  }
  else
  {
    // not inheriting RCObject
    *sipIsErr = 1;
    return 0;
  }
%End

public:
  rc_ptr_%Template1typecode%();
  rc_ptr_%Template1typecode%( %Template1PyType% * /*dont Transfer*/ );
%MethodCode
  sipCpp = new carto::rc_ptr< %Template1% >( a0 );
  // if there is no rc_ptr on this object yet, make it inactive
  if( carto::rc_ptr_trick::refCount( *sipCpp ) == 1 )
  {
    sipCpp->release();
    delete sipCpp;
    sipCpp = new carto::rc_ptr< %Template1% >( a0, true );
  }
%End

  void reset( %Template1PyType% * /*dont Transfer*/ );
  void release();
  bool isNull() const;
%MethodCode
  sipRes = !sipCpp->get();
%End

  %Template1PyType% *_get();
%MethodCode
  sipRes = sipCpp->get();
  if( sipRes )
    {
      if( !SUPERSUBCLASS( carto::RCObject, %Template1PyType% ) )
      {
        /* For objects inheriting RCObject, the counter has already been
           incremented in RCObject.
           For objects not inheriting RCObject, we set in the python
           object for RCObject a reference to self (rc_ptr) so the rc_ptr
           is never destroyed before the python RCObject is.
           TODO: in SIP >= 4.5, the python object is never destroyed before
           the C++ object, so there is a leak.
        */
        PyObject *po =
          sipConvertFromInstance( sipCpp->get(), sipClass_%Template1sipClass%, 
                                  0 );
        PyObject_SetAttrString( po, "_rc", sipSelf );
      }
    }
  else
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_TypeError, "Null rc_ptr used" );
    }
%End

  bool operator == ( const rc_ptr_%Template1typecode% & x ) const;
  bool operator != ( const rc_ptr_%Template1typecode% & x ) const;
  bool operator < ( const rc_ptr_%Template1typecode% & x ) const;
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

};

