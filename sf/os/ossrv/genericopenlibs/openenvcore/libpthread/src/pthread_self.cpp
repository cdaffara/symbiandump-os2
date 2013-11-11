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
// Name     : pthread_self.cpp
// Part of  : PThread library
// pthread_self call is implemented.
// Version:
//



#include <pthread.h>
#include "threadglobals.h"
#include "threadcreate.h"
#include "pthreadmisc.h"


EXPORT_C pthread_t pthread_self()
{
    THR_PRINTF("[pthread] Begin pthread_self\n");
    
    //Get the TLS value (self node pointer)
    TAny *tlsAddr = (_pthread_node_t*) _pthread_getTls();
    
    THR_PRINTF("[pthread] End pthread_self\n");
    
    return ((pthread_t) tlsAddr);
}
// End of File
