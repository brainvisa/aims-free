
// system-dependent definitions

%#if defined( __APPLE__ ) || defined( __LP64__ )%

typedef unsigned long size_t;
typedef vector_ULONG vector_SIZE_T;

%#elif defined( _WIN64 )%

typedef unsigned long long size_t;
typedef vector_ULONGLONG vector_SIZE_T;

%#else%

typedef unsigned size_t;
typedef vector_U32 vector_SIZE_T;

%#endif%
