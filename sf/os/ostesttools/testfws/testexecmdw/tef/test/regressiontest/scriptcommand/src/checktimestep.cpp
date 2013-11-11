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
 @file checkresultstep.cpp
*/
#include "checktimestep.h"

CCheckTimeStep::~CCheckTimeStep()
/**
 * Destructor
 */
	{
	}

CCheckTimeStep::CCheckTimeStep(CTe_RegScriptCommandServer* aServer) : iServer(aServer)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCheckTimeStep);
	}

TVerdict CCheckTimeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates opening up a handle to the data saved at RChunk base using template class
 * Overwrite shared data with values and also demonstrates the locking mechanism
 */
	{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("Check time."));
	_LIT(KTimeSeconds, "TimeSeconds");
	TInt timeSeconds;
	if (!GetIntFromConfig(ConfigSection(), KTimeSeconds(), timeSeconds))
		{
		ERR_PRINTF1(_L("Error get time seconds from config file."));
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	TTime timeNow;
	timeNow.HomeTime();
	TDateTime dateTime = timeNow.DateTime();
	TTimeIntervalSeconds secs;
	TInt err = timeNow.SecondsFrom(Server()->StartTime(), secs);
	if (KErrNone != err)
		{
		ERR_PRINTF2(_L("Error get seconds interval, error code %d"), err);
		}
	else if ((secs.Int()-timeSeconds)<=1 || (timeSeconds-secs.Int())<=1)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}
