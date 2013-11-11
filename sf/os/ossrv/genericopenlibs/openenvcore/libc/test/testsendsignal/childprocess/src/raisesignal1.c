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
* Name        : raisesignal.c
* To validate signal raising api's, checked only for default behaviour
*
*/



// INCLUDE FILES

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <spawn.h>

int main(int argc, char *argv[]) 
        {
/* Start :: test for "raise" API */
        int err = 0;
        pid_t pid;
        char *argv1[3] = {"raisesignal1.exe","1",NULL};
        argc =  argc; //to avoid the warning saying variable not used
        if(NULL == argv[1])
        	{
        	err = posix_spawn(&pid, "z:\\sys\\bin\\raisesignal1.exe", NULL, NULL, argv1, NULL);
	        if(0 == err)
	        	{
	        	err = raise(1);
	        	}
        	}
        return err;
        }
//  End of File
