// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test code for pipes, using dubious WINS extension for multiple processes...
// 
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>	// for MAXPATHLEN 
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <e32debug.h> // for RDebug::Print

#include <e32std.h>
#include <estlib.h>	// for multi-threading control

extern "C" {
#include "CTEST.H"
}

#ifdef _DEBUG
#define DebugPrint	RDebug::Print
#else
inline void DebugPrint(const TDesC&, ...) {}
#endif

// prepare buf for pipe write/read opereations
void fillbuf(int seed, char* buf, int buflen)
	{
	int j;
	sleep(seed/3);
	seed += 'A';
	for (j=0; j<buflen; j++)
		{
		buf[j]=(char)seed;
		seed+=13;
		if (seed > 127)
			seed = seed - 127 + 'A';
		}
	}

int fids[3];

// Producer, writing buf to pipe(fid)
void producer(int fid)
	{

	test_Data;
	char buf[128];
	int nbytes;

	test_Title("Producer");
	
	fillbuf(1,buf,sizeof(buf));
	fflush(stdout);
	
	// Pipe Write test
	nbytes=write(fid, buf, sizeof(buf));
	
	fflush(stdout);
	test(nbytes==sizeof(buf));
	
	TProcessId id=RProcess().Id();
	TUint pid=*REINTERPRET_CAST(TUint*,&id);
	DebugPrint(_L("Process %d: Pipe Write success"), pid);
	
	printf("1.\n\n");	
	return;
	}
	
#define select_test(fid)	\
		mask=E32SELECT_READ+E32SELECT_WRITE; \
		err=ioctl(fid,E32IOSELECT,(void*)&mask);

// consumer, doing ioctl test and then read from pipe(fid)					
void consumer(int fid)
	{

	test_Data;
	char buf[256];
	char checkbuf[256];
	int nbytes;
	int mask=E32SELECT_READ;
	int err=0;

	test_Title("Consumer");
	fillbuf(1,checkbuf,128);
	
	test_Next("Simple read, exactly matching write\n");
	
	// Pipe Ioctl test
	select_test(fid);

	test(err==0);
	test(mask==E32SELECT_READ);
	
	// Pipe Read test
	nbytes=read(fid,buf,128);
	
	test(nbytes==128);
	test(memcmp(buf,checkbuf,128)==0);

	TProcessId id=RProcess().Id();
	TUint pid=*REINTERPRET_CAST(TUint*,&id);
	DebugPrint(_L("Process %d: Pipe Read success"), pid);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-UT-1572
@SYMTestCaseDesc	    Tests for cancellation on pipe operations
@SYMTestPriority 	    High
@SYMTestActions  	    Cancel an outstanding pipe operation request 
						and check if CPosixIPCSession::PipeCancel() handling correct.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void pipeCancel(int fid)
	{
	test_Data;
	int err=0;
	int mask=E32SELECT_READ+E32SELECT_WRITE; 
	
	test_Title("Pipe cancellation");
	DebugPrint(_L("Pipe cancellation test"));
	TRequestStatus aStatus;

	// Issue an ansynchronous pipe operation request
	err=ioctl(fid,E32IOSELECT,(void*)&mask, aStatus);
	
	// Cancel pipe ioctl
	err=ioctl_cancel(fid);
	test(err==0);
	
	TProcessId id=RProcess().Id();
	TUint pid=*REINTERPRET_CAST(TUint*,&id);
	DebugPrint(_L("Process %d: Pipe Cancellation SUCCESS"), pid);
	
	test_Close();
	}

void do_parent()
	{
	// testing pipe with writing to child stdin
	producer(fids[0]);
	}
	
void do_child()
	{
	// testing pipe with reading from stdin
	consumer(0);

	// pipe cancellation test
	pipeCancel(0);
	
	// close pipe
	close(0);	
	}
	

// Linked with mcrt0.o, so that the exe starts the CPosixServer automatically as per the
// plan all along.

int main(int argc, char* argv[])
	{
	void* proc2;

	start_redirection_server();

	if (argc==1)
		{
		// create Child process with read/err pipes	
		proc2 = create_process(do_child, "CHILD", "r", fids);
		if (proc2)
			start_process(proc2);
		else
			perror("Failed to start process CHILD: ");

		if (proc2)
			{
			int exit;
			
			// parent process
			do_parent();
			exit=wait_for_process(proc2);
			printf("wait_for_process() returned %d\r\n", exit);
			}
		}
	else
		{
		// child process
		do_child();
		}

	// exit here, for the moment crt0 libraries panic
	exit(0);
	
	return KErrNone;
	}
	
