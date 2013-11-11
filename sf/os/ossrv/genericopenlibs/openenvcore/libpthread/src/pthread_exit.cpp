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
// Name     : pthread_exit.cpp
// Part of  : PThread library
// pthread_exit call is implemented.
// Version:
//



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <e32base.h>
#include "threadglobals.h"
#include "pthreadmisc.h"

#define THREAD_COUNT_ONE 1


EXPORT_C void pthread_exit(void *retValPtr)
{
    _global_data_t *glbPtr;
    _pthread_node_t *tempPtr;
    _pthread_node_t *selfNodePtr;
        
    THR_PRINTF("[pthread] Begin pthread_exit\n");
    
    //Get the TLS value (self node pointer)
    selfNodePtr = (_pthread_node_t*) _pthread_getTls();
    //coverity[var_compare_op]
    if (NULL == selfNodePtr)    // This should never happen
    {
        THR_PRINTF("[pthread] FATAL :TLS is not initialized \n");
        THR_PRINTF("[pthread] Terminating the process\n");
        RProcess rp;
        rp.Kill(0);                // Terminate the process
    }

    //coverity[var_deref_op]
    if (_MAIN_THREAD == selfNodePtr->mainFlag)
    {
    	// Make the main thread non critical, so that the process does not 
    	// terminate, after main exits with pthread_exit()
    	User::SetCritical(User::ENotCritical);
    }
    
    glbPtr = selfNodePtr->glbDataPtr;  // point to global struct
    
    // Call destructors for TLS keys
    _pthread_destroyKeys(glbPtr,selfNodePtr);
    
    glbPtr->lockThreadTable.Wait();    // Acquire the thread table lock
    
    // Traverse through the list, till destination node is reached
    for (tempPtr = glbPtr->start; 
         ((tempPtr != selfNodePtr) && (tempPtr != NULL)); 
         tempPtr = tempPtr->next)
    {
        ;             // Empty loop 
    }
    
    if (NULL == tempPtr)              // Not found; 
    {
        THR_PRINTF("[pthread] FATAL :thread node not found \n");
        THR_PRINTF("[pthread] Terminating the process\n");
        RProcess rp;
        rp.Kill(0);                // Terminate the process
    }
    
    selfNodePtr->lockNode.Wait(); // Acquire the node lock 

    selfNodePtr->returnValue = retValPtr;  // Store the return val
    
    //added to solve memoryleak
    CTrapCleanup* pCleanup = (CTrapCleanup*)selfNodePtr->cleanStackPtr; 
    selfNodePtr->cleanStackPtr = NULL;    
    
    // Check the thread count and if it becoming zero then terminate process
    --(glbPtr->threadCount); // Update the thread count
    if (THREAD_COUNT_ZERO == glbPtr->threadCount)
    {
		THR_PRINTF("[pthread] Terminating the process\n");
		// Terminate the process by calling exit(0)
		exit(0);
        /*
		RProcess rp;
        rp.Kill(0);*/

        // Free the cleanup stack
        delete pCleanup;
    }

    if (PTHREAD_CREATE_DETACHED == selfNodePtr->detachState)
    {
        // Delete the Node;
        _pthread_deleteNode(selfNodePtr, glbPtr, NULL);
        // It also unlocks the node aswell unlocks the thread table lock
    }
    else
    {
        selfNodePtr->threadState = _THREAD_ZOMBIE;
        selfNodePtr->lockNode.Signal();    // Unlock the node
        
        glbPtr->lockThreadTable.Signal();  // release thread table lock
    }
    
    // Free the cleanup stack
    delete pCleanup;    
    
    //Terminate the current thread
    THR_PRINTF("[pthread] Terminating the current thread\n");
    /*
    
      "RThread.Kill  cannot be used to terminate the thread in question because the thread uses a shared heap and 
     RThread.Kill  will cause the internal array used for thread-local storage(TLS) to be leaked only on specific ARM platforms
     which implement the CP15 feature."
     */
    User::Exit(0);
        

}

// End of file
