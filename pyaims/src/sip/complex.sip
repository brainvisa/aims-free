%MappedType cdouble
{
%TypeHeaderCode
#include <cartobase/type/types.h>
%End

%ConvertToTypeCode
  // Allow a Python complex whenever a cdouble is expected.

  if (sipIsErr == NULL)
    return PyComplex_Check( sipPy );

  if( sipPy == Py_None )
  {
    *sipCppPtr = new cdouble;

    return sipGetState( sipTransferObj );
  }

  *sipCppPtr = new cdouble( PyComplex_RealAsDouble( sipPy ),
    PyComplex_ImagAsDouble( sipPy ) );
  return sipGetState( sipTransferObj );
%End

%ConvertFromTypeCode
  // cdouble -> PyObject
  if( !sipCpp )
    return PyComplex_FromDoubles( 0, 0 );

  return PyComplex_FromDoubles( sipCpp->real(), sipCpp->imag() );
%End

};


%MappedType cfloat
{
%TypeHeaderCode
#include <cartobase/type/types.h>
%End

%ConvertToTypeCode
  // Allow a Python complex whenever a cfloat is expected.

  if (sipIsErr == NULL)
    return PyComplex_Check( sipPy );

  if( sipPy == Py_None )
  {
    *sipCppPtr = new cfloat;

    return sipGetState( sipTransferObj );
  }

  *sipCppPtr = new cfloat( (float) PyComplex_RealAsDouble( sipPy ),
    (float) PyComplex_ImagAsDouble( sipPy ) );
  return sipGetState( sipTransferObj );
%End

%ConvertFromTypeCode
  // cfloat -> PyObject
  if( !sipCpp )
    return PyComplex_FromDoubles( 0, 0 );

  return PyComplex_FromDoubles( sipCpp->real(), sipCpp->imag() );
%End

};


