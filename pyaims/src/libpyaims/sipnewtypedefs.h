#ifndef PYAIMS_SIPNEWTYPEDEFS_H
#define PYAIMS_SIPNEWTYPEDEFS_H

#include <sip.h>

#if SIP_ABI_MAJOR_VERSION < 13
typedef long Aims_hash_t;
#else
typedef Py_hash_t Aims_hash_t;
#endif

#endif
