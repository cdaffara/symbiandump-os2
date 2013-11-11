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
* Name        : defaulthandler.c
* 
*
*/



// INCLUDE FILES

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main( int argc, char** argv)
{	
	int val1 =0,signum,sigapi;
	struct sigaction sa;
	argc =  argc;  //to avoid the warning saying variable not used
	signum = atoi(argv[1]);
	sigapi = atoi(argv[2]);
	
	switch(sigapi)
		{
		case 1:
			if (signal(signum,SIG_DFL) == SIG_ERR)
				{
				return -1;
				}
			break;
		case 2:
			if (sigset(sigapi,SIG_DFL) == SIG_ERR)
				{
				return -1;
				}
			break;
		case 3:
			if (bsd_signal(sigapi,SIG_DFL) == SIG_ERR)
				{
				return -1;
				}
			break;
		case 4:
			sa.sa_handler = SIG_DFL;
			sa.sa_flags = 0;
			if (sigaction(sigapi,&sa,NULL) == -1)
				{
				return -1;
				}
			break;
		default:
			return -1;
		}
	
	val1 = raise (sigapi);
	if (val1 == -1)
		{
		return -2;
		}
	return 1;
}


//  End of File
