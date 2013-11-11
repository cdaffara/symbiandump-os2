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
// Example CTestStep derived implementation
// 
//

/**
 @file conn_closeStep.cpp
*/
#include "conn_closeStep.h"
#include "Te_RFileLoggerSuiteDefs.h"

Cconn_closeStep::~Cconn_closeStep()
/**
 * Destructor
 */
	{
	}

Cconn_closeStep::Cconn_closeStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kconn_closeStep);
	}

TVerdict Cconn_closeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_RFileLoggerSuiteStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict Cconn_closeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if(TestStepResult()==EFail) return TestStepResult();
	TInt err;
	INFO_PRINTF1(_L("1.  Connect test ..."));
	RFileFlogger ilogger;  
	err=ilogger.Connect();
	if(err!=KErrNone) 
	{
		ERR_PRINTF2(_L("Connect error. The error number = %d"),err);
		SetTestStepResult(EFail);
		return TestStepResult();
	}
	else  INFO_PRINTF1(_L("Connect test passed"));
	err=ilogger.Connect();
	INFO_PRINTF1(_L("2.  Connect again test after connected"));
	if(err==KErrAlreadyExists)
	{
		INFO_PRINTF2(_L("Test pass. Return value = %d as expected"),err);
	}
	else  
	{
		ERR_PRINTF2(_L("Failed. the error = %d"), err);
		SetTestStepResult(EFail);
		return TestStepResult();

	}
	INFO_PRINTF1(_L("3.  Close test..."));
	ilogger.Close();
	err=ilogger.Connect();
	if(!err)
	{
		INFO_PRINTF1(_L("Close test passed"));
	}
	else  
	{
		ERR_PRINTF2(_L("Close error value = %d"),err);
		SetTestStepResult(EFail);
		return TestStepResult();

	}
	INFO_PRINTF1(_L("4.  CreateLog test..."));	
	err=ilogger.CreateLog(KLogFile1,RFileFlogger::ELogModeAppend);
	if(!err)
	{
		INFO_PRINTF1(_L("CreateLog test passed"));
	}
	else  
	{
		ERR_PRINTF2(_L("CreateLog error value = %d"),err);
		SetTestStepResult(EFail);
		return TestStepResult();

	}
	ilogger.Close();

	return TestStepResult();
	}



TVerdict Cconn_closeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
