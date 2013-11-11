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
// Name        : tsemtrywait.cpp
// Part of     : testsemtrywait
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tsemtrywait.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestSemtrywait::~CTestSemtrywait() 
	{ 
	}  

CTestSemtrywait::CTestSemtrywait(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSemtrywait::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSemtrywait::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSemtrywait::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestSem388)
   		{
   	   	INFO_PRINTF1(_L("TestSem388():"));
   	   	err = TestSem388();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem389)
   		{
   	   	INFO_PRINTF1(_L("TestSem389():"));
   	   	err = TestSem389();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem390)
   		{
   	   	INFO_PRINTF1(_L("TestSem390():"));
   	   	err = TestSem390();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem391)
   		{
   	   	INFO_PRINTF1(_L("TestSem391():"));
   	   	err = TestSem391();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem392)
   		{
   	   	INFO_PRINTF1(_L("TestSem392():"));
   	   	err = TestSem392();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem700)
   		{
   	   	INFO_PRINTF1(_L("TestSem700():"));
   	   	err = TestSem700();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	   	   	
	return TestStepResult(); 

	}
 
	
