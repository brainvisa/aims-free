
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


