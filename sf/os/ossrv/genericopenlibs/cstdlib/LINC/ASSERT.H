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




#ifdef __cplusplus
extern "C" {
#endif

#include "_ansi.h"

#undef assert
/**
@publishedAll
@released
*/
#ifdef NDEBUG           /* required by ANSI standard */
#define assert(p)  	((void)0)
#else
#define assert(e)       ((e) ? (void)0 : __assert(__FILE__, __LINE__, #e))
#endif /* NDEBUG */
/**
@publishedAll
@released
*/
IMPORT_C void __assert(const char *, int, const char *);

#ifdef __cplusplus
}
#endif
