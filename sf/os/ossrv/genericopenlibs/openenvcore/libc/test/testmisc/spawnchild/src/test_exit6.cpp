// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : test_exit4.cpp
// 
//

// INCLUDE FILES
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/fcntl.h>

void* threadFn(void *)
{
	char ch;
	int fd = 0;
	fd = open("COM1:",O_WRONLY);
	if(fd == -1)
	{
		exit(-1);
		return 0;
	}
	read(fd,&ch,sizeof(ch));
	close(fd);
	fd = 0;
	return 0;
}

int main()
	{
	pthread_t hdl;
	if(pthread_create(&hdl, 0, threadFn, 0) != 0)
	     return -1;
	sleep(2);
	exit(0);
    return 0;
	}
