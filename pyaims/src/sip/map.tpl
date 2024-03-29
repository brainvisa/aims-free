
%#if '%Template1needs_typedef%' == '1' %

// 1. template1 == size_t

/* This bidouille is needed because of a probable bug in sip 4.19.15, which 
   doesn't allow size_t in a MappedType. So we use a typedef here.
*/

%#if '%Template2needs_typedef%' == '1' %

// 1.1. tempalte1 == template1 == size_t

%ModuleHeaderCode
#ifndef AIMS_MAP_SIZE_T_SIZE_T_DEFINED
#define AIMS_MAP_SIZE_T_SIZE_T_DEFINED
#include <map>
typedef std::map<%Template1%, %Template2% > map_SIZE_T_SIZE_T;
#endif
%End

%MappedType map_SIZE_T_SIZE_T

// end 1.1

%#else%

// 1.2. template1 == size_t

%ModuleHeaderCode
#ifndef AIMS_MAP_SIZE_T_%Template2typecode%_DEFINED
#define AIMS_MAP_SIZE_T_%Template2typecode%_DEFINED
#include <map>
%Template2sipinclude%
typedef std::map<%Template1%, %Template2% > map_SIZE_T_%Template2typecode%;
#endif
%End

%MappedType map_SIZE_T_%Template2typecode%

// end 1.2

%#endif%

// end 1.

%#else%

// 2. template1 != size_t

%#if '%Template2needs_typedef%' == '1' %

// 2.1. template2 == size_t

/* This bidouille is needed because of a probable bug in sip 4.19.15, which
   doesn't allow size_t in a MappedType. So we use a typedef here.
*/

%ModuleHeaderCode
#ifndef AIMS_MAP_%Template1typecode%_SIZE_T_DEFINED
#define AIMS_MAP_%Template1typecode%_SIZE_T_DEFINED
#include <map>
%Template1sipinclude%
typedef std::map<%Template1%, %Template2% > map_%Template1typecode%_SIZE_T;
#endif
%End

%MappedType map_%Template1typecode%_SIZE_T

// end 2.1

%#else%

// 2.2. no size_t

%MappedType std::map<%Template1%, %Template2% >

%#endif%
%#endif%

{
%TypeHeaderCode
#include <map>
%Template1typeinclude%
%Template1sipinclude%
%Template2typeinclude%
%Template2sipinclude%
#include <cartobase/stream/sstream.h>
%End

%ConvertFromTypeCode

  if (!sipCpp)
    return PyDict_New();

  // Convert to a Python dict of %Template1% : %Template2%.

  PyObject *l;

  // Create the dict.

  if ((l = PyDict_New() ) == NULL)
    return NULL;

  // Get it.

  std::map<%Template1%, %Template2% >::iterator i, e = sipCpp->end();
  for( i=sipCpp->begin(); i!=e; ++i )
    if( PyDict_SetItem(l,%Template1pyFromC%(%Template1address%(i->first) ), 
                         %Template2pyFromC%(%Template2address%(i->second) ) ) 
        < 0 )
      {
        Py_DECREF(l);

        return NULL;
      }

  return l;
%End

%ConvertToTypeCode
   // Convert a Python dict of %Template1% to a map<%Template1%, %Template2% >
   // on the heap.
 
   if (sipIsErr == NULL)
     return PyMapping_Check(sipPy);

  if (sipPy == Py_None)
    {
      *sipCppPtr = NULL;

      return 0;
    }

  std::map<%Template1%, %Template2% > *qa 
    = new std::map<%Template1%, %Template2% >;

  PyErr_Clear();
  PyObject *l = PyMapping_Items( sipPy ), *item;
  bool err = false;

  for (int i = 0; i < PyList_GET_SIZE(l); ++i)
    {
      item = PyList_GET_ITEM( l, i );
      PyObject *value = PyTuple_GET_ITEM( item, 1 );
      if( !%Template2testPyType%( value ) )
        err = true;
      else
        (*qa)[ %Template1deref%(%Template1% %Template1deref%)
                      %Template1CFromPy%( PyTuple_GET_ITEM( item, 0 ) ) ]
            = %Template2deref%(%Template2% %Template2deref%)
                   %Template2CFromPy%( value );

      if( err || (PyErr_Occurred() != NULL) )
      {
        delete qa;
        *sipIsErr = 1;
        Py_DECREF( l );
        std::ostringstream msg;
        msg << "item " << i << " could not be converted to a pair of "
              "%Template1%, %Template2%";
        PyErr_SetString( PyExc_TypeError, msg.str().c_str() );
        return 0;
      }
    }
 
  *sipCppPtr = qa;
  Py_DECREF( l );
 
  return 1;
%End
};

