/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
*/


#include <kernel\u32std.h>	// need this for __NAKED__, __JUMP and __POPRET

extern "C" {
#ifdef __GCC32__
__NAKED__ long long __divdi3(long long /*dividend*/, long long /*divisor*/)
//
// Dividend in r1:r0, divisor in r3:r2, Return quotient in r1:r0
//
	{
	asm("stmfd sp!, {r4-r8,lr} ");
	asm("eor r8, r1, r3 ");				// sign of result into r8
	asm("movs r1, r1 ");
	asm("bpl 1f ");
	asm("rsbs r0, r0, #0 ");			// ABS(dividend)
	asm("rsc r1, r1, #0 ");
	asm("1: ");
	asm("movs r3, r3 ");
	asm("bpl 2f ");
	asm("rsbs r2, r2, #0 ");			// ABS(divisor)
	asm("rsc r3, r3, #0 ");
	asm("2: ");
	asm("bl UDiv01 ");					// do the division, result in r4,r5
	asm("eors r0, r4, r8, asr #32 ");	// quotient into r1:r0, inverted if quotient -ve
	asm("eors r1, r5, r8, asr #32 ");
	asm("adcs r0, r0, #0 ");		// if quotient -ve, add 1
	asm("adcs r1, r1, #0 ");
	__POPRET("r4-r8,");
	}

__NAKED__ long long __moddi3(long long /*dividend*/, long long /*divisor*/)	/* signed */
	{
	asm("stmfd sp!, {r4-r8,lr} ");
	asm("movs r8, r1 ");				// sign of remainder (=sign of dividend) into r8
	asm("bpl 1f ");
	asm("rsbs r0, r0, #0 ");			// ABS(dividend)
	asm("rsc r1, r1, #0 ");
	asm("1: ");
	asm("movs r3, r3 ");
	asm("bpl 2f ");
	asm("rsbs r2, r2, #0 ");			// ABS(divisor)
	asm("rsc r3, r3, #0 ");
	asm("2: ");
	asm("bl UDiv01 ");					// do the division, remainder in r3,r6
	asm("eors r0, r3, r8, asr #32 ");	// remainder into r1:r0, inverted if dividend -ve
	asm("eors r1, r6, r8, asr #32 ");
	asm("adcs r0, r0, #0 ");			// if dividend -ve, add 1
	asm("adcs r1, r1, #0 ");
	__POPRET("r4-r8,");
	}

__NAKED__ long long __umoddi3(unsigned long long /*dividend*/, unsigned long long /*divisor*/)	/* unsigned */
	{
	asm("stmfd sp!, {r4-r7,lr} ");
	asm("bl UDiv01 ");					// do the division, remainder in r6:r3
	asm("mov r0, r3 ");
	asm("mov r1, r6 ");
	__POPRET("r4-r7,");
	}

__NAKED__ long long __ashrdi3(long long /*value*/, unsigned int /*count*/)
	{
	asm("cmp r2, #63 ");
	asm("movhi r2, #63 ");			// count>63 same as count=63
	asm("cmp r2, #32 ");
	asm("bcs Asr01 ");				// jump if shift count >=32
	asm("rsb r12, r2, #32 ");		// r12=32-shift count
	asm("mov r0, r0, lsr r2 ");		// shift ls word right
	asm("orr r0, r0, r1, lsl r12 ");	// or in bits shifted out of ms word
	asm("mov r1, r1, asr r2 ");		// shift ms word right
	__JUMP(,lr);
	asm("Asr01: ");
	asm("sub r2, r2, #32 ");		// r2=shift count-32
	asm("mov r0, r1, asr r2 ");		// ls word = ms word >> (count-32)
	asm("mov r1, r1, asr #32 ");	// ms word of result=sign extension of r1
	__JUMP(,lr);
	}

__NAKED__ long long __ashldi3(long long /*value*/, unsigned int /*count*/)
	{
	asm("cmp r2, #63 ");
	asm("movhi r2, #64 ");			// count>63 same as count=64
	asm("cmp r2, #32 ");
	asm("bcs Asl01 ");				// jump if shift count >=32
	asm("rsb r12, r2, #32 ");		// r12=32-shift count
	asm("mov r1, r1, asl r2 ");		// shift ms word left
	asm("orr r1, r1, r0, lsr r12 ");	// or in bits shifted out of ls word
	asm("mov r0, r0, asl r2 ");		// shift ls word left
	__JUMP(,lr);
	asm("Asl01: ");
	asm("sub r2, r2, #32 ");		// r2=shift count-32
	asm("mov r1, r0, asl r2 ");		// result ms word = ls word << (count-32)
	asm("mov r0, #0 ");				// ls word of result is zero
	__JUMP(,lr);
	}

__NAKED__ unsigned long long __lshrdi3(unsigned long long /*value*/, unsigned int /*count*/)
	{
	asm("cmp r2, #63 ");
	asm("movhi r2, #64 ");			// count>63 same as count=64
	asm("cmp r2, #32 ");
	asm("bcs Lsr01 ");				// jump if shift count >=32
	asm("rsb r12, r2, #32 ");		// r12=32-shift count
	asm("mov r0, r0, lsr r2 ");		// shift ls word right
	asm("orr r0, r0, r1, lsl r12 ");	// or in bits shifted out of ms word
	asm("mov r1, r1, lsr r2 ");		// shift ms word right
	__JUMP(,lr);
	asm("Lsr01: ");
	asm("sub r2, r2, #32 ");		// r2=shift count-32
	asm("mov r0, r1, lsr r2 ");		// ls word = ms word >> (count-32)
	asm("mov r1, #0 ");				// ms word of result = 0
	__JUMP(,lr);
	}

__NAKED__ long long __muldi3(long long /*multiplicand*/, long long /*multiplier*/)
	{
	asm("mul r1, r2, r1 ");				// r1=low2*high1
	asm("mov ip, r0 ");					// ip=low1
	asm("mla r1, r0, r3, r1 ");			// r1+=low1*high2
	asm("mov r0, #0 ");
	asm("umlal r0, r1, r2, ip ");		// r1:r0 += high1*low1
	__JUMP(,lr);
	}

__NAKED__ long long __negdi2(long long /*argument*/)
	{
	asm("rsbs r0, r0, #0 ");		// r0=0-r0, set carry
	asm("rscs r1, r1, #0 ");		// r1=0-r1-(1-C)
	__JUMP(,lr);
	}

__NAKED__ unsigned long long __udivmoddi4 (unsigned long long /*dividend*/,
													unsigned long long /*divisor*/,
													unsigned long long* /*p_remainder*/)
	{
	asm("stmfd sp!, {r4-r7,lr} ");
	asm("bl UDiv01 ");					// do the division, quotient in r5:r4 remainder in r6:r3
	asm("ldr r7, [sp, #20] ");			// r7=p_remainder
	asm("mov r0, r4 ");					// r0=quotient low
	asm("stmia r7, {r3,r6} ");			// store remainder
	asm("mov r1, r5 ");					// r0=quotient high
	__POPRET("r4-r7,");
	}

__NAKED__ int __cmpdi2(long long /*a*/, long long /*b*/)
	{
	// return 0 if a<b, 1 if a=b, 2 if a>b
	asm("subs r0, r2, r0 ");
	asm("sbcs r1, r3, r1 ");			// r1:r0 = b-a, set flags
	asm("movlt r0, #2 ");				// if b<a r0=2
	__JUMP(lt,lr);						// if b<a return
	asm("cmpeq r0, #0 ");				// if top word of difference=0, look at bottom
	asm("moveq r0, #1 ");				// if a=b, r0=1
	asm("movne r0, #0 ");				// else r=0
	__JUMP(,lr);
	}

__NAKED__ int __ucmpdi2(unsigned long long /*a*/, unsigned long long /*b*/)
	{
	// return 0 if a<b, 1 if a=b, 2 if a>b
	asm("cmp r1, r3 ");
	asm("cmpeq r0, r2 ");				// compare r1:r0 - r3:r2
	asm("movhi r0, #2 ");				// r0=2 if a>b
	asm("moveq r0, #1 ");				// r0=1 if a=b
	asm("movlo r0, #0 ");				// r0=0 if a<b
	__JUMP(,lr);
	}
#endif

#if defined(__GCC32__)
void __division_by_zero();
#define DIV_BY_ZERO " __division_by_zero "
#elif defined(__ARMCC__)
void __rt_div0 (void);
#define DIV_BY_ZERO " __cpp(__rt_div0) "
#endif

void __division_by_zero()
	{
	RThread().RaiseException(EExcIntegerDivideByZero);
    }

__NAKED__ unsigned long long __udivdi3(unsigned long long /*dividend*/, unsigned long long /*divisor*/)
//
// Dividend in r1:r0, divisor in r3:r2, Return quotient in r1:r0
//
	{
	asm("stmfd sp!, {r4-r7,lr} ");
	asm("bl UDiv01 ");					// do the division, result in r4,r5
	asm("mov r0, r4 ");
	asm("mov r1, r5 ");
	__POPRET("r4-r7,");

	// Unsigned 64-bit division. Dividend in r0,r1, divisor in r2,r3
	// Quotient returned in r4,r5, Remainder in r3,r6
	// Registers r0-r7,r12 used, r8-r11 unmodified
	asm(".global UDiv01 ");
	asm("UDiv01: ");
	asm("movs r3, r3 ");				// check if divisor fits in 32 bits
	asm("bne udiv64a ");				// branch if not
	asm("movs r2, r2 ");				// check if divisor fits in 31 bits
	asm("bmi udiv64e ");				// branch if not
	asm("beq udiv64_divby0 ");			// if divisor=0, branch to error routine

	// Divisor is <0x80000000
	// This means that a 32-bit accumulator is sufficient
	asm("mov r4, #0 ");					// use r3 as acc, result in r4, r5
	asm("mov r5, #0 ");
	asm("mov r6, #8 ");					// do 2 set of 32 iterations
	asm("udiv64b: ");
	asm("adds r1, r1, r1 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r5, r5, r5 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r1, r1, r1 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r5, r5, r5 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r1, r1, r1 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r5, r5, r5 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r1, r1, r1 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r5, r5, r5 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("subs r6, r6, #1 ");			// loop
	asm("bne udiv64b ");
	asm("mov r6, #8 ");					// 2nd set of 32 iterations
	asm("udiv64c: ");
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("subs r6, r6, #1 ");			// loop
	asm("bne udiv64c ");
	__JUMP(,lr);

	// 2^31 <= Divisor < 2^32
	// Need 33-bit accumulator - use carry flag as 33rd bit
	asm("udiv64e: ");
	asm("mov r4, #0 ");					// use r3 as acc, result in r4, r5
	asm("mov r5, #0 ");
	asm("mov r6, #8 ");					// do 2 set of 32 iterations
	asm("udiv64f: ");
	asm("adds r1, r1, r1 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subcs r3, r3, r2 ");
	asm("subccs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r5, r5, r5 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r1, r1, r1 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subcs r3, r3, r2 ");
	asm("subccs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r5, r5, r5 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r1, r1, r1 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subcs r3, r3, r2 ");
	asm("subccs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r5, r5, r5 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r1, r1, r1 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subcs r3, r3, r2 ");
	asm("subccs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r5, r5, r5 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("subs r6, r6, #1 ");			// loop
	asm("bne udiv64f ");
	asm("mov r6, #8 ");					// 2nd set of 32 iterations
	asm("udiv64g: ");
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subcs r3, r3, r2 ");
	asm("subccs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subcs r3, r3, r2 ");
	asm("subccs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subcs r3, r3, r2 ");
	asm("subccs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r3, r3, r3 ");
	asm("subcs r3, r3, r2 ");
	asm("subccs r3, r3, r2 ");			// subtract divisor from acc
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("addcc r3, r3, r2 ");			// if borrow, add back
	asm("subs r6, r6, #1 ");			// loop
	asm("bne udiv64g ");
	__JUMP(,lr);
	
	// Divisor >= 2^32, so quotient < 2^32
	// Use 64 bit accumulator, 32 bit quotient
	asm("udiv64a: ");
	asm("mov r4, #0 ");					// quotient in r4, use r1, r6 as accumulator
	asm("mov r6, #0 ");
	asm("mov r5, #8 ");					// do 32 iterations
	asm("udiv64d: ");
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r1, r1, r1 ");
	asm("adcs r6, r6, r6 ");
	asm("subs r7, r1, r2 ");			// subtract divisor from acc, result into r7,r12
	asm("sbcs r12, r6, r3 ");
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("movcs r1, r7 ");				// if no borrow, update acc
	asm("movcs r6, r12 ");
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r1, r1, r1 ");
	asm("adcs r6, r6, r6 ");
	asm("subs r7, r1, r2 ");			// subtract divisor from acc, result into r7,r12
	asm("sbcs r12, r6, r3 ");
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("movcs r1, r7 ");				// if no borrow, update acc
	asm("movcs r6, r12 ");
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r1, r1, r1 ");
	asm("adcs r6, r6, r6 ");
	asm("subs r7, r1, r2 ");			// subtract divisor from acc, result into r7,r12
	asm("sbcs r12, r6, r3 ");
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("movcs r1, r7 ");				// if no borrow, update acc
	asm("movcs r6, r12 ");
	asm("adds r0, r0, r0 ");			// shift dividend left into acc
	asm("adcs r1, r1, r1 ");
	asm("adcs r6, r6, r6 ");
	asm("subs r7, r1, r2 ");			// subtract divisor from acc, result into r7,r12
	asm("sbcs r12, r6, r3 ");
	asm("adc r4, r4, r4 ");				// shift result bit left into quotient
	asm("movcs r1, r7 ");				// if no borrow, update acc
	asm("movcs r6, r12 ");
	asm("subs r5, r5, #1 ");			// loop
	asm("bne udiv64d ");
	asm("mov r3, r1 ");					// remainder in r3,r6
	__JUMP(,lr);

	asm("udiv64_divby0: ");
	asm("str lr, [sp, #-4]! ");
	asm("bl " DIV_BY_ZERO);
	__POPRET("");
	}

}

