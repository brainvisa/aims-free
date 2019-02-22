
// system-dependent definitions

%#if defined( __APPLE__ ) || defined( __LP64__ )%

// size_t is builtin in sip >= 4.19.14
// we can't use %If() because sip >= 4.19.14 generates a syntax error anyway
%#if SIP_VERSION < 0x04130D%
typedef unsigned long size_t;
%#endif%

typedef vector_ULONG vector_SIZE_T;

%#elif defined( _WIN64 )%

typedef unsigned long long size_t;
typedef vector_ULONGLONG vector_SIZE_T;

%#else%

// size_t is builtin in sip >= 4.19.14
// we can't use %If() because sip >= 4.19.14 generates a syntax error anyway
%#if SIP_VERSION < 0x04130D%
typedef unsigned size_t;
%#endif

typedef vector_U32 vector_SIZE_T;

%#endif%
