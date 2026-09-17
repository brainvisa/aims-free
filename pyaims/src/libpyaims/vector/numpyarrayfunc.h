#ifndef PYAIMS_VECTOR_NUMPYARRAYFUNC_H
#define PYAIMS_VECTOR_NUMPYARRAYFUNC_H

#include <pyaims/numpyarray.h>
#include <pyaims/vector/numpytypes.h>

namespace aims
{
  PyObject* initNumpyArray( PyObject*, PyArray_Descr *numType, int ndim,
                            int* dims,
                            char* buffer, bool xyzorder = true,
                            long *strides = 0 );
  void resizeNumpyArray( PyObject*, int ndim, int* dims, char* buffer,
                         long *strides = 0 );
  void releaseNumpyArray( PyObject* );
}

#endif
