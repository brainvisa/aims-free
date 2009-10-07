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
