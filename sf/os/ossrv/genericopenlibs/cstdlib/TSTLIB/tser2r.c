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



#define SIZE 10000



void Writer(void)
	{
	int Port;
	SerialConfig sc;
	char *buffer;

	Port = open("COM1:",0);
	(void)ioctl(Port, COMMIOCTL_GETCONFIG, &sc);
	sc.iRate = Bps115200;
	sc.iParity = ParityNone;
	(void)ioctl(Port, COMMIOCTL_SETCONFIG, &sc);
	
	buffer = (char*)malloc(SIZE);
	memset(buffer, '*', SIZE);

	write(Port, buffer, SIZE);
	
	close(Port);

	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1101
@SYMTestCaseDesc	    Tests for reading from the serial port
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for reading into a buffer of size 1000 bytes from the serial port
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void Reader(void)
	{
	int res;
	int Port;
	SerialConfig sc;
	char *buffer, *p;
	int x;


	Port = open("COM2:",0);
	res = ioctl(Port, COMMIOCTL_GETCONFIG, &sc);
	sc.iRate = Bps115200;
	sc.iParity = ParityNone;
	res = ioctl(Port, COMMIOCTL_SETCONFIG, &sc);
	
	buffer = (char*)malloc(SIZE);

	res = 0;
	p = buffer;
	

	x = 5000;
	res = ioctl(Port, COMMIOCTL_SETREADTIMEOUT, &x);

	for(x = 0;;x++)
		{
		res = read(Port, p, 100);
		if (res > 0)
			{
			printf("read block %d int addr %x, bytes %d\n",x,p,res);
			p += res;
			}

		else
			break;
		}
		
		
	x = 100;
	res = ioctl(Port, COMMIOCTL_SETREADTHRESHOLD, &x);
	p = buffer;

	printf("And again with a threshold of 100\n");
	
	for(x = 0;;x++)
		{
		res = read(Port, p, 100);
		if (res > 0)
			{
			printf("read block %d int addr %x, bytes %d\n",x,p,res);
			p += res;
			}

		else
			break;
		}
		
	
	close(Port);

	}

int main(void)
	{
	Reader();
	return 0;
	}

