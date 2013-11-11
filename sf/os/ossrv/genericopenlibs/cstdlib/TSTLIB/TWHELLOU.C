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
* THELLOU.C
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
int triangle_recurse(wchar_t* prog, wchar_t* num, int progress)
	{
	printf("\nBut not under WINS where I'll be told 0\n");
	return 0;
	}

int triangle_parallel(wchar_t* prog, wchar_t* num)
	{
	printf("\nBut not under WINS where I'll be told 0\n");
	return 0;
	}

#else

int triangle_recurse(wchar_t* prog, wchar_t* num, int progress)
	{
	int n;
	int pid, ret;
	char cmd[100];
	wchar_t wcmd[100];
	int fids[3];
	char tmpbuf[100];


	wcstombs(tmpbuf, num, 50);
	n = atol(tmpbuf);
	if (n<1)
		return 0;

	wcstombs(tmpbuf, prog, 50);
	sprintf(cmd, "%s do_triangle %d", tmpbuf, n-1);
	mbstowcs(wcmd, cmd, 100);
	if (progress)
		{
		pid=wpopen3(wcmd, (wchar_t*)L"", 0, fids);
		if (pid<0)
			{
			fprintf(stderr, "Executing %s, ", cmd);
			perror("wpopen3 failed");
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
		ret=wsystem(wcmd);
	return n+ret;
	}

int triangle_parallel(wchar_t* prog, wchar_t* num)
	{
	int n;
	int pid, pid1, pid2, ret1, ret2, base, split;
	char cmd[100];
	wchar_t wcmd[100];
	int fids1[3], fids2[3];
	char* basep;
	char tmpbuf[100];

	wcstombs(tmpbuf, num, 50);
	n = atol(tmpbuf);
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

	wcstombs(tmpbuf, prog, 100);
	sprintf(cmd, "%s do_trianglep %d", tmpbuf, base+split);
	mbstowcs(wcmd, cmd, 100);
	pid1=wpopen3(wcmd, (wchar_t*)L"", 0, fids1);
	if (pid1<0)
		{
		fprintf(stderr, "Doing %d..%d of %d..%d", base+split, base, n, base);
		perror("popen3 failed");
		return -1999999;
		}
	
	sprintf(cmd, "%d", base+split+1);
	mbstowcs(wcmd, cmd, 100);
	wsetenv((wchar_t*)L"TRIANGLE_BASE", wcmd, 1);

	sprintf(cmd, "%s do_trianglep %d", prog, n);
	mbstowcs(wcmd, cmd, 100);
	pid2=wpopen3(wcmd, (wchar_t*)L"", 0, fids2);
	if (pid2<0)
		{
		fprintf(stderr, "Doing %d..%d of %d..%d", n, base+split+1, n, base);
		perror("wpopen3 failed");
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

@SYMTestCaseID          SYSLIB-STDLIB-CT-1096
@SYMTestCaseDesc	    Tests for printing to standard output stream
@SYMTestPriority 	    High
@SYMTestActions  	    Prints some stuff (looks at args, getenv("USER"), cwd) to stdout and stderr.
						Will compute triangle numbers by calling itself recursively, or parallel subdivison
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
int wmain (int argc, wchar_t *argv[])
	{
	wchar_t *user=wgetenv((wchar_t*)L"USER");
	char cwd[MAXPATHLEN];
	int ret=3, i;
	char tmpbuf[100];

	start_redirection_server();
	
	if (argc==3 && wcscmp(argv[1],(wchar_t*)L"do_triangle")==0)
		return triangle_recurse(argv[0], argv[2], 0);

	if (argc==3 && wcscmp(argv[1],(wchar_t*)L"do_trianglep")==0)
		return triangle_parallel(argv[0], argv[2]);

	if (user)
		{
		char tmp[100];
		wcstombs(tmp,user,100);
		printf("Hello %s\n", tmp);
		ret=4;
		}
	else
		printf("Greetings.\n");

	printf("I am process %d\n", getpid());

	if (getcwd(cwd,sizeof(cwd))!=0)
		printf("I am speaking to you from %s\n", cwd);

	printf("I have %d arguments: ", argc);
	for (i=0; i<argc; i++)
		{
		wcstombs(tmpbuf, argv[i], sizeof(tmpbuf));
		printf(">%s< ", tmpbuf);
		}
	printf("\r\n");

	printf("In a few moments I shall say %c to stderr:\n", 'a'+argc);
	fflush(stdout);

	fprintf(stderr, "%c\n", 'a'+argc);
	fflush(stderr);

	if (argc==3 && wcscmp(argv[1],(wchar_t*)L"triangle")==0)
		{
		printf("For my next trick I shall compute the triangle of %s: ", argv[2]);
		fflush(stdout);
		ret=triangle_recurse(argv[0], argv[2], 1);
		printf("it's %d\n", ret);
		}

	if (argc==3 && wcscmp(argv[1],(wchar_t*)L"trianglep")==0)
		{
		printf("I shall now compute the triangle of %s using a parallel algorithm: ", argv[2]);
		fflush(stdout);
		wsetenv((wchar_t*)L"TRIANGLE_BASE", (wchar_t*)L"1", 0);
		ret=triangle_parallel(argv[0], argv[2]);
		printf("it's %d\n", ret);
		}

	printf("Farewell...\nPress a key");
	getchar();
	return ret;
	}
