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
 @file TestSetSecondaryFilterAllStep.cpp
 @internalTechnology
*/
#include "testsetsecondaryfilterallstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestSetSecondaryFilterAllStep::~CTestSetSecondaryFilterAllStep()
/**
 * Destructor
 */
	{
	}

CTestSetSecondaryFilterAllStep::CTestSetSecondaryFilterAllStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestSetSecondaryFilterAllStep);
	}

TVerdict CTestSetSecondaryFilterAllStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestSetSecondaryFilterAllStep started"));
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CTestSetSecondaryFilterAllStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
{
	
	if(TestStepResult() == EPass)
	{
		iSession->Connect();
		TRAPD( err, iSession->SetSecondaryFilteringEnabled(EFalse));
		if( KErrNone ==  err )
		{
			INFO_PRINTF1(_L("All secondary filters have been enabled"));
			SetTestStepResult(EPass);
		}
		else
		{
			INFO_PRINTF2(_L("failed to enable secondary filters, error code %d"), err);
			SetTestStepResult(EFail);
		}
	}
	if(TestStepResult() == EPass)
	{
		iSession->Connect();
		TRAPD( err, iSession->SetSecondaryFilteringEnabled(ETrue));
		if( KErrNone ==  err )
		{
			INFO_PRINTF1(_L("secondary filters are in current mode"));
			TRAPD( err, iSession->SetSecondaryFilteringEnabled(EFalse));
			if( KErrNone ==  err )
			{
				INFO_PRINTF1(_L("All secondary filters have been enabled"));
				SetTestStepResult(EPass);
			}
			else
			{
				INFO_PRINTF2(_L("failed to enable secondary filters, error code %d"), err);
				SetTestStepResult(EFail);
			}
		}
		else
		{
			INFO_PRINTF2(_L("failed to Set current mode %d"), err);
			SetTestStepResult(EFail);
		}
	}
	return TestStepResult();
}



TVerdict CTestSetSecondaryFilterAllStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestSetSecondaryFilterAllStep completed"));
	CTestUloggerClientApiStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
