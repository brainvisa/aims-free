
%Exception std::exception(SIP_Exception) /PyName=StdException/
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


%Exception std::runtime_error(std::exception) /PyName=RuntimeError/
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


%Exception std::invalid_argument(std::exception) /PyName=ValueError/
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


%Exception carto::io_error(std::runtime_error) /PyName=IOError/
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


%Exception carto::assert_error(std::exception) /PyName=AssertionError/
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


%Exception carto::errno_error(std::runtime_error) /PyName=ErrnoError/
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


%Exception carto::file_error(std::runtime_error) /PyName=FileError/
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


