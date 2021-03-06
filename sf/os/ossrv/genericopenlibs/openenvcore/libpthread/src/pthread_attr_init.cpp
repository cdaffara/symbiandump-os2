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
// Name     : pthread_attr_init.cpp
// Part of  : PThread library
// pthread_attr_init call implementation.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "threadcreate.h"

EXPORT_C int pthread_attr_init(pthread_attr_t *attrib)
{
    THR_PRINTF("[pthread] Begin pthread_attr_init\n");

    _pthread_attr *thAttrPtr;
    
    if (NULL == attrib)
    {
        THR_PRINTF("[pthread] End of pthread_attr_init\n");
        return EINVAL;
    }
    
    thAttrPtr = (_pthread_attr*) attrib;
    
    thAttrPtr->stackSize = DEFAULT_STACK_SIZE;
    thAttrPtr->detachState = PTHREAD_CREATE_JOINABLE;
    thAttrPtr->scope = PTHREAD_SCOPE_SYSTEM;
    thAttrPtr->policy = SCHED_RR;
    thAttrPtr->sp.sched_priority = 100; /* this leads to EPriorityNormal */
    thAttrPtr->priority = EPriorityNormal;
    
    THR_PRINTF("[pthread] End pthread_attr_init\n");
    
    return 0;
}

// End of File
