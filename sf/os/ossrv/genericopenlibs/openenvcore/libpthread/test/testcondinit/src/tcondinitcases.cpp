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
// Name        : tcondinitcases.cpp
// Part of     : testcondinit
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tcondinit.h"

int CondInit(ThreadData* aThreadData)
	{
	int retval;
	retval = pthread_cond_init(aThreadData->iTestCondVar,NULL);	
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
int CondDestroy(ThreadData* aThreadData)
	{
	int retval=0;
	retval = pthread_cond_destroy(aThreadData->iTestCondVar);	
	return retval;
	}
int CondSignal(ThreadData* aThreadData)
	{
	int retval=0;
	retval = pthread_cond_signal(aThreadData->iTestCondVar);
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
int Cond403Init(ThreadData* aThreadData)
	{	
	int retval=0;	
	aThreadData->iInitTwiceData = new pthread_cond_t;
	memcpy(aThreadData->iInitTwiceData,aThreadData->iTestCondVar,sizeof(pthread_cond_t));
	retval = pthread_cond_init(aThreadData->iTestCondVar,NULL);
	return retval;
	}
void Cond403Destroy(ThreadData* aThreadData)
	{
	pthread_cond_destroy((pthread_cond_t*)aThreadData->iInitTwiceData);
	pthread_cond_t *ptr_cond = (pthread_cond_t*) aThreadData->iInitTwiceData;
	delete ptr_cond;
	ptr_cond = NULL;
	aThreadData->iInitTwiceData = NULL;
	}
	
//pthread_cond_init called on uninitialized cond var

TInt CTestCondinit::TestCond401( )
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
	retval = VerifyResult(&lThreadData,0);
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


//pthread_cond_init called on already initialized cond var

TInt CTestCondinit::TestCond403( )
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
	retval = Cond403Init(&lThreadData);
	retval = VerifyResult(&lThreadData,0);
	retval = CondDestroy(&lThreadData);
	Cond403Destroy(&lThreadData);
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


//race between pthread_cond_init and pthread_cond_int

TInt CTestCondinit::TestCond404( )
	{
	/*
	TRACE("+TestCond404\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ECondInit},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		{EThread1, ELoop, (void*) (int) ECondInit},
		{EThread2, ELoop, (void*) (int) ECondInit},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, ECondDestroy},
		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestCond404\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}


//statically  initialized pthread_cond_t variable is valid

TInt CTestCondinit::TestCond668( )
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
	
	retval = CondSignal(&lThreadData);
	retval = VerifyResult(&lThreadData,0);
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


//race of pthread_cond_init and pthread_cond_signal

TInt CTestCondinit::TestCond670( )
	{
	/*
	TRACE("+TestCond670\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ECondInit},
		{EThreadMain, EMutexInitNULL},

		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		{EThread1, ELoop, (void*) (int) ECondInit},
		{EThread2, ELoop, (void*) (int) ECondSignal},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EMutexDestroy},
		{EThreadMain, ECondDestroy},
		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestCond670\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}


//race of pthread_cond_init and pthread_cond_broadcast

TInt CTestCondinit::TestCond671( )
	{
	/*
	TRACE("+TestCond671\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ECondInit},
		{EThreadMain, EMutexInitNULL},

		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		{EThread1, ELoop, (void*) (int) ECondInit},
		{EThread2, ELoop, (void*) (int) ECondInit},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EMutexDestroy},
		{EThreadMain, ECondDestroy},
		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestCond671\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}


//race of pthread_cond_init and pthread_cond_wait
TInt CTestCondinit::TestCond672( )
	{
	/*
	TRACE("+TestCond672\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ECondInit},
		{EThreadMain, EMutexInitNULL},

		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		{EThread2, EMutexLock},
		{EThread1, ELoop, (void*) (int) ECondInit},
		{EThread2, ELoop, (void*) (int) ECondWait},

		{EThread2, EMutexUnlock},
		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EMutexDestroy},
		{EThreadMain, ECondDestroy},
		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestCond672\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}


//race of pthread_cond_init and pthread_cond_timedwait
TInt CTestCondinit::TestCond673( )
	{
	/*
	TRACE("+TestCond673\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ECondInit},
		{EThreadMain, EMutexInitNULL},

		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		{EThread2, EMutexLock},

		{EThread1, ELoop, (void*) (int) ECondInit},
		{EThread2, ELoop, (void*) (int) ECondTimedWait},

		{EThread2, EMutexUnlock},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EMutexDestroy},
		{EThreadMain, ECondDestroy},
		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestCond673\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}



