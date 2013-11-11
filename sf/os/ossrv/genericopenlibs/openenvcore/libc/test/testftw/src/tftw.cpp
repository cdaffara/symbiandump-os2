// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tftw.cpp
// 
//

#include "tftw.h"


CTestftw::~CTestftw() 
	{ 
	}  

CTestftw::CTestftw(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);	
	iParamCnt=0;	
	}

TVerdict CTestftw::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	iParamCnt=0;
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestftw::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestftw::doTestStepL()
	{
	int err;

	if(TestStepName() == KTestFtw)
		{
   		INFO_PRINTF1(_L("TestFtw():"));
   		err = TestFtw();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	if(TestStepName() == KTestFtw2)
		{
   		INFO_PRINTF1(_L("TestFtw2():"));
   		err = TestFtw2();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	if(TestStepName() == KTestFtw3)
		{
   		INFO_PRINTF1(_L("TestFtw3():"));
   		err = TestFtw3();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	if(TestStepName() == KTestFtw4)
		{
   		INFO_PRINTF1(_L("TestFtw4():"));
   		err = TestFtw4();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	if(TestStepName() == KTestFtw5)
		{
   		INFO_PRINTF1(_L("TestFtw5():"));
   		err = TestFtw5();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	return TestStepResult(); 

	}
 


