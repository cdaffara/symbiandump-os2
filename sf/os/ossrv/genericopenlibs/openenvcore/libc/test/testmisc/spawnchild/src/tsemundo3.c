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
* Name        : tsemundo3.cpp
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

// semoptest18

int main()
	{
	int semkey,get=0,msq_id,err;

	union semun tmpo;
	unsigned short tmpi[2];
	struct sembuf lSemBuf[3];
	char *path;
	
	#if defined (__EPOC32__)
		path = "z:\\tstdapis\\tmisc.ini";
	#else
		path = "c:\\tstdapis\\tmisc.ini";
	#endif
	
	if ((semkey = ftok(path, 'r')) == (key_t) -1) 
		{
		printf("fail1");
		}
    
    if ((msq_id = semget(semkey,2, IPC_R|IPC_CREAT |0666)) == -1) 
		{
		printf("fail2");
		}
    

    
		lSemBuf[0].sem_num =0;
        lSemBuf[0].sem_op = 1;
        lSemBuf[0].sem_flg = 0;

		lSemBuf[1].sem_num =1;
        lSemBuf[1].sem_op = -3;
        lSemBuf[1].sem_flg = SEM_UNDO;
        
	err = semop(msq_id, &lSemBuf[0], 2);

    if(err == -1)
    	{
		printf("fail4");
		}
    tmpo.array = &tmpi[0];
    get = semctl(msq_id,1,GETALL,tmpo);
    if(get == -1)
    	{
    	printf("fail5");
    	}
    if( tmpi[0] != 1 && tmpi[1] != 1)
    	{
		printf("fail6");
		}
    
    return 0;
	}
