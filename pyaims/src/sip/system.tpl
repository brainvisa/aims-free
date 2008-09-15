
// system-dependent definitions

%#if defined( __APPLE__ ) || defined( __LP64__ )%

typedef unsigned long size_t;

%#else%

typedef unsigned size_t;

%#endif%

