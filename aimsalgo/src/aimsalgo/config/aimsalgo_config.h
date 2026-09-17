
#ifndef AIMS_AIMSALGO_CONFIG_H
#define AIMS_AIMSALGO_CONFIG_H

#include <cartobase/config/cartobase_config.h>

#if defined( _WIN32 ) && !defined( __GNUC__ )
#ifdef AIMSALGO_EXPORTS
#define AIMSALGO_API __declspec(dllexport)
#else
#define AIMSALGO_API __declspec(dllimport)
#endif
#else // _WIN32
#define AIMSALGO_API
#endif // _WIN32

#endif
