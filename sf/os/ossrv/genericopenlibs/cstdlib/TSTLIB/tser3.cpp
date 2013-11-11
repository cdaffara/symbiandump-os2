// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <estlib.h>
#include <stdlib.h>	/* definition of exit() */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>
#include "CTEST.H"

#ifdef __cplusplus
extern "C" {
#endif


#define SIZE 10000

int ReaderPort;
int WriterPort;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1102
@SYMTestCaseDesc	    Tests for serial port
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by writing a string buffer to the port
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void Writer()
	{
	char *buffer;
	
	buffer = (char*)malloc(SIZE);
	memset(buffer, '*', SIZE);

	printf("\nbig write\n");
	(void)write(WriterPort, buffer, SIZE);
	(void)errno;
	printf("\n>>Writer<<small read\n");
	read(WriterPort,buffer,1);
	printf("\nbig write\n");
	write(WriterPort, buffer, SIZE);
	printf("\n>>Writer<<small read\n");
	read(WriterPort,buffer,1);

	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1103
@SYMTestCaseDesc	    Tests for serial port
@SYMTestPriority 	    High
@SYMTestActions  	    Notifies on a read operation 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void Watcher()
	{
	int res=0;
	int notify[2] = {0,0};
	int supported = 0;


	res = ioctl(ReaderPort, COMMIOCTL_NOTIFYSUPPORTED, &supported);
	printf("\nsupported notifies are %08x\n",supported);
	while (-1 != res)
		{
		notify[0] = supported;
		printf("asking for %08x\n",notify[0]);
		res = ioctl(ReaderPort, COMMIOCTL_NOTIFY, &notify[0]);
		printf("\n**Notify** res = %d params %d, %d, errno %d\n",res, notify[0], notify[1],errno);
		}

	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1104
@SYMTestCaseDesc	    Tests for serial port
@SYMTestPriority 	    High
@SYMTestActions  	    Reads to buffer from serial port,test for failure conditions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void Reader()
	{
	int res;
	char *buffer, *p;
	int x;


	
	buffer = (char*)malloc(SIZE*2);

	res = 0;
	p = buffer;
	

	x = 5000;
	res = ioctl(ReaderPort, COMMIOCTL_SETREADTIMEOUT, &x);

	for(x = 0;;x++)
		{
		res = read(ReaderPort, p, 100);
		if (res > 0)
			{
			putchar('.');
			p += res;
			}
		else
			{
			printf("\nread failed with errno %d\n",errno);
			break;
			}
		}
		
	printf("\n>>Reader<<small write\n");
	res = write(ReaderPort, buffer ,1);

	printf("\n>>Reader<<set threshold\n");
	x = 100;
	res = ioctl(ReaderPort, COMMIOCTL_SETREADTHRESHOLD, &x);
	p = buffer;

	printf("\nAnd again with a threshold of 100\n");
	
	for(x = 0;;x++)
		{
		res = read(ReaderPort, p, 100);
		if (res > 0)
			{
			putchar('.');
			p += res;
			}
		else
			{
			printf("\nread failed with errno %d\n",errno);
			break;
			}
		}
		
	printf("\n>>Reader<<Small write\n");
	write(ReaderPort, buffer ,1);
	

	}

#ifdef __cplusplus
}
#endif

int main(int, char**)
	{
	void* t1;
	void* t2;
	void* t3;
	
	SerialConfig sc;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */

	WriterPort = open("COM1:",0);
	if (-1 == WriterPort)
		exit(-1);
	(void)ioctl(WriterPort, COMMIOCTL_GETCONFIG, &sc);
	sc.iHandshake = 0;
	sc.iRate = Bps115200;
	sc.iParity = ParityNone;
	(void)ioctl(WriterPort, COMMIOCTL_SETCONFIG, &sc);
	
	ReaderPort = open("COM2:",0);
	if (-1 == ReaderPort)
		exit(-2);
	(void)ioctl(ReaderPort, COMMIOCTL_GETCONFIG, &sc);
	sc.iHandshake = 0;
	sc.iRate = Bps115200;
	sc.iParity = ParityNone;
	(void)ioctl(ReaderPort, COMMIOCTL_SETCONFIG, &sc);

	t3 = create_thread(Watcher, "watcher1");
	t1 = create_thread(Reader, "reader");
	t2 = create_thread(Writer, "writer");

	start_thread(t3);
	start_thread(t2);
	start_thread(t1);

	(void)wait_for_thread(t1);
	(void)wait_for_thread(t2);

	close(WriterPort);
	close(ReaderPort);
	(void)wait_for_thread(t3);
	printf("\nPress a key\n");
	getchar();
	return 0;
	}


