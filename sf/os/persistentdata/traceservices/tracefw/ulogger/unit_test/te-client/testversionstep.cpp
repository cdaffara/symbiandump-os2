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
 @file TestVersionStep.cpp
 @internalTechnology
*/
#include "testversionstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestVersionStep::~CTestVersionStep()
/**
 * Destructor
 */
	{
	}

CTestVersionStep::CTestVersionStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestVersionStep);
	}

TVerdict CTestVersionStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestVersionStep started"));
	 CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CTestVersionStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
			iSession->Connect();
			TVersion iVer = iSession->Version();
			if(iVer.iMajor != 0) //just dummy test
			{	
				TPtrC v(iVer.Name());
				INFO_PRINTF2(_L("Server version is %S"), &v);
				SetTestStepResult(EPass);
			}
			else
			{
				TPtrC v(iVer.Name());
				INFO_PRINTF2(_L("query version return wrong version : %S"), &v);
				SetTestStepResult(EFail);
			}
		}
		
	  return TestStepResult();
	}



TVerdict CTestVersionStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CTestVersionStep"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_DummySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
