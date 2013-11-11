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

EXPORT_C void (*bsd_signal(int sig, void (*disp)(int)))(int)
	{
	return _bsd_signal_r(&errno, sig, disp);
	}

EXPORT_C void (*sigset(int sig, void (*disp)(int)))(int)
	{
	return _sigset_r(&errno, sig, disp);
	}

EXPORT_C void (*signal(int sig, void (*disp)(int)))(int)
	{
	return _signal_r(&errno, sig, disp);
	}

EXPORT_C int sigignore(int sig)
	{
	return _sigignore_r(&errno, sig);
	}
