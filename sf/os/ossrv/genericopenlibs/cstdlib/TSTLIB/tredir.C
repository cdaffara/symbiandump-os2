/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TDIRS.C
* Test code for directory and file handling
* 
*
*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <errno.h>

#include "CTEST.H"	/* includes C interface to EPOC32 threads, and SpawnPosixServer */

test_Data;
int close_console=0;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1098
@SYMTestCaseDesc	    Tests for redirection of server
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for running of threads.Check for error codes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void tests(void)
	{
	char bob[200];
	int x;
	printf("calling gets\n");
	gets (bob);
	printf("got %s\nnow for a getchar > ", bob);
	x = getchar();
	printf("got %d\n", x);
	scanf("%s", bob);
	printf("got %s\n", bob);

	if (close_console)
		{
		test_Close();
		close(0);
		close(1);
		close(2);
		}


	}


int main()
	{
	void* client;
	int err;

	test_Title("Redirection Server");

	start_posix_server();
	
	close_console=1;
	client=create_thread(tests, "tests");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);

	test(err==0);

	test_Close();
	return 0;
	}
