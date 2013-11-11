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
// Name        : tcondsignal.cpp
// Part of     : testcondsignal
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tcondsignal.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestCondsignal::~CTestCondsignal() 
	{ 
	}  

CTestCondsignal::CTestCondsignal(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestCondsignal::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestCondsignal::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestCondsignal::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestCond439)
   		{
   	  	INFO_PRINTF1(_L("TestCond439():"));
   	  	err = TestCond439();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond440)
   		{
   	   	INFO_PRINTF1(_L("TestCond440():"));
   	   	err = TestCond440();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond441)
   		{
   	   	INFO_PRINTF1(_L("TestCond441():"));
   	   	err = TestCond441();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond442)
   		{
   	   	INFO_PRINTF1(_L("TestCond442():"));
   	   	err = TestCond442();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond443)
   		{
   	   	INFO_PRINTF1(_L("TestCond443():"));
   	   	err = TestCond443();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond444)
   		{
   	   	INFO_PRINTF1(_L("TestCond444():"));
   	   	err = TestCond444();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond445)
   		{
   	   	INFO_PRINTF1(_L("TestCond445():"));
   	   	err = TestCond445();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond678)
   		{
   	   	INFO_PRINTF1(_L("TestCond678():"));
   	   	err = TestCond678();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond679)
   		{
   	   	INFO_PRINTF1(_L("TestCond679():"));
   	   	err = TestCond679();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond680)
   		{
   	   	INFO_PRINTF1(_L("TestCond680():"));
   	   	err = TestCond680();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	return TestStepResult(); 

	}
 
	
