

%MappedType rc_ptr_%Template1typecode%
{
%TypeHeaderCode
#include <cartobase/smart/rcptr.h>
%Template1typeinclude%
%Template1sipinclude%
typedef carto::rc_ptr<%Template1% > rc_ptr_%Template1typecode%;
#include <cartobase/smart/rcptrtrick.h>
#include <cartobase/type/conversion.h>
#include <pyaims/data/data.h>
%End

%ConvertFromTypeCode

  // Convert to a Python wrapping for %Template1%.

  if (!sipCpp)
    return Py_None;

  PyObject *po = 0;
  %Template1% *obj = sipCpp->get();
  if( obj )
  {
#if 0
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
#endif

        po = sipConvertFromInstance( sipCpp->get(),
                                     sipClass_%Template1sipClass%,
                                     0 );
        // PyObject_SetAttrString( po, "_rc", sipSelf );
      // }
  }
  else
  {
    // sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "Null rc_ptr used" );
  }

  return po;
%End

%ConvertToTypeCode
  // Convert a Python wrapper for %Template1% to a rc_ptr<%Template1% >
  // on the heap.

  if( !sipCanConvertToInstance( sipPy, sipClass_%Template1sipClass%,
    SIP_NOT_NONE | SIP_NO_CONVERTORS )
    || sipCanConvertToInstance( sipPy, sipClass_%Template1sipClass%,
    SIP_NOT_NONE) )
  {
    *sipCppPtr = NULL;

    return 0;
  }

  typedef %Template1% %Template1PyType%;

  if( SUPERSUBCLASS( carto::RCObject, %Template1PyType% ) )
  {
    int state = 0;
    int err = 0;
    int *sipIsErr = &err;

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
    // *sipIsErr = 1;
    return 0;
  }

%End
};


