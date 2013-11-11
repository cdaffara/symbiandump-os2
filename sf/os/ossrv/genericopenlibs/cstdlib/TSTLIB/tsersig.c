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
*
*/



#include <stdlib.h>	/* definition of exit() */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>
#include "CTEST.H"


int GlobalPort;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1109
@SYMTestCaseDesc	    Tests for serial port
@SYMTestPriority 	    High
@SYMTestActions  	    Open and configures the serial ports
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void OpenAndConfigure(void)
	{
	SerialConfig sc;

	GlobalPort = open("COM1:",0);
	(void)ioctl(GlobalPort, COMMIOCTL_GETCONFIG, &sc);
	sc.iRate = Bps57600;
	sc.iParity = ParityNone;
	(void)ioctl(GlobalPort, COMMIOCTL_SETCONFIG, &sc);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1110
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
	int signals = 0;

	
	while (ret != -1)
		{
		val[0] = 0;
		val[1] = -1;
		ret = ioctl(GlobalPort, COMMIOCTL_NOTIFYSUPPORTED, &signals);
		val[0] = KNotifyDataAvailable;
		ret = ioctl(GlobalPort, COMMIOCTL_NOTIFY, &val[0]);
		if ((ret != -1) || (val[0] != KNotifyDataAvailable))
			printf("NotifyThread returned %d, val[0] %d when checking data available\n", ret, val[0]);


		val[0] = signals & ~KNotifyDataAvailable; //no data avail as it returns an error
		ret = ioctl(GlobalPort, COMMIOCTL_NOTIFY, &val[0]);
		printf("NotifyThread ret = %d, val[0] is %d, val[1] is %d, errno is %d\n", ret, val[0], val[1], errno);

		}
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1111
@SYMTestCaseDesc	    Tests for serial port
@SYMTestPriority 	    High
@SYMTestActions  	    Keep reading infinitely from the port
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/					
void readloop(void)
	{
	char buf[100];
	int readcount = 10;

	while (readcount-- > 0)
		{
		read(GlobalPort, buf, 100);
		printf("ReadThread got %x\n", buf[0]);
		}
	}

int main(void)
	{
	void* t1;
	void*t2;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */

	OpenAndConfigure();


	t1 = create_thread(WaitForNotifyThread, "notify");
	t2 = create_thread(readloop, "readloop");

	start_thread(t1);
	start_thread(t2);

	(void)wait_for_thread(t2);
	close(GlobalPort);		//closing the port will force a notify
	(void)wait_for_thread(t1);
	
	return 0;
	}

