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
// Name        : tsemtrywaitcases.cpp
// Part of     : testsemtrywait
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tsemtrywait.h"


int (*fp)(void* arg);
int func(void* arg);
int func1(void* arg);
int func2(void* arg);
int func3(void* arg);
int func4(void* arg);

int SemTryWait(ThreadData* aThreadData)
	{
	int retval=0;
	retval = sem_trywait(aThreadData->iTestSemaphore);
	return retval;
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
int SemWait(ThreadData* aThreadData)
	{
	int retval;
	aThreadData->iSuspending = true;
	sem_post(aThreadData->iSuspendSemaphore);
	retval = sem_wait(aThreadData->iTestSemaphore);
	aThreadData->iSuspending = false;
	return retval;
	}
	
int SemTimedWait(ThreadData *aThreadData,struct timespec* abstime)
	{
	int retval;
	struct timespec defaulttimeout;
	struct timeval now;
	gettimeofday(&now,NULL);
	
	defaulttimeout.tv_sec = now.tv_sec + 10;
	defaulttimeout.tv_nsec = 0;
	aThreadData->iSuspending = true;
	sem_post(aThreadData->iSuspendSemaphore);
	if(!abstime)
		{
		abstime = &defaulttimeout;
		}
	retval = sem_timedwait(aThreadData->iTestSemaphore,abstime);
	aThreadData->iSuspending = false;
	return retval;
	}
	
int SemPost(ThreadData* aThreadData)
	{
	int retval=0;
	retval = sem_post(aThreadData->iTestSemaphore);
	return retval;
	}

int VerifyErrno(ThreadData* aThreadData,int verifyno)
	{
	int retval=0;
	if ( (verifyno == errno)  )
		{
		errno = 0;
		}
	else
		{
		#ifdef WINDOWS
			printf("Expected retval %d Seen %d Expected errno %d Seen %d\n",verifyno, aThreadData->iRetValue,aThreadData->iExpectederrno,aThreadData->ierrno);
		#else
			;
		#endif
		retval = KUnexpectedResult;
		}
 	aThreadData->iRetValue = 0;
	aThreadData->ierrno    = 0;
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
int WaitForSignal(ThreadData* aThreadData)
	{
	int retval;
	retval = sem_wait(aThreadData->iSignalSemaphore);
	return retval;
	}

int PostSignal(ThreadData* aThreadData)
	{
	int retval=0;
	retval = sem_post(aThreadData->iSignalSemaphore);
	return retval;
	}
int LoopCommand1(ThreadData* aThreadData)
	{	
	int j = 0,retval=0;
	struct timespec abstime;
	struct timeval now;
	gettimeofday(&now,NULL);
	
	abstime.tv_sec = now.tv_sec - 5;
	abstime.tv_nsec = now.tv_usec * 1000;
	
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
		retval = SemTimedWait(aThreadData,&abstime);
		retval = SemPost(aThreadData);
		}
	return retval;
	}
int LoopCommand2(ThreadData* aThreadData)
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

	retval = SemTryWait(pData);
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
	
	retval = SemTryWait(pData);
	retval= VerifyErrno(pData,EAGAIN);
	retval = PostSignal(pData);
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
	
	StopThread(pData);
	
	return retval;
	}

int func3(void* arg)
	{
	int retval=0;
	ThreadData* pData = (ThreadData*) arg;
	
	retval = LoopCommand1(pData);
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
	
	retval = LoopCommand2(pData);
	if(retval == 0)
		{
		StopThread(pData);
		}
	return retval;
	}
	
//sem_trywait called on an uninitialized semaphore
TInt CTestSemtrywait::TestSem388( )
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
	
	retval = SemTryWait(&lThreadData);
	retval= VerifyErrno(&lThreadData,EINVAL);
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


//sem_trywait called on a posted semaphore
TInt CTestSemtrywait::TestSem389( )
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
	
	retval = SemInit(&lThreadData);
	retval = SemPost(&lThreadData);
	
	fp=func;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	retval = WaitForSignal(&lThreadData);
	retval = ThreadDestroy(&lThreadData, (void*) EThread1);
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


//sem_trywait called on a non posted semaphore

TInt CTestSemtrywait::TestSem390( )
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

	retval = SemInit(&lThreadData);
	
	fp=func1;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	retval = WaitForSignal(&lThreadData);
	retval = SemDestroy(&lThreadData);
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


//race between sem_trywait and sem_timedwait on a posted semaphore

TInt CTestSemtrywait::TestSem391( )
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

	retval = SemInit(&lThreadData);
	
	fp=func2;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
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

//race between sem_trywait and sem_post on a non-posted semaphore

TInt CTestSemtrywait::TestSem392( )
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

	retval = SemInit(&lThreadData);
	
	fp=func2;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	fp=func4;
	retval = ThreadCreate(&lThreadData, (void*) EThread2);

	retval = SemPost(&lThreadData);
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


//race between sem_trywait and sem_trywait

TInt CTestSemtrywait::TestSem700( )
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

	retval = SemInit(&lThreadData);
	
	fp=func2;
	retval = ThreadCreate(&lThreadData, (void*) EThread1);
	
	fp=func2;
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

