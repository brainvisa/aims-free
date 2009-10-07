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

#ifndef __ZAtomic__
#define __ZAtomic__ 1
#include "zconfig.h"

#include "ZDebug.h" // For ZAssertCompile

// AG 2000-02-06. Much of the code in this file inlines. For that to work we need the various compilers
// to see quite different code. The first section of this file is #ifed out of existence, but should be
// considered to be the canonical API. The second section has the various versions of the real code.

struct ZAtomic_t
	{
	volatile int fValue;
	};

// Ensure that ZAtomic_t packs properly
ZAssertCompile(sizeof(ZAtomic_t) == sizeof(int));

#if 0
int ZAtomic_Get(const ZAtomic_t* inValueAddress);
int ZAtomic_Set(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Add(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_And(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Or(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Xor(ZAtomic_t* inValueAddress, int inParam);

bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress);
void ZAtomic_Inc(ZAtomic_t* inValueAddress);
void ZAtomic_Dec(ZAtomic_t* inValueAddress);

#endif

// ====================================================================================================
#if (ZCONFIG_Compiler_CodeWarrior == ZCONFIG_Compiler) && (ZCONFIG_Processor_68K == ZCONFIG_Processor)
#pragma mark -
#pragma mark * CodeWarrior/68K

inline int ZAtomic_Get(const ZAtomic_t* inValueAddress)
	{ return inValueAddress->fValue; }

int ZAtomic_Set(ZAtomic_t* inValueAddress : __A0, int inParam : __D1);
int ZAtomic_Add(ZAtomic_t* inValueAddress : __A0, int inParam : __D1);
int ZAtomic_And(ZAtomic_t* inValueAddress : __A0, int inParam : __D1);
int ZAtomic_Or(ZAtomic_t* inValueAddress : __A0, int inParam : __D1);
int ZAtomic_Xor(ZAtomic_t* inValueAddress : __A0, int inParam : __D1);

asm inline bool ZAtomic_DecAndTest(ZAtomic_t* : __A0)
	{
	subq.l #1, (a0)
	seq d0
	}

asm inline void ZAtomic_Inc(ZAtomic_t* : __A0)
	{
	add.l #1, (a0)
	}

asm inline void ZAtomic_Dec(ZAtomic_t* : __A0)
	{
	sub.l #1, (a0)
	}

// ====================================================================================================
#elif (ZCONFIG_Compiler_CodeWarrior == ZCONFIG_Compiler) && (ZCONFIG_Processor_PPC == ZCONFIG_Processor)
#pragma mark -
#pragma mark * CodeWarrior/PPC

// Although CW can inline PPC assembler, for some reason it has a hard time satisfying the constraints of enclosing
// code and the inlined stuff simultaneously. We're able to use one (!) register in inlined code, but almost all this code
// requires two or more. The code is thus defined out of line in ZAtomic.cpp. The penalty for doing so is not as bad as
// it might seem, given that they are leaf routines and thus can be switched to/returned from in one clock cycle.

inline int ZAtomic_Get(const ZAtomic_t* inValueAddress)
	{ return inValueAddress->fValue; }

int ZAtomic_Set(register ZAtomic_t* inValueAddress, register int inParam);

int ZAtomic_Add(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_And(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Or(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Xor(ZAtomic_t* inValueAddress, int inParam);

bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress);
void ZAtomic_Inc(ZAtomic_t* inValueAddress);
void ZAtomic_Dec(ZAtomic_t* inValueAddress);

// ====================================================================================================
#elif (ZCONFIG_Compiler_CodeWarrior == ZCONFIG_Compiler) && (ZCONFIG_Processor_x86 == ZCONFIG_Processor)
#pragma mark -
#pragma mark * CodeWarrior/Intel x86

// CodeWarrior sucks rocks when it comes to reliably including inline x86 assmbly with general C/C++ code, or
// generating the correct opcodes for parameters. To work around the problem the code has to do superfluous
// register/register transfers, causing massive bloat. So for the time being these routines are defined out of
// line in ZAtomic.cpp, which will also make it easier to tweak them without having massive rebuilds.

inline int ZAtomic_Get(const ZAtomic_t* inValueAddress)
	{ return inValueAddress->fValue; }

int ZAtomic_Set(register ZAtomic_t* inValueAddress, int inParam);

int ZAtomic_Add(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_And(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Or(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Xor(ZAtomic_t* inValueAddress, int inParam);

bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress);
void ZAtomic_Inc(ZAtomic_t* inValueAddress);
void ZAtomic_Dec(ZAtomic_t* inValueAddress);

// ====================================================================================================
#elif (ZCONFIG_Compiler_GCC == ZCONFIG_Compiler) && (ZCONFIG_Processor_x86 == ZCONFIG_Processor)
#if 0 // denis 2005/12/09 this causes warnings and is unused by gcc4
#pragma mark -
#pragma mark * GCC/Intel x86
#endif

// AG 2000-02-08. My install of RedHat Linux, upgraded to egcs 1.0b1, does not know
// how to assemble >386 instructions. For now I'm just injecting the bytes manually.
// So, don't mess with the assignment of values to registers.

#if defined(__i486__) || defined(__i586__) || defined(__i686__)
#	define ZAtomic_cmpxchg "lock; cmpxchg %%ecx, (%%esi)\n"
#else
#	define ZAtomic_cmpxchg ".byte 0xF0, 0x0F, 0xB1, 0x0E\n" // (which is lock; cmpxchg %ecx, (%esi)
#endif

#define ZAtomic_Op_Macro(theOp) \
	int oldValue, dummy1, dummy2; \
	asm volatile \
		( \
		"1: mov (%1), %0\n" \
		"mov %0, %%ecx\n" \
		#theOp" %2, %%ecx\n" \
		ZAtomic_cmpxchg \
		"jne 1b" \
		: "=a" (oldValue), "=&S" (dummy1), "=&d" (dummy2) \
		: "1" (inValueAddress), "2" (inParam) \
		: "ecx", "cc" \
		); \
	return oldValue;

inline int ZAtomic_Get(const ZAtomic_t* inValueAddress)
	{ return inValueAddress->fValue; }

inline int ZAtomic_Set(ZAtomic_t* inValueAddress, int inParam)
	{
	asm volatile
		(
		"lock; xchg %0, (%1)"
		: "+q" (inParam)
		: "q" (inValueAddress)
		: "cc", "memory"
		);
	return inParam;
	}

inline int ZAtomic_Add(ZAtomic_t* inValueAddress, int inParam)
	{
	asm volatile
		(
		"lock; xadd %0, (%1)"
		: "+q" (inParam)
		: "q" (inValueAddress)
		: "cc", "memory"
		);
	return inParam;
	}

inline int ZAtomic_And(ZAtomic_t* inValueAddress, int inParam)
	{
	ZAtomic_Op_Macro(and)
	}

inline int ZAtomic_Or(ZAtomic_t* inValueAddress, int inParam)
	{
	ZAtomic_Op_Macro(or)
	}

inline int ZAtomic_Xor(ZAtomic_t* inValueAddress, int inParam)
	{
	ZAtomic_Op_Macro(xor)
	}

inline bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress)
	{
	bool isZero;
	asm volatile
		(
		"lock; decl (%1)\n"
		"sete %0"
		: "=q" (isZero)
		: "q" (inValueAddress)
		: "cc", "memory"
		);
	return isZero;
	}

inline void ZAtomic_Inc(ZAtomic_t* inValueAddress)
	{
	asm volatile
		(
		"lock; incl (%0)"
		: // No outputs
		: "q" (inValueAddress)
		: "cc", "memory"
		);
	}

inline void ZAtomic_Dec(ZAtomic_t* inValueAddress)
	{
	asm volatile
		(
		"lock; decl (%0)"
		: // No outputs
		: "q" (inValueAddress)
		: "cc", "memory"
		);
	}

#undef ZAtomic_cmpxchg
#undef ZAtomic_Op_Macro

// ====================================================================================================
#elif (ZCONFIG_Compiler_GCC == ZCONFIG_Compiler) && (ZCONFIG_Processor_PPC == ZCONFIG_Processor)
#pragma mark -
#pragma mark * GCC/PPC

#define ZAtomic_Op_Macro(theOp) \
	int oldValue; \
	int newValue; \
	asm volatile \
		( \
		"1: lwarx %1, 0, %4\n" \
		#theOp" %0, %1, %3\n" \
		"stwcx. %0, 0, %4\n" \
		"bne 1b" \
		: "=&r" (newValue), "=&r" (oldValue), "=m" (*inValueAddress) \
		: "r" (inParam), "r" (inValueAddress) \
		: "cc" \
		); \
	return oldValue;

inline int ZAtomic_Get(const ZAtomic_t* inValueAddress)
	{ return inValueAddress->fValue; }

inline int ZAtomic_Set(ZAtomic_t* inValueAddress, int inParam)
	{
	int oldValue;
	asm volatile
		(
		"1: lwarx %0, 0, %3\n"
		"stwcx. %2, 0, %3\n"
		"bne 1b"
		: "=&r" (oldValue), "=m" (*inValueAddress)
		: "r" (inParam), "r" (inValueAddress) 
		: "cc"
		);
	return oldValue;
	}

inline int ZAtomic_Add(ZAtomic_t* inValueAddress, int inParam)
	{
	ZAtomic_Op_Macro(add)
	}

inline int ZAtomic_And(register ZAtomic_t* inValueAddress, register int inParam)
	{
	ZAtomic_Op_Macro(and)
	}

inline int ZAtomic_Or(register ZAtomic_t* inValueAddress, register int inParam)
	{
	ZAtomic_Op_Macro(or)
	}

inline int ZAtomic_Xor(register ZAtomic_t* inValueAddress, register int inParam)
	{
	ZAtomic_Op_Macro(xor)
	}

inline bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress)
	{
	return ZAtomic_Add(inValueAddress, -1) == 1;
	}

inline void ZAtomic_Inc(ZAtomic_t* inValueAddress)
	{
	ZAtomic_Add(inValueAddress, 1);
	}

inline void ZAtomic_Dec(ZAtomic_t* inValueAddress)
	{
	ZAtomic_Add(inValueAddress, -1);
	}

// ====================================================================================================
#elif (ZCONFIG_Compiler_MSVC == ZCONFIG_Compiler) && (ZCONFIG_Processor_x86 == ZCONFIG_Processor)
#pragma mark -
#pragma mark * MSVC/Intel x86

// Defined out of line in ZAtomic.cpp till I can determine how effective MSVC is at
// inlining assembly code.

inline int ZAtomic_Get(const ZAtomic_t* inValueAddress)
	{ return inValueAddress->fValue; }

int ZAtomic_Set(register ZAtomic_t* inValueAddress, int inParam);

int ZAtomic_Add(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_And(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Or(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Xor(ZAtomic_t* inValueAddress, int inParam);

bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress);
void ZAtomic_Inc(ZAtomic_t* inValueAddress);
void ZAtomic_Dec(ZAtomic_t* inValueAddress);


// ====================================================================================================
#else // ZCONFIG_Compiler/ZCONFIG_Processor
#pragma mark -
#pragma mark * Dumb version

// Defined out of line in ZAtomic.cpp, using a global mutex to ensure atomicity. Performance will not
// be great, but it will at least work everywhere that we have a ZThread implementation.

inline int ZAtomic_Get(const ZAtomic_t* inValueAddress)
	{ return inValueAddress->fValue; }

int ZAtomic_Set(register ZAtomic_t* inValueAddress, int inParam);

int ZAtomic_Add(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_And(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Or(ZAtomic_t* inValueAddress, int inParam);
int ZAtomic_Xor(ZAtomic_t* inValueAddress, int inParam);

bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress);
void ZAtomic_Inc(ZAtomic_t* inValueAddress);
void ZAtomic_Dec(ZAtomic_t* inValueAddress);

#endif // ZCONFIG_Compiler/ZCONFIG_Processor

// ====================================================================================================



#endif // __ZAtomic__
