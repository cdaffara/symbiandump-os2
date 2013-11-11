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
// Name     : sem_microsleepwait_np.cpp
// Part of  : PThread library
// sem_microsleepwait_np call is implemented.
// Version:
//



#include <e32def.h>
#include <errno.h>

#include "semaphore.h"
#include "semaphoretypes.h"
#include "pthread.h"


int sem_microsleepwait_np (sem_t* sem, const long aSleepTime)
{
    if (!sem || !(*sem) )
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

    int count = --(lsem->iCount);
    lsem->iMutex.Signal();

    int retval = 0;
    if(count <0)
    {
    //push to cleanup

    retval = lsem->iSemaphore.Wait(aSleepTime);
    if(retval != KErrNone)
    {
        //timeout
        //the semaphore count goes up
        lsem->iMutex.Wait();
        if(lsem->iState != _sem_t::EInitialized)
        {
            errno = EINVAL;
            return -1;
        }
        ++(lsem->iCount);
        lsem->iMutex.Signal();
        errno =  ETIMEDOUT;
        return -1;
    }

    //pop from cleanup
    }
    return 0;

}//sem_microsleepwait_np()

//End of File
