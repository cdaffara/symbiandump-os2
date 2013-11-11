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
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
// this child process would try to write to a already locked over region
// Now the expected return on success is 0 and on failure is -2.
int main()
	{
	int fd = open("c:\\openexclusivelockfile.txt", O_RDWR, 0600);
	if (fd <= 0)
		{
		return -2;
		}
	int writeret = write(fd, "whale in the sea", 18);
	if (writeret < 0)
		{
		close(fd);
		return 0;
		}
	close(fd);
	return -2;
	}

