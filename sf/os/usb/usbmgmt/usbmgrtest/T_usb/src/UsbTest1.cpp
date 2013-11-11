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
* This contains Usb Test Section 1
*
*/

// EPOC includes
#include <e32base.h>

// Usb Test system includes
#include <networking/log.h>
#include "UsbStep.h"
#include "UsbSuite.h"
#include "UsbTest1.h"

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.1
CTestStartUsb::CTestStartUsb()
	{
	// store the name of this test case
	iTestStepName = _L("StartUsb");
	}

// Test 1.1 Destructor
CTestStartUsb::~CTestStartUsb()
	{	
	}

// Test 1.1 Main Code
enum TVerdict CTestStartUsb::doTestStepL()
	{
	if (!iUsbSuite->iUsbOpen)
		{
		TESTL(iUsbSuite->iUsb.Connect() == KErrNone);
		iUsbSuite->iUsbOpen = ETrue;
		}

	iUsbSuite->iUsb.Start(iUsbSuite->iStartStatus);
	Log(_L("CTestStartUsb::doTestStepL() - Requested USB service start"));
	return iTestStepResult;
	}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.2
CTestWaitForUsb::CTestWaitForUsb()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForUsb");
	}

// Test 1.2 Destructor
CTestWaitForUsb::~CTestWaitForUsb()
	{
	}

// Test 1.2	Main Code
enum TVerdict CTestWaitForUsb::doTestStepL()
	{
	User::WaitForRequest(iUsbSuite->iStartStatus);
	Log(_L("CTestWaitForUsb::doTestStepL() - Usb Start returned %d"), iUsbSuite->iStartStatus.Int());
	TESTL(iUsbSuite->iStartStatus.Int() == KErrNone);
	return iTestStepResult;
	}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.3
CTestStartCsy::CTestStartCsy()
	{
	// store the name of this test case
	iTestStepName = _L("StartCSY");
	}

// destructor
CTestStartCsy::~CTestStartCsy()
	{	
	}

// Test 1.3	Main Code
enum TVerdict CTestStartCsy::doTestStepL()
	{
	OpenCommServerL();
	return iTestStepResult;
	}	


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.4
CTestOpenDTEPort::CTestOpenDTEPort()
	{
	// store the name of this test case
	iTestStepName = _L("OpenDTE");
	}

// Test 1.4 Destructor
CTestOpenDTEPort::~CTestOpenDTEPort()
	{	
	}

// Test 1.4	Main Code
enum TVerdict CTestOpenDTEPort::doTestStepL()
	{
	OpenCommServerL();

	if (!iUsbSuite->iPortOpen)
		{
		TInt err = iUsbSuite->iCommPort.Open(iUsbSuite->iCommServer, PORT_NAME, ECommExclusive, ECommRoleDTE);
		TESTL(err == KErrNone);
		iUsbSuite->iPortOpen = ETrue;
		Log(_L("CTestOpenDTEPort::doTestStepL() - Opened ACM comm port, role DTE"));
		}
	else
		{
		TEST(ETrue);
		}

	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.5
CTestOpenDCEPort::CTestOpenDCEPort()
	{
	// store the name of this test case
	iTestStepName = _L("OpenDCE");
	}

// Test 1.5 Destructor
CTestOpenDCEPort::~CTestOpenDCEPort()
	{
	}

// Test 1.5 Main Code
enum TVerdict CTestOpenDCEPort::doTestStepL()
	{
	OpenCommServerL();

	if (!iUsbSuite->iPortOpen)
		{
		TInt err = iUsbSuite->iCommPort.Open(iUsbSuite->iCommServer, PORT_NAME, ECommExclusive, ECommRoleDCE);
		TESTL(err == KErrNone);
		iUsbSuite->iPortOpen = ETrue;

		Log(_L("CTestOpenDCEPort::doTestStepL() - Opened ACM comm port, role DCE"));
		}
	else
		{
		TEST(ETrue);
		}

	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.6
CTestPostRead::CTestPostRead()
	{
	// store the name of this test case
	iTestStepName = _L("Read");
	}

// Test 1.6 Destructor
CTestPostRead::~CTestPostRead()
	{
	}

// Test 1.6 Main Code
enum TVerdict CTestPostRead::doTestStepL()
	{
	iUsbSuite->iCommPort.Read(iUsbSuite->iReadStatus, iUsbSuite->iReadBuffer);
	Log(_L("CTestPostRead::doTestStepL() - Read posted on ACM comm port"));
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.7
CTestPostWrite::CTestPostWrite()
	{
	// store the name of this test case
	iTestStepName = _L("Write");
	}

// Test 1.7 Destructor
CTestPostWrite::~CTestPostWrite()
	{
	}

// Test 1.7 Main Code
enum TVerdict CTestPostWrite::doTestStepL()
	{
	iUsbSuite->iWriteBuffer.SetLength(24);
	iUsbSuite->iWriteBuffer.Copy(_L("Hello There!"));
	iUsbSuite->iCommPort.Write(iUsbSuite->iWriteStatus, iUsbSuite->iWriteBuffer);
	Log(_L("CTestPostWrite::doTestStepL() - Write posted on ACM comm port"));
	TEST(ETrue);
	return iTestStepResult;
	}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.8
CTestCancelRead::CTestCancelRead()
	{
	// store the name of this test case
	iTestStepName = _L("CancelRead");
	}

// Test 1.8 Destructor
CTestCancelRead::~CTestCancelRead()
	{
	}

// Test 1.8 Main Code
enum TVerdict CTestCancelRead::doTestStepL()
	{
	iUsbSuite->iCommPort.ReadCancel();
	Log(_L("CTestCancelRead::doTestStepL() - Read Cancelled"));
	TEST(ETrue);
	return iTestStepResult;
	}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.9
CTestCancelWrite::CTestCancelWrite()
	{
	// store the name of this test case
	iTestStepName = _L("CancelWrite");
	}

// Test 1.9 Destructor
CTestCancelWrite::~CTestCancelWrite()
	{
	}

// Test 1.9 Main Code
enum TVerdict CTestCancelWrite::doTestStepL()
	{
	iUsbSuite->iCommPort.WriteCancel();
	Log(_L("CTestCancelWrite::doTestStepL() - Write Cancelled"));
	TEST(ETrue);
	return iTestStepResult;
	}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.10
CTestWaitForRead::CTestWaitForRead()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForRead");
	}

// Test 1.10 Destructor
CTestWaitForRead::~CTestWaitForRead()
	{
	}

// Test 1.10 Main Code
enum TVerdict CTestWaitForRead::doTestStepL()
	{
	User::WaitForRequest(iUsbSuite->iReadStatus);
	Log(_L("CTestWaitForRead::doTestStepL() - Read request returned %d"), iUsbSuite->iReadStatus.Int());
	TESTL(iUsbSuite->iReadStatus.Int() == KErrNone);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.11
CTestWaitForWrite::CTestWaitForWrite()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForWrite");
	}

// Test 1.11 Destructor
CTestWaitForWrite::~CTestWaitForWrite()
	{
	}

// Test 1.11 Main Code
enum TVerdict CTestWaitForWrite::doTestStepL()
	{
	User::WaitForRequest(iUsbSuite->iWriteStatus);
	Log(_L("CTestWaitForWrite::doTestStepL() - Write request returned %d"), iUsbSuite->iWriteStatus.Int());
	TESTL(iUsbSuite->iWriteStatus.Int() == KErrNone);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.12
CTestPostReadOneOrMore::CTestPostReadOneOrMore()
	{
	// store the name of this test case
	iTestStepName = _L("ReadOneOrMore");
	}

// Test 1.12 Destructor
CTestPostReadOneOrMore::~CTestPostReadOneOrMore()
	{
	}

// Test 1.12 Main Code
enum TVerdict CTestPostReadOneOrMore::doTestStepL()
	{
	iUsbSuite->iCommPort.ReadOneOrMore(iUsbSuite->iReadStatus, iUsbSuite->iReadBuffer);
	Log(_L("CTestPostReadOneOrMore::doTestStepL() - Read posted on ACM comm port"));
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.13
CTestClosePort::CTestClosePort()
	{
	// store the name of this test case
	iTestStepName = _L("ClosePort");
	}

// Test 1.13 Destructor
CTestClosePort::~CTestClosePort()
	{
	}

// Test 1.13 Main Code
enum TVerdict CTestClosePort::doTestStepL()
	{
	iUsbSuite->iCommPort.Close();
	iUsbSuite->iPortOpen = EFalse;
	Log(_L("CTestClosePort::doTestStepL() - ACM comm port closed"));
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.14
CTestCloseCommServer::CTestCloseCommServer()
	{
	// store the name of this test case
	iTestStepName = _L("CloseCommServer");
	}

// Test 1.14 Destructor
CTestCloseCommServer::~CTestCloseCommServer()
	{
	}

// Test 1.14 Main Code
enum TVerdict CTestCloseCommServer::doTestStepL()
	{
	iUsbSuite->iCommServer.Close();
	iUsbSuite->iCommServerOpen = EFalse;
	Log(_L("CTestCloseCommServer::doTestStepL() - Comm Server handle closed"));
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.15
CTestCloseUsbServer::CTestCloseUsbServer()
	{
	// store the name of this test case
	iTestStepName = _L("CloseUsb");
	}

// Test 1.15 Destructor
CTestCloseUsbServer::~CTestCloseUsbServer()
	{
	}

// Test 1.15 Main Code
enum TVerdict CTestCloseUsbServer::doTestStepL()
	{
	iUsbSuite->iUsb.Close();
	iUsbSuite->iUsbOpen = EFalse;
	Log(_L("CTestCloseUsbServer::doTestStepL() - Usb Server handle closed"));
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.16
CTestStopUsb::CTestStopUsb()
	{
	// store the name of this test case
	iTestStepName = _L("StopUsb");
	}

// Test 1.16 Destructor
CTestStopUsb::~CTestStopUsb()
	{
	}

// Test 1.16 Main Code
enum TVerdict CTestStopUsb::doTestStepL()
	{
	iUsbSuite->iUsb.Stop();
	Log(_L("CTestCloseUsbServer::doTestStepL() - Usb Services stopped"));
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.17
CTestNotifySignalChange::CTestNotifySignalChange()
	{
	// store the name of this test case
	iTestStepName = _L("NotifySignalChange");
	}

// Test 1.17 Destructor
CTestNotifySignalChange::~CTestNotifySignalChange()
	{
	}

// Test 1.17 Main Code
enum TVerdict CTestNotifySignalChange::doTestStepL()
	{
	iUsbSuite->iCommPort.NotifySignalChange(iUsbSuite->iNotifySignalChangeStatus, iUsbSuite->iSignals);
	Log(_L("CTestNotifySignalChange::doTestStepL() - Posted signal change notification request"));
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.18
CTestWaitForSignalChange::CTestWaitForSignalChange()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForSignalChange");
	}

// Test 1.18 Destructor
CTestWaitForSignalChange::~CTestWaitForSignalChange()
	{
	}

// Test 1.18 Main Code
enum TVerdict CTestWaitForSignalChange::doTestStepL()
	{
	Log(_L("CTestWaitForSignalChange::doTestStepL() - Waiting for signal change"));
	User::WaitForRequest(iUsbSuite->iNotifySignalChangeStatus);
	Log(_L("CTestNotifySignalChange::doTestStepL() - Signal status completed with %d"), iUsbSuite->iNotifySignalChangeStatus.Int());
	TESTL(iUsbSuite->iNotifySignalChangeStatus.Int() == KErrNone);
	Log(_L("CTestNotifySignalChange::doTestStepL() - Signal = %d"), iUsbSuite->iSignals);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.19
CTestWaitForReadCancel::CTestWaitForReadCancel()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForReadCancel");
	}

// Test 1.19 Destructor
CTestWaitForReadCancel::~CTestWaitForReadCancel()
	{
	}

// Test 1.19 Main Code
enum TVerdict CTestWaitForReadCancel::doTestStepL()
	{
	Log(_L("CTestWaitForReadCancel::doTestStepL() - Waiting for read status to be cancelled"));
	User::WaitForRequest(iUsbSuite->iReadStatus);
	Log(_L("CTestWaitForReadCancel::doTestStepL() - Signal status completed with %d"), iUsbSuite->iReadStatus.Int());
	TESTL(iUsbSuite->iReadStatus.Int() == KErrCancel);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.20
CTestPostReadThenCancel::CTestPostReadThenCancel()
	{
	// store the name of this test case
	iTestStepName = _L("ReadThenCancel");
	}

// Test 1.20 Destructor
CTestPostReadThenCancel::~CTestPostReadThenCancel()
	{
	}

// Test 1.20 Main Code
enum TVerdict CTestPostReadThenCancel::doTestStepL()
	{
	iUsbSuite->iCommPort.Read(iUsbSuite->iReadStatus, iUsbSuite->iReadBuffer);
	Log(_L("CTestPostReadThenCancel::doTestStepL() - Read posted on ACM comm port"));
	iUsbSuite->iCommPort.ReadCancel();
	Log(_L("CTestPostReadThenCancel::doTestStepL() - Cancelling read"));
	User::WaitForRequest(iUsbSuite->iReadStatus);
	Log(_L("CTestPostReadThenCancel::doTestStepL() - Read status completed with %d"), iUsbSuite->iReadStatus.Int());
	TESTL(iUsbSuite->iReadStatus.Int() == KErrCancel);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.21
CTestNotifyConfigChange::CTestNotifyConfigChange()
	{
	// store the name of this test case
	iTestStepName = _L("NotifyConfigChange");
	}

// Test 1.21 Destructor
CTestNotifyConfigChange::~CTestNotifyConfigChange()
	{
	}

// Test 1.21 Main Code
enum TVerdict CTestNotifyConfigChange::doTestStepL()
	{
	iUsbSuite->iCommPort.NotifyConfigChange(iUsbSuite->iNotifyConfigChangeStatus, iUsbSuite->iConfig);
	Log(_L("CTestNotifyConfigChange::doTestStepL() - Posted config change notification request"));
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.23
CTestNotifyBreak::CTestNotifyBreak()
	{
	// store the name of this test case
	iTestStepName = _L("NotifyBreak");
	}

// Test 1.23 Destructor
CTestNotifyBreak::~CTestNotifyBreak()
	{
	}

// Test 1.23 Main Code
enum TVerdict CTestNotifyBreak::doTestStepL()
	{
	iUsbSuite->iCommPort.NotifyBreak(iUsbSuite->iNotifyBreakStatus);
	Log(_L("CTestNotifyBreak::doTestStepL() - Posted break notification request"));
	TEST(ETrue);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.24
CTestWaitForBreak::CTestWaitForBreak()
	{
	// store the name of this test case
	iTestStepName = _L("WaitForBreak");
	}

// Test 1.24 Destructor
CTestWaitForBreak::~CTestWaitForBreak()
	{
	}

// Test 1.24 Main Code
enum TVerdict CTestWaitForBreak::doTestStepL()
	{
	Log(_L("CTestWaitForBreak::doTestStepL() - Waiting for break"));
	User::WaitForRequest(iUsbSuite->iNotifyBreakStatus);
	Log(_L("CTestWaitForBreak::doTestStepL() - Break status completed with %d"), iUsbSuite->iNotifyBreakStatus.Int());
	TESTL(iUsbSuite->iNotifyBreakStatus.Int() == KErrNone);
	return iTestStepResult;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test 1.25
CTestCloseAll::CTestCloseAll()
	{
	// store the name of this test case
	iTestStepName = _L("CloseAll");
	}

// Test 1.25 Destructor
CTestCloseAll::~CTestCloseAll()
	{
	}

// Test 1.25 Main Code
enum TVerdict CTestCloseAll::doTestStepL()
	{
	iUsbSuite->CloseAll();
	TESTL(ETrue);
	return iTestStepResult;
	}

