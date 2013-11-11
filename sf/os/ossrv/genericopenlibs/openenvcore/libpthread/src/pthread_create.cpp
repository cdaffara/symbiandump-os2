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
// Name     : pthread_create.cpp
// Part of  : PThread library
// All thread create related functions are 
// implemented in this file.
// Version:
//



#include <e32math.h>
#include <e32base.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include "threadglobals.h"
#include "threadcreate.h"
#include "pthreadmisc.h"

// For avoiding warning: Warning: #550-D: variable "b" was set but never used
// RVCT compiler only
#ifndef __WINS__
#pragma diag_remark 550
#endif

extern int _insertKeyNodeInList(_pkey_node_t *newTlsPtr,_pthread_node_t *temp);

int _createKeyList(_global_data_t *glbPtr,_pthread_node_t *selfNodePtr)
{
    int keyNumber;
    _pkey_node_t *newTlsPtr;
    
    glbPtr->lockThreadTable.Wait(); // Acquire the global linked-list lock
    selfNodePtr->lockNode.Wait();
    
    for (keyNumber = 0; keyNumber < PTHREAD_KEYS_MAX; keyNumber++)
    {
        int idx;
        idx = keyNumber / 32;
        
        int bitPos;
        bitPos = keyNumber % 32;
        
        if (((glbPtr->statusflag[idx]) & (0x1<<bitPos)) != _KEY_UNUSED) //Used
        {
            // Create Node for it
            newTlsPtr = new _pkey_node_t;
            if (NULL == newTlsPtr) // This is added ot avoid OOM testing bug
            {
                newTlsPtr = new _pkey_node_t;
            }
            if (NULL == newTlsPtr) // Really memory exhosted..
            {
                return -1;
            }
            newTlsPtr->next = NULL;
            newTlsPtr->tls = NULL;
            newTlsPtr->keyNumber = keyNumber;        
            
            if (_insertKeyNodeInList(newTlsPtr,selfNodePtr) == -1)
            {
            	delete newTlsPtr;
            }
        }
    }
    
    selfNodePtr->lockNode.Signal();
    glbPtr->lockThreadTable.Signal();// Release the global linked-list lock    
    return 0;
}

int _pthread_entryPtFun(void *args)
{
	int retVal = 0;
    _wrapperFunArgs *param;
    _wrapperFunArgs  tParam;
    _pthread_node_t *selfNodePtr;
    _global_data_t *glbPtr;
    
    THR_PRINTF("[pthread] Begin _pthread_entryPtFun\n");
    param = (_wrapperFunArgs*)args;
    selfNodePtr = (_pthread_node_t*) param->nodePtr;
    glbPtr = selfNodePtr->glbDataPtr;

    Dll::SetTls((TAny*)selfNodePtr);  // Set node address to TLS
    
    // Make all the thread Non critical, else if any thread terminates,
    // then process terminates.
    User::SetCritical(User::ENotCritical);
    
    tParam.begin_routine = param->begin_routine;
    tParam.args = param->args;
    delete param;                       // Free param
    
    // Create TLS keys link list
    if (_createKeyList(glbPtr,selfNodePtr) == -1)
        return KErrNoMemory;
    // Create Cleanup Stack for this thread
    CTrapCleanup* pCleanup = CTrapCleanup::New();
    if( !(pCleanup) ) //This is added to solve the out of memory 
    {                 //testing problem
        pCleanup = CTrapCleanup::New();
    }
    selfNodePtr->cleanStackPtr = (void*)pCleanup; //added to solve memoryleak 
	if( pCleanup )
		{
		// Call user entry point function with in TRAP
		TRAP(retVal, selfNodePtr->returnValue = tParam.begin_routine(tParam.args));  
		//selfNodePtr->cleanStackPtr = NULL;
		}
	else
		{
		//No Memory!!
		pthread_exit((void*)selfNodePtr->returnValue);
		retVal = KErrNoMemory;
		return retVal;
		}
    TRAP(retVal, pthread_exit((void*)selfNodePtr->returnValue));
    delete pCleanup;
    THR_PRINTF("[pthread] End of _pthread_entryPtFun\n");
    return retVal; //This return values goes to Symbian
}

void _pthread_insertNode(void *tlsPtr, _pthread_node_t *newNode)
{
    _pthread_node_t *currNode;
    _global_data_t *glbPtr;
         
    THR_PRINTF("[pthread] Begin _pthread_insertNode\n");
    currNode = (_pthread_node_t *) tlsPtr;
    glbPtr = currNode->glbDataPtr;
    
    glbPtr->lockThreadTable.Wait(); // Acquire the global linked-list lock
    
    // Append the new node at the beginning
    newNode->next = glbPtr->start;
    glbPtr->start = newNode;
    
    (glbPtr->threadCount)++;       // Increment the thread count
    
    glbPtr->lockThreadTable.Signal();// Release the global linked-list lock
    THR_PRINTF("[pthread] End of _pthread_insertNode\n");
    
}


int _pthread_setAttributes(_pthread_attr *thAttr, pthread_attr_t *attrib)
{
    _pthread_attr *thAttrPtr;
        
    THR_PRINTF("[pthread] Begin _pthread_setAttributes\n");
    
    // If attributes are passed by user then set accordingly;
    // else set default attributes.    
    if (NULL == attrib)
    {
        // Initialise default values for attributes
        thAttr->stackSize = DEFAULT_STACK_SIZE;
        thAttr->detachState = PTHREAD_CREATE_JOINABLE;
        thAttr->scope = PTHREAD_SCOPE_SYSTEM;
        thAttr->policy = SCHED_RR;
        
        /* this leads to EPriorityNormal */
        thAttr->sp.sched_priority = DEFAULT_THREAD_PRIORITY;
        thAttr->priority = EPriorityNormal;
        
        THR_PRINTF("[pthread] End of _pthread_setAttributes: Success\n");
        return 0;
    }

    thAttrPtr = (_pthread_attr*) attrib;
    
    // Check whether attribute is valid or not: stack size
    if (PTHREAD_STACK_MIN <= thAttrPtr->stackSize)
    {
        thAttr->stackSize = thAttrPtr->stackSize;    
    }
    else 
    {
        THR_PRINTF("[pthread] Invalid Stack size\n");
        return EINVAL;
    }
    
    // Check whether attribute is valid or not: detach state
    if ((PTHREAD_CREATE_JOINABLE == thAttrPtr->detachState) ||
        (PTHREAD_CREATE_DETACHED == thAttrPtr->detachState))
    {
        thAttr->detachState = thAttrPtr->detachState;    
    }
    else
    {
        THR_PRINTF("[pthread] Invalid Detach state\n");
        return EINVAL;
    }
    
    // Check whether attribute is valid or not: Contentionscope
    if (PTHREAD_SCOPE_SYSTEM == thAttrPtr->scope)
    {
        thAttr->scope = thAttrPtr->scope;    
    }
    else
    {
        THR_PRINTF("[pthread] Invalid Contention scope\n");
        return EINVAL;
    }
    
    // Check whether attribute is valid or not: scheduling policy
    if (SCHED_RR == thAttrPtr->policy)
    {
        thAttr->policy = thAttrPtr->policy;
    }
    else
    {
        THR_PRINTF("[pthread] Invalid policy \n");
        return EINVAL;        
    }
    
    // Check whether attribute is valid or not: priority (sched param)
    if ((thAttrPtr->sp.sched_priority < MIN_RR_PRIORITY) ||
        (thAttrPtr->sp.sched_priority > MAX_RR_PRIORITY))
    {
        THR_PRINTF("[pthread] Invalid priority \n");
        return EINVAL;        
    }
    
    thAttr->sp.sched_priority = thAttrPtr->sp.sched_priority;

    if ((EPriorityMuchLess != thAttrPtr->priority) &&
        (EPriorityLess != thAttrPtr->priority) &&
        (EPriorityNormal != thAttrPtr->priority) &&
        (EPriorityMore != thAttrPtr->priority) &&                
        (EPriorityMuchMore != thAttrPtr->priority) )
    {
        THR_PRINTF("[pthread] Invalid Internal priority \n");
        return EINVAL;        
    }
    
    thAttr->priority = thAttrPtr->priority;
        
    THR_PRINTF("[pthread] End of _pthread_setAttributes: Success\n");
    return 0;
}

EXPORT_C int pthread_create(pthread_t *threadhdl, pthread_attr_t *attrib, 
                   thread_begin_routine begin_routine,  void * param)         
{
    _pthread_node_t *selfNodePtr;
    _pthread_node_t *newNodePtr;
    _pthread_attr thAttr;
    _global_data_t *glbPtr;
    _wrapperFunArgs *wArgs;
    TBuf<MAX_THREAD_NAME_LEN> threadName;
    int retVal;
    int loopVar;
    
    THR_PRINTF("[pthread] Begin pthread_create\n");
    
    //Get the TLS value (self node pointer)
    selfNodePtr = (_pthread_node_t*) _pthread_getTls();
    if (NULL == selfNodePtr)
    {
        THR_PRINTF("[pthread] FATAL :TLS is not initialized \n");
        return EAGAIN;
    }
   
    // Global data strcuture Pointer 
    glbPtr = selfNodePtr->glbDataPtr;
    
    // Initialize the attributes
    if (_pthread_setAttributes(&thAttr,attrib) == EINVAL)
    {
        return EINVAL;
    }
    
    newNodePtr = new _pthread_node_t;
    if (NULL == newNodePtr)
    {
        THR_PRINTF("[pthread] FATAL :MemoryAlloc fail for new threadnode");
        return EAGAIN;
    }
    
    //Initialize the new node
    newNodePtr->next = NULL;
    if (newNodePtr->lockNode.CreateLocal() != KErrNone)
    {
        delete newNodePtr;
        THR_PRINTF("[pthread] FATAL :Mutex create failed");
        return EAGAIN;        
    }
    
    newNodePtr->glbDataPtr = glbPtr;
    newNodePtr->detachState = thAttr.detachState;
    newNodePtr->threadState = _THREAD_RUNNING;
    newNodePtr->returnValue = NULL;
    newNodePtr->hasAnyThreadJoined = EFalse;
    newNodePtr->priority = thAttr.sp.sched_priority;
    newNodePtr->mainFlag = _NON_MAIN_THREAD;
    newNodePtr->tlsHead = NULL;
    newNodePtr->cleanStackPtr = NULL;
    
    // Allocate memory for the arguments, to pass to the new thread.
    // Free
    wArgs = new _wrapperFunArgs;
    if (NULL == wArgs)
    {
        newNodePtr->lockNode.Close();
        delete newNodePtr;
        THR_PRINTF("[pthread] FATAL:MemoryAlloc failed!Wrapper function args");
        return EAGAIN;
    }
    
    // Fill the arguments that has to be passed to new thread.
    wArgs->begin_routine = begin_routine;
    wArgs->args = param;
    wArgs->nodePtr = newNodePtr;

    // Their is possiblity that Symbian OS might delay the previous thread
    // destruction, due to that we might get the KErrAlreadyExists error
    // To avoid that random number is concatenated to thread name
    // This trial is done for MAX_THREAD_CREATE_FAILURE_COUNT times
    for (loopVar = 0; loopVar < MAX_THREAD_CREATE_FAILURE_COUNT; loopVar++)
    {
        // Generate a unique thread name. Use node address itself.
        threadName.NumFixedWidth(TUint(newNodePtr), EHex, THREAD_NAME_WIDTH);
        
        TUint randNumber = Math::Random();
        TBuf<MAXLEN_RANDOM_NUM> tempBuf;
        tempBuf.NumFixedWidth(randNumber, EHex, THREAD_NAME_WIDTH);
        
        threadName.Append(tempBuf);
        
        // Create the thread
        retVal = newNodePtr->rtHandle.Create(
                                 (const TDesC &)threadName, // Thread Name
                                 _pthread_entryPtFun, // Entry pt function  
                                 thAttr.stackSize,      // Stack Size
                                 NULL,          // Use common heap
                                 (TAny*)wArgs ); // Args to entry pt function
                                 
        if (KErrNone != retVal)
        {
            if(KErrAlreadyExists == retVal)
            {
                // Try again
                continue;
            }
            newNodePtr->lockNode.Close();
            delete newNodePtr;
            delete wArgs;
            THR_PRINTF("[pthread] RThread Create failed.");
            return EAGAIN;
        }
        else
        {
            // Successfully Thread is created
            break;
        }
        
    }    
    
    // Store the thread ID                             
    newNodePtr->threadId = newNodePtr->rtHandle.Id();
    
    // Do not close the thread handle rtHandle.
    
    //Insert the node to the link list
    _pthread_insertNode((void*)selfNodePtr, newNodePtr);
    
    // Set the priority if it is not default
    if (EPriorityNormal != thAttr.priority)
    {
        newNodePtr->rtHandle.SetPriority((TThreadPriority)thAttr.priority);
    }
    
    // Bring the new thread to Running state
    newNodePtr->rtHandle.Resume();
    
    //Set the handle of the newly created thread
    *threadhdl = (pthread_t) newNodePtr;

    THR_PRINTF("[pthread] End of pthread_create: Success \n");    
    return 0; 
}

// End of file
