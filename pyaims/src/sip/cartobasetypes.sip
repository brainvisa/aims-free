
%MappedType Void
{
%TypeHeaderCode
#include <cartobase/type/types.h>
%End

%ConvertToTypeCode
  // Allow Python None object whenever a Void is expected.

  if (sipIsErr == NULL)
    return sipPy == Py_None;

  *sipCppPtr = new Void;
  return 1;
%End

%ConvertFromTypeCode
  // Void -> None
  Py_INCREF( Py_None );
  return Py_None;
%End
};

