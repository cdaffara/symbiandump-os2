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
// Name        : tclock.cpp
// 
//

#include "tclock.h"


CTestclock::~CTestclock() 
	{ 
	}  

CTestclock::CTestclock(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestclock::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestclock::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTestclock::doTestStepL()
	{
	int err;
	if(TestStepName() == KTestgetclockid1)
   		{
   	   	INFO_PRINTF1(_L("Testgetclockid1():"));
   	   	err = Testgetclockid1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   	
	if(TestStepName() == KTestgetclockid2)
   		{
   	   	INFO_PRINTF1(_L("Testgetclockid2():"));
   	   	err = Testgetclockid2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     	
	if(TestStepName() == KTestgetclockid3)
   		{
   	   	INFO_PRINTF1(_L("Testgetclockid3():"));
   	   	err = Testgetclockid3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     	
	if(TestStepName() == KTestgetclockid4)
   		{
   	   	INFO_PRINTF1(_L("Testgetclockid4():"));
   	   	err = Testgetclockid4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestgetclockid5)
   		{
   	   	INFO_PRINTF1(_L("Testgetclockid5():"));
   	   	err = Testgetclockid5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestclockresolution1)
   		{
   	   	INFO_PRINTF1(_L("Testclockresolution1():"));
   	   	err = Testclockresolution1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}       	
	if(TestStepName() == KTestclockresolution2)
   		{
   	   	INFO_PRINTF1(_L("Testclockresolution2():"));
   	   	err = Testclockresolution2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestclockresolution3)
   		{
   	   	INFO_PRINTF1(_L("Testclockresolution3():"));
   	   	err = Testclockresolution3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     	
	if(TestStepName() == KTestclocknanosleep1)
   		{
   	   	INFO_PRINTF1(_L("Testclocknanosleep1():"));
   	   	err = Testclocknanosleep1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}         	
	if(TestStepName() == KTestclocknanosleep2)
   		{
   	   	INFO_PRINTF1(_L("Testclocknanosleep2():"));
   	   	err = Testclocknanosleep2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestclocknanosleep3)
   		{
   	   	INFO_PRINTF1(_L("Testclocknanosleep3():"));
   	   	err = Testclocknanosleep3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}         	
	if(TestStepName() == KTestclocknanosleep4)
   		{
   	   	INFO_PRINTF1(_L("Testclocknanosleep4():"));
   	   	err = Testclocknanosleep4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}     	
	if(TestStepName() == KTestclocknanosleep5)
   		{
   	   	INFO_PRINTF1(_L("Testclocknanosleep5():"));
   	   	err = Testclocknanosleep5();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}         	
	if(TestStepName() == KTestclocknanosleep6)
   		{
   	   	INFO_PRINTF1(_L("Testclocknanosleep6():"));
   	   	err = Testclocknanosleep6();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  	
	if(TestStepName() == KTestclockgettime1)
   		{
   	   	INFO_PRINTF1(_L("Testclockgettime1():"));
   	   	err = Testclockgettime1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestclockgettime2)
   		{
   	   	INFO_PRINTF1(_L("Testclockgettime2():"));
   	   	err = Testclockgettime2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestclockgettime3)
   		{
   	   	INFO_PRINTF1(_L("Testclockgettime3():"));
   	   	err = Testclockgettime3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    	
	if(TestStepName() == KTestclocksettime1)
   		{
   	   	INFO_PRINTF1(_L("Testclocksettime1():"));
   	   	err = Testclocksettime1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestclocksettime2)
   		{
   	   	INFO_PRINTF1(_L("Testclocksettime2():"));
   	   	err = Testclocksettime2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}      	
	if(TestStepName() == KTestclocksettime3)
   		{
   	   	INFO_PRINTF1(_L("Testclocksettime3():"));
   	   	err = Testclocksettime3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
	return TestStepResult();
	}
 

