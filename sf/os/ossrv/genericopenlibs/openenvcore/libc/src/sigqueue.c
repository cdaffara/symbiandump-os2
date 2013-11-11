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

EXPORT_C int sigqueue(pid_t pid, int sig, const union sigval value)
{
	return _sigqueue_r(&errno, pid, sig, value);
}
