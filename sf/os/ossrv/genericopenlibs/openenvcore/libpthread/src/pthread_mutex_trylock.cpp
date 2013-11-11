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
// Name        : pthread_mutex_trylock.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of mutexes on Symbian
// Version     :
//



#include "mutextypes.h"

EXPORT_C int pthread_mutex_trylock (pthread_mutex_t * mutex)
{
    if( !VALID_MUTEX(mutex) )
    {
        return EINVAL;
    }
    _pthread_mutex_t* pMutex = mutex->iPtr;

    pMutex->iDataLock.Wait();
    if(mutex->iState == _EDestroyed)
    {
       pMutex->iDataLock.Signal();
       return EINVAL;
    }

    //return immediately if mutex is locked, unless the mutex is a recursive 
    // mutex and we are locking again.  
    if(pMutex->iSemaphoreState != SEM_FREE)
    {
        if(pMutex->iMutexType != PTHREAD_MUTEX_RECURSIVE || 
           pMutex->iLockingThread != pthread_self())
        {
           pMutex->iDataLock.Signal();
           return EBUSY;  
        }
    }

    int retval = 0;
    switch(pMutex->iMutexType)
    {
        case  PTHREAD_MUTEX_NORMAL:
        {
         
            pMutex->iSignalSemaphore.Wait();
            pMutex->iSemaphoreState++; 
            pMutex->iLockingThread = pthread_self();
            break;
        }

        case  PTHREAD_MUTEX_RECURSIVE:
        {
            if(pMutex->iCount)
            {
                pMutex->iCount++;       
            }
            else
            {
                pMutex->iSignalSemaphore.Wait();
                pMutex->iCount = 1; 
                pMutex->iSemaphoreState++;
                pMutex->iLockingThread = pthread_self();
            }
            break;
        }

        case  PTHREAD_MUTEX_ERRORCHECK:
        {
            pMutex->iSignalSemaphore.Wait();
            pMutex->iSemaphoreState++;   
            pMutex->iLockingThread = pthread_self();
            break;
        }
            
        default:
            retval = ENOSYS;
            break;

    }
    pMutex->iDataLock.Signal();
    return retval;
 }


// End of File
