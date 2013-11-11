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
* Test code for pipes, using dubious WINS extension for multiple processes...
* 
*
*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>	/* for MAXPATHLEN */
#include <sys/errno.h>
#include <sys/ioctl.h>

#include "CTEST.H"

int fids[3];

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1092
@SYMTestCaseDesc	    Tests for operations on pipes 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for command line arguments,directory operations,environment variables.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
int do_main(int argc, wchar_t* argv[])
	{
	test_Data;
	int i;
	wchar_t* var;
	wchar_t* varname;
	wchar_t cwd[MAXPATHLEN];
	char buf[200];
	char buf1[200];

	test_Title("PIPE");

	test_Next("Command line arguments");
	test(argc>0);
	test(argv!=0);
	printf("  argc=%d\r\n", argc);
	for (i=0; i<argc; i++)
		{
		test(argv[i]!=0);
		test(-1 != wcstombs(buf, argv[i], sizeof(buf)));
		printf("  argv[%d]=\"%s\" length %d\r\n", i, buf, strlen(buf));
		}
	printf("\r\n");

	test_Next("Current working directory");
	var=wgetcwd(cwd,sizeof(cwd)/2);
	test(var==cwd);
	test(-1 != wcstombs(buf, cwd, sizeof(buf)));
	printf("  %s\r\n\n", buf);

	test_Next("Change directory");
	i=wchdir((wchar_t*)L"z:/system");
	test(i==0);
	var=wgetcwd(cwd,sizeof(cwd)/2);
	test(var==cwd);
	test(-1 != wcstombs(buf, cwd, sizeof(buf)));
	printf("  %s\r\n\n", buf);

	test_Next("Environment variables");

	varname=(wchar_t*)L"CLASSPATH";
	var=wgetenv(varname);
	test(var!=0);
	test(-1 != wcstombs(buf, var, sizeof(buf)));
	test(-1 != wcstombs(buf1, varname, sizeof(buf1)));
	printf("  %s=%s\r\n", buf1, buf);

	varname=(wchar_t*)"VARIABLE2";
	var=wgetenv(varname);
	if (var!=0)
		{
		test(-1 != wcstombs(buf, var, sizeof(buf)));
		test(-1 != wcstombs(buf1, varname, sizeof(buf1)));
		printf("  %s=%s\r\n", buf1, buf);
		wunsetenv((wchar_t*)"VARIABLE2");
		}

	varname=(wchar_t*)L"USER";
	var=wgetenv(varname);
	test(var!=0);
	test(-1 != wcstombs(buf, var, sizeof(buf)));
	test(-1 != wcstombs(buf1, varname, sizeof(buf1)));
	printf("  %s=%s\r\n", buf1, buf);

	sleep(5);

	test_Close();
	return 0;
	}

/* bodge up two pretend processes */

void do_mainA()
	{
	wchar_t* argv[] = { (wchar_t*)L"twpipe.exe", (wchar_t*)L"A" };

	wsetenv((wchar_t*)L"CLASSPATH",(wchar_t*)L".;classes;?:/classes;?:/system/java/lib/classes.zip",0);
	wsetenv((wchar_t*)L"VARIABLE2",(wchar_t*)L"value2",0);
	wsetenv((wchar_t*)L"USER",(wchar_t*)L"Tumblin' Wide Boy - Go fer yer guns!",0);

	do_main(2, argv);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1093
@SYMTestCaseDesc	    Tests for operations on a child pipe
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for reading from a pipe in the child process
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void do_mainB()
	{
	test_Data;
	wchar_t* argv[] = {(wchar_t*) L"twpipe.exe", (wchar_t*)L"B" };
	int nbytes=1;
	int mask, err;
	int firstread=1;

	/* Don't use test_* because it involves fflush and will cause deadlock */
	printf("PIPE Child");

	printf("Child reads from STDIN\n");
	while (nbytes>0)
		{
		char buf[5];
		printf("Child calls read of fid=0\n");
		nbytes=read(0,buf,sizeof(buf));
		printf("Child got %d bytes\n", nbytes);
		if (nbytes>0)
			{
			if (firstread)
				{
				/* We hope that there is still stuff to read */
				printf("Child calls E32IOSELECT(WRITE) on STDIN\n");
				mask=E32SELECT_WRITE;
				err=ioctl(0,E32IOSELECT,(void*)&mask);
				test(err==0);
				test(mask==0);
				printf("Child calls E32IOSELECT(READ) on STDIN\n");
				mask=E32SELECT_READ+E32SELECT_WRITE;
				err=ioctl(0,E32IOSELECT,(void*)&mask);
				test(err==0);
				test(mask==E32SELECT_READ);
				firstread=0;
				}
			printf("  %02d >%.*s<\n", nbytes, nbytes, buf);
			}
		else
		if (nbytes<0)
			printf("  %d (errno=%d)\n", nbytes, errno);
		}

	printf("Child calls E32IOSELECT on STDOUT");
	mask=E32SELECT_READ+E32SELECT_WRITE;
	err=ioctl(1,E32IOSELECT,(void*)&mask);
	test(err==0);
	test(mask==E32SELECT_WRITE);

	fflush(stdout);

	do_main(2, argv);
	fclose(stdout);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1094
@SYMTestCaseDesc	    Tests for pipe file descriptors
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing to the pipe file descriptors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void do_piping()
	{
	test_Data;
	char* s;
	char buf[180];

	test_Title("PIPE");

	test_Next("Pipe file descriptors");
	printf("  fids= %d, %d, %d\n", fids[0], fids[1], fids[2]);

	if (fids[0]>=0)
		{
		int nbytes;
		int remainder;
		int mask, err;
		const char* buf="test data 18 bytes";

		test_Next("E32IOSELECT(READ) on Child STDIN");
		mask=E32SELECT_READ;
		err=ioctl(fids[0],E32IOSELECT,(void*)&mask);
		test(err==0);
		test(mask==0);
		test_Next("E32IOSELECT(WRITE) on Child STDIN");
		mask=E32SELECT_READ+E32SELECT_WRITE;
		err=ioctl(fids[0],E32IOSELECT,(void*)&mask);
		test(err==0);
		test(mask==E32SELECT_WRITE);

		test_Next("Child STDIN");
		remainder=strlen(buf);
		while (remainder>0)
			{
			int length=(remainder<7)?remainder:7;
			nbytes=write(fids[0],buf,length);
			printf("  >%.*s<",nbytes,buf);
			buf+=nbytes;
			remainder-=nbytes;
			test(nbytes>0);
			test(nbytes<=length);
			}
		printf("\n");
		close(fids[0]);
		sleep(5);
		}
	if (fids[1]>=0)
		{
		FILE* cout;
		int mask, err;
		int firstread=1;

		test_Next("Child STDOUT");
		cout=wfdopen(fids[1],(wchar_t*)L"r");
		test(cout!=0);

		do
			{
			s=fgets(buf,sizeof(buf),cout);
			if (s!=0)
				{
				printf("  >%s", s);
				if (firstread)
					{
					test_Next("\nE32IOSELECT(WRITE) on Child STDOUT");
					mask=E32SELECT_WRITE;
					err=ioctl(fids[1],E32IOSELECT,(void*)&mask);
					test(err==0);
					test(mask==0);
					test_Next("E32IOSELECT(READ) on Child STDOUT");
					mask=E32SELECT_READ+E32SELECT_WRITE;
					err=ioctl(fids[1],E32IOSELECT,(void*)&mask);
					test(err==0);
					test(mask==E32SELECT_READ);
					firstread=0;
					}
				}
			}
		while (s!=0);
		fclose(cout);
		}

	test_Close();
	}

/* Linked with mcrt0.o, so that the exe starts the CPosixServer automatically as per the
 * plan all along.
 */

int wmain(int argc, wchar_t* argv[])
	{
	void* proc2;

	start_redirection_server();

	if (argc==1)
		{
		do_mainA();

		proc2 = create_process(do_mainB, "B", "rw", fids);
		if (proc2)
			start_process(proc2);
		else
			perror("Failed to start processB: ");

		if (proc2)
			{
			int exit;
			do_piping();
			exit=wait_for_process(proc2);
			printf("wait_for_process returned %d\r\n", exit);
			}
		}
	else
		{
		do_mainB();
		}

	return 0;
	}
