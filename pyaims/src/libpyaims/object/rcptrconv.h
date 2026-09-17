#ifndef CARTO_PYAIMS_OBJECT_RCPTRCONV_H
#define CARTO_PYAIMS_OBJECT_RCPTRCONV_H

#include <sip.h>
#include <cartobase/smart/sharedptr.h>

namespace pyaims
{

  template <typename T> inline
  T* fromT( PyObject * sipPy, sipTypeDef* tclass, PyObject *transferObj );

  template <typename T, typename RcT> inline
  T* fromRcptr( PyObject * sipPy, sipTypeDef* rcclass,
                PyObject *transferObj );

  template <typename T> inline
  T* fromRcptrs( PyObject * sipPy, PyObject* tclass, PyObject* rcclass,
                 PyObject* wsclass, PyObject* wkclass, PyObject *transferObj );

  inline PyObject* extractPyObjectFromProxy( PyObject* sipPy );
  inline int canConvertFromProxy( PyObject *sipPy, sipTypeDef* tclass );
  inline int canConvertFromProxy( PyObject *sipPy, const sipTypeDef* tclass );

  template <typename T> inline
  T* fromProxy( PyObject * sipPy, sipTypeDef* tclass,
                PyObject *transferObj, int *state, int *iserr );
  template <typename T> inline
  T* fromProxy( PyObject * sipPy, const sipTypeDef* tclass,
                PyObject *transferObj, int *state, int *iserr );

  template <typename T> inline
  int standardConvertToTypeCode( PyObject* sipPy, sipTypeDef* tclass,
                                 PyObject *sipTransferObj, int *sipIsErr,
                                 T** sipCppPtr );

  template <typename T> inline
  int standardConvertToTypeCode( PyObject* sipPy, const sipTypeDef* tclass,
                                 PyObject *sipTransferObj, int *sipIsErr,
                                 T** sipCppPtr );

  // ---

  template <typename T> inline
  T* fromT( PyObject * sipPy, sipTypeDef* tclass, PyObject *transferObj )
  {
    if( sipCanConvertToType( sipPy, tclass, SIP_NO_CONVERTORS ) )
    {
      int state = 0;
      int sipIsErr = 0;
      void *obj = sipConvertToType( sipPy, tclass, transferObj,
                                    SIP_NO_CONVERTORS, &state,
                                    &sipIsErr );
      /* *don't* call sipReleaseType here because we are not done
      with the object. But sipReleaseType should be called later on it
      (normally internally by sip).
      */
      return reinterpret_cast<T *>( obj );
    }
    return 0;
  }

  template <typename T, typename RcT> inline 
  T* fromRcptr( PyObject * sipPy, sipTypeDef* rcclass,
                PyObject *transferObj )
  {
    if( sipCanConvertToType( sipPy, rcclass, SIP_NO_CONVERTORS ) )
    {
      int state = 0;
      int sipIsErr = 0;
      void *obj = sipConvertToType( sipPy, rcclass, transferObj,
                                    SIP_NO_CONVERTORS, &state, &sipIsErr );
      RcT *robj = reinterpret_cast<RcT *>( obj );
      /* release rc_ptr instance after using sipConvertToType: allow it
      to be deleted later.
      We don't need the rc_ptr instance any longer, but its reference count
      must not go down to 0 otherwise the contained T is deleted.
      However it should not be such if we get an existing rc_ptr.
      So we *must* use SIP_NO_CONVERTORS here.
      */
      sipReleaseType( obj, rcclass, state );
      return robj->get();
    }
    return 0;
  }


  template <typename T> inline
  T* fromRcptrs( PyObject * sipPy, PyObject* tclass, PyObject* rcclass,
                 PyObject* wsclass, PyObject* wkclass, PyObject *transferObj )
  {
    return fromT<T>( sipPy, tclass, transferObj )
        || fromRcptr<T, carto::rc_ptr<T> >( sipPy, rcclass, transferObj )
        || fromRcptr<T, carto::weak_shared_ptr<T> >( sipPy, wsclass,
            transferObj )
        || fromRcptr<T, carto::weak_ptr<T> >( sipPy, wkclass, transferObj );
  }


  inline PyObject* extractPyObjectFromProxy( PyObject* sipPy )
  {
    /* WARNING the call to PyObject_HasAttrString() sometimes fails (returns 0)
       whereras the object actuallt the _get attribute. Calling it twice
       actually avoids the problem.
       I don't understand that thing, it's completely insane.
       (Denis 2024/05/14)
    */
    PyErr_Clear();
    PyObject_HasAttrString( sipPy, "_get" );
    if( !PyObject_HasAttrString( sipPy, "_get" ) )
      return 0;
    PyObject* pointee = PyObject_CallMethod( sipPy, (char *) "_get", NULL );
    if( !pointee )
    {
      PyErr_Clear();
      return 0;
    }
    return pointee;
  }


  inline int canConvertFromProxy( PyObject *sipPy, sipTypeDef* tclass )
  {
    PyObject *inside = extractPyObjectFromProxy( sipPy );
    if( !inside )
      return 0;
    int res = sipCanConvertToType( inside, tclass, SIP_NOT_NONE );
    Py_DECREF( inside );
    return res;
  }


  inline int canConvertFromProxy( PyObject *sipPy, const sipTypeDef* tclass )
  {
    PyObject *inside = extractPyObjectFromProxy( sipPy );
    if( !inside )
      return 0;
    int res = sipCanConvertToType( inside, tclass, SIP_NOT_NONE );
    Py_DECREF( inside );
    return res;
  }


  template <typename T> inline
  T* fromProxy( PyObject * sipPy, sipTypeDef* tclass,
                PyObject *transferObj, int *state, int *iserr )
  {
    *iserr = 0;
    PyObject *inside = extractPyObjectFromProxy( sipPy );
    if( !inside )
      return 0;
    int res = sipCanConvertToType( inside, tclass, SIP_NOT_NONE );
    if( !res )
    {
      Py_DECREF( inside );
      return 0;
    }
    T *sipRes = (T*) sipConvertToType( inside, tclass, transferObj,
                                       SIP_NOT_NONE, state, iserr );
    Py_DECREF( inside );
    if( *iserr )
    {
      sipReleaseType( sipRes, tclass, *state );
      sipRes = 0;
    }
    return sipRes;
  }


  template <typename T> inline
  T* fromProxy( PyObject * sipPy, const sipTypeDef* tclass,
                PyObject *transferObj, int *state, int *iserr )
  {
    *iserr = 0;
    PyObject *inside = extractPyObjectFromProxy( sipPy );
    if( !inside )
      return 0;
    int res = sipCanConvertToType( inside, tclass, SIP_NOT_NONE );
    if( !res )
    {
      Py_DECREF( inside );
      return 0;
    }
    T *sipRes = (T*) sipConvertToType( inside, tclass, transferObj,
                                       SIP_NOT_NONE, state, iserr );
    Py_DECREF( inside );
    if( *iserr )
    {
      sipReleaseType( sipRes, tclass, *state );
      sipRes = 0;
    }
    return sipRes;
  }


  template <typename T> inline
  int standardConvertToTypeCode( PyObject* sipPy, sipTypeDef* tclass,
                                 PyObject *sipTransferObj, int *sipIsErr,
                                 T** sipCppPtr )
  {
    if( !sipIsErr )
      return sipCanConvertToType( sipPy, tclass,
                                  SIP_NOT_NONE | SIP_NO_CONVERTORS )
        || pyaims::canConvertFromProxy( sipPy, tclass );

    if( sipPy == Py_None )
    {
      *sipCppPtr = 0;
      return 0;
    }

    int state = 0;

    T * dat = (T *) sipForceConvertToType( sipPy, tclass, sipTransferObj,
                                           SIP_NO_CONVERTORS, &state,
                                           sipIsErr );
    if( dat && *sipIsErr )
    {
      sipReleaseType( dat, tclass, state );
      dat = 0;
    }
    else if( dat )
    {
      *sipCppPtr = dat;
      // return 0; //sipGetState(sipTransferObj);
      // Still don't understand this state/return codes, Denis 2011/12/20
      return state;
    }

    *sipIsErr = 0;
    *sipCppPtr = fromProxy<T>( sipPy, tclass, 0, &state,
                               sipIsErr );

    if( !*sipCppPtr && !*sipIsErr )
      *sipIsErr = 1; // if conversion failed, set an error flag

    if( *sipIsErr )
    {
      PyErr_Clear();
      // return 0; //sipGetState(sipTransferObj);
      return state;
    }

    return state;
  }


  template <typename T> inline
  int standardConvertToTypeCode( PyObject* sipPy, const sipTypeDef* tclass,
                                 PyObject *sipTransferObj, int *sipIsErr,
                                 T** sipCppPtr )
  {
    if( !sipIsErr )
      return sipCanConvertToType( sipPy, tclass,
                                  SIP_NOT_NONE | SIP_NO_CONVERTORS )
        || pyaims::canConvertFromProxy( sipPy, tclass );

    if( sipPy == Py_None )
    {
      *sipCppPtr = 0;
      return 0;
    }

    int state = 0;

    T * dat = (T *) sipForceConvertToType( sipPy, tclass, sipTransferObj,
                                           SIP_NO_CONVERTORS, &state,
                                           sipIsErr );
    if( dat && *sipIsErr )
    {
      sipReleaseType( dat, tclass, state );
      dat = 0;
    }
    else if( dat )
    {
      *sipCppPtr = dat;
      // return 0; //sipGetState(sipTransferObj);
      // Still don't understand this state/return codes, Denis 2011/12/20
      return state;
    }

    *sipIsErr = 0;
    *sipCppPtr = fromProxy<T>( sipPy, tclass, sipTransferObj, &state,
                               sipIsErr );

    if( !*sipCppPtr && !*sipIsErr )
      *sipIsErr = 1; // if conversion failed, set an error flag

    if( *sipIsErr )
    {
      PyErr_Clear();
      return state;
    }
    return state;
  }

}

#endif

