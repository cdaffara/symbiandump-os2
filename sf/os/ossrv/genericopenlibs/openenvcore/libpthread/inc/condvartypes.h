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
* Name        : condvartypes.h
* Part of     : pthread
* Interface   : POSIX, pthread
* POSIX implementation of pthread on Symbian
* Version     :
*
*/



#ifndef  CONDVARTYPES_H
#define  CONDVARTYPES_H

#include <stddef.h>
#include <limits.h>
#include <e32def.h>
#include <e32std.h>

#include <errno.h>
#include <stdlib.h>

#include "pthread.h"
#include "semaphore.h"
#include "sysif.h"

extern void* _pthread_getTls();
extern int _pthread_atomicMutexLock(void *tlsPtr);
extern int _pthread_atomicMutexUnlock(void *tlsPtr);

#ifdef __SYMBIAN_COMPILE_UNUSED__
extern int pthread_cancel (pthread_t thread);
extern int pthread_setcancelstate (int state, int *oldstate);
extern int pthread_setcanceltype (int type, int *oldtype);
#endif

extern void pthread_testcancel (void);

struct _CondNode
{
    sem_t       iSemaphore;
    _CondNode*      iNext;
	/*******************************************************************
	Overloading new and delete operators so that they will
	allocate and deallocare memory from/to the private heap of backend
	********************************************************************/
	inline TAny* operator new(TUint aSize, TAny* aBase) __NO_THROW
		{
		Mem::FillZ(aBase, aSize); return aBase;
		}
		
	inline TAny* operator new(TUint aSize) __NO_THROW
		{
		return Backend()->Alloc(aSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave)
		{
		TAny* ptr = Backend()->Alloc(aSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
		
	inline TAny* operator new(TUint aSize, TUint aExtraSize) __NO_THROW
		{
		return Backend()->Alloc(aSize + aExtraSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave, TUint aExtraSize)
		{
		TAny* ptr = Backend()->Alloc(aSize + aExtraSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
	
	inline void operator delete(TAny *aPtr) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
};

extern bool _QueueInit(_CondQueue* aQueue);
extern int _QueueDestroy(_CondQueue* aQueue);
extern int _internalCondWait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                             const struct timespec *abstime);

extern bool _staticCondInit(pthread_cond_t* cond);
extern void  _QueueUnlock(_CondQueue* aQueue);
extern void  _QueueLock(_CondQueue* aQueue);
extern _CondNode* _QueueRemoveHeadNoLock(_CondQueue* aQueue);
extern _CondNode* _QueueRemoveHead(_CondQueue* aQueue);
extern bool _QueueRemove(_CondQueue* aQueue, _CondNode* aNode);
extern bool _QueueAdd(_CondQueue* aQueue, _CondNode* aNode);

#endif //  CONDVARTYPES_H
//End of File

