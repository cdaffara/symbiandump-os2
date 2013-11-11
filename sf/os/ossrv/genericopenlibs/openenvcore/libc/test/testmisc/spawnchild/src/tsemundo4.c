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
* Name        : tsemundo4.cpp
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

// semoptest19

// *** Thread1 
void* StartFn1(void* arg)
	{
	int semnops=3,get,x;    
    int msgq_id = (int)arg;
    struct sembuf lSemBuf[3];
    union semun arg1,arg2,arg3;
    
	// wait for -7,-12,-17
	
	lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op  = -7;
    lSemBuf[0].sem_flg = 0;
	
	lSemBuf[1].sem_num = 1;
	lSemBuf[1].sem_op  = -12;
	lSemBuf[1].sem_flg = 0;
        
    lSemBuf[2].sem_num = 2;
	lSemBuf[2].sem_op  = -17;
	lSemBuf[2].sem_flg = 0;
	
	x = semop(msgq_id,&lSemBuf[0],semnops);
	if(x==-1)
		{
		exit(0);
		}
	
	// wait
	sleep(5);
	
	// setval 8,13,18
	arg1.val = 8;
	get = semctl(msgq_id, 0, SETVAL,arg1);
	
	arg2.val = 13;
	get = semctl(msgq_id, 1, SETVAL,arg2);
	
	arg3.val = 18;
	get = semctl(msgq_id, 2, SETVAL,arg3);
	
	if(get == -1)
		{
		return NULL;
		}
	// exit
    return NULL;
	}
	

// *** Thread2 ***

void* StartFn2(void* arg)
	{
	int semnops=3,get,x;    
    int msgq_id = (int)arg;
    struct sembuf lSemBuf[3];
    union semun arg1,arg2,arg3;
    
	// wait for wait for 8,13,18
	
	lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op  = -8;
    lSemBuf[0].sem_flg = 0;
	
	lSemBuf[1].sem_num = 1;
	lSemBuf[1].sem_op  = -13;
	lSemBuf[1].sem_flg = 0;
        
    lSemBuf[2].sem_num = 2;
	lSemBuf[2].sem_op  = -18;
	lSemBuf[2].sem_flg = 0;
	
	x = semop(msgq_id,&lSemBuf[0],semnops);
	if(x==-1)
		{
		exit(0);
		}
	
	// wait
	sleep(5);
	
	// setval 9,14,19
	arg1.val = 9;
	get = semctl(msgq_id, 0, SETVAL,arg1);
	
	arg2.val = 14;
	get = semctl(msgq_id, 1, SETVAL,arg2);
	
	arg3.val = 19;
	get = semctl(msgq_id, 2, SETVAL,arg3);
	
	if(get == -1)
		{
		return NULL;
		}
	// exit
    return NULL;
	}

int main()
	{
	int semkey,get,msq_id,x;

	union semun arg1,arg2,arg3;
	struct sembuf lSemBuf[3];
	pthread_t testThread1;
	pthread_t testThread2;
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
    
    // wait for 9,14,19
    lSemBuf[0].sem_num = 0;
    lSemBuf[0].sem_op  = -9;
    lSemBuf[0].sem_flg = 0;
	
	lSemBuf[1].sem_num = 1;
	lSemBuf[1].sem_op  = -14;
	lSemBuf[1].sem_flg = 0;
        
    lSemBuf[2].sem_num = 2;
	lSemBuf[2].sem_op  = -19;
	lSemBuf[2].sem_flg = 0;
    
    // create the threads
    pthread_create(&testThread1,NULL,StartFn1,(TAny*)msq_id);
	pthread_create(&testThread2,NULL,StartFn2,(TAny*)msq_id);
	
	x = semop(msq_id,&lSemBuf[0],3);
	if(x==-1)
		{
		exit(0);
		}
	
    // setval to 10,15,20
	arg1.val = 10;
	get = semctl(msq_id, 0, SETVAL,arg1);
	
	arg2.val = 15;
	get = semctl(msq_id, 1, SETVAL,arg2);
	
	arg3.val = 20;
	get = semctl(msq_id, 2, SETVAL,arg3);
	
	if(get == -1)
		{
		return -1;
		}
    
	return 0;
	}
