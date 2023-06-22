
namespace carto
{

class RCObject
{
%TypeHeaderCode
#include <cartobase/smart/rcptrtrick.h>
// #define PYAIMS_DEBUG_RC
%End

%ConvertToSubClassCode
  // std::cout << "ConvertToSubClassCode RCObject " << sipCpp << " / " << typeid( *sipCpp ).name() << std::endl;
  sipType = 0;

  PyErr_Clear(); // prevent "<class 'something'> returned a result with an exception set"

  if( dynamic_cast<carto::SharedObject *>( sipCpp ) )
  {
    sipType = sipType_carto_SharedObject;
    *sipCppRet = dynamic_cast<carto::SharedObject *>( sipCpp );
    // std::cout << "I'm a SharedObject: " << sipCppRet << std::endl;
  }
  else if( dynamic_cast<carto::PropertySet *>( sipCpp ) )
  {
    sipType = sipType_carto_PropertySet;
    *sipCppRet = dynamic_cast<carto::PropertySet *>( sipCpp );
  }
  else if( dynamic_cast<carto::GenericObject *>( sipCpp ) )
  {
    // std::cout << "I'm a GenericObject\n";
    sipType = sipType_carto_GenericObject;
    *sipCppRet = dynamic_cast<carto::GenericObject *>( sipCpp );

    if( dynamic_cast<Tree *>( sipCpp ) )
    {
      sipType = sipType_Tree;
      *sipCppRet = dynamic_cast<Tree *>( sipCpp );
      if( dynamic_cast<aims::Hierarchy *>( sipCpp ) )
      {
        sipType = sipType_aims_Hierarchy;
        *sipCppRet = dynamic_cast<aims::Hierarchy *>( sipCpp );
      }
    }
    else if( dynamic_cast<GraphObject *>( sipCpp ) )
    {
      sipType = sipType_GraphObject;
      *sipCppRet = dynamic_cast<GraphObject *>( sipCpp );

      if( dynamic_cast<Vertex *>( sipCpp ) )
      {
        sipType = sipType_Vertex;
        *sipCppRet = dynamic_cast<Vertex *>( sipCpp );
      }
      else if( dynamic_cast<Edge *>( sipCpp ) )
      {
        sipType = sipType_Edge;
        *sipCppRet = dynamic_cast<Edge *>( sipCpp );
      }
      else if( dynamic_cast<Graph *>( sipCpp ) )
      {
        sipType = sipType_Graph;
        *sipCppRet = dynamic_cast<Graph *>( sipCpp );
      }
    }
  }
  else if( dynamic_cast<carto::MutexedObject *>( sipCpp ) )
  {
    sipType = sipType_carto_MutexedObject;
    *sipCppRet = dynamic_cast<carto::MutexedObject *>( sipCpp );
  }
  else if( dynamic_cast<carto::Headered *>( sipCpp ) )
  {
    sipType = sipType_carto_Headered;
    *sipCppRet = dynamic_cast<carto::Headered *>( sipCpp );

    if( dynamic_cast<carto::Volume<uint8_t> *>( sipCpp ) )
    {
      sipType = sipType_Volume_U8;
      *sipCppRet = dynamic_cast<carto::Volume<uint8_t> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<int16_t> *>( sipCpp ) )
    {
      sipType = sipType_Volume_S16;
      *sipCppRet = dynamic_cast<carto::Volume<int16_t> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<uint16_t> *>( sipCpp ) )
    {
      sipType = sipType_Volume_U16;
      *sipCppRet = dynamic_cast<carto::Volume<uint16_t> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<int32_t> *>( sipCpp ) )
    {
      sipType = sipType_Volume_S32;
      *sipCppRet = dynamic_cast<carto::Volume<int32_t> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<uint32_t> *>( sipCpp ) )
    {
      sipType = sipType_Volume_U32;
      *sipCppRet = dynamic_cast<carto::Volume<uint32_t> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<bool> *>( sipCpp ) )
    {
      sipType = sipType_Volume_BOOL;
      *sipCppRet = dynamic_cast<carto::Volume<bool> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<float> *>( sipCpp ) )
    {
      sipType = sipType_Volume_FLOAT;
      *sipCppRet = dynamic_cast<carto::Volume<float> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<double> *>( sipCpp ) )
    {
      sipType = sipType_Volume_DOUBLE;
      *sipCppRet = dynamic_cast<carto::Volume<double> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<AimsRGB> *>( sipCpp ) )
    {
      sipType = sipType_Volume_RGB;
      *sipCppRet = dynamic_cast<carto::Volume<AimsRGB> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<AimsRGBA> *>( sipCpp ) )
    {
      sipType = sipType_Volume_RGBA;
      *sipCppRet = dynamic_cast<carto::Volume<AimsRGBA> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<AimsHSV> *>( sipCpp ) )
    {
      sipType = sipType_Volume_HSV;
      *sipCppRet = dynamic_cast<carto::Volume<AimsHSV> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<cfloat> *>( sipCpp ) )
    {
      sipType = sipType_Volume_CFLOAT;
      *sipCppRet = dynamic_cast<carto::Volume<cfloat> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<cdouble> *>( sipCpp ) )
    {
      sipType = sipType_Volume_CDOUBLE;
      *sipCppRet = dynamic_cast<carto::Volume<cdouble> *>( sipCpp );
    }
    else if( dynamic_cast<carto::Volume<Point3df> *>( sipCpp ) )
    {
      sipType = sipType_Volume_POINT3DF;
      *sipCppRet = dynamic_cast<carto::Volume<Point3df> *>( sipCpp );
    }
  }
  else if( dynamic_cast<BucketMap_VOID *>( sipCpp ) )
  {
    sipType = sipType_BucketMap_VOID;
    *sipCppRet = dynamic_cast<BucketMap_VOID *>( sipCpp );
  }
  else if( dynamic_cast<BucketMap_S16 *>( sipCpp ) )
  {
    sipType = sipType_BucketMap_S16;
    *sipCppRet = dynamic_cast<BucketMap_S16 *>( sipCpp );
  }
  else if( dynamic_cast<BucketMap_S32 *>( sipCpp ) )
  {
    sipType = sipType_BucketMap_S32;
    *sipCppRet = dynamic_cast<BucketMap_S32 *>( sipCpp );
  }
  else if( dynamic_cast<BucketMap_U32 *>( sipCpp ) )
  {
    sipType = sipType_BucketMap_U32;
    *sipCppRet = dynamic_cast<BucketMap_U32 *>( sipCpp );
  }
  else if( dynamic_cast<BucketMap_FLOAT *>( sipCpp ) )
  {
    sipType = sipType_BucketMap_FLOAT;
    *sipCppRet = dynamic_cast<BucketMap_FLOAT *>( sipCpp );
  }
  else if( dynamic_cast<BucketMap_DOUBLE *>( sipCpp ) )
  {
    sipType = sipType_BucketMap_DOUBLE;
    *sipCppRet = dynamic_cast<BucketMap_DOUBLE *>( sipCpp );
  }
  else if( dynamic_cast<carto::Transformation *>( sipCpp ) )
  {
    sipType = sipType_soma_Transformation;
    *sipCppRet = dynamic_cast<soma::Transformation *>( sipCpp );
    if( dynamic_cast<carto::Transformation3d *>( sipCpp ) )
    {
      sipType = sipType_soma_Transformation3d;
      *sipCppRet = dynamic_cast<soma::Transformation3d *>( sipCpp );
      // std::cout << "I'm a Transformation3d: " << sipCppRet << std::endl;
      if( dynamic_cast<soma::AffineTransformation3dBase *>( sipCpp ) )
      {
        sipType = sipType_soma_AffineTransformation3dBase;
        *sipCppRet = dynamic_cast<soma::AffineTransformation3dBase *>( sipCpp );
        if( dynamic_cast<aims::AffineTransformation3d *>( sipCpp ) )
        {
          sipType = sipType_aims_AffineTransformation3d;
          *sipCppRet = dynamic_cast<aims::AffineTransformation3d *>( sipCpp );
          // std::cout << "I'm an AffineTransformation3d: " << sipCppRet << std::endl;
        }
      }
      else if( dynamic_cast<aims::TransformationChain3d *>( sipCpp ) )
      {
        sipType = sipType_aims_TransformationChain3d;
        *sipCppRet = dynamic_cast<aims::TransformationChain3d *>( sipCpp );
        // std::cout << "I'm an TransformationChain3d: " << *sipCppRet << std::endl;
      }
    }
    else if( dynamic_cast<carto::AffineTransformationBase *>( sipCpp ) )
    {
      sipType = sipType_soma_AffineTransformationBase;
      *sipCppRet = dynamic_cast<soma::AffineTransformationBase *>( sipCpp );
    }
  }
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

  Py_hash_t __hash__() const;
%MethodCode
  sipRes = static_cast<long>( reinterpret_cast<unsigned long long>(sipCpp) );
%End

  static void _releaseRC( carto::RCObject & /GetWrapper/ );
%MethodCode
#ifdef PYAIMS_DEBUG_RC
  std::cout << "releaseRC for " << a0 << ", c++ refcount: "
    << carto::rc_ptr_trick::refCount( *a0 ) << ", pyowned: "
    << sipIsPyOwned( (sipSimpleWrapper *) a0Wrapper ) << std::endl;
  if( carto::rc_ptr_trick::refCount( *a0 ) > 0 )
    std::cout << "    " << typeid(*a0).name() << std::endl;
#endif
  carto::RCObject::RefCounterType & rc
    = carto::rc_ptr_trick::refCount( *a0 );
  if( rc > 0 )
  {
    if( PyObject_HasAttrString( a0Wrapper, "_cpp_rcptr" ) )
    {
#ifdef PYAIMS_DEBUG_RC
      std::cout << "has _cpp_ptr\n";
#endif
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
#ifdef PYAIMS_DEBUG_RC
    else
      std::cout << "no _cpp_ptr\n";
#endif
  }
  else if( sipIsPyOwned( (sipSimpleWrapper *) a0Wrapper ) )
  {
    // this situation should not happen, it is caused after forced
    // destruction in C++ layer
#ifdef PYAIMS_DEBUG_RC
      std::cout << "refcount <= 0, the RCObject is probably already deleted. Transfer ownership to C++\n";
#endif
      // give ownership to C++ to avoid re-destruction by python
      sipTransferTo( a0Wrapper, 0 );
      // and leave a marker
#if PY_VERSION_HEX >= 0x03000000
      PyObject_SetAttrString( a0Wrapper, "_dontdel", PyLong_FromLong( 1 ) );
#else
      PyObject_SetAttrString( a0Wrapper, "_dontdel", PyInt_FromLong( 1 ) );
#endif
  }
%End

private:
  RCObject();
};

};

