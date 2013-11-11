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
// Name        : pthread.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of pthread on Symbian
// Version     :
//



#include <stddef.h>
#include <limits.h>
#include <e32def.h>
#include <e32std.h>
#include <stdlib.h>
#include <errno.h>

#include "pthread.h"

#ifdef __SYMBIAN_COMPILE_UNUSED__
int pthread_cancel (pthread_t /*thread*/)
{
    return ENOSYS;
}

int pthread_setcancelstate (int /*state*/, int* /*oldstate*/)
{
    return ENOSYS;
}

int pthread_setcanceltype (int /*type*/, int* /*oldtype*/)
{
    return ENOSYS;
}
#endif

void pthread_testcancel (void)
{
    return;
}

// End of File
