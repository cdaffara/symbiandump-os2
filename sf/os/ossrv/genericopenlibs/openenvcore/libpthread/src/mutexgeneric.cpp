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
// Name       : mutexgeneric.cpp
// Part of    : PThread library
// APIs needed for implementing pthread mutex APIs 
// are implemented.
// Version    :
//



#include "threadglobals.h"
#include "threadcreate.h"

int _pthread_atomicMutexLock(void *tlsPtr)
{
    _pthread_node_t *selfNodePtr;
    _global_data_t *glbPtr;
    
    THR_PRINTF("[pthread] Begin _pthread_atomicMutexLock\n");
    if (NULL == tlsPtr)
    {
        THR_PRINTF("[pthread] FATAL: TLS is NULL\n");
        return -1;
    }
    
    selfNodePtr = (_pthread_node_t *)tlsPtr;
    
    // Global data strcuture Pointer 
    glbPtr = selfNodePtr->glbDataPtr;
    
    glbPtr->globalLockForMutex.Wait(); 
    THR_PRINTF("[pthread] End _pthread_atomicMutexLock\n");    
    return 0;
}

int _pthread_atomicMutexUnlock(void *tlsPtr)
{
    _pthread_node_t *selfNodePtr;
    _global_data_t *glbPtr;
    
    THR_PRINTF("[pthread] Begin _pthread_atomicMutexUnlock\n");
    if (NULL == tlsPtr)
    {
        THR_PRINTF("[pthread] FATAL: TLS is NULL\n");
        return -1;
    }
    
    selfNodePtr = (_pthread_node_t *)tlsPtr;
    
    // Global data strcuture Pointer 
    glbPtr = selfNodePtr->glbDataPtr;
    
    glbPtr->globalLockForMutex.Signal(); 
    
    THR_PRINTF("[pthread] End _pthread_atomicMutexUnlock\n");
    return 0;
}

// End of file
