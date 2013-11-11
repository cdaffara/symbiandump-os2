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
// Name        : pthread_cond_timedwait.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of pthread on Symbian
// Version     :
//



#include "condvartypes.h"

EXPORT_C int pthread_cond_timedwait (pthread_cond_t * cond,pthread_mutex_t * mutex,const struct timespec *abstime)
{
    if(!abstime)
    {
        return EINVAL;
    }
    
    return _internalCondWait(cond,mutex,abstime);
}


//End of File

