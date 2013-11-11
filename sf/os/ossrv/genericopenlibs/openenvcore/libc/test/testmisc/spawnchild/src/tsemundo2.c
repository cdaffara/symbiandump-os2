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
* Name        : tsemundo2.cpp
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

// semoptest17

int main()
	{
	int semkey,get,msq_id,err;

	union semun arg,tmpo,arg1;
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
    
    arg.val = 2;
    
		lSemBuf[0].sem_num =0;
        lSemBuf[0].sem_op = 2;
        lSemBuf[0].sem_flg = SEM_UNDO;

		lSemBuf[1].sem_num =1;
        lSemBuf[1].sem_op = 3;
        lSemBuf[1].sem_flg = SEM_UNDO;
        
	err = semop(msq_id, &lSemBuf[0], 2);

    if(err == -1)
    	{
		printf("fail4");
		}
    tmpo.array = &tmpi[0];
    get = semctl(msq_id,1,GETALL,tmpo);
    if( tmpi[0] != 7 && tmpi[1] != 13)
    	{
		printf("fail5");
		}
    
    arg.val = 20;
    arg1.val = 30;
    
    get = semctl(msq_id, 0, SETVAL,arg);
    if (get == -1) 
    	{
		printf("fail6");
		}
    
	get = semctl(msq_id, 1, SETVAL,arg1);
    if (get == -1) 
    	{
		printf("fail7");
		}
    
    return 0;
	}
