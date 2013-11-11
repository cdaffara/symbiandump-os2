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
// Name     : pthread_getschedparam.cpp
// Part of  : PThread library
// pthread_getschedparam call implementation.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "threadcreate.h"
#include "pthreadmisc.h"

EXPORT_C int pthread_getschedparam(pthread_t thr, int *policy,
                                   struct sched_param *param)
{
    _global_data_t *glbPtr;
    _pthread_node_t *tempPtr;
    _pthread_node_t *selfNodePtr;
    _pthread_node_t *thHandle;
        
    THR_PRINTF("[pthread] Begin pthread_getschedparam\n");
    
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
    
    thHandle = (_pthread_node_t *)thr;
    if (selfNodePtr == thHandle) // Self thread
    {
        glbPtr = selfNodePtr->glbDataPtr;  // point to global struct
        glbPtr->lockThreadTable.Wait();    // Acquire the thread table lock
        selfNodePtr->lockNode.Wait();
        
        param->sched_priority = selfNodePtr->priority;
        *policy = SCHED_RR;

        selfNodePtr->lockNode.Signal();
        glbPtr->lockThreadTable.Signal(); // Release thread table lock        

        THR_PRINTF("[pthread] End of pthread_getschedparam\n");
        return 0;
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
        THR_PRINTF("[pthread] End of pthread_getschedparam");
        glbPtr->lockThreadTable.Signal();  //release global lock
        return ESRCH;
    }
    
    thHandle->lockNode.Wait(); // Acquire the node lock 
                              
    param->sched_priority = thHandle->priority;
    *policy = SCHED_RR;    
    
    thHandle->lockNode.Signal();
    glbPtr->lockThreadTable.Signal();  // release thread table lock

    THR_PRINTF("[pthread] End pthread_getschedparam\n");
    
    return 0;
}


// End of File

