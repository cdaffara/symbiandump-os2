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



#include "tpthread.h"
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <pthread.h>
#include <semaphore.h>

void* threadFun(void *);
void* threadwrite(void *);
void* threadread(void *);
void* threadappend(void *);
void* FileWriteThreadEntryPoint( void* aParam );
void* FileReadThreadEntryPoint( void* aParam );
void* ThreadEntryPoint(void* aParam);


CTestPThread::~CTestPThread() 
	{ 
	}  

CTestPThread::CTestPThread(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestPThread::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestPThread::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestPThread::doTestStepL()
	{
	int err;
   	if(TestStepName() == KPThreadBasicApisTest)
   		{
   	  	INFO_PRINTF1(_L("PThreadBasicApisTest():"));
   	   	err = PThreadBasicApisTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestMultiplePThread)
   		{
   	  	INFO_PRINTF1(_L("TestMultiplePThread():"));
   	   	err = TestMultiplePThread();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KSocketReadAndWritePThread)
   		{
   	   	INFO_PRINTF1(_L("SocketReadAndWritePThread():"));
   	   	err = SocketReadAndWritePThread();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KFileReadAndWritePThread)
   		{
   	   	INFO_PRINTF1(_L("FileReadAndWritePThread():"));
   	   	err = FileReadAndWritePThread();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KPThreadSchedTest)
   		{
   	   	INFO_PRINTF1(_L("PThreadSchedTest():"));
   	   	err = PThreadSchedTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KPThreadJoinTest)
   		{
   	   	INFO_PRINTF1(_L("PThreadJoinTest():"));
   	   	err = PThreadJoinTest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KPThreadExitMemLeak)
   		{
   	   	INFO_PRINTF1(_L("PThreadExitMemLeak():"));
   	   	err = PThreadExitMemLeak();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
		
   	return TestStepResult(); 

	}
 
