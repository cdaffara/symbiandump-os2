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
// Name        : mutex.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of mutexes on Symbian
// Version     :
//



#include "mutextypes.h"

/*
This function waits on the aLock semaphore. 
*/
int _doWait(RSemaphore* aLock, const struct timespec *abstime)
{
    int retval = 0;
    if(!abstime)
    {
        aLock->Wait();
    }
    else
    {
        long lSleepTime = _microsleeptime(abstime);
        if(lSleepTime <0)
        {
            if (EINVAL == errno)
            {
                errno = 0;
                return EINVAL;    
            }
            else if(ETIMEDOUT == errno)
            {
                errno = 0;
                return ETIMEDOUT;
            }
            
        }

        retval = aLock->Wait(lSleepTime);
        if(retval != KErrNone)
        {
            return  ETIMEDOUT;
        }

    }
    return 0;
}

/*
This internal function destroys the mutex object.
*/
void _closeMutex(pthread_mutex_t* mutex)
{
    _pthread_mutex_t* pMutex = mutex->iPtr;
    if(pMutex)
    {
        if(pMutex->iSemaphoreCreated)
        {
            pMutex->iSignalSemaphore.Close();       
        }
        if(pMutex->iDataLockCreated)
        {
            pMutex->iDataLock.Close();  
        }
        mutex->iPtr = NULL;
        delete pMutex; 
    }
    return;
}

int _mutexCreate (pthread_mutex_t * mutex, 
                  const pthread_mutexattr_t* attr)
{

    if( attr->iSharedType != PTHREAD_PROCESS_PRIVATE)
    {
        return ENOSYS;
    }

    mutex->iState = _EInvalid;
    _pthread_mutex_t* pMutex = new _pthread_mutex_t; 
    mutex->iPtr = pMutex;
    if(!pMutex)
    {
        return ENOMEM;
    }

    pMutex->iSharedType = attr->iSharedType;
    pMutex->iMutexType = attr->iMutexType;
    pMutex->iDataLockCreated = false;
    pMutex->iSemaphoreCreated = false;

    if(pMutex->iSignalSemaphore.CreateLocal(1) != KErrNone)
    {
        _closeMutex(mutex);
        return EAGAIN;
    }
    pMutex->iSemaphoreCreated = true;

    if(pMutex->iDataLock.CreateLocal() != KErrNone)
    {
        _closeMutex(mutex);
        return EAGAIN;
    }
    pMutex->iDataLockCreated = true;

    pMutex->iCount = 0;
    pMutex->iSemaphoreState = SEM_FREE;
    pMutex->iLockingThread = 0;
    mutex->iState = _EInitialized;
    return 0;
}


/*
This function is an internal  harness function used to resolve the race to dynamically initialize a static mutex
*/
bool _InitOnce(pthread_mutex_t* mutex)
{

    pthread_mutexattr_t defaultattributes = 
                            PTHREAD_MUTEX_ATTRIBUTE_DEFAULT_INITIALIZER;
    pthread_mutexattr_t recursiveattributes = 
                            PTHREAD_MUTEX_ATTRIBUTE_RECURSIVE_INITIALIZER;
    pthread_mutexattr_t errorcheckattributes = 
                            PTHREAD_MUTEX_ATTRIBUTE_ERRORCHECK_INITIALIZER;

    if(mutex->iState == _EInitialized)
    {
        return true;
    }
   
    mutex->iReentry = 0xBABA;

    pthread_mutexattr_t* attr;
    switch(mutex->iState)
    {
        case _ENeedsNormalInit:
        {
            attr = &defaultattributes;
            break;
        }

        case _ENeedsRecursiveInit:
        {
            attr = &recursiveattributes;
            break;
        }

        case _ENeedsErrorCheckInit:
        {
            attr = &errorcheckattributes;
            break;
        }

        default:
            return false;
    }

#ifndef NOUSE_INTERNALS
    void* tlsPtr;

    // Get the TLS pointer
    tlsPtr = _pthread_getTls(); 

    if(!tlsPtr)
    {
        return false;
    }
#endif    

    int retval=0;
#ifndef NOUSE_INTERNALS
    _pthread_atomicMutexLock(tlsPtr);
#endif
    if(mutex->iState != _EInitialized)
    {
        retval = _mutexCreate(mutex,attr);
    }
#ifndef NOUSE_INTERNALS   
    _pthread_atomicMutexUnlock(tlsPtr);
#endif   
     
    if(retval == 0)
    {
        return true;
    }
    else 
    {
        return false;
    }
}


/*
This function is an internal function used to provide the POSIX mutex lock 
functionality.
*/
int _internalMutexLock(pthread_mutex_t * mutex,const struct timespec *abstime)
{
    if( !VALID_MUTEX(mutex) )
    {
        return EINVAL;
    }

    _pthread_mutex_t* pMutex = mutex->iPtr;


    int retval = 0;
    pthread_t self;
    switch(pMutex->iMutexType)
    {
        case  PTHREAD_MUTEX_NORMAL:
        	{
        	  self = pthread_self();          
        	  pMutex->iDataLock.Wait();
        	  if(mutex->iState == _EDestroyed)
        	  {
        	     pMutex->iDataLock.Signal();
        	     return EINVAL;
        	  }
        	  pMutex->iSemaphoreState++;   
        	  pMutex->iDataLock.Signal();
        	           
        	  retval = _doWait(&pMutex->iSignalSemaphore,abstime);
        	  if(retval == 0)
        	  {
        	     pMutex->iDataLock.Wait();
        	     if(mutex->iState == _EDestroyed)
        	     {
        	        pMutex->iDataLock.Signal();
        	        return EINVAL;
        	     }
        	     pMutex->iLockingThread  = self;
        	     pMutex->iDataLock.Signal();
               }
        	   break;
        	 }

        case  PTHREAD_MUTEX_RECURSIVE:
        	
        {
            self = pthread_self();
            if(pMutex->iLockingThread == self)
            {
                pMutex->iDataLock.Wait();
                if(mutex->iState == _EDestroyed)
                {
                   pMutex->iDataLock.Signal();
                   return EINVAL;
                }
                pMutex->iCount++;
                pMutex->iDataLock.Signal();
            }
            else
            {
                pMutex->iDataLock.Wait();
                if(mutex->iState == _EDestroyed)
                {
                   pMutex->iDataLock.Signal();
                   return EINVAL;
                }
                pMutex->iSemaphoreState++;   
                pMutex->iDataLock.Signal();
                
                retval = _doWait(&pMutex->iSignalSemaphore,abstime);
                if(retval == 0)
                {
                    pMutex->iDataLock.Wait();
                    if(mutex->iState == _EDestroyed)
                    {
                      pMutex->iDataLock.Signal();
                      return EINVAL;
                    }
                    pMutex->iCount = 1;
                    pMutex->iLockingThread = self;
                    pMutex->iDataLock.Signal();
                }
            }
            break;
        }                          

        case  PTHREAD_MUTEX_ERRORCHECK:
        {
            self = pthread_self();
            if(pMutex->iLockingThread == self)
            {
                return EDEADLK;
            }
            else
            {
                pMutex->iDataLock.Wait();
                if(mutex->iState == _EDestroyed)
                {
                   pMutex->iDataLock.Signal();
                   return EINVAL;
                }
                pMutex->iSemaphoreState++;     
                pMutex->iDataLock.Signal();
                
                retval = _doWait(&pMutex->iSignalSemaphore,abstime);
                if(retval == 0)
                {
                  pMutex->iDataLock.Wait();
                  if(mutex->iState == _EDestroyed)
                  {
                     pMutex->iDataLock.Signal();
                     return EINVAL;
                  }
                  pMutex->iLockingThread  = self;
                  pMutex->iDataLock.Signal();
                }
            }
            break;
        }

        default:
        retval = ENOSYS;
        break;

    }
    return retval;

}

//End of File
