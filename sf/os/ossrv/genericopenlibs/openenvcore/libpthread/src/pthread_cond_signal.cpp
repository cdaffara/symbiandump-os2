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
// Name        : pthread_cond_signal.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of pthread on Symbian
// Version     :
//



#include "condvartypes.h"

/*
This function restarts one of the threads that are waiting on the condition 
variable cond. If no threads are waiting on cond, nothing happens. If several 
threads are waiting on cond, exactly one is restarted. 
*/
EXPORT_C int pthread_cond_signal(pthread_cond_t *cond)
{
    if(!_staticCondInit(cond))
    {
        return EINVAL;
    }
    _CondNode* node = _QueueRemoveHead(&cond->iQueue);
    if(node)
    {
        sem_post(&node->iSemaphore);    
    }
    return 0;
}


//End of File

