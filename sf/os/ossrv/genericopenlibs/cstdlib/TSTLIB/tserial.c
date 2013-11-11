/*
* Copyright (c) 1997-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <stdlib.h>	/* definition of exit() */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>
#include "ctest.h"


int GlobalPort;

void DoSharedStuff(void)
	{
		int p1, p2, ret;
		SerialConfig config;
		p1 = open("COM1:",  1);
		p2 = open("COM1:",  1);

		//try changing the settings
		ret = ioctl(p1, COMMIOCTL_GETCONFIG, &config);

		config.iRate = Bps115200;
		ret = ioctl(p2, COMMIOCTL_SETCONFIG, &config);

		config.iRate = Bps1200;
		ret = ioctl(p1, COMMIOCTL_GETCONFIG, &config);

		close(p2);
		config.iRate = Bps1200;
		ret = ioctl(p1, COMMIOCTL_GETCONFIG, &config);
		close(p1);

	}
void OpenAndConfigure(void)
	{
	int res;
	SerialConfig sc;

	GlobalPort = open("COM1:",0);
	res = ioctl(GlobalPort, COMMIOCTL_GETCONFIG, &sc);
	sc.iRate = Bps115200;
	sc.iParity = ParityNone;
	res = ioctl(GlobalPort, COMMIOCTL_SETCONFIG, &sc);
	}


void WaitAndRead(void)
	{
	int port = GlobalPort;
	int ret;
	char buffer[100];

	printf("Wait and read\n");
	ret = read(port, buffer, 10);
	printf("got it %x\n",buffer[0]);
	}


void WriteABit(int x)
	{
	int port = GlobalPort;
	char bob[100];
	sprintf(bob, "Test Write >>%d<<    >>%x<<\n", x, x);
	write(port, bob, strlen(bob));
	}

void writelots(void)
	{
	int n;
	for (n=0;n<10000;n++)
		WriteABit(n);
	}


void DoTests(void)
	{	
	int port1, port2;
	int timeout = 0;
	int threshold = 0;
	int ret=0;

	port1 = open("COM1:",  0);

	//this one should fail
	port2 = open("COM1:",  0);

	if (-1 != port1)
		{
		//play with the signals
		int sigs[] = {0,0};
 		int sig = 0;
		ret = ioctl(port1, COMMIOCTL_SETSIGNALS, &sigs[0]);
		ret = ioctl(port1, COMMIOCTL_GETSIGNALS, &sig);
		sig = 5000;
		ret = ioctl(port1, COMMIOCTL_BREAK, &sig);
		close(port1);
		}

	port1 = open("COM1:",  0);
	port2 = open("COM2:",0);


	if (-1 != port2)
		{
		int len;
		SerialConfig config;
		char buffer[1024];
		size_t size = 100;


		timeout = 2000;
		ret = ioctl(port2, COMMIOCTL_SETREADTIMEOUT, &timeout);
		timeout = 0;
		ret = ioctl(port2, COMMIOCTL_GETREADTIMEOUT, &timeout);

				
		threshold = 2000;
		ret = ioctl(port2, COMMIOCTL_SETREADTHRESHOLD, &threshold);
		threshold = 0;
		ret = ioctl(port2, COMMIOCTL_GETREADTHRESHOLD, &threshold);


		//buffer lengths
		len = 0;
		ret = ioctl(port1, COMMIOCTL_GETBUFFERLENGTH, &len);

		len = 2048;
		ret = ioctl(port1, COMMIOCTL_SETBUFFERLENGTH, &len);

		len = 0;
		ret = ioctl(port1, COMMIOCTL_GETBUFFERLENGTH, &len);

		len = 10000000;
		ret = ioctl(port1, COMMIOCTL_SETBUFFERLENGTH, &len);
		
		len = 0;
		ret = ioctl(port1, COMMIOCTL_GETBUFFERLENGTH, &len);


		//try changing the settings
		ret = ioctl(port1, COMMIOCTL_GETCONFIG, &config);

		config.iRate = Bps115200;
		ret = ioctl(port1, COMMIOCTL_SETCONFIG, &config);

		config.iRate = Bps1200;
		ret = ioctl(port1, COMMIOCTL_GETCONFIG, &config);


		timeout = threshold = -1;
		ret = ioctl(port1, COMMIOCTL_SETREADTHRESHOLD, &threshold);
		ret = ioctl(port1, COMMIOCTL_SETREADTIMEOUT, &timeout);
		
		printf("Talk to me, ESC to exit\n");
		do
			{
			ret = read(port1, buffer, size);
			if (ret != -1)
				printf("look what I got >>> %c\t%x\n",buffer[0],buffer[0]);
			else
				printf("error, errno is %x \n",errno);
			}
		while (buffer[0] != 27);

		printf("Talk to me in blocks of 5, EXIT! to exit\n");
		threshold = 5;
		ret = ioctl(port1, COMMIOCTL_SETREADTHRESHOLD, &threshold);
		do
			{
			ret = read(port1, buffer, size);
			if (ret >=0)
				{
				buffer[ret] = 0;
				printf("look what I got >>> %s\n",buffer);
				}
			else
				printf("error\n");
			}
		while (strcmp(buffer, "EXIT!"));
		printf("Wait 10 seconds then give up\n");
		timeout = 10000;
		ret = ioctl(port1, COMMIOCTL_SETREADTIMEOUT, &timeout);
		threshold = 1;
		ret = ioctl(port1, COMMIOCTL_SETREADTHRESHOLD, &threshold);
		ret = read(port1, buffer, size);
		if (ret > 0)
			printf("look what I got >>> %c\t%x\n",buffer[0],buffer[0]);
		else
			printf("timeout\n");

		
		//no threshold, but with a 5 second timeout
		printf("Now for the hard one!\n");
		timeout = 5000;
		ret = ioctl(port1, COMMIOCTL_SETREADTIMEOUT, &timeout);
		threshold = -1;
		ret = ioctl(port1, COMMIOCTL_SETREADTHRESHOLD, &threshold);
		ret = read(port1, buffer, size);
		if (ret<0)
			printf("got an error, errno is 56 if it a timeout. erno = %d\n", errno);
		else
			printf("look what I got >>> %c\t%x\n",buffer[0],buffer[0]);

		printf("Going to write some stuff now\n");
		ret = write(port1, "Lardy dardy dardy cheese\n\n",26);
		printf("Written some stuff now and got a ret of %d\n", ret);


		close(port2);
		}
	
	if (-1 != port1)
		close(port1);

	printf("Press key to exit");
	getchar();
	}



void pong(void)
	{
		char buf[50];
		int port = open("IRCOM:0", 0);
		int x;
		strcpy(buf,"test shot #");
		for (x=0;x<10;x++);
			{
			write(port, buf, strlen(buf));
			read(port,buf, 50);
			strcat(buf,"#");
			}
		close(port);
	}

void ping(void)
	{
		char buf[50];
		int x;
		int port = open("IRCOM:0", 0);
		for (x=0;x<10;x++);
			{
			read(port,buf, 50);
			strcat(buf,"*");
			write(port, buf, strlen(buf));
			}
		close(port);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1112
@SYMTestCaseDesc	    Tests for serial port
@SYMTestPriority 	    High
@SYMTestActions  	    Notifies a thread for events on port 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void WaitForNotifyThread(void)
	{
	int val[2];
	int ret = 0;
	
	while (!ret)
		{
		val[0] = KNotifyFramingError|KNotifyOverrunError|KNotifyParityError;
		val[1] = -1;
		ret = ioctl(GlobalPort, COMMIOCTL_NOTIFY, &val[0]);
		printf("NotifyThread ret = %d, val[0] is %d, val[1] is %d\n", ret, val[0], val[1]);
		}
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1113
@SYMTestCaseDesc	    Tests for serial port
@SYMTestPriority 	    High
@SYMTestActions  	    Keep reading infinitely from the port
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void readloop(void)
	{
	char buf[100];
	int ret = 0;
	while (!ret)
		{
		read(GlobalPort, buf, 100);
		printf("ReadThread got %x\n", buf[0]);
		}
	}

int main(void)
	{
	void* client;
	void* t1;
	void*t2;
	void*t3;
	int err;
	

//	DoTests();

//	printf("Do it again using the CPosixServer\n");

	start_posix_server();	/* calls SpawnPosixServer from C++ code */

//	client=create_thread(DoTests, "serial tests");
//	start_thread(client);
//	err=wait_for_thread(client);

	
//	t3 = create_thread(OpenAndConfigure,"cfg");
//	start_thread(t3);
//	err=wait_for_thread(t3);

//	t1 = create_thread(WaitAndRead, "open and wait");
//	t2 = create_thread(writelots, "writelots");
//	start_thread(t1);
//	start_thread(t2);
//	err=wait_for_thread(t1);
//	err=wait_for_thread(t2);
//	close(GlobalPort);

	
	OpenAndConfigure();
	t1 = create_thread(WaitForNotifyThread, "notify");
	t2 = create_thread(readloop, "readloop");

	start_thread(t1);
	start_thread(t2);

	err=wait_for_thread(t2);
	err=wait_for_thread(t1);
	
	close(GlobalPort);
	return 0;
	}

