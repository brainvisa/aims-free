
class Texture_%Template1typecode%
{
%TypeHeaderCode
#include <aims/mesh/texture.h>
#ifndef PYAIMS_TEXTURE_%Template1typecode%_DEFINED
#define PYAIMS_TEXTURE_%Template1typecode%_DEFINED
typedef Texture<%Template1% > Texture_%Template1typecode%;
#endif
#include <weakrefobject.h>
#ifndef PY_ARRAY_TYPES_PREFIX
#define PY_ARRAY_TYPES_PREFIX PyAIMSTypes
#include <pyaims/numpyarray.h>
#endif
%Template1sipinclude%
%End

%TypeCode
#include <pyaims/vector/numpyarrayfunc.h>
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy,
    sipFindType( "Texture_%Template1typecode%" ),
    sipTransferObj, sipIsErr, sipCppPtr );
%End


public:

  Texture_%Template1typecode%();
  Texture_%Template1typecode%( unsigned );
  virtual ~Texture_%Template1typecode%();
  unsigned nItem() const;
  %Template1PyType% item( int ) const;

  void reserve( unsigned );
  void push_back( const %Template1PyType% & );
%MethodCode
  sipCpp->data().push_back( %Template1deref% a0 );

  std::vector<int> dims( 1, sipCpp->data().size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &sipCpp->data()[0] );
%End

  void push_back( unsigned, const %Template1PyType% & );
%MethodCode
  sipCpp->data().insert( sipCpp->data().end(), a0,
    static_cast<const vector_%Template1typecode%::value_type &>(
      const_cast<%Template1PyType% &>( %Template1deref%a1 ) ) );
  std::vector<int> dims( 1, sipCpp->data().size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &sipCpp->data()[0] );
%End

  void resize( unsigned );
%MethodCode
  sipCpp->data().resize( a0 );
  std::vector<int> dims( 1, sipCpp->data().size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &sipCpp->data()[0] );
%End

  void resize( unsigned, const %Template1PyType% & );
%MethodCode
  sipCpp->data().resize( a0,
    static_cast<const vector_%Template1typecode%::value_type &>(
      const_cast<%Template1PyType% &>( %Template1deref%a1 ) )  );
  std::vector<int> dims( 1, sipCpp->data().size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &sipCpp->data()[0] );
%End

// const vector_%Template1typecode% & data() const;
  vector_%Template1typecode% & data();
  void erase();

  unsigned size() const;
%MethodCode
  sipRes = sipCpp->nItem();
%End

  Py_ssize_t __len__() const;
%MethodCode
  sipRes = (int) sipCpp->nItem();
%End

  void append( const %Template1PyType% & );
%MethodCode
  sipCpp->push_back( %Template1deref%( a0 ) );
  std::vector<int> dims( 1, sipCpp->data().size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &sipCpp->data()[0] );
%End

  %Template1PyType% __getitem__( int );
%MethodCode
  sipRes = %Template1address%(*sipCpp)[ a0 ];
%End

  void __setitem__( int, const %Template1PyType% & );
%MethodCode
  (*sipCpp)[ a0 ] = %Template1deref%( a1 );
%End

  int __contains__( const %Template1PyType% & );
%MethodCode
  unsigned i, n = sipCpp->nItem();
  for( i=0; i<n; ++i )
    if( (*sipCpp)[ i ] == %Template1deref%( a0 ) )
      break;
  sipRes = (i != n);
%End

  SIP_PYLIST list();
%MethodCode
  // Convert to a Python list of %Template1%.

  // Create the list.
  if( (sipRes = PyList_New(sipCpp->nItem())) == NULL )
    return NULL;

  // Get it.
  unsigned n = sipCpp->nItem();
  for( unsigned i = 0; i < n; ++i )
    if( PyList_SetItem(sipRes,i,
        %Template1pyFromC%(%Template1address%(sipCpp->item(i))))
        < 0 )
      {
        Py_DECREF(sipRes);
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_TypeError, "item conversion failed" );
        break;
      }
%End

  bool operator == ( const Texture_%Template1typecode% & ) const /ReleaseGIL/;

%%Template1defScalar%%
%%Template1defNumpyBindings%%
%#if defined( PYAIMS_SCALAR ) || defined( PYAIMS_NUMPY_BINDINGS )%
%%Template1defNumpyIsSubArray%%

  Texture_%Template1typecode%( SIP_PYOBJECT ) [(int)];
%MethodCode
  bool done = false;
  if( PyArray_Check( a0 ) )
  {
    PyArrayObject *arr = 0;
    arr = (PyArrayObject *) a0;
    if( PyArray_NDIM( arr ) < 0 || PyArray_NDIM( arr ) >1 )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, 
                       "Array dimensions are not compatible with Texture" );
      done = true; 
    }
    else if( PyArray_TYPE(arr) != %Template1NumType% )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_TypeError, "wrong array data type" );
    }
    if( !sipIsErr )
    {
      // retreive dimensions
      int dims = PyArray_DIMS( arr )[0], i;

      sipCpp = new sipTexture_%Template1typecode%( dims );
      for( i=0; i<dims; ++i )
        (*sipCpp)[i] = ((%Template1% *)( PyArray_DATA(arr) ))[i];
      done = true; 
    }
  }
  if( !done )
  {
    if( !PySequence_Check( a0 ) )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_TypeError, "wrong argument type" );
    }
    else
    {
      unsigned	i, n = PySequence_Size( a0 );
      sipCpp = new sipTexture_%Template1typecode%( n );
      PyObject	*pyitem;
      for( i=0; i<n; ++i )
      {
        pyitem = PySequence_GetItem(a0,i);
        if( !pyitem || !%Template1testPyType%( pyitem ) )
        {
          delete sipCpp;
          sipCpp = 0;
          std::ostringstream s;
          s << "wrong list item type, item " << i;
          if( !sipIsErr )
          {
            sipIsErr = 1;
            PyErr_SetString( PyExc_TypeError, s.str().c_str() );
          }
          if( pyitem )
            Py_DECREF( pyitem );
          break;
        }

        (*sipCpp)[i] = %Template1pyderef% %Template1castFromSip%
                       %Template1CFromPy%( pyitem );
        Py_DECREF( pyitem );
      }
    }
  }

%End

  SIP_PYOBJECT arraydata() /Factory/;
%MethodCode
  std::vector<int> dims( 1, sipCpp->nItem() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );

  PyArray_Descr *descr = %Template1NumType_Descr%;
  if( !descr )
    descr = PyArray_DescrFromType( %Template1NumType% );
  sipRes = aims::initNumpyArray( sipSelf, descr, dims.size(),
                                 &dims[0],
                                 (char *) &sipCpp->item( 0 ) );
%#ifdef PYAIMS_NPY_IS_SUBARRAY%
  PyObject *sub_arr = PyMapping_GetItemString( sipRes,
                                               const_cast<char *>( "v" ) );
  if( sub_arr )
  {
    Py_DECREF( sipRes ); // we don't use the whole array
    sipRes = sub_arr;
  }
%#endif%
%End

  SIP_PYOBJECT __array__() /Factory/;
%MethodCode
  std::vector<int> dims( 1, sipCpp->nItem() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );

  PyArray_Descr *descr = %Template1NumType_Descr%;
  if( !descr )
    descr = PyArray_DescrFromType( %Template1NumType% );
  sipRes = aims::initNumpyArray( sipSelf, descr, dims.size(),
                                 &dims[0],
                                 (char *) &sipCpp->item( 0 ) );
%#ifdef PYAIMS_NPY_IS_SUBARRAY%
  PyObject *sub_arr = PyMapping_GetItemString( sipRes,
                                               const_cast<char *>( "v" ) );
  if( sub_arr )
  {
    Py_DECREF( sipRes ); // we don't use the whole array
    sipRes = sub_arr;
  }
%#endif%
%End


  SIP_PYOBJECT array_struct() /Factory/;
%MethodCode
  std::vector<int> dims( 1, sipCpp->nItem() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );

  PyArray_Descr *descr = %Template1NumType_Descr%;
  if( !descr )
    descr = PyArray_DescrFromType( %Template1NumType% );
  sipRes = aims::initNumpyArray( sipSelf, descr, dims.size(),
                                 &dims[0],
                                 (char *) &sipCpp->item( 0 ) );
%End

  void checkResize();
%MethodCode
  std::vector<int> dims( 1, sipCpp->data().size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &sipCpp->data()[0] );
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

  // assignation operator: uses copy operator
  void assign( const vector_%Template1typecode% & );
%MethodCode
  sipCpp->data() = *a0;
  std::vector<int> dims( 1, sipCpp->data().size() );
  std::vector<int> added_dims = %Template1NumDims%;
  dims.insert( dims.end(), added_dims.begin(), added_dims.end() );
  aims::resizeNumpyArray( sipSelf, dims.size(), &dims[0],
                          (char *) &sipCpp->data()[0] );
%End

%#endif%
};


class TimeTexture_%Template1typecode% : carto::RCObject
{
%TypeHeaderCode
#include <aims/mesh/texture.h>
#ifndef PYAIMS_TIMETEXTURE_%Template1typecode%_DEFINED
#define PYAIMS_TIMETEXTURE_%Template1typecode%_DEFINED
typedef TimeTexture<%Template1% > TimeTexture_%Template1typecode%;
#endif
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy,
    sipFindType( "TimeTexture_%Template1typecode%" ),
    sipTransferObj, sipIsErr, sipCppPtr );
%End


public:

  TimeTexture_%Template1typecode%();
  TimeTexture_%Template1typecode%( int, unsigned );
  virtual  ~TimeTexture_%Template1typecode%();

  unsigned size() const;

  Texture_%Template1typecode% & operator [] ( int );
%MethodCode
  sipRes = &(*sipCpp)[ a0 ];
%End

  void __setitem__ ( int, const Texture_%Template1typecode% & );
%MethodCode
  (*sipCpp)[ a0 ] = *a1;
%End

  bool has_key( int ) const;
%MethodCode
  sipRes = ( sipCpp->find( a0 ) != sipCpp->end() );
%End

  Py_ssize_t __len__() const;
%MethodCode
  sipRes = sipCpp->size();
%End

  SIP_PYLIST keys() const;
%MethodCode
  if( (sipRes = PyList_New(sipCpp->size())) == NULL )
    return NULL;

  TimeTexture_%Template1typecode%::const_iterator it, et = sipCpp->end();
  unsigned i = 0;

  for( it=sipCpp->begin(); it!=et; ++it, ++i )
#if PY_VERSION_HEX >= 0x03000000
    if( PyList_SetItem( sipRes, i, PyLong_FromLong( it->first ) ) < 0 )
#else
    if( PyList_SetItem( sipRes, i, PyInt_FromLong( it->first ) ) < 0 )
#endif
      {
        Py_DECREF(sipRes);
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_TypeError, "item conversion failed" );
        break;
      }
%End

  // const carto::GenericObject & header() const;
  carto::GenericObject & header();

/*
  void setHeader( const carto::GenericObject & );
%MethodCode
  sipCpp->setHeader( aims::PythonHeader( *a0 ) );
%End
*/

  unsigned nItem() const;
  void reserve( unsigned );
  void push_back( const %Template1PyType% & );
  void erase();

  // conversion from Object
  static  rc_ptr_TimeTexture_%Template1typecode%
  fromObject( carto::GenericObject ) /Factory/;
%MethodCode
  try
    {
      carto::rc_ptr<TimeTexture_%Template1typecode%> & obj 
        = a0->value<carto::rc_ptr<TimeTexture_%Template1typecode%> >();
      sipRes = new carto::rc_ptr<TimeTexture_%Template1typecode%>( obj );
    }
  catch( std::exception & e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
%End

  bool operator == ( const TimeTexture_%Template1typecode% & ) const /ReleaseGIL/;
};

