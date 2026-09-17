#ifndef GRAPH_CONFIG_CONFIG_H
#define GRAPH_CONFIG_CONFIG_H

#include <cartobase/config/cartobase_config.h>

#if defined( _WIN32 ) && !defined( __GNUC__ )
#ifdef GRAPH_EXPORTS
#define GRAPH_API __declspec(dllexport)
#else
#define GRAPH_API __declspec(dllimport)
#endif
#else // _WIN32
#define GRAPH_API
#endif // _WIN32

#endif
