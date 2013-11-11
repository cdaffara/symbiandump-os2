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
// Name        : tsemdestroycases.cpp
// Part of     : testsemdestroycases
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tsemdestroy.h"

#define KMAXTIMES 5


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

int SemDestroy(ThreadData* aThreadData)	
	{
	int retval;
	retval =  sem_destroy(aThreadData->iTestSemaphore);
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

//sem_destroy on a valid semaphore

TInt CTestSemdestroy::TestSem275( )
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
	retval = SemDestroy(&lThreadData);
	retval = VerifyResult(&lThreadData,0);
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

//sem_destroy on an invalid (already destroyed) semaphore
TInt CTestSemdestroy::TestSem276( )
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
	
	
	retval = SemDestroy(&lThreadData);
	retval = VerifyErrno(&lThreadData,EINVAL);
	retval = SemInit(&lThreadData);
	retval = SemDestroy(&lThreadData);
	retval = SemDestroy(&lThreadData);
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


//race between sem_destroy and sem_post

TInt CTestSemdestroy::TestSem277( )
	{
	
	/*
	TRACE("+TestSem277\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		{EThread1, ELoop, (void*) (int) ESemDestroy},
		{EThread2, ELoop, (void*) (int) ESemPost},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem277\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}



//race between  sem_destroy and sem_wait

TInt CTestSemdestroy::TestSem278( )
	{
	/*
	TRACE("+TestSem278\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		{EThreadMain, ESemPost},

		{EThread1, ELoop, (void*) (int) ESemDestroy},
		{EThread2, ELoop, (void*) (int) ESemWait},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem278\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}



//race between sem_destroy and sem_destroy

TInt CTestSemdestroy::TestSem649( )
	{
	/*
	TRACE("+TestSem649\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		
		{EThread1, ELoop, (void*) (int) ESemDestroy},
		{EThread2, ELoop, (void*) (int) ESemDestroy},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem649\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}


//race between sem_destroy and sem_getvalue

TInt CTestSemdestroy::TestSem703( )
	{
	/*
	TRACE("+TestSem703\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		
		{EThread1, ELoop, (void*) (int) ESemDestroy},
		{EThread2, ELoop, (void*) (int) ESemGetValue},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem703\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}



//race between sem_destroy and sem_trywait

TInt CTestSemdestroy::TestSem704( )
	{

	/*
	TRACE("+TestSem704\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		{EThreadMain, ESemPost},
		{EThread1, ELoop, (void*) (int) ESemDestroy},
		{EThread2, ELoop, (void*) (int) ESemTryWait},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem704\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}



//race between sem_destroy and sem_timedwait

TInt CTestSemdestroy::TestSem705( )
	{
	/*
	TRACE("+TestSem705\n");
	HarnessCommand lCommandArr[255] = 
		{
		{EThreadMain, ESemInit,NULL},
		
		{EThreadMain, EThreadCreate, (void*) EThread1},
		{EThreadMain, EThreadCreate, (void*) EThread2},

		{EThreadMain, ESemPost},
		{EThread1, ELoop, (void*) (int) ESemDestroy},
		{EThread2, ELoop, (void*) (int) ESemTimedWait},

		{EThread1, EStop},
		{EThread2, EStop},

		{EThreadMain, EThreadDestroy, (void*) EThread1},
		{EThreadMain, EThreadDestroy, (void*) EThread2},
		{EThreadMain, EStop},
		{ENoThread, ELastCommand},
		};

	TRACE("-TestSem705\n");
	return LoadHarness(lCommandArr);
	*/
	return KErrNone;
	}

