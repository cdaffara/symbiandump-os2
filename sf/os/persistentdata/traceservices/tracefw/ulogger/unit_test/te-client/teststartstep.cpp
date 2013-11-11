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
 @file TestStartStep.cpp
 @internalTechnology
*/
#include "teststartstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestStartStep::~CTestStartStep()
/**
 * Destructor
 */
	{
	}

CTestStartStep::CTestStartStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestStartStep);
	}

TVerdict CTestStartStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
{
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult(); 
}

TVerdict CTestStartStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
{
	if (TestStepResult() == EPass)
	{
		TInt iErrCode = KErrNone;
		//iErrCode = iSession->Start();
		//if( KErrNone != iErrCode )
		//{
			CTestUloggerClientApiStepBase::prepareForStartTestL();
			iErrCode = iSession->ActivateOutputPlugin(_L8("uloggerfileplugin"));
			if(!( (iErrCode==KErrNone) || (iErrCode==KErrAlreadyExists) ))
			{
				INFO_PRINTF2(_L("Failed to Set ActiveOutput plugin : %d"), iErrCode);	
				SetTestStepResult(EFail);
			}
			else
			{
				iErrCode = iSession->Start();
				if( KErrNone == iErrCode || KErrInUse == iErrCode)
				{
					INFO_PRINTF1(_L("Logging started"));		
					SetTestStepResult(EPass);
				}
				else if(KErrNotFound == iErrCode)
				{
					INFO_PRINTF2(_L("Logging failed to start with error KErrNotFound (%d)"), iErrCode);
					SetTestStepResult(EPass);
				}
				else
				{
				 	INFO_PRINTF2(_L("Logging failed to start with error: %d"), iErrCode);
				 	SetTestStepResult(EFail);
				}
			}
	//}
	//else
	//{
		//INFO_PRINTF1(_L("Session returned sucess without connection"));
		//SetTestStepResult(EFail);
	//}
	}

	return TestStepResult();
}

TVerdict CTestStartStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
{
	INFO_PRINTF1(_L("TestStartStep completed"));
	CTestUloggerClientApiStepBase::doTestStepPostambleL();
	return TestStepResult();
}
