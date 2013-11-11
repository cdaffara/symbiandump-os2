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
// Name        : tseminitcases.cpp
// Part of     : testseminit
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tseminit.h"

TInt SemInit(ThreadData* aThreadData,semParam_t* semParam)
	{
	TInt retval=0;
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

int SemDestroy(ThreadData* aThreadData)	
	{
	int retval;
	retval =  sem_destroy(aThreadData->iTestSemaphore);
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

int Sem274Init(ThreadData* aThreadData)
	{
	int retval=0;
	aThreadData->iInitTwiceData = new sem_t;
	memcpy(aThreadData->iInitTwiceData,aThreadData->iTestSemaphore,sizeof(sem_t));
	retval = sem_init(aThreadData->iTestSemaphore,0,0);
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

void Sem274Destroy(ThreadData* aThreadData)
	{
	sem_destroy((sem_t*)aThreadData->iInitTwiceData);
	sem_t *ptr = (sem_t*)aThreadData->iInitTwiceData;
	delete ptr;
	ptr = NULL;
	aThreadData->iInitTwiceData = NULL;
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

int SemGetValue(ThreadData* aThreadData)
	{
	int retval=0;
	retval = sem_getvalue(aThreadData->iTestSemaphore,&(aThreadData->iValue));
	return retval;
	}
	
int CheckValue(ThreadData* aThreadData,int expectedResult)
	{
	int retval=0;
	if ( (expectedResult == aThreadData->iValue) )
		{
		retval = 0;
		}
	else
		{
		retval = KValueMismatch;
		}
	return retval;
	}

int SemWait(ThreadData* aThreadData)
	{
	int retval=0;
	aThreadData->iSuspending = true;
	sem_post(aThreadData->iSuspendSemaphore);
	retval = sem_wait(aThreadData->iTestSemaphore);
	aThreadData->iSuspending = false;
	return retval;
	}
	
TInt CTestSeminit::TestSem273( )
	{
	
	TRACE("+TestSem273\n");
	
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
	
	retval = SemInit(&lThreadData,NULL);
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



//calling sem_init twice on the same semaphore an already initialized
TInt CTestSeminit::TestSem274( )
	{
	TRACE("+TestSem274\n");
	
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
	
	retval = SemInit(&lThreadData,NULL);
	retval = Sem274Init(&lThreadData);
	retval = VerifyResult(&lThreadData,0);
	retval = SemDestroy(&lThreadData);
	Sem274Destroy(&lThreadData);
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



//race between sem_init and sem_init
TInt CTestSeminit::TestSem689( )
	{
	TRACE("+TestSem689\n");
/*	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		
		{EThread1, ELoop, (void*) (int) ESemInit},
		{EThread2, ELoop, (void*) (int) ESemInit},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem689\n");
	return LoadHarness(lCommandArr);*/
	return 0;
	}


//race between sem_init and sem_post

TInt CTestSeminit::TestSem690( )
	{
	TRACE("+TestSem690\n");
/*	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		
		{EThread1, ELoop, (void*) (int) ESemInit},
		{EThread2, ELoop, (void*) (int) ESemPost},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem690\n");
	return LoadHarness(lCommandArr);*/
	return 0;
	}


//race between sem_init and sem_wait

TInt CTestSeminit::TestSem691( )
	{
	TRACE("+TestSem691\n");
/*	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		{EThreadMain, ESemPost},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		
		{EThread1, ELoop, (void*) (int) ESemInit},
		{EThread2, ELoop, (void*) (int) ESemWait},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem691\n");
	return LoadHarness(lCommandArr);*/
	return 0;
	}


//race between sem_init and sem_timedwait

TInt CTestSeminit::TestSem692( )
	{
	TRACE("+TestSem692\n");
/*	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		{EThreadMain, ESemPost},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		
		{EThread1, ELoop, (void*) (int) ESemInit},
		{EThread2, ELoop, (void*) (int) ESemTimedWait},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem692\n");
	return LoadHarness(lCommandArr);*/
	return 0;
	}


//sem_init called with positive value for pshared argument

TInt CTestSeminit::TestSem693( )
	{
	struct semParam_t semParam;
		
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	
	semParam.pshared = 1;
	semParam.value = 0;
	
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
	
	retval = SemInit(&lThreadData,&semParam);
	retval = VerifyErrno(&lThreadData,ENOSYS);
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


//sem_init called with 1 as value argument

TInt CTestSeminit::TestSem694( )
	{
	struct semParam_t semParam;
	
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	
	semParam.pshared = 0;
	semParam.value = 1;
	
	
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
	
	retval = SemInit(&lThreadData,&semParam);
	retval = SemGetValue(&lThreadData);
	retval = CheckValue(&lThreadData,1);
	retval = SemWait(&lThreadData);
	retval = SemGetValue(&lThreadData);
	retval = CheckValue(&lThreadData,0);
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


//sem_init called with n as value argument

TInt CTestSeminit::TestSem695( )
	{
	struct semParam_t semParam;
	
	
	int retval = 0;
	int errsum=0, err = 0;
	ThreadData lThreadData;
	
	sem_t lSignalSemaphore;
	sem_t lSuspendSemaphore;

	sem_t           lTestSemaphore;
	
	semParam.pshared = 0;
	semParam.value = 4;
		
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
	
	retval = SemInit(&lThreadData,&semParam);
	retval = SemGetValue(&lThreadData);
	retval = CheckValue(&lThreadData,4);
	retval = SemWait(&lThreadData);
	retval = SemWait(&lThreadData);
	retval = SemWait(&lThreadData);
	retval = SemWait(&lThreadData);
	retval = SemGetValue(&lThreadData);
	retval = CheckValue(&lThreadData,0);
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



//race between sem_init and sem_trywait

TInt CTestSeminit::TestSem696( )
	{
	TRACE("+TestSem696\n");
/*	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		{EThreadMain, ESemPost},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		
		{EThread1, ELoop, (void*) (int) ESemInit},
		{EThread2, ELoop, (void*) (int) ESemTryWait},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem696\n");
	return LoadHarness(lCommandArr);*/
	return 0;
	}

