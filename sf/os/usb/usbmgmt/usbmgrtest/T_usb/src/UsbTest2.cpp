/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This contains Usb Test Section 2
*
*/

// EPOC includes
#include <e32base.h>

// Usb Test system includes
#include <networking/log.h>
#include "UsbStep.h"
#include "UsbSuite.h"
#include "UsbTest2.h"

_LIT(KSignalSection,        "Signal");
_LIT(KSignalMark,           "Mark");
_LIT(KSignalSpace,          "Space");
_LIT(KSignalCheckValue,     "CheckValue");
_LIT(KSignalCheckValueMask, "CheckValueMask");

_LIT(KConfigSection,		"Config");
_LIT(KConfigRate,			"Rate");
_LIT(KConfigStopBits,		"StopBits");
_LIT(KConfigDataBits,		"DataBits");
_LIT(KConfigParity,			"Parity");

_LIT(KWriteSection,			"Write");
_LIT(KWriteFailureValue,	"FailureValue");

_LIT(KReadSection,			"Read");
_LIT(KReadFailureValue,		"FailureValue");

_LIT(KPortSection,			"Port");
_LIT(KPortOpenMode,			"OpenMode");
_LIT(KPortOpenRole,			"OpenRole");
_LIT(KPortOpenReturnValue,	"OpenReturnValue");

_LIT(KStateSection,			"State");
_LIT(KStateTarget,			"Target");

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 2.1
CTestSetSignalsToMark::CTestSetSignalsToMark()
	{
	// store the name of this test case
	iTestStepName = _L("SetSignalsToMarkC");
	}

// Test 2.1 Destructor
CTestSetSignalsToMark::~CTestSetSignalsToMark()
	{	
	}

// Test 2.1 Main Code
enum TVerdict CTestSetSignalsToMark::doTestStepL()
	{
	TUint mask;
	TESTL(GetIntFromConfig(KSignalSection, KSignalMark, (TInt&) mask));
	iUsbSuite->iCommPort.SetSignalsToMark(mask);
	Log(_L("CTestSetSignalsToMark::doTestStepL() - Signal to mark set to 0x%x"), mask);
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 2.2
CTestSetSignalsToSpace::CTestSetSignalsToSpace()
	{
	// store the name of this test case
	iTestStepName = _L("SetSignalsToSpaceC");
	}

// Test 2.2 Destructor
CTestSetSignalsToSpace::~CTestSetSignalsToSpace()
	{	
	}

// Test 2.2 Main Code
enum TVerdict CTestSetSignalsToSpace::doTestStepL()
	{
	TUint mask;
	TESTL(GetIntFromConfig(KSignalSection, KSignalSpace, (TInt&) mask));
	iUsbSuite->iCommPort.SetSignalsToSpace(mask);
	Log(_L("CTestSetSignalsToSpace::doTestStepL() - Signal to space set to 0x%x"), mask);
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 2.3
CTestCheckSignals::CTestCheckSignals()
	{
	// store the name of this test case
	iTestStepName = _L("CheckSignalsC");
	}

// Test 2.3 Destructor
CTestCheckSignals::~CTestCheckSignals()
	{	
	}

// Test 2.3 Main Code
enum TVerdict CTestCheckSignals::doTestStepL()
	{
	TUint checkValue;
	TUint checkValueMask;
	TUint currentValue;

	TESTL(GetIntFromConfig(KSignalSection, KSignalCheckValue, (TInt&) checkValue));

	if (GetIntFromConfig(KSignalSection, KSignalCheckValueMask, (TInt&) checkValueMask))
		currentValue = iUsbSuite->iCommPort.Signals(checkValueMask);
	else
		currentValue = iUsbSuite->iCommPort.Signals();

	Log(_L("CTestCheckSignals::doTestStepL() - Current signals set to 0x%x with mask 0x%x"), currentValue, checkValueMask);
	TEST(currentValue == checkValue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 2.4
CTestWaitForReadFailure::CTestWaitForReadFailure()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForReadFailureC");
	}

// Test 2.4 Destructor
CTestWaitForReadFailure::~CTestWaitForReadFailure()
	{	
	}

// Test 2.4 Main Code
enum TVerdict CTestWaitForReadFailure::doTestStepL()
	{
	TInt failureValue;

	TESTL(GetIntFromConfig(KReadSection, KReadFailureValue, failureValue));

	Log(_L("CTestWaitForReadFailure::doTestStepL() - Waiting for read status to be complete with error"));
	User::WaitForRequest(iUsbSuite->iReadStatus);
	Log(_L("CTestWaitForReadFailure::doTestStepL() - Read status completed with %d"), iUsbSuite->iReadStatus.Int());
	TEST(iUsbSuite->iReadStatus.Int() == failureValue);

	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 2.5
CTestWaitForWriteFailure::CTestWaitForWriteFailure()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForWriteFailureC");
	}

// Test 2.5 Destructor
CTestWaitForWriteFailure::~CTestWaitForWriteFailure()
	{	
	}

// Test 2.5 Main Code
enum TVerdict CTestWaitForWriteFailure::doTestStepL()
	{
	TInt failureValue;

	TESTL(GetIntFromConfig(KWriteSection, KWriteFailureValue, failureValue));

	Log(_L("CTestWaitForWriteFailure::doTestStepL() - Waiting for write status to be complete with error"));
	User::WaitForRequest(iUsbSuite->iWriteStatus);
	Log(_L("CTestWaitForWriteFailure::doTestStepL() - Read status completed with %d"), iUsbSuite->iWriteStatus.Int());
	TEST(iUsbSuite->iWriteStatus.Int() == failureValue);

	return iTestStepResult;
	}
 
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 2.6
CTestOpenPort::CTestOpenPort()
	{
	// store the name of this test case
	iTestStepName = _L("OpenPortC");
	}

// Test 2.6 Destructor
CTestOpenPort::~CTestOpenPort()
	{	
	}

// Test 2.6 Main Code
enum TVerdict CTestOpenPort::doTestStepL()
	{
	TCommAccess openMode;
	TCommRole role;
	TInt returnValue;

	TESTL(GetIntFromConfig(KPortSection, KPortOpenMode, (TInt&) openMode));
	TESTL(GetIntFromConfig(KPortSection, KPortOpenRole, (TInt&) role));
	TESTL(GetIntFromConfig(KPortSection, KPortOpenReturnValue, returnValue));

	OpenCommServerL();

	if (!iUsbSuite->iPortOpen)
		{
		Log(_L("CTestOpenPort::doTestStepL() - Attempting to open ACM comm port with mode %d and role %d"), openMode, role);
		TInt err = iUsbSuite->iCommPort.Open(iUsbSuite->iCommServer, PORT_NAME, openMode, role);

		if (err == KErrNone)
			{
			iUsbSuite->iPortOpen = ETrue;
			Log(_L("CTestOpenPort::doTestStepL() - Opened ACM comm port, mode %d, role %d"), openMode, role);
			}
		TESTL(err == returnValue);
		}
	else
		{
		TEST(ETrue);
		}

	return iTestStepResult;
	}
 
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 2.7
CTestWaitForConfigChange::CTestWaitForConfigChange()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForConfigChangeC");
	}

// Test 2.7 Destructor
CTestWaitForConfigChange::~CTestWaitForConfigChange()
	{
	}

// Test 2.7 Main Code
enum TVerdict CTestWaitForConfigChange::doTestStepL()
	{
	TCommConfigV01 config;

	TESTL(GetIntFromConfig(KConfigSection, KConfigRate,     (TInt&) config.iRate));
	TESTL(GetIntFromConfig(KConfigSection, KConfigStopBits, (TInt&) config.iStopBits));
	TESTL(GetIntFromConfig(KConfigSection, KConfigDataBits, (TInt&) config.iDataBits));
	TESTL(GetIntFromConfig(KConfigSection, KConfigParity,   (TInt&) config.iParity));

	Log(_L("CTestWaitForConfigChange::doTestStepL() - Waiting for config change"));
	User::WaitForRequest(iUsbSuite->iNotifyConfigChangeStatus);
	Log(_L("CTestWaitForConfigChange::doTestStepL() - Config status completed with %d"), iUsbSuite->iNotifyConfigChangeStatus.Int());
	TESTL(iUsbSuite->iNotifyConfigChangeStatus.Int() == KErrNone);

	Log(_L("iRate=%d iStopBits=%d iDataBits=%d iParity=%d"), iUsbSuite->iConfig().iRate, iUsbSuite->iConfig().iStopBits, iUsbSuite->iConfig().iDataBits, iUsbSuite->iConfig().iParity);

	TESTL(iUsbSuite->iConfig().iRate     == config.iRate);
	TESTL(iUsbSuite->iConfig().iStopBits == config.iStopBits);
	TESTL(iUsbSuite->iConfig().iDataBits == config.iDataBits);
	TESTL(iUsbSuite->iConfig().iParity   == config.iParity);

	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 2.8
CTestNotifyStateChange::CTestNotifyStateChange()
	{
	// store the name of this test case
	iTestStepName = _L("NotifyStateChange");
	}

// Test 2.8 Destructor
CTestNotifyStateChange::~CTestNotifyStateChange()
	{	
	}

// Test 2.8 Main Code
enum TVerdict CTestNotifyStateChange::doTestStepL()
	{
	TESTL(OpenLdd() == KErrNone);
	iUsbSuite->iLdd.AlternateDeviceStatusNotify(iUsbSuite->iNotifyStateStatus, iUsbSuite->iUsbState);
	TEST(ETrue);
	return iTestStepResult;
	}
 
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 2.9
CTestWaitForStateChange::CTestWaitForStateChange()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForStateChangeC");
	}

// Test 2.9 Destructor
CTestWaitForStateChange::~CTestWaitForStateChange()
	{
	}

// Test 2.9 Main Code
enum TVerdict CTestWaitForStateChange::doTestStepL()
	{
	Log(_L("CTestWaitForConfigChange::doTestStepL() - Waiting for state change..."));
	User::WaitForRequest(iUsbSuite->iNotifyStateStatus);

	if (iUsbSuite->iNotifyStateStatus.Int() != KErrNone)
		{
		Log(_L("CTestWaitForConfigChange::doTestStepL() - Error = %d"), iUsbSuite->iNotifyStateStatus.Int());
		TESTL(EFalse);
		return iTestStepResult;
		}

	TUint targetState;

	TESTL(GetIntFromConfig(KStateSection, KStateTarget, (TInt&) targetState));

	if (!(iUsbSuite->iUsbState & KUsbAlternateSetting))
		{
		Log(_L("CTestWaitForStateChange::doTestStepL() - State Changed to %d"), iUsbSuite->iUsbState);
		TESTL(iUsbSuite->iUsbState == targetState);
		}
	else
		{
		// Repost notify as state hasn't changed yet
		Log(_L("CTestWaitForStateChange::doTestStepL() - State hasn't changed, reposting notify"));
		iUsbSuite->iLdd.AlternateDeviceStatusNotify(iUsbSuite->iNotifyStateStatus, iUsbSuite->iUsbState);
		doTestStepL();
		return iTestStepResult;
		}

	return iTestStepResult;
	}



