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
// Name     : pthread_attr_getscope.cpp
// Part of  : PThread library
// pthread_attr_getscope call implementation.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "threadcreate.h"


EXPORT_C int pthread_attr_getscope(const pthread_attr_t *attrib,
                                   int* scope)
{
    THR_PRINTF("[pthread] Begin pthread_attr_getscope\n");

    _pthread_attr *thAttrPtr;
    
    if ((NULL == attrib) || (NULL == scope))
    {
        THR_PRINTF("[pthread] End of pthread_attr_getscope\n");
        return EINVAL;
    }
    
    thAttrPtr = (_pthread_attr*) attrib;
    if (PTHREAD_SCOPE_SYSTEM != thAttrPtr->scope)
    {
        THR_PRINTF("[pthread] Invalid Contention scope\n");
        return EINVAL;
    }    

    *scope = thAttrPtr->scope;
    
    THR_PRINTF("[pthread] End pthread_attr_getscope\n");
    
    return 0;
}


// End of File

