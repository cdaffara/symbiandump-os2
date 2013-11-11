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


#include <fcntl.h> 
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sem.h>
#include <time.h>
#include <string.h> 

int GivenUpControlOnSemaphore(int semid)
	{
	struct sembuf sops[2];
	//struct sembuf *sops = (struct sembuf *) malloc(2*sizeof(struct sembuf)); 	
	int nsops = 1; /* number of operations to do */
	int result;
	       
	/* wait for semaphore to reach zero */
	sops[0].sem_num = 0;
	sops[0].sem_op = -1; /* Give UP Control of track */
	sops[0].sem_flg = SEM_UNDO | IPC_NOWAIT; /* take off semaphore, asynchronous  */
	       
	result = semop(semid, sops, nsops);
	return result;
	}

int TakeControlOnSemaphore(int semid)
	{
	struct sembuf sops[2];
	int nsops; /* number of operations to do */
	int result;


	/* wait for semaphore to reach zero */	   
	sops[0].sem_num = 0; /* We only use one track */
	sops[0].sem_op = 0; /* wait for semaphore flag to become zero */
	sops[0].sem_flg = SEM_UNDO; /* take off semaphore asynchronous  */
	   
	   
	sops[1].sem_num = 0;
	sops[1].sem_op = 1; /* increment semaphore -- take control of track */
	sops[1].sem_flg = SEM_UNDO | IPC_NOWAIT; /* take off semaphore */
	
	nsops = 2;

	result = semop(semid, sops, nsops);
	return result;
	}


/**
 * four steps in main:
 * 1. get semaphore, open designated file with append mode, release semaphore, sleep
 * 2&3. get semaphore, append to the file with value in command line parameter, release semaphore
 * 4. get semaphore, close the file, release semaphore
 * @param  argv[1] : int, semaphore id
 * @param  argv[2] : char*, file name string ptr
 * @param  argv[3] : int, length of append content
 * @param  argv[4] : unsigned char, byte value of content appending to file
 * @return 0: always zero
*/
int main (int argc, char *argv[ ])
	{
	int semid; /* semid of semaphore set */
	int writelength = 0;
	unsigned char bytevalue = 0;
	char* filename = NULL;
	int steps = 0;
	int testFD = -1;
	int writeresult;
	time_t rawtime;
	
	if(argc < 4)
		return 1;

	semid = atoi(argv[1]);
	filename = argv[2];
	writelength = atoi(argv[3]);
	bytevalue = (unsigned char) atoi(argv[4]);
	
	while(steps < 4)
		{
		if(TakeControlOnSemaphore(semid) == -1)
			{
			break;
			}
		time(&rawtime);
		printf("get semaphoer at time : %s", ctime (&rawtime));
		if(steps == 0)
			{
			// open file with append mode
			testFD = open(filename, O_APPEND | O_RDWR  | O_SYNC);
			printf("Open file %s, %d.\n", filename, testFD);
			}
		else if(steps == 1 || steps == 2)
			{
			// append to file
			char* membuf = (char*) malloc(sizeof(char) * writelength);
			if(NULL != membuf)
				{
				memset(membuf, bytevalue, writelength);
				writeresult = write(testFD, membuf, writelength);
				free(membuf);
				printf("append to file %s with bytevalue %x, length is %d, result is %d.\n", filename, bytevalue, writelength, writeresult);
				}
			}
		else if(steps == 3)
			{
			// close file
			int ret = close(testFD);
			if (ret != 0)
				{
				break;
				}
			printf("close file with name %s.", filename);
			}
		if(GivenUpControlOnSemaphore(semid) == -1)
			{
			break;
			}
		printf("Give UP semaphore. \n\n");
		sleep(1); 
		steps++;
		}
		
	return 0;
	}

