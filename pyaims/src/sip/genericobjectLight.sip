
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

%ConvertToSubClassCode
  if (dynamic_cast<carto::GenericObject *>(sipCpp))
	sipType = sipType_carto_GenericObject;
  else if (dynamic_cast<carto::RCObject *>(sipCpp))
	sipType = sipType_carto_RCObject;
  else  sipType = NULL;
%End

  public:
    virtual std::string type() const = 0;

    // Size API
    virtual size_t size() const = 0;
    Py_ssize_t __len__() const;
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

    SIP_PYTUPLE keys() const;
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
          PyTuple_SetItem( po, c, carto::PyString_FromStdString( i->key().c_str() ) );
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


    SIP_PYTUPLE items() const;
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
          PyTuple_SetItem( pi, 0, carto::PyString_FromStdString( i->key().c_str() ) );
          PyTuple_SetItem( pi, 1,
            sipConvertFromType( new carto::Object( i->currentValue() ),
              sipType_carto_Object, Py_None ) );
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


    SIP_PYTUPLE values() const;
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
            sipConvertFromType( new carto::Object( i->currentValue() ),
              sipType_carto_Object, Py_None ) );
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


    carto::Object __getitem__( const std::string & ) /Factory/;
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
                                  carto::Object::value
                                   ( (int) PyLong_AsLong( a1 ) ) );
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
        else if( PyUnicode_Check( a1 ) )
          {
            sipCpp->setArrayItem
              ( a0,
                carto::Object::value
                ( carto::PyString_AsStdString( a1 ) ) );
          }
        else if( ( obj = sipForceConvertToType( a1, sipType_carto_Object,
          0, 0, &state, &sipIsErr ) ) )
          {
            carto::Object	*o = (carto::Object *) obj;
            sipCpp->setArrayItem( a0, *o );
            sipReleaseType( obj, sipType_carto_Object, state );
          }
        else if( ( obj = sipForceConvertToType( a1,
          sipType_carto_GenericObject, 0, 0, &state, &sipIsErr ) ) )
          {
            carto::GenericObject	*o = (carto::GenericObject *) obj;
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
        sipIsErr = 1;
        PyErr_SetString( PyExc_AttributeError, e.what() );
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
                                 carto::Object::value
                                   ( (int) PyLong_AsLong( a1 ) ) );
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
        else if( PyUnicode_Check( a1 ) )
          {
            sipCpp->setProperty
              ( *a0, 
                carto::Object::value
                ( carto::PyString_AsStdString( a1 ) ) );
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
        sipIsErr = 1;
        PyErr_SetString( PyExc_AttributeError, e.what() );
      }
%End


    void __delitem__( const std::string & );
%MethodCode
    sipCpp->removeProperty( *a0 );
%End


    // printing

    SIP_PYOBJECT __str__();
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
 
        sipRes = carto::PyString_FromStdString(s2);
      }
    catch( std::exception & e )
      {
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_RuntimeError, e.what() );
      }
%End

    // Syntaxed API

    virtual bool hasSyntax() const;
%MethodCode
    const carto::SyntaxedInterface
      *si = sipCpp->getInterface<carto::SyntaxedInterface>();
    if( si && si->hasSyntax() )
      sipRes = true;
    else
      sipRes = false;
%End

    virtual std::string getSyntax() const /Factory/;
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
    carto::Object __objiter__() const /Factory/;
%MethodCode
    //std::cout << "__iter__\n";
    if( sipCpp->isIterator() )
      sipRes = new carto::Object( const_cast<carto::GenericObject *>( sipCpp ) );
    else
      sipRes = new carto::Object( sipCpp->objectIterator() );
%End

    int __contains__( const std::string & ) const;
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

    int __contains__( carto::Object ) const;
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

    SIP_PYOBJECT next();
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
          sipRes = carto::PyString_FromStdString( sipCpp->key().c_str() );
        else
          sipRes = sipConvertFromType
            ( new carto::Object( sipCpp->currentValue() ), 
              sipType_carto_Object, 0 );
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

