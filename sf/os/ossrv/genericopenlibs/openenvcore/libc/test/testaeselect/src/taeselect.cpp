// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : taeselect.cpp
//

#include "taeselect.h"

CTestAESelect::~CTestAESelect() 
	{
	// No implementation required
	}  

CTestAESelect::CTestAESelect(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestAESelect::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestAESelect::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

TVerdict CTestAESelect::doTestStepL()
	{
	int err;
   	if(TestStepName() == KTestTimeout)
   		{
   		INFO_PRINTF1(_L("TestTimeout():"));
   		err = TestTimeout();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else if(TestStepName() == KTestASelect)
   		{
   	   	INFO_PRINTF1(_L("TestASelect():"));
   	   	err = TestAselect();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestCancelASelect)
   		{
   	   	INFO_PRINTF1(_L("TestCancelASelect():"));
   	   	err = TestCancelAselect();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestAselectRequestStatus)
   		{
   	   	INFO_PRINTF1(_L("TestAselectRequestStatus():"));
   	   	err = TestAselectRequestStatus();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestMultipleASelect)
   		{
   	   	INFO_PRINTF1(_L("TestMultipleASelect():"));
   	   	err = TestMultipleAselect();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == KTestSelectwithEselect)
   	   		{
   	   	   	INFO_PRINTF1(_L("TestSelectwithEselect():"));
   	   	   	err = TestSelectwithEselect();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   	else if(TestStepName() == KTestESelect)
   	   		{
   	   	   	INFO_PRINTF1(_L("TestESelect():"));
   	   	   	err = TestEselect();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   else if(TestStepName() == KTestEselectRequestStatus)
   	   		{
   	   	   	INFO_PRINTF1(_L("KTestEselectRequestStatus():"));
   	   	   	err = TestEselectRequestStatus();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   else if(TestStepName() == KTestESelectTimeout)
   	   		{
   	   	   	INFO_PRINTF1(_L("KTestEselectTimeout():"));
   	   	   	err = TestEselectTimeout();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   	else if(TestStepName() == KTestEselectInvalidInput_01)
   	   		{
   	   	   	INFO_PRINTF1(_L("TestEselectInvalidInput_01():"));
   	   	   	err = TestEselectInvalidInput_01();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   	else if(TestStepName() == KTestEselectInvalidInput_02)
   	   		{
   	   	   	INFO_PRINTF1(_L("TestEselectInvalidInput_02():"));
   	   	   	err = TestEselectInvalidInput_02();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}   	
   	return TestStepResult();   	
	}

