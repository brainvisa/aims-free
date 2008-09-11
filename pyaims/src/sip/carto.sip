
%ModuleHeaderCode
#include <Python.h>
%End

namespace carto
{
	SIP_PYOBJECT PyObjectfromObject( carto::GenericObject &);
%MethodCode
	try
	{
		sipRes = a0->value<PyObject *>();
		Py_INCREF(sipRes);
	}
	catch( std::exception & e )
	{
		sipIsErr = 1;
		PyErr_SetString( PyExc_RuntimeError, e.what() );
		sipRes = 0;
	}
%End

};

