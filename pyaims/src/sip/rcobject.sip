
namespace carto
{

class RCObject
{
%TypeHeaderCode
#include <cartobase/smart/rcptrtrick.h>
// #define PYAIMS_DEBUG_RC
%End

public:
  RCObject( const carto::RCObject & );
  virtual ~RCObject();

  int __refcount__();
%MethodCode
  sipRes = carto::rc_ptr_trick::refCount( *sipCpp );
%End

  // carto::RCObject & operator = ( const carto::RCObject & );

  static void _setupRC( carto::RCObject & /GetWrapper/ );
%MethodCode
#ifdef PYAIMS_DEBUG_RC
  std::cout << "setupRC for " << a0 << " / " << typeid( *a0 ).name() << ", c++ refcount: "
    << carto::rc_ptr_trick::refCount( *a0 ) << ", pyowned: "
    << sipIsPyOwned( (sipSimpleWrapper *) a0Wrapper ) << std::endl;
#endif
#if SIP_VERSION >= 0x040800
if( sipIsPyOwned( (sipSimpleWrapper *) a0Wrapper ) 
      || carto::rc_ptr_trick::refCount( *a0 ) > 0 )
#else
if( sipIsPyOwned( (sipWrapper *) a0Wrapper )
      || carto::rc_ptr_trick::refCount( *a0 ) > 0 )
#endif
  {
    // increase C++ ref counter to avoid destruction by C++
    ++carto::rc_ptr_trick::refCount( *a0 );
#if PY_VERSION_HEX >= 0x03000000
    PyObject_SetAttrString( a0Wrapper, "_cpp_rcptr", PyLong_FromLong( 1 ) );
#else
    PyObject_SetAttrString( a0Wrapper, "_cpp_rcptr", PyInt_FromLong( 1 ) );
#endif
#ifdef PYAIMS_DEBUG_RC
    std::cout << "inc rc: " << carto::rc_ptr_trick::refCount( *a0 )
      << std::endl;
#endif
  }
%End

  long __hash__() const /AutoGen/;
%MethodCode
  sipRes = (long) sipCpp;
%End

  static void _releaseRC( carto::RCObject & /GetWrapper/ );
%MethodCode
#ifdef PYAIMS_DEBUG_RC
  std::cout << "releaseRC for " << a0 << ", c++ refcount: "
    << carto::rc_ptr_trick::refCount( *a0 ) << ", pyowned: "
    << sipIsPyOwned( (sipSimpleWrapper *) a0Wrapper ) << std::endl;
#endif
  if( PyObject_HasAttrString( a0Wrapper, "_cpp_rcptr" ) )
  {
#ifdef PYAIMS_DEBUG_RC
    std::cout << "has _cpp_ptr\n";
#endif
    carto::RCObject::RefCounterType & rc 
      = carto::rc_ptr_trick::refCount( *a0 );
    if( rc > 0 )
    {
      --rc;
#ifdef PYAIMS_DEBUG_RC
      std::cout << "dec rc: " << rc << std::endl;
#endif
      if( rc == 0 )
      {
#ifdef PYAIMS_DEBUG_RC
        std::cout << "zero count - destruction by python\n";
#endif
        // can' call delete a0 because RCObject has no virtual destructor
        // but leave the destruction job to python
        sipTransferBack( a0Wrapper );
        // here C++ doesn't own an instance anymore since rc==0
        // and _cpp_ptr is set, so ownership has once been set to python
      }
      else
      {
#ifdef PYAIMS_DEBUG_RC
        std::cout << "transfer ownership to C++\n";
#endif
        // give ownership to C++ to avoid destruction by python now
        sipTransferTo( a0Wrapper, 0 );
        // and leave a marker
#if PY_VERSION_HEX >= 0x03000000
        PyObject_SetAttrString( a0Wrapper, "_dontdel", PyLong_FromLong( 1 ) );
#else
        PyObject_SetAttrString( a0Wrapper, "_dontdel", PyInt_FromLong( 1 ) );
#endif
      }
    }
  }
%End

private:
  RCObject();
};

};

