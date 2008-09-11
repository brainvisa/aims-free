
%ModuleCode
#if SIP_VERSION >= 0x040700
#ifndef PYAIMS_BUCKETMAP_VOID_DEFINED
#define PYAIMS_BUCKETMAP_VOID_DEFINED
#include <aims/bucket/bucket.h>
typedef aims::BucketMap< Void > BucketMap_VOID;
#endif
#endif
%End


namespace carto
{

  class Object : rc_ptr_GenericObject
  {
%TypeHeaderCode
#include <pyaims/object/object.h>
#include <cartobase/object/pythonwriter.h>
#include <cartobase/stream/sstream.h>
#if SIP_VERSION < 0x040700
#include "sipaimssiprc_ptr_GenericObject.h"
#endif
%End

%TypeCode
#include <cartobase/smart/rcptrtrick.h>

#if SIP_VERSION >= 0x040700
#ifndef PYAIMS_RCPTR_GenericObject_DEFINED
#define PYAIMS_RCPTR_GenericObject_DEFINED
typedef carto::rc_ptr<carto::GenericObject> rc_ptr_GenericObject;
#endif
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
      if( PyInt_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                                        ( (int) PyInt_AsLong( sipPy ) ) );
      else if( PyLong_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                                        ( PyLong_AsLong( sipPy ) ) );
      else if( PyFloat_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                                        ( PyFloat_AsDouble( sipPy ) ) );
      else if( PyString_Check( sipPy ) )
        *sipCppPtr = new sipcarto_Object( carto::Object::value
                              ( std::string( PyString_AsString( sipPy ) ) ) );
      else if( sipCanConvertToInstance( sipPy, sipClass_carto_Object,
               SIP_NO_CONVERTORS ) )
      {
        obj = sipConvertToInstance( sipPy, sipClass_carto_Object,
                                    sipTransferObj,
                                    SIP_NO_CONVERTORS, &state, sipIsErr );
        carto::Object       *o = (carto::Object *) obj;
        sipReleaseInstance( obj, sipClass_carto_Object, state );
        *sipCppPtr = new carto::Object( *o ); //sipcarto_Object( *o );
        return sipGetState( sipTransferObj );
      }
      else if( sipCanConvertToInstance( sipPy, sipClass_carto_GenericObject,
               SIP_NO_CONVERTORS ) )
      {
        obj = sipConvertToInstance( sipPy, sipClass_carto_GenericObject,
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
        sipReleaseInstance( obj, sipClass_carto_GenericObject, state );
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


  protected:
    Object();
    Object( const carto::Object & );
    Object( carto::GenericObject*, bool = false );
%MethodCode
  // std::cout << "Object( GenericObject* )\n";
  if( !a0 )
    sipCpp = new sipcarto_Object;
  else
  {
    sipCpp = new sipcarto_Object( a0, false );
    // if there is no rc_ptr on this object yet, make it inactive
    if( carto::rc_ptr_trick::refCount( *sipCpp ) == 1 )
    {
      sipCpp->release();
      delete sipCpp;
      sipCpp = new sipcarto_Object( a0, true );
    }
  }
%End
  };



  // --------


  class GenericObject : carto::RCObject
  {
%TypeHeaderCode
#include <pyaims/object/object.h>
#include <cartobase/object/pythonwriter.h>
#include <cartobase/stream/sstream.h>
#include <graph/graph/graph.h>
#include <graph/tree/tree.h>
#include <aims/roi/hie.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipcartoGenericObject.h"
#include "sipaimssipGraph.h"
#include "sipaimssipTree.h"
#include "sipaimssipaimsHierarchy.h"
#include "sipaimssipGraphObject.h"
#include "sipaimssipBucketMap_VOID.h"
#endif
#include <stdio.h>
%End

%ConvertToSubClassCode
  if (dynamic_cast<carto::GenericObject *>(sipCpp))
	sipClass = sipClass_carto_GenericObject;
  else if (dynamic_cast<carto::RCObject *>(sipCpp))
	sipClass = sipClass_carto_RCObject;
  else  sipClass = NULL;
%End

  public:
    virtual std::string type() const = 0;

    // Size API
    virtual size_t size() const = 0;
    int __len__() const;
%MethodCode
    carto::Object i = sipCpp->objectIterator();
    int	c = 0;
    for( ; i->isValid(); i->next(), ++c );
    sipRes = c;
%End

    // Scalar API
    virtual double getScalar() const = 0;

    // String API
    virtual std::string getString() const = 0;


    // Array API

    virtual bool isArray() const = 0;
    virtual bool isDynArray() const = 0;

    carto::Object __getitem__( int ) /Factory/;
%MethodCode
    try
      {
        carto::Object o = sipCpp->getArrayItem( a0 );
        sipRes = new carto::Object( o );
      }
    catch( ... )
      {
        sipIsErr = 1;
        sipRes = 0;
        std::ostringstream x;
        x << a0;
        PyErr_SetString( PyExc_KeyError, x.str().c_str() );
      }
%End

    // Dictionary API

    virtual bool isDictionary() const = 0;

    SIP_PYTUPLE keys() const /AutoGen/;
%MethodCode
    carto::Object i;
    unsigned c = 0;
    try
      {
        // I can't rely on size() which is sometimes wrongly overriden
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c );
        PyObject	*po = PyTuple_New( c );
        c = 0;
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c )
          PyTuple_SetItem( po, c, PyString_FromString( i->key().c_str() ) );
        sipRes = po;
      }
    catch( std::exception & e )
      {
        PyErr_SetString( PyExc_RuntimeError, 
                         ( std::string( "error while iterating dictionary: " ) 
                           + e.what() ).c_str() );
        sipIsErr = 1;
      }
%End


    SIP_PYTUPLE items() const /AutoGen/;
%MethodCode
    carto::Object i;
    unsigned c = 0;
    try
    {
        // I can't rely on size() which is sometimes wrongly overriden
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c );
        PyObject	*po = PyTuple_New( c ), *pi;
        c = 0;
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c )
        {
          pi = PyTuple_New( 2 );
          PyTuple_SetItem( pi, 0, PyString_FromString( i->key().c_str() ) );
          PyTuple_SetItem( pi, 1,
            sipConvertFromInstance( new carto::Object( i->currentValue() ),
              sipClass_carto_Object, Py_None ) );
          PyTuple_SetItem( po, c, pi );
        }
        sipRes = po;
    }
    catch( std::exception & e )
    {
        PyErr_SetString( PyExc_RuntimeError, 
                         ( std::string( "error while iterating dictionary: " ) 
                           + e.what() ).c_str() );
        sipIsErr = 1;
    }
%End


    SIP_PYTUPLE values() const /AutoGen/;
%MethodCode
    carto::Object i;
    unsigned c = 0;
    try
    {
        // I can't rely on size() which is sometimes wrongly overriden
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c );
        PyObject	*po = PyTuple_New( c );
        c = 0;
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c )
          PyTuple_SetItem( po, c,
            sipConvertFromInstance( new carto::Object( i->currentValue() ),
              sipClass_carto_Object, Py_None ) );
        sipRes = po;
    }
    catch( std::exception & e )
    {
        PyErr_SetString( PyExc_RuntimeError, 
                         ( std::string( "error while iterating dictionary: " ) 
                           + e.what() ).c_str() );
        sipIsErr = 1;
    }
%End


    carto::Object __getitem__( const std::string & ) /Factory, AutoGen/;
%MethodCode
    try
      {
        carto::Object o = sipCpp->getProperty( *a0 );
        sipRes = new carto::Object( o );
      }
    catch( ... )
      {
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_KeyError, a0->c_str() );
      }
%End


    void __setitem__( int, const SIP_PYOBJECT ) /AutoGen/;
%MethodCode
    // std::cout << "setitem(int)\n";
    try
      {
        void	*obj;
        if( PyInt_Check( a1 ) )
          {
            sipCpp->setArrayItem( a0,
                                  carto::Object::value
                                   ( (int) PyInt_AsLong( a1 ) ) );
          }
        else if( PyLong_Check( a1 ) )
          {
            sipCpp->setArrayItem( a0,
                                 carto::Object::value( PyLong_AsLong( a1 ) ) );
          }
        else if( PyFloat_Check( a1 ) )
          {
            sipCpp->setArrayItem( a0, 
                                 carto::Object::value
                                 ( PyFloat_AsDouble( a1 ) ) );
          }
        else if( PyString_Check( a1 ) )
          {
            sipCpp->setArrayItem
              ( a0,
                carto::Object::value
                ( std::string( PyString_AsString( a1 ) ) ) );
          }
        else if( ( obj = sipForceConvertTo_carto_Object( a1, &sipIsErr ) ) )
          {
            carto::Object	*o = (carto::Object *) obj;
            sipCpp->setArrayItem( a0, *o );
          }
        else if( ( obj = sipForceConvertTo_carto_GenericObject( a1, &sipIsErr ) ) )
          {
            carto::GenericObject	*o = (carto::GenericObject *) obj;
            sipCpp->setArrayItem( a0, carto::Object( o ) );
          }
        else
          {
            sipCpp->setArrayItem( a0, carto::Object::value( a1 ) );
          }
        sipIsErr = 0;
        PyErr_Clear();
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        PyErr_SetString( PyExc_AttributeError, e.what() );
      }
%End

    void __setitem__( const std::string &, const SIP_PYOBJECT ) /AutoGen/;
%MethodCode
    // std::cout << "setitem\n";
    try
      {
        void    *obj;
        if( PyInt_Check( a1 ) )
          {
            sipCpp->setProperty( *a0, 
                                 carto::Object::value
                                   ( (int) PyInt_AsLong( a1 ) ) );
          }
        else if( PyLong_Check( a1 ) )
          {
            sipCpp->setProperty( *a0, 
                                 carto::Object::value( PyLong_AsLong( a1 ) ) );
          }
        else if( PyFloat_Check( a1 ) )
          {
            sipCpp->setProperty( *a0, 
                                 carto::Object::value
                                 ( PyFloat_AsDouble( a1 ) ) );
          }
        else if( PyString_Check( a1 ) )
          {
            sipCpp->setProperty
              ( *a0, 
                carto::Object::value
                ( std::string( PyString_AsString( a1 ) ) ) );
          }
        else if( ( obj = sipForceConvertTo_carto_Object( a1, &sipIsErr ) ) )
          {
            carto::Object       *o = (carto::Object *) obj;
            sipCpp->setProperty( *a0, *o );
          }
        else if( ( obj = sipForceConvertTo_carto_GenericObject( a1, &sipIsErr ) ) )
          {
            carto::GenericObject        *o = (carto::GenericObject *) obj;
            sipCpp->setProperty( *a0, carto::Object( o ) );
          }
        else
          {
            sipCpp->setProperty( *a0, carto::Object::value( a1 ) );
          }
        sipIsErr = 0;
        PyErr_Clear();
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        PyErr_SetString( PyExc_AttributeError, e.what() );
      }
%End


    void __delitem__( const std::string & ) /AutoGen/;
%MethodCode
    sipCpp->removeProperty( *a0 );
%End


    // printing

    SIP_PYOBJECT __str__() /AutoGen/;
%MethodCode
    try
      {
        carto::PythonWriter	pw;
        std::ostringstream	ss;
        pw.attach( ss );
        pw.setSingleLineMode( true );
        pw.setCatchFunction( &carto::_objectStrCatchFunction );
        pw.write( *sipCpp, false, false );
        std::string	s = ss.str();
        const char	*s2 = s.c_str();

        if( s2 == 0 )
          s2 = "";
 
        sipRes = PyString_FromString(s2);
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_RuntimeError, e.what() );
      }
%End

    // Syntaxed API

    virtual bool hasSyntax() const /AutoGen/;
%MethodCode
    carto::SyntaxedInterface 
      *si = sipCpp->getInterface<carto::SyntaxedInterface>();
    if( si && si->hasSyntax() )
      sipRes = true;
    else
      sipRes = false;
%End

    virtual std::string getSyntax() const /Factory, AutoGen/;
%MethodCode
    try
      {
        carto::SyntaxedInterface 
          *si = sipCpp->getInterface<carto::SyntaxedInterface>();
        if( si && si->hasSyntax() )
          sipRes = new std::string( si->getSyntax() );
        else
          {
            sipRes = 0;
            sipIsErr = 1;
            PyErr_SetString( PyExc_RuntimeError, "object has no syntax" );
          }
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_RuntimeError, e.what() );
      }
%End

    void setSyntax( const std::string & ) /AutoGen/;
%MethodCode
    try
      {
        carto::SyntaxedInterface 
          *si = sipCpp->getInterface<carto::SyntaxedInterface>();
        if( si && si->hasSyntax() )
          si->setSyntax( *a0 );
        else
          {
            sipIsErr = 1;
            PyErr_SetString( PyExc_RuntimeError, "object has no syntax" );
          }
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        PyErr_SetString( PyExc_RuntimeError, e.what() );
      }
%End

    // iterable API

    virtual bool isIterable() const = 0;
    /*virtual*/ carto::Object objectIterator() const = 0;

    // __iter__ isn't called by python iter() function when defined in SIP
    carto::Object __objiter__() const /Factory, AutoGen/;
%MethodCode
    //std::cout << "__iter__\n";
    if( sipCpp->isIterator() )
      sipRes = new carto::Object( sipCpp );
    else
      sipRes = new carto::Object( sipCpp->objectIterator() );
%End

    int __contains__( const std::string & ) const /AutoGen/;
%MethodCode
    carto::Object i = sipCpp->objectIterator();
    if( i->isValid() && i->isDictionaryIterator() )
      {
        for( ; i->isValid() && i->key()!= *a0; i->next() );
        sipRes = i->isValid();
      }
    else
      {
        for( ; i->isValid(); i->next() )
          try
            {
              if( i->currentValue()->getString() == *a0 )
                break;
            }
          catch( std::exception & )
            {
            }
        sipRes = i->isValid();
      }
%End

    int __contains__( carto::Object ) const /AutoGen/;
%MethodCode
    carto::Object i = sipCpp->objectIterator();
    if( i->isValid() && i->isDictionaryIterator() )
      {
        std::string k = (*a0)->getString();
        for( ; i->isValid() && i->key()!= k; i->next() );
        sipRes = i->isValid();
      }
    else
      {
        for( ; i->isValid() && i->currentValue()!= *a0; i->next() );
        sipRes = i->isValid();
      }
%End

    virtual bool hasProperty( const std::string & ) const = 0 /PyName=has_key/;

    // iterator API
    virtual bool isIterator() const = 0;
    virtual bool isDictionaryIterator() const = 0;
    virtual bool isValid() const = 0;
    /*virtual*/ carto::Object currentValue() const = 0 /Factory/;

    SIP_PYOBJECT next() /AutoGen/;
%MethodCode
    //std::cout << "it next\n";
    if( !sipCpp->isIterator() )
      {
        sipRes = 0;
        sipIsErr = 1;
        PyErr_SetString( PyExc_TypeError, "not an iterator" );
      }
    else if( !sipCpp->isValid() )
      {
        sipRes = 0;
        sipIsErr = 1;
        PyErr_SetString( PyExc_StopIteration, "iterator outside bounds" );
      }
    else
      {
        
        if( sipCpp->isDictionaryIterator() )
          sipRes = PyString_FromString( sipCpp->key().c_str() );
        else
#if SIP_VERSION >= 0x040400
          sipRes = sipConvertFromInstance
            ( new carto::Object( sipCpp->currentValue() ), 
              sipClass_carto_Object, 0 );
#else
          sipRes = sipMapCppToSelfSubClass
            ( new carto::Object( sipCpp->currentValue() ), 
              sipClass_carto_Object );
#endif
        sipCpp->next();
      }
%End

    virtual std::string key() const = 0 /Factory/;

  protected:
    GenericObject();

  private:
    GenericObject( const carto::GenericObject & );
    virtual carto::GenericObject* clone() const = 0;
  };

};

// --------

