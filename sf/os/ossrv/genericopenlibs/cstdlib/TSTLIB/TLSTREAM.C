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
* Test of AF_LOCAL stream sockets
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

#include "ctest.h"	/* includes C interface to EPOC32 threads, and SpawnPosixServer */

test_Data;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1115
@SYMTestCaseDesc	    Tests for AF_LOCAL stream sockets
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for create socket,binding and other simple socket operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void testSimple()
	{
	int fd1, fd2;
	size_t addrsize;
	int err;
	struct sockaddr addr1, addr2;
	int optionbuf[20];
	size_t optionsize;

	test_Next("Create stream sockets");
	fd1=socket(AF_LOCAL, SOCK_STREAM, 0);
	test_ok(fd1>=0);

	fd2=socket(AF_LOCAL, SOCK_STREAM, 0);
	test_ok(fd2>=0);

	test_Next("Some binding tests");

#if 0 /* causes socket server panic with ESOCK 058 */
	addr1.sa_family=AF_UNSPEC;
	addr1.sa_port=65530;
	err=bind(fd1,&addr1, sizeof(addr1));	/* wrong family, port out of range */
	test_ok(err!=0);
#endif

	addr1.sa_family=AF_LOCAL;
	addr1.sa_port=1;
	err=bind(fd1, &addr1, sizeof(addr1));
	test_ok(err==0);

	addr1.sa_family=AF_LOCAL;
	addr1.sa_port=2;
	err=bind(fd1, &addr1, sizeof(addr1));	/* already bound */
	test_errno(err!=0, EEXIST);

	test_Next("Get associated addresses");

	addrsize=sizeof(addr2);
	err=getsockname(fd1,&addr2,&addrsize);
	test_ok(err==0);
	/* test(addr2.sa_family==AF_LOCAL);	problem in IPC.PRT, for ESOCK 058 */
	test(addr2.sa_port==1);
	test(addrsize<=sizeof(addr2));

	addrsize=sizeof(addr2);
	err=getpeername(fd1,&addr2,&addrsize);	/* not connected */
	test_errno(err!=0,-2);	/* OMISSION - can't report proper reason for failure */

	addrsize=sizeof(addr2);
	err=getsockname(fd2,&addr2,&addrsize);	/* not bound */
	test_errno(err!=0,-2);	/* OMISSION - can't report proper reason for failure */

	test_Next("More binding");

	addr1.sa_family=AF_LOCAL;
	addr1.sa_port=1;
	err=bind(fd2, &addr1, sizeof(addr1));	/* address in use */
	test_errno(err!=0, EACCES);

#if 0	/* this isn't supported by AF_LOCAL */
	addr1.sa_family=AF_LOCAL;
	addr1.sa_port=0;
	err=bind(fd2, &addr1, sizeof(addr1));	/* unspecified port number */
	test_ok(err==0);

	addrsize=sizeof(addr2);
	err=getsockname(fd2,&addr2,&addrsize);
	test_ok(err==0);
	/* test(addr2.sa_family==AF_LOCAL);	problem in IPC.PRT, for ESOCK 058 */
	test(addr2.sa_port!=1);
	test(addr2.sa_port!=0);
	test(addrsize<=sizeof(addr2));
#endif

	err=listen(fd1,1);
	test_ok(err==0);

	test_Next("Socket options");

	optionbuf[0]=3500000;	/* implausible size */
	optionsize=sizeof(optionbuf[0]);
	err=getsockopt(fd1,SOL_SOCKET,SO_SNDBUF,optionbuf,&optionsize);
	test_ok(err==0);
	test(optionbuf[0]!=3500000);

	optionbuf[0]=7*1024;
	optionsize=sizeof(optionbuf[0]);
	err=setsockopt(fd1,SOL_SOCKET,SO_SNDBUF,optionbuf,optionsize);
	test_ok(err==0);

	optionbuf[0]=3500000;	/* implausible size */
	optionsize=sizeof(optionbuf[0]);
	err=getsockopt(fd1,SOL_SOCKET,SO_SNDBUF,optionbuf,&optionsize);
	test_ok(err==0);
	test(optionbuf[0]==7*1024);

	optionbuf[0]=1;
	optionsize=sizeof(optionbuf[0]);
	err=getsockopt(fd1,SOL_SOCKET,1234,optionbuf,&optionsize);	/* invalid option */
	test_errno(err<0,ENOSYS);
	test(optionbuf[0]==1);

	optionbuf[0]=13;
	optionsize=sizeof(optionbuf[0]);
	err=setsockopt(fd1,SOL_SOCKET,1234,optionbuf,optionsize);	/* invalid option */
	test_errno(err<0,ENOSYS);
	test(optionbuf[0]==13);

	err=close(fd1);
	test_ok(err==0);

	err=close(fd2);
	test_ok(err==0);
	}

/* Client and server take it in turns to send, starting with the client.
 * Each matches the message they receive with the string expected
 */
char *message_sequence[] = {
	"Hello from client",
	"Hello from server",
	"Test of send",
	"Test of recv",
	"Try sendto",
	"Try recvfrom",
	"Send to shutdown socket",
	"Send to closed socket",
	0
	};

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1116
@SYMTestCaseDesc	    Tests for server socket
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for server socket,create,accept,send and receive functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void testServer()
	{
	int fd1, fd2, nbytes, i;
	size_t addrsize;
	int err;
	struct sockaddr addr1, addr2;
	char buf[80];
	char **mp = message_sequence;

	test_Next("Create server socket");
	fd1=socket(AF_LOCAL, SOCK_STREAM, 0);
	test_ok(fd1>=0);

#if 0
	/* causes ESOCK to panic the client */
	addrsize=sizeof(addr2);
	fd2=accept(fd1,&addr2,&addrsize);	/* can't accept on an unbound socket */
	test_ok(fd2<0);
#endif

	addr1.sa_family=AF_LOCAL;
	addr1.sa_port=1;
	err=bind(fd1, &addr1, sizeof(addr1));
	test_ok(err==0);

#if 0
	/* causes ESOCK to panic the client */
	addrsize=sizeof(addr2);
	fd2=accept(fd1,&addr2,&addrsize);	/* can't accept before listening */
	test_ok(fd2<0);
#endif

	err=listen(fd1,1);
	test_ok(err==0);

	addrsize=sizeof(addr2);
	fd2=accept(fd1,&addr2,&addrsize);
	test_ok(fd2>=0);
	/* test(addr2.sa_family==AF_LOCAL);	problem in IPC.PRT, for ESOCK 058 */
	test(addr2.sa_port!=1);
	test(addr2.sa_port!=0);
	test(addrsize<=sizeof(addr2));

	test_Next("Server read/write");

	/* read */
	nbytes=strlen(*mp);
	err=read(fd2, buf, nbytes+1);
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);

	/* write */
	mp++;
	nbytes=strlen(*mp);
	for (i=0; i<nbytes+1; i++)
		{
		err=write(fd2,(*mp)+i,1);
		test_ok(err==1);
		}

	test_Next("Server send/recv");

	/* recv */
	mp++;
	nbytes=strlen(*mp);
	err=recv(fd2, buf, nbytes+1,0);
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);

	/* send */
	mp++;
	nbytes=strlen(*mp);
	err=send(fd2, *mp, nbytes+1,0);
	test_ok(err==nbytes+1);

	/* recvfrom */
	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr2);
	addr2.sa_port=0;
	err=recvfrom(fd2, buf, nbytes+1,0,&addr2,&addrsize);
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);
	/* test(addr2.sa_family==AF_LOCAL);	problem in IPC.PRT, for ESOCK 058 */
	test(addr2.sa_port!=1);
	test(addr2.sa_port!=0);
	test(addrsize<=sizeof(addr2));

	/* sendto */
	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr1);
	err=sendto(fd2, *mp, nbytes+1,0,&addr1,addrsize);	/* not allowed on streams */
	test_errno(err<0,ENOSYS);
	err=send(fd2, *mp, nbytes+1,0);		/* to keep synchronisation */
	test_ok(err==nbytes+1);

	test_Next("Server shutdown reception");
	err=shutdown(fd2,0);
	test_ok(err==0);

	sleep(2);	/* so that the client's sleep(1) finishes before we awake */

	err=close(fd2);
	test_ok(err==0);

	err=close(fd1);
	test_ok(err==0);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1117
@SYMTestCaseDesc	    Tests for client socket
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for client socket,create,accept,send and receive functions
                        Write to a connection closed socket and test for error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void testClient()
	{
	int fd1, nbytes, nbytes2, i, status;
	size_t addrsize;
	int err;
	struct sockaddr addr1, addr2;
	char buf[80];
	char **mp = message_sequence;

	test_Next("Create client socket");
	fd1=socket(AF_LOCAL, SOCK_STREAM, 0);
	test_ok(fd1>=0);

	addr1.sa_family=AF_LOCAL;
	addr1.sa_port=1;
	addrsize=sizeof(addr1);
	err=connect(fd1,&addr1,addrsize);
	test_ok(err==0);

	addrsize=sizeof(addr2);
	err=getpeername(fd1,&addr2,&addrsize);
	test_ok(err==0);
	/* test(addr2.sa_family==AF_LOCAL);	problem in IPC.PRT, for ESOCK 058 */
	test(addr2.sa_port==1);
	test(addrsize<=sizeof(addr2));

	addrsize=sizeof(addr2);
	err=getsockname(fd1,&addr2,&addrsize);
	test_ok(err==0);
	/* test(addr2.sa_family==AF_LOCAL);	problem in IPC.PRT, for ESOCK 058 */
	test(addr2.sa_port!=1);
	test(addr2.sa_port!=0);
	test(addrsize<=sizeof(addr2));

	test_Next("Client read/write");

	/* write */
	nbytes=strlen(*mp);
	err=write(fd1, *mp, nbytes+1);
	test_ok(err==nbytes+1);

	/* read */
	mp++;
	nbytes=strlen(*mp);
	err=read(fd1, buf, nbytes+1);
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);

	test_Next("Client send/recv");

	/* send */
	mp++;
	nbytes=strlen(*mp);
	for (i=0; i<nbytes+1; i++)
		{
		err=send(fd1,(*mp)+i,1,0);
		test_ok(err==1);
		}

	/* recv - get the first byte so that we know the buffer is full */
	mp++;
	nbytes=strlen(*mp);
	err=recv(fd1,buf,1,0);
	test_ok(err==1);

	/* ioctl */
	nbytes2=-1;
	err=ioctl(fd1,E32IONREAD,&nbytes2);
#if 0
	test_ok(err==0);
	test(nbytes2==nbytes);
#else
	test_errno(err<0,ENOSYS);	/* IPC.PRT doesn't implement KSoReadBytesPending */
#endif

	nbytes2=E32SELECT_READ|E32SELECT_WRITE|E32SELECT_EXCEPT;
	err=ioctl(fd1,E32IOSELECT,&nbytes2);
	test_ok(err==0);
	test(nbytes2==(E32SELECT_READ|E32SELECT_WRITE));

	nbytes2=E32SELECT_READ|E32SELECT_WRITE|E32SELECT_EXCEPT;
	err=async_ioctl(fd1,E32IOSELECT,&nbytes2,&status);
	test_ok(err==0);
	err=async_ioctl_completion(fd1,E32IOSELECT,&nbytes2,&status);
	test_ok(err==0);
	if (nbytes2!=(E32SELECT_READ|E32SELECT_WRITE))
		{
		nbytes2=E32SELECT_READ|E32SELECT_WRITE|E32SELECT_EXCEPT;
		err=ioctl(fd1,E32IOSELECT,&nbytes2);
		test_ok(err==0);
		test(nbytes2==(E32SELECT_READ|E32SELECT_WRITE));
		}		

	/* recv - get the rest of the data */
	for (i=1; i<nbytes+1; i++)
		{
		err=recv(fd1,buf+i,1,0);
		test_ok(err==1);
		}
	test(strcmp(buf,*mp)==0);

	/* ioctl again - this time there is no data pending */
	nbytes2=-1;
	err=ioctl(fd1,E32IONREAD,&nbytes2);
#if 0
	test_ok(err==0);
	test(nbytes2==0);
#else
	test_errno(err<0,ENOSYS);	/* IPC.PRT doesn't implement KSoReadBytesPending */
#endif

	nbytes2=E32SELECT_READ|E32SELECT_WRITE|E32SELECT_EXCEPT;
	err=ioctl(fd1,E32IOSELECT,&nbytes2);
	test_ok(err==0);
	test(nbytes2==E32SELECT_WRITE);

	nbytes2=E32SELECT_READ|E32SELECT_WRITE|E32SELECT_EXCEPT;
	err=async_ioctl(fd1,E32IOSELECT,&nbytes2,&status);
	test_ok(err==0);
	err=async_ioctl_completion(fd1,E32IOSELECT,&nbytes2,&status);
	test_ok(err==0);
	test(nbytes2==E32SELECT_WRITE);

	/* sendto */
	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr1);
	err=sendto(fd1, *mp, nbytes+1,0,&addr1,addrsize);
	test_errno(err<0,ENOSYS);
	err=send(fd1, *mp, nbytes+1,0);		/* to keep synchronisation */
	test_ok(err==nbytes+1);

	/* recvfrom */
	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr2);
	addr2.sa_port=0;
	err=recvfrom(fd1, buf, nbytes+1,0,&addr2,&addrsize);
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);
	/* test(addr2.sa_family==AF_LOCAL);	problem in IPC.PRT, for ESOCK 058 */
	test(addr2.sa_port==1);
	test(addrsize<=sizeof(addr2));

	sleep(1);

	test_Next("Client write to closed connection");

	/* write to half-closed socket */
	mp++;
	nbytes=strlen(*mp);
	err=write(fd1, *mp, nbytes+1);
#if 0
	/* IPC doesn't seem to care! */
	test_errno(err<0, EPIPE);
#else
	test_ok(err==nbytes+1);
#endif

	sleep(2);	
	
	/* write to a connection closed by the other end */
	mp++;
	nbytes=strlen(*mp);
	err=write(fd1, *mp, nbytes+1);
	test_errno(err<0, EPIPE);

	err=close(fd1);
	test_ok(err==0);
	}

int close_console=0;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1118
@SYMTestCaseDesc	    Tests for STREAM server
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for server entity
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void testConnection()
	{
	int err;
	void* server;
	
	server=create_thread(testServer, "TLSTREAM server");
	test(server!=0);
	start_thread(server);

	sleep(1);	/* give the server a chance to get started */
	testClient();

	err=wait_for_thread(server);
	test(err==0);

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

	test_Title("AF_LOCAL Streams");

	testSimple();
	testConnection();

	test_Next("Do it again using the CPosixServer (for them, not me)");
	close_console=1;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */

	client=create_thread(testSimple, "TLSTREAM simple");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);

	client=create_thread(testConnection, "TLSTREAM client");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);

	test_Close();
	return 0;
	}