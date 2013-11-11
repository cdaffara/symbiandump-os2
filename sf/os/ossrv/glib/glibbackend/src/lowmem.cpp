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
#include "glibbackend_wsd.h"
#include <pthread.h>
#include <stdlib.h>

#if EMULATOR

PLS(key,lowmem,pthread_key_t)
PLS(key_once,lowmem,pthread_once_t)

#define key (*FUNCTION_NAME(key,lowmem )())
#define key_once (*FUNCTION_NAME(key_once,lowmem )())

#else
pthread_key_t key;
pthread_once_t key_once = PTHREAD_ONCE_INIT;
#endif /* EMULATOR */

static void make_key()
{
	pthread_key_create(&key,NULL);
}

EXPORT_C mem_info * get_thread_specific_data()
{
	pthread_once(&key_once, make_key);
	return (mem_info *)pthread_getspecific(key);
}

/* This function sets the thread specfic data which is of the type 	*
 * struct mem_info.The function return 0 in case of success and a	*
 * non zero value in case of failure.								*
 */
EXPORT_C int set_thread_specific_data(mem_info *m)
{
	pthread_once(&key_once, make_key);
	if(pthread_setspecific(key,(void *)m))
	{
		return 1;
	}
	
	return 0;
}

EXPORT_C int push(cleanUpStack *cs,void *ptr)
{
	if((*cs).top == 999)	
		return -1;
	else
		(*cs).ptr[++((*cs).top)] = ptr;
	return 0;
}

EXPORT_C void * pop(cleanUpStack *cs)
{
	return (*cs).ptr[((*cs).top)--];
}

EXPORT_C void clearCleanUpStack(cleanUpStack *cs)
{
	(*cs).top = -1;
}

EXPORT_C void destroyCleanUpStack(cleanUpStack *cs)
{
	while((*cs).top != -1)
	{
		free((*cs).ptr[((*cs).top)--]);
	}
}


EXPORT_C void findAndDestroy(cleanUpStack *cs,void *ptr)
{
	int i;
	for(i = 0; i < (*cs).top ; i++ )
	{
		if((*cs).ptr[i] == ptr)
		{
			(*cs).ptr[i] = NULL;
			break;
		}
	}
}
