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
* Test of NETDB.H functions - you need a real Internet connection for this!
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
#include <errno.h>

#include "CTEST.H"	/* includes C interface to EPOC32 threads, and SpawnPosixServer */

test_Data;

#define N_ADDRESSES	3
unsigned long addresses[N_ADDRESSES][3] =
	{
		{ 121, 0x112233, 0x79112233 },	/* class A 121.xxxxxxxx */
		{ 33771, 0x1122, 0x83eb1122 },	/* class B 131.235.xxxx */
		{ 12747009,  90, 0xc281015a }	/* class C 194.129.1.xx */
	};

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1072
@SYMTestCaseDesc	    Tests for ARPA net functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for all basic network functions 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void testArpa()
	{
	char* cp;
	char* cp2;
	struct in_addr iaddr;
	unsigned long ul1, ul2;
	int err;
	int i;

	test_Next("ARPA/INET.H functions");

	iaddr.s_addr=11;
	cp="16.33.50.67";
	err=inet_aton(cp, &iaddr);
	test(err==1);
	test(iaddr.s_addr==htonl(0x10213243));
	test(iaddr.s_addr==inet_addr(cp));
	cp2=inet_ntoa(iaddr);
	test(strcmp(cp2,cp)==0);

	iaddr.s_addr=11;
	err=inet_aton("16.rubbish.67", &iaddr);
	test(err==0);

	for (i=0;i<N_ADDRESSES;i++)
		{
		iaddr=inet_makeaddr(addresses[i][0], addresses[i][1]);
		test(iaddr.s_addr==ntohl(addresses[i][2]));
		ul1=inet_netof(iaddr);
		ul2=inet_lnaof(iaddr);
		test(ul1==addresses[i][0]);
		test(ul2==addresses[i][1]);
		}
	}

#define N_NAMES	3
const char* names[N_NAMES][3] = 
	{
		{ "phoenix.doc.ic.ac.uk",  "146.169.1.160",    "phoenix.doc.ic.ac.uk" },
		{ "httpsmtp.test.intra",   "192.168.20.11",   "httpsmtp.test.intra" },
		{ "unix.sri.com",          "128.18.30.211",    "unix.sri.com" }
	};

int close_console=0;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1073
@SYMTestCaseDesc	    Tests for ARPA net functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for get host by name and host by address functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void testNetDB()
	{
	char hostname[128];
	struct in_addr addr, *addrp;
	int err, i;
	struct hostent *hp;

	test_Next("Get Host Name");

	err=gethostname(hostname,sizeof(hostname));
	test(err==0);
	printf("  hostname = >%s<\n", hostname);

	test_Next("Get Host By Name");

	for (i=0; i<N_NAMES; i++)
		{
		hp=gethostbyname(names[i][0]);
		test_ok(hp!=0);
		addrp=(struct in_addr*)(hp->h_addr_list[0]);
		printf("  %-30s => address %-15s\n", hp->h_name, inet_ntoa(*addrp));
		test(strcasecmp(hp->h_name,names[i][0])==0);
		test(addrp->s_addr==inet_addr(names[i][1]));
		}

	hp=gethostbyname("nosuchname.symbian.com");
	test_errno(hp==0,ENOENT);
	test(errno==HOST_NOT_FOUND);

	test_Next("Get Address of \"\"");
	hp=gethostbyname("");
	test_ok(hp!=0);
	addrp=(struct in_addr*)(hp->h_addr_list[0]);
	printf("  %-30s => address %-15s\n", hp->h_name, inet_ntoa(*addrp));

	test_Next("Get Host By Addr");

	for (i=0; i<N_NAMES; i++)
		{
		addr.s_addr=inet_addr(names[i][1]);
		hp=gethostbyaddr((const char *)&addr,sizeof(addr),AF_INET);
		test_ok(hp!=0);
		addrp=(struct in_addr*)(hp->h_addr_list[0]);
		printf("  address %-15s => %s\n", inet_ntoa(*addrp), hp->h_name);
		test(addrp->s_addr==addr.s_addr);
		test(strcasecmp(hp->h_name,names[i][2])==0);
		}

	addr.s_addr=inet_addr("10.11.199.255");
	hp=gethostbyaddr((const char *)&addr,sizeof(addr),AF_INET);
	test_errno(hp==0,ENOENT);
	test(errno==HOST_NOT_FOUND);
	
/*
	struct sockaddr_in testaddr;
	int fd;
	test_Next("Connect to the Imperial College Echo server");

	fd=socket(AF_INET, SOCK_STREAM, 0);
	test_ok(fd>=0);
	testaddr.sin_family=AF_INET;
	testaddr.sin_addr.s_addr=inet_addr("193.63.255.1");
	testaddr.sin_port=htons(7);	// echo
	err=connect(fd,(struct sockaddr*)&testaddr, sizeof(testaddr));
	test(err==0);
	close(fd);
*/

	test_Next("Get Address of roundrobin.test.intra which has multiple address");
	hp=gethostbyname("roundrobin.test.intra");	
	test_ok(hp!=0);

	if (hp)
		{
		if (hp->h_addr_list)
			{
			int Index = 0;
			while (hp->h_addr_list[Index])
				{
				addrp = (struct in_addr*)(hp->h_addr_list[Index]);
				printf("  %-30s => address %-15s\n", hp->h_name, inet_ntoa(*addrp));
				Index++;
				}
			}
		}

	test_Next("Get Host name of 192.168.255.4 which has multiple host name");
	addr.s_addr=inet_addr("192.168.255.4");
	hp=gethostbyaddr((const char *)&addr,sizeof(addr),AF_INET);
	test_ok(hp!=0);

	if (hp)
		{
		addrp=(struct in_addr*)(hp->h_addr_list[0]);
		printf("  address %-15s => %s\n", inet_ntoa(*addrp), hp->h_name);

		if (hp->h_aliases)
			{
			int Index = 0;
			while (hp->h_aliases[Index])
				{
				printf("  address %-15s => %s\n", inet_ntoa(*addrp), hp->h_aliases[Index]);
				Index++;
				}
			}
		}

	if (close_console)
		{
		test_Close();
		close(0);
		close(1);
		close(2);
		}
	}


int main(int argc, char *argv[])
	{
	void* client;
	int err;

	test_Title("NETDB.H Functionality");

	err=CommInit(0);	/* ensure a workable comms environment */
	test(err==0);

	testArpa();	/* doesn't use the MSystemInterface so only tested once */

	if (argc==1)
		{
		/* Run the test(s) without a CPosixServer first */		
		testNetDB();
		}

	test_Next("Do it all using the CPosixServer (for them, not me)");
	close_console=1;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */

	client=create_thread(testNetDB, "TNETDB NetDB");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);

	test_Close();
	exit(0);
	return 0;
	}
