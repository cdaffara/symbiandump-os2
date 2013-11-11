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
#include "oomstep.h"
#include "te_regsteptestsuitedefs.h"

_LIT(KIterationCount, "IterationCount");

COOMTestStep::~COOMTestStep()
/**
 * Destructor
 */
	{
	}

COOMTestStep::COOMTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KOOMTestStep);
	}

TVerdict COOMTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EFail);
	if (GetIntFromConfig(ConfigSection(), KIterationCount, iIterationCount))
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}


TVerdict COOMTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);
	iIterationIndex++;
	for (TInt i=0; i<iIterationCount; i++)
		{
		TInt* testVar = new (ELeave) TInt;
		delete testVar;
		}
	
	if (iIterationIndex == iIterationCount+2)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}



TVerdict COOMTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
