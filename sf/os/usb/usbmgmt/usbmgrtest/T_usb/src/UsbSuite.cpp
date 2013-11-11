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
* This main DLL entry point for the T_Usb.dll
*
*/



// EPOC includes
#include <e32base.h>
#include <c32comm.h>

// Usb Test system includes
#include <networking/log.h>
#include "UsbStep.h"
#include "UsbSuite.h"

// Test case includes
#include "UsbTest1.h"
#include "UsbTest2.h"
#include "UsbManTest1.h"



EXPORT_C CTestSuiteUsb* NewTestSuiteUsb( void ) 
    {
	CTestSuiteUsb* ts = 0;
	TRAPD(err,ts = new (ELeave) CTestSuiteUsb);
	if (err == KErrNone)
		return ts;
	return 0;
    }

// destructor
CTestSuiteUsb::~CTestSuiteUsb()
	{
	}

// Add a test step into the suite
void CTestSuiteUsb::AddTestStepL(CTestStepUsb* aTestStep)
{
	// test steps contain a pointer back to the suite which owns them
	aTestStep->iUsbSuite = this; 

	// add the step using the base class method
	CTestSuite::AddTestStepL(aTestStep);
}


// second phase constructor for Usb test suite
// this creates all the Usb test steps and
// stores them inside CTestSuiteUsb
void CTestSuiteUsb::InitialiseL( void )
{
	iPortOpen		= EFalse;
	iCommServerOpen = EFalse;
	iUsbOpen		= EFalse;

	TInt ret = StartC32();
	if ( KErrNone != ret && KErrAlreadyExists != ret )
		{
		User::Leave( ret );
		}
	
	// store the name of this test suite
	OverrideSuiteName(_L("Usb"));

	// Add Test steps
	AddTestStepL(new (ELeave) CTestStartUsb);
	AddTestStepL(new (ELeave) CTestWaitForUsb);
	AddTestStepL(new (ELeave) CTestStartCsy);
	AddTestStepL(new (ELeave) CTestOpenDTEPort);
	AddTestStepL(new (ELeave) CTestOpenDCEPort);
	AddTestStepL(new (ELeave) CTestPostRead);
	AddTestStepL(new (ELeave) CTestPostWrite);
	AddTestStepL(new (ELeave) CTestCancelRead);
	AddTestStepL(new (ELeave) CTestCancelWrite);
	AddTestStepL(new (ELeave) CTestWaitForRead);
	AddTestStepL(new (ELeave) CTestWaitForWrite);
	AddTestStepL(new (ELeave) CTestPostReadOneOrMore);
	AddTestStepL(new (ELeave) CTestClosePort);
	AddTestStepL(new (ELeave) CTestCloseCommServer);
	AddTestStepL(new (ELeave) CTestCloseUsbServer);
	AddTestStepL(new (ELeave) CTestStopUsb);
	AddTestStepL(new (ELeave) CTestNotifySignalChange);
	AddTestStepL(new (ELeave) CTestWaitForSignalChange);
	AddTestStepL(new (ELeave) CTestWaitForReadCancel);
	AddTestStepL(new (ELeave) CTestPostReadThenCancel);
	AddTestStepL(new (ELeave) CTestNotifyConfigChange);
	AddTestStepL(new (ELeave) CTestNotifyBreak);
	AddTestStepL(new (ELeave) CTestWaitForBreak);
	AddTestStepL(new (ELeave) CTestCloseAll);

	// Test steps that require a configuration file
	AddTestStepL(new (ELeave) CTestSetSignalsToMark);
	AddTestStepL(new (ELeave) CTestSetSignalsToSpace);
	AddTestStepL(new (ELeave) CTestCheckSignals);
	AddTestStepL(new (ELeave) CTestWaitForReadFailure);
	AddTestStepL(new (ELeave) CTestWaitForWriteFailure);
	AddTestStepL(new (ELeave) CTestOpenPort);
	AddTestStepL(new (ELeave) CTestWaitForConfigChange);
	AddTestStepL(new (ELeave) CTestNotifyStateChange);
	AddTestStepL(new (ELeave) CTestWaitForStateChange);

	// UsbMan tests
	AddTestStepL(new (ELeave) CTestUsbManWaitForStateChange);
	AddTestStepL(new (ELeave) CTestUsbManNotifyStateChange);
}


// make a version string available for test system 
_LIT(KTxtVersion,"1.001");
TPtrC CTestSuiteUsb::GetVersion( void )
	{
	return KTxtVersion();
	}

void CTestSuiteUsb::CloseAll()
	{
	if (iPortOpen)
		{
		iCommPort.Close();
		iPortOpen = EFalse;
		}

	if (iCommServerOpen)
		{
		iCommServer.Close();
		iCommServerOpen = EFalse;
		}

	if (iUsbOpen)
		{
		iUsb.Close();
		iUsbOpen = EFalse;
		}

	if (iLddOpen)
		{
		iLdd.Close();
		iLddOpen = EFalse;
		}
	}
