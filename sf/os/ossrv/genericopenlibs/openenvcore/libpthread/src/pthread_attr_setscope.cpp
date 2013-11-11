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
// Name     : pthread_attr_setscope.cpp
// Part of  : PThread library
// pthread_attr_setscope call implementation.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "threadcreate.h"


EXPORT_C int pthread_attr_setscope(pthread_attr_t *attrib,int conscope)
{
    THR_PRINTF("[pthread] Begin pthread_attr_setscope\n");

    _pthread_attr *thAttrPtr;
    
    if (NULL == attrib)
    {
        THR_PRINTF("[pthread] End of pthread_attr_setscope\n");
        return EINVAL;
    } 
    
    thAttrPtr = (_pthread_attr*) attrib;
    if (PTHREAD_SCOPE_SYSTEM != thAttrPtr->scope)
    {
        THR_PRINTF("[pthread] Invalid Contention scope\n");
        return EINVAL;
    }
    
    switch (conscope) 
    {
        case PTHREAD_SCOPE_SYSTEM:
            thAttrPtr->scope = PTHREAD_SCOPE_SYSTEM;
            THR_PRINTF("[pthread] End pthread_attr_setscope\n");
            return 0;
            
        default:
            THR_PRINTF("[pthread] End pthread_attr_setscope\n");        
            return EINVAL;
    }
}

// End of File

