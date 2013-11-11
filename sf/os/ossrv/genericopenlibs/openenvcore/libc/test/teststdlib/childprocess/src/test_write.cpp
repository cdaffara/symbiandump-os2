/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

void PrintHello();
int main()
	{
	PrintHello();
	return 0;
	}

void PrintHello()
	{
	int fd = -1;
	char *filename = "C:\\lseek_sample.txt";
	char *buf = "Hello";
	fd = open(filename, O_RDWR | O_APPEND  );
	write(fd, buf, strlen(buf));
	close(fd);
	}
