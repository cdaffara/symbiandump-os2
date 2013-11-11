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
// Name        : tcondbroadcastcases.cpp
// Part of     : testcondbroadcast
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tcondbroadcast.h"



#define KMAXTIMES 5

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
int func10(void* arg);
int func11(void* arg);
int func12(void* arg);
int func13(void* arg);
int func14(void* arg);
int func15(void* arg);
int func16(void* arg);
int func17(void* arg);

int CondInit(ThreadData* aThreadData)
	{
	int retval;
	retval = pthread_cond_init(aThreadData->iTestCondVar,NULL);	
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
	
int CondDestroy(ThreadData* aThreadData)
	{
	int retval=0;
	retval = pthread_cond_destroy(aThreadData->iTestCondVar);	
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

int MutexDestroy(ThreadData* aThreadData)
	{
	int retval = 0;
	retval = pthread_mutex_destroy(aThreadData->iTestMutex);	
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

int WaitForSignal(ThreadData* aThreadData)
	{
	return (sem_wait(aThreadData->iSignalSemaphore));
	}

int StopSpin(ThreadData* aThreadData,void* aThreadId)
	{
	int retval=0;
	ThreadData* lTarget;
	HarnessThread threadId;
	
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
int Cond452A(ThreadData *aThreadData)
	{
	int i,retval=0;
	for(i =0; i< 1000; i +=100)
		{
		retval  += pthread_cond_broadcast(aThreadData->iTestCondVar);
		}
	return retval;	
	}
int Cond453A(ThreadData *aThreadData)
	{
	int i,retval=0;
	for(i =0; i< 1000; i +=100)
		{
		retval  += pthread_mutex_lock(aThreadData->iTestMutex);
		retval  += pthread_cond_broadcast(aThreadData->iTestCondVar);
		retval  += pthread_mutex_unlock(aThreadData->iTestMutex);
		}
	return retval;
	}
int Cond453B(ThreadData *aThreadData)
	{
	int i,retval=0;
	i =0;
	while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
		{
		retval += pthread_mutex_lock(aThreadData->iTestMutex);
		retval += pthread_cond_wait(aThreadData->iTestCondVar,aThreadData->iTestMutex);
		retval += pthread_mutex_unlock(aThreadData->iTestMutex);
		i += 100;
		}
	return retval;
	}
int Cond676A(ThreadData *aThreadData)
	{
	int retval=0;
	retval = Cond453A(aThreadData);
	return retval;
	}
int Cond676B(ThreadData *aThreadData)
	{
	int retval=0;
	retval = Cond453B(aThreadData);
	return retval;
	}
int Cond677B(ThreadData *aThreadData)
	{
	int retval=0;	
	int i =0;
	struct timespec defaulttimeout;
	struct timeval now;
	struct timespec* abstime;
	
	while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
		{
		int temp;
		retval += pthread_mutex_lock(aThreadData->iTestMutex);
	   
		gettimeofday(&now,NULL);
		defaulttimeout.tv_sec = now.tv_sec + 10;
		defaulttimeout.tv_nsec = 0;
	    abstime = &defaulttimeout;
		temp = pthread_cond_timedwait(aThreadData->iTestCondVar,aThreadData->iTestMutex,abstime);
		if (temp != ETIMEDOUT)
			{
			retval += 0;
			}
		else 
			{
			retval += 0;
			}
		retval += pthread_mutex_unlock(aThreadData->iTestMutex);
		i += 100;
		}
	return retval;
	}
int PostSignal(ThreadData *aThreadData)
	{
	int retval;
	retval = sem_post(aThreadData->iSignalSemaphore);
	return retval;		
	}
int CondBroadCast(ThreadData *aThreadData)
	{
	int retval=0;
	retval = pthread_cond_broadcast(aThreadData->iTestCondVar);
	return retval;
	}
int VerifyResult(ThreadData* aThreadData,int expectedResult)
	{
	int retval=0;
	if ( (expectedResult == aThreadData->iRetValue) && (aThreadData->iExpectederrno == aThreadData->ierrno) )
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
		retval = 0;
		}
 	aThreadData->iRetValue = 0;
	aThreadData->ierrno    = 0;
	return retval;
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
int MutexLock(ThreadData* aThreadData)
	{
	int retval=0;
	aThreadData->iSuspending = true;
	sem_post(aThreadData->iSuspendSemaphore);
	retval = pthread_mutex_lock(aThreadData->iTestMutex);
	aThreadData->iSuspending = false;
	return retval;
	}
int MutexUnlock(ThreadData* aThreadData)
	{
	int retval=0;
	retval = pthread_mutex_unlock(aThreadData->iTestMutex);
	return retval;
	}
int CondWait(ThreadData* aThreadData)
	{
	int retval=0;
	aThreadData->iSuspending = true;
	sem_post(aThreadData->iSuspendSemaphore);
	retval = pthread_cond_wait(aThreadData->iTestCondVar,aThreadData->iTestMutex);
	aThreadData->iSuspending = false;
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
		retval = CondBroadCast(aThreadData);
		retval = CondBroadCast(aThreadData);
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
	retval = Cond452A(pData);
	if(retval == 0)
		{
		retval = PostSignal(pData);
		if(retval == 0)
			StopThread(pData);	
		}
	return retval;
	}

int func1(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = Cond453A(pData);
	if(retval == 0)
		{
		retval = PostSignal(pData);
		if(retval == 0)
			StopThread(pData);	
		}
	return retval;
	}

int func2(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = Cond453B(pData);
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
	retval = CondBroadCast(pData);
	retval = VerifyResult(pData,EINVAL);
	if(retval == 0)
		{
		retval = PostSignal(pData);
		if(retval == 0)
			StopThread(pData);	
		}
	return retval;
	}

int func4(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = CondBroadCast(pData);
	retval = MutexLock(pData);
	retval = PostSignal(pData);
	retval = CondWait(pData);
	retval = MutexUnlock(pData);
	if(retval == 0)
		{
		retval = PostSignal(pData);
		if(retval == 0)
			StopThread(pData);	
		}
	return retval;
	}

int func5(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = MutexLock(pData);
	retval = CondBroadCast(pData);
	retval = MutexUnlock(pData);
	StopThread(pData);	
		
	return retval;
	}

int func6(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = MutexLock(pData);
	retval = PostSignal(pData);
	retval = CondWait(pData);
	retval = MutexUnlock(pData);
	StopThread(pData);	
		
	return retval;
	}

int func7(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = MutexLock(pData);
	retval = CondBroadCast(pData);
	retval = MutexUnlock(pData);
	StopThread(pData);	
		
	return retval;
	}

int func8(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = CondBroadCast(pData);
	retval = VerifyResult(pData,0);
	StopThread(pData);	
		
	return retval;
	}

int func9(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = MutexLock(pData);
	retval = CondWait(pData);
	retval = MutexUnlock(pData);
	retval = PostSignal(pData);
	StopThread(pData);	
		
	return retval;
	}

int func10(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = MutexLock(pData);
	retval = CondWait(pData);
	retval = MutexUnlock(pData);
	retval = PostSignal(pData);
	StopThread(pData);	
		
	return retval;
	}
	
int func11(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = MutexLock(pData);
	retval = CondWait(pData);
	retval = MutexUnlock(pData);
	retval = PostSignal(pData);
	StopThread(pData);	
		
	return retval;
	}
	
int func12(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = MutexLock(pData);
	retval = CondWait(pData);
	retval = MutexUnlock(pData);
	retval = PostSignal(pData);
	StopThread(pData);	
		
	return retval;
	}

int func13(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = MutexLock(pData);
	retval = CondBroadCast(pData);
	retval = MutexUnlock(pData);
	StopThread(pData);	
		
	return retval;
	}

int func14(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = Cond676A(pData);
	retval = PostSignal(pData);
	StopThread(pData);	
		
	return retval;
	}

int func15(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = Cond676B(pData);
	retval = PostSignal(pData);
	StopThread(pData);	
		
	return retval;
	}

int func16(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = Cond453A(pData);
	retval = PostSignal(pData);
	StopThread(pData);	
		
	return retval;
	}

int func17(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	retval = Cond677B(pData);
	retval = PostSignal(pData);
	StopThread(pData);	
		
	return retval;
	}
int func18(void* arg)
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

//pthread_cond_broadcast is called without mutex locked
TInt CTestCondbroadcast::TestCond452()
	{
	int retval = 0;
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

	retval = CondInit(&lThreadData);
	if(retval != 0)
		{
		return retval;
		}
		
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	fp=func;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = CondDestroy(&lThreadData);
	retval = MutexDestroy(&lThreadData);
	StopThread(&lThreadData);
	
	int errsum=0, err = 0;
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


//pthread_cond_broadcast is called with mutex locked
TInt CTestCondbroadcast::TestCond453()
	{
	
	int errsum=0, err = 0;
	int retval = 0;
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
	
	retval = CondInit(&lThreadData);
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	
	fp=func1;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	retval = WaitForSignal(&lThreadData);
		
	fp=func2;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	retval = StopSpin(&lThreadData,(void *)EThread2);
	retval = CondBroadCast(&lThreadData);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = CondDestroy(&lThreadData);
	retval = MutexDestroy(&lThreadData);
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

//pthread_cond_broadcast is called on uninitialized cond var
TInt CTestCondbroadcast::TestCond454()
	{
	int errsum=0, err = 0;
	int retval = 0;
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
	
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	fp=func3;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	retval = WaitForSignal(&lThreadData);
	retval = MutexDestroy(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
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


//pthread_cond_broadcast called on statically initialized cond var
TInt CTestCondbroadcast::TestCond455()
	{
	int errsum=0, err = 0;
	int retval = 0;
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
	
	(lThreadData.iTestCondVar) = lThreadData.iCommonData->iStaticCondVar;
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	fp=func4;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	retval = WaitForSignal(&lThreadData);
	WaitTillSuspended(&lThreadData, (void*) EThread1);
	fp=func5;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = MutexDestroy(&lThreadData);
	retval = CondDestroy(&lThreadData);
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

//pthread_cond_broadcast is called with one thread suspended on cond var
TInt CTestCondbroadcast::TestCond456()
	{
	
	int errsum=0, err = 0;
	int retval = 0;
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
	
	retval = CondInit(&lThreadData);
	if(retval != 0)
		{
		return retval;
		}
		
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	fp=func6;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	retval = WaitForSignal(&lThreadData);
	WaitTillSuspended(&lThreadData, (void*) EThread1);
	fp=func7;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = MutexDestroy(&lThreadData);
	retval = CondDestroy(&lThreadData);
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

//pthread_cond_broadcast is called with no theards suspended on the cond var
TInt CTestCondbroadcast::TestCond457()
	{
	int errsum=0, err = 0;
	int retval = 0;
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
	
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	retval = CondInit(&lThreadData);
	fp=func8;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = CondDestroy(&lThreadData);
	retval = MutexDestroy(&lThreadData);
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

//pthread_cond_broadcast is called with n threads suspended on cond var
TInt CTestCondbroadcast::TestCond458()
	{
	int errsum=0, err = 0;
	int retval = 0;
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
	
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	retval = CondInit(&lThreadData);
	
	fp=func9;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	fp=func10;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	fp=func11;
	retval = ThreadCreate(&lThreadData, (void*) EThread3);
	
	fp=func12;
	retval = ThreadCreate(&lThreadData, (void*) EThread4);
	
	fp=func13;
	retval = ThreadCreate(&lThreadData, (void*) EThread5);
	
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = ThreadDestroy(&lThreadData, (void*) EThread3);
	retval = ThreadDestroy(&lThreadData, (void*) EThread4);
	retval = ThreadDestroy(&lThreadData, (void*) EThread5);
	retval = CondDestroy(&lThreadData);
	retval = MutexDestroy(&lThreadData);
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

//race between pthread_cond_broadcast and pthread_cond_broadcast
TInt CTestCondbroadcast::TestCond675()
	{
	
	int errsum=0, err = 0;
	int retval = 0;
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
	
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	retval = CondInit(&lThreadData);
	
	fp=func18;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	fp=func18;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	retval = WaitForSignal(&lThreadData);
	retval = WaitForSignal(&lThreadData);
	
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = MutexDestroy(&lThreadData);
	retval = CondDestroy(&lThreadData);
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


//race between pthread_cond_broadcast and pthread_cond_wait
TInt CTestCondbroadcast::TestCond676()
	{
	int errsum=0, err = 0;
	int retval = 0;
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
	
	retval = CondInit(&lThreadData);
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	
	
	fp=func14;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	retval = WaitForSignal(&lThreadData);
	
	fp=func15;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	retval = StopSpin(&lThreadData,(void *)EThread2);
	retval = CondBroadCast(&lThreadData);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = CondDestroy(&lThreadData);
	retval = MutexDestroy(&lThreadData);
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


//race between pthread_cond_broadcast and pthread_cond_timedwait
TInt CTestCondbroadcast::TestCond677()
	{
	int errsum=0, err = 0;
	int retval = 0;
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
	
	retval = CondInit(&lThreadData);
	lThreadData.iTestMutex = lThreadData.iCommonData->iErrorCheckMutex;
	
	fp=func16;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	retval = WaitForSignal(&lThreadData);
	
	fp=func17;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	retval = StopSpin(&lThreadData,(void *)EThread2);
	retval = CondBroadCast(&lThreadData);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
	retval = ThreadDestroy(&lThreadData, (void*) EThread2);
	retval = CondDestroy(&lThreadData);
	retval = MutexDestroy(&lThreadData);
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

