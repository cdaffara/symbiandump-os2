/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tsemundo5.cpp
* 
*
*/



#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <stdapis/netinet/in.h>
#include <stdapis/net/if.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/cdefs.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>
#include <wchar.h>
#include <spawn.h>

#include <sys/sem.h>
#include <errno.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// semoptest_cov
#define MESSAGE_Q_KEY 1000
int main()
	{
	int msq_id,ret = 0;
	
	if ((msq_id = semget(MESSAGE_Q_KEY,1, IPC_CREAT | IPC_EXCL | 0666)) == -1) 
		{
		ret = -2;
    	}
    if (semctl(msq_id, 0, IPC_RMID) == -1) 
    	{
		ret = -2;
    	}

	return ret;
	}

