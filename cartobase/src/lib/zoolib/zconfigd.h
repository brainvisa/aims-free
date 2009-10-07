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

#ifndef __zconfigd__
#define __zconfigd__ 1

// The flags are done as *defines* rather than enums or anything else because they may
// be used in resource files, and they control the selection of source to compile.

// Which compiler -- ZCONFIG_Compiler
#define ZCONFIG_Compiler_CodeWarrior 1
#define ZCONFIG_Compiler_MSVC 2
#define ZCONFIG_Compiler_GCC 3

// Which variant of the STL -- ZCONFIG_STL
#define ZCONFIG_STL_Modena 1
#define ZCONFIG_STL_Unknown 2

// Which OS -- ZCONFIG_OS
#define ZCONFIG_OS_MacOS7 1
#define ZCONFIG_OS_Carbon 2
#define ZCONFIG_OS_Win32 3
#define ZCONFIG_OS_POSIX 4
#define ZCONFIG_OS_Be 5

// Which processor -- ZCONFIG_Processor
#define ZCONFIG_Processor_68K 1
#define ZCONFIG_Processor_PPC 2
#define ZCONFIG_Processor_x86 3
#define ZCONFIG_Processor_Sparc 4

// Byte order -- ZCONFIG_Endian
#define ZCONFIG_Endian_Big 1
#define ZCONFIG_Endian_Little 2

// ZCONFIG_API_Thread
#define ZCONFIG_API_Thread_Unknown 0
#define ZCONFIG_API_Thread_Mac 1
#define ZCONFIG_API_Thread_Win32 2
#define ZCONFIG_API_Thread_POSIX 3
#define ZCONFIG_API_Thread_Be 4
// There is also an on/off option, ZCONFIG_Fiber_ThreadManager, which is used to set which
// threading implementation to use on Mac. By default it is set to one in zconfigl.h.
// If you set it to zero we will use deferred task threading, and the standard
// library will make appropriate use of locks to ensure thread safety. You'll have to
// replace CodeWarrior's mutex.h header file with an edited version -- check the docs for details.


// ZCONFIG_API_OSWindow
#define ZCONFIG_API_OSWindow_Unknown 0
#define ZCONFIG_API_OSWindow_Mac 1
#define ZCONFIG_API_OSWindow_Win32 2
#define ZCONFIG_API_OSWindow_X 3
#define ZCONFIG_API_OSWindow_Be 4

// ZCONFIG_API_Graphics
// We support more than one graphics API simultaneously, so check for set bits *not* equivalence.
#define ZCONFIG_API_Graphics_Unknown 0
#define ZCONFIG_API_Graphics_QD 1
#define ZCONFIG_API_Graphics_GDI 2
#define ZCONFIG_API_Graphics_X 4
#define ZCONFIG_API_Graphics_Be 8
#define ZCONFIG_API_Graphics_Generic 128

// ZCONFIG_API_File
#define ZCONFIG_API_File_Unknown 0
#define ZCONFIG_API_File_Mac 1
#define ZCONFIG_API_File_Win32 2
#define ZCONFIG_API_File_POSIX 3
#define ZCONFIG_API_File_Be 4

// ZCONFIG_API_Net
// We support more than one net API simultaneously, so check for set bits *not* equivalence.
#define ZCONFIG_API_Net_Unknown 0
#define ZCONFIG_API_Net_MacClassic 1
#define ZCONFIG_API_Net_MacOT 2
#define ZCONFIG_API_Net_WinSock 4
#define ZCONFIG_API_Net_Socket 8
#define ZCONFIG_API_Net_BeSocket 16


// Debugging
// ZCONFIG_Debug can take on any value, but by convention use 0 for a non-debug build, 1 for light debugging, 2 for heavy.

#endif // __zconfigd__
