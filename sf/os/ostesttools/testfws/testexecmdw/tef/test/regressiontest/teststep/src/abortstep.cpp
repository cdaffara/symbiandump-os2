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
 @file WriteStringStep.cpp
*/
#include "abortstep.h"
#include "te_regsteptestsuitedefs.h"

CAbortTestStep::~CAbortTestStep()
/**
 * Destructor
 */
	{
	}

CAbortTestStep::CAbortTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KAbortTestStep);
	}

TVerdict CAbortTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// There is no initiation work so set this result to pass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CAbortTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("This test step should be aborted!"));
	SetTestStepResult(EFail);
	User::After(100000000);
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CAbortTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
