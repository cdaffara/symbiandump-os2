/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#include <signal.h>
#include <sys/errno.h>
#include "sysreent.h"

EXPORT_C int sigwait(const sigset_t *set, int *sig)
{
	return _sigwait_r(&errno, set, sig);
}

EXPORT_C int sigtimedwait(const sigset_t *set,
       siginfo_t *info, const struct timespec *timeout)
	{
	return _sigtimedwait_r(&errno,set,info,timeout);
	}

EXPORT_C int sigwaitinfo(const sigset_t *set, siginfo_t *info)
{
	return _sigwaitinfo_r(&errno,set,info);
}
