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
 @file te_regperformancereleasememteststep.cpp
 @internalTechnology
*/
#include <hal.h>
#include "te_regperformancereleasememteststep.h"

CTe_RegPerformanceReleaseMemTestStep::~CTe_RegPerformanceReleaseMemTestStep()
/**
 * Destructor
 */
	{
	
	}

CTe_RegPerformanceReleaseMemTestStep::CTe_RegPerformanceReleaseMemTestStep(RChunk& aChunk) : iChunk(aChunk)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTe_RegPerformanceReleaseMemTestStep);
	}

TVerdict CTe_RegPerformanceReleaseMemTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTe_RegPerformanceReleaseMemTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("Release Low Ram Condition"));

	TInt totalFree =0;
	User::LeaveIfError(HAL::Get(HALData::EMemoryRAMFree, totalFree));
	INFO_PRINTF2(_L("RAM status before releasing: Total Free RAM:%D bytes"), totalFree);

	iChunk.Close();
	User::LeaveIfError(HAL::Get(HALData::EMemoryRAMFree, totalFree));

	INFO_PRINTF2(_L("RAM status after releasing: Total Free RAM:%D bytes"), totalFree);

	return TestStepResult();
	}



TVerdict CTe_RegPerformanceReleaseMemTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
