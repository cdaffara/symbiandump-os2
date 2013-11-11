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
* Name        : openshmem.c
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
	int fd;
	fd = shm_open("shmemtest",O_RDWR,0777);
	if (fd > 0)
		{
		close(fd);
		return 1;
		}
	else
		{
		return 0;
		}	
	}

//  End of File
