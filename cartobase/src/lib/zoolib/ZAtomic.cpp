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

#include "ZAtomic.h"

// ====================================================================================================
#if (ZCONFIG_Compiler_CodeWarrior == ZCONFIG_Compiler) && (ZCONFIG_Processor_68K == ZCONFIG_Processor)
#pragma mark -
#pragma mark * CodeWarrior/68K

// There are no SMP 68K machines to worry about, so we just need to disable interrupts.
// WARNING. DO NOT TRACE THROUGH THIS CODE. INTERRUPTS WILL BE DISABLED AND YOU *WILL* LOCK UP

#define ZAtomic_Op_Macro(theOp) \
/* Grab the current status register into d2 */ \
	move.w sr, d2; \
/* Set interrupt mask to 7. Tracing this instruction will hang your machine */ \
	or.w #0x0700, sr; \
/* Grab the original value referenced by inValueAddress */ \
	move.l (a0), d0; \
/* Do the operation on value referenced by inValueAddress */ \
	theOp.l d1, (a0); \
/* Restore the status register, and hence the interrupt mask */ \
	move.w d2, sr; \
/* All done. */ \
	rts

asm int ZAtomic_Set(ZAtomic_t* : __A0, int : __D1)
	{
	ZAtomic_Op_Macro(move)
	}

asm int ZAtomic_Add(ZAtomic_t* : __A0, int : __D1)
	{
	ZAtomic_Op_Macro(add)
	}

asm int ZAtomic_And(ZAtomic_t* : __A0, int : __D1)
	{
	ZAtomic_Op_Macro(and)
	}

asm int ZAtomic_Or(ZAtomic_t* : __A0, int : __D1)
	{
	ZAtomic_Op_Macro(or)
	}

asm int ZAtomic_Xor(ZAtomic_t* : __A0, int : __D1)
	{
	ZAtomic_Op_Macro(eor)
	}

#undef ZAtomic_Op_Macro

// ====================================================================================================
#elif (ZCONFIG_Compiler_CodeWarrior == ZCONFIG_Compiler) && (ZCONFIG_Processor_PPC == ZCONFIG_Processor)
#pragma mark -
#pragma mark * CodeWarrior/PPC

// AG 2000-06-29. Switch off profiling, otherwise inParam in ZAtomic_Set does not get assigned
// to a register (go figure).
#pragma profile off

int ZAtomic_Set(register ZAtomic_t* inValueAddress, register int inParam)
	{
	register int oldValue;
	asm
		{		
	loop:
		lwarx oldValue, 0, inValueAddress
		stwcx. inParam, 0, inValueAddress
		bne loop
		}
	return oldValue;
	}

#define ZAtomic_Op_Macro(theOp) \
	register int oldValue; \
	register int newValue; \
	asm { \
	loop: \
		lwarx oldValue, 0, inValueAddress; \
		theOp newValue, oldValue, inParam; \
		stwcx. newValue, 0, inValueAddress; \
		bne loop; \
		} \
	return oldValue;


int ZAtomic_Add(register ZAtomic_t* inValueAddress, register int inParam)
	{
	ZAtomic_Op_Macro(add)
	}

int ZAtomic_And(register ZAtomic_t* inValueAddress, register int inParam)
	{
	ZAtomic_Op_Macro(and)
	}

int ZAtomic_Or(register ZAtomic_t* inValueAddress, register int inParam)
	{
	ZAtomic_Op_Macro(or)
	}

int ZAtomic_Xor(register ZAtomic_t* inValueAddress, register int inParam)
	{
	ZAtomic_Op_Macro(xor)
	}

#undef ZAtomic_Op_Macro

bool ZAtomic_DecAndTest(register ZAtomic_t* inValueAddress)
	{
	register int oldValue;
	register int newValue;
	register bool result = false;
	asm
		{
	loop:
		lwarx oldValue, 0, inValueAddress
		subi newValue, oldValue, 1
		stwcx. newValue, 0, inValueAddress
		bne loop
		cmpi cr0, newValue, 0
		bne cr0, notZero
		li result, 1
	notZero:
		}
	return result;
	}

void ZAtomic_Inc(register ZAtomic_t* inValueAddress)
	{
	register int oldValue;
	register int newValue;
	asm
		{
	loop:
		lwarx oldValue, 0, inValueAddress
		addi newValue, oldValue, 1
		stwcx. newValue, 0, inValueAddress
		bne loop
		}
	}

void ZAtomic_Dec(register ZAtomic_t* inValueAddress)
	{
	register int oldValue;
	register int newValue;
	asm
		{
	loop:
		lwarx oldValue, 0, inValueAddress
		subi newValue, oldValue, 1
		stwcx. newValue, 0, inValueAddress
		bne loop
		}
	}

// ====================================================================================================
#elif (ZCONFIG_Compiler_GCC == ZCONFIG_Compiler) && (ZCONFIG_Processor_x86 == ZCONFIG_Processor)
#if 0 // denis 2005/12/09: disable this, it causes warnings
#pragma mark -
#pragma mark * GCC/Intel x86
#endif

// ====================================================================================================
#elif (ZCONFIG_Compiler_GCC == ZCONFIG_Compiler) && (ZCONFIG_Processor_PPC == ZCONFIG_Processor)
#pragma mark -
#pragma mark * GCC/PPC

// ====================================================================================================
#elif (ZCONFIG_Compiler_MSVC == ZCONFIG_Compiler) && (ZCONFIG_Processor_x86 == ZCONFIG_Processor)
#pragma mark -
#pragma mark * MSVC/Intel x86

// ====================================================================================================
#elif (ZCONFIG_Compiler_CodeWarrior == ZCONFIG_Compiler) && (ZCONFIG_Processor_x86 == ZCONFIG_Processor)
#pragma mark -
#pragma mark * CodeWarrior/Intel x86

#define ZAtomic_Op_Macro(theOp) \
	int oldValue; \
	__asm \
		{ \
		__asm mov edx, inValueAddress \
		__asm loop: mov eax, [edx] \
		__asm mov ebx, eax \
		__asm theOp ebx, inParam \
		__asm lock cmpxchg [edx], ebx \
		__asm jne loop \
		__asm mov oldValue, eax \
		} \
	return oldValue;

int ZAtomic_Set(ZAtomic_t* inValueAddress, int inParam)
	{
	asm
		{
		mov eax, inParam
		mov edx, inValueAddress
		lock xchg [edx], eax
		mov inParam, eax
		}
	return inParam;
	}

int ZAtomic_Add(ZAtomic_t* inValueAddress, int inParam)
	{
	asm
		{
		mov eax, inParam
		mov edx, inValueAddress
		lock xadd [edx], eax
		mov inParam, eax
		}
	return inParam;
	}

int ZAtomic_And(ZAtomic_t* inValueAddress, int inParam)
	{
	ZAtomic_Op_Macro(and)
	}

int ZAtomic_Or(ZAtomic_t* inValueAddress, int inParam)
	{
	ZAtomic_Op_Macro(or)
	}

int ZAtomic_Xor(ZAtomic_t* inValueAddress, int inParam)
	{
	ZAtomic_Op_Macro(xor)
	}

bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress)
	{
	bool isZero;
	asm
		{
		mov edx, inValueAddress
		lock dec [edx]
		sete isZero
		}
	return isZero;
	}

void ZAtomic_Inc(ZAtomic_t* inValueAddress)
	{
	asm
		{
		mov edx, inValueAddress
		lock inc [edx]
		}
	}

void ZAtomic_Dec(ZAtomic_t* inValueAddress)
	{
	asm
		{
		mov edx, inValueAddress
		lock dec [edx]
		}
	}

#undef ZAtomic_Op_Macro

// ====================================================================================================
#elif (ZCONFIG_Compiler_MSVC == ZCONFIG_Compiler) && (ZCONFIG_Processor_x86 == ZCONFIG_Processor)
#pragma mark -
#pragma mark * MSVC/Intel x86

int ZAtomic_Set(ZAtomic_t* inValueAddress, int inParam)
	{
	asm
		{
		lock xchg [inValueAddress], inParam
		}
	return inParam;
	}

int ZAtomic_Add(ZAtomic_t* inValueAddress, register int inParam)
	{
	asm
		{
		lock xadd [edx], inParam
		}
	return inParam;
	}

int ZAtomic_And(ZAtomic_t* inValueAddress, int inParam)
	{
	int oldValue;
	asm
		{ 
	loop: 
		mov eax, [inValueAddress] 
		mov ebx, eax 
		and ebx, inParam 
		lock cmpxchg [edx], ebx 
		jne loop 
		mov oldValue, eax 
		} 
	return oldValue;
	}

int ZAtomic_Or(ZAtomic_t* inValueAddress, int inParam)
	{
	int oldValue;
	asm
		{ 
	loop: 
		mov eax, [inValueAddress] 
		mov ebx, eax 
		or ebx, inParam 
		lock cmpxchg [edx], ebx 
		jne loop 
		mov oldValue, eax 
		} 
	return oldValue;
	}

int ZAtomic_Xor(ZAtomic_t* inValueAddress, int inParam)
	{
	int oldValue;
	asm
		{ 
	loop: 
		mov eax, [inValueAddress] 
		mov ebx, eax 
		xor ebx, inParam 
		lock cmpxchg [edx], ebx 
		jne loop 
		mov oldValue, eax 
		} 
	return oldValue;
	}

bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress)
	{
	bool isZero;
	asm
		{
		lock dec [inValueAddress]
		sete isZero
		}
	return isZero;
	}

void ZAtomic_Inc(ZAtomic_t* inValueAddress)
	{
	asm
		{
		lock inc [inValueAddress]
		}
	}

void ZAtomic_Dec(ZAtomic_t* inValueAddress)
	{
	asm
		{
		lock dec [inValueAddress]
		}
	}

// ====================================================================================================
#else // ZCONFIG_Compiler/ZCONFIG_Processor
#pragma mark -
#pragma mark * Dumb version

#include "ZThread.h"

static ZMutexNR sMutexNR;

int ZAtomic_Set(register ZAtomic_t* inValueAddress, int inParam)
	{
	sMutexNR.Acquire();
	int priorValue = inValueAddress->fValue;
	inValueAddress->fValue = inParam;
	sMutexNR.Release();
	return priorValue;
	}

int ZAtomic_Add(ZAtomic_t* inValueAddress, int inParam)
	{
	sMutexNR.Acquire();
	int priorValue = inValueAddress->fValue;
	inValueAddress->fValue = priorValue + inParam;
	sMutexNR.Release();
	return priorValue;
	}

int ZAtomic_And(ZAtomic_t* inValueAddress, int inParam)
	{
	sMutexNR.Acquire();
	int priorValue = inValueAddress->fValue;
	inValueAddress->fValue = priorValue & inParam;
	sMutexNR.Release();
	return priorValue;
	}

int ZAtomic_Or(ZAtomic_t* inValueAddress, int inParam)
	{
	sMutexNR.Acquire();
	int priorValue = inValueAddress->fValue;
	inValueAddress->fValue = priorValue | inParam;
	sMutexNR.Release();
	return priorValue;
	}

int ZAtomic_Xor(ZAtomic_t* inValueAddress, int inParam)
	{
	sMutexNR.Acquire();
	int priorValue = inValueAddress->fValue;
	inValueAddress->fValue = priorValue ^ inParam;
	sMutexNR.Release();
	return priorValue;
	}

bool ZAtomic_DecAndTest(ZAtomic_t* inValueAddress)
	{
	sMutexNR.Acquire();
	bool isZero = inValueAddress->fValue-- == 0;
	sMutexNR.Release();
	return isZero;
	}

void ZAtomic_Inc(ZAtomic_t* inValueAddress)
	{
	sMutexNR.Acquire();
	++inValueAddress->fValue;
	sMutexNR.Release();
	}

void ZAtomic_Dec(ZAtomic_t* inValueAddress)
	{
	sMutexNR.Acquire();
	--inValueAddress->fValue;
	sMutexNR.Release();
	}

#endif // ZCONFIG_Compiler/ZCONFIG_Processor

// ====================================================================================================
