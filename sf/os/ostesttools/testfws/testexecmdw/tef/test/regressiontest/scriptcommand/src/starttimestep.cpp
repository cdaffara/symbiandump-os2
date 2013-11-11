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
#include "starttimestep.h"

CStartTimeStep::~CStartTimeStep()
/**
 * Destructor
 */
	{
	}

CStartTimeStep::CStartTimeStep(CTe_RegScriptCommandServer* aServer) : iServer(aServer)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KStartTimeStep);
	}

TVerdict CStartTimeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates opening up a handle to the data saved at RChunk base using template class
 * Overwrite shared data with values and also demonstrates the locking mechanism
 */
	{
	SetTestStepResult(EFail);

	INFO_PRINTF1(_L("Record start time."));
	TTime timeNow;
	timeNow.HomeTime();
	Server()->StartTime() = timeNow;
	SetTestStepResult(EPass);
	
	return TestStepResult();
	}
