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
// Name        : pthread_mutexattr_getpshared.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of mutexes on Symbian
// Version     :
//



#include "mutextypes.h"

/*
This function retrieves the current value of the process shared attribute in 
attr and stores it in the location pointed to by pshared.
*/
EXPORT_C int pthread_mutexattr_getpshared (const pthread_mutexattr_t * attr, 
                                           int *pshared)
{
    if(!VALID_MUTEX_ATTR(attr) || !pshared)
    {
        return EINVAL;
    }
    *pshared = attr->iSharedType;
    return 0;
}

// End of File
