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

EXPORT_C int sigfillset(sigset_t *set)
{
	return _sigfillset_r(&errno, set);
}

EXPORT_C int sigaddset(sigset_t *set, int signo)
{
	return _sigaddset_r(&errno, set, signo);
}

EXPORT_C int sigdelset(sigset_t *set, int signo)
{
	return _sigdelset_r(&errno, set, signo);
}

EXPORT_C int sigismember(const sigset_t *set, int signo)
{
	return _sigismember_r(&errno, set, signo);
}

EXPORT_C int sigandset(sigset_t * set, const sigset_t * left, const sigset_t * right)
{
	return _sigandset_r(&errno, set, left, right);
}

EXPORT_C int sigorset(sigset_t * set, const sigset_t * left, const sigset_t * right)
{
	return _sigorset_r(&errno, set, left, right);
}

EXPORT_C int sigisemptyset(const sigset_t * set)
{
	return _sigisemptyset_r(&errno, set);
}
