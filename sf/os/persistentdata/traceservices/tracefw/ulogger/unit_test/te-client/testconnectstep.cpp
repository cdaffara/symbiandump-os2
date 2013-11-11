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
 @file TestConnectStep.cpp
 @internalTechnology
*/
#include "testconnectstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestConnectStep::~CTestConnectStep()
/**
 * Destructor
 */
	{
	}

CTestConnectStep::CTestConnectStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestConnectStep);
	}

TVerdict CTestConnectStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
{
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	if(TestStepResult() == EPass)
	{
		INFO_PRINTF1(_L("Initailizing TestConnectStep"));
		SetTestStepResult(EPass); //No initialization required just pass the step
	}
	
	return TestStepResult();
}


TVerdict CTestConnectStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
{

	TInt iErrCount = 0;
	if (TestStepResult()==EPass)
		{
			if(iSession)
			{	
				if(KErrNone == iSession->Connect())
				{
					//connect again to test simultaneous connections
					for(TInt i = 0; i <= 20;i++ )
					 {
					 	TInt iConnectRes = iSession->Connect();
					 	if( iConnectRes != KErrAlreadyExists && iConnectRes != KErrNone )
					 	{
					 		iErrCount++;
					 		break;
					 	}
						
					 }
					 if(iErrCount == 0)
					 {
					 	SetTestStepResult(EPass);
					 }
					 else
					 {
					 	TBuf<128> iInfoMsg;
					 	iInfoMsg.AppendFormat(_L("Simultaneous connection failed on %d atempt"), iErrCount);
					 	INFO_PRINTF1(iInfoMsg);
					 	SetTestStepResult(EFail);
					 }
				}
				else
				{
					INFO_PRINTF1(_L("connection to server failed"));
					SetTestStepResult(EFail);
				}
			}
		}
	  return TestStepResult();
}



TVerdict CTestConnectStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
{
	INFO_PRINTF1(_L("TestConnectStep completed"));

	iSession->Close(); //close the session in either case
	CTestUloggerClientApiStepBase::doTestStepPostambleL();

	return TestStepResult();
}
