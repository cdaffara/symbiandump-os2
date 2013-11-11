/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
 @publishedAll
 @released
*/

#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <_ansi.h>

/** see MSVC 4.0 SETJMP.H & E32STD.H class TTrap
 */
#define _JBLEN	16		
#define _JBTYPE	unsigned long

typedef	_JBTYPE jmp_buf[_JBLEN];

IMPORT_C void longjmp(jmp_buf __jmpb, int __retval);
IMPORT_C int setjmp(jmp_buf __jmpb);

#ifdef __cplusplus
}
#endif
#endif /* _SETJMP_H_ */

