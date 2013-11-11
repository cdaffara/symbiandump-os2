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
// Name        : pthread_mutex_destroy.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of mutexes on Symbian
// Version     :
//



#include "mutextypes.h"

EXPORT_C int pthread_mutex_destroy (pthread_mutex_t * mutex)
{

    if(!mutex)
    {
        return EINVAL;
    }

    if(mutex->iState == _EDestroyed)
    {
        return EINVAL;
    }

    if(mutex->iState != _EInitialized)
    {
        return 0;
    }

    _pthread_mutex_t* pMutex = mutex->iPtr;

    if(pMutex->iDataLock.IsHeld())
    {
        return EBUSY;
    }

    pMutex->iDataLock.Wait();
    if(pMutex->iSemaphoreState != SEM_FREE)
    {
        pMutex->iDataLock.Signal();
        return EBUSY;
    }

    mutex->iState = _EDestroyed;
    pMutex->iDataLock.Signal();
    _closeMutex(mutex);
    return 0;
}


// End of File
