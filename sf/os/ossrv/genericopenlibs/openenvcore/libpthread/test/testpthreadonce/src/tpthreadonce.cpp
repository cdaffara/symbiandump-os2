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
// Name        : tpthreadonce.cpp
// Part of     : testpthreadonce
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tpthreadonce.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestPthreadonce::~CTestPthreadonce() 
	{ 
	}  

CTestPthreadonce::CTestPthreadonce(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestPthreadonce::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestPthreadonce::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestPthreadonce::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestFramework)
		{
   		INFO_PRINTF1(_L("TestFramework():"));
   		err = TestFramework();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   		
    if(TestStepName() == KTestOnce1682)
		{
   		INFO_PRINTF1(_L("TestOnce1682():"));
   		err = TestOnce1682();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestOnce324)
		{
   		INFO_PRINTF1(_L("TestOnce324():"));
   		err = TestOnce324();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KTestOnce325)
		{
   		INFO_PRINTF1(_L("TestOnce325():"));
   		err = TestOnce325();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KTestOnce326)
		{
   		INFO_PRINTF1(_L("TestOnce326():"));
   		err = TestOnce326();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KTestOnce327)
		{
   		INFO_PRINTF1(_L("TestOnce327():"));
   		err = TestOnce327();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KTestOnce328)
		{
   		INFO_PRINTF1(_L("TestOnce328():"));
   		err = TestOnce328();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KTestOnce329)
		{
   		INFO_PRINTF1(_L("TestOnce329():"));
   		err = TestOnce329();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	return TestStepResult(); 

	}
 
	
