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
// Name     : pthread_join.cpp
// Part of  : PThread library
// pthread_join call is implemented.
// Version:
//



#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include "threadglobals.h"
#include "pthreadmisc.h"

EXPORT_C int pthread_join(pthread_t thrHandle, void **retValPtr)
{
    _global_data_t *glbPtr;
    _pthread_node_t *tempPtr;
    _pthread_node_t *selfNodePtr;
    _pthread_node_t *thHandle;
        
    THR_PRINTF("[pthread] Begin pthread_join\n");
    
    //Get the TLS value (self node pointer)
    selfNodePtr = (_pthread_node_t*) _pthread_getTls();
	//coverity[var_compare_op]
    if (NULL == selfNodePtr)
    {
        THR_PRINTF("[pthread] FATAL :TLS is not initialized \n");
        THR_PRINTF("[pthread] Terminating the process\n");
        RProcess rp;
        rp.Kill(0);                // Terminate the process
    }
    
    thHandle = (_pthread_node_t *)thrHandle;
    if (selfNodePtr == thHandle) // Thread joing to itself
    {
        THR_PRINTF("[pthread] End of pthread_join\n");
        return EDEADLK;
    }
	//coverity[var_deref_op]
    glbPtr = selfNodePtr->glbDataPtr;  // point to global struct
    
    glbPtr->lockThreadTable.Wait();    // Acquire the thread table lock
    
    // Traverse through the list, till destination node is reached
    for (tempPtr = glbPtr->start; 
         ((tempPtr != thHandle) && (tempPtr != NULL)); 
         tempPtr = tempPtr->next)
    {
        ;             //Empty loop 
    }
    
    if (NULL == tempPtr)              // Not found; 
    {
        THR_PRINTF("[pthread] End of pthread_join");
        glbPtr->lockThreadTable.Signal();  //release global lock
        return ESRCH;
    }
    
    thHandle->lockNode.Wait(); // Acquire the node lock 
                              
    if (_THREAD_ZOMBIE == thHandle->threadState) // already terminated
    {
        _pthread_deleteNode(thHandle, glbPtr, retValPtr); //Free it
        
        THR_PRINTF("[pthread] End of pthread_join\n");
        return 0;
    }
    
    glbPtr->lockThreadTable.Signal();  // release thread table lock
    
    if (PTHREAD_CREATE_DETACHED == thHandle->detachState) //can't join;detached
    {
        THR_PRINTF("[pthread] End of pthread_join\n");
        thHandle->lockNode.Signal();
        return EINVAL;
    }
    
    if (thHandle->hasAnyThreadJoined)  // some thread has already joined
    {                                               
        THR_PRINTF("[pthread] End of pthread_join\n");
        thHandle->lockNode.Signal();
        return EINVAL;
    }
    
    // Now it is clear to join....
    thHandle->hasAnyThreadJoined = ETrue;
    
    TRequestStatus status;
    thHandle->rtHandle.Logon(status); // register for thread termination 
                                      // notification
    
    thHandle->lockNode.Signal();   // Release thenode lock before blocking for
                                   // its termination.
                                   
    User::WaitForRequest(status);  // Wait till thread is terminated    

    glbPtr->lockThreadTable.Wait();    // Acquire the thread table lock
    thHandle->lockNode.Wait(); // Acquire the node lock 
    _pthread_deleteNode(thHandle, glbPtr, retValPtr);
    
    THR_PRINTF("[pthread] End of pthread_join\n");
    return 0;
    
}

// End of file
