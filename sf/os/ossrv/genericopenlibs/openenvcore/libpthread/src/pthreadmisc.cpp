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
// Name     : pthreadmisc.cpp
// Part of  : PThread library
// Thread Miscellaneous functions are implemented in this file.
// Version:
//



#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <e32base.h>
#include "threadglobals.h"

#define THREAD_NAME_WIDTH 8
#define THREAD_COUNT_ZERO 0
#define MAX_THREAD_CREATE_FAILURE_COUNT 20
#define MAX_THREAD_NAME_LEN 255

#ifdef __EPOC32__
class XPthreadTLSCleanup
	{
public:
	~XPthreadTLSCleanup()
		{
		Dll::FreeTls();
		}
	};

// Create a global here. It's desctructor invoked on libpthread unload will reset libpthread's TLS
XPthreadTLSCleanup gPthreadTLSCleanup;
#endif

int _copy_already_defined_keys(_pthread_node_t* prev_node, _pthread_node_t* new_node);

// Internal Function
// return : NULL - means no memory
void* _pthread_getTls()
{
    _pthread_node_t *selfNodePtr;
    _global_data_t *glbPtr = glbHeadNode;
    
    THR_PRINTF("[pthread] Begin _pthread_getTls\n");
    // Get the TLS contents. 
    TAny *tlsAddr = Dll::Tls();
    
    // If TLS is already initialised then just return the node address
    if (NULL != tlsAddr)
    {
        THR_PRINTF("[pthread] End _pthread_getTls\n");
        return ((void*) tlsAddr);
    }
    
    // Create node for main thread and also global data structure.
    selfNodePtr = new _pthread_node_t;
    THR_NULL_ASSERT(selfNodePtr,NULL,
          "[pthread] FATAL: Memory alloc failed for threadnode");
    
    //Initialize the node
    selfNodePtr->next = NULL;
    if (selfNodePtr->lockNode.CreateLocal() != KErrNone)
    {
        delete selfNodePtr;
        THR_PRINTF("[pthread] FATAL :Mutex create failed");
        return ((void*)NULL);        
    }
    selfNodePtr->glbDataPtr = glbPtr;
    selfNodePtr->detachState = PTHREAD_CREATE_JOINABLE;
    selfNodePtr->threadState = _THREAD_RUNNING;
    selfNodePtr->returnValue = NULL;
    selfNodePtr->hasAnyThreadJoined = EFalse;
    selfNodePtr->threadId = (TUint)selfNodePtr->rtHandle.Id();
    selfNodePtr->priority = DEFAULT_THREAD_PRIORITY; 
    selfNodePtr->mainFlag = _MAIN_THREAD;
    selfNodePtr->tlsHead = NULL;
    selfNodePtr->cleanStackPtr = NULL; //added to solve memory leak problem

//    Insert the node in to the global linked list and increment thread count.
    glbPtr->lockThreadTable.Wait();
	//copy the already defined keys
	if( _copy_already_defined_keys(glbPtr->start, selfNodePtr))
	{
		delete selfNodePtr;
        THR_PRINTF("[pthread] FATAL :failed copying defined keys");
		
		glbPtr->lockThreadTable.Signal();
        return NULL;
	}
    selfNodePtr->next = glbPtr->start;
    
	glbPtr->start = selfNodePtr;
    glbPtr->threadCount++;
	
    glbPtr->lockThreadTable.Signal();
        
    //Set node address to TLS
    Dll::SetTls((TAny*)selfNodePtr);
    THR_PRINTF("[pthread] End _pthread_getTls\n");
    
    // libc needs to know whether application is multithreaded or not...
    __isthreaded = 1;
    
    return ((void*) selfNodePtr);
}

int _copy_already_defined_keys(_pthread_node_t* prev_node, _pthread_node_t* new_node)
{
	if(!prev_node || !new_node)
		return 0;

	_pkey_node_t *tlsPtr;
	_pkey_node_t *newTlsPtr, *prevTlsPtr=NULL;
    for (tlsPtr = prev_node->tlsHead; tlsPtr != NULL; tlsPtr = tlsPtr->next)
	{
		newTlsPtr = new _pkey_node_t;
		if (NULL == newTlsPtr)
        {
            return -1;
        }
		if(new_node->tlsHead == NULL)
			new_node->tlsHead = newTlsPtr;
		else{
			prevTlsPtr->next = newTlsPtr;
		}
        newTlsPtr->tls = NULL;
        newTlsPtr->keyNumber = tlsPtr->keyNumber;
		prevTlsPtr = newTlsPtr;
	}
	 
	return 0;
}

// This function has to be called holding global lock and node lock.
// Before returning all lock will be opened
void _pthread_deleteNode(_pthread_node_t *selfNodePtr, 
                           _global_data_t *glbPtr,
                           void ** retValPtr)
{
    _pthread_node_t *tempPtr;
    _pthread_node_t *prevPtr = NULL;
    
    THR_PRINTF("[pthread] Begin _pthread_deleteNode\n");
    // Traverse through the list, till node which has to be deleted is found
    for (tempPtr = glbPtr->start; 
         ((tempPtr != selfNodePtr) && (tempPtr != NULL)); 
         tempPtr = tempPtr->next)
    {
        prevPtr = tempPtr;             // Store the previous node ptr
    }
    
    if (NULL == tempPtr)              //Not found; this should never happen
    {
        THR_PRINTF("[pthread] FATAL: Unable to delete the node");
        selfNodePtr->lockNode.Signal();
        glbPtr->lockThreadTable.Signal();  //release global lock
        return;
    }
    if (tempPtr == glbPtr->start)       // Deleting first node
    {
        glbPtr->start = tempPtr->next;  //Update the link list
    }
    else
    {
        prevPtr->lockNode.Wait();       //Acquire the previous node lock
        prevPtr->next = tempPtr->next;  //Update the link list
        prevPtr->lockNode.Signal();     //Release the previous node lock
    }
    
    if (NULL != retValPtr)               // Get the return value
    {
        *retValPtr = selfNodePtr->returnValue;
    }

    selfNodePtr->lockNode.Signal();     // Unlock & Free the node 
    selfNodePtr->lockNode.Close();
    selfNodePtr->rtHandle.Close();
    
    delete selfNodePtr;

    //--(glbPtr->threadCount);           // Update the thread count
    //if (THREAD_COUNT_ZERO == glbPtr->threadCount)
    //{
    //    RProcess rp;
    //    rp.Kill(0);                    // Terminate the process
    //}
    
    glbPtr->lockThreadTable.Signal();  // release thread table lock
    THR_PRINTF("[pthread] End of _pthread_deleteNode\n");
}


void* _getKeyValueSetNull(int keyNumber,_pkey_node_t *tlsPtr)
{
    void* retVal;
    for ( ; tlsPtr != NULL; tlsPtr = tlsPtr->next)
    {
        if (tlsPtr->keyNumber == keyNumber)
        {
                retVal = tlsPtr->tls;
                tlsPtr->tls = NULL;
                return (retVal);
        }
    }
    //This should never happen; hence dest function will not be called
    return NULL;
}
// Before calling this function, global lock and node lock must be open
void _pthread_destroyKeys(_global_data_t *glbPtr,
                            _pthread_node_t *selfNodePtr)
{
    int loopvar;
    int allempty;
    int keyNumber;
    destructor_routine dest;
    void *arg;
    
    glbPtr->lockThreadTable.Wait();    // Acquire the thread table lock
    selfNodePtr->lockNode.Wait();      // Lock the TCB

    for (loopvar = 0, allempty = 1;
        allempty && (loopvar < PTHREAD_DESTRUCTOR_ITERATIONS);loopvar++) 
    {
        allempty = 0; // Assuming all keys are not used.
        
        for (keyNumber = 0; keyNumber < PTHREAD_KEYS_MAX; keyNumber++)
        {
            int temp;
            temp = keyNumber / 32;
            
            int bitPos;
            bitPos = keyNumber % 32;
        
            if (((glbPtr->statusflag[temp]) & (0x1<<bitPos)) &&      //Key used
                (glbPtr->pthread_key_list[keyNumber].destr != NULL))
            {
                if ((arg = _getKeyValueSetNull(keyNumber,selfNodePtr->tlsHead)) != NULL)
                {
                    dest = glbPtr->pthread_key_list[keyNumber].destr;
                
                    selfNodePtr->lockNode.Signal();    // Unlock TCB node
                    glbPtr->lockThreadTable.Signal();  // release thread table lock

                    dest(arg);  //Call destructor

                    glbPtr->lockThreadTable.Wait();// Acquire the thread table lock
                    selfNodePtr->lockNode.Wait();      // Lock the TCB
                    
                    allempty = 1;   // Atleast one key was used
                }
            }
        }
    }
    
    selfNodePtr->lockNode.Signal();    // Unlock TCB node
    glbPtr->lockThreadTable.Signal();  // release thread table lock
}

// End of File
