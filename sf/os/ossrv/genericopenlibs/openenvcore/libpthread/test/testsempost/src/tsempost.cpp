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
// Name        : tsempost.cpp
// Part of     : testsempost
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tsempost.h"

CTestSempost::~CTestSempost() 
	{ 
	}  

CTestSempost::CTestSempost(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSempost::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestSempost::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSempost::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestSem280)
   		{
   	   	INFO_PRINTF1(_L("TestSem280():"));
   	   	err = TestSem280();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == KTestSem281)
   		{
   	   	INFO_PRINTF1(_L("TestSem281():"));
   	   	err = TestSem281();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem282)
   		{
   	   	INFO_PRINTF1(_L("TestSem282():"));
   	   	err = TestSem282();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem283)
   		{
   	   	INFO_PRINTF1(_L("TestSem283():"));
   	   	err = TestSem283();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem284)
   		{
   	   	INFO_PRINTF1(_L("TestSem284():"));
   	   	err = TestSem284();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem697)
   		{
   	   	INFO_PRINTF1(_L("TestSem697():"));
   	   	err = TestSem697();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem698)
   		{
   	   	INFO_PRINTF1(_L("TestSem698():"));
   	   	err = TestSem698();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	return EPass;

	}
 
