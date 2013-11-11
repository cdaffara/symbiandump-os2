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

EXPORT_C int sigprocmask(int how, const sigset_t *set,sigset_t *oset)
{
	return _sigprocmask_r(&errno, how, set, oset);
}

EXPORT_C int sighold(int signo)
{
	return _sighold_r(&errno,signo);
}

EXPORT_C int sigrelse(int signo)
{
	return _sigrelse_r(&errno,signo);
}

EXPORT_C int sigpause(int signo)
{
	return _sigpause_r(&errno,signo);
}
