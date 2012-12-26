/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

#ifndef CARTOBASE_CONFIG_CARTOBASE_CONFIG_H
#define CARTOBASE_CONFIG_CARTOBASE_CONFIG_H

// define this to issue compiler warnings when using obsolete headers
#define CARTO_ATTRIBUTED_OBSOLETE_WARNING

#if defined( _WIN32 ) && !defined( __GNUC__ )
#ifdef CARTOBASE_EXPORTS
#define CARTOBASE_API __declspec(dllexport)
#else
#define CARTOBASE_API __declspec(dllimport)
#endif
#else // _WIN32
#define CARTOBASE_API
#endif // _WIN32

#if defined(_MSC_VER)
#  define CARTO_NO_PARTIAL_TEMPLATE_SPECIALIZATION 1
#elif defined(__BORLANDC__)
#  define CARTO_NO_MEMBER_TEMPLATE_FRIENDS 1
#  define CARTO_BROKEN_FRIEND_TEMPLATE_FUNCTION 1
#elif defined (__GNUC__)
#  if ( __GNUC__-0 == 2 )
#    define CARTO_USE_PRE_ISO_HEADERS
#    if ( __GNUC_MINOR__-0 == 95 )
#      define CARTO_NO_MEMBER_TEMPLATE_FRIENDS 1
#    endif
#  endif
#endif

// useful macro which is not always provided in the system
#ifndef __GLIBC_PREREQ
#if defined( __GLIBC__ ) && defined( __GLIBC_MINOR__ )
// taken from /usr/include/features.h in case it's not defined
#define __GLIBC_PREREQ(maj, min) \
	((__GLIBC__ << 16) + __GLIBC_MINOR__ >= ((maj) << 16) + (min))
#else
#define __GLIBC_PREREQ(maj, min) 1
#endif
#endif

// allow large files
#undef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGE_FILES
#define _LARGE_FILES
#endif

#if ( defined(_WIN32) && !defined( __GNUC__ ) ) || defined(__osf__)
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
#ifndef INT8_MIN
#define INT8_MIN   (-128)
#endif
#ifndef INT8_MAX
#define INT8_MAX   (127)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX  (255)
#endif
#ifndef INT16_MIN
#define INT16_MIN  (-32768)
#endif
#ifndef INT16_MAX
#define INT16_MAX  (32767)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX (65535)
#endif
#ifndef INT32_MIN
#define INT32_MIN  (-2147483648)
#endif
#ifndef INT32_MAX
#define INT32_MAX  (2147483647)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX (4294967295U)
#endif
#elif defined(__sun) || defined(__sgi)
#include <inttypes.h>
#else
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#endif

// isnan() portablility function or macro
#if defined( _WIN32 ) && !defined( isnan )
#ifdef __MINGW32__
#include <math.h>
#else
#include <float.h>
#define rint(x) floor(x+0.5)
#define isnan(x) _isnan(x)
#endif
#endif

// BUG On MacOS 10.2 / 10.3
#if defined( __APPLE__ )
#include <cmath>
#if (__GNUC__==3) && (__GNUC_MINOR__<=3)
#define      isnan( x )         ( ( sizeof ( x ) == sizeof(double) ) ?        \
                              __isnand ( x ) :                                \
                                ( sizeof ( x ) == sizeof( float) ) ?          \
                              __isnanf ( x ) :                                \
                              __isnan  ( x ) )
#else
// g++ > 3.3 (MacOS 10.4+)
using std::isnan;
using std::isinf;
#endif
#endif

#ifdef __MINGW32__
#define CARTO_BROKEN_EXTERN_TEMPLATES
#endif

#include <cartobase/config/config.h>

#endif // CARTOBASE_CONFIG_CARTOBASE_CONFIG_H


