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
// Name        : tsemgetvalue.cpp
// Part of     : testsemgetvalue
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tsemgetvalue.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestSemgetvalue::~CTestSemgetvalue() 
	{ 
	}  

CTestSemgetvalue::CTestSemgetvalue(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSemgetvalue::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSemgetvalue::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSemgetvalue::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestSem313)
   		{
   	   	INFO_PRINTF1(_L("TestSem313():"));
   	   	err = TestSem313();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem314)
   		{
   	   	INFO_PRINTF1(_L("TestSem314():"));
   	   	err = TestSem314();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem315)
   		{
   	   	INFO_PRINTF1(_L("TestSem315():"));
   	   	err = TestSem315();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem316)
   		{
   	   	INFO_PRINTF1(_L("TestSem316():"));
   	   	err = TestSem316();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem317)
   		{
   	   	INFO_PRINTF1(_L("TestSem317():"));
   	   	err = TestSem317();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem318)
   		{
   	   	INFO_PRINTF1(_L("TestSem318():"));
   	   	err = TestSem318();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem319)
   		{
   	   	INFO_PRINTF1(_L("TestSem319():"));
   	   	err = TestSem319();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	return TestStepResult(); 

	}
 
	
