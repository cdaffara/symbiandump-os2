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
// Name     : pthread_key_delete.cpp
// Part of  : PThread library
// pthread_key_delete call is implemented.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "pthreadmisc.h"

static int _deleteKeyInAllThreads(_global_data_t *glbPtr, int keyNumber)
{
    _pthread_node_t *temp;
    _pkey_node_t *tlsPtr;
    _pkey_node_t *prevTlsPtr;
    
    //For all threads
    for (temp = glbPtr->start; temp != NULL; temp = temp->next) 
    {
        // Lock the TCB
        temp->lockNode.Wait();

        // Delete in a sorted list
        for (tlsPtr = temp->tlsHead, prevTlsPtr = NULL; 
             tlsPtr != NULL; 
             prevTlsPtr = tlsPtr,tlsPtr = tlsPtr->next)
        {
            if (tlsPtr->keyNumber == keyNumber)
            {
                if (temp->tlsHead == tlsPtr) //first node case
                {
                    temp->tlsHead = tlsPtr->next;
                    delete tlsPtr;
                    break; // Inner loop
                }
                else
                {
                    prevTlsPtr->next = tlsPtr->next;
                    delete tlsPtr;
                    break;  //Inner loop
                }

            }
            
        } // Inner for loop end...
        
        if (tlsPtr == NULL) // Means not found
        {
            // This should never happen; key to be deleted not exists
            temp->lockNode.Signal();
            User::Panic(_L("Invalid Key"),0);
            return -1;            
        }

        // Unlock TCB node
        temp->lockNode.Signal();
    } // end of outer for loop
    
    return 0;
}

EXPORT_C int pthread_key_delete(pthread_key_t key)
{
    _global_data_t *glbPtr;
    _pthread_node_t *selfNodePtr;
        
    THR_PRINTF("[pthread] Begin pthread_key_delete\n");
    
    if (key >= PTHREAD_KEYS_MAX)
    {
        THR_PRINTF("[pthread] End pthread_key_delete\n");
        return EINVAL;
    }
        
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

    int idx;
    idx = key / 32;
    
    int bitPos;
    bitPos = key % 32;
    
    if (((glbPtr->statusflag[idx]) & (0x1<<bitPos)) == _KEY_UNUSED)
    {
        THR_PRINTF("[pthread] End pthread_key_delete\n");
        glbPtr->lockThreadTable.Signal();  // release thread table lock
        return EINVAL;
    }
    
    glbPtr->statusflag[idx] &= ~(0x1<<bitPos);  // Set it to unused. 
    glbPtr->pthread_key_list[key].destr = NULL;    
    
    // Delete the key in all the threads
    _deleteKeyInAllThreads(glbPtr, key);
    
    glbPtr->lockThreadTable.Signal();  // release thread table lock
    
    THR_PRINTF("[pthread] End pthread_key_delete\n");
    return 0;
    
}

// End of file
