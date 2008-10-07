
// system-dependent definitions

%#if defined( __APPLE__ ) || defined( __LP64__ )%

typedef unsigned long size_t;
typedef vector_ULONG vector_SIZE_T;

%#else%

typedef unsigned size_t;
typedef vector_U32 vector_SIZE_T;

%#endif%

