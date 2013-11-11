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
// Name        : twaitsignal.cpp
// 
//

#include "twaitsignal.h"


CTestWaitsignal::~CTestWaitsignal() 
	{ 
	}  

CTestWaitsignal::CTestWaitsignal(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestWaitsignal::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestWaitsignal::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTestWaitsignal::doTestStepL()
	{
	int err;

   	if(TestStepName() == KTestpendingsignal1)
   		{
   	   	INFO_PRINTF1(_L("Testpendingsignal1():"));
   	   	err = Testpendingsignal1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestpendingsignal2)
   		{
   	   	INFO_PRINTF1(_L("Testpendingsignal2():"));
   	   	err = Testpendingsignal2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestpendingsignal3)
   		{
   	   	INFO_PRINTF1(_L("Testpendingsignal3():"));
   	   	err = Testpendingsignal3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestpendingsignal4)
   		{
   	   	INFO_PRINTF1(_L("Testpendingsignal4():"));
   	   	err = Testpendingsignal4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	if(TestStepName() == KTestwaitsignal1)
   		{
   	   	INFO_PRINTF1(_L("Testwaitsignal1():"));
   	   	err = Testwaitsignal1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitsignal2)
   		{
   	   	INFO_PRINTF1(_L("Testwaitsignal2():"));
   	   	err = Testwaitsignal2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitsignal3)
   		{
   	   	INFO_PRINTF1(_L("Testwaitsignal3():"));
   	   	err = Testwaitsignal3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitsignal4)
   		{
   	   	INFO_PRINTF1(_L("Testwaitsignal4():"));
   	   	err = Testwaitsignal4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitsignal5)
   		{
   	   	INFO_PRINTF1(_L("Testwaitsignal5():"));
   	   	err = Testwaitsignal5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitsignal6)
   		{
   	   	INFO_PRINTF1(_L("Testwaitsignal6():"));
   	   	err = Testwaitsignal6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitsignal7)
   		{
   	   	INFO_PRINTF1(_L("Testwaitsignal7():"));
   	   	err = Testwaitsignal7();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitsignal8)
   		{
   	   	INFO_PRINTF1(_L("Testwaitsignal8():"));
   	   	err = Testwaitsignal8();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitsignal9)
   		{
   	   	INFO_PRINTF1(_L("Testwaitsignal9():"));
   	   	err = Testwaitsignal9();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitinfosignal1)
   		{
   	   	INFO_PRINTF1(_L("Testwaitinfosignal1():"));
   	   	err = Testwaitinfosignal1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitinfosignal2)
   		{
   	   	INFO_PRINTF1(_L("Testwaitinfosignal2():"));
   	   	err = Testwaitinfosignal2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitinfosignal3)
   		{
   	   	INFO_PRINTF1(_L("Testwaitinfosignal3():"));
   	   	err = Testwaitinfosignal3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitinfosignal4)
   		{
   	   	INFO_PRINTF1(_L("Testwaitinfosignal4():"));
   	   	err = Testwaitinfosignal4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitinfosignal5)
   		{
   	   	INFO_PRINTF1(_L("Testwaitinfosignal5():"));
   	   	err = Testwaitinfosignal5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitinfosignal6)
   		{
   	   	INFO_PRINTF1(_L("Testwaitinfosignal6():"));
   	   	err = Testwaitinfosignal6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitinfosignal7)
   		{
   	   	INFO_PRINTF1(_L("Testwaitinfosignal7():"));
   	   	err = Testwaitinfosignal7();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitinfosignal8)
   		{
   	   	INFO_PRINTF1(_L("Testwaitinfosignal8():"));
   	   	err = Testwaitinfosignal8();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestwaitinfosignal9)
   		{
   	   	INFO_PRINTF1(_L("Testwaitinfosignal9():"));
   	   	err = Testwaitinfosignal9();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal1)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal1():"));
   	   	err = Testtimedwaitsignal1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal2)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal2():"));
   	   	err = Testtimedwaitsignal2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal3)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal3():"));
   	   	err = Testtimedwaitsignal3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal4)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal4():"));
   	   	err = Testtimedwaitsignal4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal5)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal5():"));
   	   	err = Testtimedwaitsignal5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal6)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal6():"));
   	   	err = Testtimedwaitsignal6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal7)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal7():"));
   	   	err = Testtimedwaitsignal7();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal8)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal8():"));
   	   	err = Testtimedwaitsignal8();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal9)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal9():"));
   	   	err = Testtimedwaitsignal9();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal10)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal10():"));
   	   	err = Testtimedwaitsignal10();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal11)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal11():"));
   	   	err = Testtimedwaitsignal11();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal12)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal12():"));
   	   	err = Testtimedwaitsignal12();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal13)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal13():"));
   	   	err = Testtimedwaitsignal13();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTesttimedwaitsignal14)
   		{
   	   	INFO_PRINTF1(_L("Testtimedwaitsignal14():"));
   	   	err = Testtimedwaitsignal14();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	
	return TestStepResult();

	}
 

