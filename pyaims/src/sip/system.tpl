
// system-dependent definitions

%#if defined( __APPLE__ ) || defined( __LP64__ )%

// size_t is builtin in sip >= 4.19.14
// we can't use %If() because sip >= 4.19.14 generates a syntax error anyway
%#if SIP_VERSION < 0x04130D%
typedef unsigned long size_t;
%#endif%

%#elif defined( _WIN64 )%

typedef unsigned long long size_t;

%#else%

// size_t is builtin in sip >= 4.19.14
// we can't use %If() because sip >= 4.19.14 generates a syntax error anyway
%#if SIP_VERSION < 0x04130E%
typedef unsigned size_t;
%#endif%

%#endif%

int sip_module_version();
%MethodCode
  sipRes = SIP_VERSION;
%End

std::string sip_module();
%MethodCode
  sipRes = new std::string( SIP_MODULE );
%End

%#if SIP_VERSION < 0x060700%

typedef long Py_hash_t;

%#if SIP_VERSION < 0x060000%
typedef int Py_ssize_t;
%#endif%

%#endif%

