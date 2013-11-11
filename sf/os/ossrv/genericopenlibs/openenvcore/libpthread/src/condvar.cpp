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
// Name        : condvar.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of pthread on Symbian
// Version     :
//



#include "condvartypes.h"

/*
This internal function initializes the passed queue to be an empty queue
*/
bool _QueueInit(_CondQueue* aQueue)
{
    if(!aQueue)
    {
        return false;
    }
    pthread_mutex_init(&aQueue->iMutex,NULL);
    aQueue->iHead = aQueue->iTail = NULL;
    return true;
}

/*
This internal function destroys the passed 
*/
int _QueueDestroy(_CondQueue* aQueue)
{
    if(!aQueue)
    {
        return EINVAL;
    }
    pthread_mutex_lock(&aQueue->iMutex);
    _CondNode* head = aQueue->iHead;
    pthread_mutex_unlock(&aQueue->iMutex);
    if (head) 
    {
        return EBUSY;   
    }
    else
    {
        return pthread_mutex_destroy(&aQueue->iMutex);
    }
}


/*
This internal function atomically adds a element
to the passed queue
*/
bool _QueueAdd(_CondQueue* aQueue, _CondNode* aNode)
{
    if( (!aQueue) || (!aNode) )
    {
        return false;
    }
    aNode->iNext = NULL;
    pthread_mutex_lock(&aQueue->iMutex);
    if(!(aQueue->iHead))
    {
        aQueue->iHead = aQueue->iTail = aNode;
    }
    else
    {
        aQueue->iTail->iNext = aNode;
        aQueue->iTail = aNode;
    }
    pthread_mutex_unlock(&aQueue->iMutex);
    return true;
}
  
    
/*
This internal function atomically removes the pased element
from the queue
*/
bool _QueueRemove(_CondQueue* aQueue, _CondNode* aNode)
{
    if( (!aQueue) || (!aNode) )
    {
        return false;
    }
    pthread_mutex_lock(&aQueue->iMutex);
        
    _CondNode* prev = NULL;
    _CondNode* node = aQueue->iHead;
    for(; node; node = node->iNext)
    {
        if(node == aNode)
        {
            break;
        }
        prev = node;
    }
    if(node)
    {
        if(node == aQueue->iHead)
        {
            aQueue->iHead = node->iNext;
        }
        if(node == aQueue->iTail)
        {
            aQueue->iTail = prev;
        }
        if(prev)
        {
            prev->iNext = node->iNext;
        }
    }// node found
    pthread_mutex_unlock(&aQueue->iMutex);
    return (node) ? true : false;
}

/*
This internal function atomically removes the head of the queue, and
returns a pointer to this node
*/
_CondNode* _QueueRemoveHead(_CondQueue* aQueue)
{
    _CondNode* retNode = NULL;
    pthread_mutex_lock(&aQueue->iMutex);
    retNode = aQueue->iHead;
    if(retNode)
    {
        aQueue->iHead = retNode->iNext;
        if(aQueue->iTail == retNode)
        {
            aQueue->iTail = aQueue->iHead = NULL;
        }
    }
    pthread_mutex_unlock(&aQueue->iMutex);
    return retNode;
}


/*
This internal function  removes the head of the queue, and
returns a pointer to this node WITHOUT locking the associated mutex
*/
_CondNode* _QueueRemoveHeadNoLock(_CondQueue* aQueue)
{
    _CondNode* retNode = NULL;
    retNode = aQueue->iHead;
    if(retNode)
    {
        aQueue->iHead = retNode->iNext;
        if(aQueue->iTail == retNode)
        {
            aQueue->iTail = aQueue->iHead = NULL;
        }
    }
    return retNode;
}

/*
This internal function  locks the queue mutex
*/
void  _QueueLock(_CondQueue* aQueue)
{
    pthread_mutex_lock(&aQueue->iMutex);
}

/*
This internal function  unlocks the queue mutex
*/
void  _QueueUnlock(_CondQueue* aQueue)
{
    pthread_mutex_unlock(&aQueue->iMutex);
}

/*
This internal function atomically resolves the race to dynamically initialize a static cond var
*/
bool _staticCondInit(pthread_cond_t* cond)
{
    if(!cond)
    {
        return false;
    }
    if(cond->iState == _EInitialized)
    {
        return true;
    }


    if(cond->iState != _ENeedsNormalInit)
    {
        return false;
    }
    int retval=0;

    #ifndef NOUSE_INTERNALS
    void* tlsPtr;

    // Get the TLS pointer
    tlsPtr = _pthread_getTls();  
    if(NULL == tlsPtr)
    {
        return false;
    }
    _pthread_atomicMutexLock(tlsPtr);
    #endif

    //another thread might win race
    if(cond->iState != _EInitialized)
    {
        retval = pthread_cond_init(cond,NULL);
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
This internal function provides the functionality to wait on a condition variable,
*/
int _internalCondWait(pthread_cond_t *cond, pthread_mutex_t *mutex,const struct timespec *abstime)
{
    if(!_staticCondInit(cond))
    {
        return EINVAL;
    }
    
    _CondNode* newNode = new _CondNode;  
    if(!newNode)
    {
        return ENOMEM;
    }
    sem_init(&newNode->iSemaphore,0,0);
      
    _QueueAdd(&cond->iQueue,newNode);

    int retval=0;
    
    //unlock passed mutex
    retval = pthread_mutex_unlock(mutex);
    if(retval != 0)
    {
        _QueueRemove(&cond->iQueue,newNode);
        sem_destroy(&newNode->iSemaphore);
        delete newNode; 
        errno = 0; // Condition variables should not set errno
        return retval;
    }    

    //node removed from queue by pthread_cond_signal or pthread_cond_broadcast
    //by timeout below
    if(abstime)
    {
        retval = sem_timedwait(&newNode->iSemaphore,abstime);   
        if(retval != 0)
        {
            _QueueRemove(&cond->iQueue,newNode);
        }//timed_wait timedout
    }
    else
    {
        retval = sem_wait(&newNode->iSemaphore);    
    }
    if(retval != 0)
    {
        retval = errno;
    }
    sem_destroy(&newNode->iSemaphore);
    delete newNode; 
    /* This is a cancellation point */
    //test for cancel
    pthread_testcancel();

    //lock the passed mutex
    pthread_mutex_lock(mutex);

    errno = 0; // Condition variables should not set errno
    return retval;
}

//End of File
