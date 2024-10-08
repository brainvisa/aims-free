
%ModuleCode
#ifndef PYAIMS_BUCKETMAP_VOID_DEFINED
#define PYAIMS_BUCKETMAP_VOID_DEFINED
#include <aims/bucket/bucket.h>
typedef aims::BucketMap< Void > BucketMap_VOID;
#endif
%End


namespace carto
{

  class GenericObject : carto::RCObject
  {
%TypeHeaderCode
#include <pyaims/object/object.h>
#include <cartobase/object/pythonwriter.h>
#include <cartobase/stream/sstream.h>
#include <graph/graph/graph.h>
#include <graph/tree/tree.h>
#include <aims/roi/hie.h>
#include <stdio.h>
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
#include <pyaims/object/numconv.h>
%End

%ConvertToTypeCode
  if( !sipIsErr )
  {
    return ( sipPy == Py_None )
      || sipCanConvertToType( sipPy, sipType_carto_GenericObject,
           SIP_NO_CONVERTORS )
      || sipCanConvertToType( sipPy, sipType_carto_Object, 0 );
  }

  // std::cout << "ConvertToTypeCode GenericObject\n";
  *sipCppPtr = pyaims::fromT<carto::GenericObject>( sipPy,
    sipType_carto_GenericObject, sipTransferObj );
  /*
  if( *sipCppPtr ) std::cout << "is GenericObject: " << *sipCppPtr << ", "
    << (*sipCppPtr)->type() << ", rc: "
    << carto::rc_ptr_trick::refCount( **sipCppPtr )
    << ", rcobj: " << static_cast<carto::RCObject *>( *sipCppPtr )
    << std::endl;
  else std::cout << "not a GenericObject\n";
  */
  if( !*sipCppPtr )
  {
    // std::cout << "fromRcptr...\n";
    *sipCppPtr = pyaims::fromRcptr<carto::GenericObject, carto::Object>
      ( sipPy, sipType_carto_Object, 0 /*sipTransferObj*/ );
    /*
    std::cout << "rcptr? : " << *sipCppPtr << std::endl;
    if( *sipCppPtr ) std::cout << "is Object: " << *sipCppPtr << ", "
      << (*sipCppPtr)->type() << std::endl;
    else std::cout << "not a Rcptr\n";
    */

    if( *sipCppPtr )
      return 0; // not a temporary: it's an existing GenericObject
    else
    {
      // then create an appropriate GenericObject around a python instance
      *sipCppPtr = new carto::ValueObject<PyObject *>( sipPy );
      // this time it may be a temporary object and may be deleted after use.
      return sipGetState( sipTransferObj );
    }
  }
  if( !*sipCppPtr )
  {
    std::cout << "error\n";
    *sipIsErr = 1;
  }
  return 0; // NOT a temporary: build from an existing instance.
  // I don't get everything from this return code, but returning anything
  // else results in a segfault sooner or later.

%End

  public:
    virtual std::string type() const = 0;

    // Size API
    virtual size_t size() const = 0;
    Py_ssize_t __len__() const;
%MethodCode
    try
    {
      carto::Object i = sipCpp->objectIterator();
      int	c = 0;
      for( ; i->isValid(); i->next(), ++c ) {}
      sipRes = c;
    }
    catch( std::exception & e )
    {
      SIP_BLOCK_THREADS
      PyErr_SetString( PyExc_TypeError, "object has no size" );
      SIP_UNBLOCK_THREADS
    }
%End

  Aims_hash_t __hash__() const;
%MethodCode
  sipRes = static_cast<long>( reinterpret_cast<unsigned long long>(sipCpp) );
%End

    // Scalar API
    virtual bool isScalar() const = 0;
    virtual double getScalar() const = 0;

    // String API
    virtual bool isString() const = 0;
    virtual std::string getString() const = 0;

    // None API
    virtual bool isNone() const = 0;

    // Array API

    virtual bool isArray() const = 0;
    virtual bool isDynArray() const = 0;

    carto::Object __getitem__( int ) /Factory, ReleaseGIL/;
%MethodCode
    try
      {
        if( sipCpp->isArray() )
        {
          // check for int key iterator
          carto::Object i = sipCpp->objectIterator();
          if( i.get() && i->isValid() && i->isIntKeyIterator() )
          {
            // int key dict: size does not matter...
          }
          else
          {
            long n = sipCpp->size();
            if( a0 < 0 )
              a0 += n;
            if( a0 < 0 || a0 >= n )
              throw std::runtime_error( "key error: out of range" );
          }
        }
        carto::Object o = sipCpp->getArrayItem( a0 );
        if( !o.get() )
        {
          sipIsErr = 1;
          sipRes = 0;
          std::ostringstream x;
          x << a0;
          SIP_BLOCK_THREADS
          PyErr_SetString( PyExc_KeyError, x.str().c_str() );
          SIP_UNBLOCK_THREADS
        }
        else
          sipRes = new carto::Object( o );
      }
    catch( ... )
      {
        sipIsErr = 1;
        sipRes = 0;
        std::ostringstream x;
        x << a0;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_KeyError, x.str().c_str() );
        SIP_UNBLOCK_THREADS
      }
%End

    int index( carto::Object ) const;
%MethodCode
    sipIsErr = 1;
    try
    {
      if( sipCpp->isArray() )
      {
        // check for iterator
        carto::Object i = sipCpp->objectIterator();
        int n = 0;
        if( i.get() && i->isValid() )
        {
          for( ; i->isValid(); i->next(), ++n )
          {
            carto::Object item = i->currentValue();
            if( item == *a0 )
            {
              sipIsErr = 0;
              sipRes = n;
              break;
            }
          }
        }
      }
    }
    catch( ... )
    {
    }

    if( sipIsErr )
    {
      sipRes = -1;
      SIP_BLOCK_THREADS
      PyErr_SetString( PyExc_ValueError, "item is not in list" );
      SIP_UNBLOCK_THREADS
    }
%End

    // Dictionary API

    virtual bool isDictionary() const = 0;

    SIP_PYTUPLE keys() const;
%MethodCode
    carto::Object i;
    unsigned c = 0;
    try
      {
        // I can't rely on size() which is sometimes wrongly overriden
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c ) {}
        PyObject	*po = PyTuple_New( c );
        c = 0;
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c )
        {
          try
          {
            PyTuple_SetItem( po, c,
                             carto::PyString_FromStdString( i->key() ) );
          }
          catch( std::runtime_error & )
          {
            try
            {
              PyTuple_SetItem( po, c, PyLong_FromLong( i->intKey() ) );
            }
            catch( std::runtime_error & )
            {
              carto::Object key = i->keyObject();
              PyObject *obj = key->value<PyObject *>();
              Py_INCREF( obj );
              PyTuple_SetItem( po, c, obj );
            }
          }
        }
        sipRes = po;
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        sipRes = 0;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_RuntimeError,
                         ( std::string( "error while iterating dictionary: " )
                           + e.what() ).c_str() );
        SIP_UNBLOCK_THREADS
      }
%End


    SIP_PYTUPLE items() const;
%MethodCode
    carto::Object i;
    unsigned c = 0;
    try
    {
        // I can't rely on size() which is sometimes wrongly overriden
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c ) {}
        PyObject	*po = PyTuple_New( c ), *pi;
        c = 0;
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c )
        {
          pi = PyTuple_New( 2 );
          try
          {
            PyTuple_SetItem( pi, 0,
                             carto::PyString_FromStdString( i->key() ) );
          }
          catch( std::runtime_error & )
          {
            PyTuple_SetItem( pi, 0, PyLong_FromLong( i->intKey() ) );
          }
          PyObject* pitem = sipConvertFromType(
            new carto::Object( i->currentValue() ),
            sipType_carto_Object, Py_None );
          if( PyObject_HasAttrString( pitem, "getPython" ) )
          {
            // pitem is a wrapper to Object, but may be able to be converted
            // into a lower-lever python type
            PyObject *pitem2 = PyObject_CallMethod( pitem, "getPython", NULL );
            if( pitem2 )
            {
              Py_DECREF( pitem );
              pitem = pitem2;
            }
            // if getPython raises an exception, ignore it and stick with pitem
            PyErr_Clear();
          }
          PyTuple_SetItem( pi, 1, pitem );
          PyTuple_SetItem( po, c, pi );
        }
        sipRes = po;
    }
    catch( std::exception & e )
    {
        sipIsErr = 1;
        sipRes = 0;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_RuntimeError, 
                         ( std::string( "error while iterating dictionary: " ) 
                           + e.what() ).c_str() );
        SIP_UNBLOCK_THREADS
    }
%End


    SIP_PYTUPLE values() const;
%MethodCode
    carto::Object i;
    unsigned c = 0;
    try
    {
        // I can't rely on size() which is sometimes wrongly overriden
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c ) {}
        PyObject	*po = PyTuple_New( c );
        c = 0;
        for( i=sipCpp->objectIterator(); i->isValid(); i->next(), ++c )
        {
          PyObject* pitem = sipConvertFromType(
            new carto::Object( i->currentValue() ),
            sipType_carto_Object, Py_None );
          if( PyObject_HasAttrString( pitem, "getPython" ) )
          {
            // pitem is a wrapper to Object, but may be able to be converted
            // into a lower-lever python type
            PyObject *pitem2 = PyObject_CallMethod( pitem, "getPython", NULL );
            if( pitem2 )
            {
              Py_DECREF( pitem );
              pitem = pitem2;
            }
          }
          PyTuple_SetItem( po, c, pitem );
        }
        sipRes = po;
    }
    catch( std::exception & e )
    {
        sipRes = 0;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_RuntimeError,
                         ( std::string( "error while iterating dictionary: " ) 
                           + e.what() ).c_str() );
        SIP_UNBLOCK_THREADS
        sipIsErr = 1;
    }
%End


    carto::Object __getitem__( const std::string & )
      /Factory, ReleaseGIL/;
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
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_KeyError, a0->c_str() );
        SIP_UNBLOCK_THREADS
      }
%End


    SIP_PYOBJECT get( const std::string &, SIP_PYOBJECT = Py_None );
%MethodCode
    try
    {
      carto::Object o = sipCpp->getProperty( *a0 );
      sipRes = sipConvertFromType( new carto::Object( o ),
                                   sipType_carto_Object, Py_None );
    }
    catch( ... )
    {
      sipRes = a1;
      Py_INCREF( sipRes );
    }
%End


    SIP_PYOBJECT get( int, SIP_PYOBJECT = Py_None );
%MethodCode
    try
    {
      carto::Object o = sipCpp->getArrayItem( a0 );
      sipRes = sipConvertFromType( new carto::Object( o ),
                                   sipType_carto_Object, Py_None );
    }
    catch( ... )
    {
      sipRes = a1;
      Py_INCREF( sipRes );
    }
%End


    void __setitem__( int, const SIP_PYOBJECT );
%MethodCode
    // std::cout << "setitem(int)\n";
    int state;
    try
    {
        void	*obj;
        if( PyLong_Check( a1 ) )
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
        else if( carto::PyStdString_Check( a1 ) )
          {
            sipCpp->setArrayItem
              ( a0,
                carto::Object::value
                ( carto::PyString_AsStdString( a1 ) ) );
          }
        else if( ( obj = sipForceConvertToType( a1, sipType_carto_Object,
          0, 0, &state, &sipIsErr ) ) )
          {
            carto::Object       *o = (carto::Object *) obj;
            sipCpp->setArrayItem( a0, *o );
            sipReleaseType( obj, sipType_carto_Object, state );
          }
        else if( ( obj = sipForceConvertToType( a1,
          sipType_carto_GenericObject, 0, 0, &state, &sipIsErr ) ) )
          {
            carto::GenericObject        *o = (carto::GenericObject *) obj;
            sipCpp->setArrayItem( a0, carto::Object( o ) );
            sipReleaseType( obj, sipType_carto_GenericObject, state );
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
      /* try casts with conversions to Object

         Indeed, the latest case, using Object::value() will build a
         GenericObject<PyObject>, which will not be understood by every
         undelying data dype for their setArrayItem() method.
         Converters to Object, however, will use the original type (inside
         the Pyobject of a0) to convert to Object.
      */

      try
      {
        void *obj;

        if( sipCanConvertToType( a1, sipType_carto_Object, 0 ) )
        {
          obj = sipForceConvertToType( a1, sipType_carto_Object,
            0, 0, &state, &sipIsErr );
          carto::Object	*o = (carto::Object *) obj;
          sipCpp->setArrayItem( a0, *o );
          sipReleaseType( obj, sipType_carto_Object, state );
        }
        else if( sipCanConvertToType( a1, sipType_carto_GenericObject,
                 0 ) )
        {
          obj = sipForceConvertToType( a1,
            sipType_carto_GenericObject, 0, 0, &state, &sipIsErr );
          carto::GenericObject	*o = (carto::GenericObject *) obj;
          sipCpp->setArrayItem( a0, carto::Object( o ) );
          sipReleaseType( obj, sipType_carto_GenericObject, state );
        }
      }
      catch( std::exception & e )
      {
        sipIsErr = 1;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_AttributeError, e.what() );
        SIP_UNBLOCK_THREADS
      }
    }
%End

    void __setitem__( const std::string &, const SIP_PYOBJECT );
%MethodCode
    // std::cout << "setitem\n";
    int state;
    try
    {
        void    *obj;
        if( PyLong_Check( a1 ) )
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
        else if( carto::PyStdString_Check( a1 ) )
          {
            sipCpp->setProperty
              ( *a0, 
                carto::Object::value( carto::PyString_AsStdString( a1 ) ) );
          }
        else if( ( obj = sipForceConvertToType( a1, sipType_carto_Object,
          0, 0, &state, &sipIsErr ) ) )
          {
            carto::Object       *o = (carto::Object *) obj;
            sipCpp->setProperty( *a0, *o );
            sipReleaseType( obj, sipType_carto_Object, state );
          }
        else if( ( obj = sipForceConvertToType( a1,
          sipType_carto_GenericObject, 0, 0, &state, &sipIsErr ) ) )
          {
            carto::GenericObject        *o = (carto::GenericObject *) obj;
            sipCpp->setProperty( *a0, carto::Object( o ) );
            sipReleaseType( obj, sipType_carto_GenericObject, state );
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
      /* try casts with conversions to Object

         Indeed, the latest case, using Object::value() will build a
         GenericObject<PyObject>, which will not be understood by every
         undelying data dype for their setArrayItem() method.
         Converters to Object, however, will use the original type (inside
         the Pyobject of a0) to convert to Object.
      */

      try
      {
        void *obj;

        if( sipCanConvertToType( a1, sipType_carto_Object, 0 ) )
        {
          obj = sipForceConvertToType( a1, sipType_carto_Object,
            0, 0, &state, &sipIsErr );
          carto::Object	*o = (carto::Object *) obj;
          sipCpp->setProperty( *a0, *o );
          sipReleaseType( obj, sipType_carto_Object, state );
        }
        else if( sipCanConvertToType( a1, sipType_carto_GenericObject,
                 0 ) )
        {
          obj = sipForceConvertToType( a1,
            sipType_carto_GenericObject, 0, 0, &state, &sipIsErr );
          carto::GenericObject	*o = (carto::GenericObject *) obj;
          sipCpp->setProperty( *a0, carto::Object( o ) );
          sipReleaseType( obj, sipType_carto_GenericObject, state );
        }
      }
      catch( std::exception & e )
      {
        sipIsErr = 1;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_AttributeError, e.what() );
        SIP_UNBLOCK_THREADS
      }
    }
%End


    void __delitem__( const std::string & );
%MethodCode
    try
    {
      sipCpp->removeProperty( *a0 );
    }
  catch( std::exception & e )
    {
      // sipIsErr = 1;
      SIP_BLOCK_THREADS
      PyErr_SetString( PyExc_AttributeError, e.what() );
      SIP_UNBLOCK_THREADS
      return 0;
    }
%End

    void __delitem__( int );
%MethodCode
    try
    {
      sipCpp->removeArrayItem( a0 );
    }
  catch( std::exception & e )
    {
      // sipIsErr = 1;
      SIP_BLOCK_THREADS
      PyErr_SetString( PyExc_AttributeError, e.what() );
      SIP_UNBLOCK_THREADS
      return 0;
    }
%End

    void append( const carto::Object );
%MethodCode
    try
    {
      sipCpp->insertArrayItem( sipCpp->size(), *a0 );
    }
  catch( std::exception & e )
    {
      // sipIsErr = 1;
      SIP_BLOCK_THREADS
      PyErr_SetString( PyExc_AttributeError, e.what() );
      SIP_UNBLOCK_THREADS
      return 0;
    }
%End

    virtual bool operator == ( const carto::GenericObject & ) const;


    // printing

    SIP_PYOBJECT __str__() /ReleaseGIL/;
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

        SIP_BLOCK_THREADS
        sipRes = carto::PyString_FromStdString(s);
        SIP_UNBLOCK_THREADS
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        sipRes = 0;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_RuntimeError, e.what() );
        SIP_UNBLOCK_THREADS
      }
%End

    // Syntaxed API

    bool hasSyntax() const;
%MethodCode
    const carto::SyntaxedInterface 
      *si = sipCpp->getInterface<carto::SyntaxedInterface>();
    if( si && si->hasSyntax() )
      sipRes = true;
    else
      sipRes = false;
%End

    std::string getSyntax() const;
%MethodCode
    try
      {
        const carto::SyntaxedInterface
          *si = sipCpp->getInterface<carto::SyntaxedInterface>();
        if( si && si->hasSyntax() )
          sipRes = new std::string( si->getSyntax() );
        else
          {
            sipRes = 0;
            sipIsErr = 1;
            SIP_BLOCK_THREADS
            PyErr_SetString( PyExc_RuntimeError, "object has no syntax" );
            SIP_UNBLOCK_THREADS
          }
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        sipRes = 0;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_RuntimeError, e.what() );
        SIP_UNBLOCK_THREADS
      }
%End

    void setSyntax( const std::string & );
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
            SIP_BLOCK_THREADS
            PyErr_SetString( PyExc_RuntimeError, "object has no syntax" );
            SIP_UNBLOCK_THREADS
          }
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_RuntimeError, e.what() );
        SIP_UNBLOCK_THREADS
      }
%End

    // iterable API

    virtual bool isIterable() const = 0;
    /*virtual*/ carto::Object objectIterator() const = 0;

    // __iter__ isn't called by python iter() function when defined in SIP
    carto::Object __objiter__() /Factory/;
%MethodCode
    // std::cout << "__iter__ " << sipCpp->type() << std::endl;
    try
    {
        if( sipCpp->isIterator() )
              sipRes = new carto::Object( sipCpp );
        else
              sipRes = new carto::Object( sipCpp->objectIterator() );
    }
    catch( std::exception & e )
    {
      sipIsErr = 1;
      sipRes = 0;
      SIP_BLOCK_THREADS
      PyErr_SetString( PyExc_TypeError, e.what() );
      SIP_UNBLOCK_THREADS
    }
%End

    int __contains__( const std::string & ) const /ReleaseGIL/;
%MethodCode
    carto::Object i = sipCpp->objectIterator();
    if( i->isValid() && i->isDictionaryIterator() )
      {
        for( ; i->isValid(); i->next() )
        {
          try
          {
            if( i->key() == *a0 )
              break;
          }
          catch( std::runtime_error & )
          {
          }
        }
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

    int __contains__( carto::Object ) const /ReleaseGIL/;
%MethodCode
    carto::Object i = sipCpp->objectIterator();
    if( i->isValid() && i->isDictionaryIterator() )
      {
        sipRes = false;
        try
        {
          std::string k = (*a0)->getString();
          for( ; i->isValid(); i->next() )
          {
            try
            {
              if( i->key() == k )
                break;
            }
            catch( std::runtime_error & )
            {
            }
          }
          sipRes = i->isValid();
        }
        catch( ... )
        {
          try
          {
            long k = long( rint( (*a0)->getScalar() ) );
            for( ; i->isValid(); i->next() )
            {
              try
              {
                if( i->intKey() == k )
                  break;
              }
              catch( std::runtime_error & )
              {
              }
            }
            sipRes = i->isValid();
          }
          catch( std::runtime_error & )
          {
          }
        }
      }
    else
      {
        for( ; i->isValid() && i->currentValue()!= *a0; i->next() ) {}
        sipRes = i->isValid();
      }
%End

    virtual bool hasProperty( const std::string & ) const = 0
      /PyName=has_key, ReleaseGIL/;

    // iterator API
    virtual bool isIterator() const = 0;
    virtual bool isDictionaryIterator() const = 0;
    virtual bool isIntKeyIterator() const = 0;
    virtual bool isKeyIterator() const = 0;
    virtual bool isValid() const = 0;
    /*virtual*/ carto::Object currentValue() const = 0;

    SIP_PYOBJECT __objnext__();
%MethodCode
    //std::cout << "it next\n";
    if( !sipCpp->isIterator() )
      {
        sipIsErr = 1;
        sipRes = 0;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_TypeError, "not an iterator" );
        SIP_UNBLOCK_THREADS
      }
    else if( !sipCpp->isValid() )
      {
        sipRes = 0;
        sipIsErr = 1;
        SIP_BLOCK_THREADS
        PyErr_SetString( PyExc_StopIteration, "iterator outside bounds" );
        SIP_UNBLOCK_THREADS
      }
    else
      {
        if( sipCpp->isDictionaryIterator() )
        {
          try
          {
            sipRes = carto::PyString_FromStdString( sipCpp->key() );
          }
          catch( ... )
          {
            sipRes = PyLong_FromLong( sipCpp->intKey() );
          }
        }
        else
          sipRes = sipConvertFromType
            ( new carto::Object( sipCpp->currentValue() ), 
              sipType_carto_Object, 0 );
        sipCpp->next();
      }
%End


    virtual std::string key() const = 0;
    virtual long intKey() const = 0;
    virtual carto::Object keyObject() const = 0;
    virtual carto::Object clone() const = 0;

  private:
    GenericObject();
    GenericObject( const carto::GenericObject & );
    // void _sip_dont_instantiate_() = 0;
  };

// --------

  class NumericGenericObjectConverter
  {
%TypeHeaderCode
#include <pyaims/object/object.h>
%End

  public:
    static bool asBool( const carto::GenericObject & x );
    static long asInt( const carto::GenericObject & x );
    static long asLong( const carto::GenericObject & x );
  };

// --------

  class AttributedObject : carto::GenericObject
  {
%TypeHeaderCode
#include <cartobase/object/attributed.h>
%End

  public:
    AttributedObject( const std::string & s = "" );
    AttributedObject( const AttributedObject & x );
    virtual ~AttributedObject();

    virtual void setValue( Object val );

    // This function should be pure virtual but it is mandatory to
    // do explicit instantiation of TypedObject and it is not possible
    // with pure virtual classes.
    virtual Object clone() const;

    virtual std::string type() const;

    // ScalarInterface methods
    virtual bool isScalar() const;
    virtual double getScalar() const;
    virtual void setScalar( double );

    // StringInterface methods
    virtual bool isString() const;
    virtual std::string getString() const;
    virtual void setString( const std::string & );

    // ArrayInterface methods
    virtual bool isArray() const;
    virtual bool isContiguous() const;
    virtual bool hasItem( int index ) const;
    virtual Object getArrayItem( int index ) const;
    virtual void setArrayItem( int, Object );
    virtual size_t size() const;

    // DynArrayInterface methods
    virtual bool isDynArray() const;
    virtual void reserveArray( size_t );
    virtual void resizeArray( size_t );
    virtual void removeArrayItem( int );
    virtual void insertArrayItem( int, Object );

    // DictionaryInterface methods
    virtual bool getProperty( const std::string &, Object & ) const;
    virtual bool isDictionary() const;

    virtual void setProperty( const std::string &, Object );

    virtual bool removeProperty( const std::string & );
    virtual bool hasProperty( const std::string & ) const;
    virtual void clearProperties();

    // IterableInterface methods
    virtual bool isIterable() const;
    virtual Object objectIterator() const;

    // IteratorInterface methods
    virtual bool isIterator() const;
    virtual bool isValid() const;
    virtual Object currentValue() const;
    virtual void next();

    // KeyIteratorInterface methods
    virtual bool isKeyIterator() const;
    virtual Object keyObject() const;

    // DictionaryIteratorInterface methods
    virtual bool isDictionaryIterator() const;
    virtual std::string key() const;

    // IntKeyIteratorInterface methods
    virtual bool isIntKeyIterator() const;
    virtual long intKey() const;

    // NoneInterface methods
    virtual bool isNone() const;

    virtual bool operator == ( const carto::GenericObject & other ) const;

    Aims_hash_t __hash__() const;
%MethodCode
    sipRes = static_cast<long>( reinterpret_cast<unsigned long long>(sipCpp) );
%End

  };

};

