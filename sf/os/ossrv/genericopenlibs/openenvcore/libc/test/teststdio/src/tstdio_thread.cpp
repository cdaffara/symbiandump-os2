/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "tstdio.h"

typedef struct node
	{
	int iCnt;
	sem_t * iSemaphore;
	char msg[100];
	FILE *fp;
	} Node;

/***************** these functions are for fseek,ftell test **********************/

void * tell_thread(void * param)
	{
	int res=1;
	int retval;
	FILE * fp;
	Node * nd=(Node *) param;
	int chk=nd->iCnt;
	int ret;

	fp=fopen("c:\\input.txt","r");
	if(fp!=NULL)
		{
		retval=sem_wait(nd->iSemaphore);
		if(retval!=0) 
			{
			res=1;
			}
		else
			{
			ret=ftell(fp);
			if(ret==chk)
				{
				res=0;
				}
			else
				{
				res=1;
				}
			}
		fclose(fp);
		}
	return (void *)res;
	}


void * seek_thread(void * param)
	{
	int res=0;
	int retval;
	FILE * fp;
	Node * nd=(Node *) param;
	int chk=nd->iCnt;
	int ret;

	fp=fopen("c:\\input.txt","r");
	if(fp==NULL)
		{
		res=1;
		}
	else
		{
		ret=fseek(fp,chk,SEEK_SET);
		if(ret!=0)
			{
			res=1;
			}
		fclose(fp);
		}
 
	retval=sem_post(nd->iSemaphore);
	if(retval!=0)
		{
		res=1;
		}

	return (void *)res;
	}


/*****************parent thread function for fseek, ftell test ******************/

TInt CTestStdio::fseek_multithreaded()
	{

	TInt result=KErrGeneral;
	pthread_t thrid1=0,thrid2=0;
	pthread_attr_t threadAttr;
	int retval,exitReason1=0,exitReason2=0,res=0;
	sem_t semaphore;
	Node element;

	if(sem_init(&semaphore,0,0)!=0)
		{
		INFO_PRINTF1(_L("sem_init failed in main function\n"));
		return KErrGeneral;
		}

	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );

	element.iCnt=3;
	element.iSemaphore=&semaphore;

	if(pthread_create(&thrid1,&threadAttr,seek_thread,(void *)&element)!=0)
		{
		INFO_PRINTF1(_L("seek thread not created properly\n"));
		return KErrGeneral;
		}
	else
		{
		element.iCnt=0;
 
		if(pthread_create(&thrid2,&threadAttr,tell_thread,(void *)&element)!=0)
			{
			INFO_PRINTF1(_L("tell thread not created properly\n"));
			return KErrGeneral;
			}
		else
			{	
			retval= pthread_join(thrid1, (void**)&exitReason1 );
			if(retval) 
				{
				INFO_PRINTF1(_L("Case failed because of seek thread\n"));
				res=1;
				}

			retval= pthread_join(thrid2, (void**)&exitReason2 );
			if(retval) 
				{
				INFO_PRINTF1(_L("Case failed because of tell thread\n"));
				res=1;
				}
			}
		}

	if(!(exitReason1||exitReason2)) 
		{
		INFO_PRINTF1(_L("case passed successfully\n"));
		result=KErrNone;
		}
	else 
		{
		result=KErrGeneral;
		}

	if(sem_destroy(&semaphore)==-1)
		{
		result=KErrGeneral;
		}

	INFO_PRINTF2(_L("fseek multithreaded case returned with code %d\n"),res);

	return result;


	}

/***************** these functions are for printf test **********************/

void * print_thr2(void * param)
	{
	int res=KErrNone;
	int retval;
	Node * nd=(Node *) param;
	int ret;

	retval=sem_wait(nd->iSemaphore);
	if(retval!=0)
		{
		res=1;
		}
	else
	{
	ret=printf("hi again\n");
	if(ret==9)
		{
		res=0;
		}
	}

	return (void *)res;
	}


void * print_thr1(void * param)
	{
	int res=KErrNone;
	int retval;
	Node * nd=(Node *) param;
	int ret;

	ret=printf("hi\n");
	if(ret==3)
		{
		res=0;
		}
	retval=sem_post(nd->iSemaphore);
	if(retval!=0)
		{
		res=1;
		}
	return (void *)res;
	}


/*****************parent thread function for printf test ******************/

TInt CTestStdio::printf_multithreaded()
	{

	TInt result=KErrGeneral;
	pthread_t thrid1=0,thrid2=0;
	pthread_attr_t threadAttr;
	int retval,exitReason1=0,exitReason2=0,res=0;
	sem_t semaphore;
	Node element;

	if(sem_init(&semaphore,0,0)!=0)
		{
		INFO_PRINTF1(_L("sem_init failed in main function\n"));
		return KErrGeneral;
		}

	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );
	
	element.iSemaphore=&semaphore;

	if(pthread_create(&thrid1,&threadAttr,print_thr1,(void *)&element)!=0)
		{
		INFO_PRINTF1(_L("thread1 thread not created properly\n"));
		return KErrGeneral;
		}
	else
		{	
		if(pthread_create(&thrid2,&threadAttr,print_thr2,(void *)&element)!=0)
			{	
			INFO_PRINTF1(_L("thread2 thread not created properly\n"));
			return KErrGeneral;
			}
		else
			{
			retval= pthread_join(thrid1, (void**)&exitReason1 );
			if(retval) 
  				{
				INFO_PRINTF1(_L("Case failed because of thread1 thread\n"));
  				res=1;
  				}

			retval= pthread_join(thrid2, (void**)&exitReason2 );
			if(retval) 
  				{
  				INFO_PRINTF1(_L("Case failed because of thread2 thread\n"));
  				res=1;
  				}
			}
		}

	if(!(exitReason1||exitReason2)) 
		{
		INFO_PRINTF1(_L("case passed successfully\n"));
		result=KErrNone;
		}
	else
		{
		result=KErrGeneral;
		}

	if(sem_destroy(&semaphore)==-1)
		{
		result=KErrGeneral;
		}

	INFO_PRINTF2(_L("printf multithreaded case returned with code %d\n"),res);

	return result;
	}

/*************************** freopen tests ************************************************/

void * map_thread(void * param)
	{
	int res=0;
	int retval;
	FILE * fp;
	Node * nd=(Node *) param;
	char msg[100];
//	dup2(2,1);
	fp=freopen("c:\\cons.txt","w",stdout);
//	fp = fopen("c:\\cons3.txt", "w");
//	dup2(fileno(fp), 1);
/*	int fperm = O_RDWR | O_CREAT;
	int fildes = open("c:\\cons2.txt", fperm);
	dup2(fildes, 1);
	if(fildes!=0)*/
	if(fp!=NULL)
		{
		retval=sem_wait(nd->iSemaphore);
		if(retval!=0)
			{
			res=1;
			}
		else
			{
				//lseek(fildes,0,SEEK_CUR);
				fseek(fp, 0, SEEK_CUR);
				fgets(msg,sizeof(msg),fp);
			}
		
		fclose(fp);
		}
 
	return (void *)res;
	}


void * cons_thread(void * param)
	{
	int res=0;
	int retval;
	Node * nd=(Node *) param;
	int ret=KErrNone;
	if(ret==strlen(nd->msg))
		{
		res=0;
		}	
	retval=sem_post(nd->iSemaphore);
	if(retval!=0)
		{	
		res=1;
		}
	return (void *)res;
	}


/*****************parent thread function for freopen test ******************/
TInt CTestStdio::freopen_multithreaded()
	{

	TInt result=KErrGeneral;
	pthread_t thrid1=0,thrid2=0;
	pthread_attr_t threadAttr;
	int retval,exitReason1=0,exitReason2=0,res=0;
	sem_t semaphore;
	Node element;

	if(sem_init(&semaphore,0,0)!=0)
		{
		INFO_PRINTF1(_L("sem_init failed in main function\n"));
		return KErrGeneral;
		}

	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );

	element.iSemaphore=&semaphore;
	strcpy(element.msg,"hi there\n");

	if(pthread_create(&thrid1,&threadAttr,cons_thread,(void *)&element)!=0)
		{
		INFO_PRINTF1(_L("cons_thread not created properly\n"));
		return KErrGeneral;
		}
	else
		{
		if(pthread_create(&thrid2,&threadAttr,map_thread,(void *)&element)!=0)
			{
			INFO_PRINTF1(_L("map_thread not created properly\n"));
			return KErrGeneral;
			}
		else
			{
			retval= pthread_join(thrid1, (void**)&exitReason1 );
			if(retval)
				{
				INFO_PRINTF1(_L("Case failed because of cons_thread\n"));
				res=1;
				}

			retval= pthread_join(thrid2, (void**)&exitReason2 );
			if(retval)
				{
				INFO_PRINTF1(_L("Case failed because of map_thread\n"));
				res=1;
				}
			}
		}

	if(!(exitReason1||exitReason2)) 
		{
		INFO_PRINTF1(_L("case passed successfully\n"));
		result=KErrNone;
		}
	else
		{
		result=KErrGeneral;
		}

	if(sem_destroy(&semaphore)==-1)
		{
		result=KErrGeneral;
		}

	INFO_PRINTF2(_L("freopen multithreaded case returned with code %d\n"),res);

	return result;
	}


/***************************** putc_unlocked tests *******************************/

void * getc_thread(void * param)
	{
	int res=KErrNone;
	int retval;
	Node * nd=(Node *) param;
	flockfile(nd->fp);
	retval=getc_unlocked(nd->fp);
	if(retval==nd->msg[0])
		{
		res=0;
		}
	fclose(nd->fp);
	funlockfile(nd->fp);
	return (void *)res;
	}

/****************************** main thread for putc_unlocked tests *******************/

TInt CTestStdio::putc_multithreaded()
	{
	TInt result;
	pthread_t thrid1=0;
	pthread_attr_t threadAttr;
	int retval,exitReason1=0,res=0;
	sem_t semaphore;
	Node element;
	FILE * fp;

	element.iCnt=3;
	element.iSemaphore=&semaphore;
	element.msg[0]='a';

	fp=fopen("C:\\cons.txt","w");
	element.fp = fp;
	flockfile(fp);

	pthread_attr_init( &threadAttr );
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_JOINABLE );
	
	if(pthread_create(&thrid1,&threadAttr,getc_thread,(void *)&element)!=0)
		{
		funlockfile(fp);
		fclose(fp);
		INFO_PRINTF1(_L("getc thread not created properly\n"));
		}
	else
		{
		putc_unlocked('a',fp);
		funlockfile(fp);
		fclose(fp);
		retval=pthread_join(thrid1,(void **)&exitReason1);
		if(retval)
			{
			INFO_PRINTF1(_L("pthread join failed\n"));
			}
		}

	if(exitReason1)
		{
		res=1;
		}

	if(!res)
		{
		result=KErrNone;
		}	
	else
		{
		result=KErrGeneral;
		}
		

	INFO_PRINTF2(_L("putc multithreaded case returned with code %d\n"),res);
	return result;
	}


