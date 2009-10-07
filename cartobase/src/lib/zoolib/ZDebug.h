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

#ifndef __ZDebug__
#define __ZDebug__ 1
#include <cstdlib>
#include "zconfig.h"

/* ====================================================================================================
AG 2000-08-11.
Another rework, this time breaking out the implementation of ZDebug_DisplayMessage into separate
ZDebug_XXX implementation files, and putting more capabilities in place, like detecting the presence
of a debugger on Windows, and dumping stack crawls to log files on MacOS and Windows.

 
AG 1998-11-10.
ZDebug has been radically revised to provide more control over when debugging messages are output, and
whether we're dropped into the debugger/exit()ed from the app.

All these macros/routines take a debug level as the first parameter and will only have an effect if the level is
less than or equal to the value of ZCONFIG_Debug. So an assertion with a level parameter of 0 will show up in
all versions of the app, a level 1 will not show up in non-debug apps.

Pseudo-prototypes (not quite real C++ because of the parenthesized printf-style stuff)
-----------------
void ZDebugStop(int debugLevel);
void ZDebugStopf(int debugLevel, (const char* message, ...));
void ZDebugLog(int debugLevel);
void ZDebugLogf(int debugLevel, (const char* message, ...));

bool ZAssertStop(int debugLevel, bool assertCondition);
bool ZAssertStopf(int debugLevel, bool assertCondition, (const char* message, ...));
bool ZAssertLog(int debugLevel, bool assertCondition);
bool ZAssertLogf(int debugLevel, bool assertCondition, (const char* message, ...));

Usage
-----
ZDebugStop takes a debug level parameter, and unconditionally stops/exits the app if ZCONFIG_Debug is >= the parameter.
Example:
ZDebugStop(2); // Will unconditionally stop/exit the app when ZCONFIG_Debug is 2, become a no-op when ZCONFIG_Debug is 0 or 1

ZDebugStopf takes a debug level, and additionally (in parentheses [coz of macro weirdness]) a printf-style
parameter list (called printf-params in these docs). Otherwise its behavior is the same as ZDebugStop
Example: (assuming an accesible int called theValue)
ZDebugStop(1, ("Our integer's value is %d", theValue));

ZDebugLog takes a debug level, and if the level is active will cause a statement of the form:
"filename: line#" to be logged. Its functionality is basically that of a TRACE macro.

ZDebugLogf takes a debug level and printf-params. It will output the parameters if the debug level is active,using an appropriate
mechanism for the platform (MacsBug, source debugger, output to stderr).
ZDebugLogf does *not* stop or exit the application -- it just logs the message and continues operation.

The ZAssert macros are similar to the ZDebug macros, with the addition of a parameter inserted between the debug level
and the printf-params. The second parameter should be a boolean condition expression. If the debug level is
active, and the condition evaluates *false*, the printf-params will be output, along with the text of the
failed condition, ZAssertStopf will stop/exit the app. ZAssertLogf will just output the text and resume execution.

ZAssertLog (note the lack of an 'f' on the end) does not take printf-params, it just outputs the text of the
failed condition (along with the line number and file, like all the other macros) and lets the app continue.
ZAssertStop does the same but stops/exits the application.

ZAssert and ZDebugString are provided for backward compatibility. They are equivalent to ZAssertStop and
ZDebugStopf respectively, each with a debug level of 0. ZUnimplemented is also provided for backward compatibility.

Note that ZAssertLog/ZAssertLogf/ZAssertStop/ZAssertStopf return a boolean (the value of the condition expression). So
you can write code like this:
if (!ZAssertLogf(2, theObject.IsValid(), ("Whoaa, our object: %08x was not valid\n", &theObject)))
	{
// Some recovery code
	}

[Note the '!' before the ZAssertLogf -- the value returned by ZAssertXXX is the value of the condition
expression, which will be true when the condition is satisfied, and false when not.]

IMPORTANT. The "recovery code" will only be executed if the assertion fails, *and* the debug level parameter is <=
the value of ZCONFIG_Debug. So if ZCONFIG_Debug were set to 1, the entire chunk of code above would drop out
of the application.


TO DO
-----
Allow an application to install a function to be called to output a debug message (so a unix daemon can output to stdlog, for example).
==================================================================================================== */

// Note. I'm sorry, but (I think) all this stuff needs to be visible in ZDebug.h, so that inlines can be
// optimized away and generated code size minimized.

enum ZDebug_Action { eDebugAction_Continue, eDebugAction_Stop };

// ZDebug_FormatMessage and ZDebug_DisplayMessage are defined either in ZDebug.cpp or in ZDebug_XXX.cpp. ZDebug_FormatMessage
// takes the printf string and invokes sprintf on a static buffer, it may also acquire a critical section to ensure thread
// safety. ZDebug_DisplayMessage does the actual work of dumping the text to the console, invoking the debugger, or whatever is appropriate.

// Note that we use overloading on ZDebug_FormatMessage to simplify the macros, and allow inlining to occur.
const char* ZDebug_FormatMessage(const char* inMessage, ...);
inline const char* ZDebug_FormatMessage() { return 0; }

void ZDebug_DisplayMessage(int inLevel, ZDebug_Action inAction, const char* inFilename, int inLine, const char* inAssertionMessage, const char* inUserMessage);

// The actual macros that get used in source code. The funkiness of these macros (should) generate minimal inline code, so that
// an assertion that's active at level 2 will generate *no code* at level 1 or 0. This does rely on the compiler optimizing
// away statements/expressions with no effect.
#define ZDebugLogf(a, b) ((a)<=ZCONFIG_Debug ? ZDebug_DisplayMessage(a, eDebugAction_Continue, __FILE__, __LINE__, 0, ZDebug_FormatMessage b): (void)0)
#define ZDebugStopf(a, b) ((a)<=ZCONFIG_Debug ? ZDebug_DisplayMessage(a, eDebugAction_Stop, __FILE__, __LINE__, 0, ZDebug_FormatMessage b): (void)0)
#define ZDebugLog(a) ((a)<=ZCONFIG_Debug ? ZDebug_DisplayMessage(a, eDebugAction_Continue, __FILE__, __LINE__, 0, 0): (void)0)
#define ZDebugStop(a) ((a)<=ZCONFIG_Debug ? ZDebug_DisplayMessage(a, eDebugAction_Stop, __FILE__, __LINE__, 0, 0): (void)0)

#define ZAssertLogf(a, b, c) ((a)<=ZCONFIG_Debug && !(b) ? ZDebug_DisplayMessage(a, eDebugAction_Continue, __FILE__, __LINE__, #b, ZDebug_FormatMessage c), false: true)
#define ZAssertStopf(a, b, c) ((a)<=ZCONFIG_Debug && !(b) ? ZDebug_DisplayMessage(a, eDebugAction_Stop, __FILE__, __LINE__, #b, ZDebug_FormatMessage c), false: true)
#define ZAssertLog(a, b) ((a)<=ZCONFIG_Debug && !(b) ? ZDebug_DisplayMessage(a, eDebugAction_Continue, __FILE__, __LINE__, #b, 0), false: true)
#define ZAssertStop(a, b) ((a)<=ZCONFIG_Debug && !(b) ? ZDebug_DisplayMessage(a, eDebugAction_Stop, __FILE__, __LINE__, #b, 0), false: true)


// ZAssertCompile can be used to enforce a constraint at compile time, (for example that
// a struct obeys necessary alignment rules). It either drops out completely or generates an error,
// depending on whether the expression evaulates true or false.
#define ZAssertCompile(a) extern int sCompileTimeAssertionViolated[(a) ? 1 : 0]

// I'd like to formalize ZUnimplemented a little more sometime. Perhaps it should throw an exception in production code.
#define ZUnimplemented() ZDebugStopf(0, ("Unimplemented routine"))


// There are still quite a lot of places where plain old ZAssert is used.
#define ZAssert(a) ZAssertStop(1, a);

// ====================================================================================================
// These defines could go into a private header, but I'd rather not add yet
// another file. Perhaps later.

#define ZDebug_Message_AssertionAndUser "Assertion failed: %s. %s, %s:%d"
#define ZDebug_Message_AssertionOnly "Assertion failed: %s, %s:%d"
#define ZDebug_Message_UserOnly "%s, %s:%d"
#define ZDebug_Message_None "%s:%d"

#endif // __ZDebug__
