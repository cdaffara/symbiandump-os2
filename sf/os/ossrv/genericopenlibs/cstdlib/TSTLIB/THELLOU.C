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
* A variant of "Hello World" which reports various bits of its environment
* and returns an interesting exit status
* 
*
*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "CTEST.H"

#ifdef __WINS__
int triangle_recurse(char* prog, char* num, int progress)
	{
	printf("\nBut not under WINS where I'll be told 0\n");
	return 0;
	}

int triangle_parallel(char* prog, char* num)
	{
	printf("\nBut not under WINS where I'll be told 0\n");
	return 0;
	}

#else

int triangle_recurse(char* prog, char* num, int progress)
	{
	int n;
	int pid, ret;
	char cmd[100];
	int fids[3];

	n = atol(num);
	if (n<1)
		return 0;

	sprintf(cmd, "%s do_triangle %d", prog, n-1);
	if (progress)
		{
		pid=popen3(cmd, "", 0, fids);
		if (pid<0)
			{
			fprintf(stderr, "Executing %s, ", cmd);
			perror("popen3 failed");
			return -1999999;
			}
		while (1)
			{
			int n=waitpid(pid, &ret, WNOHANG);
			if (n==pid)
				break;
			printf(".");
			fflush(stdout);
			sleep(1);
			}
		}
	else
		ret=system(cmd);
	return n+ret;
	}

int triangle_parallel(char* prog, char* num)
	{
	int n;
	int pid, pid1, pid2, ret1, ret2, base, split;
	char cmd[100];
	int fids1[3], fids2[3];
	char* basep;

	n = atol(num);
	if (n<1)
		return 0;
	basep=getenv("TRIANGLE_BASE");
	if (basep==0)
		return 0;
	base=atol(basep);

	/* we have to add up the numbers base..n inclusive
	 */
	if (base==n)
		return n;
	if (base+1==n)
		return base+n;

	/* At least 3 numbers, so split it into subtasks for child processes
	 */
	split = (n-base)/2;

	sprintf(cmd, "%s do_trianglep %d", prog, base+split);
	pid1=popen3(cmd, "", 0, fids1);
	if (pid1<0)
		{
		fprintf(stderr, "Doing %d..%d of %d..%d", base+split, base, n, base);
		perror("popen3 failed");
		return -1999999;
		}
	
	sprintf(cmd, "%d", base+split+1);
	setenv("TRIANGLE_BASE", cmd, 1);

	sprintf(cmd, "%s do_trianglep %d", prog, n);
	pid2=popen3(cmd, "", 0, fids2);
	if (pid2<0)
		{
		fprintf(stderr, "Doing %d..%d of %d..%d", n, base+split+1, n, base);
		perror("popen3 failed");
		return -1999999;
		}
	
	/* Now collect the results */
	ret1=-10000;
	ret2=-10000;
	for (n=0; n<2; n++)
		{
		int ret=-3999999;
		pid=wait(&ret);
		if (pid<0)
			{
			perror("waitpid failed");
			return -2999999;
			}
		if (pid==pid1)
			{
			pid1=-1;
			ret1=ret;
			}
		else if (pid==pid2)
			{
			pid2=-1;
			ret2=ret;
			}
		else
			printf("Unexpected pid %d (not %d or %d)\n", pid, pid1, pid2);
		}
	return ret1+ret2;
	}

#endif

/**
Usage:  THELLOU [triangle[p] <n>]

@SYMTestCaseID          SYSLIB-STDLIB-CT-1058
@SYMTestCaseDesc	    Tests for printing to standard output stream
@SYMTestPriority 	    High
@SYMTestActions  	    Prints some stuff (looks at args, getenv("USER"), cwd) to stdout and stderr.
						Will compute triangle numbers by calling itself recursively, or parallel subdivison
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
int main (int argc, char *argv[])
	{
	char *user=getenv("USER");
	char cwd[MAXPATHLEN];
	int ret=3, i;

	start_redirection_server();
	
	if (argc==3 && strcmp(argv[1],"do_triangle")==0)
		return triangle_recurse(argv[0], argv[2], 0);

	if (argc==3 && strcmp(argv[1],"do_trianglep")==0)
		return triangle_parallel(argv[0], argv[2]);

	if (user)
		{
		printf("Hello %s\n", user);
		ret=4;
		}
	else
		printf("Greetings.\n");

	printf("I am process %d\n", getpid());

	if (getcwd(cwd,sizeof(cwd))!=0)
		printf("I am speaking to you from %s\n", cwd);

	printf("I have %d arguments: ", argc);
	for (i=0; i<argc; i++)
		printf(">%s< ", argv[i]);
	printf("\r\n");

	printf("In a few moments I shall say %c to stderr:\n", 'a'+argc);
	fflush(stdout);

	fprintf(stderr, "%c\n", 'a'+argc);
	fflush(stderr);

	if (argc==3 && strcmp(argv[1],"triangle")==0)
		{
		printf("For my next trick I shall compute the triangle of %s: ", argv[2]);
		fflush(stdout);
		ret=triangle_recurse(argv[0], argv[2], 1);
		printf("it's %d\n", ret);
		}

	if (argc==3 && strcmp(argv[1],"trianglep")==0)
		{
		printf("I shall now compute the triangle of %s using a parallel algorithm: ", argv[2]);
		fflush(stdout);
		setenv("TRIANGLE_BASE", "1", 0);
		ret=triangle_parallel(argv[0], argv[2]);
		printf("it's %d\n", ret);
		}

	printf("Farewell...\nPress a key");
	getchar();
	return ret;
	}
