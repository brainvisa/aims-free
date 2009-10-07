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

#ifndef __zconfigl__
#define __zconfigl__ 1

// Ensure our definitions have been set up
#include "zconfigd.h"

// ==================================================
// Compiler

#ifndef ZCONFIG_ResourceCompilerInvoked
#	if defined(mw_rez) || defined(RC_INVOKED) || defined(BE_RES_COMPILE)
#		define ZCONFIG_ResourceCompilerInvoked 1
#	else
#		define ZCONFIG_ResourceCompilerInvoked 0
#	endif
#endif


#if !ZCONFIG_ResourceCompilerInvoked
#	ifndef ZCONFIG_Compiler
#		if defined(__MWERKS__)
#			define ZCONFIG_Compiler ZCONFIG_Compiler_CodeWarrior
#		elif defined(__GNUC__)
#			define ZCONFIG_Compiler ZCONFIG_Compiler_GCC
#		elif defined(_MSC_VER)
#			define ZCONFIG_Compiler ZCONFIG_Compiler_MSVC
#		endif
#	endif
#	ifndef ZCONFIG_Compiler
#		error "Don't know what compiler we're using."
#	endif
#endif

// ==================================================
// OS

#if defined(RC_INVOKED)
#	define ZCONFIG_OS ZCONFIG_OS_Win32
#elif defined(mw_rez)
#	define ZCONFIG_OS ZCONFIG_OS_MacOS7
#elif defined(BE_RES_COMPILE)
#	define ZCONFIG_OS ZCONFIG_OS_Be
#endif

#if !ZCONFIG_ResourceCompilerInvoked
#	ifndef ZCONFIG_OS
#		if defined(__MWERKS__)
#			if TARGET_API_MAC_CARBON
#				define ZCONFIG_OS ZCONFIG_OS_Carbon
#			elif __MACOS__
#				define ZCONFIG_OS ZCONFIG_OS_MacOS7
#			elif __INTEL__
#				define ZCONFIG_OS ZCONFIG_OS_Win32
#			elif __BEOS__
#				define ZCONFIG_OS ZCONFIG_OS_Be
#			endif
#		elif defined(__GNUC__)
#			if defined(__unix__) || defined( __APPLE__ )
#				define ZCONFIG_OS ZCONFIG_OS_POSIX
#			elif __BEOS__
#				define ZCONFIG_OS ZCONFIG_OS_Be
#			endif
#		elif defined(_MSC_VER)
#			define ZCONFIG_OS ZCONFIG_OS_Win32
#		endif
#	endif
#	ifndef ZCONFIG_OS
#		error "Don't know what OS we're using."
#	endif
#endif

// ==================================================
// Which variant of the STL

#if !ZCONFIG_ResourceCompilerInvoked
#	ifndef ZCONFIG_STL
#		if defined(__MWERKS__)
#			define ZCONFIG_STL ZCONFIG_STL_Modena
#		else
#			define ZCONFIG_STL ZCONFIG_STL_Unknown
#		endif
#	endif
#endif

// ==================================================
// Which processor?

#if !ZCONFIG_ResourceCompilerInvoked
#	ifndef ZCONFIG_Processor
#		if defined(__MWERKS__)
#			if __POWERPC__
#				define ZCONFIG_Processor ZCONFIG_Processor_PPC
#			elif __MC68K__ || defined(mw_rez)
#				define ZCONFIG_Processor ZCONFIG_Processor_68K
#			elif __INTEL__ || defined(RC_INVOKED)
#				define ZCONFIG_Processor ZCONFIG_Processor_x86
#			endif
#		elif defined(__GNUC__)
#			if #cpu(i386)
#				define ZCONFIG_Processor ZCONFIG_Processor_x86
#			elif #cpu(sparc)
#				define ZCONFIG_Processor ZCONFIG_Processor_Sparc
#			elif #cpu(powerpc) || defined( __POWERPC__ )
#				define ZCONFIG_Processor ZCONFIG_Processor_PPC
#			endif
#		elif defined(_MSC_VER)
#			define ZCONFIG_Processor ZCONFIG_Processor_x86
#		endif
#	endif
#	ifndef ZCONFIG_Processor
#		error "Don't know what processor we're using."
#	endif
#endif


// ==================================================
// Byte order

#ifndef ZCONFIG_Endian
#	if !ZCONFIG_ResourceCompilerInvoked && !defined(ZCONFIG_Endian)
#		if (ZCONFIG_Processor == ZCONFIG_Processor_x86)
#			define ZCONFIG_Endian ZCONFIG_Endian_Little
#		else
#			define ZCONFIG_Endian ZCONFIG_Endian_Big
#		endif
#	endif
#endif

// ==================================================
// We have two different defines for debugging. ZCONFIG_DebugLevel is what type of debugging
// is wanted for a debug build (one with SYM information being generated). 0 generally means
// almost no debug code will be generated, although it's legal to have assertions that are always
// present in the code, even in a level 0 build.
// Level 1 is for lighter weight debugging -- useful for code issued to external sites, code which has
// had all the serious panic situations handled for heavily-executed code, but which may still have
// lurking problems in less busy parts of the app. Level 2 is for development. Remember, these are
// suggested values only, and the ZDebug suite of macros lets you determine whether we should panic
// and kill the app, or just fprintf(stderr, ..) and go on our way.

#ifndef ZCONFIG_DebugLevel
#	define ZCONFIG_DebugLevel 1
#endif

// ZCONFIG_Debug takes its value from ZCONFIG_DebugLevel if we're doing a SYM build, otherwise it
// is set to 0. You can override ZCONFIG_Debug by setting its value in your zconfig.h before including this file.
#ifndef ZCONFIG_Debug
#	if defined(__MWERKS__)
#		if __option(sym) && !ZCONFIG_ResourceCompilerInvoked
#			define ZCONFIG_Debug ZCONFIG_DebugLevel
#		else
#			define ZCONFIG_Debug 0
#		endif
#	else
#		if defined(DEBUG)
#			define ZCONFIG_Debug ZCONFIG_DebugLevel
#		else
#			define ZCONFIG_Debug 0
#		endif
#	endif
#endif

#ifndef ZCONFIG_Debug
#	error "Can't figure out if we're debugging."
#endif

// ==================================================
// Which threads API?

#if !ZCONFIG_ResourceCompilerInvoked
#	ifndef ZCONFIG_API_Thread
#		if ZCONFIG_OS_MacOS7 == ZCONFIG_OS
#			define ZCONFIG_API_Thread ZCONFIG_API_Thread_Mac
#		elif ZCONFIG_OS_Carbon == ZCONFIG_OS
#			define ZCONFIG_API_Thread ZCONFIG_API_Thread_Mac
#		elif ZCONFIG_OS_Win32 == ZCONFIG_OS
#			define ZCONFIG_API_Thread ZCONFIG_API_Thread_Win32
#		elif ZCONFIG_OS_POSIX == ZCONFIG_OS
#			define ZCONFIG_API_Thread ZCONFIG_API_Thread_POSIX
#		elif ZCONFIG_OS_Be == ZCONFIG_OS
#			define ZCONFIG_API_Thread ZCONFIG_API_Thread_Be
#		endif
#	endif
#	ifndef ZCONFIG_API_Thread
#		error "Don't know what thread API we're using"
#	endif
#endif

// Should we use the Thread Manager or deferred task mulit threading?
#ifndef ZCONFIG_Fiber_ThreadManager
#	define ZCONFIG_Fiber_ThreadManager 1
#endif

// ==================================================
// Which windowing API?

#if !ZCONFIG_ResourceCompilerInvoked
#	ifndef ZCONFIG_API_OSWindow
#		if ZCONFIG_OS_MacOS7 == ZCONFIG_OS
#			define ZCONFIG_API_OSWindow ZCONFIG_API_OSWindow_Mac
#		elif ZCONFIG_OS_Carbon == ZCONFIG_OS
#			define ZCONFIG_API_OSWindow ZCONFIG_API_OSWindow_Mac
#		elif ZCONFIG_OS_Win32 == ZCONFIG_OS
#			define ZCONFIG_API_OSWindow ZCONFIG_API_OSWindow_Win32
#		elif ZCONFIG_OS_POSIX == ZCONFIG_OS
#			define ZCONFIG_API_OSWindow ZCONFIG_API_OSWindow_X
#		elif ZCONFIG_OS_Be == ZCONFIG_OS
#			define ZCONFIG_API_OSWindow ZCONFIG_API_OSWindow_Be
#		endif
#	endif
#endif

#ifndef ZCONFIG_API_OSWindow
#	define ZCONFIG_API_OSWindow ZCONFIG_API_Window_Unknown
#endif

// ==================================================
// Which graphics API(s)?

#if !ZCONFIG_ResourceCompilerInvoked
#	ifndef ZCONFIG_API_Graphics
#		if ZCONFIG_OS_MacOS7 == ZCONFIG_OS
#			define ZCONFIG_API_Graphics ZCONFIG_API_Graphics_QD
#		elif ZCONFIG_OS_Carbon == ZCONFIG_OS
#			define ZCONFIG_API_Graphics ZCONFIG_API_Graphics_QD
#		elif ZCONFIG_OS_Win32 == ZCONFIG_OS
#			define ZCONFIG_API_Graphics ZCONFIG_API_Graphics_GDI
#		elif ZCONFIG_OS_POSIX == ZCONFIG_OS
#			define ZCONFIG_API_Graphics ZCONFIG_API_Graphics_X
#		elif ZCONFIG_OS_Be == ZCONFIG_OS
#			define ZCONFIG_API_Graphics ZCONFIG_API_Graphics_Be
#		endif
#	endif
#endif

#ifndef ZCONFIG_API_Graphics
#	define ZCONFIG_API_Graphics ZCONFIG_API_Graphics_Unknown
#endif

// Set up a definition that will be true iff there is more than one graphics API active
#define ZCONFIG_API_Graphics_Multi\
		(\
		ZCONFIG_API_Graphics_QD != ZCONFIG_API_Graphics &&\
		ZCONFIG_API_Graphics_GDI != ZCONFIG_API_Graphics &&\
		ZCONFIG_API_Graphics_X != ZCONFIG_API_Graphics &&\
		ZCONFIG_API_Graphics_Be != ZCONFIG_API_Graphics &&\
		ZCONFIG_API_Graphics_Generic != ZCONFIG_API_Graphics\
		)

// ==================================================
// Which file system API?

#if !ZCONFIG_ResourceCompilerInvoked
#	ifndef ZCONFIG_API_File
#		if ZCONFIG_OS_MacOS7 == ZCONFIG_OS
#			define ZCONFIG_API_File ZCONFIG_API_File_Mac
#		elif ZCONFIG_OS_Carbon == ZCONFIG_OS
#			define ZCONFIG_API_File ZCONFIG_API_File_Mac
#		elif ZCONFIG_OS_Win32 == ZCONFIG_OS
#			define ZCONFIG_API_File ZCONFIG_API_File_Win32
#		elif ZCONFIG_OS_POSIX == ZCONFIG_OS
#			define ZCONFIG_API_File ZCONFIG_API_File_POSIX
#		elif ZCONFIG_OS_Be == ZCONFIG_OS
#			define ZCONFIG_API_File ZCONFIG_API_File_Be
#		endif
#	endif
#endif

#ifndef ZCONFIG_API_File
#	define ZCONFIG_API_File ZCONFIG_API_File_Unknown
#endif

// ==================================================
// Which network API(s)?

#if !ZCONFIG_ResourceCompilerInvoked
#	ifndef ZCONFIG_API_Net
#		if ZCONFIG_OS_MacOS7 == ZCONFIG_OS
#			define ZCONFIG_API_Net ZCONFIG_API_Net_MacClassic
#		elif ZCONFIG_OS_Carbon == ZCONFIG_OS
#			define ZCONFIG_API_Net ZCONFIG_API_Net_MacOT
#		elif ZCONFIG_OS_Win32 == ZCONFIG_OS
#			define ZCONFIG_API_Net ZCONFIG_API_Net_WinSock
#		elif ZCONFIG_OS_POSIX == ZCONFIG_OS
#			define ZCONFIG_API_Net ZCONFIG_API_Net_Socket
#		elif ZCONFIG_OS_Be == ZCONFIG_OS
#			define ZCONFIG_API_Net ZCONFIG_API_Net_BeSocket
#		endif
#	endif
#endif

#ifndef ZCONFIG_API_Net
#	define ZCONFIG_API_Net ZCONFIG_API_Net_Unknown
#endif

// ==================================================
// Declare namespace std and use it -- injecting all std names into global
// namespace. A necessary hack until we've gone through all source and done this on
// a case by case basis. This can be switched *off* by defining ZCONFIG_NamespaceHack
// as zero in your zconfig.h file.

#ifndef ZCONFIG_NamespaceHack
#	define ZCONFIG_NamespaceHack 0
#endif

#if ZCONFIG_NamespaceHack && defined(__cplusplus) && !ZCONFIG_ResourceCompilerInvoked
	namespace std {}
	using namespace std;
#endif

// ==================================================
// Some extra bits to patch up some CodeWarrior issues.
#if defined(__MWERKS__)
#	if __option(precompile)
#		error "Don't precompile zconfig -- settings on individual files will not match claimed configuration settings."
#	endif
#	ifndef NEWMODE
#		define NEWMODE NEWMODE_MALLOC
#	endif
#	if !ZCONFIG_Fiber_ThreadManager
#		if defined(_MWMT)
#			if _MWMT != 1
#				error "_MWMT must be 1 when not using Thread Manager -- check your headers to see who set it to zero."
#			endif
#		else
#			define _MWMT 1
#		endif
#	endif
#	define MSL_USE_PRECOMPILED_HEADERS 0
#endif

// ==================================================
// Some extra bits to patch up some MSVC issues.
#if defined(_MSC_VER)
#	if !defined(_MT)
#		error "_MT is not defined. ZooLib requires thread safe libraries and headers."
#	endif

// Ghastly workaround for MSVC non compliance with C++ standard
#	define for if (0) {} else for

// Disable illegal pragma warning -- we use pragma mark fairly extensively
#	pragma warning(disable:4068)

// Disable debug info truncated to 255 characters (STL template expansion will often generate this)
#	pragma warning(disable:4786)

#endif

// ==================================================
// Define _REENTRANT for all platforms, although it's only significant for POSIX.
#ifndef _REENTRANT
#	define _REENTRANT
#endif

// ==================================================
// A definition for 'nil', which is used extensively where NULL might more normally be expected.
// The reason for this is that I was following the lead of Cyrus Ghalambor, who in turn used it (I think)
// because a team he previously worked with had a fair number of ex-pascal people on it.
#if 0 // denis 2005/12/09: disable this which conflicts with STL and libsigc++
#ifndef nil
#	define nil (0)
#endif
#endif

#endif // __zconfigl__
