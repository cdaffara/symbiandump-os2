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
//

/**
 @file Te_uloggerclientSuiteStepBase.cpp
 @internalTechnology
*/

#include "te_uloggerclientsuitestepbase.h"
#include "te_uloggerclientsuitedefs.h"

// Device driver constants

TVerdict CTestUloggerClientApiStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	
		INFO_PRINTF1(_L("Instantiating RULogger object"));
		iSession = new (ELeave) Ulogger::RULogger();
		if(iSession)
		{
			INFO_PRINTF1(_L("OK"));
			SetTestStepResult(EPass);
		}
		else
		{
			INFO_PRINTF1(_L("RULogger object construction failed"));
			SetTestStepResult(EFail);
		}
		return TestStepResult();
	}



TVerdict CTestUloggerClientApiStepBase::prepareForStartTestL()
/**
 * @return - TVerdict
 * It is used to prepare the framework for testing TestStartStep.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
{
	
	//continue from previous step
	if(TestStepResult() == EPass)
	{
		INFO_PRINTF1(_L("before iSession->Connect()"));
		if(KErrNone == iSession->Connect())
		{
			INFO_PRINTF1(_L("Client session connected"));
			SetTestStepResult(EPass);
		}
		else
		{
			INFO_PRINTF1(_L("Client session connection failed"));
			SetTestStepResult(EFail);
		}
	}
	else
	{
		INFO_PRINTF1(_L("Dependency test failed, CTestStartStep will fail"));
		SetTestStepResult(EFail);
	}
	
	return TestStepResult();
}
 TVerdict CTestUloggerClientApiStepBase::prepareForStopTestL()
/**
 * @return - TVerdict
 * It is used to prepare the framework for testing TestStopStep.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
{
	prepareForStartTestL();
	if(TestStepResult() == EPass)
	{
		//start server here
		INFO_PRINTF1(_L("beffore iSession->Start()"));
		TInt iErrCode = iSession->Start();
		if( KErrNone == iErrCode || KErrInUse == iErrCode  )
		{			
			SetTestStepResult(EPass);
		}
		else if(KErrNotFound == iErrCode)
		{
			INFO_PRINTF2(_L("Logging failed to strat with error : %d"), iErrCode);
			SetTestStepResult(EPass);
		} 	
		else
		{
			INFO_PRINTF2(_L("Logging failed to strat with error : %d"), iErrCode);
			SetTestStepResult(EFail);
		}
	}
	
	return TestStepResult();
}


TVerdict CTestUloggerClientApiStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
{

	if(TestStepResult() == EPass)
	{
		INFO_PRINTF1(_L("Deleting RULogger object"));
		if(iSession)
		{
			INFO_PRINTF1(_L("before delete iSession"));
			delete 	iSession;
		    SetTestStepResult(EPass);  // destruction sucessfull
		}
		else
		{
			SetTestStepResult(EFail);
		}
	}
	return TestStepResult();
}
CTestUloggerClientApiStepBase::~CTestUloggerClientApiStepBase()
	{
	}

CTestUloggerClientApiStepBase::CTestUloggerClientApiStepBase():iSession(0)
	{
	}
