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
// Name        : tconddestroy.cpp
// Part of     : testconddestroy
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tconddestroy.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestConddestroy::~CTestConddestroy() 
	{ 
	}  

CTestConddestroy::CTestConddestroy(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestConddestroy::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestConddestroy::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestConddestroy::doTestStepL()
	{
	int err;

	if(TestStepName() == KTestCond447)
   		{
   	   	INFO_PRINTF1(_L("TestCond447():"));
   	   	err = TestCond447();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond448)
   		{
   	   	INFO_PRINTF1(_L("TestCond448():"));
   	   	err = TestCond448();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond449)
   		{
   	   	INFO_PRINTF1(_L("TestCond449():"));
   	   	err = TestCond449();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond450)
   		{
   	   	INFO_PRINTF1(_L("TestCond450():"));
   	   	err = TestCond450();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond451)
   		{
   	   	INFO_PRINTF1(_L("TestCond451():"));
   	   	err = TestCond451();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCond651)
   		{
   	   	INFO_PRINTF1(_L("TestCond651():"));
   	   	err = TestCond651();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond674)
   		{
   	   	INFO_PRINTF1(_L("TestCond674():"));
   	   	err = TestCond674();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	return TestStepResult(); 
	}
 
	
