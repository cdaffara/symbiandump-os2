/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* Example CTestStep derived implementation
*
*/



/**
 @file AccessStep.cpp
*/
#include "accessstep.h"
#include <test/testshareddata.h>
#include <test/testexecutelog.h>

CAccessStep1::~CAccessStep1()
/**
 * Destructor
 */
	{
	}

CAccessStep1::CAccessStep1()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KAccessStep1);
	}

TVerdict CAccessStep1::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates opening up a handle to the data saved at RChunk base using template class
 * Overwrite shared data with values without locking mechanism for data access
 */
	{
	INFO_PRINTF1(_L("Access Step1 doTestStepL"));
	SetTestStepResult(EFail);
	
	TBuf<256> txtReadData(KNull);
	TRAPD(err,ReadSharedDataL(_L("SharedData"), txtReadData));
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("Value of Shared Data in Access Step1 initially is %S"), &txtReadData);
		_LIT(KDesVal, "Good Morning James! Good Morning Nataraj! Good Morning Bala! Good Morning Ganesh! Good Morning Sridhar! Good Morning Naresh! Good Morning Sanjeev! Good Morning Kathirvel!");
		TPtrC txtPtr;
		txtPtr.Set(KDesVal);
		WriteSharedDataL(_L("SharedData"), txtPtr, ESetText);
		txtReadData.Zero();
		ReadSharedDataL(_L("SharedData"), txtReadData);
		INFO_PRINTF2(_L("Value of Shared Data in Access Step1 updated is %S"), &txtReadData);
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

CAccessStep2::~CAccessStep2()
/**
 * Destructor
 */
	{
	}

CAccessStep2::CAccessStep2()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KAccessStep2);
	}

TVerdict CAccessStep2::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates opening up a handle to the data saved at RChunk base using template class
 * Overwrite shared data with values and also demonstrates the locking mechanism
 */
	{
	INFO_PRINTF1(_L("Access Step2 doTestStepL"));
	SetTestStepResult(EFail);
	
	TBuf<256> txtReadData(KNull);
	TRAPD(err,ReadSharedDataL(_L("SharedData1"), txtReadData));
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("Value of Shared Data1 in Access Step2 initially is %S"), &txtReadData);
		_LIT(KDesVal, "Good Morning Ganesh!");
		TPtrC txtPtr;
		txtPtr.Set(KDesVal);
		WriteSharedDataL(_L("SharedData1"), txtPtr, EAppendText);
		txtReadData.Zero();
		ReadSharedDataL(_L("SharedData1"), txtReadData);
		INFO_PRINTF2(_L("Value of Shared Data1 in Access Step2 updated is %S"), &txtReadData);
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}
