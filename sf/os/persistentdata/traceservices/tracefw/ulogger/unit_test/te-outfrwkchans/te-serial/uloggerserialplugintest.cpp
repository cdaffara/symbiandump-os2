// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "uloggerserialplugintest.h"
#include "uloggerdatatypes.h"
using namespace Ulogger;


/*
 * TestNewL: Test that the NewL method correctly constructs a CSerialWriter object
 *
 * Expected Verdict: PASS/FAIL/PANIC
 *
 * Prerequisites:
 *
 * Description: Invoke CSerialWriter::NewL() and check that a non-NULL pointer is
 *              returned.
 *
 */
void CSerial0Step::TestNewL()
	{
	Ulogger::CSerialWriter* serialWriter = Ulogger::CSerialWriter::NewL();
	INFO_PRINTF1(_L("Checking that pointer returned by CSerialWriter::NewL() is not NULL"));
	ASSERT_NOT_NULL(serialWriter);
	delete serialWriter;
	}

/*
 * TestNewL: Test that the CloseOutputPlugin method ???
 *
 * Expected Verdict: PASS/FAIL/PANIC
 *
 * Prerequisites:
 *
 * Description: Retrieve and print some data from an external ini data file
 *
 */
void CSerial0Step::TestUnlockResourcesL()
	{
	Ulogger::CSerialWriter* serialWriter = Ulogger::CSerialWriter::NewL();
	INFO_PRINTF1(_L("Invoking CSerialWriter::CloseOutputPlugin(). CloseOutputPlugin() is supposed to do nothing."));
	serialWriter->CloseOutputPlugin();
	ASSERT_TRUE(ETrue);
	delete serialWriter;
	}

/*
 * TestNewL: Test that the Settings method ???
 *
 * Expected Verdict: PASS/FAIL/PANIC
 *
 * Prerequisites:
 *
 * Description: Retrieve and print some data from an external ini data file
 *
 */
void CSerial0Step::TestSettingsL()
	{
	Ulogger::CSerialWriter* serialWriter = Ulogger::CSerialWriter::NewL();
	INFO_PRINTF1(_L("Passing empty RPointerArray to CSerialWriter::ConfigureOutputPlugin()"));
	RPointerArray<TPluginConfiguration> emptyPointerArray;
	serialWriter->ConfigureOutputPlugin(emptyPointerArray);

	//
	// Need More thorough testing of CSerialWriter::ConfigureOutputPlugin()
	//

	delete serialWriter;
	}

/*
 * TestNewL: Test that the Write method ???
 *
 * Expected Verdict: PASS/FAIL/PANIC
 *
 * Prerequisites:
 *
 * Description: Invoke CSerialWriter::Write() and check that test string is
 *              written to KLogDefaultSerialName correctly.
 *
 */
// The implementation for this is not working correctly! Check this
void CSerial0Step::TestWriteL()
	{
	INFO_PRINTF1(_L("Checking that CSerialWriter::Write(const TDesC8&) writes a string correctly to the serial port"));

	// Create new CSerialWriter instance and write a test string to it
	Ulogger::CSerialWriter* serialWriter = Ulogger::CSerialWriter::NewL();
	_LIT8(KTxt, "Test");
	INFO_PRINTF1(_L("Writing 'Test' to the log using CSerialWriter::Write()"));
	ASSERT_EQUALS(KErrNone, serialWriter->Write(KTxt));
	
	//
	// More thorough testing of CSerialWriter::Write()
	//

	// Destroy CSerialWriter instance
	delete serialWriter;
	}



CSerial0Step::~CSerial0Step()
	{
	}

CSerial0Step::CSerial0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KSerial0Step);
	}

TVerdict CSerial0Step::doTestStepPreambleL()
	{
	CTe_serialSuiteStepBase::doTestStepPreambleL();
	
	INFO_PRINTF1(_L("Connecting to file server"));
	User::LeaveIfError(iFileServer.Connect());
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSerial0Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{	
	  if (TestStepResult()==EPass)
		{
		TestNewL();
		TestUnlockResourcesL();
		TestSettingsL();
		TestWriteL();
		
		if(iErrors == 0)
			SetTestStepResult(EPass);
		else
			{
			SetTestStepResult(EFail);
			TBuf<64> buf;
			INFO_PRINTF1(_L("********"));
			buf.AppendFormat(_L("%d errors were found!"), iErrors);
			INFO_PRINTF1(buf); 
			INFO_PRINTF1(_L("********"));
			}
		}
	  return TestStepResult();
	}

TVerdict CSerial0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_serialSuiteStepBase::doTestStepPostambleL();
	iFileServer.Close();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


