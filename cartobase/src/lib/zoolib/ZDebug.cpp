static const char rcsid[] = "@(#) $Id$";

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

#include "ZDebug.h"

#include <cstdarg>
#include <cstdio>
#include <stdlib.h> // denis 2005/12/09: needed for abort()


static const int sStringBufferSize = 1024;

// ====================================================================================================

#if ZCONFIG_OS_POSIX == ZCONFIG_OS

const char* ZDebug_FormatMessage(const char* message, ...)
	{
// This is not threadsafe.
	static char sDebugMessageString[sStringBufferSize];
	va_list args;
	va_start(args, message);
	int count = vsnprintf(sDebugMessageString, sStringBufferSize, message, args);
	va_end(args);
	return sDebugMessageString;
	}

void ZDebug_DisplayMessage(int inLevel, ZDebug_Action inAction, const char* inFilename, int inLine, const char* inAssertionMessage, const char* inUserMessage)
	{
	static const size_t sStringBufferSize = 1024;

	char realDebugMessageString[sStringBufferSize];

	if (inAssertionMessage)
		{
		if (inUserMessage)
			::snprintf(realDebugMessageString, sStringBufferSize, ZDebug_Message_AssertionAndUser"\n", inAssertionMessage, inUserMessage, inFilename, inLine);
		else
			::snprintf(realDebugMessageString, sStringBufferSize, ZDebug_Message_AssertionOnly"\n", inAssertionMessage, inFilename, inLine);
		}
	else
		{
		if (inUserMessage)
			::snprintf(realDebugMessageString, sStringBufferSize, ZDebug_Message_UserOnly"\n", inUserMessage, inFilename, inLine);
		else
			::snprintf(realDebugMessageString, sStringBufferSize, ZDebug_Message_None"\n", inFilename, inLine);
		}
	
	::fprintf(stderr, realDebugMessageString);

	if (inAction == eDebugAction_Stop)
		::abort();
	}

#endif // ZCONFIG_OS_POSIX == ZCONFIG_OS

// ====================================================================================================

#if ZCONFIG_OS_Be == ZCONFIG_OS

#include <kernel/OS.h>

const char* ZDebug_FormatMessage(const char* message, ...)
	{
// This is not threadsafe.
	static char sDebugMessageString[sStringBufferSize];
	va_list args;
	va_start(args, message);
	int count = vsnprintf(sDebugMessageString, sStringBufferSize, message, args);
	va_end(args);
	return sDebugMessageString;
	}

void ZDebug_DisplayMessage(int inLevel, ZDebug_Action inAction, const char* inFilename, int inLine, const char* inAssertionMessage, const char* inUserMessage)
	{
	static const size_t sStringBufferSize = 1024;

	char realDebugMessageString[sStringBufferSize];

	if (inAssertionMessage)
		{
		if (inUserMessage)
			::snprintf(realDebugMessageString, sStringBufferSize, ZDebug_Message_AssertionAndUser"\n", inAssertionMessage, inUserMessage, inFilename, inLine);
		else
			::snprintf(realDebugMessageString, sStringBufferSize, ZDebug_Message_AssertionOnly"\n", inAssertionMessage, inFilename, inLine);
		}
	else
		{
		if (inUserMessage)
			::snprintf(realDebugMessageString, sStringBufferSize, ZDebug_Message_UserOnly"\n", inUserMessage, inFilename, inLine);
		else
			::snprintf(realDebugMessageString, sStringBufferSize, ZDebug_Message_None"\n", inFilename, inLine);
		}

	if (inAction == eDebugAction_Stop)
		::debugger(realDebugMessageString);
	else
		{
		::fprintf(stderr, realDebugMessageString);
		}
	}

#endif // ZCONFIG_OS_Be == ZCONFIG_OS

// ====================================================================================================
