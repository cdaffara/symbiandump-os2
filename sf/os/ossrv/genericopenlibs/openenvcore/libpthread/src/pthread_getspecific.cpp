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
// Name     : pthread_getspecific.cpp
// Part of  : PThread library
// pthread_getspecific call is implemented.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "pthreadmisc.h"

EXPORT_C void* pthread_getspecific(pthread_key_t key)
{
    _global_data_t *glbPtr;
    _pthread_node_t *selfNodePtr;
    _pkey_node_t *tlsPtr;
    void * result;
    
    THR_PRINTF("[pthread] Begin pthread_getspecific\n");
    
    if (key >= PTHREAD_KEYS_MAX)
    {
        THR_PRINTF("[pthread] End pthread_getspecific\n");
        return NULL;
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
        THR_PRINTF("[pthread] End pthread_getspecific\n");
        glbPtr->lockThreadTable.Signal();  // release thread table lock
        return NULL;
    }
    
    // Lock the TCB
    selfNodePtr->lockNode.Wait();
    
    for (tlsPtr = selfNodePtr->tlsHead; tlsPtr != NULL; tlsPtr = tlsPtr->next)
    {
        if (tlsPtr->keyNumber == key)
        {
                result = tlsPtr->tls;
                break; 
        }
    }

    if (tlsPtr == NULL) // First node
    {
        // This should never happen; key tobe set
        selfNodePtr->lockNode.Signal();
        glbPtr->lockThreadTable.Signal();  // release thread table lock
        User::Panic(_L("Invalid Key"),0);
        return NULL;
    }
    
    // Unlock TCB node
    selfNodePtr->lockNode.Signal();

    glbPtr->lockThreadTable.Signal();  // release thread table lock
    
    THR_PRINTF("[pthread] End pthread_getspecific\n");
    return result;
}
// End of file
