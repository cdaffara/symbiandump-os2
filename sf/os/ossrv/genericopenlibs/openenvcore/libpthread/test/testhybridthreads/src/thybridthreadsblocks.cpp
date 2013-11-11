// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : thybridthreadsblocks.cpp
// Test cases for hybrid thread scenarios
//

#include <pthread.h>
#include "thybridthreads.h"

void* threadBMethod(void* aParams)
{
	RSemaphore bSem;
	int ret = KErrNone;
	
	aParams = aParams;
	ret = bSem.OpenGlobal(_L("HybridB"));
	if(ret != KErrNone) {
		pthread_exit((void*)-1);
		return 0;
	}
	
	bSem.Signal();

	sleep(10);
	bSem.Close();
	pthread_exit(0);
	return 0;
}

void* threadAMethod(void* aParams)
{
	pthread_t* threadBP;
	
	threadBP = (pthread_t*)aParams;	
	
	int ret = pthread_create(threadBP,NULL,threadBMethod,NULL);
	if(0 != ret){
		pthread_exit((void*)-1);
	}

	RSemaphore aSem;
	ret = aSem.OpenGlobal(_L("HybridA"));
	if(ret != KErrNone) {
		pthread_exit((void*)-1);
		return 0;
	}
	aSem.Wait();
	aSem.Close();
	
	pthread_exit(0);
	return 0;
}


// -----------------------------------------------------------------------------
// CTestHybridThreads::Testpthreadjoin
// 
// API tested: pthread_join()
// Description: Test case to verify pthread_join in a hybrid thread model
// -----------------------------------------------------------------------------

TVerdict CTestHybridThreads::Testpthreadjoin (  )
	{
	int 					ret = 0, SleepAmt = 0;
	pthread_t				threadB;
	
	SetTestStepResult(EFail);
	
	RSemaphore bSem;
	
	void* rptr = 0;
	ret = bSem.CreateGlobal(_L("HybridB"),0);
	if(ret != KErrNone) 
		{
		ERR_PRINTF1(_L("Unable to create semaphore"));
		bSem.Close();
		return TestStepResult();
		}
	
	RSemaphore aSem;
	RThread tmpThread;
	ret = aSem.CreateGlobal(_L("HybridA"),0);
	if(ret != KErrNone) 
		{
		ERR_PRINTF1(_L("Unable to create semaphore"));
		goto close;
		}
	
	ret = GetIntFromConfig(ConfigSection(), _L("SleepAmt"),SleepAmt);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the sleep amount from ini")) ;
		goto close;
		}


	
	ret = tmpThread.Create(_L("HybridThread"),reinterpret_cast<TThreadFunction>(threadAMethod),
								0x5000,NULL,(void*)&threadB);
	if(0 != ret)
		{
		ERR_PRINTF2(_L("Error in the creation of the threadA: %d"),ret);
		goto close;
		}
	
	tmpThread.Resume();

	// Wait for B to start
	bSem.Wait();

	INFO_PRINTF1(_L("Before pthread_join"));

	
	ret = pthread_join(threadB,&rptr);
	if(0 != ret){
		ERR_PRINTF2(_L("Error returned by pthread_join: %d"),ret);
		return TestStepResult();
	}
	
	INFO_PRINTF1(_L("After pthread_join"));

	aSem.Signal();

	INFO_PRINTF1(_L("Main thread exiting"));
  
	if(rptr && (rptr == (void*)-1))
		SetTestStepResult(EFail);
	else
		SetTestStepResult(EPass);
	
close:
	aSem.Close();
	bSem.Close();

	return TestStepResult();
	}


// -----------------------------------------------------------------------------
// CTestHybridThreads::Testpthread_setspecific
// 
// APIs tested: pthread_setspecific(), pthread_getspecific(), pthread_key_create()
// Description: Test case to verify pthread_setspecific in a hybrid application
// -----------------------------------------------------------------------------
void foo(void);  /* Functions that use the threadSpecific data */
void bar(void);
void dataDestructor(void *data);
typedef TInt(*TThreadFunction )(TAny*); 
static TInt theThread(TAny*);

typedef struct {
  int          threadSpecific1;
  int          threadSpecific2;
} threadSpecific_data_t;
 
#define                 NUMTHREADS   2
pthread_key_t           threadSpecificKey;
 
 
TInt theThread(TAny *parm)
{
   int               rc;
   threadSpecific_data_t    *gData;
   gData = (threadSpecific_data_t *)parm;
   rc = pthread_setspecific(threadSpecificKey, gData);
   rc = rc;
   foo();
   pthread_exit(0);
   return 0;
}
 
void foo() 
   {
   threadSpecific_data_t *gData = (threadSpecific_data_t*)pthread_getspecific(threadSpecificKey);
   bar();
}
 
void bar() {
   threadSpecific_data_t *gData = (threadSpecific_data_t*)pthread_getspecific(threadSpecificKey);
   return;
}
 
void dataDestructor(void *data) 
    {
    int rc;
    rc = pthread_setspecific(threadSpecificKey, NULL);
    rc = rc;
    threadSpecific_data_t    *gData = (threadSpecific_data_t    *)data;
    free(gData);
    }

TVerdict CTestHybridThreads::Testpthread_setspecific()
    {
    int                   rc=0;
    int                   i;
    threadSpecific_data_t        *gData;     
    rc = pthread_key_create(&threadSpecificKey, dataDestructor);
    if (!rc)
        {
        SetTestStepResult(EPass);
        }
    RThread lNewThread[NUMTHREADS];
    INFO_PRINTF1(_L("Create/start threads"));
    for (i=0; i <NUMTHREADS; ++i) 
        {
        /* Create per-thread threadSpecific data and pass it to the thread */
        gData = (threadSpecific_data_t *)malloc(sizeof(threadSpecific_data_t));
        gData->threadSpecific1 = i;
        gData->threadSpecific2 = (i+1)*2;
        lNewThread[i].Create(KNullDesC, // Thread Name
             theThread, // Entry pt function
            KDefaultStackSize,      // Stack Size
            NULL,          // Use common heap
            (TAny*)gData); // Args to entry pt function
        lNewThread[i].Resume();
        }
    INFO_PRINTF1(_L("Wait for the threads to complete, and release their resources"));
    INFO_PRINTF1(_L("Main completed"));
    return TestStepResult();
    }
