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
* Name        : sendsigtoparent.c
* To send a signal to the parent process
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
	
	int pid, ret, sig;
	argc = argc; //to avoid the warning saying variable not used
	pid = atoi(argv[1]);
	sig = atoi(argv[2]);
	sleep(2);
	ret = kill(pid,sig);
	
	if(ret == 0)
		{
		return 1;
		}

	return -1;
}


//  End of File
