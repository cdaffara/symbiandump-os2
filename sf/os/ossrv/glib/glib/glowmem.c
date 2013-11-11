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



#include "lowmem.h"
#include "glibbackend.h"
#include <glowmem.h>

EXPORT_C mem_info * _get_thread_specific_data()
{
	return get_thread_specific_data();
}


/* This function sets the thread specfic data which is of the type 	*
 * struct mem_info.The function return 0 in case of success and a	*
 * non zero value in case of failure.								*
 */
EXPORT_C int _set_thread_specific_data(mem_info *m)
{
	return set_thread_specific_data(m);
}

/* This function pushes the pointer on the top of the stack and returns *
 * the new top of the stack												*
 */
EXPORT_C int _push(cleanUpStack *cs,void *ptr)
{
	return push(cs,ptr);
}

/* This functions pops the pointer at the top of the stack and saves   *
 * the pointer in arg ptr. The top of the stack in returned.		   *
 */
EXPORT_C void *_pop(cleanUpStack *cs)
{
	return pop(cs);
}

/* This function searches and deleted a member in the stack and returns *
 * the top of the stack													*
 */
EXPORT_C void _findAndDestroy(cleanUpStack *cs,void *ptr)
{
	findAndDestroy(cs,ptr);
}

/* This function deletes the entire stack. It also calls free on all 	*
 * the pointer that are stored in the stack								*
 */
EXPORT_C void _destroCleanUpStack(cleanUpStack *cs)
{
	destroyCleanUpStack(cs);
}

/* This function deletes the stack and frees all the memory				*
 */
EXPORT_C void _clearCleanUpStack(cleanUpStack *cs)
{
	clearCleanUpStack(cs);
}

EXPORT_C void * _pAlloc(size_t size)
{
	return pAlloc(size);
}

EXPORT_C void _dummy1()
{
	
}

EXPORT_C void _dummy2()
{
	
}

EXPORT_C void _dummy3()
{
	
}

