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
// Name        : tcondbroadcast.cpp
// Part of     : testcondbroadcast
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tcondbroadcast.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestCondbroadcast::~CTestCondbroadcast() 
	{ 
	}  

CTestCondbroadcast::CTestCondbroadcast(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestCondbroadcast::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestCondbroadcast::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestCondbroadcast::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestCond452)
   		{
   		INFO_PRINTF1(_L("TestCond452():"));
   		err = TestCond452();
		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KTestCond453)
   		{
   		INFO_PRINTF1(_L("TestCond453():"));
   		err = TestCond453();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KTestCond454)
   		{
		INFO_PRINTF1(_L("TestCond454():"));
		err = TestCond454();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == KTestCond455)
   		{
		INFO_PRINTF1(_L("TestCond455():"));
   		err = TestCond455();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == KTestCond456)
   		{
   	   	INFO_PRINTF1(_L("TestCond456():"));
   	   	err = TestCond456();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond457)
   		{
   	   	INFO_PRINTF1(_L("TestCond457():"));
   	   	err = TestCond457();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond458)
   		{
   	   	INFO_PRINTF1(_L("TestCond458():"));
   	   	err = TestCond458();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond675)
   		{
   	   	INFO_PRINTF1(_L("TestCond675():"));
   	   	err = TestCond675();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond676)
   		{
   	   	INFO_PRINTF1(_L("TestCond676():"));
   	   	err = TestCond676();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond677)
   		{
   	   	INFO_PRINTF1(_L("TestCond677():"));
   	   	err = TestCond677();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	     	   	   	   	
	return TestStepResult(); 

	}
 
	
