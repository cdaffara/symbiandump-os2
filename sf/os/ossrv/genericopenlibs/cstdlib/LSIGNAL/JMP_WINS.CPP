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

#ifdef __CW32__
#define _asm asm
#endif

extern "C" {

EXPORT_C int setjmp(jmp_buf __jmpb)
	{
	// preamble does "push ebp; mov ebp,esp;"
	_asm mov eax, __jmpb
	_asm mov [eax],    ebx
	_asm mov [eax+4],  esi
	_asm mov [eax+8],  edi
	_asm mov [eax+12], ebp		// caller's ESP
	_asm mov [eax+16], ds
	_asm mov [eax+20], es
	_asm mov [eax+24], fs
	_asm mov [eax+28], gs
	_asm mov edx, [ebp]
	_asm mov [eax+32], edx		// caller's EBP
	_asm mov edx, [ebp+4]
	_asm mov [eax+36], edx		// return address EIP
	return(0);
	}

EXPORT_C void longjmp(jmp_buf __jmpb, int __retval)
	{
	__jmpb[10]= (__retval == 0) ? 1 : __retval;		// so we can return it after changing ESP/EBP
	_asm mov eax, __jmpb
	_asm mov ebp, [eax+12]
	_asm mov esp, ebp		// restore setjmp ESP (and leave EBP==ESP)
	_asm mov ebx, [eax]
	_asm mov esi, [eax+4]
	_asm mov edi, [eax+8]
	_asm mov ds, [eax+16]
	_asm mov es, [eax+20]
	_asm mov fs, [eax+24]
	_asm mov gs, [eax+28]
	_asm mov edx, [eax+32]
	_asm mov [ebp], edx		// put setjmp caller's EBP back into stack
	_asm mov edx, [eax+36]
	_asm mov [ebp+4], edx		// put setjmp caller's EIP back into stack
	_asm mov eax, [eax+40]
	_asm pop ebp
	_asm ret 
	}

} // extern "C"