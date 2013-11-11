// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
*/
#include "comparestep.h"
#include "tef_schsvr_bursuitedefs.h"

CCompareStep::~CCompareStep()
/**
 * Destructor
 */
	{
	}

CCompareStep::CCompareStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KCompareStep);
	}

TVerdict CCompareStep::doTestStepL()
	{
	// expected result is always the execution of the task, the expected failures will be checked in the test script 
    SetTestStepResult(EPass);
    
	if (iTrigger == 1) // condition trigger
		{
		INFO_PRINTF1(_L("Condition schedule test for BUR"));		
		INFO_PRINTF1(_L("Set test property to trigger task"));
		// Set property to trigger task execution
		User::LeaveIfError(RProperty::Set(KTestUid, KTestKey1, KKeyValue));
		}
	else if (iTrigger == 2) // time trigger
		{
		INFO_PRINTF1(_L("Time schedule test for BUR"));		
		INFO_PRINTF1(_L("Set clock to 4 s before task due"));
	    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am
		}

    TInt ret = iSem.WaitL(KTaskLaunchTimeout);
    INFO_PRINTF2(_L("Sema wait return code is %d"), ret);
    TEST(ret == KErrNone);

	return TestStepResult();
	}

TVerdict CCompareStep::doTestStepPreambleL()
	{
	_LIT(KTriggerTask, "TriggerTask");
	
	GetParameterL(KTriggerTask, iTrigger);

	if (iTrigger)
		{
		// Create global sema
		iSem.CreateL();
		}
		
	return TestStepResult();
	}

TVerdict CCompareStep::doTestStepPostambleL()
	{
	CleanupHelpers::KillProcess(KMinimalTaskHandler);

	if (iTrigger)
		{
		iSem.Close();
		}

	// Reset the value so as not to affect next test
	RProperty::Set(KTestUid, KTestKey1, 0);
    
	return TestStepResult();
	}

