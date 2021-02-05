%ModuleHeaderCode
#include <aims/math/hypergeometric.h>
#define PY_ARRAY_TYPES_PREFIX PyAIMSTypes
#include <pyaims/numpyarray.h>
%End

double hyp0f1(double a, SIP_PYOBJECT x, int MAX);
%MethodCode
	PyArrayObject	*a = NULL;
	
	if ((a1 == Py_None) or !PyArray_Check(a1))
	{
		sipIsErr = 1;
		PyErr_SetString(PyExc_TypeError, "wrong argument type for x");
	} else
	{	
		a = (PyArrayObject *) a1;
		if (!PyArray_ISCONTIGUOUS(a))
		{
			sipIsErr = 1;
			PyErr_SetString( PyExc_RuntimeError,
				"x must be a contiguous array");
      		} else {
			double	*x = NULL;
			int n = PyArray_DIMS( a )[0];
    			x = (double *) PyArray_DATA( a );
			sipRes = hyp0f1(a0, x, n, a2);
		}
	}
%End

double hyp1f1(double a, double b, SIP_PYOBJECT x, int MAX);
%MethodCode
	PyArrayObject	*a = NULL;
	
	if ((a2 == Py_None) or !PyArray_Check(a2))
	{
		sipIsErr = 1;
		PyErr_SetString(PyExc_TypeError, "wrong argument type for x");
	} else
	{	
		a = (PyArrayObject *) a2;
		if (!PyArray_ISCONTIGUOUS(a))
		{
			sipIsErr = 1;
			PyErr_SetString( PyExc_RuntimeError,
				"x must be a contiguous array");
      		} else {
			double	*x = NULL;
			int n = PyArray_DIMS( a )[0];
    			x = (double *) PyArray_DATA( a );
			sipRes = hyp1f1(a0, a1, x, n, a3);
		}
	}
%End

double loghyp0f1(double a, SIP_PYOBJECT x, int MAX);
%MethodCode
	PyArrayObject	*a = NULL;
	
	if ((a1 == Py_None) or !PyArray_Check(a1))
	{
		sipIsErr = 1;
		PyErr_SetString(PyExc_TypeError, "wrong argument type for x");
	} else
	{	
		a = (PyArrayObject *) a1;
		if (!PyArray_ISCONTIGUOUS(a))
		{
			sipIsErr = 1;
			PyErr_SetString( PyExc_RuntimeError,
				"x must be a contiguous array");
      		} else {
			double	*x = NULL;
			int n = PyArray_DIMS( a )[0];
    			x = (double *) PyArray_DATA( a );
			sipRes = loghyp0f1(a0, x, n, a2);
		}
	}
%End

double loghyp1f1(double a, double b, SIP_PYOBJECT x, int MAX);
%MethodCode
	PyArrayObject	*a = NULL;
	
	if ((a2 == Py_None) or !PyArray_Check(a2))
	{
		sipIsErr = 1;
		PyErr_SetString(PyExc_TypeError, "wrong argument type for x");
	} else
	{	
		a = (PyArrayObject *) a2;
		if (!PyArray_ISCONTIGUOUS(a))
		{
			sipIsErr = 1;
			PyErr_SetString( PyExc_RuntimeError,
				"x must be a contiguous array");
      		} else {
			double	*x = NULL;
			int n = PyArray_DIMS( a )[0];
    			x = (double *) PyArray_DATA( a );
			sipRes = loghyp1f1(a0, a1, x, n, a3);
		}
	}
%End
