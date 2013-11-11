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
// Name     : pthread_key_create.cpp
// Part of  : PThread library
// pthread_key_create call is implemented.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "pthreadmisc.h"

int _insertKeyNodeInList(_pkey_node_t *newTlsPtr,_pthread_node_t *temp)
{
    _pkey_node_t *tlsPtr;
    _pkey_node_t *prevTlsPtr;

    // Insert in a sorted list
    tlsPtr = temp->tlsHead;
    if (tlsPtr == NULL) // First node
    {
        temp->tlsHead = newTlsPtr;
    }
    else 
    {
        for (tlsPtr = temp->tlsHead, prevTlsPtr = NULL; 
             tlsPtr != NULL; 
             prevTlsPtr = tlsPtr,tlsPtr = tlsPtr->next)
        {
            if (tlsPtr->keyNumber == newTlsPtr->keyNumber)
            {
                // This happen very rearly, but valid when called from
                // _createKeyList
                return -1;
            }
            else if(tlsPtr->keyNumber > newTlsPtr->keyNumber)
            {
                if (temp->tlsHead == tlsPtr) //Sorted first node case
                {
                    newTlsPtr->next = tlsPtr;
                    temp->tlsHead = newTlsPtr;
                    break; 
                }
                else   // other than first node
                {
                    newTlsPtr->next = tlsPtr;
                    prevTlsPtr->next = newTlsPtr;
                    break; 
                }
            } 
            
        } 
        
        if (tlsPtr == NULL) // Means add at the end
        {
            prevTlsPtr->next = newTlsPtr;
        }
    }
    return 0;
    
}



static int _createKeyInAllThreads(_global_data_t *glbPtr, int keyNumber)
{
    _pthread_node_t *temp;
    _pkey_node_t *newTlsPtr;
    
    //For all threads
    for (temp = glbPtr->start; temp != NULL; temp = temp->next) 
    {
        newTlsPtr = new _pkey_node_t;
        if (NULL == newTlsPtr) // This is a temporary fix for the OOM bug.
        {              
            newTlsPtr = new _pkey_node_t;
        }
        if (NULL == newTlsPtr) // Memory really exhausted
        {              // Permanent fix is to delete old keys from all threads
            return -1;
        }        
        newTlsPtr->next = NULL;
        newTlsPtr->tls = NULL;
        newTlsPtr->keyNumber = keyNumber;
        
        // Lock the TCB
        temp->lockNode.Wait();

        _insertKeyNodeInList(newTlsPtr,temp);
        
        // Unlock TCB node
        temp->lockNode.Signal();
    } // end of outer for loop
    
    return 0;
}





EXPORT_C int pthread_key_create(pthread_key_t *key, destructor_routine dest)
{
    
    int loopvar;
    _global_data_t *glbPtr;
    _pthread_node_t *selfNodePtr;
    
    THR_PRINTF("[pthread] Begin pthread_key_create\n");
    
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
    glbPtr = selfNodePtr->glbDataPtr;  // point to global struct
    glbPtr->lockThreadTable.Wait();    // Acquire the thread table lock
    
    // Find a free key in pthread_key_list
    for (loopvar = 0; loopvar < PTHREAD_KEYS_MAX; loopvar++)
    {
    
        int idx;
        idx = loopvar / 32;
        
        int bitPos;
        bitPos = loopvar % 32;
        
        if (((glbPtr->statusflag[idx]) & (0x1<<bitPos)) == _KEY_UNUSED)
        {
            // Set it to used
            glbPtr->statusflag[idx] |= (0x1<<bitPos);  
            glbPtr->pthread_key_list[loopvar].destr = dest;
            // Create one key node in all the threads
            if (_createKeyInAllThreads(glbPtr, loopvar) == -1)
            {
                // Error : Should not hit any time
                glbPtr->lockThreadTable.Signal();  //release thread table lock
                return EAGAIN;
            }
            
            glbPtr->lockThreadTable.Signal();  // release thread table lock
            *key = loopvar;
            THR_PRINTF("[pthread] End pthread_key_create\n");
            return 0;
        }
    }
    glbPtr->lockThreadTable.Signal();  // release thread table lock
    THR_PRINTF("[pthread] End pthread_key_create\n");
    
    return EAGAIN;
}

// End of file
