// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : tsempostcases.cpp
// Part of     : testsempost
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//




#include "tsempost.h"

// ============================ MEMBER FUNCTIONS ===============================
static bool g_spinFlag[EThreadMain] = 
	{
    true,
    true,
    true,
    true,
    true
	};
int (*fp)(void* arg);
int func(void* arg);
int func1(void* arg);
int func2(void* arg);
int func3(void* arg);
int func4(void* arg);
int func5(void* arg);
int func6(void* arg);
int func7(void* arg);
int func8(void* arg);
int func9(void* arg);

//sem_post on an uninitialized semaphore
int SemPost(ThreadData* aThreadData)
	{
	int retval=0;
	retval = sem_post(aThreadData->iTestSemaphore);
	return retval;
	}

int VerifyErrno(ThreadData* aThreadData,int expectedResult)
	{
	int retval=0;
	if ( (expectedResult == errno)  )
		{
		errno = 0;
		}
	else
		{
		#ifdef WINDOWS
			printf("Expected retval %d Seen %d Expected errno %d Seen %d\n",expectedResult, aThreadData->iRetValue,aThreadData->iExpectederrno,aThreadData->ierrno);
		#else
			;
		#endif
		retval = KUnexpectedResult;
		}
 	aThreadData->iRetValue = 0;
	aThreadData->ierrno    = 0;
	return retval;
	}

void StopThread(ThreadData* aThreadData)
	{
	if(aThreadData->iSelf != EThreadMain)
		{
		aThreadData->iStopped = true;
		sem_post(aThreadData->iSuspendSemaphore);
		#ifdef USE_RTHREAD
			User::Exit(KErrNone);
		#endif
		}
	}
void WaitTillSuspended(ThreadData* aThreadData,void*  aThreadId)
	{
	HarnessThread threadId;
	ThreadData* lTarget;
	int signalsEaten =0;
	int i=0;
	
	threadId = (HarnessThread) (int) aThreadId;
	lTarget = NULL;
	if(threadId >=0 && threadId < EThreadMain)
		{
		lTarget = aThreadData->iTDArr[threadId];
		}
	if(lTarget)
		{
		sem_wait(lTarget->iSuspendSemaphore);
		while(lTarget->iSuspending == false)
			{
			signalsEaten++;
			sem_wait(lTarget->iSuspendSemaphore);
			}
		for(i=0; i <signalsEaten; i++)
			{
			sem_post(lTarget->iSuspendSemaphore);
			}
		}
	}

TInt SemInit(ThreadData* aThreadData)
	{
	TInt retval=0;
	semParam_t* semParam =NULL;
	if(semParam)
		{
		retval = sem_init(aThreadData->iTestSemaphore,semParam->pshared,semParam->value);
		}
	else
		{
		retval = sem_init(aThreadData->iTestSemaphore,0,0);
		}
	return retval;
	}
int ThreadCreate(ThreadData* aThreadData, void* aThreadId)
	{
	HarnessThread threadId;
	int retval=0;
	if(aThreadData->iSelf != EThreadMain)
		{
		retval = KNoPermission;
		}
	else
		{
		threadId = (HarnessThread)(int) aThreadId;
		retval = NewThread(aThreadData,threadId);
		}
	return retval;
	}

int NewThread(ThreadData* aThreadData, HarnessThread aThreadId)
	{
	ThreadData* newData = new ThreadData;
	if(!newData)
		{
		return KNoMemory;
		}

	if(aThreadId < EThreadMain && aThreadId >= 0)
		{
		aThreadData->iTDArr[aThreadId] = newData;
		}
	else
		{
		return KNoArgument;
		}

	newData->iSignalSemaphore = aThreadData->iSignalSemaphore;
	newData->iSuspendSemaphore = aThreadData->iSuspendSemaphore;


	newData->iTestSemaphore = aThreadData->iTestSemaphore;
	newData->iTestMutex = aThreadData->iTestMutex;
	newData->iTestCondVar = aThreadData->iTestCondVar;
	newData->iDefaultAttr = aThreadData->iDefaultAttr;
	newData->iErrorcheckAttr = aThreadData->iErrorcheckAttr;
	newData->iRecursiveAttr = aThreadData->iRecursiveAttr;
	newData->iCondAttr = aThreadData->iCondAttr;
	newData->iSuspending = false;
	newData->iSpinCounter = 0;
	newData->iCurrentCommand = -1;
	newData->iSelf = aThreadId;
	newData->iValue = 0;
	newData->iRetValue = 0;
	newData->ierrno = 0;
	newData->iValue = 0;
	newData->iExpectederrno = 0;
	newData->iTimes = 0;
	newData->iStopped = false;
	newData->iCommonData = aThreadData->iCommonData;

	#ifdef USE_RTHREAD
		TBuf<10> threadName;
		threadName.NumFixedWidth(TUint(aThreadId), EDecimal, 10);
		RThread lNewThread;
		lNewThread.Create(
            (const TDesC &)threadName, // Thread Name
               _mrtEntryPtFun, // Entry pt function
               KDefaultStackSize,      // Stack Size
               NULL,          // Use common heap
               (TAny*)newData); // Args to entry pt function
		lNewThread.Resume();
		lNewThread.Close();
	#else
		pthread_create(&aThreadData->iIdArr[aThreadId],NULL,StartFn,(void*)newData);
	#endif
	return 0;
	}
int DeleteThread(ThreadData* aThreadData, HarnessThread aThreadId)
	{	
	#ifndef USE_RTHREAD
		pthread_join(aThreadData->iIdArr[aThreadId],NULL);
	#else
		int signalsEaten = 0;
		sem_wait(aThreadData->iSuspendSemaphore);
		while(aThreadData->iTDArr[aThreadId]->iStopped == false)
			{
			signalsEaten++;
			sem_wait(aThreadData->iSuspendSemaphore);
			}
		for(int i=0; i<signalsEaten; i++)
			{
			sem_post(aThreadData->iSuspendSemaphore);
			}
	#endif
		if(aThreadData->iTDArr[aThreadId]->iRetValue != 0)
			{
			printf("Thread %d errvalue %d\n",aThreadId,aThreadData->iTDArr[aThreadId]->iRetValue);
			}
		delete aThreadData->iTDArr[aThreadId];
		aThreadData->iTDArr[aThreadId] = NULL;
		return 0;
	}
int WaitForSignal(ThreadData* aThreadData)
	{
	int retval;
	retval = sem_wait(aThreadData->iSignalSemaphore);
	return retval;
	}
int ThreadDestroy(ThreadData* aThreadData,void* aThreadId)
	{
	int retval=0;
	HarnessThread threadId;
	if(aThreadData->iSelf != EThreadMain)
		{
		retval = KNoPermission;
		}
	else
		{
		threadId = (HarnessThread)(int) aThreadId;
		retval = DeleteThread(aThreadData,threadId);
		}
	return retval;
	}
int SemDestroy(ThreadData* aThreadData)
	{	
	int retval;
	retval =  sem_destroy(aThreadData->iTestSemaphore);
	return retval;
	}
int SemWait(ThreadData* aThreadData)
	{
	int retval;
	aThreadData->iSuspending = true;
	sem_post(aThreadData->iSuspendSemaphore);
	retval = sem_wait(aThreadData->iTestSemaphore);
	aThreadData->iSuspending = false;
	return retval;
	}
int StopSpin(ThreadData* aThreadData,void*  aThreadId)			
	{
	HarnessThread threadId;
	ThreadData* lTarget;
	int retval=0;
	
	if(aThreadData->iSelf != EThreadMain)
		{
		retval = KNoPermission;
		return retval;
		}
	threadId = (HarnessThread) (int) aThreadId;
	lTarget = NULL;
	if(threadId >=0 && threadId < EThreadMain)
		{
		lTarget = aThreadData->iTDArr[threadId];
		}
	if(lTarget)
		{
		g_spinFlag[threadId] = false;
		}
	else
		{
		retval = KNoArgument;
		}
	return retval;
	}
void SemPostAsNeeded(ThreadData* aThreadData)
	{
	int n,i;
	sem_getvalue(aThreadData->iTestSemaphore,&n);
	n = -n;
	for(i =0; i<n; i++)
		{
		sem_post(aThreadData->iTestSemaphore);
		}
	}
int PostSignal(ThreadData* aThreadData)
	{
	int retval=0;
	retval = sem_post(aThreadData->iSignalSemaphore);
	return retval;
	}
int Sem284B(ThreadData* aThreadData)
	{
	int i=0,retval=0;
	i =0;
	while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
		{
		retval = sem_wait(aThreadData->iTestSemaphore);
		i += 100;
		}
	return retval;
	}
int Sem697B(ThreadData* aThreadData)
	{
	int i =0,retval=0;
	while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
		{
		retval = sem_trywait(aThreadData->iTestSemaphore);
		if(retval == -1 && errno == EAGAIN)
			{
			retval = 0;
			errno  = 0;
			}
		i += 100;
		}
	return retval;
	}
int Sem698B(ThreadData* aThreadData)
	{	
	int i =0,retval=0;
	struct timespec* abstime;
	struct timespec defaulttimeout;
	struct timeval now;
	gettimeofday(&now,NULL);
	defaulttimeout.tv_sec = now.tv_sec + 10;
	defaulttimeout.tv_nsec = 0;
	abstime = &defaulttimeout;
	
	while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
		{
		retval = sem_timedwait(aThreadData->iTestSemaphore,abstime);
		i += 100;
		}
	return retval;
	}
int LoopCommand(ThreadData* aThreadData)
	{	
	int j = 0,retval=0;
		
	for(int i=0; i < 1000; i += 100)
		{
		j++;
		if(j == 255)
			{
			j = 0;
			#ifdef WINDOWS
				printf("%dLooping Id:  %d Cmd1 %d Cmd2 %d\n",i,aData->iSelf,Command1, Command2);
			#else
				{
				//MSG(_L("%dLooping Id:  %d Cmd1 %d Cmd2 %d\n"),i,aData->iSelf,Command1, Command2);
				}
			#endif
			}
		retval = SemPost(aThreadData);
		retval = SemWait(aThreadData);
		}
	return retval;
	}
int LoopCommand1(ThreadData* aThreadData)
	{	
	int j = 0,retval=0;
		
	for(int i=0; i < 1000; i += 100)
		{
		j++;
		if(j == 255)
			{
			j = 0;
			#ifdef WINDOWS
				printf("%dLooping Id:  %d Cmd1 %d Cmd2 %d\n",i,aData->iSelf,Command1, Command2);
			#else
				{
				//MSG(_L("%dLooping Id:  %d Cmd1 %d Cmd2 %d\n"),i,aData->iSelf,Command1, Command2);
				}
			#endif
			}
		retval = SemPost(aThreadData);
		retval = SemPost(aThreadData);
		}
	return retval;
	}
void* StartFn(void* arg)
	{
	int retval=0;
	
	retval = (*fp)(arg);
	return (void *)retval;
	
	}
int func(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = SemWait(pData);
	retval = PostSignal(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}

int func1(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = SemPost(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}
int func2(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = SemPost(pData);
	retval = PostSignal(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}
int func3(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = SemWait(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}

int func4(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = LoopCommand(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}
int func5(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = LoopCommand1(pData);
	retval = PostSignal(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}
int func6(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = Sem284B(pData);
	retval = PostSignal(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}
int func7(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = LoopCommand(pData);
	retval = PostSignal(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}
int func8(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = Sem697B(pData);
	retval = PostSignal(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}	
int func9(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = Sem698B(pData);
	retval = PostSignal(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}	
TInt CTestSempost::TestSem279( )
	{
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	pthread_mutex_t lTestMutex;
	pthread_cond_t  lTestCondVar;
	pthread_condattr_t  lCondAttr;
	pthread_mutexattr_t lTestMutexAttr;

	pthread_mutexattr_t defaultattr;
	pthread_mutexattr_t errorcheckattr;
	pthread_mutexattr_t recursiveattr;

	pthread_mutexattr_init(&defaultattr);
	pthread_mutexattr_init(&errorcheckattr);
	pthread_mutexattr_init(&recursiveattr);

	pthread_mutexattr_settype(&errorcheckattr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutexattr_settype(&recursiveattr,PTHREAD_MUTEX_RECURSIVE);


	pthread_mutex_t l_staticmutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t l_errorcheckmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	pthread_mutex_t l_recursivemutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_cond_t  l_staticcondvar = PTHREAD_COND_INITIALIZER;


    CommonData lCommonData;
    lCommonData.iStaticMutex = &l_staticmutex;
	lCommonData.iErrorCheckMutex = &l_errorcheckmutex;
	lCommonData.iRecursiveMutex = &l_recursivemutex;
	lCommonData.iStaticCondVar = &l_staticcondvar;

	retval = sem_init(&lSignalSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	retval = sem_init(&lSuspendSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	lThreadData.iSignalSemaphore = &lSignalSemaphore;
	lThreadData.iSuspendSemaphore = &lSuspendSemaphore;
	lThreadData.iTestSemaphore   = &lTestSemaphore;
	lThreadData.iTestMutex       = &lTestMutex;
	lThreadData.iTestMutexAttr   = &lTestMutexAttr;
	lThreadData.iTestCondVar     = &lTestCondVar;
	lThreadData.iDefaultAttr     = &defaultattr;
	lThreadData.iErrorcheckAttr = &errorcheckattr;
	lThreadData.iRecursiveAttr   = &recursiveattr;

	lThreadData.iCondAttr        = &lCondAttr;
	for (int loop = 0; loop < EThreadMain; loop++)
		{
	    g_spinFlag[loop] = true;
		}
	lThreadData.iSuspending      = false;
	lThreadData.iSpinCounter     = 0;
	lThreadData.iCurrentCommand  = -1;
	lThreadData.iSelf            = EThreadMain;
	lThreadData.iValue           = 0;
	lThreadData.iRetValue        = 0;
	lThreadData.ierrno           = 0;
	lThreadData.iExpectederrno   = 0;
	lThreadData.iTimes           = 0;
	lThreadData.iStopped         = false;
	lThreadData.iCommonData      = &lCommonData;
	
	retval = SemPost(&lThreadData);
	retval = VerifyErrno(&lThreadData,EINVAL);
	StopThread(&lThreadData);
	
	err = pthread_cond_destroy(&l_staticcondvar);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_recursivemutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_errorcheckmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_staticmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutexattr_destroy(&recursiveattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&errorcheckattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&defaultattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = sem_destroy(&lSignalSemaphore);
	if(err != EINVAL)
		{	
		errsum += err;
		}
	err = sem_destroy(&lSuspendSemaphore);
	if(err != EINVAL)
		{
		errsum += err;
		}
	return retval+errsum;
	}

//sem_post when another thread suspended on sem_wait

TInt CTestSempost::TestSem280( )
	{
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	pthread_mutex_t lTestMutex;
	pthread_cond_t  lTestCondVar;
	pthread_condattr_t  lCondAttr;
	pthread_mutexattr_t lTestMutexAttr;

	pthread_mutexattr_t defaultattr;
	pthread_mutexattr_t errorcheckattr;
	pthread_mutexattr_t recursiveattr;

	pthread_mutexattr_init(&defaultattr);
	pthread_mutexattr_init(&errorcheckattr);
	pthread_mutexattr_init(&recursiveattr);

	pthread_mutexattr_settype(&errorcheckattr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutexattr_settype(&recursiveattr,PTHREAD_MUTEX_RECURSIVE);


	pthread_mutex_t l_staticmutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t l_errorcheckmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	pthread_mutex_t l_recursivemutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_cond_t  l_staticcondvar = PTHREAD_COND_INITIALIZER;


    CommonData lCommonData;
    lCommonData.iStaticMutex = &l_staticmutex;
	lCommonData.iErrorCheckMutex = &l_errorcheckmutex;
	lCommonData.iRecursiveMutex = &l_recursivemutex;
	lCommonData.iStaticCondVar = &l_staticcondvar;

	retval = sem_init(&lSignalSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	retval = sem_init(&lSuspendSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	lThreadData.iSignalSemaphore = &lSignalSemaphore;
	lThreadData.iSuspendSemaphore = &lSuspendSemaphore;
	lThreadData.iTestSemaphore   = &lTestSemaphore;
	lThreadData.iTestMutex       = &lTestMutex;
	lThreadData.iTestMutexAttr   = &lTestMutexAttr;
	lThreadData.iTestCondVar     = &lTestCondVar;
	lThreadData.iDefaultAttr     = &defaultattr;
	lThreadData.iErrorcheckAttr = &errorcheckattr;
	lThreadData.iRecursiveAttr   = &recursiveattr;

	lThreadData.iCondAttr        = &lCondAttr;
	for (int loop = 0; loop < EThreadMain; loop++)
		{
	    g_spinFlag[loop] = true;
		}
	lThreadData.iSuspending      = false;
	lThreadData.iSpinCounter     = 0;
	lThreadData.iCurrentCommand  = -1;
	lThreadData.iSelf            = EThreadMain;
	lThreadData.iValue           = 0;
	lThreadData.iRetValue        = 0;
	lThreadData.ierrno           = 0;
	lThreadData.iExpectederrno   = 0;
	lThreadData.iTimes           = 0;
	lThreadData.iStopped         = false;
	lThreadData.iCommonData      = &lCommonData;
	
	retval = SemInit(&lThreadData);
	
	fp=func;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	WaitTillSuspended(&lThreadData, (void*) EThread1);
	
	fp=func1;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = SemDestroy(&lThreadData);
	StopThread(&lThreadData);
	
	err = pthread_cond_destroy(&l_staticcondvar);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_recursivemutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_errorcheckmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_staticmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutexattr_destroy(&recursiveattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&errorcheckattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&defaultattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = sem_destroy(&lSignalSemaphore);
	if(err != EINVAL)
		{	
		errsum += err;
		}
	err = sem_destroy(&lSuspendSemaphore);
	if(err != EINVAL)
		{
		errsum += err;
		}
	return retval+errsum;
	}


//sem_post when no threads are suspended on semaphore
TInt CTestSempost::TestSem281()
	{
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	pthread_mutex_t lTestMutex;
	pthread_cond_t  lTestCondVar;
	pthread_condattr_t  lCondAttr;
	pthread_mutexattr_t lTestMutexAttr;

	pthread_mutexattr_t defaultattr;
	pthread_mutexattr_t errorcheckattr;
	pthread_mutexattr_t recursiveattr;

	pthread_mutexattr_init(&defaultattr);
	pthread_mutexattr_init(&errorcheckattr);
	pthread_mutexattr_init(&recursiveattr);

	pthread_mutexattr_settype(&errorcheckattr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutexattr_settype(&recursiveattr,PTHREAD_MUTEX_RECURSIVE);


	pthread_mutex_t l_staticmutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t l_errorcheckmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	pthread_mutex_t l_recursivemutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_cond_t  l_staticcondvar = PTHREAD_COND_INITIALIZER;


    CommonData lCommonData;
    lCommonData.iStaticMutex = &l_staticmutex;
	lCommonData.iErrorCheckMutex = &l_errorcheckmutex;
	lCommonData.iRecursiveMutex = &l_recursivemutex;
	lCommonData.iStaticCondVar = &l_staticcondvar;

	retval = sem_init(&lSignalSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	retval = sem_init(&lSuspendSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	lThreadData.iSignalSemaphore = &lSignalSemaphore;
	lThreadData.iSuspendSemaphore = &lSuspendSemaphore;
	lThreadData.iTestSemaphore   = &lTestSemaphore;
	lThreadData.iTestMutex       = &lTestMutex;
	lThreadData.iTestMutexAttr   = &lTestMutexAttr;
	lThreadData.iTestCondVar     = &lTestCondVar;
	lThreadData.iDefaultAttr     = &defaultattr;
	lThreadData.iErrorcheckAttr = &errorcheckattr;
	lThreadData.iRecursiveAttr   = &recursiveattr;

	lThreadData.iCondAttr        = &lCondAttr;
	for (int loop = 0; loop < EThreadMain; loop++)
		{
	    g_spinFlag[loop] = true;
		}
	lThreadData.iSuspending      = false;
	lThreadData.iSpinCounter     = 0;
	lThreadData.iCurrentCommand  = -1;
	lThreadData.iSelf            = EThreadMain;
	lThreadData.iValue           = 0;
	lThreadData.iRetValue        = 0;
	lThreadData.ierrno           = 0;
	lThreadData.iExpectederrno   = 0;
	lThreadData.iTimes           = 0;
	lThreadData.iStopped         = false;
	lThreadData.iCommonData      = &lCommonData;
	
	retval = SemInit(&lThreadData);
	
	fp=func2;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	retval = WaitForSignal(&lThreadData);
	
	fp=func3;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = SemDestroy(&lThreadData);
	StopThread(&lThreadData);
	
	err = pthread_cond_destroy(&l_staticcondvar);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_recursivemutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_errorcheckmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_staticmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutexattr_destroy(&recursiveattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&errorcheckattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&defaultattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = sem_destroy(&lSignalSemaphore);
	if(err != EINVAL)
		{	
		errsum += err;
		}
	err = sem_destroy(&lSuspendSemaphore);
	if(err != EINVAL)
		{
		errsum += err;
		}
	return retval+errsum;
	}

//race between multiple sem_post when no threads suspended on semaphore
TInt CTestSempost::TestSem282()
	{
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	pthread_mutex_t lTestMutex;
	pthread_cond_t  lTestCondVar;
	pthread_condattr_t  lCondAttr;
	pthread_mutexattr_t lTestMutexAttr;

	pthread_mutexattr_t defaultattr;
	pthread_mutexattr_t errorcheckattr;
	pthread_mutexattr_t recursiveattr;

	pthread_mutexattr_init(&defaultattr);
	pthread_mutexattr_init(&errorcheckattr);
	pthread_mutexattr_init(&recursiveattr);

	pthread_mutexattr_settype(&errorcheckattr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutexattr_settype(&recursiveattr,PTHREAD_MUTEX_RECURSIVE);


	pthread_mutex_t l_staticmutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t l_errorcheckmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	pthread_mutex_t l_recursivemutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_cond_t  l_staticcondvar = PTHREAD_COND_INITIALIZER;


    CommonData lCommonData;
    lCommonData.iStaticMutex = &l_staticmutex;
	lCommonData.iErrorCheckMutex = &l_errorcheckmutex;
	lCommonData.iRecursiveMutex = &l_recursivemutex;
	lCommonData.iStaticCondVar = &l_staticcondvar;

	retval = sem_init(&lSignalSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	retval = sem_init(&lSuspendSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	lThreadData.iSignalSemaphore = &lSignalSemaphore;
	lThreadData.iSuspendSemaphore = &lSuspendSemaphore;
	lThreadData.iTestSemaphore   = &lTestSemaphore;
	lThreadData.iTestMutex       = &lTestMutex;
	lThreadData.iTestMutexAttr   = &lTestMutexAttr;
	lThreadData.iTestCondVar     = &lTestCondVar;
	lThreadData.iDefaultAttr     = &defaultattr;
	lThreadData.iErrorcheckAttr = &errorcheckattr;
	lThreadData.iRecursiveAttr   = &recursiveattr;

	lThreadData.iCondAttr        = &lCondAttr;
	for (int loop = 0; loop < EThreadMain; loop++)
		{
	    g_spinFlag[loop] = true;
		}
	lThreadData.iSuspending      = false;
	lThreadData.iSpinCounter     = 0;
	lThreadData.iCurrentCommand  = -1;
	lThreadData.iSelf            = EThreadMain;
	lThreadData.iValue           = 0;
	lThreadData.iRetValue        = 0;
	lThreadData.ierrno           = 0;
	lThreadData.iExpectederrno   = 0;
	lThreadData.iTimes           = 0;
	lThreadData.iStopped         = false;
	lThreadData.iCommonData      = &lCommonData;
	
	retval = SemInit(&lThreadData);
	
	fp=func4;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	fp=func4;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = SemDestroy(&lThreadData);
	StopThread(&lThreadData);
	
	err = pthread_cond_destroy(&l_staticcondvar);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_recursivemutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_errorcheckmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_staticmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutexattr_destroy(&recursiveattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&errorcheckattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&defaultattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = sem_destroy(&lSignalSemaphore);
	if(err != EINVAL)
		{	
		errsum += err;
		}
	err = sem_destroy(&lSuspendSemaphore);
	if(err != EINVAL)
		{
		errsum += err;
		}
	return retval+errsum;
	}



//race between multiple sem_post when  threads suspended on semaphore
TInt CTestSempost::TestSem283( )
	{
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	pthread_mutex_t lTestMutex;
	pthread_cond_t  lTestCondVar;
	pthread_condattr_t  lCondAttr;
	pthread_mutexattr_t lTestMutexAttr;

	pthread_mutexattr_t defaultattr;
	pthread_mutexattr_t errorcheckattr;
	pthread_mutexattr_t recursiveattr;

	pthread_mutexattr_init(&defaultattr);
	pthread_mutexattr_init(&errorcheckattr);
	pthread_mutexattr_init(&recursiveattr);

	pthread_mutexattr_settype(&errorcheckattr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutexattr_settype(&recursiveattr,PTHREAD_MUTEX_RECURSIVE);


	pthread_mutex_t l_staticmutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t l_errorcheckmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	pthread_mutex_t l_recursivemutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_cond_t  l_staticcondvar = PTHREAD_COND_INITIALIZER;


    CommonData lCommonData;
    lCommonData.iStaticMutex = &l_staticmutex;
	lCommonData.iErrorCheckMutex = &l_errorcheckmutex;
	lCommonData.iRecursiveMutex = &l_recursivemutex;
	lCommonData.iStaticCondVar = &l_staticcondvar;

	retval = sem_init(&lSignalSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	retval = sem_init(&lSuspendSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	lThreadData.iSignalSemaphore = &lSignalSemaphore;
	lThreadData.iSuspendSemaphore = &lSuspendSemaphore;
	lThreadData.iTestSemaphore   = &lTestSemaphore;
	lThreadData.iTestMutex       = &lTestMutex;
	lThreadData.iTestMutexAttr   = &lTestMutexAttr;
	lThreadData.iTestCondVar     = &lTestCondVar;
	lThreadData.iDefaultAttr     = &defaultattr;
	lThreadData.iErrorcheckAttr = &errorcheckattr;
	lThreadData.iRecursiveAttr   = &recursiveattr;

	lThreadData.iCondAttr        = &lCondAttr;
	for (int loop = 0; loop < EThreadMain; loop++)
		{
	    g_spinFlag[loop] = true;
		}
	lThreadData.iSuspending      = false;
	lThreadData.iSpinCounter     = 0;
	lThreadData.iCurrentCommand  = -1;
	lThreadData.iSelf            = EThreadMain;
	lThreadData.iValue           = 0;
	lThreadData.iRetValue        = 0;
	lThreadData.ierrno           = 0;
	lThreadData.iExpectederrno   = 0;
	lThreadData.iTimes           = 0;
	lThreadData.iStopped         = false;
	lThreadData.iCommonData      = &lCommonData;
	
	retval = SemInit(&lThreadData);
	
	fp=func3;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	fp=func3;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	fp=func3;
	retval = ThreadCreate(&lThreadData, (void*) EThread3);
	
	WaitTillSuspended(&lThreadData, (void*) EThread1);
	WaitTillSuspended(&lThreadData, (void*) EThread2);
	WaitTillSuspended(&lThreadData, (void*) EThread3);
	
	fp=func4;
	retval = ThreadCreate(&lThreadData, (void*) EThread4);
	
	fp=func4;
	retval = ThreadCreate(&lThreadData, (void*) EThread5);
	
	retval = SemPost(&lThreadData);
	retval = SemPost(&lThreadData);
	retval = SemPost(&lThreadData);
	retval = SemPost(&lThreadData);
	retval = SemPost(&lThreadData);
	
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = ThreadDestroy(&lThreadData, (void*) EThread3);
	retval = ThreadDestroy(&lThreadData, (void*) EThread4);
	retval = ThreadDestroy(&lThreadData, (void*) EThread5);
	
	retval = SemDestroy(&lThreadData);
	StopThread(&lThreadData);
	
	err = pthread_cond_destroy(&l_staticcondvar);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_recursivemutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_errorcheckmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_staticmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutexattr_destroy(&recursiveattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&errorcheckattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&defaultattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = sem_destroy(&lSignalSemaphore);
	if(err != EINVAL)
		{	
		errsum += err;
		}
	err = sem_destroy(&lSuspendSemaphore);
	if(err != EINVAL)
		{
		errsum += err;
		}
	return retval+errsum;
	}


//race between sem_post and sem_wait when no threads are suspended
TInt CTestSempost::TestSem284()
	{
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	pthread_mutex_t lTestMutex;
	pthread_cond_t  lTestCondVar;
	pthread_condattr_t  lCondAttr;
	pthread_mutexattr_t lTestMutexAttr;

	pthread_mutexattr_t defaultattr;
	pthread_mutexattr_t errorcheckattr;
	pthread_mutexattr_t recursiveattr;

	pthread_mutexattr_init(&defaultattr);
	pthread_mutexattr_init(&errorcheckattr);
	pthread_mutexattr_init(&recursiveattr);

	pthread_mutexattr_settype(&errorcheckattr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutexattr_settype(&recursiveattr,PTHREAD_MUTEX_RECURSIVE);


	pthread_mutex_t l_staticmutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t l_errorcheckmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	pthread_mutex_t l_recursivemutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_cond_t  l_staticcondvar = PTHREAD_COND_INITIALIZER;


    CommonData lCommonData;
    lCommonData.iStaticMutex = &l_staticmutex;
	lCommonData.iErrorCheckMutex = &l_errorcheckmutex;
	lCommonData.iRecursiveMutex = &l_recursivemutex;
	lCommonData.iStaticCondVar = &l_staticcondvar;

	retval = sem_init(&lSignalSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	retval = sem_init(&lSuspendSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	lThreadData.iSignalSemaphore = &lSignalSemaphore;
	lThreadData.iSuspendSemaphore = &lSuspendSemaphore;
	lThreadData.iTestSemaphore   = &lTestSemaphore;
	lThreadData.iTestMutex       = &lTestMutex;
	lThreadData.iTestMutexAttr   = &lTestMutexAttr;
	lThreadData.iTestCondVar     = &lTestCondVar;
	lThreadData.iDefaultAttr     = &defaultattr;
	lThreadData.iErrorcheckAttr = &errorcheckattr;
	lThreadData.iRecursiveAttr   = &recursiveattr;

	lThreadData.iCondAttr        = &lCondAttr;
	for (int loop = 0; loop < EThreadMain; loop++)
		{
	    g_spinFlag[loop] = true;
		}
	lThreadData.iSuspending      = false;
	lThreadData.iSpinCounter     = 0;
	lThreadData.iCurrentCommand  = -1;
	lThreadData.iSelf            = EThreadMain;
	lThreadData.iValue           = 0;
	lThreadData.iRetValue        = 0;
	lThreadData.ierrno           = 0;
	lThreadData.iExpectederrno   = 0;
	lThreadData.iTimes           = 0;
	lThreadData.iStopped         = false;
	lThreadData.iCommonData      = &lCommonData;
	
	retval = SemInit(&lThreadData);
	
	fp=func5;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	retval = WaitForSignal(&lThreadData);
	
	fp=func6;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	retval = StopSpin(&lThreadData, (void*) EThread2);
	SemPostAsNeeded(&lThreadData);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	
	
	retval = SemDestroy(&lThreadData);
	StopThread(&lThreadData);

	err = pthread_cond_destroy(&l_staticcondvar);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_recursivemutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_errorcheckmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_staticmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutexattr_destroy(&recursiveattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&errorcheckattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&defaultattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = sem_destroy(&lSignalSemaphore);
	if(err != EINVAL)
		{	
		errsum += err;
		}
	err = sem_destroy(&lSuspendSemaphore);
	if(err != EINVAL)
		{
		errsum += err;
		}
	return retval+errsum;
	}

//race between sem_post and sem_trywait
TInt CTestSempost::TestSem697()
	{
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	pthread_mutex_t lTestMutex;
	pthread_cond_t  lTestCondVar;
	pthread_condattr_t  lCondAttr;
	pthread_mutexattr_t lTestMutexAttr;

	pthread_mutexattr_t defaultattr;
	pthread_mutexattr_t errorcheckattr;
	pthread_mutexattr_t recursiveattr;

	pthread_mutexattr_init(&defaultattr);
	pthread_mutexattr_init(&errorcheckattr);
	pthread_mutexattr_init(&recursiveattr);

	pthread_mutexattr_settype(&errorcheckattr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutexattr_settype(&recursiveattr,PTHREAD_MUTEX_RECURSIVE);


	pthread_mutex_t l_staticmutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t l_errorcheckmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	pthread_mutex_t l_recursivemutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_cond_t  l_staticcondvar = PTHREAD_COND_INITIALIZER;


    CommonData lCommonData;
    lCommonData.iStaticMutex = &l_staticmutex;
	lCommonData.iErrorCheckMutex = &l_errorcheckmutex;
	lCommonData.iRecursiveMutex = &l_recursivemutex;
	lCommonData.iStaticCondVar = &l_staticcondvar;

	retval = sem_init(&lSignalSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	retval = sem_init(&lSuspendSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	lThreadData.iSignalSemaphore = &lSignalSemaphore;
	lThreadData.iSuspendSemaphore = &lSuspendSemaphore;
	lThreadData.iTestSemaphore   = &lTestSemaphore;
	lThreadData.iTestMutex       = &lTestMutex;
	lThreadData.iTestMutexAttr   = &lTestMutexAttr;
	lThreadData.iTestCondVar     = &lTestCondVar;
	lThreadData.iDefaultAttr     = &defaultattr;
	lThreadData.iErrorcheckAttr = &errorcheckattr;
	lThreadData.iRecursiveAttr   = &recursiveattr;

	lThreadData.iCondAttr        = &lCondAttr;
	for (int loop = 0; loop < EThreadMain; loop++)
		{
	    g_spinFlag[loop] = true;
		}
	lThreadData.iSuspending      = false;
	lThreadData.iSpinCounter     = 0;
	lThreadData.iCurrentCommand  = -1;
	lThreadData.iSelf            = EThreadMain;
	lThreadData.iValue           = 0;
	lThreadData.iRetValue        = 0;
	lThreadData.ierrno           = 0;
	lThreadData.iExpectederrno   = 0;
	lThreadData.iTimes           = 0;
	lThreadData.iStopped         = false;
	lThreadData.iCommonData      = &lCommonData;
	
	retval = SemInit(&lThreadData);
	retval = SemPost(&lThreadData);
	
	fp=func7;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	fp=func8;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	retval = WaitForSignal(&lThreadData);
	
	retval = StopSpin(&lThreadData, (void*) EThread2);
	SemPostAsNeeded(&lThreadData);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	
	
	retval = SemDestroy(&lThreadData);
	StopThread(&lThreadData);
	
	err = pthread_cond_destroy(&l_staticcondvar);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_recursivemutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_errorcheckmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_staticmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutexattr_destroy(&recursiveattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&errorcheckattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&defaultattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = sem_destroy(&lSignalSemaphore);
	if(err != EINVAL)
		{	
		errsum += err;
		}
	err = sem_destroy(&lSuspendSemaphore);
	if(err != EINVAL)
		{
		errsum += err;
		}
	return retval+errsum;
	}


//race between sem_post and sem_timedwait
TInt CTestSempost::TestSem698()
	{
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	pthread_mutex_t lTestMutex;
	pthread_cond_t  lTestCondVar;
	pthread_condattr_t  lCondAttr;
	pthread_mutexattr_t lTestMutexAttr;

	pthread_mutexattr_t defaultattr;
	pthread_mutexattr_t errorcheckattr;
	pthread_mutexattr_t recursiveattr;

	pthread_mutexattr_init(&defaultattr);
	pthread_mutexattr_init(&errorcheckattr);
	pthread_mutexattr_init(&recursiveattr);

	pthread_mutexattr_settype(&errorcheckattr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutexattr_settype(&recursiveattr,PTHREAD_MUTEX_RECURSIVE);


	pthread_mutex_t l_staticmutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t l_errorcheckmutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	pthread_mutex_t l_recursivemutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_cond_t  l_staticcondvar = PTHREAD_COND_INITIALIZER;


    CommonData lCommonData;
    lCommonData.iStaticMutex = &l_staticmutex;
	lCommonData.iErrorCheckMutex = &l_errorcheckmutex;
	lCommonData.iRecursiveMutex = &l_recursivemutex;
	lCommonData.iStaticCondVar = &l_staticcondvar;

	retval = sem_init(&lSignalSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	retval = sem_init(&lSuspendSemaphore,0,0);
	if(retval != 0)
		{
		return retval;
		}

	lThreadData.iSignalSemaphore = &lSignalSemaphore;
	lThreadData.iSuspendSemaphore = &lSuspendSemaphore;
	lThreadData.iTestSemaphore   = &lTestSemaphore;
	lThreadData.iTestMutex       = &lTestMutex;
	lThreadData.iTestMutexAttr   = &lTestMutexAttr;
	lThreadData.iTestCondVar     = &lTestCondVar;
	lThreadData.iDefaultAttr     = &defaultattr;
	lThreadData.iErrorcheckAttr = &errorcheckattr;
	lThreadData.iRecursiveAttr   = &recursiveattr;

	lThreadData.iCondAttr        = &lCondAttr;
	for (int loop = 0; loop < EThreadMain; loop++)
		{
	    g_spinFlag[loop] = true;
		}
	lThreadData.iSuspending      = false;
	lThreadData.iSpinCounter     = 0;
	lThreadData.iCurrentCommand  = -1;
	lThreadData.iSelf            = EThreadMain;
	lThreadData.iValue           = 0;
	lThreadData.iRetValue        = 0;
	lThreadData.ierrno           = 0;
	lThreadData.iExpectederrno   = 0;
	lThreadData.iTimes           = 0;
	lThreadData.iStopped         = false;
	lThreadData.iCommonData      = &lCommonData;
	
	retval = SemInit(&lThreadData);
		
	fp=func7;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	retval = WaitForSignal(&lThreadData);
	
	fp=func9;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	retval = SemPost(&lThreadData);
	
	retval = StopSpin(&lThreadData, (void*) EThread2);
	SemPostAsNeeded(&lThreadData);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	
	
	retval = SemDestroy(&lThreadData);
	StopThread(&lThreadData);

	err = pthread_cond_destroy(&l_staticcondvar);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_recursivemutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_errorcheckmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutex_destroy(&l_staticmutex);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = pthread_mutexattr_destroy(&recursiveattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&errorcheckattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
	err = pthread_mutexattr_destroy(&defaultattr);
	if(err != EINVAL)
		{
		errsum += err;
		}
    err = sem_destroy(&lSignalSemaphore);
	if(err != EINVAL)
		{	
		errsum += err;
		}
	err = sem_destroy(&lSuspendSemaphore);
	if(err != EINVAL)
		{
		errsum += err;
		}
	return retval+errsum;
	}
