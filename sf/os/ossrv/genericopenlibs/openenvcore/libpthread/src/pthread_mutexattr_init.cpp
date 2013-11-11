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
// Name        : pthread_mutexattr_init.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of mutexes on Symbian
// Version     :
//



#include "mutextypes.h"

/*
This function initializes the mutex attribute object attr and fills it with default values for the attributes.
*/
EXPORT_C int pthread_mutexattr_init (pthread_mutexattr_t * attr)
{
    if(!attr)
    {
        return EINVAL;
    }
    attr->iState = _EInitialized;
    attr->iSharedType = PTHREAD_PROCESS_PRIVATE;
    attr->iMutexType  = PTHREAD_MUTEX_DEFAULT;
    return 0;
}

