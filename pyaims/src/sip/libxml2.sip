%ModuleHeaderCode
#include <libxml/parser.h>
%End

void xmlInitParser();
%MethodCode
  try {
    xmlInitParser();
  }
  catch( std::exception & e ) {
    sipIsErr = 1;
    PyErr_SetString( PyExc_RuntimeError, e.what() );
  }
%End
    