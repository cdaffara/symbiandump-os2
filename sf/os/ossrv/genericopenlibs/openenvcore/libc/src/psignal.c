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
#include <stdio.h>

EXPORT_C void psignal(int sig, const char *s)
	{
	if (sig < 1 || sig >= NSIG)  //What about RT signal(RTMAX)
		{
		if(s)
			fprintf(stderr,"%s: Unknown signal %d\n",s,sig);
		else
			fprintf(stderr,"Unknown signal %d\n",sig);
		}
	else
		{
		if(s)
			fprintf(stderr,"%s: %s\n",s,sys_siglist[sig]);
		else
			fprintf(stderr,"%s\n",sys_siglist[sig]);
		}
	return;
	}
