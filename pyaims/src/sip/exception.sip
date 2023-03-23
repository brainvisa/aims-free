
/*
%Exception std::exception(SIP_Exception) /PyName=StdException, Default/
{
%TypeHeaderCode
#include <exception>
%End
%RaiseCode
        const char *detail = sipExceptionRef.what();

        SIP_BLOCK_THREADS
        PyErr_SetString(PyExc_Exception, detail);
        SIP_UNBLOCK_THREADS
%End
};
*/

%Exception std::exception
{
%TypeHeaderCode
#include <exception>
%End
%RaiseCode
        const char *detail = sipExceptionRef.what();

        SIP_BLOCK_THREADS
        PyErr_SetString(PyExc_Exception, detail);
        SIP_UNBLOCK_THREADS
%End
};


%Exception std::runtime_error
{
%TypeHeaderCode
#include <stdexcept>
%End
%RaiseCode
        const char *detail = sipExceptionRef.what();
        SIP_BLOCK_THREADS
        PyErr_SetString(PyExc_RuntimeError, detail);
        SIP_UNBLOCK_THREADS
%End
};


%Exception std::invalid_argument
{
%TypeHeaderCode
#include <stdexcept>
%End
%RaiseCode
        const char *detail = sipExceptionRef.what();
        SIP_BLOCK_THREADS
        PyErr_SetString(PyExc_ValueError, detail);
        SIP_UNBLOCK_THREADS
%End
};


%Exception std::out_of_range
{
%TypeHeaderCode
#include <stdexcept>
%End
%RaiseCode
        const char *detail = sipExceptionRef.what();
        SIP_BLOCK_THREADS
        PyErr_SetString(PyExc_IndexError, detail);
        SIP_UNBLOCK_THREADS
%End
};


%Exception carto::io_error
{
%TypeHeaderCode
#include <cartobase/exception/ioexcept.h>
%End
%RaiseCode
        const char *detail = sipExceptionRef.what();
        SIP_BLOCK_THREADS
        PyErr_SetString(PyExc_IOError, detail);
        SIP_UNBLOCK_THREADS
%End
};


%Exception carto::assert_error
{
%TypeHeaderCode
#include <cartobase/exception/assert.h>
%End
%RaiseCode
        const char *detail = sipExceptionRef.what();
        SIP_BLOCK_THREADS
        if( !detail )
          PyErr_SetString(PyExc_AssertionError, "assertion failed" );
        else
          PyErr_SetString(PyExc_AssertionError, detail);
        SIP_UNBLOCK_THREADS
%End
};


%Exception carto::errno_error
{
%TypeHeaderCode
#include <cartobase/exception/errno.h>
#include <string.h>
%End
%RaiseCode
  const char *detail = sipExceptionRef.what();
  SIP_BLOCK_THREADS
  if( !detail )
  {
    char s[214] = "errno error: ";
    strerror_r( errno, s+13, 200 );
    PyErr_SetString(PyExc_RuntimeError, s );
  }
  else
  {
    PyErr_SetString(PyExc_RuntimeError, detail);
  }
  SIP_UNBLOCK_THREADS
%End
};


%Exception carto::file_error
{
%TypeHeaderCode
  #include <cartobase/exception/file.h>
%End
%RaiseCode
  const char *detail = sipExceptionRef.what();
  SIP_BLOCK_THREADS
  PyErr_SetString(PyExc_IOError, detail);
  SIP_UNBLOCK_THREADS
%End
};


