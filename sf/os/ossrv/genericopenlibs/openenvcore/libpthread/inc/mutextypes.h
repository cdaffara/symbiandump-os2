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
* Name        : mutextypes.h
* Part of     : pthread
* Interface   : POSIX, pthread
* POSIX implementation of mutexes on Symbian
* Version     :
*
*/



#ifndef  MUTEXTYPES_H
#define  MUTEXTYPES_H

#include <stddef.h>
#include <limits.h>
#include <e32def.h>
#include <e32std.h>

#include <errno.h>
#include <stdlib.h>

#include "semaphore.h"
#include "pthread.h"
#include "sysif.h"

struct _pthread_mutex_t
{
    int iSharedType;
    int iMutexType;
    RMutex iDataLock;
    bool  iDataLockCreated;
    RSemaphore iSignalSemaphore;
    bool iSemaphoreCreated;
    int iCount; //for recursive mutexes
    int iSemaphoreState;
    pthread_t iLockingThread;
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


#define SEM_FREE -1

#define PTHREAD_MUTEX_ATTRIBUTE_DEFAULT_INITIALIZER  { _EInitialized, PTHREAD_PROCESS_PRIVATE,PTHREAD_MUTEX_DEFAULT }
#define PTHREAD_MUTEX_ATTRIBUTE_RECURSIVE_INITIALIZER  { _EInitialized, PTHREAD_PROCESS_PRIVATE,PTHREAD_MUTEX_RECURSIVE }
#define PTHREAD_MUTEX_ATTRIBUTE_ERRORCHECK_INITIALIZER  { _EInitialized, PTHREAD_PROCESS_PRIVATE,PTHREAD_MUTEX_ERRORCHECK }


#define VALID_MUTEX(mutex) (!mutex || !(_InitOnce(mutex)) || !(mutex->iPtr) || (mutex)->iState != _EInitialized) ? 0 : 1 
#define VALID_MUTEX_ATTR(attr) ( !attr || attr->iState != _EInitialized) ? 0 : 1

extern void* _pthread_getTls();
extern int _pthread_atomicMutexLock(void *tlsPtr);
extern int _pthread_atomicMutexUnlock(void *tlsPtr);
extern long _microsleeptime(const struct timespec* abstime);


extern int _doWait(RSemaphore* aLock, const struct timespec *abstime);

extern bool _InitOnce(pthread_mutex_t* mutex);

extern int _internalMutexLock(pthread_mutex_t * mutex,const struct timespec *abstime);

extern void _closeMutex(pthread_mutex_t* mutex);

#endif  // MUTEXTYPES_H
