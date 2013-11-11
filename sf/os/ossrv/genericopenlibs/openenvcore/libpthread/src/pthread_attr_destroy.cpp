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
// Name     : pthread_attr_destroy.cpp
// Part of  : PThread library
// pthread_attr_destroy call implementation.
// Version:
//



#include <pthread.h>
#include <errno.h>
#include "threadglobals.h"
#include "threadcreate.h"

EXPORT_C int pthread_attr_destroy(pthread_attr_t *attrib)
{
    THR_PRINTF("[pthread] Begin pthread_attr_destroy\n");

    if (NULL == attrib)
    {
        THR_PRINTF("[pthread] End of pthread_attr_destroy\n");
        return EINVAL;
    }
    
    THR_PRINTF("[pthread] End pthread_attr_destroy\n");
    
    return 0;
}

// End of File

