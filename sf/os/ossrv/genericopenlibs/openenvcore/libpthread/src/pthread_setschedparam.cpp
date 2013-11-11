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
// Name     : pthread_setschedparam.cpp
// Part of  : PThread library
// pthread_setschedparam call implementation.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "threadcreate.h"
#include "pthreadmisc.h"

EXPORT_C int pthread_setschedparam(pthread_t thread, int policy, 
                                   const struct sched_param *param)
{
    int priority;
    _global_data_t *glbPtr;
    _pthread_node_t *tempPtr;
    _pthread_node_t *selfNodePtr;
    _pthread_node_t *thHandle;
    
    THR_PRINTF("[pthread] Begin pthread_setschedparam\n");

    if (NULL == param)
    {
        THR_PRINTF("[pthread] End of pthread_setschedparam\n");
        return EINVAL;
    }
    /* Check the policy */
    switch (policy) 
    {
        case SCHED_RR:
            // Nothing has to be done.
            THR_PRINTF("[pthread] End pthread_setschedparam\n");
            break;
            
        default:
            THR_PRINTF("[pthread] End pthread_setschedparam\n");        
            return EINVAL;
    }    

    if ((param->sched_priority < MIN_RR_PRIORITY) ||
        (param->sched_priority > MAX_RR_PRIORITY))
    {
        THR_PRINTF("[pthread] End of pthread_setschedparam\n");
        return EINVAL;
    }
    
    /*  Mapping POSIX priorities to Symbian Threads priorities 
        POSIX     Symbian
    ---------     ------------------
      0 -  49  => EPriorityMuchLess
     50 -  99  => EPriorityLess
    100 - 149  => EPriorityNormal
    150 - 199  => EPriorityMore
    200 - 255  => EPriorityMuchMore
    */
    
    switch (param->sched_priority / 50)
    {
        case 0:
            priority = EPriorityMuchLess;
            break;
        case 1:
            priority = EPriorityLess;
            break;
        case 2:
            priority = EPriorityNormal;
            break;
        case 3:
            priority = EPriorityMore;
            break;
        default:
            priority = EPriorityMuchMore;
            break;                                                
    }

    /* Verify the thread handle */

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

    thHandle = (_pthread_node_t *)thread;

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
        THR_PRINTF("[pthread] End of pthread_setschedparam");
        glbPtr->lockThreadTable.Signal();  //release global lock
        return ESRCH;
    }
    
    thHandle->lockNode.Wait(); // Acquire the node lock 
                              
    thHandle->rtHandle.SetPriority((TThreadPriority)priority);
    thHandle->priority = priority;
        
    thHandle->lockNode.Signal();
    glbPtr->lockThreadTable.Signal();  // release thread table lock

    THR_PRINTF("[pthread] End pthread_setschedparam\n");
    
    return 0;
}

// End of File

