/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* Example CTestStep derived implementation
*
*/



/**
 @file ReadConfigStep.cpp
*/
#include "readconfigstep.h"
#include "te_regsteptestsuitedefs.h"

CReadConfigStep::~CReadConfigStep()
/**
 * Destructor
 */
	{
	}

CReadConfigStep::CReadConfigStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KReadConfigStep);
	}

TVerdict CReadConfigStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// There is no initiation work so set this result to pass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CReadConfigStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("This step tests the function of GetXXXFromConfig."));
	SetTestStepResult(EFail);
	
	TPtrC TheString;
	TBool TheBool;
	TInt TheInt;
	TInt64 TheInt64;
	if(!GetStringFromConfig(ConfigSection(),KTe_RegressionSuiteString, TheString) ||
		!GetBoolFromConfig(ConfigSection(),KTe_RegressionSuiteBool,TheBool) ||
		!GetIntFromConfig(ConfigSection(),KTe_RegressionSuiteInt,TheInt)||
		!GetIntFromConfig(ConfigSection(),KTe_RegressionSuiteInt64,TheInt64)
		)
		{
		// Leave if there's any error.
		User::Leave(KErrNotFound);
		}
	else
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}



TVerdict CReadConfigStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
