#ifndef PYAIMS_NUMPYARRAY_H
#define PYAIMS_NUMPYARRAY_H

#ifdef slots  // Qt defines this as a macro
#pragma push_macro("slots")
#define _slots_saved_
#undef slots
#endif

#include <Python.h>
#ifndef PY_ARRAY_UNIQUE_SYMBOL
#define PY_ARRAY_UNIQUE_SYMBOL PyAims_NumpyAPI
#define NPY_API_SYMBOL_ATTRIBUTE
#endif
#ifndef NO_IMPORT_ARRAY
#define NO_IMPORT_ARRAY
#endif
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

#ifdef _slots_saved_
#pragma pop_macro("slots")
#undef _slots_saved_
#endif

// numpy 1.x / 2.x compatibility
#if NPY_ABI_VERSION < 0x02000000
  #define PyDataType_ELSIZE(descr) ((descr)->elsize)
  #define PyDataType_SET_ELSIZE(descr, val) ((descr)->elsize = val)
#endif

#endif
