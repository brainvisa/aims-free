#include <pyaims/init.h>
#include <Python.h>
#define PY_ARRAY_UNIQUE_SYMBOL PyAims_NumpyAPI
#define NPY_API_SYMBOL_ATTRIBUTE
#include <numpy/arrayobject.h>

namespace
{
  /* import_array() is a macro which defines a block of code, which
     includes a conditional return statement. This return statement, in
     python 2, does not return any value, but in python 3, returns NULL.
     So we must use it as part of a function declaration, which conforms to
     this return policy.
   */
#if PY_VERSION_HEX >= 0x03000000
  void *npy_import_array()
  {
    import_array();
    return NULL;
  }
#else
  void npy_import_array()
  {
    import_array();
  }
#endif
}

namespace carto
{
  void pyaims_init(void)
  {
    npy_import_array();
  }
}
