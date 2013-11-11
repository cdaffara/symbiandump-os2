; Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
; All rights reserved.
; This component and the accompanying materials are made available
; under the terms of "Eclipse Public License v1.0"
; which accompanies this distribution, and is available
; at the URL "http://www.eclipse.org/legal/epl-v10.html".
;
; Initial Contributors:
; Nokia Corporation - initial contribution.
;
; Contributors:
;
; Description:
; TEF\ulogger\performance\te_stackinfo.s
;

	AREA |te-bkpt$$Code|, CODE, READONLY, ALIGN=6

	CODE32

	; UPT

; 
;



 	;EXPORT Te_Get_Current_Stack_Pointer
 	EXPORT Te_Fill_Remaining_Stack
 	EXPORT Te_Get_Stack_Size


Te_Fill_Remaining_Stack
	mov r2,r0 ; r2 = stacklimit
	mov r0,sp ; r0 = loop address to store deadbeef
	ldr r3,=0xDEADBEEF
	subs r0,r0,#4
my_name
	str r3,[r0]
	subs r0,r0,#4
	cmp r0,r2
	bge my_name	
	mov r0,sp
	bx lr
	

Te_Get_Stack_Size
	mov r2, r0
	mov r0,sp
	ldr r3,=0xDEADBEEF
new_name
	ldr r1,[r2]
	cmp r3,r1
	addeq r2,r2,#4
	beq new_name
	mov r0,r2
	bx lr

	END
	
; End of file - te_stackinfo.s
