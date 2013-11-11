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
// Name     : sem_wait.cpp
// Part of  : PThread library
// sem_wait call is implemented.
// Version:
//



#include <e32def.h>
#include <errno.h>

#include "semaphore.h"
#include "semaphoretypes.h"
#include "pthread.h"

EXPORT_C int sem_wait (sem_t * sem)
{
    if (!sem || !(*sem))
    {
        errno = EINVAL;
        return -1;
    }

    _sem_t* lsem = *sem;
    if(lsem->iState != _sem_t::EInitialized)
    {
        errno = EINVAL;
        return -1;
    }

    if(_cancelable_mutex_wait(lsem) != 0)
    {
        return -1;
    }

    int prevCount = lsem->iCount;
    lsem->iCount--;    
    lsem->iMutex.Signal();

    if(prevCount <=0)
    {
        //pthread_cleanup_push();
        lsem->iSemaphore.Wait();
        //pthread_cleanup_pop();
    }
    return 0;
}//sem_wait()

// End of File

