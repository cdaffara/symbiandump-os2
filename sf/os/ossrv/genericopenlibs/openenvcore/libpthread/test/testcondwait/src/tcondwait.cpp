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
// Name        : tcondwait.cpp
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
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestCondwait::~CTestCondwait() 
	{ 
	}  

CTestCondwait::CTestCondwait(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestCondwait::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestCondwait::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestCondwait::doTestStepL()
	{
	int err;

  	if(TestStepName() == KTestCond405)
   		{
   	   	INFO_PRINTF1(_L("TestCond405():"));
   	   	err = TestCond405();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else
   	   	if(TestStepName() == KTestCond406)
   		{
   	   	INFO_PRINTF1(_L("TestCond406():"));
   	   	err = TestCond406();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond407)
   		{
   	   	INFO_PRINTF1(_L("TestCond407():"));
   	   	err = TestCond407();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond408)
   		{
   	   	INFO_PRINTF1(_L("TestCond408():"));
   	   	err = TestCond408();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond409)
   		{
   	   	INFO_PRINTF1(_L("TestCond409():"));
   	   	err = TestCond409();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond410)
   		{
   	   	INFO_PRINTF1(_L("TestCond410():"));
   	   	err = TestCond410();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond411)
   		{
   	   	INFO_PRINTF1(_L("TestCond411():"));
   	   	err = TestCond411();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond413)
   		{
   	   	INFO_PRINTF1(_L("TestCond413():"));
   	   	err = TestCond413();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond414)
   		{
   	   	INFO_PRINTF1(_L("TestCond414():"));
   	   	err = TestCond414();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond415)
   		{
   	   	INFO_PRINTF1(_L("TestCond415():"));
   	   	err = TestCond415();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond681)
   		{
   	   	INFO_PRINTF1(_L("TestCond681():"));
   	   	err = TestCond681();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond682)
   		{
   	   	INFO_PRINTF1(_L("TestCond682():"));
   	   	err = TestCond682();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCondTWait1)
   		{
   	   	INFO_PRINTF1(_L("TestCondTWait1():"));
   	   	err = TestCondTWait1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	return TestStepResult(); 

	}
 
	
