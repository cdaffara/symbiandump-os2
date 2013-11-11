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
// UloggerFilePluginTest.cpp
// 
//

#include "uloggersysstarttest.h"
using namespace Ulogger;



/*
 * TestStart: Test the start method ???
 *
 * Expected Verdict: PASS/FAIL/PANIC
 *
 * Prerequisites:
 *
 * Description: Invoke CULoggerSysStarter::Start() and check that the start metod works
 *
 */
void CSysStart0Step::TestStart()
	{
	INFO_PRINTF1(_L("Checking that CSysStart0Step::Start works fine"));

	INFO_PRINTF1(_L("Writing 'Test' to the log using CULoggerSysStarter::Write()"));
 	Ulogger::SysStart();
	
	}



CSysStart0Step::~CSysStart0Step()
	{
	}

CSysStart0Step::CSysStart0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KSysStart0Step);
	}

TVerdict CSysStart0Step::doTestStepPreambleL()
	{
	CTe_sysstartSuiteStepBase::doTestStepPreambleL();
	
	INFO_PRINTF1(_L("Connecting to file server"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSysStart0Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{	
	  if (TestStepResult()==EPass)
		{
		TestStart();

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

TVerdict CSysStart0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_sysstartSuiteStepBase::doTestStepPostambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}
