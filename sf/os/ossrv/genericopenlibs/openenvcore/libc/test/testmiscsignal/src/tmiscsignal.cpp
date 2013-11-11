// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tmiscsignal.cpp
// 
//

#include "tmiscsignal.h"

CTestMiscsignal::~CTestMiscsignal() 
	{ 
	}  

CTestMiscsignal::CTestMiscsignal(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestMiscsignal::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestMiscsignal::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTestMiscsignal::doTestStepL()
	{
	int err;
	if(TestStepName() == KTestpsignal1)
   		{
   	   	INFO_PRINTF1(_L("Testpsignal1():"));
   	   	err = Testpsignal1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestpsignal2)
   		{
   	   	INFO_PRINTF1(_L("Testpsignal2():"));
   	   	err = Testpsignal2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestcombinesignal1)
   		{
   	   	INFO_PRINTF1(_L("Testcombinesignal1():"));
   	   	err = Testcombinesignal1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestcombinesignal2)
   		{
   	   	INFO_PRINTF1(_L("Testcombinesignal2():"));
   	   	err = Testcombinesignal2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestcombinesignal3)
   		{
   	   	INFO_PRINTF1(_L("Testcombinesignal3():"));
   	   	err = Testcombinesignal3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestsigchildsignal)
   		{
   	   	INFO_PRINTF1(_L("Testsigchildsignal():"));
   	   	err = Testsigchildsignal();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestsigpipesignal)
   		{
   	   	INFO_PRINTF1(_L("Testsigpipesignal():"));
   	   	err = Testsigpipesignal();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestinheritsignal)
   		{
   	   	INFO_PRINTF1(_L("Testinheritsignal():"));
   	   	err = Testinheritsignal();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestsigalrmsignal1)
   		{
   	   	INFO_PRINTF1(_L("Testsigalrmsignal1():"));
   	   	err = Testsigalrmsignal1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestsigalrmsignal2)
   		{
   	   	INFO_PRINTF1(_L("Testsigalrmsignal2():"));
   	   	err = Testsigalrmsignal2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestsigalrmsignal3)
   		{
   	   	INFO_PRINTF1(_L("Testsigalrmsignal3():"));
   	   	err = Testsigalrmsignal3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestsigalrmsignal4)
   		{
   	   	INFO_PRINTF1(_L("Testsigalrmsignal4():"));
   	   	err = Testsigalrmsignal4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestsigalrmsignal5)
   		{
   	   	INFO_PRINTF1(_L("Testsigalrmsignal5():"));
   	   	err = Testsigalrmsignal5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	
	return TestStepResult();

	}
 

