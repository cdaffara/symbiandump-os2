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
// Name     : sem_post.cpp
// Part of  : PThread library
// sem_post call is implemented.
// Version:
//



#include <e32def.h>
#include <errno.h>

#include "semaphore.h"
#include "semaphoretypes.h"
#include "pthread.h"

EXPORT_C int sem_post (sem_t * sem)
{
    if (!sem || !(*sem))
    {
        errno = EINVAL;
        return -1;
    }

    _sem_t* gsem = *sem;
    if(gsem->iState != _sem_t::EInitialized)
    {
        errno = EINVAL;
        return -1;
    }

    gsem->iMutex.Wait();
    if(gsem->iState != _sem_t::EInitialized)
    {
        errno = EINVAL;
        return -1;
    }

    int count = ++(gsem->iCount);

    if(count <=0)
    {
        //blocked waiters
        gsem->iSemaphore.Signal(); 
    }
    gsem->iMutex.Signal();
    return 0;
}//sem_post()

//End of File
