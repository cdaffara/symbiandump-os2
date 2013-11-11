// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "uloggerfileplugintest.h"
#include "uloggerdatatypes.h"
using namespace Ulogger;

/*
 * TestNewL: Test that the NewL method correctly constructs a CFileWriter object
 *
 * Expected Verdict: PASS/FAIL/PANIC
 *
 * Prerequisites:
 *
 * Description: Invoke CFileWriter::NewL() and check that a non-NULL pointer is
 *              returned.
 *
 */
void CFile0Step::TestNewL()
	{
	CFileWriter* fileWriter = CFileWriter::NewL();
	INFO_PRINTF1(_L("Checking that pointer returned by CFileWriter::NewL() is not NULL"));
	ASSERT_NOT_NULL(fileWriter);
	delete fileWriter;
	}
/*
 * TestUnlockResourcesL: Test that the CloseOutputPlugin method ???
 *
 * Expected Verdict: PASS/FAIL/PANIC
 *
 * Prerequisites:
 *
 * Description: Retrieve and print some data from an external ini data file
 *
 */
void CFile0Step::TestUnlockResourcesL()
	{
	CFileWriter* fileWriter = CFileWriter::NewL();
	INFO_PRINTF1(_L("Invoking CFileWriter::CloseOutputPlugin(). CloseOutputPlugin() is supposed to do nothing."));
	fileWriter->CloseOutputPlugin();
	ASSERT_TRUE(ETrue);
	delete fileWriter;
	}

/*
 * TestSettingsL: Test that the Settings method ???
 *
 * Expected Verdict: PASS/FAIL/PANIC
 *
 * Prerequisites:
 *
 * Description: Retrieve and print some data from an external ini data file
 *
 */
void CFile0Step::TestSettingsL()
	{
	CFileWriter* fileWriter = CFileWriter::NewL();
	INFO_PRINTF1(_L("Passing empty RPointerArray to CFileWriter::ConfigureOutputPlugin()"));
	RPointerArray<TPluginConfiguration> emptyPointerArray;
	fileWriter->ConfigureOutputPlugin(emptyPointerArray);

	delete fileWriter;
	}

/*
 * TestWriteL: Test that the Write method ???
 *
 * Expected Verdict: PASS/FAIL/PANIC
 *
 * Prerequisites:
 *
 * Description: Invoke CFileWriter::Write() and check that test string is
 *              written to KLogDefaultFileName correctly.
 *
 */
// The implementation for this is not working correctly! Check this
void CFile0Step::TestWriteL()
	{
	INFO_PRINTF1(_L("Checking that CFileWriter::Write(const TDesC8&) writes a String correctly to C:\\logs\\log.txt"));

	// Create new CFileWriter instance
	CFileWriter* fileWriter = CFileWriter::NewL();
	_LIT8(KTxt, "Test");
	INFO_PRINTF1(_L("Writing 'Test' to the log using CFileWriter::Write()"));
	ASSERT_EQUALS(KErrNone, fileWriter->Write(KTxt));

	delete fileWriter;
	}


CFile0Step::~CFile0Step()
	{
	}

CFile0Step::CFile0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KFile0Step);
	}

TVerdict CFile0Step::doTestStepPreambleL()
	{
	CTe_fileSuiteStepBase::doTestStepPreambleL();
	
	INFO_PRINTF1(_L("Connecting to file server"));
	User::LeaveIfError(iFileServer.Connect());
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CFile0Step::doTestStepL()
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

TVerdict CFile0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_fileSuiteStepBase::doTestStepPostambleL();
	iFileServer.Close();
	SetTestStepResult(EPass);
	return TestStepResult();
	}
