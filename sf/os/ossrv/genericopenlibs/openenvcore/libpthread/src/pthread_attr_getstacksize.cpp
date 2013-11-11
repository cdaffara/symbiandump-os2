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
// Name     : pthread_attr_getstacksize.cpp
// Part of  : PThread library
// pthread_attr_getstacksize call implementation.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "threadcreate.h"


EXPORT_C int pthread_attr_getstacksize(const pthread_attr_t *attrib,
                                       size_t *stkSize)
{
    THR_PRINTF("[pthread] Begin pthread_attr_getstacksize\n");

    _pthread_attr *thAttrPtr;
    
    if ((NULL == attrib) || (NULL == stkSize))
    {
        THR_PRINTF("[pthread] End of pthread_attr_getstacksize\n");
        return EINVAL;
    }
    
    thAttrPtr = (_pthread_attr*) attrib;
    
    if (thAttrPtr->stackSize < PTHREAD_STACK_MIN)
    {
        THR_PRINTF("[pthread] End of pthread_attr_getstacksize\n");
        return EINVAL;        
    }
    
    *stkSize = thAttrPtr->stackSize;
    
    THR_PRINTF("[pthread] End pthread_attr_getstacksize\n");
    
    return 0;
}


// End of File

