// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Test code for multi-threaded file descriptors etc.
// 
//

#include <e32std.h>
#include <e32svr.h>	// for RDebug
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/errno.h>

extern "C" {
#include "CTEST.H"
}

#include <estlib.h>	// for multi-threading control


#ifdef _DEBUG
	#define ttest_Next(testname)	\
	RDebug::Print(_L("t%d: %s"), (TInt)thread, testname); \
	printf("t%d: ", (TInt)thread); \
	test_Next(testname)
#else
	//don't use RDebug::Print when it's a release build
	#define ttest_Next(testname)   test_Next(testname)
#endif

test_Data;
RSemaphore waiting[2];

void over(TInt n)
	{
	// RDebug::Print(_L("Over(%d)"),n);
	waiting[1-n].Signal();
	waiting[n].Wait();
	}

#define THREAD0		if (thread==0)
#define THREAD1		if (thread==(TAny*)1)
#define OVER		over((TInt)thread)

// Shared variables

int fd;

/**
Interleaved test code

@SYMTestCaseID          SYSLIB-STDLIB-CT-1071
@SYMTestCaseDesc	    Tests for multi-threaded file descriptors
@SYMTestPriority 	    High
@SYMTestActions  	    Run threads to open,read,close test files
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
TInt testfunction(TAny* thread)
	{
	int n;
	char *p;
	ttest_Next("Entering testfunction");
	//
	ttest_Next("Competing console reads - press 'A' then 'B'...");
	fflush(stdout);
	int c=getchar();
	fprintf(stderr, "t%d: read char %d\r\n", thread, c);
	//
	THREAD0
		{
		waiting[0].Wait();	// until Thread1 says OVER
		ttest_Next("Create test file");
		fd=open("c:\\testfile", O_RDWR+O_CREAT+O_TRUNC, 0777);
		test_ok(fd>=0);
		}
	//
	OVER;
	ttest_Next("Get the sequencing sorted out...");
	OVER;
	THREAD1
		{
		ttest_Next("Write to test file");
		p="Hello from thread 1\r\n";
		n=write(fd,p,strlen(p));
		test_ok(n==strlen(p));
		}
	THREAD0
		{
		ttest_Next("Close test file");
		close(fd);
		}
	OVER;
	THREAD1
		{
		ttest_Next("Reopen test file");
		fd=open("c:\\testfile",O_RDONLY,0);
		test_ok(fd>=0);
		}
	THREAD0
		{
		ttest_Next("Read from test file");
		char buf[80];
		buf[6]='\0';
		n=read(fd,buf,6);
		test_ok(n==6);
		test(strncmp(buf,"Hello ",6)==0);
		printf("Read >%s<... \r\n",buf);
		fflush(stdout);
		}
	OVER;
	THREAD1
		{
		ttest_Next("Read from test file");
		close(0);
		n=dup2(fd,0);
		test(n>=0);	// associate stdin with "testfile"
		char buf[80];
		p=fgets(buf,80,stdin);
		test_ok(p==buf);
		fprintf(stderr, "Read >%s<\r\n", buf);
		}
	THREAD0
		{
		ttest_Next("Close test file");
		close(fd);
		}
	OVER;
	ttest_Next("Completed testfunction");
	waiting[0].Signal();	// allow thread0 to continue
	waiting[1].Signal();	// allow thread1 to continue
	return 0;
	}

// Thread management - main thread is thread0

void init_threads()
	{
	waiting[0].CreateLocal(0);
	waiting[1].CreateLocal(0);

	RThread thread1;
	TInt err=thread1.Create(_L("Thread1"),testfunction,0x10000,NULL,(TAny*)1);
	test(err==KErrNone);
	test_Next("Starting thread1");
	thread1.Resume();
	test_Next("entering main test...");
	}

int main(int argc, char *argv[])
	{
	// SpawnPosixServerThread(); - provided by MCRT0.OBJ

	start_redirection_server();

	test_Title("TMTHREAD");
	init_threads();
	testfunction(0);
	test_Close();
	return KErrNone;
	}
