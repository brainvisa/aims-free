#include <pyaims/init.h>
#include <Python.h>
#define PY_ARRAY_UNIQUE_SYMBOL PyAims_NumpyAPI
#include <numpy/arrayobject.h>

namespace carto
{
	void    pyaims_init(void)
	{
		import_array();
	}
}
