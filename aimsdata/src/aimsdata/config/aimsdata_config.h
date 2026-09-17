#ifndef AIMS_AIMSDATA_CONFIG_H
#define AIMS_AIMSDATA_CONFIG_H

#include <cartobase/config/cartobase_config.h>
#include <aims/config/config.h>

#if defined( _WIN32 ) && !defined( __GNUC__ )
#ifdef AIMSDATA_EXPORTS
#define AIMSDATA_API __declspec(dllexport)
#else
#define AIMSDATA_API __declspec(dllimport)
#endif
#else // _WIN32
#define AIMSDATA_API
#endif // _WIN32

#if defined(_MSC_VER) && !defined(max)
#define max _cpp_max
#endif

#ifdef __APPLE__
#ifdef MAP_COPY
#undef MAP_COPY
#endif
#endif // __APPLE__

#endif
