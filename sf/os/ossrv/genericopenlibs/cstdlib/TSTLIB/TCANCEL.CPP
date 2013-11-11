// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test code for interrupted IO operations
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

#include <sys/ioctl.h>		// for E32IOSELECT
#include <sys/socket.h>
#include <libc/netinet/in.h>

extern "C" {
#include "CTEST.H"
}
test_Data;

#include <estlib.h>	// for multi-threading control

int testFid;
struct sockaddr_in testAddr;
RSemaphore semaphores[2];

void init_test()
	{
	// We need a datagram socket that we can send to

	test_Next("Create datagram socket");
	testFid=socket(PF_INET, SOCK_DGRAM, 0);
	test_ok(testFid>=0);
	
	test_Next("Bind to local address");
	IN_SET_LOOPBACK_ADDR(&testAddr);
	testAddr.sin_port=0;

	size_t len=sizeof(testAddr);
	int err=bind(testFid, (struct sockaddr*)&testAddr, len);
	test_ok(err==0);

	err=getsockname(testFid, (struct sockaddr*)&testAddr, &len);
	test_ok(err==0);

	// We now have a datagram socket with a known address

	test_Next("Create semaphores for test synchronisation");
	for (int i=0; i<2; i++)
		{
		err=semaphores[i].CreateLocal(0);
		test(err==KErrNone);
		}
	}

/**
Test of the asynchronous ioctl & cancel code

@SYMTestCaseID          SYSLIB-STDLIB-CT-1044
@SYMTestCaseDesc	    Tests for the asynchronous ioctl & cancel code
@SYMTestPriority 	    High
@SYMTestActions  	    Tests that E32IOSELECT ioctl is working
						Tests for asynchronous IOCTL.
						Setup asynchronous IOCTL which won't complete, then cancel.Check for cancel status
						Cancel a completed asynchronous IOCTL,and check for no error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void test_ioctl()
	{
	// confirm that the E32IOSELECT ioctl is working
	test_Next("Check synchronous IOCTL");
	int mask=E32SELECT_READ|E32SELECT_WRITE|E32SELECT_EXCEPT;
	int err=ioctl(testFid,E32IOSELECT,&mask);
	test_ok(err==0);
	test(mask==E32SELECT_WRITE);

	test_Next("Check asynchronous IOCTL");
	TRequestStatus status;
	mask=E32SELECT_READ|E32SELECT_WRITE|E32SELECT_EXCEPT;
	err=ioctl(testFid,E32IOSELECT,&mask,status);
	test_ok(err==0);
	User::WaitForRequest(status);
	test(status.Int()==KErrNone);
	err=ioctl_complete(testFid,E32IOSELECT,&mask,status);
	test_ok(err==0);
	test(mask==E32SELECT_WRITE);

	test_Next("Setup asynchronous IOCTL which won't complete, then cancel");
	mask=E32SELECT_READ;
	err=ioctl(testFid,E32IOSELECT,&mask,status);
	test_ok(err==0);
	test(status.Int()==KRequestPending);	// i.e. waiting for input to arrive

	// test_Next("Cancel the pending IOCTL");
	// Sadly this will do write() which will get a KErrInUse and then panic...
	err=ioctl_cancel(testFid);
	test_ok(err==0);
	User::WaitForRequest(status);
	test_Next("Check that ioctl_cancel worked");
	test(status.Int()==KErrCancel);			// i.e. it was cancelled

	test_Next("Cancel a completed asynchronous IOCTL");
	mask=E32SELECT_WRITE;
	err=ioctl(testFid,E32IOSELECT,&mask,status);
	test_ok(err==0);
	
	while (status.Int() == KRequestPending)
		User::After(5000);

	test(status.Int()!=KRequestPending);	// i.e. select has completed

	// test_Next("Cancel the pending IOCTL");
	// Sadly this will do write() which will get a KErrInUse and then panic...
	err=ioctl_cancel(testFid);
	test_ok(err==0);
	User::WaitForRequest(status);
	test(status.Int()==KErrNone);			// i.e. it was cancelled

	}

// Thread functions which will block accessing the socket

TInt ioctl_block(TAny* aSemIndex)
	{
	semaphores[(TInt)aSemIndex].Wait();		// block until the test is ready

	int mask=E32SELECT_READ;
	int err=ioctl(testFid,E32IOSELECT,&mask);
	test_ok(err==0);
	test(mask==E32SELECT_READ);
	return 1;
	}

TInt recv_block(TAny* aSemIndex)
	{
	semaphores[(TInt)aSemIndex].Wait();		// block until the test is ready

	char buf[256];
	// Problem in recvfrom: int err=recvfrom(testFid,buf,MSG_PEEK,sizeof(buf),0,0);
	struct sockaddr_in junk;
	size_t junklen=sizeof(junk);
	// Problem in esock/tcpip: int err=recvfrom(testFid,buf,sizeof(buf),MSG_PEEK,(struct sockaddr*)&junk,&junklen);
	int err=recvfrom(testFid,buf,sizeof(buf),0,(struct sockaddr*)&junk,&junklen);
	// Problem in recfrom: test_ok(err>0);
	test_ok(err>=0);
	return 1;
	}

void kill_and_check(RThread& aVictim, TRequestStatus& aVictimStatus, TRequestStatus& aSurvivorStatus)
	{
	test(aVictimStatus.Int()==KRequestPending);
	test(aSurvivorStatus.Int()==KRequestPending);
	aVictim.Kill(666);
	char* KDatagram="Mine is the last voice you will ever hear";
	int length=strlen(KDatagram)+1;
	
	test_Next("Send datagram");
	int err=sendto(testFid,KDatagram, length,0,(struct sockaddr*)&testAddr,sizeof(testAddr));
	test_ok(err==length);
	User::After(1);

	test_Next("Check the victim status");
	User::WaitForRequest(aVictimStatus);
	test(aVictimStatus.Int()==666);

	test_Next("Check the survivor status");
	User::WaitForRequest(aSurvivorStatus);
	test(aSurvivorStatus.Int()==1);

	// Check to see if the datagram has been swallowed
	int mask=E32SELECT_READ|E32SELECT_WRITE|E32SELECT_EXCEPT;
	err=ioctl(testFid,E32IOSELECT,&mask);
	test_ok(err==0);
	if ((mask&E32SELECT_READ)==0)
		return;		// yes - nothing to read

	test_Next("Consume the datagram");
	char buf[256];
	// Problem in recvfrom: err=recvfrom(testFid,buf,sizeof(buf),0,0,0);
	struct sockaddr_in junk;
	size_t junklen=sizeof(junk);
	err=recvfrom(testFid,buf,sizeof(buf),0,(struct sockaddr*)&junk,&junklen);
	// Problem in recfrom: test_ok(err==length);
	test_ok(err>=0);
	}
/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1045
@SYMTestCaseDesc	    Tests for killing an ioctl
@SYMTestPriority 	    High
@SYMTestActions  	    Create two threads which will block on the socket
						then kill one of them and send a datagram 
						check the exit status of both threads
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void test_killing(TInt aThread, TThreadFunction aFunction, char* aTitle)
	{
	_LIT(KThreadName, "TCancel Test Thread %d");
	TBuf<80> threadName;
	static TInt threadNumber=0;

	test_Next(aTitle);
	// test_Next("Create test thread A");
	RThread thread1;
	TRequestStatus status1;
	threadName.Format(KThreadName,++threadNumber);
	TInt err=thread1.Create(threadName,aFunction,0x10000,NULL,(TAny*)0);
	test(err==KErrNone);
	thread1.Logon(status1);
	thread1.Resume();

	// test_Next("Create test thread B");
	RThread thread2;
	TRequestStatus status2;
	threadName.Format(KThreadName,++threadNumber);
	err=thread2.Create(threadName,aFunction,0x10000,NULL,(TAny*)1);
	test(err==KErrNone);
	thread2.Logon(status2);
	thread2.Resume();

	test_Next("Start thread A, then thread B...");
	semaphores[0].Signal();
	User::After(1);
	semaphores[1].Signal();
	User::After(1);

	if (aThread==1)
		{
		test_Next("Kill thread A");
		kill_and_check(thread1, status1, status2);
		}
	else
		{
		test_Next("Kill thread B");
		kill_and_check(thread2, status2, status1);
		}

	thread1.Close();
	thread2.Close();
	}

int main()
	{
	
	start_redirection_server();

	test_Title("TCANCEL");
	init_test();
	test_ioctl();		// explicit cancellation
	test_killing(1,ioctl_block, "Cancellation of active ioctl");	
	test_killing(2,ioctl_block, "Cancellation of queued ioctl");
	test_killing(1,recv_block,  "Cancellation of active recvfrom");	
	test_killing(2,recv_block,  "Cancellation of queued recvfrom");
	test_Close();
	return KErrNone;
	}
