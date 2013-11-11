/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : isthreaded.c
* Part of     : libc library
* get/set __isthreaded
* This material, including documentation and any related 
* computer programs, is protected by copyright controlled by 
* Nokia. All rights are reserved. Copying, including 
* reproducing, storing, adapting or translating, any 
* or all of this material requires the prior written consent of 
* Nokia. This material also contains confidential 
* information which may not be disclosed to others without the 
* prior written consent of Nokia.
*
*/



#include <_ansi.h>

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

//Definition of _isthreaded which will be used by other module as extern
#ifndef EMULATOR
int _isthreaded = 0;
#endif //EMULATOR

/**
 isthreaded :  To set and get the variable __isthreaded
 in exit.c (libc)
 This function is used by the library libpthread to set the variable 
 __isthreaded to true when pthread_create() is called 
 Returns: __isthreaded.
*/

EXPORT_C int* isthreaded(void)
	{
#ifdef EMULATOR
	return &(GetGlobals()->_g___isthreaded);
#else
	return &_isthreaded;
#endif /* EMULATOR */
	}
