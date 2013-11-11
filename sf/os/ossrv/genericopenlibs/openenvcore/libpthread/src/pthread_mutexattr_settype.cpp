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
// Name        : pthread_mutexattr_settype.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of mutexes on Symbian
// Version     :
//



#include "mutextypes.h"

EXPORT_C int pthread_mutexattr_settype (pthread_mutexattr_t * attr, int type)
{
    if(!VALID_MUTEX_ATTR(attr))
    {
        return EINVAL;
    }
    if(type != PTHREAD_MUTEX_NORMAL && type != PTHREAD_MUTEX_RECURSIVE && type != PTHREAD_MUTEX_ERRORCHECK)
    {
        return EINVAL;
    }
    attr->iMutexType = type;
    return 0; 
}

// End of File
