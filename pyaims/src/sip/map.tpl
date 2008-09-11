
%MappedType std::map<%Template1%, %Template2% >
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

  for (int i = 0; i < PyList_GET_SIZE(l); ++i)
    {
      item = PyList_GET_ITEM( l, i );
      (*qa)[ %Template1deref%(%Template1% %Template1deref%)
                   %Template1CFromPy%( PyTuple_GET_ITEM( item, 0 ) ) ]
        = %Template2deref%(%Template2% %Template2deref%)
                   %Template2CFromPy%( PyTuple_GET_ITEM( item, 1 ) );
 
      if (PyErr_Occurred() != NULL)
        {
          delete qa;
          *sipIsErr = 1;
          Py_DECREF( l );
          std::ostringstream msg;
          msg << "item " << i << " could not be converted to a pair of " 
                "%Template1%, %Template1%";
          PyErr_SetString( PyExc_TypeError, msg.str().c_str() );
          return 0;
        }
    }
 
  *sipCppPtr = qa;
  Py_DECREF( l );
 
  return 1;
%End
};

