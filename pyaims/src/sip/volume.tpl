
%#define PyAims_Volume_%Template1typecode%_defined%

class Volume_%Template1typecode% : carto::RCObject
{
%TypeHeaderCode
#include <cartodata/volume/volume.h>
%Template1typeinclude%
#ifndef PYAIMS_VOLUME_%Template1typecode%_DEFINED
#define PYAIMS_VOLUME_%Template1typecode%_DEFINED
typedef carto::Volume<%Template1% > Volume_%Template1typecode%;
#endif
#include <cartobase/smart/rcptr.h>
#ifndef PYAIMS_RCPTR_Volume_%Template1typecode%_DEFINED
#define PYAIMS_RCPTR_Volume_%Template1typecode%_DEFINED
typedef carto::rc_ptr<carto::Volume<%Template1% > > rc_ptr_Volume_%Template1typecode%;
#endif
#include <cartodata/volume/volumeoperators.h>
#include <weakrefobject.h>
#define PY_ARRAY_TYPES_PREFIX PyAIMSTypes
#include <pyaims/vector/stdvector.h>
#ifndef PYAIMS_VOLUME_BOOL_DEFINED
#define PYAIMS_VOLUME_BOOL_DEFINED
typedef carto::Volume<bool> Volume_BOOL;
#endif
%End

%TypeCode
#include <iostream>
#include <pyaims/vector/numpyarrayfunc.h>
#include <pyaims/object/rcptrconv.h>
%Template1sipinclude%
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy,
    sipType_Volume_%Template1typecode%,
    sipTransferObj, sipIsErr, sipCppPtr );

%End


public:
    class Position4Di
    {
%Docstring
4 ints for position or size, used for Volume views.
%End
    public:
      Position4Di( int x = 0, int y = 0, int z = 0, int t = 0 );
      Position4Di( const Volume_%Template1typecode%::Position4Di & );
      ~Position4Di();
      int operator [] ( int coord ) const;
      Py_ssize_t __len__() const;
%MethodCode
      sipRes = 4;
%End
    };

    Volume_%Template1typecode%( int = 1, int = 1, int = 1, int = 1,
      const carto::AllocatorContext& allocatorContext
        = carto::AllocatorContext(), bool = true ) /ReleaseGIL/;
    Volume_%Template1typecode%( int sizeX, int sizeY, int sizeZ,
      int sizeT, int bordersize,
      const carto::AllocatorContext& allocatorContext
      = carto::AllocatorContext(), bool allocated = true ) /ReleaseGIL/;
    Volume_%Template1typecode%( const Volume_%Template1typecode% & )
      /ReleaseGIL/;
    Volume_%Template1typecode%( rc_ptr_Volume_%Template1typecode% other,
      const vector_S32 & pos = vector_S32( 4, 0 ),
      const vector_S32 & size = vector_S32(),
      const carto::AllocatorContext & allocContext
        = carto::AllocatorContext() );
    Volume_%Template1typecode%( const vector_S32 &,
      const carto::AllocatorContext& allocatorContext,
      bool = true ) /ReleaseGIL/;
    Volume_%Template1typecode%( const vector_S32 &,
      const vector_S32 &,
      const carto::AllocatorContext& allocatorContext
        = carto::AllocatorContext(), bool = true ) /ReleaseGIL/;
    /*
      The constructor taking a vector<int> is moved to the end of this class,
      because the numpy array constructor should be used first.
    */

    virtual ~Volume_%Template1typecode%() /ReleaseGIL/;

  // conversion from Object
  static  rc_ptr_Volume_%Template1typecode%
  fromObject( carto::GenericObject ) /Factory, ReleaseGIL/;
%MethodCode
  try
    {
      carto::rc_ptr<carto::Volume<%Template1% > > &
        rcp = a0->value<carto::rc_ptr<carto::Volume<%Template1% > > >();
      sipRes = new carto::rc_ptr<carto::Volume<%Template1% > >( rcp );
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

    int getSizeX() const;
    int getSizeY() const;
    int getSizeZ() const;
    int getSizeT() const;
    vector_S32 getSize() const;
%Docstring
Number of voxels in each dimension (list of 4 ints)
%End

    vector_FLOAT getVoxelSize() const;
%Docstring
Voxel sizes in mm (list of 4 floats)
%End

    void setVoxelSize( const vector_FLOAT & );
%Docstring
setVoxelSize(vs_list)
setVoxelSize(vx, vy=1., vz=1., vt=1.)

Set voxel sizes in mm (list of at least 4 floats)
%End

    void setVoxelSize( float vx, float vy=1., float vz=1., float vt=1. );

    virtual void copyHeaderFrom( const carto::PropertySet & other,
                                 bool stopOnError = true );
    virtual void copyHeaderFrom( const carto::Object & other,
                                 bool stopOnError = true );

    carto::AllocatorContext & allocatorContext();
%MethodCode
    // handle constness of C++ returned object
    // and the fact that a copied context is always unallocated
    sipRes = const_cast<carto::AllocatorContext *>(
        &sipCpp->allocatorContext() );
    // Problem: the returned object is actually modifiable, which is not safe.
%End

    void fill( const %Template1% & value ) /ReleaseGIL/;
%Docstring
fill(value)

Fill Volume_%Template1typecode% using the given value.
%End

    void fillBorder( const %Template1% & value ) /ReleaseGIL/;
%Docstring
fillBorder(value)

Fill the surrounding of the volume view in the reference volume (if any) using the given value.
%End

%#ifdef PyAims_Volume_U8_defined%
    void fill( int ) /ReleaseGIL/;
%MethodCode
        sipCpp->fill( %Template1deref%a0 );
%End
%#endif%

    bool all() const;
    bool any() const;
    %Template1PyType% min() const;
    %Template1PyType% max() const;

    rc_ptr_Volume_%Template1typecode% refVolume() const;
%Docstring
If the volume is a view into another (larger) one, this returns the "parent" one.
%End

    vector_S32 posInRefVolume() const;
%Docstring
If the volume is a view into another (larger) one, this returns the position in "parent" one.
%End

    void setPosInRefVolume(const vector_S32 & pos);
%Docstring
Set position in parent volume
%End

  %Template1PyType% at( long, long = 0, long = 0, long = 0 ) const;
%Docstring
at(posx, posy=0, posz=0, post=0)

Returns the volume value for the selected voxel.
%End

  %Template1PyType% at( long, long, long, long, long, long=0, long=0, long=0 ) const;
%Docstring
at(posx1, posx2, posx3, posx4, posx5, posx6=0, posx7=0, posx8=0)

Returns the volume value for the selected voxel.
%End

  %Template1PyType% at( const vector_S32 & ) const;
%Docstring
at(vector_int)

Returns the volume value for the selected voxel.
%End

  %Template1PyType% value( long, long = 0, long = 0,
                           long = 0 ) const;
%Docstring
value(posx, posy=0, posz=0, post=0)

value is an alias to :py:meth:`at`: returns the volume value for the selected voxel.
%End

%MethodCode
 sipRes = %Template1new%( sipCpp->at( a0, a1, a2, a3 ) );
%End

 void setValue( %Template1%, long, long = 0, long = 0,
                long = 0 );
%Docstring
setValue(value, x, y=0, z=0, t=0)

Set the voxel value at the given position.
%End

%MethodCode
 sipCpp->at( a1, a2, a3, a4 ) = %Template1deref%a0;
%End

 void setValue( %Template1%, const vector_S32 & );
%Docstring
setValue(value, [x1, x2, x3, x4, x5, x6...])

Set the voxel value at the given position.
%End

%MethodCode
 sipCpp->at( *a1 ) = %Template1deref%a0;
%End

%#ifdef PyAims_Volume_U8_defined%
 void setValue( int, long, long = 0, long = 0, long = 0 );
%Docstring
Set a voxel value at given position
%End
%MethodCode
 sipCpp->at( a1, a2, a3, a4 ) = %Template1deref%a0;
%End

 void setValue( int, const vector_S32 & );
%Docstring
Set a voxel value at given position
%End
%MethodCode
 sipCpp->at( *a1 ) = %Template1deref%a0;
%End
%#endif%

  vector_S64 getStrides() const;
  vector_S32 getBorders() const;

  void allocateBorders( int bsx, int bsy = -1, int bsz = -1 );
%Docstring
  reallocate the volume with given borders, keep (copy) the contents.
%End

  void allocateBorders( const vector_S32 & border );
%Docstring
  reallocate the volume with given borders, keep (copy) the contents.
  only 1 value is mandatory.
%End

  SIP_PYOBJECT header() /Factory/;
%Docstring
The header contains all meta-data.
%End
%MethodCode
  carto::PropertySet	& ps = sipCpp->header();
  carto::Object* h = new carto::Object( carto::Object::reference( ps ) );
  sipRes = sipConvertFromNewType( h, sipType_carto_Object, 0 );
  // set into header a reference to the volume to forbid it to die before the
  // python header object
  if( PyObject_SetAttrString( sipRes, "_volref", sipSelf ) == -1 )
  {
    std::cerr << "cannot set header ._volref" << std::endl;
  }
%End

  carto::Referential & referential();

  void flipToOrientation( const std::string & orient );
%MethodCode
  sipCpp->flipToOrientation( *a0 );
  aims::releaseNumpyArray( sipSelf );
%End

  void flipToOrientation( const std::string & orient,
                          const std::string & force_memory_layout );
%Docstring
  volume.flipToOrientation(orient, force_memory_layout="")

  Flip the volume to a given orientation

  The volume voxels will be reordered to match the given orientation.

  If ``force_memory_layout`` is not given, then only the strides will be
  changed,  and the data block will remain preserverd.

  Orientation is given as a 3 char string: "LPI" (the default orientation
  in AIMS), "RAS", and combinations of these 6 letters. See
  https://brainvisa.info/aimsdata/user_doc/coordinates_systems.html and
  http://www.grahamwideman.com/gw/brain/orientation/orientterms.htm.

  If ``force_memory_layout`` is used, on the contrary, the voxels data block
  will be reallocated and flipped to match the given orientation. It is
  also given as a 3 char string, thus it may specify a different memory
  layout from the one used for indices.
%End
%MethodCode
  sipCpp->flipToOrientation( *a0, *a1 );
  aims::releaseNumpyArray( sipSelf );
%End

  carto::Object reorientedHeader( const std::string & orient ) const;

  vector_S32 memoryLayoutOrientation() const;
%Docstring
    determine the memory layout orientation from strides and current
    indices orientation.

    Use
    ``volume.referential().orientationStr(volume.memoryLayoutOrientation())``
    to get a more readable string description.
%End

  vector_S32 storageLayoutOrientation() const;
%Docstring
    determine the storage (disk) layout orientation.

    The storage orientation is optionnally stored int the header
    "storage_to_memory" matrix. If not, the storage orientation is
    undefined.

    Use
    volume.referential().orientationStr(volume.storageLayoutOrientation())
    to get a more readable string description.
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

/*
  Volume_%Template1typecode%* __radd__( const %Template1% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = %Template1deref%a0 + r1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End
*/

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

/*
  Volume_%Template1typecode%* __rsub__( const %Template1% & )
    /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = %Template1deref%a0 - r1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End
*/

  Volume_%Template1typecode%* operator * ( double ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r3 = r1 * a1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  Volume_%Template1typecode%* operator * ( long ) /Factory, ReleaseGIL/;
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

  void operator *= ( long ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  r1 *= a0;
  r1.release();
%End

/*
  Volume_%Template1typecode%* __rmul__( double ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = r1 * a0;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  Volume_%Template1typecode%* __rmul__( long ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = r1 * a0;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End
*/

  Volume_%Template1typecode%* operator / ( double ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a0 );
  carto::VolumeRef<%Template1% > r3 = r1 / a1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

  Volume_%Template1typecode%* operator / ( long ) /Factory, ReleaseGIL/;
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

  void operator /= ( long ) /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  r1 /= a0;
  r1.release();
%End

  int __nonzero__() const /ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r( const_cast<carto::Volume<%Template1% > *>(sipCpp) );
  sipRes = r->all();
  r.release();
%End

  Volume_BOOL * operator == ( Volume_%Template1typecode% & )
  /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r2( a0 );
  
  carto::VolumeRef<bool> r = 
        carto::copyStructure<bool, %Template1% >( r1 );
  carto::volumeutil::applyTowards( *r1, *r2, *r, 
        carto::volumeutil::equal_to<%Template1%, %Template1% >() );
  
  sipRes = r.get();
  r.release();
  r1.release();
  r2.release();
%End

  Volume_BOOL * operator != ( Volume_%Template1typecode% & )
  /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r2( a0 );
  
  carto::VolumeRef<bool> r = 
        carto::copyStructure<bool, %Template1% >( r1 );
  carto::volumeutil::applyTowards( *r1, *r2, *r, 
        carto::volumeutil::not_equal_to<%Template1%, %Template1% >() );
  
  sipRes = r.get();
  r.release();
  r1.release();
  r2.release();
%End

%%Template1defScalar%%
%#ifdef PYAIMS_SCALAR%

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

/*
  Volume_%Template1typecode%* __rdiv__( double ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( sipCpp );
  carto::VolumeRef<%Template1% > r3 = a0 / r1;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End
*/

%#endif%

// numpy bindings

%%Template1defNumpyBindings%%
%#if defined( PYAIMS_SCALAR ) || defined( PYAIMS_NUMPY_BINDINGS) %

  Volume_%Template1typecode%( SIP_PYOBJECT )
    [(vector_S32, %Template1% *, const vector_S64 *)];
%MethodCode
  PyArrayObject *arr = 0;
  if( !PyArray_Check( a0 ) )
  {
    /* copy of the code of Volume( const std::vector<int32_t> & )
    */
    {
        const vector_S32* a0;
        int a0State = 0;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "J1", sipType_vector_S32, &a0, &a0State))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp = new sipVolume_%Template1typecode%(*a0,
              carto::AllocatorContext(), true);
            }
            catch (...)
            {
                Py_BLOCK_THREADS

            sipReleaseType(const_cast<vector_S32 *>(a0),sipType_vector_S32,a0State);
                sipRaiseUnknownException();
                return NULL;
            }
            Py_END_ALLOW_THREADS
            sipReleaseType(const_cast<vector_S32 *>(a0),sipType_vector_S32,a0State);

            sipCpp->sipPySelf = sipSelf;

            return sipCpp;
        }
    }

     sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "wrong argument type" );
  }
  else
  {
    static std::list<std::set<int> > compatibletypes;
    if( compatibletypes.empty() )
    {
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        tl.insert( NPY_BYTE );
        tl.insert( NPY_INT8 );
      }
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        tl.insert( NPY_UBYTE );
        tl.insert( NPY_UINT8 );
      }
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        tl.insert( NPY_SHORT );
        tl.insert( NPY_INT16 );
      }
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        tl.insert( NPY_USHORT );
        tl.insert( NPY_UINT16 );
      }
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        tl.insert( NPY_INT32 );
      }
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        // tl.insert( NPY_UINT );
        tl.insert( NPY_UINT32 );
      }
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        // tl.insert( NPY_INT );
        tl.insert( NPY_INT64 );
      }
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        tl.insert( NPY_UINT );
        tl.insert( NPY_UINT64 );
      }
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        tl.insert( NPY_FLOAT32 );
      }
      compatibletypes.push_back( std::set<int>() );
      {
        std::set<int> & tl = compatibletypes.back();
        // tl.insert( NPY_FLOAT );
        tl.insert( NPY_DOUBLE );
        tl.insert( NPY_FLOAT64 );
      }
    }

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

    if( PyArray_NDIM( arr ) < 1
        || PyArray_NDIM( arr ) > carto::Volume<%Template1%>::DIM_MAX )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError,
                       "Array dimensions are not compatible with Volume" );
    }
    else if( PyArray_DESCR( arr )->type_num != %Template1NumType% )
    {
      std::list<std::set<int> >::const_iterator ict, ect = compatibletypes.end();
      bool ok = false;
      int i = 0;
      for( ict=compatibletypes.begin(); ict!=ect; ++ict, ++i )
        if( ict->find( PyArray_DESCR( arr )->type_num ) != ict->end() )
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
    int nd = PyArray_NDIM( arr );
    if( nd < 4 )
      nd = 4;
    bool xyzorder = false;
    std::vector<int> dims( nd, 1 );
    int inc = 1, start = 0;

    std::vector<long> strides( nd, 1 );

    for( int i=0; i<PyArray_NDIM( arr ); ++i )
    {
      dims[i] = PyArray_DIMS( arr )[ start + inc * i];
      strides[i] = PyArray_STRIDES( arr )[ start + inc * i ]
        / long( sizeof( %Template1% ) );
    }

    sipCpp = new sipVolume_%Template1typecode%(
      dims, ( %Template1% *) PyArray_DATA( arr ), &strides );
    /* keep ref to the array to prevent its destruction
       WARNING: this is not enough in every situation: if the python object
       is returned to C++, then the python attribute will be deleted and the
       numpy array will be released, thus it can be destroyed while the Volume
       is still living.
    */
    PyObject_SetAttrString( (PyObject *) sipSelf, "_arrayext", a0 );
  }
  else
  {
    sipIsErr = 1;
    sipCpp = 0;
  }

%End

  SIP_PYOBJECT arraydata() /Deprecated,Factory/;
%Docstring
.. note::

    *arraydata()* returns a numpy array to the internal memory block, with "inverted" shape and strides.

    **WARNING:** this is an obsolete method, which behaviour has changed. Most of the time you need `numpy.asarray(volume)`, or more simply: `volume.np`, which actually provides an array with the same indices ordering. Here it is a transposed one.

    Given the internal ordering of Aims Volumes, the resulting numpy array is indexed as [t][z][y][x]. This order corresponds to the numpy "fortran" order: ``order='F'``
    If you need the inverse, more natural, [x][y][z][t] ordering, use the following:

    >>> volarray = volume.np

    or:

    >>> volarray = numpy.array(volume, copy=False)

    or:

    >>> volarray = numpy.asarray(volume)

    Using arraydata(), as the indices are reversed, if you do something like::

        vol2 = aims.Volume(vol.arraydata())

    You will build a transposed volume.
%End

%MethodCode
  std::vector<int> vdims = sipCpp->getSize();
  int i, n= vdims.size();
  std::vector<long> vstrides, strides( n );
  std::vector<int> dims( n );
  carto::rc_ptr<Volume_%Template1typecode% > ref = sipCpp->refVolume();

  vstrides = sipCpp->getStrides();

  for( i=0; i<n; ++i )
  {
    dims[i] = vdims[i];
    strides[i] = vstrides[i] * sizeof( %Template1% );
  }

  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  for( i=0; i<added_dims.size(); ++i )
    strides.insert( strides.end(), sizeof( %Template1% ) / added_dims[i] ); // FIXME

  PyArray_Descr *descr = %Template1NumType_Descr%;
  if( !descr )
    descr = PyArray_DescrFromType( %Template1NumType% );
  sipRes = aims::initNumpyArray( sipSelf, descr,
                                 dims.size(), &dims[0],
                                 (char *) &sipCpp->at( 0 ),
                                 false,
                                 &strides[0] );
%End

  SIP_PYOBJECT __array__() /Factory/;
%MethodCode
  std::vector<int> vdims = sipCpp->getSize();
  int i, n= vdims.size();
  std::vector<long> vstrides, strides( n );
  std::vector<int> dims( n );
  carto::rc_ptr<Volume_%Template1typecode% > ref = sipCpp->refVolume();

  vstrides = sipCpp->getStrides();

  for( i=0; i<n; ++i )
  {
    dims[i] = vdims[i];
    strides[i] = vstrides[i] * sizeof( %Template1% );
  }

  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  for( i=0; i<added_dims.size(); ++i )
    strides.insert( strides.end(), sizeof( %Template1% ) / added_dims[i] ); // FIXME

  PyArray_Descr *descr = %Template1NumType_Descr%;
  if( !descr )
    descr = PyArray_DescrFromType( %Template1NumType% );
  sipRes = aims::initNumpyArray( sipSelf, descr,
                                 dims.size(), &dims[0],
                                 (char *) &sipCpp->at( 0 ),
                                 true,
                                 &strides[0] );
%End

  void checkResize();
%MethodCode
  std::vector<int> vdims = sipCpp->getSize();
  int i, n= vdims.size();
  std::vector<long> vstrides, strides( n );
  std::vector<int> dims( n );
  carto::rc_ptr<Volume_%Template1typecode% > ref = sipCpp->refVolume();

  vstrides = sipCpp->getStrides();

  for( i=0; i<n; ++i )
  {
    dims[i] = vdims[i];
    strides[i] = vstrides[i] * sizeof( %Template1% );
  }

  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  for( i=0; i<added_dims.size(); ++i )
    strides.insert( strides.end(), sizeof( %Template1% ) / added_dims[i] ); // FIXME

  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &sipCpp->at( 0 ), &strides[0] );
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

%#endif%

};


%#ifdef PYAIMS_SCALAR%

Volume_%Template1typecode% * operator + ( %Template1%, Volume_%Template1typecode% & ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a1 );
  carto::VolumeRef<%Template1% > r3 = r1 + a0;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

Volume_%Template1typecode% * operator - ( %Template1%, Volume_%Template1typecode% & ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a1 );
  carto::VolumeRef<%Template1% > r3 = -r1 + a0;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

Volume_%Template1typecode% * operator * ( double, Volume_%Template1typecode% & ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a1 );
  carto::VolumeRef<%Template1% > r3 = r1 * a0;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

Volume_%Template1typecode% * operator * ( long, Volume_%Template1typecode% & ) /Factory, ReleaseGIL/;
%MethodCode
  carto::VolumeRef<%Template1% > r1( a1 );
  carto::VolumeRef<%Template1% > r3 = r1 * a0;
  r1.release();
  sipRes = r3.get();
  r3.release();
%End

%#endif%

%#undef PyAims_Volume_%Template1typecode%_defined%

