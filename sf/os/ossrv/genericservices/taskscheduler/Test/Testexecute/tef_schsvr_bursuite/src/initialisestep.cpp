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
#include "initialisestep.h"
#include "tef_schsvr_bursuitedefs.h"

CInitialiseStep::~CInitialiseStep()
/**
 * Destructor
 */
	{
	}

CInitialiseStep::CInitialiseStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KInitialiseStep);
	}

TVerdict CInitialiseStep::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);
        
    // Delete old files.
    SchSvrHelpers::DeleteScheduleFilesL();
    
	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

    SetTestStepResult(EPass);
    return TestStepResult();
	}

TVerdict CInitialiseStep::doTestStepL()
	{
    SetTestStepResult(EFail);	

 	INFO_PRINTF1(_L("Scheduler BUR test Init"));

    TSchedulerItemRef ref,ref2;

	_LIT(KBackupCondition, "AddCondition");
	
	TPtrC data;
	if (!GetStringFromConfig(ConfigSection(), KBackupCondition, data))
		{
		ERR_PRINTF1(KMissingInputParam);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	_LIT(KTimeStr, "Time");
	_LIT(KTestStr, "Test");
	_LIT(KBackupStr, "Backup");
	_LIT(KRestoreStr, "Restore");
	_LIT(KRestoreStr1, "Restore1");

 	if (data == KTimeStr)
		{
		// Creates a time based schedule    
		_LIT(KSchTestName, "Persistent Schedule Test (Time)");				
		
	    // This is the time when we want the time-based schedule to fire
	    TDateTime datetime1(2000, EJanuary, 1, 10, 0, 0, 0);
	    TTsTime dueTime(datetime1, EFalse);

		AddTimeScheduleL(KSchTestName, dueTime, ref);
		_LIT(KSchTestName2, "Transient Time Schedule Test");				
		AddTransientTimeScheduleL(KSchTestName2, dueTime, ref2);
		}
	else
		{
		// Creates a condition based schedule    		
		_LIT(KSchTestName, "Persistent Schedule Test (Condition)");		
		
	    // If condition not met, task is run one year from now.
	    TDateTime datetime1(2001, EJanuary, 1, 10, 0, 0, 0);
	    TTsTime dueTime(datetime1, EFalse);

	    CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(2);
	    CleanupStack::PushL(conditionList);
		
		if (data == KTestStr)
			{
			CreatePSKeyL(KTestKey1);
			AddTestConditionL(*conditionList, KTestKey1);
			AddConditionScheduleL(KSchTestName, dueTime, ref, *conditionList);
			}
		else if (data == KBackupStr)
			{
			// Only create sema if condition not triggered by test but by SBE
			iSem.CreateL();
			CreatePSKeyL(conn::KUidBackupRestoreKey);
			AddBackupConditionL(*conditionList);
			AddConditionScheduleL(KSchTestName, dueTime, ref, *conditionList);
			}
		else if (data == KRestoreStr || data == KRestoreStr1)
			{
			// Only create sema if condition not triggered by test but by SBE
			TRAPD(ignoreifalreadycreated, iSem.CreateL());
			if (ignoreifalreadycreated != KErrNone && ignoreifalreadycreated != KErrAlreadyExists)
				{
				User::Leave(ignoreifalreadycreated);
				}
			CreatePSKeyL(conn::KUidBackupRestoreKey);				
			AddRestoreConditionL(*conditionList);

			if (data==KRestoreStr1)
				{
				AddTransientConditionScheduleL(KSchTestName, dueTime,*conditionList);
				}
			else
				{
				AddConditionScheduleL(KSchTestName, dueTime, ref, *conditionList);				
				}				
			}
		else
			{
			SetTestStepResult(EFail);
			User::Leave(KErrCorrupt);
			}
		
		CleanupStack::PopAndDestroy(conditionList);    
		}

	SetTestStepResult(EPass);
	
	return TestStepResult();
	}

