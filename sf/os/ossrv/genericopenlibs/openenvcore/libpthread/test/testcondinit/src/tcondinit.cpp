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
// Name        : tcondinit.cpp
// Part of     : testcondinit
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



#include "tcondinit.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestCondinit::~CTestCondinit() 
	{ 
	}  

CTestCondinit::CTestCondinit(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestCondinit::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestCondinit::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestCondinit::doTestStepL()
	{
	int err;

	if(TestStepName() == KTestCond401)
   		{
   	   	INFO_PRINTF1(_L("TestCond401():"));
   	   	err = TestCond401();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond403)
   		{
   	   	INFO_PRINTF1(_L("TestCond403():"));
   	   	err = TestCond403();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond404)
   		{
   	   	INFO_PRINTF1(_L("TestCond404():"));
   	   	err = TestCond404();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond668)
   		{
   	   	INFO_PRINTF1(_L("TestCond668():"));
   	   	err = TestCond668();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond670)
   		{
   	   	INFO_PRINTF1(_L("TestCond670():"));
   	   	err = TestCond670();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond671)
   		{
   	   	INFO_PRINTF1(_L("TestCond671():"));
   	   	err = TestCond671();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond672)
   		{
   	   	INFO_PRINTF1(_L("TestCond672():"));
   	   	err = TestCond672();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == KTestCond673)
   		{
   	   	INFO_PRINTF1(_L("TestCond673():"));
   	   	err = TestCond673();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	   	   	
	return TestStepResult(); 

	}
 
	
