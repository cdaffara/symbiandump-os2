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
// Name        : tblocksignal.cpp
// 
//

#include "tblocksignal.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestBlocksignal::~CTestBlocksignal() 
	{ 
	}  

CTestBlocksignal::CTestBlocksignal(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestBlocksignal::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestBlocksignal::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTestBlocksignal::doTestStepL()
	{
	int err;
	if(TestStepName() == KTestempty1)
   		{
   	   	INFO_PRINTF1(_L("Testempty1():"));
   	   	err = Testempty1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestempty2)
   		{
   	   	INFO_PRINTF1(_L("Testempty2():"));
   	   	err = Testempty2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestaddset1)
   		{
   	   	INFO_PRINTF1(_L("Testaddset1():"));
   	   	err = Testaddset1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestaddset2)
   		{
   	   	INFO_PRINTF1(_L("Testaddset2():"));
   	   	err = Testaddset2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestaddset3)
   		{
   	   	INFO_PRINTF1(_L("Testaddset3():"));
   	   	err = Testaddset3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestaddset4)
   		{
   	   	INFO_PRINTF1(_L("Testaddset4():"));
   	   	err = Testaddset4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestaddset5)
   		{
   	   	INFO_PRINTF1(_L("Testaddset5():"));
   	   	err = Testaddset5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestaddndelset1)
   		{
   	   	INFO_PRINTF1(_L("Testaddndelset1():"));
   	   	err = Testaddndelset1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestaddndelset2)
   		{
   	   	INFO_PRINTF1(_L("Testaddndelset2():"));
   	   	err = Testaddndelset2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestaddndelset3)
   		{
   	   	INFO_PRINTF1(_L("Testaddndelset3():"));
   	   	err = Testaddndelset3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestaddndelset4)
   		{
   	   	INFO_PRINTF1(_L("Testaddndelset4():"));
   	   	err = Testaddndelset4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigprocmask1)
   		{
   	   	INFO_PRINTF1(_L("Testsigprocmask1():"));
   	   	err = Testsigprocmask1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigprocmask2)
   		{
   	   	INFO_PRINTF1(_L("Testsigprocmask2():"));
   	   	err = Testsigprocmask2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigprocmask3)
   		{
   	   	INFO_PRINTF1(_L("Testsigprocmask3():"));
   	   	err = Testsigprocmask3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigprocmask4)
   		{
   	   	INFO_PRINTF1(_L("Testsigprocmask4():"));
   	   	err = Testsigprocmask4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigprocmask5)
   		{
   	   	INFO_PRINTF1(_L("Testsigprocmask5():"));
   	   	err = Testsigprocmask5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigprocmask6)
   		{
   	   	INFO_PRINTF1(_L("Testsigprocmask6():"));
   	   	err = Testsigprocmask6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigprocmask7)
   		{
   	   	INFO_PRINTF1(_L("Testsigprocmask7():"));
   	   	err = Testsigprocmask7();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigprocmask8)
   		{
   	   	INFO_PRINTF1(_L("Testsigprocmask8():"));
   	   	err = Testsigprocmask8();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigfillset1)
   		{
   	   	INFO_PRINTF1(_L("Testsigfillset1():"));
   	   	err = Testsigfillset1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigfillset2)
   		{
   	   	INFO_PRINTF1(_L("Testsigfillset2():"));
   	   	err = Testsigfillset2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigandset1)
   		{
   	   	INFO_PRINTF1(_L("Testsigandset1():"));
   	   	err = Testsigandset1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigandset2)
   		{
   	   	INFO_PRINTF1(_L("Testsigandset2():"));
   	   	err = Testsigandset2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigandset3)
   		{
   	   	INFO_PRINTF1(_L("Testsigandset3():"));
   	   	err = Testsigandset3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigorset1)
   		{
   	   	INFO_PRINTF1(_L("Testsigorset1():"));
   	   	err = Testsigorset1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigorset2)
   		{
   	   	INFO_PRINTF1(_L("Testsigorset2():"));
   	   	err = Testsigorset2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigorset3)
   		{
   	   	INFO_PRINTF1(_L("Testsigorset3():"));
   	   	err = Testsigorset3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestandorset1)
   		{
   	   	INFO_PRINTF1(_L("Testandorset1():"));
   	   	err = Testandorset1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestandorset2)
   		{
   	   	INFO_PRINTF1(_L("Testandorset2():"));
   	   	err = Testandorset2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestandorset3)
   		{
   	   	INFO_PRINTF1(_L("Testandorset3():"));
   	   	err = Testandorset3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsighold1)
   		{
   	   	INFO_PRINTF1(_L("KTestsighold1():"));
   	   	err = Testsighold1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsighold2)
   		{
   	   	INFO_PRINTF1(_L("Testsighold2():"));
   	   	err = Testsighold2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigrelse1)
   		{
   	   	INFO_PRINTF1(_L("Testsigrelse1():"));
   	   	err = Testsigrelse1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestsigrelse2)
   		{
   	   	INFO_PRINTF1(_L("Testsigrelse2():"));
   	   	err = Testsigrelse2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	if(TestStepName() == KTestholdnrelse1)
   		{
   	   	INFO_PRINTF1(_L("Testholdnrelse1():"));
   	   	err = Testholdnrelse1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	

   	if(TestStepName() == KTestsigpause1)
   		{
   	   	INFO_PRINTF1(_L("Testsigpause1():"));
   	   	err = Testsigpause1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  	
   	if(TestStepName() == KTestnegsigpause1)
   		{
   		INFO_PRINTF1(_L("Testnegsigpause1():"));
   		err = Testnegsigpause1();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   	
	return TestStepResult();

	}
 

