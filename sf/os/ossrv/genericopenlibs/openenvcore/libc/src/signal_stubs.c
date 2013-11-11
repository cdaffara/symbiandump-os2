/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/* These are simulated APIs for some signal APIs. The dont do anything. 
   They return with the success value for the APIs  */

#include <signal.h>
#ifdef SYMBIAN_OE_POSIX_SIGNALS
#include <sys/errno.h>
#include "sysreent.h"
#endif

EXPORT_C int sigaction(int sig, const struct sigaction *act,struct sigaction *oact)
{
#ifdef SYMBIAN_OE_POSIX_SIGNALS
	return _sigaction_r(&errno, sig, act, oact);
#else
#ifdef __SYMBIAN32__
	sig = sig; /*to fix warning 'variable/argument not used in function' */
	act = act;
	oact = oact;
#endif //__SYMBIAN32__	
	return 0;
#endif
}

EXPORT_C int sigemptyset(sigset_t * set)
{
#ifdef SYMBIAN_OE_POSIX_SIGNALS
	return _sigemptyset_r(&errno, set);
#else
#ifdef __SYMBIAN32__
	set = set; /*to fix warning 'variable/argument not used in function' */
#endif //__SYMBIAN32__	
	return 0;
#endif
}

EXPORT_C int sigenable()
{
#ifdef SYMBIAN_OE_POSIX_SIGNALS
	return _sigenable_r(&errno);
#else
	return 0;
#endif
}

