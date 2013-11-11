// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name     : sem_init.cpp
// Part of  : PThread library
// sem_init call is implemented.
// Version:
//



#include <e32def.h>
#include <errno.h>

#include "semaphore.h"
#include "semaphoretypes.h"
#include "pthread.h"

EXPORT_C int sem_init (sem_t * sem, int pshared, unsigned int value)
{
    //no check for sem_init called twice or more
    //also no locking of semaphore while initializing that is, if T1 has 
    //called sem_init and T2 calls sem_post or sem_wait concurrently,
    //the result is undefined standard also clearly states that same sem_t 
    //variable should be passed around and not a copy of the variable.
    //implementation will not work with copy of sem_t being passed around

    if (pshared != 0)
    {
        errno = ENOSYS;
        return -1;
    }

    if (value > (unsigned int)SEM_VALUE_MAX || (!sem) )
    {
        errno = EINVAL;
        return -1;
    }

#ifndef NOUSE_INTERNALS  
    void* tlsPtr;

    // Get the TLS pointer
    tlsPtr = _pthread_getTls();
    if(NULL== tlsPtr)
    {
        errno = ENOSPC;
        return -1;
    }
    _pthread_atomicMutexLock(tlsPtr);
#endif

    _sem_t* lsem;
    lsem =  new _sem_t; 
    *sem = lsem;
    if(!lsem)
    {
        errno= ENOSPC;
#ifndef NOUSE_INTERNALS     
        _pthread_atomicMutexUnlock(tlsPtr);
#endif      
        return -1;
    }

    if(lsem->iMutex.CreateLocal() != KErrNone)
    {
        lsem->iState = _sem_t::EInvalid;
        errno = ENOSPC;
        delete lsem;
        *sem = NULL;
#ifndef NOUSE_INTERNALS     
        _pthread_atomicMutexUnlock(tlsPtr);
#endif      
        return -1;
    }

    if(lsem->iSemaphore.CreateLocal(0) != KErrNone)
    {
        lsem->iMutex.Close();
        lsem->iState = _sem_t::EInvalid;
        errno = ENOSPC;
        delete lsem;
        *sem = NULL;        
#ifndef NOUSE_INTERNALS         
        _pthread_atomicMutexUnlock(tlsPtr);
#endif      
        return -1;
    }

    lsem->iCount = value;
    lsem->iState = _sem_t::EInitialized;

#ifndef NOUSE_INTERNALS       
    _pthread_atomicMutexUnlock(tlsPtr);
#endif

    // Add sem to sem-list
    _addToSemList(*sem);  
    
    return 0;

}//sem_init()           


// End of file

