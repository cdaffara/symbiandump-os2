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
// Name        : pthread_mutexattr_destroy.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of mutexes on Symbian
// Version     :
//



#include "mutextypes.h"

/*
This function destroys the mutex attribute object attr and marks it as a destroyed object.
*/
EXPORT_C int pthread_mutexattr_destroy (pthread_mutexattr_t * attr)
{
    if(!VALID_MUTEX_ATTR(attr))
    {
        return EINVAL;
    }
    attr->iState = _EDestroyed;
    return 0;
}


