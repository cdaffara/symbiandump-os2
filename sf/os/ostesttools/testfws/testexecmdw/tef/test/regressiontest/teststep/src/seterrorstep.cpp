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
 @file SetErrorStep.cpp
*/
#include "seterrorstep.h"
#include "te_regsteptestsuitedefs.h"

CSetErrorStep::~CSetErrorStep()
/**
 * Destructor
 */
	{
	}

CSetErrorStep::CSetErrorStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KSetErrorStep);
	}



TVerdict CSetErrorStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);
	TPtrC TheString;
	TInt TheInt;
	if(!GetIntFromConfig(ConfigSection(),KTe_RegStepTestSuiteInt, TheInt))
		{
		// Leave if there's any error.
		User::Leave(KErrNotFound);
		}
	else
		{
		INFO_PRINTF2(_L("The Error Code is %d"), TheInt); // Block end
		}

	SetTestStepError(TheInt);
	SetTestStepResult((TVerdict)TheInt);

	return TestStepResult();
	}



