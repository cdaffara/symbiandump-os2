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
// Name        : tsendsignal.cpp
// 
//

#include "tsendsignal.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestSendsignal::~CTestSendsignal() 
	{ 
	}  

CTestSendsignal::CTestSendsignal(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSendsignal::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSendsignal::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTestSendsignal::doTestStepL()
	{
	int err;
	if(TestStepName() == KTestRaise)
   		{
   	   	INFO_PRINTF1(_L("TestRaise():"));
   	   	err = TestRaise();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
	else if(TestStepName() == KTestRaise1)
	   		{
	   	   	INFO_PRINTF1(_L("TestRaise1():"));
	   	   	err = TestRaise1();
	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	   	   	}
	else if(TestStepName() == KTestNegativeRaise)
   		{
   	   	INFO_PRINTF1(_L("TestNegativeRaise():"));
   	   	err = TestNegativeRaise();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}    	 
   	else if(TestStepName() == KTestKill1)
   		{
   	   	INFO_PRINTF1(_L("TestKill1():"));
   	   	err = TestKill1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == KTestKill2)
   		{
   	   	INFO_PRINTF1(_L("TestKill2():"));
   	   	err = TestKill2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == KTestNegativeKill1)
   		{
   	   	INFO_PRINTF1(_L("TestNegativeKill1():"));
   	   	err = TestNegativeKill1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == KTestNegativeKill2)
   		{
   	   	INFO_PRINTF1(_L("TestNegativeKill2():"));
   	   	err = TestNegativeKill2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}  
   	else if(TestStepName() == KTestNegativeKill3)
   		{
   	   	INFO_PRINTF1(_L("TestNegativeKill3():"));
   	   	err = TestNegativeKill3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == KTestNegativeKill4)
   		{
   	   	INFO_PRINTF1(_L("TestNegativeKill4():"));
   	   	err = TestNegativeKill4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestSigqueue)
   		{
   	   	INFO_PRINTF1(_L("TestSigqueue():"));
   	   	err = TestSigqueue();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 
   	else if(TestStepName() == KTestNegativeSigqueue1)
   		{
   	   	INFO_PRINTF1(_L("TestNegativeSigqueue1():"));
   	   	err = TestNegativeSigqueue1();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestNegativeSigqueue2)
   		{
   	   	INFO_PRINTF1(_L("TestNegativeSigqueue2():"));
   	   	err = TestNegativeSigqueue2();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestNegativeSigqueue3)
   		{
   	   	INFO_PRINTF1(_L("TestNegativeSigqueue3():"));
   	   	err = TestNegativeSigqueue3();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestNegativeSigqueue4)
   		{
   	   	INFO_PRINTF1(_L("TestNegativeSigqueue4():"));
   	   	err = TestNegativeSigqueue4();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestSigKillwithTmpfile)
   	   		{
   	   	   	INFO_PRINTF1(_L("TestSigKillwithTmpfile():"));
   	   	   	err = TestSigKillwithTmpfile();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
	return TestStepResult();

	}
 

