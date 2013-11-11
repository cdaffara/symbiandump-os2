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
// Name     : sem_open.cpp
// Part of  : PThread library
// sem_open call is implemented.
// Version:
//



#include <e32def.h>
#include <errno.h>

#include "semaphore.h"
#include "semaphoretypes.h"
#include "pthread.h"

EXPORT_C sem_t* sem_open (const char * /*name*/, int /*oflag*/, mode_t /*mode*/, unsigned int /*value*/)
{
    errno = ENOSYS;
    return (sem_t*)SEM_FAILED;
}//sem_open()

//End of File

