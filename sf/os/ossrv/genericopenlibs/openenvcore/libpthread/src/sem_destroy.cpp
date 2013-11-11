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
// Name     : sem_destroy.cpp
// Part of  : PThread library
// sem_destroy call is implemented.
// Version:
//



#include <e32def.h>
#include <errno.h>

#include "semaphore.h"
#include "semaphoretypes.h"
#include "pthread.h"

EXPORT_C int sem_destroy (sem_t * sem)
{
    if (!sem || !(*sem))
    {
        errno = EINVAL;
        return -1;
    }

    _sem_t* lsem = *sem;
    
    if (_findAndFreeSem(lsem) == _SEM_NOT_FOUND_IN_LIST)
    {
        errno = EINVAL;
        return -1;    
    }    
    
    if(lsem->iState != _sem_t::EInitialized)
    {
        errno = EINVAL;
        return -1;
    }

    if(lsem->iCount <0)
    {
        // Add sem to sem-list
        _addToSemList(lsem);  
    
        errno = EBUSY;
        return -1;
    }

#ifndef NOUSE_INTERNALS     
    void* tlsPtr;

    // Get the TLS pointer
    tlsPtr = _pthread_getTls();
    if(NULL == tlsPtr)
    {
        User::Panic(_L("pthread lib could not initailize"),0);
        errno = EBUSY;
        return -1;
    }
    _pthread_atomicMutexLock(tlsPtr);
#endif

    if(lsem->iState == _sem_t::EDestroyed)
    {
#ifndef NOUSE_INTERNALS     
        _pthread_atomicMutexUnlock(tlsPtr);
#endif
        return 0;
    }

    lsem->iMutex.Wait();
    lsem->iState = _sem_t::EDestroyed;
    lsem->iSemaphore.Close();
    lsem->iMutex.Signal();
    lsem->iMutex.Close();  

    delete lsem; 
    *sem = NULL;

#ifndef NOUSE_INTERNALS       
    _pthread_atomicMutexUnlock(tlsPtr);
#endif  
    return 0;
}//sem_destroy()

//End of File

