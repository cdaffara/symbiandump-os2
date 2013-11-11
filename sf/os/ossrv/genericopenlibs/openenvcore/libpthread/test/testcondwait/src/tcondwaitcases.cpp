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
// Name        : tcondwaitcases.cpp
// Part of     : testcondwait
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tcondwait.h"

int (*fp)(void* arg);
int func(void* arg);
int func1(void* arg);
int func2(void* arg);
int func3(void* arg);
int func4(void* arg);

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
int CondDestroy(ThreadData* aThreadData)
	{
	int retval=0;
	retval = pthread_cond_destroy(aThreadData->iTestCondVar);	
	return retval;
	}
int MutexDestroy(ThreadData* aThreadData)
	{
	int retval = 0;
	retval = pthread_mutex_destroy(aThreadData->iTestMutex);	
	return retval;
	}
int MutexInitNULL(ThreadData* aThreadData)
	{
	int retval=0;
	retval = pthread_mutex_init(aThreadData->iTestMutex,NULL);
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
int CondBroadCast(ThreadData *aThreadData)
	{
	int retval=0;
	retval = pthread_cond_broadcast(aThreadData->iTestCondVar);
	return retval;
	}
int CondInit(ThreadData* aThreadData)
	{
	int retval;
	retval = pthread_cond_init(aThreadData->iTestCondVar,NULL);	
	return retval;
	}

int CondSignal(ThreadData* aThreadData)
	{
	int retval=0;
	retval = pthread_cond_signal(aThreadData->iTestCondVar);
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
	
	retval = MutexLock(pData);
	retval = CondWait(pData);
	retval = MutexUnlock(pData);
	StopThread(pData);	
	
	return retval;
	}
int func1(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = MutexLock(pData);
	retval = CondSignal(pData);
	retval = MutexUnlock(pData);
	StopThread(pData);	
	
	return retval;
	}
int func2(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = CondWait(pData);
	retval = VerifyResult(pData,EINVAL);
	StopThread(pData);	
	
	return retval;
	}
int func3(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = MutexLock(pData);
	retval = CondBroadCast(pData);
	retval = MutexUnlock(pData);
	StopThread(pData);	
	
	return retval;
	}

int func4(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = MutexLock(pData);
	retval = CondWait(pData);
	retval = CondSignal(pData);
	retval = MutexUnlock(pData);
	StopThread(pData);	
	
	return retval;
	}
	
//pthread_cond_wait called on statically initialized cond var

TInt CTestCondwait::TestCond405()
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
	(lThreadData.iTestMutex) = lThreadData.iCommonData->iRecursiveMutex;
	
	fp=func;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	WaitTillSuspended(&lThreadData, (void*) EThread1);
	
	fp=func1;
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


//pthread_cond_wait called on statically initialized mutex

TInt CTestCondwait::TestCond406()
	{
	/*
	TRACE("+TestCond406\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ECondInit},
		{EThreadMain, EMutexInitStaticRecursive},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThread1, EMutexLock},
		{EThread1, ECondWait},
		{EThreadMain, EWaitTillSuspended, (void*) EThread1},

		{EThreadMain, EThreadCreate, (void*) EThread2},
		{EThread2, EMutexLock},
		{EThread2, ECondSignal},
		{EThread2, EMutexUnlock},
		{EThread2, EStop},

		{EThread1, EMutexUnlock},
		{EThread1, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EMutexDestroy},
		{EThreadMain, ECondDestroy},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestCond406\n");
	return LoadHarness(lCommandArr);*/
	return 0;
	}


//pthread_cond_wait called on statically initialized cond var and mutex

TInt CTestCondwait::TestCond407()
	{
	/*
	TRACE("+TestCond407\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ECondInitStatic},
		{EThreadMain, EMutexInitStaticErrorCheck},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThread1, EMutexLock},
		{EThread1, ECondWait},
		{EThreadMain, EWaitTillSuspended, (void*) EThread1},

		{EThreadMain, EThreadCreate, (void*) EThread2},
		{EThread2, EMutexLock},
		{EThread2, ECondSignal},
		{EThread2, EMutexUnlock},
		{EThread2, EStop},

		{EThread1, EMutexUnlock},
		{EThread1, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EMutexDestroy},
		{EThreadMain, ECondDestroy},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestCond407\n");
	return LoadHarness(lCommandArr);*/
	return 0;
	}


//pthread_cond_wait called on invalid  cond var

TInt CTestCondwait::TestCond408()
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
	
	retval = MutexInitNULL(&lThreadData);
	retval = MutexLock(&lThreadData);
	retval = CondWait(&lThreadData);
	retval = VerifyResult(&lThreadData,EINVAL);
	retval = MutexUnlock(&lThreadData);
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


//pthread_cond_wait called on invalid mutex

TInt CTestCondwait::TestCond409()
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
	retval = CondWait(&lThreadData);
	retval = VerifyResult(&lThreadData,EINVAL);
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


//pthread_cond_wait called on not locked mutex

TInt CTestCondwait::TestCond410()
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
	
	fp=func2;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
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



//pthread_cond_wait called after a call to  pthread_cond_signal

TInt CTestCondwait::TestCond411()
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
	retval = MutexInitNULL(&lThreadData);
	
	fp=func;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	WaitTillSuspended(&lThreadData, (void*) EThread1);
	
	fp=func1;
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

//pthread_cond_wait called before a call to  pthread_cond_signal

TInt CTestCondwait::TestCond413()
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
	retval = MutexInitNULL(&lThreadData);
	
	fp=func;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	WaitTillSuspended(&lThreadData, (void*) EThread1);
	
	fp=func1;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);
	
	retval = MutexLock(&lThreadData);
	retval = CondSignal(&lThreadData);
	retval = MutexUnlock(&lThreadData);
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


//pthread_cond_wait called after a call to  pthread_cond_broadcast

TInt CTestCondwait::TestCond414()
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
	retval = MutexInitNULL(&lThreadData);
	
	fp=func;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	WaitTillSuspended(&lThreadData, (void*) EThread1);
	
	fp=func3;
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


//pthread_cond_wait called before a call to  pthread_cond_broadcast

TInt CTestCondwait::TestCond415()
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
	retval = MutexInitNULL(&lThreadData);
	
	fp=func;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	WaitTillSuspended(&lThreadData, (void*) EThread1);
	
	fp=func3;
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

//race between pthread_cond_wait and pthread_cond_wait

TInt CTestCondwait::TestCond681()
	{
	/*
	TRACE("+TestCond681\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ECondInit},
		{EThreadMain, EMutexInitStaticErrorCheck},

		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},
		{EThreadMain, EThreadCreate, (void*) EThread3},
		
		{EThread1, ECond681A},
		{EThread2, ECond681B},
		{EThread3, ECond681B},

		{EThread1, EPostSignal},
		{EThread2, EPostSignal},
		{EThread3, EPostSignal},

		{EThread1, EStop},
		{EThread2, EStop},
		{EThread3, EStop},


		{EThreadMain, EWaitForSignal},
		{EThreadMain, EStopSpin, (void*) EThread2},
		{EThreadMain, EStopSpin, (void*) EThread3},
		{EThreadMain, ECondBroadcast},
		{EThreadMain, EWaitForSignal},
		{EThreadMain, EWaitForSignal},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EThreadDestroy, (void*) EThread3},

		{EThreadMain, ECondDestroy},
		{EThreadMain, EMutexDestroy},

		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestCond681\n");
	return LoadHarness(lCommandArr);*/
	return KErrNone;
	}


//race between pthread_cond_wait and pthread_cond_timedwait

TInt CTestCondwait::TestCond682()
	{
	/*
	TRACE("+TestCond682\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ECondInit},
		{EThreadMain, EMutexInitStaticErrorCheck},

		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},
		{EThreadMain, EThreadCreate, (void*) EThread3},
		
		{EThread1, ECond681A},
		{EThread2, ECond681B},
		{EThread3, ECond683B},

		{EThread1, EPostSignal},
		{EThread2, EPostSignal},
		{EThread3, EPostSignal},

		{EThread1, EStop},
		{EThread2, EStop},
		{EThread3, EStop},


		{EThreadMain, EWaitForSignal},
		{EThreadMain, EStopSpin, (void*) EThread2},
		{EThreadMain, EStopSpin, (void*) EThread3},
		{EThreadMain, ECondBroadcast},
		{EThreadMain, EWaitForSignal},
		{EThreadMain, EWaitForSignal},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EThreadDestroy, (void*) EThread3},

		{EThreadMain, ECondDestroy},
		{EThreadMain, EMutexDestroy},

		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestCond682\n");
	return LoadHarness(lCommandArr);*/
	return KErrNone;
	}

/*
 * Test the pthread_cond_timedwait() function for 
 * small timeouts.
 */  
TInt CTestCondwait::TestCondTWait1()
	{
	int delay = 1; //time out in milliseconds.
	
	//Try the timed wait 10 times.
	for(int i=0; i < 10; i++)
		{
		pthread_mutex_t mutex;
		pthread_cond_t cond; 
		
		pthread_mutex_init(&mutex, NULL);
	    pthread_cond_init(&cond, NULL);
	   	                
        //Calculate the absolute timeout
	   	struct timeval currTime;
	   	gettimeofday(&currTime, 0);
	   	
	   	timespec tSpec;           
        tSpec.tv_nsec = (currTime.tv_usec * 1000) + (delay * 1000 * 1000); 
        tSpec.tv_sec = currTime.tv_sec;
	   	       
        pthread_mutex_lock(&mutex);
	   	int err = pthread_cond_timedwait(&cond, &mutex, &tSpec);
	   	pthread_mutex_unlock(&mutex);
	   	
	   	pthread_cond_destroy(&cond);
		pthread_mutex_destroy(&mutex);
		   	
	   	if(err != ETIMEDOUT)
	   		{
	   		ERR_PRINTF1(_L("pthread_cond_timedwait() did not time out"));
	   		return KErrGeneral;
	   		}
	    }
	
	return KErrNone;
	}
