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
// Name        : pthread_mutex_unlock.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of mutexes on Symbian
// Version     :
//



#include "mutextypes.h"

EXPORT_C int pthread_mutex_unlock (pthread_mutex_t * mutex)
{
    if( !VALID_MUTEX(mutex) )
    {
        return EINVAL;
    }

    _pthread_mutex_t* pMutex = mutex->iPtr;
    pthread_t self;
    int retval = 0;
    pMutex->iDataLock.Wait();
    if(mutex->iState == _EDestroyed)
    {
       pMutex->iDataLock.Signal();
       return EINVAL;
    }
    switch(pMutex->iMutexType)
    {
        case  PTHREAD_MUTEX_NORMAL:
        {
            self = pthread_self();
            if( (pMutex->iLockingThread != self) )
            {
                retval = EPERM;
                break;
            }
            pMutex->iSignalSemaphore.Signal();
            pMutex->iSemaphoreState--;
            pMutex->iLockingThread = 0;
            break;
        }

        case  PTHREAD_MUTEX_RECURSIVE:
        {
            self = pthread_self();
            if(pMutex->iLockingThread == self)
            {
            
            pMutex->iCount--;  
            if(pMutex->iCount == 0)
               {
                  pMutex->iSemaphoreState--; 
                  pMutex->iLockingThread = 0;
                  pMutex->iSignalSemaphore.Signal();
               }
            }
            else
            {
                retval = EPERM;
            }
            break;  
        }


        case  PTHREAD_MUTEX_ERRORCHECK:
        {
            self = pthread_self();
            if( (pMutex->iLockingThread != self) )
            {
                retval = EPERM;
                break;
            }
            pMutex->iSignalSemaphore.Signal();
            pMutex->iSemaphoreState--;
            pMutex->iLockingThread = 0;
            break;
        }

        default:
            retval = ENOSYS;
            break;
    } //switch
    pMutex->iDataLock.Signal();
    return retval;

}


// End of File

