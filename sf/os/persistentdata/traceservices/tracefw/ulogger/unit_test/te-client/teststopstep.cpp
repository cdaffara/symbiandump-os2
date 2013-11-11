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
 @file TestStopStep.cpp
 @internalTechnology
*/
#include "teststopstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestStopStep::~CTestStopStep()
/**
 * Destructor
 */
	{
	}

CTestStopStep::CTestStopStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestStopStep);
	}

TVerdict CTestStopStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		CTestUloggerClientApiStepBase::doTestStepPreambleL();
		return TestStepResult();
	}


TVerdict CTestStopStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  CTestUloggerClientApiStepBase::prepareForStopTestL();	
	  if (TestStepResult()== EPass)
		{
			TInt iErrCode = iSession->Stop();
			if(KErrNone == iErrCode || KErrNotReady == iErrCode)
			{
				INFO_PRINTF1(_L("Logging has been stoped"));
				SetTestStepResult(EPass);
			}
			else
			{
				INFO_PRINTF2(_L("Stop Logging failed with error : %d"), iErrCode);
				SetTestStepResult(EFail);
			}
		}
		
	  return TestStepResult();
	}



TVerdict CTestStopStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
{
	INFO_PRINTF1(_L("TestStopStep completed"));

	// SetTestStepResult(EPass);		// or EFail
	CTestUloggerClientApiStepBase::doTestStepPostambleL();
	return TestStepResult();
}
