/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <stdlib.h>	/* definition of exit() */
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>	/* for getcwd */
#include <sys/fcntl.h>	/* for O_RDONLY */
#include <sys/types1.h>
#include "CTEST.H"

test_Data; /* needed for logging */

int close_console=0;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1120
@SYMTestCaseDesc	    Tests for defect number 050968
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for stdlib defect
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void def050968()
	{
	unsigned char p = 0xFF;

	test_Next("Check DEF050968");

	test(p==0xFF);
	FD_ZERO(&p);
	test(p==0);
	}

void doTests()
	{
	def050968();

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

	test_Title("TStdlibDefect");

	doTests();

	test_Next("Do it again using the CPosixServer (for them, not me)");
	close_console=1;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */


	client=create_thread(doTests, "TSTDLIBDEFECT tests");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);

	test_Close();

	return 0;
	}

