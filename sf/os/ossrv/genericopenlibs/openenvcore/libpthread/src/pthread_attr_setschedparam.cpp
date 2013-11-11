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
// Name     : pthread_attr_setschedparam.cpp
// Part of  : PThread library
// pthread_attr_setschedparam call implementation.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "threadcreate.h"

EXPORT_C int pthread_attr_setschedparam(pthread_attr_t *attrib, 
                                        const struct sched_param *param)
{
    THR_PRINTF("[pthread] Begin pthread_attr_setschedparam\n");

    _pthread_attr *thAttrPtr;
    
    if ((NULL == attrib) || (NULL == param))
    {
        THR_PRINTF("[pthread] End of pthread_attr_setschedparam\n");
        return EINVAL;
    }
    
    thAttrPtr = (_pthread_attr*) attrib;
    
    if ((param->sched_priority < MIN_RR_PRIORITY) ||
        (param->sched_priority > MAX_RR_PRIORITY) ||
        (thAttrPtr->sp.sched_priority < MIN_RR_PRIORITY) ||
        (thAttrPtr->sp.sched_priority > MAX_RR_PRIORITY))
    {
        THR_PRINTF("[pthread] End of pthread_attr_setschedparam\n");
        return EINVAL;        
    }
    
    memcpy (&thAttrPtr->sp, param, sizeof (struct sched_param));
    
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
            thAttrPtr->priority = EPriorityMuchLess;
            break;
        case 1:
            thAttrPtr->priority = EPriorityLess;
            break;
        case 2:
            thAttrPtr->priority = EPriorityNormal;
            break;
        case 3:
            thAttrPtr->priority = EPriorityMore;
            break;
        default:
            thAttrPtr->priority = EPriorityMuchMore;
            break;                                                
    }

    THR_PRINTF("[pthread] End pthread_attr_setschedparam\n");
    
    return 0;
}

// End of File

