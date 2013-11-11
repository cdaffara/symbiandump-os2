/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* T_WAITPID.CPP
* 
*
*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include "CTEST.H"

int fids[3];
test_Data;

// simple function so the parent can wait for child and test waitpid	
void doafunc()
	{
	sleep(5);
	exit(0);
	}
/**
@SYMTestCaseID 			SYSLIB-STDLIB-UT-1576
@SYMTestCaseDesc	    Testing if POSIX waitpid function conforms to POSIX standard
@SYMTestPriority 	    High
@SYMTestActions  	    Starts a new processes and tests the POSIX waitpid function with 
						different parameters and checks if it behaves correctly
@SYMTestExpectedResults The test should not fail.
@SYMDEF 				INC073739
*/

int test_waitpid()
	{
	test_Data;
	void* proc;
	void* proc2;
	int ret;
	int status=-1;
	test_Title("WAITPID");
	test_Next("waitpid");
	proc = create_process(doafunc, "A", "rw", fids);
	if (proc)
		start_process(proc);
	else
		perror("Failed to start process A: ");
	
	// test return values of calls to waitpid with different options and pid's
	
	if (proc)
		{
		// test with option WNOHANG and a correct pid on an active process
		ret = wait_for_process_id(proc,get_proc_id(proc),WNOHANG,&status);
		test(ret==0);
		// test with an invalid pid 
		ret = wait_for_process_id(proc,99,0,&status);
		test(errno==ECHILD&&ret==-1);
		// test with invalid options
		ret = wait_for_process_id(proc,99,17,&status);
		test(errno==EINVAL&&ret==-1);
		ret = wait_for_process_id(proc,99,18,&status);
		test(errno==EINVAL&&ret==-1);
		// process is still active keep waiting until it dies
		do 
			{
			ret = wait_for_process_id(proc,get_proc_id(proc),WNOHANG,&status);
			}
		while(!WIFEXITED(status)&&ret==0);
		test(ret==get_proc_id(proc));
   		printf("child exited, status=%d\n", WEXITSTATUS(status));
		}

	// create another process and test waitpid with pid -1 and options 0
	// this is the same as calling wait
	proc2 = create_process(doafunc, "B", "rw", fids);
	if (proc2)
		start_process(proc2);
	else
		perror("Failed to start process B: ");
	if (proc2)
		{
		//wait for proc2 to finish
		do
			{
        	ret = wait_for_process_id(proc2,-1,0,&status);
    		}
    	while (!WIFEXITED(status));
		test(ret==get_proc_id(proc2));
    	printf("child exited, status=%d\n", WEXITSTATUS(status));
		}
		
	fflush(stdout);
	fclose(stdout);
	test_Close();
	return 0;
	}



int main (int argc, char *argv[])
	{

	start_redirection_server();
	if(argc>1)
		//do something with child process
		doafunc();
	else
		test_waitpid();
	// exit here for the moment crt0 libraries panic
	exit(0);
	return 0;
	}
