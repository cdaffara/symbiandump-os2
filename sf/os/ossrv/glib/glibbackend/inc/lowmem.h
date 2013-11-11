/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


 
#ifndef __LOW_MEM_H__
#define __LOW_MEM_H__

#include <setjmp.h>
#include <_ansi.h>
#include <glowmem.h>

#ifdef __cplusplus
extern "C"
{
#endif    /* __cplusplus */

IMPORT_C mem_info * get_thread_specific_data();
IMPORT_C int set_thread_specific_data(mem_info *m);
IMPORT_C int push(cleanUpStack *cs,void *ptr);
IMPORT_C void * pop(cleanUpStack *cs);
IMPORT_C void clearCleanUpStack(cleanUpStack *cs);
IMPORT_C void destroyCleanUpStack(cleanUpStack *cs);
IMPORT_C void findAndDestroy(cleanUpStack *cs,void *ptr);
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif /* __LOW_MEM_H__ */
