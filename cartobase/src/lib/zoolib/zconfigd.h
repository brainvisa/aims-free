/*  @(#) $Id$ */

/* ------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

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
