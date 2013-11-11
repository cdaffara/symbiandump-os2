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
 @file checkprogramstep.cpp
*/
#include "checkprogramstep.h"

CCheckProgramStep::~CCheckProgramStep()
/**
 * Destructor
 */
	{
	}

CCheckProgramStep::CCheckProgramStep()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCheckProgramStep);
	}

TVerdict CCheckProgramStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates opening up a handle to the data saved at RChunk base using template class
 * Overwrite shared data with values and also demonstrates the locking mechanism
 */
	{
	SetTestStepResult(EFail);
	_LIT(KProcessName, "ProcessName");
	_LIT(KExpectedRetValue, "ExpectedRetValue");
	
	RProcess process;
	TPtrC processName;
	TInt expectedRetValue;
	if (!GetStringFromConfig(ConfigSection(), KProcessName(), processName)
			|| !GetIntFromConfig(ConfigSection(), KExpectedRetValue(), expectedRetValue))
		{
		ERR_PRINTF1(_L("Failed when getting parameters from config file."));
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	
	TInt err = process.Open(processName);
	if (expectedRetValue == err)
		{
		SetTestStepResult(EPass);
		}
	else if (KErrNone == err)
		{
		TExitType exitType = process.ExitType();
		if ((EExitKill==exitType && KErrNotFound==expectedRetValue)
				|| EExitPending==exitType && KErrNone==expectedRetValue)
			{
			SetTestStepResult(EPass);
			}
		}
	
	return TestStepResult();
	}
