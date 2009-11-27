
class Volume_%Template1typecode% : carto::RCObject
{
%TypeHeaderCode
#include <cartodata/volume/volume.h>
%Template1typeinclude%
typedef carto::Volume<%Template1% > Volume_%Template1typecode%;
#include <cartodata/volume/volumeoperators.h>
#include <weakrefobject.h>
#define PY_ARRAY_TYPES_PREFIX PyAIMSTypes
%End

%TypeCode
#include <iostream>
#include <pyaims/vector/numpyarrayfunc.h>
%End

public:
    Volume_%Template1typecode%( int = 1, int = 1, int = 1, int = 1,
      const carto::AllocatorContext& allocatorContext
        = carto::AllocatorContext() ) /ReleaseGIL/;
    Volume_%Template1typecode%( const Volume_%Template1typecode% & )
      /ReleaseGIL/;
    virtual ~Volume_%Template1typecode%() /ReleaseGIL/;

    int getSizeX() const;
    int getSizeY() const;
    int getSizeZ() const;
    int getSizeT() const;
    const carto::AllocatorContext & allocatorContext() const;
    void fill( const %Template1% & value ) /ReleaseGIL/;

  %Template1PyType% at( unsigned, unsigned = 0, unsigned = 0, 
                        unsigned = 0 ) const;

  %Template1PyType% value( unsigned, unsigned = 0, unsigned = 0, 
                           unsigned = 0 ) const;
%MethodCode
 sipRes = %Template1new%( sipCpp->at( a0, a1, a2, a3 ) );
%End

 void setValue( %Template1%, unsigned, unsigned = 0, unsigned = 0, 
                unsigned = 0 );
%MethodCode
 sipCpp->at( a1, a2, a3, a4 ) = %Template1deref%a0;
%End

  carto::Object* header() /Factory, ReleaseGIL/;
%MethodCode
  carto::PropertySet	& ps = sipCpp->header();
  sipRes = new carto::Object( carto::Object::reference( ps ) );
%End

  Volume_%Template1typecode%* operator + ( Volume_%Template1typecode% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r2( a1 );
  carto::VolumeRef<%Template1% > r3 = r1 + r2;
  r1.release();
  r2.release();
  sipRes = r3.get();
  r3.release();
%End

  void operator += ( Volume_%Template1typecode% & ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r2( a0 );
  r1 += r2;
  r1.release();
  r2.release();
%End

  Volume_%Template1typecode%* operator + ( const %Template1% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r3 = r1 + %Template1deref%a1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  void operator += ( const %Template1% & ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  r1 += %Template1deref%a0;
  r1.release();
%End

  Volume_%Template1typecode%* __radd__( const %Template1% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = %Template1deref%a0 + r1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  Volume_%Template1typecode%* __neg__() const /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = -r1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  Volume_%Template1typecode%* operator - ( Volume_%Template1typecode% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r2( a1 );
  carto::VolumeRef<%Template1% > r3 = r1 - r2;
  r1.release();
  r2.release();
  sipRes = r3.get();
  r3.release();
%End

  void operator -= ( Volume_%Template1typecode% & ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r2( a0 );
  r1 -= r2;
  r1.release();
  r2.release();
%End

  Volume_%Template1typecode%* operator - ( const %Template1% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r3 = r1 - %Template1deref%a1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  void operator -= ( const %Template1% & ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  r1 -= %Template1deref%a0;
  r1.release();
%End

  Volume_%Template1typecode%* __rsub__( const %Template1% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = %Template1deref%a0 - r1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  Volume_%Template1typecode%* operator * ( double ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r3 = r1 * a1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  void operator *= ( double ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  r1 *= a0;
  r1.release();
%End

  Volume_%Template1typecode%* __rmul__( double ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = r1 * a0;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  Volume_%Template1typecode%* operator / ( double ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r3 = r1 / a1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  void operator /= ( double ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  r1 /= a0;
  r1.release();
%End

%%Template1defScalar%%
%#ifdef PYAIMS_SCALAR%

  Volume_%Template1typecode%( SIP_PYOBJECT ) 
    [(int, int, int, int, %Template1% *)];
%MethodCode
  static std::list<std::set<int> > compatibletypes;
  if( compatibletypes.empty() )
  {
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      tl.insert( PyArray_BYTE );
      tl.insert( PyArray_INT8 );
    }
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      tl.insert( PyArray_UBYTE );
      tl.insert( PyArray_UINT8 );
    }
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      tl.insert( PyArray_SHORT );
      tl.insert( PyArray_INT16 );
    }
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      tl.insert( PyArray_USHORT );
      tl.insert( PyArray_UINT16 );
    }
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      tl.insert( PyArray_INT32 );
    }
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      // tl.insert( PyArray_UINT );
      tl.insert( PyArray_UINT32 );
    }
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      // tl.insert( PyArray_INT );
      tl.insert( PyArray_INT64 );
    }
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      tl.insert( PyArray_UINT );
      tl.insert( PyArray_UINT64 );
    }
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      tl.insert( PyArray_FLOAT32 );
    }
    compatibletypes.push_back( std::set<int>() );
    {
      std::set<int> & tl = compatibletypes.back();
      // tl.insert( PyArray_FLOAT );
      tl.insert( PyArray_DOUBLE );
      tl.insert( PyArray_FLOAT64 );
    }
  }
  PyArrayObject *arr = 0;
  if( !PyArray_Check( a0 ) )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "wrong argument type" );
  }
  else
  {
    arr = (PyArrayObject *) a0;

    /* I comment this out because transposed arrays are not seen as contiguous
    if( !PyArray_ISCONTIGUOUS( arr ) )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, 
                       "Building a Volume from a non-contiguous array is not " 
                       "supported" );
    }
    else
    */

    if( arr->nd < 0 || arr->nd >4 )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, 
                       "Array dimensions are not compatible with Volume" );
    }
    else if( arr->descr->type_num != %Template1NumType% )
    {
      std::list<std::set<int> >::const_iterator ict, ect = compatibletypes.end();
      bool ok = false;
      int i = 0;
      for( ict=compatibletypes.begin(); ict!=ect; ++ict, ++i )
        if( ict->find( arr->descr->type_num ) != ict->end() )
        {
          if( ict->find( %Template1NumType% ) != ict->end() )
          {
            ok = true;
            break;
          }
          else
            break;
        }
      if( !ok )
      {
        sipIsErr = 1;
        PyErr_SetString( PyExc_TypeError, "wrong array data type" );
      }
    }
  }
  if( !sipIsErr )
  {
    // retreive dimensions
    int dims[4] = { 1, 1, 1, 1 };
    int inc = 1, start = 0;
    // TODO: retreive exact strides and react accordingly
    if( PyArray_NDIM( arr ) >= 2 )
    {
      if( PyArray_STRIDES( arr )[1] < PyArray_STRIDES( arr )[0] )
      {
        // increments higher index first
        inc = -1;
        start = arr->nd-1;
      }
    }
    dims[0] = arr->dimensions[ start ];
    if( arr->nd >= 2 )
    {
      dims[1] = arr->dimensions[ start + inc ];
      if( arr->nd >= 3 )
        {
          dims[2] = arr->dimensions[ start + inc * 2 ];
          if( arr->nd >= 4 )
            dims[3] = arr->dimensions[ start + inc * 3 ];
        }
    }

    sipCpp = new sipVolume_%Template1typecode%( dims[0], dims[1], dims[2], 
                                                dims[3], 
                                                ( %Template1% *) arr->data );
    // keep ref to the array to prevent its destruction
    PyObject_SetAttrString( (PyObject *) sipSelf, "_arrayext", a0 );
  }
  else
  {
    sipIsErr = 1;
    sipCpp = 0;
  }

%End

  SIP_PYOBJECT arraydata() /Factory/;
%MethodCode
  std::vector<int> dims(4);
  dims[3] = sipCpp->getSizeX();
  dims[2] = sipCpp->getSizeY();
  dims[1] = sipCpp->getSizeZ();
  dims[0] = sipCpp->getSizeT();
  sipRes = aims::initNumpyArray( sipSelf, %Template1NumType%, 4, &dims[0],
                                 (char *) &sipCpp->at( 0 ) );
%End

  SIP_PYOBJECT __array__() /Factory/;
%MethodCode
  std::vector<int> dims(4);
  dims[3] = sipCpp->getSizeX();
  dims[2] = sipCpp->getSizeY();
  dims[1] = sipCpp->getSizeZ();
  dims[0] = sipCpp->getSizeT();
  sipRes = aims::initNumpyArray( sipSelf, %Template1NumType%, 4, &dims[0],
                                 (char *) &sipCpp->at( 0 ), true );
%End

  void checkResize();
%MethodCode
  std::vector<int> dims(4);
  dims[3] = sipCpp->getSizeX();
  dims[2] = sipCpp->getSizeY();
  dims[1] = sipCpp->getSizeZ();
  dims[0] = sipCpp->getSizeT();
  aims::resizeNumpyArray( sipSelf, 4, &dims[0], (char *) &sipCpp->at( 0 ) );
%End

  void _arrayDestroyedCallback( SIP_PYOBJECT );
%MethodCode
  // remove weak reference in volume
  PyObject_DelAttrString( sipSelf, "_arrayref" );
  // dec reference to self that was manually incremented in arraydata()
  // when building the numpy array
  Py_DECREF( sipSelf );
  // the aditional ref to self will be deleted when the callback
  // (method bound on self) is destroyed, just after now
  // so self can be destroyed safely
%End

  Volume_%Template1typecode%* operator * ( Volume_%Template1typecode% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r2( a1 );
  carto::VolumeRef<%Template1% > r3 = r1 * r2;
  r1.release();
  r2.release();
  sipRes = r3.get();
  r3.release();
%End

  void operator *= ( Volume_%Template1typecode% & ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r2( a0 );
  r1 *= r2;
  r1.release();
  r2.release();
%End

  Volume_%Template1typecode%* operator / ( Volume_%Template1typecode% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r2( a1 );
  carto::VolumeRef<%Template1% > r3 = r1 / r2;
  r1.release();
  r2.release();
  sipRes = r3.get();
  r3.release();
%End

  void operator /= ( Volume_%Template1typecode% & ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r2( a0 );
  r1 /= r2;
  r1.release();
  r2.release();
%End

  Volume_%Template1typecode%* __rdiv__( double ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = a0 / r1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

%#endif%

};

