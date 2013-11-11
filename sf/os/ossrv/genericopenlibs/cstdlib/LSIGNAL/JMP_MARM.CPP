// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <setjmp.h>

extern "C" {

EXPORT_C int setjmp(jmp_buf __jmpb)
	{
#ifdef __MARM_THUMB__
	//the function destroys the contents of {r0,r1,r2,r3} registers
	//r0 is used to hold the retrun value
	//r1,r2,r3 - they are used usually to hold functions arguments/temp data and are
	//not expected to preserve their values between functions calls.
	asm("stmia r0!, {r4-r7}");//bytes {0..15} are occupied by {r4,r5,r6,r7} registers
	asm("mov r2, r8");
	asm("mov r3, r9");
	asm("mov r4, r10");
	asm("mov r5, r11");
	asm("mov r6, sp");
	asm("mov r7, lr");
	asm("stmia r0!, {r2-r7}");//bytes {16..39} are occupied by {r8,r9,r10,r11,sp,lr} registers
	asm("sub r0, #40");//r0 points now to the beginning of __jmpb
	asm("ldmia r0!, {r4-r7}");//restore the contents of {r4,r5,r6,r7} registers
#else
	asm("stmia	r0, {r4-r11, sp, lr}	"); // Save the context
#endif
	return(0);
	}

EXPORT_C void longjmp(jmp_buf __jmpb, int __retval)
	{
#ifdef __MARM_THUMB__
	asm("mov	r3, r0"); // save the jmp_buf pointer
	asm("add	r3, #16");		//add on 16 to get to high registers
	asm("ldmia	r3!, {r4-r7}");  
	asm("mov	r8,r4");
	asm("mov	r9,r5");
	asm("mov	r10,r6");
	asm("mov	r11,r7");
	asm("ldmia	r3!, {r4-r5}");	//get sp and lr
	asm("mov	sp, r4");
	asm("mov	lr, r5");
	asm("mov	r3, r0");		//get the jmp_buf ptr again
	asm("ldmia  r3!, {r4-r7}");	//and restore the lo regs
	asm("mov	r0, r1"); // return(__retval)
	asm("cmp    r0, #0");
	asm("bne    1f");
	asm("mov    r0, #1");
	asm("1:");
#else
	asm("ldmia	r0, {r4-r11, sp, lr}	"); // Restore the context
	asm("movs	r0, r1"); // return(__retval == 0 ? 1 : retval)
	asm("moveq  r0, #1");
#endif
	return;
	}

} // extern "C"
