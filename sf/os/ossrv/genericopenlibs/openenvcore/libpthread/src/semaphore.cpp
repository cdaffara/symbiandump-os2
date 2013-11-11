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
// Name        : semaphore.cpp
// Part of     : semaphore 
// POSIX implementation of semaphores on Symbian
// Version     :
//



#include <stddef.h>
#include <limits.h>
#include <e32def.h>
#include <e32std.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "pthread.h"
#include "semaphore.h"
#include "semaphoretypes.h"


/*
This is an internal API used to wait on the semaphore
The thread suspended on the semaphore can be cancelled at any time unless the 
calling thread's canceltype is PTHREAD_CANCEL_DISABLE
*/
int _cancelable_mutex_wait(_sem_t* asem)
{
    //cancellation point; 
  pthread_testcancel(); 
  asem->iMutex.Wait(); 
  if(asem->iState != _sem_t::EInitialized) 
  { 
    errno = EINVAL; 
    return -1; 
  } 
  pthread_testcancel(); 
  return 0;
}

/*
This is an internal API used to convert the absolute time out time specified 
by the timespec struct into an equivalent number of microseconds to wait.
*/
long _microsleeptime_raw(const struct timespec* abstime, 
                         const struct timeval* now)
{
    if(!abstime)
    {
        errno = EINVAL;
        return -1;
    }
    
    long secs = abstime->tv_sec - now->tv_sec;
    
    long usecs = (abstime->tv_nsec - now->tv_usec * 1000)/1000;
    
    if (abs(usecs /(1000 * 1000)) > 1)
    {
        errno = EINVAL;
        return -1;
    }
    if(secs < 0 ||(secs ==0 && usecs <= 0)) 
    {
        errno = ETIMEDOUT;
        return -1;
    }
    return (secs* 1000 * 1000) + usecs;
}


/*
This is an internal API used to convert the absolute time out time specified 
by the timespec struct into an equivalent number of microseconds to wait. 
Takes care of overflow check.
*/
long _microsleeptime(const struct timespec* abstime)
{
    struct timeval  now;
    gettimeofday( &now, NULL);
    
    if((abstime->tv_nsec >= 1 * 1000 * 1000 * 1000) || (abstime->tv_nsec < 0))
    {
        errno = EINVAL;
        return -1;
    }
    
    //hard-coded overflow check
    long secs = (abstime->tv_sec - now.tv_sec);
    if(secs < 0 )
    {
        errno = ETIMEDOUT;
        return -1;
    }
    if(secs > KMaxSecsForWait)
    {
        errno = EINVAL;
        return -1;
    }
    return _microsleeptime_raw(abstime,&now);
}

// Adding semaphore into List 
void _insertIntoSemList(void *tlsPtr, _sem_node_t *newNode)
{
    _pthread_node_t *currNode;
    _global_data_t *glbPtr;
         
    currNode = (_pthread_node_t *) tlsPtr;
    glbPtr = currNode->glbDataPtr;
    
    glbPtr->lockSemTable.Wait(); // Acquire the global linked-list lock
    
    // Append the new node at the beginning
    newNode->next = glbPtr->semStart;
    glbPtr->semStart = newNode;
    
    glbPtr->lockSemTable.Signal();// Release the global linked-list lock
}

int _addToSemList(_sem_t *newSem)
{
    _sem_node_t * newNodePtr;
    _pthread_node_t *selfNodePtr;
    
    //Get the TLS value (self node pointer)
    selfNodePtr = (_pthread_node_t*) _pthread_getTls();
    if (NULL == selfNodePtr)
    {
        THR_PRINTF("[pthread] FATAL :TLS is not initialized \n");
        return EAGAIN;
    }
    
    newNodePtr = new _sem_node_t;
    if (NULL == newNodePtr)
    {
        return EAGAIN;
    }
    //Initialize the new node
    newNodePtr->next = NULL;
    newNodePtr->sem  = newSem;
    
    _insertIntoSemList((void*)selfNodePtr,newNodePtr);
    
    return 0;
}
// Deleting semaphore from List 
int _freeSemFromList(void *tlsPtr, _sem_t *delSem)
{
    _pthread_node_t *currNode;
    _global_data_t *glbPtr;
    _sem_node_t *temp;
    _sem_node_t *prev;
         
    currNode = (_pthread_node_t *) tlsPtr;
    glbPtr = currNode->glbDataPtr;
    
    glbPtr->lockSemTable.Wait(); // Acquire the global linked-list lock
    
    prev = NULL;
    for (temp = glbPtr->semStart; temp != NULL; temp = temp->next)
    {
        if(temp->sem == delSem) // Found the sem
        {
            //First Node
            if (temp == glbPtr->semStart)
            {
                glbPtr->semStart = temp->next;
                delete temp;
                glbPtr->lockSemTable.Signal();//Release global linked-list lock
                return _SEM_FOUND_IN_LIST;
            }
            else
            {
                prev->next = temp->next;
                delete temp;
                glbPtr->lockSemTable.Signal();//Release global linked-list lock
                return _SEM_FOUND_IN_LIST;                
            }
        }
        prev = temp;
    }
    
    // Not found
    glbPtr->lockSemTable.Signal();// Release the global linked-list lock
    return _SEM_NOT_FOUND_IN_LIST;
}

int _findAndFreeSem(_sem_t *delSem)
{
    _pthread_node_t *selfNodePtr;
    
    //Get the TLS value (self node pointer)
    selfNodePtr = (_pthread_node_t*) _pthread_getTls();
    if (NULL == selfNodePtr)
    {
        THR_PRINTF("[pthread] FATAL :TLS is not initialized \n");
        return _SEM_NOT_FOUND_IN_LIST;
    }

    return (_freeSemFromList ((void*)selfNodePtr,delSem));
}


// End of File
