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
* Test of the unsupported option values in setsockopt
* 
*
*/



#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <libc/netinet/in.h>
#include <libc/arpa/inet.h>
#include <netdb.h>
#include <reent.h>

#include "CTEST.H"	/* includes C interface to EPOC32 threads, and SpawnPosixServer */

test_Data;

struct sockaddr_in testaddr;

//The structure _reent contains *all* globals needed by the library.
struct _reent* p;

unsigned short int port = 2001;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1119
@SYMTestCaseDesc	    Tests for the unsupported option values in setsockopt
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for creation of a socket and setting unsupported options on a socket
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void testSimple()
	{
	int fd1;
	int err;
	int optionbuf[20];
	size_t optionsize;

	//Creating stream sockets to be used in the test
	test_Next("Create stream sockets");
	fd1=socket(AF_INET, SOCK_STREAM, 0);
	test_ok(fd1>=0);

	optionbuf[0]=7*1024;
	optionsize=sizeof(optionbuf[0]);
	
	//valid option e.g SO_SNDBUF(positive value 3)
	err=setsockopt(fd1,SOL_SOCKET,SO_SNDBUF,optionbuf,optionsize);
	p=_REENT;
	//No error in setsockopt with returned value in p->_errno=0
	test(err==0);
	test(p->_errno==0);


	//invalid(not supported) option e.g IP_ADD_MEMBERSHIP(negative value -8)
	err=setsockopt(fd1,SOL_SOCKET,IP_ADD_MEMBERSHIP,optionbuf,optionsize);
	p=_REENT;
	//Error in setsockopt with returned value in p->_errno=ENOSYS(88)
	test(err==-1);
	test(p->_errno==ENOSYS);
	
	}

int main()
	{
	int err;
	test_Title("AF_INET Streams");

	err=CommInit(0);	/* ensure a workable comms environment */
	test(err==0);
	IN_SET_LOOPBACK_ADDR(&testaddr);

	testSimple();

	test_Close();
	return 0;
	}
