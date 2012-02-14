
class Texture_%Template1typecode%
{
%TypeHeaderCode
#include <aims/mesh/texture.h>
#ifndef PYAIMS_TEXTURE_%Template1typecode%_DEFINED
#define PYAIMS_TEXTURE_%Template1typecode%_DEFINED
typedef Texture<%Template1% > Texture_%Template1typecode%;
#endif
#if SIP_VERSION < 0x040700
#include "sipaimssipvector_%Template1typecode%.h"
#endif
#include <weakrefobject.h>
#ifndef PY_ARRAY_TYPES_PREFIX
#define PY_ARRAY_TYPES_PREFIX PyAIMSTypes
#include <pyaims/numpyarray.h>
#endif
%End

%TypeCode
#include <pyaims/vector/numpyarrayfunc.h>
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy,
    sipClass_Texture_%Template1typecode%,
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

// const vector_%Template1typecode% & data() const;
  vector_%Template1typecode% & data();
  void erase();

  unsigned size() const;
%MethodCode
  sipRes = sipCpp->nItem();
%End

  int __len__() const;
%MethodCode
  sipRes = (int) sipCpp->nItem();
%End

  void append( const %Template1PyType% & );
%MethodCode
  sipCpp->push_back( %Template1deref%( a0 ) );
  int dims = sipCpp->nItem();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[ 0 ] );
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

  SIP_PYLIST list() const;
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


%%Template1defScalar%%
%%Template1defNumpyBindings%%
%#if defined( PYAIMS_SCALAR ) || defined( PYAIMS_NUMPY_BINDINGS )%

  Texture_%Template1typecode%( SIP_PYOBJECT ) [(int)];
%MethodCode
  bool done = false;
  if( PyArray_Check( a0 ) )
  {
    PyArrayObject *arr = 0;
    arr = (PyArrayObject *) a0;
    if( arr->nd < 0 || arr->nd >1 )
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
      int dims = arr->dimensions[0], i;

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
          break;
        }

        (*sipCpp)[i] = %Template1pyderef% %Template1castFromSip%
                       %Template1CFromPy%( pyitem );
      }
    }
  }

%End

  SIP_PYOBJECT arraydata() /Factory/;
%MethodCode
  int dims = sipCpp->nItem();
  sipRes = aims::initNumpyArray( sipSelf, %Template1NumType%, 1, &dims,
                                 (char *) &sipCpp->item( 0 ) );
%End

  void checkResize();
%MethodCode
  int dims = sipCpp->nItem();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[ 0 ] );
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
  int dims = sipCpp->data().size();
  aims::resizeNumpyArray( sipSelf, 1, &dims, (char *) &(*sipCpp)[0] );
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
    sipClass_TimeTexture_%Template1typecode%,
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

};

