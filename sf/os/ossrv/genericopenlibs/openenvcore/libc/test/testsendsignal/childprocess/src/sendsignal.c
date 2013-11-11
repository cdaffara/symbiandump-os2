/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sendsignal.c
* To validate signal raising api's, checked only for default behaviour
*
*/



// INCLUDE FILES

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int sig)
	{
	sig = sig;//to avoid the warning saying variable not used
	exit(0);
	}

int main( int argc, char** argv)
	{	
	int sig;
	argc =  argc; //to avoid the warning saying variable not used
	sig = atoi(argv[1]);
	if ((sig != SIGKILL) && (sig != SIGSTOP))
		{
		if (signal(sig,sighandler) == SIG_ERR)
			{
			return -1;
			}
		}
	while(1) { sleep(1); }
	return 0;
	}

//  End of File
