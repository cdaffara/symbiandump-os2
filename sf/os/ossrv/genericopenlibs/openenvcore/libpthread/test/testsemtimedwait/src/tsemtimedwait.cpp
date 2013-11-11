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
// Name        : tsemtimedwait.cpp
// Part of     : testsemtimedwait
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tsemtimedwait.h"



CTestSemtimedwait::~CTestSemtimedwait() 
	{ 
	}  

CTestSemtimedwait::CTestSemtimedwait(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSemtimedwait::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSemtimedwait::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSemtimedwait::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestSem291)
   		{
   	   	INFO_PRINTF1(_L("TestSem291():"));
   	   	err = TestSem291();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestSem292)
   		{
   	   	INFO_PRINTF1(_L("TestSem292():"));
   	   	err = TestSem292();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem303)
   		{
   	   	INFO_PRINTF1(_L("TestSem303():"));
   	   	err = TestSem303();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem304)
   		{
   	   	INFO_PRINTF1(_L("TestSem304():"));
   	   	err = TestSem304();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem305)
   		{
   	   	INFO_PRINTF1(_L("TestSem305():"));
   	   	err = TestSem305();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem306)
   		{
   	   	INFO_PRINTF1(_L("TestSem306():"));
   	   	err = TestSem306();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem701)
   		{
   	   	INFO_PRINTF1(_L("TestSem701():"));
   	   	err = TestSem701();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem702)
   		{
   	   	INFO_PRINTF1(_L("TestSem702():"));
   	   	err = TestSem702();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	return TestStepResult(); 

	}
 
	
