/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : sigpause.c
* To validate sigpause() for all signals
* 
*
*/



// INCLUDE FILES

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main( int argc, char** argv)
	{	
	int ret = 0, sig, pid;
	union sigval sival;
	argc =  argc; //to avoid the warning saying variable not used
	pid = atoi(argv[1]);
	sig = atoi(argv[2]);
	sival.sival_int = sig;
	sleep(1);
	ret = sigqueue (pid, sig, sival);
	if (ret != 0)
		{
		return -1;
		}
	return 0;
	}

//  End of File
