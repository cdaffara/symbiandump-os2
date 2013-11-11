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
// Name        : tsemdestroy.cpp
// Part of     : testsemdestroy
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tsemdestroy.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestSemdestroy::~CTestSemdestroy() 
	{ 
	}  

CTestSemdestroy::CTestSemdestroy(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSemdestroy::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSemdestroy::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSemdestroy::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestSem275)
   		{
   	   	INFO_PRINTF1(_L("TestSem275():"));
   	   	err = TestSem275();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestSem276)
   		{
   	   	INFO_PRINTF1(_L("TestSem276():"));
   	   	err = TestSem276();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestSem277)
   		{
   	   	INFO_PRINTF1(_L("TestSem277():"));
   	   	err = TestSem277();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestSem278)
   		{
   	   	INFO_PRINTF1(_L("TestSem278():"));
   	   	err = TestSem278();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem649)
   		{
   	   	INFO_PRINTF1(_L("TestSem649():"));
   	   	err = TestSem649();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem703)
   		{
   	   	INFO_PRINTF1(_L("TestSem703():"));
   	   	err = TestSem703();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem704)
   		{
   	   	INFO_PRINTF1(_L("TestSem704():"));
   	   	err = TestSem704();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestSem705)
   		{
   	   	INFO_PRINTF1(_L("TestSem705():"));
   	   	err = TestSem705();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	return TestStepResult(); 

	}
 
	
