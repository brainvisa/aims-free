%Feature AIMS_LIGHT

namespace carto
{

  class Object : rc_ptr_GenericObject
  {
%TypeHeaderCode
#include <pyaims/object/object.h>
#include <cartobase/object/pythonwriter.h>
#include <cartobase/stream/sstream.h>
%End

%TypeCode
#include <cartobase/smart/rcptrtrick.h>

#ifndef PYAIMS_RCPTR_GenericObject_DEFINED
#define PYAIMS_RCPTR_GenericObject_DEFINED
typedef carto::rc_ptr<carto::GenericObject> rc_ptr_GenericObject;
#endif
%End

  public:
    bool isNone() const;

%If(!AIMS_LIGHT)

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
      if( PyLong_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                                        ( (int) PyLong_AsLong( sipPy ) ) );
      else if( PyLong_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                                        ( PyLong_AsLong( sipPy ) ) );
      else if( PyFloat_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                                        ( PyFloat_AsDouble( sipPy ) ) );
      else if( PyUnicode_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                              ( carto:: PyString_AsStdString( sipPy ) ) );
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
        *sipCppPtr = new carto::Object( carto::Object::value( sipPy ) );
      sipIsErr = 0;
      PyErr_Clear();
      return SIP_TEMPORARY;
    }
  catch( std::exception & e )
    {
      // sipIsErr = 1;
      PyErr_SetString( PyExc_AttributeError, e.what() );
      return 0;
    }
%End

%End

  protected:
    Object();
    Object( const carto::Object & );
    Object( carto::GenericObject*, bool = false );
%MethodCode
  // std::cout << "Object( GenericObject* )\n";
  if( !a0 )
    // sipCpp = new sipcarto_Object;
    sipCpp = new carto::Object;
  else
  {
    // sipCpp = new sipcarto_Object( a0, false );
    sipCpp = new carto::Object( a0, false );
    // if there is no rc_ptr on this object yet, make it inactive
    if( carto::rc_ptr_trick::refCount( *sipCpp ) == 1 )
    {
      sipCpp->release();
      delete sipCpp;
      // sipCpp = new sipcarto_Object( a0, true );
      sipCpp = new carto::Object( a0, true );
    }
  }
%End

  };

};

