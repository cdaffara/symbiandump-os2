// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file TestSetActiveOutputPluginStep.cpp
 @internalTechnology
*/
#include "testdeactivateoutputpluginstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestDeactivateOutputPluginStep::~CTestDeactivateOutputPluginStep()
/**
 * Destructor
 */
	{
	}

CTestDeactivateOutputPluginStep::CTestDeactivateOutputPluginStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestDeactivateOutputPluginStep);
	}

TVerdict CTestDeactivateOutputPluginStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestDeactivateOutputPluginStep"));
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CTestDeactivateOutputPluginStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("The functionality 'deactivate output plug-in' is no longer supported (?)"));

	if (TestStepResult()==EPass)
		{
		iSession->Connect();
		INFO_PRINTF1(_L("TestDeactivateOutputPluginStep connecting to server"));
		TInt err=iSession->ActivateOutputPlugin(_L8("uloggerfileplugin"));
//		if(err==KErrNone || err==KErrAlreadyExists)
//			User::LeaveIfError(iSession->DeActivateOutputPlugin(_L("uloggerfileplugin")));
//		else 
//			User::LeaveIfError(err);
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CTestDeactivateOutputPluginStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	INFO_PRINTF1(_L("TestDeactivateOutputPluginStep completed"));
	return TestStepResult();
	}
