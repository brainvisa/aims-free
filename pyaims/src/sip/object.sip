
%ModuleCode
#ifndef PYAIMS_BUCKETMAP_VOID_DEFINED
#define PYAIMS_BUCKETMAP_VOID_DEFINED
#include <aims/bucket/bucket.h>
typedef aims::BucketMap< Void > BucketMap_VOID;
#endif
%End


namespace carto
{

  carto::Object none();
%Docstring
  Returns a null aims.Object
%End

  class Object : rc_ptr_GenericObject
  {
%TypeHeaderCode
#include <pyaims/object/object.h>
#include <cartobase/object/pythonwriter.h>
#include <cartobase/stream/sstream.h>
%End

%TypeCode
#include <cartobase/smart/rcptrtrick.h>
#include <pyaims/object/numconv.h>

#ifndef PYAIMS_RCPTR_GenericObject_DEFINED
#define PYAIMS_RCPTR_GenericObject_DEFINED
typedef carto::rc_ptr<carto::GenericObject> rc_ptr_GenericObject;
#endif
%End

  public:
    bool isNone() const;

%ConvertToTypeCode
  if (!sipIsErr)
  {
    // Checking whether or not None has been passed instead of a list
    // has already been done.
    // The type is always valid.
    return 1;
  }

  try
    {
      void    *obj;
      int state = 0;
      // std::cout << "convert to Object_n";
      if( sipPy == Py_None )
        *sipCppPtr = new sipcarto_Object;
      if( PyBool_Check( sipPy ) )
      {
        if( sipPy == Py_False )
          *sipCppPtr = new sipcarto_Object( carto::Object::value( false ) );
        else
          *sipCppPtr = new sipcarto_Object( carto::Object::value( true ) );
      }
#if PY_VERSION_HEX < 0x03000000
      else if( PyInt_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                                        ( (int) PyInt_AsLong( sipPy ) ) );
#endif
      else if( PyLong_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                                        ( PyLong_AsLong( sipPy ) ) );
      else if( PyFloat_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                                        ( PyFloat_AsDouble( sipPy ) ) );
#if PY_VERSION_HEX < 0x03000000
      else if( PyString_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                              ( std::string( PyString_AsString( sipPy ) ) ) );
#endif
      else if( PyUnicode_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                            ( carto::PyString_AsStdString( sipPy ) ) );
      else if( sipCanConvertToType( sipPy, sipType_carto_Object,
               SIP_NO_CONVERTORS ) )
      {
        obj = sipConvertToType( sipPy, sipType_carto_Object,
                                    sipTransferObj,
                                    SIP_NO_CONVERTORS, &state, sipIsErr );
        carto::Object       *o = (carto::Object *) obj;
        sipReleaseType( obj, sipType_carto_Object, state );
        *sipCppPtr = new carto::Object( *o ); //sipcarto_Object( *o );
        return sipGetState( sipTransferObj );
      }
      else if( sipCanConvertToType( sipPy, sipType_carto_GenericObject,
               SIP_NO_CONVERTORS ) )
      {
        obj = sipConvertToType( sipPy, sipType_carto_GenericObject,
                                    sipTransferObj,
                                    SIP_NO_CONVERTORS, &state, sipIsErr );
        carto::GenericObject        *o = (carto::GenericObject *) obj;
        *sipCppPtr = new carto::Object( o ); //sipcarto_Object( o );
        // if there is no rc_ptr on this object yet, make it inactive
        // std::cout << "Object from GenericObject\n";
        if( carto::rc_ptr_trick::refCount( **sipCppPtr ) == 1 )
        {
          // std::cout << "inactive RCObject\n";
          (*sipCppPtr)->release();
          delete *sipCppPtr;
          *sipCppPtr = new carto::Object( o, true );
          /* std::cout << "refcount: "
            << carto::rc_ptr_trick::refCount( **sipCppPtr ) << std::endl; */
        }
        sipReleaseType( obj, sipType_carto_GenericObject, state );
        return sipGetState(sipTransferObj);
      }
      else
      {
        // attempt to re-use an existing container GenericObject if it exists
        if( PyObject_HasAttrString( sipPy, "__genericobject__" ) )
        {
          PyObject *gobj = PyObject_GetAttrString( sipPy, "__genericobject__" );
          obj = sipConvertToType( gobj, sipType_carto_GenericObject,
                                      sipTransferObj, SIP_NO_CONVERTORS, &state,
                                      sipIsErr );
          carto::GenericObject        *o = (carto::GenericObject *) obj;
          *sipCppPtr = new carto::Object( o );
          // if there is no rc_ptr on this object yet, make it inactive
          if( carto::rc_ptr_trick::refCount( **sipCppPtr ) == 1 )
          {
            (*sipCppPtr)->release();
            delete *sipCppPtr;
            *sipCppPtr = new carto::Object( o, true );
          }
          sipReleaseType( obj, sipType_carto_GenericObject, state );
          Py_DECREF( gobj );
          return sipGetState(sipTransferObj);
        }
        // attempt to use sipPy.toObject() method if it is defined
        if( PyObject_HasAttrString( sipPy, "toObject" ) )
        {
          PyObject *toobj = PyObject_GetAttrString( sipPy, "toObject" );
          PyObject *restoobj = PyObject_CallFunctionObjArgs( toobj, 0 );
          if( restoobj )
          {
            if( sipCanConvertToType( restoobj, sipType_carto_Object,
               SIP_NO_CONVERTORS ) )
            {
              obj = sipConvertToType( restoobj, sipType_carto_Object,
                                          sipTransferObj, SIP_NO_CONVERTORS,
                                          &state, sipIsErr );
              carto::Object       *o = (carto::Object *) obj;
              *sipCppPtr = new carto::Object( *o );
              sipReleaseType( obj, sipType_carto_Object, state );
            }
            Py_DECREF( restoobj );
            if( *sipCppPtr )
              return sipGetState( sipTransferObj );
          }
          Py_DECREF( toobj );
        }
        // no other solution works: embed a PyObject* into a GenericObject
        // std::cout << "create Object around PyObject\n";
        *sipCppPtr = new carto::Object( carto::Object::value( sipPy ) );
        /* std::cout << "-> " << *sipCppPtr << std::endl;
        std::cout << "type: " << (**sipCppPtr)->type() << std::endl; */
      }
      sipIsErr = 0;
      PyErr_Clear();
      return sipGetState(sipTransferObj);
    }
  catch( std::exception & e )
    {
      // sipIsErr = 1;
      SIP_BLOCK_THREADS
      PyErr_SetString( PyExc_AttributeError, e.what() );
      SIP_UNBLOCK_THREADS
      return 0;
    }
  return sipGetState(sipTransferObj);
  // return 0;
%End

  bool operator == ( const carto::Object & ) const;
  bool operator == ( SIP_PYOBJECT ) const;
%MethodCode
  sipRes = sipCpp->operator == ( carto::Object::value( a0 ) );
%End

  bool operator != ( const carto::Object & ) const;
  bool operator != ( SIP_PYOBJECT ) const;
%MethodCode
  sipRes = sipCpp->operator != ( carto::Object::value( a0 ) );
%End


  protected:
    Object() /ReleaseGIL/;
    Object( const carto::Object & ) /ReleaseGIL/;
    Object( carto::GenericObject*, bool = false ) /ReleaseGIL/;
%MethodCode
  // std::cout << "Object( GenericObject* )\n";
  if( !a0 )
    sipCpp = new carto::Object;
  else
  {
    sipCpp = new carto::Object( a0, false );
    // if there is no rc_ptr on this object yet, make it inactive
    if( carto::rc_ptr_trick::refCount( *sipCpp ) == 1 )
    {
      sipCpp->release();
      delete sipCpp;
      sipCpp = new carto::Object( a0, true );
    }
  }
%End
  };

};
