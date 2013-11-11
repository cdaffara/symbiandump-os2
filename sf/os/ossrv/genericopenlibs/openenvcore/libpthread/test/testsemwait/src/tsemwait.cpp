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
// Name        : tsemwait.cpp
// Part of     : testsemwait
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tsemwait.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestSemwait::~CTestSemwait() 
	{ 
	}  

CTestSemwait::CTestSemwait(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSemwait::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSemwait::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSemwait::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestSem285)
   		{
   	   	INFO_PRINTF1(_L("TestSem285():"));
   	   	err = TestSem285();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem286)
   		{
   	   	INFO_PRINTF1(_L("TestSem286():"));
   	   	err = TestSem286();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem287)
   		{
   	   	INFO_PRINTF1(_L("TestSem287():"));
   	   	err = TestSem287();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem288)
   		{
   	   	INFO_PRINTF1(_L("TestSem288():"));
   	   	err = TestSem288();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem289)
   		{
   	   	INFO_PRINTF1(_L("TestSem289():"));
   	   	err = TestSem289();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem290)
   		{
   	   	INFO_PRINTF1(_L("TestSem290():"));
   	   	err = TestSem290();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem699)
   		{
   	   	INFO_PRINTF1(_L("TestSem699():"));
   	   	err = TestSem699();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	   	   	
	return TestStepResult(); 

	}
 
	
