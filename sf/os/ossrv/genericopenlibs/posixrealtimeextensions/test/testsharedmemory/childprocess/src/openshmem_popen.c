/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : openshmem_popen.c
* To validate usuage of shared memory in the different process
* 
*
*/



// INCLUDE FILES


#include <sys/mman.h>
#include <sys/fcntl.h>
#include <unistd.h>
int main()
	{	
	int ret = 0;
	char buf[7];
	int fd = shm_open("/shmemtest_popen", O_RDWR , 0666);
	if (fd <= 0)
		{
		return 0;			
		}
	ret = write(fd, "symbian", 7);
	if (ret == -1)
		{
		close(fd);
		return 0;
		}
	ret = lseek(fd, -6, SEEK_END);
	if (ret == -1)
		{
		close(fd);
		return 0;
		}
	ret = read(fd, (void*)buf, 6);
	if (ret == -1)
		{
		close(fd);
		return 0;
		}
	close(fd);
	return 1;
	}

//  End of File
