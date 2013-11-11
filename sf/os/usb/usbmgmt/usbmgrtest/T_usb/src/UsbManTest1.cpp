/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This contains UsbMan Test Section 1
*
*/

// EPOC includes
#include <e32base.h>

// Usb Test system includes
#include <networking/log.h>
#include "UsbStep.h"
#include "UsbSuite.h"
#include "UsbManTest1.h"


_LIT(KUsbmanStateSection,   "UsbManState");
_LIT(KUsbmanStateMask,      "StateMask");
_LIT(KUsbmanExpectedState,  "ExpectedState");

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.1
CTestUsbManNotifyStateChange::CTestUsbManNotifyStateChange()
	{
	// store the name of this test case
	iTestStepName = _L("UMNotifyStateChangeC");
	}

// Test 1.1 Destructor
CTestUsbManNotifyStateChange::~CTestUsbManNotifyStateChange()
	{	
	}

// Test 1.1 Main Code
enum TVerdict CTestUsbManNotifyStateChange::doTestStepL()
	{
	TUint32 stateMask;

	TESTL(GetIntFromConfig(KUsbmanStateSection, KUsbmanStateMask, (TInt&) stateMask));

	Log(_L("CTestUsbManNotifyStateChange::doTestStepL() - Posting notification request"));
	iUsbSuite->iUsb.StateNotification(stateMask, iUsbSuite->iUsbManState, iUsbSuite->iNotifyStateStatus);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.2
CTestUsbManWaitForStateChange::CTestUsbManWaitForStateChange()
	{
	// store the name of this test case
	iTestStepName = _L("UMWaitForStateChgeC");
	}

// Test 1.1 Destructor
CTestUsbManWaitForStateChange::~CTestUsbManWaitForStateChange()
	{	
	}

// Test 1.1 Main Code
enum TVerdict CTestUsbManWaitForStateChange::doTestStepL()
	{
	TUsbDeviceState expectedState;

	TESTL(GetIntFromConfig(KUsbmanStateSection, KUsbmanExpectedState, (TInt&) expectedState));

	User::WaitForRequest(iUsbSuite->iNotifyStateStatus);
	Log(_L("CTestUsbManWaitForStateChange::doTestStepL() - State in the config file is %d"), expectedState);
	Log(_L("CTestUsbManWaitForStateChange::doTestStepL() - State changed to %d"), iUsbSuite->iUsbManState);

	TESTL(expectedState == iUsbSuite->iUsbManState);
	return iTestStepResult;
	}

