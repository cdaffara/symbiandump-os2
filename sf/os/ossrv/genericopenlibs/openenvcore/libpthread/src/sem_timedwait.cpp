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
// Name     : sem_timedwait.cpp
// Part of  : PThread library
// sem_timedwait call is implemented.
// Version:
//



#include <e32def.h>
#include <errno.h>

#include "semaphore.h"
#include "semaphoretypes.h"
#include "pthread.h"

extern int sem_microsleepwait_np (sem_t* sem, const long aSleepTime);
EXPORT_C int sem_timedwait(sem_t* sem, const struct timespec* abstime)
{
    long lTime = _microsleeptime(abstime);
    if(lTime > 0)
    {
        return sem_microsleepwait_np(sem,lTime);
    }
    else
    {
        return -1;
    }
}//sem_timedwait()

//End of File

