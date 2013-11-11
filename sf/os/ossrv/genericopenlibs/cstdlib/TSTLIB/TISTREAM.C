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
* Test of AF_INET stream sockets
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

#include "CTEST.H"	/* includes C interface to EPOC32 threads, and SpawnPosixServer */

test_Data;

struct sockaddr_in testaddr;

unsigned short int port = 2001;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1059
@SYMTestCaseDesc	    Tests for AF_INET stream sockets 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for simple operations on stream sockets,open,bind,get socket name,listen operations and test for error code
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void testSimple()
	{
	int fd1, fd2;

	size_t addrsize;
	int err;
	struct sockaddr_in addr1, addr2;
	int optionbuf[20];
	size_t optionsize;

	test_Next("Create stream sockets");
	fd1=socket(AF_INET, SOCK_STREAM, 0);
	test_ok(fd1>=0);

	fd2=socket(AF_INET, SOCK_STREAM, 0);
	test_ok(fd2>=0);

	test_Next("Some binding tests");

	addr1.sin_family=2061;
	addr1.sin_port=htons(65530);
	addr1.sin_addr.s_addr=htonl(0x11223344);
	err=bind(fd1, (struct sockaddr*)&addr1, sizeof(addr1));	/* wrong family, port out of range */
	test_errno(err<0,ENOENT);

	addr1=testaddr;
	addr1.sin_port=htons(port);
	err=bind(fd1, (struct sockaddr*)&addr1, sizeof(addr1));
	test_ok(err==0);

	addr1=testaddr;
	addr1.sin_port=htons((unsigned short int)(port+1));
	err=bind(fd1, (struct sockaddr*)&addr1, sizeof(addr1));	/* already bound */
	test_errno(err!=0, EEXIST);

	test_Next("Get associated addresses");

	addrsize=sizeof(addr2);
	err=getsockname(fd1,(struct sockaddr*)&addr2,&addrsize);
	test_ok(err==0);
	test(addr2.sin_family==AF_INET);
	test(addr2.sin_port==htons(port));
	test(addrsize<=sizeof(addr2));

	addrsize=sizeof(addr2);
	err=getpeername(fd1,(struct sockaddr*)&addr2,&addrsize);	/* not connected */
	test_errno(err!=0,-2);	/* OMISSION - can't report proper reason for failure */

	addrsize=sizeof(addr2);
	err=getsockname(fd2,(struct sockaddr*)&addr2,&addrsize);	/* not bound */
	test_errno(err!=0,-2);	/* OMISSION - can't report proper reason for failure */

	test_Next("More binding");

	addr1=testaddr;
	addr1.sin_port=htons(port);
	err=bind(fd2, (struct sockaddr*)&addr1, sizeof(addr1));	/* address in use */
	test_errno(err!=0, EACCES);

	addr1=testaddr;
	addr1.sin_port=0;
	err=bind(fd2, (struct sockaddr*)&addr1, sizeof(addr1));	/* unspecified port number */
	test_ok(err==0);

	addrsize=sizeof(addr2);
	err=getsockname(fd2,(struct sockaddr*)&addr2,&addrsize);
	test_ok(err==0);
	test(addr2.sin_family==AF_INET);
	test(addrsize<=sizeof(addr2));
	test(addr2.sin_port!=htons(port));
	test(addr2.sin_port!=0);
	{
		unsigned short port=ntohs(addr2.sin_port);
		test(port>=IPPORT_RESERVED);
		/* NB. TCPIP 030 has 9999 for IPPORT_USERRESERVED, but TCPIP 031 dropped it to 5000 */
		test(port<=IPPORT_USERRESERVED);
		printf("  allocated port %d\n",port);
	}

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
	"Try sendto again",
	"Try recvfrom again",
	"Send to shutdown socket",
	"Send to closed socket",
	0
	};

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1060
@SYMTestCaseDesc	    Tests for server socket
@SYMTestPriority 	    High
@SYMTestActions  	    Create a steam socket,bind and test for a transmission process.Check for error codes 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void testServer()
	{
	int fd1,fd2,nbytes,i;
	size_t addrsize;
	int err;
	struct sockaddr_in addr1, addr2;
	char buf[80];
	char **mp = message_sequence;
	static int randbuf[10000];

	 fd1=fd2=nbytes=i=0;

	 /* Fill the buffer with a predetermined random number sequence */
	randbuf[0]=4441302;
	srand(randbuf[0]);
	for (i=1; i<10000; i++)
		randbuf[i]=rand();


	test_Next("Create server socket");
	fd1=socket(AF_INET, SOCK_STREAM, 0);
	test_ok(fd1>=0);

#if 0
	/* causes ESOCK to panic the client */
	addrsize=sizeof(addr2);
	fd2=accept(fd1,&addr2,&addrsize);	/* can't accept on an unbound socket */
	test_ok(fd2<0);
#endif

	IN_SET_LOOPBACK_ADDR(&addr1);
	addr1.sin_port=htons(port);
	err=bind(fd1, (struct sockaddr*)&addr1, sizeof(addr1));
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
	fd2=accept(fd1,(struct sockaddr*)&addr2,&addrsize);
	test_ok(fd2>=0);
	test(addr2.sin_family==AF_INET);
	test(addr2.sin_port!=htons(port));
	test(addr2.sin_port!=0);
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
	err=recv(fd2, buf, sizeof(buf) ,0);
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
	addr2.sin_port=0;
	err=recvfrom(fd2, buf, nbytes+1,0,(struct sockaddr*)&addr2,&addrsize);
/*
	memset(&buf[0],0, sizeof(buf));
	err=recvfrom(fd2, buf, nbytes+1,0,0,0);
*/
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);
	test(addr2.sin_family==AF_INET);
	test(addr2.sin_port!=htons(port));
	test(addr2.sin_port!=0);
	test(addrsize<=sizeof(addr2));

	/* sendto */
	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr1);
	err=sendto(fd2, *mp, nbytes+1,0,(struct sockaddr*)&addr1,addrsize);	/* not allowed on streams */
	test_errno(err<0,ENOSYS);
	err=send(fd2, *mp, nbytes+1,0);		/* to keep synchronisation */
	test_ok(err==nbytes+1);

	
	test_Next("Server revcfrom with null fromaddr");
		/* recvfrom again*/
	mp++;
	nbytes=strlen(*mp);
	memset(&buf[0],0xFF, sizeof(buf));
	err=recvfrom(fd2, buf, nbytes+1,0,0,0);
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);

	/* sendto again*/
	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr1);
	err=send(fd2, *mp, nbytes+1,0);		/* to keep synchronisation */
	test_ok(err==nbytes+1);

	
	
	/*
	test_Next("Server shutdown transmission");
	err=shutdown(fd2,1);
	test_ok(err==0);

	sleep(4);	// so that the client's sleep(1) finishes before we awake 

	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr1);
	err=send(fd2, *mp, nbytes+1,0);
	test_errno(err<0,EPIPE);

	nbytes=strlen(*mp);
	err=recv(fd2, buf, nbytes+1,0);
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);

	err=close(fd2);
	test_ok(err==0);

	// Large transmission 
*/
	test_Next("Server large transfer"); 

	addrsize=sizeof(addr2);
	fd2=accept(fd1,(struct sockaddr*)&addr2,&addrsize);
	test_ok(fd2>=0);
	test(addr2.sin_family==AF_INET);
	test(addr2.sin_port!=htons(port));
	test(addr2.sin_port!=0);
	test(addrsize<=sizeof(addr2));

	nbytes=sizeof(randbuf);
	err=write(fd2,(const char*)randbuf,nbytes);
	test_ok(err==nbytes);

	test_Next("Server close");

//	err=shutdown(fd2,1);	
//	test_ok(err==0);

//	sleep(4);
	
	err=close(fd2);
	test_ok(err==0);

	err=close(fd1);
	test_ok(err==0);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1061
@SYMTestCaseDesc	    Tests for client socket
@SYMTestPriority 	    High
@SYMTestActions  	    Create a steam socket,bind and test for a transmission process.Check for error codes 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void testClient()
	{
	int fd1, nbytes, nbytes2, i, status;

	size_t addrsize;
	int err;
	struct sockaddr_in addr1, addr2;
	char buf[80];
	char **mp = message_sequence;
	static int randbuf[10001];

	test_Next("Create client socket");
	fd1=socket(AF_INET, SOCK_STREAM, 0);
	test_ok(fd1>=0);

	sleep(5);	/* give server a chance to run */

	addr1=testaddr;
	addr1.sin_port=htons(port);
	addrsize=sizeof(addr1);
	err=connect(fd1,(struct sockaddr*)&addr1,addrsize);
	test_ok(err==0);

	addrsize=sizeof(addr2);
	err=getpeername(fd1,(struct sockaddr*)&addr2,&addrsize);
	test_ok(err==0);
	test(addr2.sin_family==AF_INET);
	test(addr2.sin_port==htons(port));
	test(addrsize<=sizeof(addr2));

	addrsize=sizeof(addr2);
	err=getsockname(fd1,(struct sockaddr*)&addr2,&addrsize);
	test_ok(err==0);
	test(addr2.sin_family==AF_INET);
	test(addr2.sin_port!=htons(port));
	test(addr2.sin_port!=0);
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
	err=send(fd1,*mp, nbytes+1,0);
	test_ok(err==nbytes+1);

	/* recv - get the first 2 bytes so that we know the buffer is full */
	mp++;
	nbytes=strlen(*mp);
	err=recv(fd1,buf,2,0);
	test_ok(err==2);

	/* ioctl */

	nbytes2=-1;
	err=ioctl(fd1,E32IONREAD,&nbytes2);
	test_ok(err==0);
	test(nbytes2==(nbytes+1-2));

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
	for (i=2; i<nbytes+1; i++)
		{
		err=recv(fd1,buf+i,1,0);
		test_ok(err==1);
		}
	test(strcmp(buf,*mp)==0);

	/* ioctl again - this time there is no data pending */
	nbytes2=-1;
	err=ioctl(fd1,E32IONREAD,&nbytes2);
	test_ok(err==0);
	test(nbytes2==0);

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
	err=sendto(fd1, *mp, nbytes+1,0,(struct sockaddr*)&addr1,addrsize);
	test_errno(err<0,ENOSYS);
	err=send(fd1, *mp, nbytes+1,0);		/* to keep synchronisation */
	test_ok(err==nbytes+1);

	/* recvfrom */
	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr2);
	addr2.sin_port=0;
	err=recvfrom(fd1, buf, nbytes+1,0,(struct sockaddr*)&addr2,&addrsize);
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);
	test(addr2.sin_family==AF_INET);
	test(addr2.sin_port==htons(port));
	test(addrsize<=sizeof(addr2));

	/* another sendto */
	test_Next("Client revcfrom with null fromaddr");
	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr1);
	err=sendto(fd1, *mp, nbytes+1,0,(struct sockaddr*)&addr1,addrsize);
	test_errno(err<0,ENOSYS);
	err=send(fd1, *mp, nbytes+1,0);		/* to keep synchronisation */
	test_ok(err==nbytes+1);

	/* recvfrom */
	mp++;
	nbytes=strlen(*mp);
	addrsize=sizeof(addr2);
	memset(buf,0xFF,sizeof(buf));
	err=recvfrom(fd1, buf, nbytes+1,0,0,0);	//do a recvfrom with a null address
	test_ok(err==nbytes+1);
	test(strcmp(buf,*mp)==0);



	sleep(2);
/*
	test_Next("Test half-closed connection");

	// read from half-closed socket 
	mp++;
	nbytes=strlen(*mp);
	err=read(fd1, buf, nbytes+1);
	test_errno(err<0, EPIPE);

	nbytes=strlen(*mp);
	err=write(fd1,*mp,nbytes+1);
	test_ok(err==nbytes+1);

	sleep(2);	
	
	// read from a connection closed by the other end 
	mp++;
	nbytes=strlen(*mp);
	err=read(fd1,buf,nbytes+1);
	test_errno(err<0, EPIPE);
*/
	err=close(fd1);
	test_ok(err==0);

	// Large transmission - and another connection to the server socket 

	test_Next("Test large transmission");

	fd1=socket(AF_INET, SOCK_STREAM, 0);
	test_ok(fd1>=0);

	sleep(2);	/* give server a chance to run */

	addr1=testaddr;
	addr1.sin_port=htons(port);
	addrsize=sizeof(addr1);

	//this connect fails.  Why is this?
	err=connect(fd1,(struct sockaddr*)&addr1,addrsize);
	test_ok(err==0);


	addrsize=sizeof(addr2);
	err=getpeername(fd1,(struct sockaddr*)&addr2,&addrsize);
	test_ok(err==0);
	test(addr2.sin_family==AF_INET);
	test(addr2.sin_port==htons(port));
	test(addrsize<=sizeof(addr2));

	addrsize=sizeof(addr2);
	err=getsockname(fd1,(struct sockaddr*)&addr2,&addrsize);
	test_ok(err==0);
	test(addr2.sin_family==AF_INET);
	test(addr2.sin_port!=htons(port));
	test(addr2.sin_port!=0);
	test(addrsize<=sizeof(addr2));

	nbytes=0;
	{
		int buf;
		char * p;

		do
		{
		buf=sizeof(randbuf)-nbytes;
		if (buf>4095)
				buf=4095;
		p = (char*)randbuf+nbytes;
//		err=read(fd1, p, buf); 
		err=recv(fd1, p, buf, 0); 
		if (err>0)
			nbytes+=err;
		}
	while (nbytes<sizeof(randbuf) && err>0);
	}

	printf("\nnbytes=%d, err=%d, errno=%d\n", nbytes, err, errno);

	test_ok(err==0);	/* want a graceful EOF */
	test(nbytes==sizeof(randbuf)-sizeof(int));	/* expect a smaller test data set */
	test(randbuf[0]==4441302);
	srand(randbuf[0]);
	for (i=1; i<10000; i++)
		test(randbuf[i]==rand());



	err=recv(fd1, (char*)randbuf, 1, 0); 
	test_ok(err==0);

	err=close(fd1);
	test_ok(err==0);

	}

int close_console=0;
void testConnection()
	{
	int err;
	void* server;
	char buf[100];

	sprintf(buf, "TISTREAM server %d", close_console);
	server=create_thread(testServer, buf);
	test(server!=0);
	start_thread(server);

	sleep(2);	/* give the server a chance to get started */
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

	test_Title("AF_INET Streams");

	err=CommInit(0);	/* ensure a workable comms environment */
	test(err==0);

	IN_SET_LOOPBACK_ADDR(&testaddr);

	if (argc==1)
		{
		// Run the test(s) without a CPosixServer first 
		testSimple();
		port +=2;
		testConnection();
		port +=2;
		}


	test_Next("Test gethostname");
	{
		char * hname;
		hname = (char*)malloc(200);
		if (hname)
			{
			err = gethostname(hname, 200);

			if (err)
				printf("gethostname, err=%d, errno=%d\n", err, errno);
			else
				printf("host name is %s\n", hname);
			
			free(hname);			}
		else
			printf("host name - malloc failure\n");

	}

	test_Next("Do it again using the CPosixServer (for them, not me)");
	close_console=1;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */

	
	client=create_thread(testSimple, "TISTREAM simple");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);
	port +=2;

	client=create_thread(testConnection, "TISTREAM client");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);

	test_Close();
	return 0;
	}
