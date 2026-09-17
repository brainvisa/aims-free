#include <iostream>
#include <limits>

// included after <iostream> to avoid a macro conflict in python on MacOS 10.11
#include <pyaims/object/numconv.h>

namespace carto
{

  uint32_t uint32_FromPy( PyObject *pyobj )
  {
    unsigned long x = PyLong_AsUnsignedLong( pyobj );
    if( x == static_cast<unsigned long>( -1 ) )
    {
      PyErr_Clear();
      double y = PyFloat_AsDouble( pyobj );
      if( y != -1.0 || !PyErr_Occurred() )
        x = static_cast<uint32_t>( round( y ) );
    }
    // Handle the case where unsigned long is larger than 32 bit
    if(x <= std::numeric_limits<uint32_t>::max())
      return x;
    else
      return -1;
  }

}

