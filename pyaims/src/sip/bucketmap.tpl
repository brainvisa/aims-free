
class BucketMap_%Template1typecode% : carto::RCObject
{
%TypeHeaderCode
#include <aims/bucket/bucket.h>
%Template1typeinclude%
#ifndef PYAIMS_BUCKETMAP_%Template1typecode%_DEFINED
#define PYAIMS_BUCKETMAP_%Template1typecode%_DEFINED
typedef aims::BucketMap< %Template1% > BucketMap_%Template1typecode%;
#endif
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy,
    sipClass_BucketMap_%Template1typecode%,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:

  BucketMap_%Template1typecode%();
  BucketMap_%Template1typecode%( const BucketMap_%Template1typecode% & );
  //BucketMap( const AimsBucket_%Template1typecode% & );
  virtual ~BucketMap_%Template1typecode%();
  //BucketMap_%Template1typecode% & operator = ( const AimsBucket_%Template1PyType% & );
  void insert( const Point3d &,
    const %Template1PyType% & = %Template1PyType%() );
  void insert( int t, const Point3d &,
    const %Template1PyType% & = %Template1PyType%() );
%MethodCode
  (*sipCpp)[a0][ *a1 ] = %Template1deref%(a2);
%End
  //void insert( const std::pair< const Point3d, %Template1PyType% > & );
  void erase( int );
%MethodCode
  sipCpp->std::map<int, BucketMap_%Template1typecode%::Bucket>::erase( a0 );
%End
  void erase( const Point3d & );
  void clear();
  void merge( const BucketMap_%Template1typecode% & );
  float sizeX() const;
  float sizeY() const;
  float sizeZ() const;
  float sizeT() const;
  void setSizeX( float );
  void setSizeY( float );
  void setSizeZ( float );
  void setSizeT( float );
  void setSizeXYZT( float, float, float, float );

  class Bucket
  {
  public:
    Bucket();
    unsigned size() const;
    int __len__() const;
%MethodCode
  sipRes = (int) sipCpp->size();
%End

    %Template1PyType% __getitem__( const Point3d & );
%MethodCode
    std::map< Point3d,%Template1% >::const_iterator i = sipCpp->find( *a0 );
    if( i != sipCpp->end() )
      sipRes = %Template1address%(*sipCpp)[ *a0 ];
    else
      {
        sipRes = 0;
        sipIsErr = 1;
        PyErr_SetString( PyExc_AttributeError, "Element not in bucket" );
      }
%End

%#define Template1_%Template1PyType% %
%#ifdef Template1_Void%
    void __setitem__( const Point3d &, const int & );
%MethodCode
    (*sipCpp)[ *a0 ] = Void();
%End
%#endif%
    void __setitem__( const Point3d &, const %Template1PyType% & );
%MethodCode
    (*sipCpp)[ *a0 ] = %Template1deref%a1;
%End

    bool has_key( const Point3d & );
%MethodCode
    sipRes = sipCpp->find( *a0 ) != sipCpp->end();
%End

    SIP_PYTUPLE keys();
%MethodCode
    PyObject	*l = PyTuple_New( sipCpp->size() );
    sipRes = l;
    std::map< Point3d,%Template1% >::const_iterator i, e = sipCpp->end();
    int n = 0;
    for( i=sipCpp->begin(); i!=e; ++i, ++n )
#if SIP_VERSION >= 0x040400
      PyTuple_SetItem( l, n, 
                       sipConvertFromNewInstance( new Point3d( i->first ), 
                                              sipClass_AimsVector_S16_3, 0 ) );
#else
      PyTuple_SetItem( l, n, sipMapCppToSelfSubClass( new Point3d( i->first ), 
                                              sipClass_AimsVector_S16_3 ) );
#endif
%End

    void clear();
  };

  unsigned size() const;

  BucketMap_%Template1typecode%::Bucket & __getitem__( int );
%MethodCode
  sipRes = &(*sipCpp)[ a0 ];
%End

  void __setitem__( int, const BucketMap_%Template1typecode%::Bucket & );
%MethodCode
  (*sipCpp)[ a0 ] = *a1;
%End

  // conversion from Object
  static  rc_ptr_BucketMap_%Template1typecode%
  fromObject( carto::GenericObject ) /Factory/;
%MethodCode
  try
    {
      carto::rc_ptr<BucketMap_%Template1typecode%> & obj 
        = a0->value<carto::rc_ptr<BucketMap_%Template1typecode%> >();
      sipRes = new carto::rc_ptr<BucketMap_%Template1typecode%>( obj );
    }
  catch( std::exception & e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
%End

  // conversion to Object

  carto::Object rcToObject();
%MethodCode
  sipRes = new carto::Object(
    carto::Object::value(
      carto::rc_ptr<BucketMap_%Template1typecode%>( sipCpp ) ) );
%End

};


