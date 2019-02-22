
// system-dependent definitions

%#if defined( __APPLE__ ) || defined( __LP64__ )%

// size_t is builtin in sip >= 4.19.14
%If ( - SIP_4_19_13 )
typedef unsigned long size_t;
%End

typedef vector_ULONG vector_SIZE_T;

%#elif defined( _WIN64 )%

typedef unsigned long long size_t;
typedef vector_ULONGLONG vector_SIZE_T;

%#else%

// size_t is builtin in sip >= 4.19.14
%If ( - SIP_4_19_13 )
typedef unsigned size_t;
%End

typedef vector_U32 vector_SIZE_T;

%#endif%
