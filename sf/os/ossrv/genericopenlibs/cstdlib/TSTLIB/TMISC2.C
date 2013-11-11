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
* Test code for miscellaneous functions implemented via the MSystemInterface
* This differs from TMISC in that the functions must be tested twice, once with
* the local interface and once with the CPosixServer.
* 
*
*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "CTEST.H"	/* includes C interface to EPOC32 threads, and SpawnPosixServer */

test_Data;

void env_check(char* name, char* correct_value)
	{
	char* value = getenv(name);
	test(value!=0);
	test(strcmp(value,correct_value)==0);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1070
@SYMTestCaseDesc	    Tests for environment variables
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for getting and setting the environment variables
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void environment()
	{
	char* value;
	int ret;

	test_Next("Environment variables");
	value=getenv("not_present");
	test(value==0);

	ret=setenv("TEST1","value1",0);
	test(ret==0);
	value=getenv("not_present");
	test(value==0);
	env_check("TEST1","value1");

	ret=setenv("TEST2","value2",0);
	test(ret==0);
	env_check("TEST2","value2");
	
	unsetenv("not_present");
	env_check("TEST1","value1");
	env_check("TEST2","value2");

	ret=setenv("TEST1","different_value",0);	/* no rewrite */
	test(ret==0);	/* apparently this isn't an error */
	env_check("TEST1","value1");
	env_check("TEST2","value2");

	ret=setenv("TEST1","different_value",1);	/* with rewrite */
	test(ret==0);
	env_check("TEST1","different_value");
	env_check("TEST2","value2");

	ret=setenv("TEST2","value2.1",1);
	test(ret==0);
	env_check("TEST1","different_value");
	env_check("TEST2","value2.1");

	unsetenv("TEST1");
	value=getenv("TEST1");
	test(value==0);
	env_check("TEST2","value2.1");

	ret=setenv("TEST1","a third value which is a really long one so I can force it to realloc some more memory when it does the conversion",0);
	test(ret==0);
	env_check("TEST1","a third value which is a really long one so I can force it to realloc some more memory when it does the conversion");
	env_check("TEST2","value2.1");

	}

int close_console=0;
void allTests()
	{
	environment();

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

	test_Title("Misc MSystemInterface functions");

	allTests();

	test_Next("Do it again using the CPosixServer (for them, not me)");
	close_console=1;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */


	client=create_thread(allTests, "TMISC2 tests");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);

	test_Close();

	return 0;
	}
