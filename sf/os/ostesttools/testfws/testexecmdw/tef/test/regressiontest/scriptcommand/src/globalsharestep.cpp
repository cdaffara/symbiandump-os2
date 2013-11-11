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
 @file GlobalShareStep.cpp
*/
#include "globalsharestep.h"
#include <test/testshareddata.h>
#include <test/testexecutelog.h>

CGlobalShareStep1::~CGlobalShareStep1()
/**
 * Destructor
 */
	{
	}

CGlobalShareStep1::CGlobalShareStep1()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KGlobalShareStep1);
	}

TVerdict CGlobalShareStep1::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates opening up a handle to the data saved at RChunk base using template class
 * Overwrite shared data with values and also demonstrates the locking mechanism
 */
	{
	INFO_PRINTF1(_L("Global Step1 doTestStepL"));
	SetTestStepResult(EFail);
	
	TBuf<256> txtReadData(KNull);
	TRAPD(err,ReadSharedDataL(_L("SharedData"), txtReadData));
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("Value of Shared Data in Global Step2 initially is %S"), &txtReadData);
		_LIT(KDesVal, "Hi Ganesh! How r u?");
		TPtrC txtPtr;
		txtPtr.Set(KDesVal);
		WriteSharedDataL(_L("SharedData"), txtPtr, EAppendText);
		txtReadData.Zero();
		ReadSharedDataL(_L("SharedData"), txtReadData);
		INFO_PRINTF2(_L("Value of Shared Data in Global Step2 updated is %S"), &txtReadData);
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

CGlobalShareStep2::~CGlobalShareStep2()
/**
 * Destructor
 */
	{
	}

CGlobalShareStep2::CGlobalShareStep2()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KGlobalShareStep2);
	}

TVerdict CGlobalShareStep2::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates opening up a handle to the data saved at RChunk base using template class
 * Overwrite shared data with values and also demonstrates the locking mechanism
 */
	{
	INFO_PRINTF1(_L("Global Step2 doTestStepL"));
	SetTestStepResult(EFail);
	TBuf<256> txtReadData(KNull);
	
	TRAPD(err,ReadSharedDataL(_L("SharedData1"), txtReadData));
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("Value of Shared Data1 in Global Step2 initially is %S"), &txtReadData);
		_LIT(KDesVal, "Hi Mahesh! How r u?");
		TPtrC txtPtr;
		txtPtr.Set(KDesVal);
		WriteSharedDataL(_L("SharedData1"), txtPtr, ESetText);
		txtReadData.Zero();
		ReadSharedDataL(_L("SharedData1"), txtReadData);
		INFO_PRINTF2(_L("Value of Shared Data1 in Global Step2 updated is %S"), &txtReadData);
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}
