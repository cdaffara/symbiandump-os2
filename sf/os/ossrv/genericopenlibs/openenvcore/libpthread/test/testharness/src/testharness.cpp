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



#include <stddef.h>
#include <limits.h>
#include <e32def.h>
#include <e32std.h>
#include <stdlib.h>
#include <errno.h>
#include <e32base.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "testharness.h"
#include "testframeworkport.h"

#define KMAXTIMES 5

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
//
// DLL entry point
//
	{
	return KErrNone;
	}
#endif

static bool g_spinFlag[EThreadMain] = 
	{
    true,
    true,
    true,
    true,
    true
	};

void CopyArr(HarnessCommand* aDstArr, HarnessCommand* aSrcArr)
	{
	int i = 0;
	while(aSrcArr[i].iAction != ELastCommand)
		{
		aDstArr[i] = aSrcArr[i];
		i++;
		}
	aDstArr[i] = aSrcArr[i];
	}


EXPORT_C int LoadHarnessActual(HarnessCommand* aCommandArr,CTestModuleBase* aBase)
	{
	int retval = 0;
	ThreadData lThreadData;


	HarnessCommand *lHeapCommandArr;
	lHeapCommandArr = new HarnessCommand[255];
	CopyArr(lHeapCommandArr,aCommandArr);


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
	lCommonData.iTestModuleBase = aBase;



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
	lThreadData.iCommandArr      = lHeapCommandArr;
	lThreadData.iCurrentCommand  = -1;
	lThreadData.iSelf            = EThreadMain;
	lThreadData.iValue           = 0;
	lThreadData.iRetValue        = 0;
	lThreadData.ierrno           = 0;
	lThreadData.iExpectederrno   = 0;
	lThreadData.iTimes           = 0;
	lThreadData.iStopped         = false;
	lThreadData.iCommonData      = &lCommonData;

	retval = ProcessCommands(&lThreadData);

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
	delete [] lHeapCommandArr;

	return retval+errsum;

	}

void* StartFn(void* arg)
	{
	ThreadData* pData = (ThreadData*) arg;
	return (void*)ProcessCommands(pData);
	}


int ProcessCommands(ThreadData* aThreadData)
	{
	int retval = 0;
	int lIndex;
	if(!aThreadData)
		{
		return -1;
		}

	lIndex = ++(aThreadData->iCurrentCommand);
	HarnessCommand lCommand = aThreadData->iCommandArr[lIndex];
	while(lCommand.iAction != ELastCommand)
		{
		if(lCommand.iThread == aThreadData->iSelf)
			{
			//Added one more check : 
			if((lCommand.iAction != EVerifyResult && lCommand.iAction != EVerifyErrno && lCommand.iAction !=EVerifyTimeOutOrPostSignal) && (aThreadData->iRetValue != 0 || aThreadData->ierrno != 0))
			//Added one more cheque for the lCommand.iAction != EVerifyTimeOutOrPostSignal
				{

				#if 0
				#ifdef WINDOWS
					printf("Thread: %d retval: %d errno: %d CurrentCommand: %d\n",aThreadData->iSelf,aThreadData->iRetValue, aThreadData->ierrno, aThreadData->iCurrentCommand);
				#else
					;
				#endif

				#endif

				retval = KPrematureError;
				break;
				}
			else if (lCommand.iAction == EStop)
				{
				#ifdef WINDOWS
					printf("Command Processing Ended thread %d\n",aThreadData->iSelf);
				#else
					;
				#endif
				if(aThreadData->iSelf != EThreadMain)
					{
					aThreadData->iStopped = true;
					sem_post(aThreadData->iSuspendSemaphore);
					#ifdef USE_RTHREAD
						User::Exit(KErrNone);
					#endif
					}
				break;
				}
			else
				{
				aThreadData->iTimes = 0;
				aThreadData->iRetValue = RunCommand(aThreadData,lCommand);
			    aThreadData->ierrno    = errno;
				}
			}//self
		lIndex = ++(aThreadData->iCurrentCommand);
		lCommand = aThreadData->iCommandArr[lIndex];
		}//while
	return retval;
	}


int RunCommand1(ThreadData* aThreadData, HarnessCommand aCommand);
int RunCommand2(ThreadData* aThreadData, HarnessCommand aCommand);

int RunCommand(ThreadData* aThreadData, HarnessCommand aCommand)
	{
	int retval;
	#ifdef HARNESS_LOG
		aThreadData->iCommonData->BeforeRun(aCommand);
	#endif
	if(aCommand.iAction < EMutexDestroy)
		{
		retval = RunCommand1(aThreadData,aCommand);
		}
	else
		{
		retval = RunCommand2(aThreadData,aCommand);
		}
	#ifdef HARNESS_LOG
		aThreadData->iCommonData->AfterRun(aCommand);
	#endif
	return retval;

	}



int RunCommand1(ThreadData* aThreadData, HarnessCommand aCommand)
	{
	int retval = 0;
	int prevValue;
	int i;
	ThreadData* lTarget;
	semParam_t* semParam;
	struct timespec* abstime;
	struct timespec defaulttimeout;
	struct timeval now;
	gettimeofday(&now,NULL);
	defaulttimeout.tv_sec = now.tv_sec + 10;
	defaulttimeout.tv_nsec = 0;

	HarnessThread threadId;
	ThreadCommand loopCommand;
	int expectedResult;
	int signalsEaten =0;
	abstime = &defaulttimeout;

	switch(aCommand.iAction)
		{
		case ELastCommand:
			retval = KIllegalCommand;
			break;

		case EThreadCreate:
			if(aThreadData->iSelf != EThreadMain)
				{
				retval = KNoPermission;
				break;
				}
			threadId = (HarnessThread)(int) aCommand.iArgument;
			retval = NewThread(aThreadData,threadId);
			break;

		case EThreadDestroy:
			if(aThreadData->iSelf != EThreadMain)
				{
				retval = KNoPermission;
				break;
				}
			threadId = (HarnessThread)(int) aCommand.iArgument;
			retval = DeleteThread(aThreadData,threadId);
			break;

		case EVerifyResult:
			expectedResult = (int) aCommand.iArgument;
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
			break;

		case ESetErrno:
			errno =  (int) aCommand.iArgument;
			break;

		case EVerifyErrno:
			expectedResult = (int) aCommand.iArgument;
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
			break;

		case ECheckValue:
			expectedResult = (int) aCommand.iArgument;
			if ( (expectedResult == aThreadData->iValue) )
				{
				retval = 0;
				}
			else
				{
				retval = KValueMismatch;
				}
 			break;

		case EVerifyTimeOutOrPostSignal:
			if ( (ETIMEDOUT == aThreadData->iRetValue) || (aThreadData->iRetValue == -1 && aThreadData->ierrno == ETIMEDOUT) )
				{
				aThreadData->iRetValue = 0;
				aThreadData->ierrno    = 0;
				errno = 0;
				}
			else
				{
				retval = sem_post(aThreadData->iSignalSemaphore);
				}
			retval = 0;
 			break;

		case EStartLog:
		case EStopLog:
			break;

		case ENoOp:
			break;


		case ELoop:
			loopCommand = (ThreadCommand)(int) aCommand.iArgument;
			Loop(aThreadData, loopCommand);
			break;

		case EWaitForSignal:
			retval = sem_wait(aThreadData->iSignalSemaphore);
			break;

		case EPostSignal:
			retval = sem_post(aThreadData->iSignalSemaphore);
			break;

		case EWaitTillSuspended:
			threadId = (HarnessThread) (int) aCommand.iArgument;
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
			break;

		case EBusySpin:
			aThreadData->iSpinCounter = 0;
			while(g_spinFlag[aThreadData->iSelf])
				{
				aThreadData->iSpinCounter++;
				}
			break;

		case EVerifySpin:
		case EVerifyNoSpin:
			if(aThreadData->iSelf != EThreadMain)
				{
				retval = KNoPermission;
				break;
				}
			threadId = (HarnessThread) (int) aCommand.iArgument;
			lTarget = NULL;
			if(threadId >=0 && threadId < EThreadMain)
				{
				lTarget = aThreadData->iTDArr[threadId];
				}
			prevValue = 0;
			if(lTarget)
				{
				prevValue = lTarget->iSpinCounter;
				}
			for(i=0; i<255; i++)
				{
				;
				}
			if(prevValue != lTarget->iSpinCounter)
				{
				if(aCommand.iAction == EVerifySpin)
					{
					retval = 0;
					}
				else
					{
					retval = KSpinChanged;
					}
				}
			else
				{
				if(aCommand.iAction == EVerifyNoSpin)
					{	
					if (aThreadData->iTimes++ >= KMAXTIMES)
						{
						retval = 0;
						}
					else
						{
						RunCommand(aThreadData,aCommand);
						}
					}
				else
					{
					if (aThreadData->iTimes++ >= KMAXTIMES)
						{
						retval = KSpinNotChanged;
						}
					else
						{
						RunCommand(aThreadData,aCommand);
						}
					}
				}
			break;


		case EWaitTillSpin:
			threadId = (HarnessThread) (int) aCommand.iArgument;
			lTarget = NULL;
			if(threadId >=0 && threadId < EThreadMain)
				{
				lTarget = aThreadData->iTDArr[threadId];
				}
			if(lTarget)
				{
				while( ! (g_spinFlag[threadId]))
					{
					; //busy loop
					}
				}
			break;


		case EStopSpin:
			if(aThreadData->iSelf != EThreadMain)
				{
				retval = KNoPermission;
				break;
				}
			threadId = (HarnessThread) (int) aCommand.iArgument;
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
			break;

		case ESemDestroy:
			retval =  sem_destroy(aThreadData->iTestSemaphore);
			break;

		case ESemGetValue:
			retval = sem_getvalue(aThreadData->iTestSemaphore,&(aThreadData->iValue));
			break;

		case ESemInit:
			semParam = (semParam_t*) aCommand.iArgument;
			if(semParam)
				{
				retval = sem_init(aThreadData->iTestSemaphore,semParam->pshared,semParam->value);
				}
			else
				{
				retval = sem_init(aThreadData->iTestSemaphore,0,0);
				}
			break;

		case ESemOpen:
			retval = KNotSupported;
			break;

		case ESemPost:
			retval = sem_post(aThreadData->iTestSemaphore);
			break;

		case ESemTimedWait:
			abstime = (struct timespec*) aCommand.iArgument;
			aThreadData->iSuspending = true;
			sem_post(aThreadData->iSuspendSemaphore);
			if(!abstime)
				{
				abstime = &defaulttimeout;
				}
			retval = sem_timedwait(aThreadData->iTestSemaphore,abstime);
			aThreadData->iSuspending = false;
			break;
			// It is not used now...
		case ESemMicroSleepWait:
			aThreadData->iSuspending = true;
			sem_post(aThreadData->iSuspendSemaphore);
			aThreadData->iSuspending = false;
			break;

		case ESemTryWait:
			retval = sem_trywait(aThreadData->iTestSemaphore);
			break;

		case ESemWait:
			aThreadData->iSuspending = true;
			sem_post(aThreadData->iSuspendSemaphore);
			retval = sem_wait(aThreadData->iTestSemaphore);
			aThreadData->iSuspending = false;
			break;
		default:
			retval = KIllegalCommand;
			break;

		}//switch
	return retval;
	}

int RunCommand2(ThreadData* aThreadData, HarnessCommand aCommand)
	{
	int retval = 0;
	int i;
	int n;
	struct timespec* abstime;
	struct timespec defaulttimeout;
	struct timeval now;
	gettimeofday(&now,NULL);
	defaulttimeout.tv_sec = now.tv_sec + 10;
	defaulttimeout.tv_nsec = 0;
	pthread_mutexattr_t uninitializedmutexattribute;
	abstime = &defaulttimeout;
	void (*pOnceFn)(void) = NULL;
	char* filename = NULL;
	int fd = 0;
	FILE* file = NULL;
	struct stat stat_buf;

	switch(aCommand.iAction)
		{

		case EMutexDestroy:
			retval = pthread_mutex_destroy(aThreadData->iTestMutex);
			break;

		case EMutexInitNULL:
			retval = pthread_mutex_init(aThreadData->iTestMutex,NULL);
			break;

		case EMutexInitDefault:
			retval = pthread_mutex_init(aThreadData->iTestMutex,aThreadData->iDefaultAttr);
			break;

		case EMutexInitErrorCheck:
			retval = pthread_mutex_init(aThreadData->iTestMutex,aThreadData->iErrorcheckAttr);
			break;

		case EMutexInitRecursive:
			retval = pthread_mutex_init(aThreadData->iTestMutex,aThreadData->iRecursiveAttr);
			break;


		case EMutexInitStaticDefault:
			(aThreadData->iTestMutex) = aThreadData->iCommonData->iStaticMutex;
			break;

		case EMutexInitStaticErrorCheck:
			(aThreadData->iTestMutex) = aThreadData->iCommonData->iErrorCheckMutex;
			break;

		case EMutexInitStaticRecursive:
			(aThreadData->iTestMutex) = aThreadData->iCommonData->iRecursiveMutex;
			break;

		case EMutexLock:
			aThreadData->iSuspending = true;
			sem_post(aThreadData->iSuspendSemaphore);
			retval = pthread_mutex_lock(aThreadData->iTestMutex);
			aThreadData->iSuspending = false;
			break;

		case EMutexTimedLock:
			abstime = (struct timespec*) aCommand.iArgument;
			if(!abstime)
				{
				abstime = &defaulttimeout;
				}
			aThreadData->iSuspending = true;
			sem_post(aThreadData->iSuspendSemaphore);
			retval = pthread_mutex_timedlock(aThreadData->iTestMutex,abstime);
			aThreadData->iSuspending = false;
			break;

		case EMutexTryLock:
			retval = pthread_mutex_trylock(aThreadData->iTestMutex);
			break;

		case EMutexUnlock:
			retval = pthread_mutex_unlock(aThreadData->iTestMutex);
			break;

		case EMutexAttrDestroy:
			retval = pthread_mutexattr_destroy(aThreadData->iTestMutexAttr);
			break;

		case EMutexAttrGetType:
		case EMutexAttrGetPShared:
		case EMutexAttrSetType:
		case EMutexAttrSetPShared:
			retval = KNotSupported;
			break;

		case EMutexAttrInit:
			retval = pthread_mutexattr_init(aThreadData->iTestMutexAttr);

		case EOnceInit:
			aThreadData->iCommonData->iOnceControl = PTHREAD_ONCE_INIT;
			break;

		case ECallOnce:
			pOnceFn = (void (*)(void) ) aCommand.iArgument;
			retval = pthread_once(&(aThreadData->iCommonData->iOnceControl), pOnceFn);
			break;

		case EMicroSleepTime:
		case EErrno:
			retval = KNotSupported;
			break;

		case ECondBroadcast:
			retval = pthread_cond_broadcast(aThreadData->iTestCondVar);
			break;

		case ECondDestroy:
			retval = pthread_cond_destroy(aThreadData->iTestCondVar);
			break;

		case ECondInit:
			retval = pthread_cond_init(aThreadData->iTestCondVar,NULL);
			break;

		case ECondInitStatic:
			(aThreadData->iTestCondVar) = aThreadData->iCommonData->iStaticCondVar;
			break;


		case ECondSignal:
			retval = pthread_cond_signal(aThreadData->iTestCondVar);
			break;

		case ECondWait:
			aThreadData->iSuspending = true;
			sem_post(aThreadData->iSuspendSemaphore);
			retval = pthread_cond_wait(aThreadData->iTestCondVar,aThreadData->iTestMutex);
			aThreadData->iSuspending = false;
			break;

		case ECondTimedWait:
			abstime = (struct timespec*) aCommand.iArgument;
			if(abstime == NULL)
				{
				abstime = &defaulttimeout;
				}
			aThreadData->iSuspending = true;
			sem_post(aThreadData->iSuspendSemaphore);
			retval = pthread_cond_timedwait(aThreadData->iTestCondVar,aThreadData->iTestMutex, abstime);
			aThreadData->iSuspending = false;
			break;


		case ECondAttrDestroy:
		case ECondAttrInit:
			retval = KNotSupported;
			break;

		case ESem274Init:
			aThreadData->iInitTwiceData = new sem_t;
			memcpy(aThreadData->iInitTwiceData,aThreadData->iTestSemaphore,sizeof(sem_t));
			retval = sem_init(aThreadData->iTestSemaphore,0,0);
			break;

		case ESem274Destroy:
			{
			sem_destroy((sem_t*)aThreadData->iInitTwiceData);
			sem_t *ptr = (sem_t*)aThreadData->iInitTwiceData;
			delete ptr;
			ptr = NULL;
			aThreadData->iInitTwiceData = NULL;
			}
			break;

		case ESem282:
			retval = KNotSupported;
			break;

		case ESem284B:
			i =0;
			while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
				{
				retval = sem_wait(aThreadData->iTestSemaphore);
				i += 100;
				}
		//no one waiting
			break;

		case ESem697B:
			i =0;
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
		//no one waiting
			break;

		case ESem698B:
			i =0;
			while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
				{
				retval = sem_timedwait(aThreadData->iTestSemaphore,abstime);
				i += 100;
				}
		//no one waiting
			break;

		case ESemPostAsNeeded:
			sem_getvalue(aThreadData->iTestSemaphore,&n);
			n = -n;
			for(i =0; i<n; i++)
				{
				sem_post(aThreadData->iTestSemaphore);
				}
			break;

		case ESem701A:
			for(i =0; i< 1000; i +=100)
				{
				retval  += sem_post(aThreadData->iTestSemaphore);
				}
			break;

		case ESem701B:
			i =0;
			while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
				{
				int temp;
				gettimeofday(&now,NULL);
				defaulttimeout.tv_sec = now.tv_sec + 10;
				defaulttimeout.tv_nsec = 0;
	        
				temp = sem_timedwait(aThreadData->iTestSemaphore,abstime);
				if (temp != ETIMEDOUT)
					{
					retval += 0;
					errno = 0;
					}
				else 
					{
					retval += 0;
					errno = 0;
					}
				i += 100;
				}
		//no one waiting
			break;


		case EMutex330:
			retval = pthread_mutex_init(aThreadData->iTestMutex,&uninitializedmutexattribute);
			break;

		case ECond403Init:
			aThreadData->iInitTwiceData = new pthread_cond_t;
			memcpy(aThreadData->iInitTwiceData,aThreadData->iTestCondVar,sizeof(pthread_cond_t));
			retval = pthread_cond_init(aThreadData->iTestCondVar,NULL);
			break;

		case ECond403Destroy:
			{
			pthread_cond_destroy((pthread_cond_t*)aThreadData->iInitTwiceData);
			pthread_cond_t *ptr_cond = (pthread_cond_t*) aThreadData->iInitTwiceData;
			delete ptr_cond;
			ptr_cond = NULL;
			aThreadData->iInitTwiceData = NULL;
			}
			break;

		case ECond452A:
			for(i =0; i< 1000; i +=100)
				{
				retval  += pthread_cond_broadcast(aThreadData->iTestCondVar);
				}
			break;


		case ECond452B:
		case ECond453B:
		case ECond676B:
		case ECond679B:
		case ECond681B:
			i =0;
			while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
				{
				retval += pthread_mutex_lock(aThreadData->iTestMutex);
				retval += pthread_cond_wait(aThreadData->iTestCondVar,aThreadData->iTestMutex);
				retval += pthread_mutex_unlock(aThreadData->iTestMutex);
				i += 100;
				}
			//no one waiting
			break;

		case ECond453A:
		case ECond676A:
		case ECond677A:
		case ECond681A:
		case ECond683A:
			for(i =0; i< 1000; i +=100)
				{
				retval  += pthread_mutex_lock(aThreadData->iTestMutex);
				retval  += pthread_cond_broadcast(aThreadData->iTestCondVar);
				retval  += pthread_mutex_unlock(aThreadData->iTestMutex);
				}
			break;

		case ECond677B:
		case ECond680B:
		case ECond683B:
			i =0;
			while( (g_spinFlag[aThreadData->iSelf]) && (i < 1000))
				{
				int temp;
				retval += pthread_mutex_lock(aThreadData->iTestMutex);
		    
				gettimeofday(&now,NULL);
				defaulttimeout.tv_sec = now.tv_sec + 10;
				defaulttimeout.tv_nsec = 0;
	        
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
		//no one waiting
		
			break;

		case ECond679A:
		case ECond680A:
			for(i =0; i< 1000; i +=100)
			{
				retval  += pthread_mutex_lock(aThreadData->iTestMutex);
				retval  += pthread_cond_signal(aThreadData->iTestCondVar);
				retval  += pthread_mutex_unlock(aThreadData->iTestMutex);
			}
			break;

		case EPrintSuspendedStatus:
			if(aThreadData->iSelf != EThreadMain)
			{
				retval = KNoPermission;
				break;
			}
			for(i =0; i <EThreadMain; i++)
				{
				if(aThreadData->iTDArr[i]->iSuspending)
					{
					//MSG(_L("Thread %d is suspended\n"),i);
					}
				else
					{
					//MSG(_L("Thread %d is not suspended\n"),i);
					}
				}
			retval = 0;
 			break;

		case EStat:
			filename = (char*) aCommand.iArgument;
			retval = stat(filename,&stat_buf);
			if(retval == 0 &&stat_buf.st_size != 5)
				{
				retval = KErrGeneral;
				}
			break;
	
		case EFStat:
			fd = (int) aCommand.iArgument;
			retval = fstat(fd, &stat_buf);
			if(retval == 0 &&stat_buf.st_size != 5)
				{
				retval = KErrGeneral;
				}
			break;

		case EWriteName:
			filename = (char*) aCommand.iArgument;
			file = fopen(filename,"w");
			if(file)
				{
				fwrite("12345",1,5,file);
				fclose(file);
				}
			else
				{
				retval = KErrArgument;
				}
			break;

		case EWriteFd:
			fd = (int) aCommand.iArgument;
			write(fd,"12345",5);
			break;

		default:
			retval = KIllegalCommand;
			break;
		}//end switch

	return retval;
	}//fn

TInt _mrtEntryPtFun(TAny* aArg)
	{
	return (TInt) StartFn(aArg);
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
	newData->iCommandArr = aThreadData->iCommandArr;
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

int Loop(ThreadData* aData, ThreadCommand aCommand)
	{
	ThreadCommand Command1 = aCommand;
	ThreadCommand Command2 = ENoOp;

	void* argument1 = NULL;
	void* argument2 =NULL;
	struct semParam_t semParam;

	semParam.pshared = 0;
	semParam.value = 0;

	struct timespec abstime;
	struct timeval now;
	gettimeofday(&now,NULL);
	abstime.tv_sec  = now.tv_sec + 1;
	abstime.tv_nsec = 0;

	int retval = 0;
	switch(aCommand)
		{
		case ESemDestroy:
			argument2 = &semParam;
			Command2  = ESemInit;
			break;

		case ESemPost:
			Command2 = ESemWait;
			break;

		case ESemPost284:
			Command1 = ESemPost;
			Command2 = ESemPost;
			break;		

		case ESemWait:
			Command2 = ESemPost;
			break;

		case ESemTimedWait:
			argument1 = &abstime;
			Command2 = ESemPost;
			break;

		case ESemGetValue:
			Command2 = ENoOp;
			break;


		case EMutexDestroy:
			Command2 = EMutexInitNULL;
			break;

		case EMutexLock:
		case EMutexTryLock:
			Command2 = EMutexUnlock;
			break;

		case EMutexTimedLock:
			argument1 = &abstime;
			Command2 = EMutexUnlock;
			break;

		case ECondBroadcast:
			Command2 = ECondBroadcast;
			break;

		case ECondWait:
			Command2 = ECondWait;
			break;

		case ECondSignal:
			Command2 = ECondSignal;
			break;


		case ECondTimedWait:
			argument1 = &abstime;
			argument2 = &abstime;
			Command2 = ECondTimedWait;
			break;

		case ECondInit:
			Command2 = ECondDestroy;
			break;

		case ECondDestroy:
			Command2 = ECondInit;
			break;

		case ESem282:
			Command1 = ESemPost;
			Command2 = ESemPost;
			break;


		default:
			retval = -1;
			break;
		}

	if(retval != 0)
		{
		return retval;
		}


	HarnessCommand harnessCommand;
	int j = 0;
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
		harnessCommand.iThread = aData->iSelf;
		harnessCommand.iArgument = argument1;
		harnessCommand.iAction = Command1;
		RunCommand(aData, harnessCommand);
		harnessCommand.iThread = aData->iSelf;
		harnessCommand.iArgument = argument2;
		harnessCommand.iAction = Command2;
		retval = RunCommand(aData, harnessCommand);
		}
	return retval;
	}
