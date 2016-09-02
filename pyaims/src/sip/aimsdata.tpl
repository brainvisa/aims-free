%#define PyAims_AimsData_%Template1typecode%_defined%

class AimsData_%Template1typecode% : carto::RCObject
{
%TypeHeaderCode
#include <aims/data/data.h>
%Template1typeinclude%
typedef AimsData<%Template1% > AimsData_%Template1typecode%;
%End

%TypeCode
#include <aims/data/pheader.h>
#include <pyaims/object/rcptrconv.h>
// DEBUG
// #include <cartobase/smart/rcptrtrick.h>
%Template1sipinclude%
%End

%ConvertToTypeCode
  // std::cout << "ConvertToTypeCode AimsData_%Template1typecode%\n";
  if (!sipIsErr)
  {
    // check type
    return sipCanConvertToInstance( sipPy,
      sipClass_AimsData_%Template1typecode%,
      SIP_NOT_NONE | SIP_NO_CONVERTORS )
    || sipCanConvertToInstance( sipPy,
      sipClass_rc_ptr_Volume_%Template1typecode%,
      SIP_NOT_NONE | SIP_NO_CONVERTORS )
    || sipCanConvertToInstance( sipPy,
        sipClass_rc_ptr_Volume_%Template1typecode%, SIP_NOT_NONE)
    || pyaims::canConvertFromProxy( sipPy,
        sipClass_AimsData_%Template1typecode% );
  }

  if( sipPy == Py_None )
  {
    *sipCppPtr = 0;
    return 0;
  }

  int state = 0;

  int res = pyaims::standardConvertToTypeCode( sipPy,
    sipClass_AimsData_%Template1typecode%, sipTransferObj, sipIsErr, sipCppPtr );
  if( *sipCppPtr )
    return res;

  carto::rc_ptr<carto::Volume<%Template1% > > * obj
    = (carto::rc_ptr<carto::Volume<%Template1% > > *)
      sipConvertToInstance( sipPy,
        sipClass_rc_ptr_Volume_%Template1typecode%,
        sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
  if( *sipIsErr && obj )
  {
    sipReleaseInstance( obj, sipClass_rc_ptr_Volume_%Template1typecode%,
                        state );
    obj = 0;
  }
  if( !obj )
  {
    *sipIsErr = 0;
    obj
      = (carto::rc_ptr<carto::Volume<%Template1% > > *)
        sipForceConvertToInstance( sipPy,
          sipClass_rc_ptr_Volume_%Template1typecode%,
          sipTransferObj, 0, &state, sipIsErr );
  }
  if( obj )
  {
    /* std::cout << "conversion to AimsData possible\n";
    std::cout << "rc: "
      << carto::rc_ptr_trick::refCount( *obj )
      << std::endl; */
    *sipCppPtr = new AimsData< %Template1% >( *obj );
    sipReleaseInstance( obj, sipClass_rc_ptr_Volume_%Template1typecode%,
                        state );
    return sipGetState(sipTransferObj);
  }
  *sipIsErr = 1;
  return 0;
%End


public:
  AimsData_%Template1typecode%( int = 1, int = 1, int = 1, int = 1, int = 0 )
    /ReleaseGIL/;
  AimsData_%Template1typecode%( int, int, int, int, int,
    const carto::AllocatorContext &) /ReleaseGIL/;
  AimsData_%Template1typecode%( const AimsData_%Template1typecode% & )
    /ReleaseGIL/;
  AimsData_%Template1typecode%( const AimsData_%Template1typecode% &, int )
    /ReleaseGIL/;
  AimsData_%Template1typecode%( const rc_ptr_Volume_%Template1typecode% & )
    /ReleaseGIL/;
  virtual ~AimsData_%Template1typecode%() /ReleaseGIL/;

  rc_ptr_Volume_%Template1typecode% volume() /ReleaseGIL/;

  int dimX () const;
  int dimY () const;
  int dimZ () const;
  int dimT () const;
  float sizeX () const /ReleaseGIL/;
  float sizeY () const /ReleaseGIL/;
  float sizeZ () const /ReleaseGIL/;
  float sizeT () const /ReleaseGIL/;
  void setSizeX( float ) /ReleaseGIL/;
  void setSizeY( float ) /ReleaseGIL/;
  void setSizeZ( float ) /ReleaseGIL/;
  void setSizeT( float ) /ReleaseGIL/;
  void setSizeXYZT( float = 1, float = 1, float = 1, float = 1 ) /ReleaseGIL/;
  void setSizeXYZT( const AimsData_%Template1typecode% & ) /ReleaseGIL/;

  %Template1% value( unsigned = 0, unsigned = 0, unsigned = 0, 
               unsigned = 0 ) const;
%MethodCode
 sipRes = %Template1new%( sipCpp->operator() ( a0, a1, a2, a3 ) );
%End

 void setValue( %Template1%, unsigned = 0, unsigned = 0, unsigned = 0, 
                unsigned = 0 );
%MethodCode
 sipCpp->operator() ( a1, a2, a3, a4 ) = %Template1deref%a0;
%End

%#ifdef PyAims_AimsData_U8_defined%
 void setValue( int, unsigned, unsigned = 0, unsigned = 0,
                unsigned = 0 );
%MethodCode
 sipCpp->operator() ( a1, a2, a3, a4 ) = %Template1deref%a0;
%End
%#endif%

  AimsData_%Template1typecode% operator +
    ( const AimsData_%Template1typecode% & ) /ReleaseGIL/;
  AimsData_%Template1typecode% operator -
    ( const AimsData_%Template1typecode% & ) /ReleaseGIL/;
  AimsData_%Template1typecode% operator * ( float ) /ReleaseGIL/;
/* // these operators are not defined in cartodata volumes
  int operator == ( const AimsData_%Template1typecode% & );
  int operator == ( const %Template1% & );
*/

  AimsData_%Template1typecode% clone () const /ReleaseGIL/;
  void fillBorder( const %Template1% & ) /ReleaseGIL/;

  void fill( const %Template1% & ) /ReleaseGIL/;
%MethodCode
  *sipCpp = %Template1deref%a0;
%End

  carto::GenericObject* header() /ReleaseGIL/;
%MethodCode
  aims::PythonHeader	*h 
    = dynamic_cast<aims::PythonHeader *>( sipCpp->header() );
  if( h )
    sipRes = h;
  else
    sipRes = 0;
%End

  // conversion from Object
  static  rc_ptr_AimsData_%Template1typecode%
  fromObject( carto::GenericObject ) /Factory, ReleaseGIL/;
%MethodCode
  try
    {
      carto::rc_ptr<AimsData<%Template1% > > & 
        rcp = a0->value<carto::rc_ptr<AimsData<%Template1% > > >();
      sipRes = new carto::rc_ptr<AimsData<%Template1% > >( rcp );
    }
  catch( std::exception & e )
    {
      sipIsErr = 1;
      SIP_BLOCK_THREADS
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      SIP_UNBLOCK_THREADS
      sipRes = 0;
    }
%End

%%Template1defScalar%%
%#ifdef PYAIMS_SCALAR%

  %Template1% minimum() const /ReleaseGIL/;
  %Template1% maximum() const /ReleaseGIL/;
  SIP_PYOBJECT minIndex() const /ReleaseGIL/;
%MethodCode
  int	x, y, z, t;
  %Template1%	v = sipCpp->minIndex( &x, &y, &z, &t );
  SIP_BLOCK_THREADS
  PyObject	*res = PyTuple_New( 2 );
  PyObject	*pos = PyTuple_New( 4 );
#if PY_VERSION_HEX >= 0x03000000
  PyTuple_SET_ITEM( pos, 0, PyLong_FromLong( x ) );
  PyTuple_SET_ITEM( pos, 1, PyLong_FromLong( y ) );
  PyTuple_SET_ITEM( pos, 2, PyLong_FromLong( z ) );
  PyTuple_SET_ITEM( pos, 3, PyLong_FromLong( t ) );
#else
  PyTuple_SET_ITEM( pos, 0, PyInt_FromLong( x ) );
  PyTuple_SET_ITEM( pos, 1, PyInt_FromLong( y ) );
  PyTuple_SET_ITEM( pos, 2, PyInt_FromLong( z ) );
  PyTuple_SET_ITEM( pos, 3, PyInt_FromLong( t ) );
#endif
  PyTuple_SET_ITEM( res, 0, pos );
  PyTuple_SET_ITEM( res, 1, %Template1pyFromC%( v ) );
  sipRes = res;
  SIP_UNBLOCK_THREADS
%End

  SIP_PYOBJECT maxIndex() const /ReleaseGIL/;
%MethodCode
  int	x, y, z, t;
  %Template1%	v = sipCpp->maxIndex( &x, &y, &z, &t );
  SIP_BLOCK_THREADS
  PyObject	*res = PyTuple_New( 2 );
  PyObject	*pos = PyTuple_New( 4 );
#if PY_VERSION_HEX >= 0x03000000
  PyTuple_SET_ITEM( pos, 0, PyLong_FromLong( x ) );
  PyTuple_SET_ITEM( pos, 1, PyLong_FromLong( y ) );
  PyTuple_SET_ITEM( pos, 2, PyLong_FromLong( z ) );
  PyTuple_SET_ITEM( pos, 3, PyLong_FromLong( t ) );
#else
  PyTuple_SET_ITEM( pos, 0, PyInt_FromLong( x ) );
  PyTuple_SET_ITEM( pos, 1, PyInt_FromLong( y ) );
  PyTuple_SET_ITEM( pos, 2, PyInt_FromLong( z ) );
  PyTuple_SET_ITEM( pos, 3, PyInt_FromLong( t ) );
#endif
  PyTuple_SET_ITEM( res, 0, pos );
  PyTuple_SET_ITEM( res, 1, %Template1pyFromC%( v ) );
  sipRes = res;
  SIP_UNBLOCK_THREADS
%End

%#endif%

};

%#ifdef PYAIMS_SCALAR%

AimsData_%Template1typecode% * operator + ( AimsData_%Template1typecode% &, %Template1% ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0->volume() );
  carto::VolumeRef<%Template1% > r3 = r1 + a1;
  r1.release();
  sipRes = new AimsData<%Template1% >( r3 );
  r3.release();
%End

AimsData_%Template1typecode% * operator + ( %Template1%, AimsData_%Template1typecode% & ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a1->volume() );
  carto::VolumeRef<%Template1% > r3 = r1 + a0;
  r1.release();
  sipRes = new AimsData<%Template1% >( r3 );
  r3.release();
%End

AimsData_%Template1typecode% * operator - ( %Template1%, AimsData_%Template1typecode% & ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a1->volume() );
  carto::VolumeRef<%Template1% > r3 = -r1 + a0;
  r1.release();
  sipRes = new AimsData<%Template1% >( r3 );
  r3.release();
%End

AimsData_%Template1typecode% * operator - ( AimsData_%Template1typecode% &, %Template1% ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0->volume() );
  carto::VolumeRef<%Template1% > r3 = -r1 + a1;
  r1.release();
  sipRes = new AimsData<%Template1% >( r3 );
  r3.release();
%End

AimsData_%Template1typecode% * operator * ( double, AimsData_%Template1typecode% & ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a1->volume() );
  carto::VolumeRef<%Template1% > r3 = r1 * a0;
  r1.release();
  sipRes = new AimsData<%Template1% >( r3 );
  r3.release();
%End

AimsData_%Template1typecode% * operator * ( long, AimsData_%Template1typecode% & ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a1->volume() );
  carto::VolumeRef<%Template1% > r3 = r1 * a0;
  r1.release();
  sipRes = new AimsData<%Template1% >( r3 );
  r3.release();
%End

AimsData_%Template1typecode% * operator / ( AimsData_%Template1typecode% &, double ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0->volume() );
  carto::VolumeRef<%Template1% > r3 = r1 / a1;
  r1.release();
  sipRes = new AimsData<%Template1% >( r3 );
  r3.release();
%End

AimsData_%Template1typecode% * operator / ( AimsData_%Template1typecode% &, long ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0->volume() );
  carto::VolumeRef<%Template1% > r3 = r1 / a1;
  r1.release();
  sipRes = new AimsData<%Template1% >( r3 );
  r3.release();
%End

%#endif%


%#undef PyAims_AimsData_%Template1typecode%_defined%

