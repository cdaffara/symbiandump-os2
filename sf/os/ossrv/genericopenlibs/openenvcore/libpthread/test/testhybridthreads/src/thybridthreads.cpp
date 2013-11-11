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
// Name        : thybridthreads.cpp
// 
//

#include "thybridthreads.h"

CTestHybridThreads::~CTestHybridThreads() 
	{ 
	}  

CTestHybridThreads::CTestHybridThreads(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestHybridThreads::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EFail);
	return TestStepResult();
	}



TVerdict CTestHybridThreads::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTestHybridThreads::doTestStepL()
	{
	TVerdict err = EFail;
	if(TestStepName() == KTestpthreadjoin)
   		{
   	   	INFO_PRINTF1(_L("Testpthreadjoin():"));
   	   	err = Testpthreadjoin();
   	   	}
	if(TestStepName() == KTestpthread_setspecific)
	        {
	        INFO_PRINTF1(_L("Testpthread_setspecific():"));
	        err = Testpthread_setspecific();
	        }
	return err;

	}
 

