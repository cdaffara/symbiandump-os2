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
* Name        : semaphoretypes.h
* Part of     : semaphore 
* POSIX implementation of semaphores on Symbian
* Version     :
*
*/



#ifndef  SEMAPHORETYPES_H
#define  SEMAPHORETYPES_H

#include <e32def.h>
#include <e32std.h>
#include "pthread.h"
#include "threadglobals.h"

enum
{
    _SEM_FOUND_IN_LIST,
    _SEM_NOT_FOUND_IN_LIST
};

extern void* _pthread_getTls();
extern int _pthread_atomicMutexLock(void *tlsPtr);
extern int _pthread_atomicMutexUnlock(void *tlsPtr);

#ifdef __SYMBIAN_COMPILE_UNUSED__
extern int pthread_cancel (pthread_t thread);
extern int pthread_setcancelstate (int state, int *oldstate);
extern int pthread_setcanceltype (int type, int *oldtype);
#endif

extern void pthread_testcancel (void);

#define KMaxSecsForWait 2148

extern int _cancelable_mutex_wait(_sem_t* asem);
extern long _microsleeptime(const struct timespec* abstime);

extern void _insertIntoSemList(void *tlsPtr, _sem_node_t *newNode);
extern int _addToSemList(_sem_t *newSem);
extern int _freeSemFromList(void *tlsPtr, _sem_t *delSem);
extern int _findAndFreeSem(_sem_t *delSem);




#endif //SEMAPHORETYPES_H

