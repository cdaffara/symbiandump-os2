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
* Name        : inheritsignal.c
* Child process inheriting the parents signal mask 
* 
*
*/



// INCLUDE FILES

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main( int argc, char** argv)
{	
	int ret, signum, sig;
	sigset_t set;
	argc = argc; //to avoid the warning saying variable not used
	sig = atoi(argv[1]);
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		return 0;
		}
	ret = sigprocmask(SIG_SETMASK,NULL,&set);
	if(ret != 0)
		{
		return 0;
		}
	for(signum = 1; signum <= sig; signum++)
		{
		if((signum == SIGKILL) || (signum == SIGSTOP))
			{
			continue;
			}
		ret = sigismember(&set,signum);
		if (ret != 1)
			{
			return 0;
			}
		}
	return 1;
	
	
}


//  End of File
