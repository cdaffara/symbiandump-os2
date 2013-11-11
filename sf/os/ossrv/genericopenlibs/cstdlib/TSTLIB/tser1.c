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

test_Data;
int close_console=0;

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1099
@SYMTestCaseDesc	    Tests for serial port
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for serial port,and under posix server
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void DoTests(char*port)
	{	
	int port1, port2;
	int int1 = 0;
	int int2 = 0;
	int ret=0;
	int sigs[] = {0,0};
 	int sig = 0;

	SerialConfig config;
	SerialConfig newconfig;

	char buffer[1024];
	size_t size = 100;

	memset(&newconfig, 0, sizeof(newconfig));

	test_Next("Open port");
	
	port1 = open(port,  0);	//exclusive
	if (-1 != port1)
		{

		//this one should fail
		port2 = open(port,  0);
		test_errno(-1==port2, EACCES);

		
		test_Next("set signals");
		//play with the signals
		ret = ioctl(port1, COMMIOCTL_SETSIGNALS, &sigs[0]);
		test(-1 != ret);

		test_Next("get signals");
		ret = ioctl(port1, COMMIOCTL_GETSIGNALS, &sig);
		test(-1 != ret);

		//try changing the settings
		test_Next("port configuration");
		ret = ioctl(port1, COMMIOCTL_GETCONFIG, &config);

		config.iRate = Bps115200;
		config.iDataBits = DBits6;
		config.iStopBits=Stop2;
		config.iParity=ParityOdd;
		config.iHandshake=0;
		config.iParityError=ConfigParityErrorFail;
		config.iFifo=3;
		config.iSpecialRate=0;
		config.iTerminatorCount=0;
		config.iTerminator[0]=0;
		config.iXonChar=1;
		config.iXoffChar=2;
		config.iParityErrorChar=3;
		config.iSIREnable=SIRDisable;
		config.iSIRSettings=0;

		ret = ioctl(port1, COMMIOCTL_SETCONFIG, &config);
		ret = ioctl(port1, COMMIOCTL_GETCONFIG, &newconfig);

		test(config.iRate == newconfig.iRate);
		test(config.iDataBits==newconfig.iDataBits);
		test(config.iStopBits==newconfig.iStopBits);
		test(config.iParity==newconfig.iParity);
		test(config.iHandshake==newconfig.iHandshake);
		test(config.iParityError==newconfig.iParityError);
		test(config.iFifo==newconfig.iFifo);
		test(config.iSpecialRate==newconfig.iSpecialRate);
		test(config.iTerminatorCount==newconfig.iTerminatorCount);
		test(config.iTerminator[0]==newconfig.iTerminator[0]);
		test(config.iXonChar==newconfig.iXonChar);
		test(config.iXoffChar==newconfig.iXoffChar);
		test(config.iParityErrorChar==newconfig.iParityErrorChar);
		test(config.iSIREnable==newconfig.iSIREnable);
		test(config.iSIRSettings==newconfig.iSIRSettings);

		test_Next("Buffer lengths");
		int1 = 0;
		ret = ioctl(port1, COMMIOCTL_GETBUFFERLENGTH, &int1);
		test(ret != -1);
		test(int1 != 0);

		int1 = 2048;
		ret = ioctl(port1, COMMIOCTL_SETBUFFERLENGTH, &int1);
		test(ret != -1);

		int2 = 0;
		ret = ioctl(port1, COMMIOCTL_GETBUFFERLENGTH, &int2);
		test(ret != -1);
		test(int1 == int2);


		int1 = 100000000;
		ret = ioctl(port1, COMMIOCTL_SETBUFFERLENGTH, &int1);
		test(ret != -1);
		
		ret = ioctl(port1, COMMIOCTL_GETBUFFERLENGTH, &int1);
		test(ret != -1);
		test(int1 == int2);	//shoudn't have changed, val too big.


		test_Next("break");
		sig = 5000;
		ret = ioctl(port1, COMMIOCTL_BREAK, &sig);
		//break isn't supported
		test_errno(-1==ret, ENOSYS);


		test_Next("read timeout");
		int1 = 2000;
		ret = ioctl(port1, COMMIOCTL_SETREADTIMEOUT, &int1);
		test (ret != -1);

		int2 = 0;
		ret = ioctl(port1, COMMIOCTL_GETREADTIMEOUT, &int2);
		test (ret != -1);
		test(int2 == int1);

				
		test_Next("read threshold");
		int1 = 2000;
		ret = ioctl(port1, COMMIOCTL_SETREADTHRESHOLD, &int1);
		test (ret != -1);

		int2 = 0;
		ret = ioctl(port1, COMMIOCTL_GETREADTHRESHOLD, &int2);
		test(ret != -1);
		test(int1 == int2);



		//signals
		test_Next("test notifications supported");
		ret = ioctl(port1, COMMIOCTL_NOTIFYSUPPORTED, &int1);
		test((int1 & (KNotifyFramingError|KNotifyOverrunError|KNotifyParityError)) == (KNotifyFramingError|KNotifyOverrunError|KNotifyParityError));
		printf("Notifications supported are %08x\n",int1);

		test_Next("Test read timeout with a threshold");
		int2 = 3000;
		ret = ioctl(port1, COMMIOCTL_SETREADTIMEOUT, &int2);
		test (ret != -1);
		
		int1 = 1;
		ret = ioctl(port1, COMMIOCTL_SETREADTHRESHOLD, &int1);
		test (ret != -1);

		ret = read(port1, buffer, size);
		test_errno(-1==ret, ETIMEDOUT);

		//no threshold, but with a 1 second timeout
		test_Next("no threshold with timeout");
		int2 = 5000;
		ret = ioctl(port1, COMMIOCTL_SETREADTIMEOUT, &int2);
		test (ret != -1);

		int1 = -1;
		ret = ioctl(port1, COMMIOCTL_SETREADTHRESHOLD, &int1);
		test (ret != -1);

		ret = read(port1, buffer, size);
		test_errno(-1==ret, ETIMEDOUT);


		close(port1);
		}
	else
		printf("failed to open port %s with errno %d\n", port,errno);
	}



void DoTestsSerial(void)
	{
	DoTests("COM1:");

	if (close_console)
		{
		test_Close();
		close(0);
		close(1);
		close(2);
		}
	}



int main(void)
	{
	void* client;
	int err;
	
	test_Title("Serial port test1");
	test_Next("Do tests with local server for COM1:");
	DoTests("COM1:");
	test_Next("Do tests with local server for COM2:");
	DoTests("COM2:");
	test_Next("Do tests with local server for COM3:");
	DoTests("COM3:");
	test_Next("Do tests with local server for COM4:");
	DoTests("COM4:");


	start_posix_server();	/* calls SpawnPosixServer from C++ code */

	close_console=1;
	test_Next("Do tests with posix server for COM1:");
	client=create_thread(DoTestsSerial, "serial tests");
	test(client);

	start_thread(client);
	err=wait_for_thread(client);
	test(0 == err);


	test_Close();
	return 0;
	}

