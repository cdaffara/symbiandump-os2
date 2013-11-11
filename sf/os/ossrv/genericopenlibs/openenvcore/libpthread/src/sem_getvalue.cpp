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
// Name     : sem_getvalue.cpp
// Part of  : PThread library
// sem_getvalue call is implemented.
// Version:
//



#include <e32def.h>
#include <errno.h>

#include "semaphore.h"
#include "semaphoretypes.h"
#include "pthread.h"

EXPORT_C int sem_getvalue (sem_t * sem, int *sval)
{
    if (!sem || !(*sem) || !sval)
    {
        errno = EINVAL;
        return -1;
    }

    _sem_t* gsem = *sem;
    if(gsem->iState != _sem_t::EInitialized)
    {
        errno = EINVAL;
        return -1;
    }

    //value of sval need not reflect actual value
    // due to calls to sem_post/sem_wait from other threads
    *sval = gsem->iCount;
    return 0;
}//sem_getvalue()

//End of File
