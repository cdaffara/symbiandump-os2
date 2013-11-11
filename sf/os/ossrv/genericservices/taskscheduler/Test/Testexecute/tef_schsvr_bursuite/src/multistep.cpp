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
#include "multistep.h"
#include "tef_schsvr_bursuitedefs.h"

CMultiStep::~CMultiStep()
/**
 * Destructor
 */
	{
	}

CMultiStep::CMultiStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KMultiStep);
	}

void CMultiStep::AddTimeSchedulesL()
	{
	// Creates a time based schedule    
	_LIT(KSchTestName1, "Multiple Persistent Schedule Test1");				
	_LIT(KSchTestName2, "Multiple Persistent Schedule Test2");
	
    // This is the time when we want the first time-based schedule to fire
    TDateTime datetime1(2000, EJanuary, 1, 10, 0, 0, 0); // 10:00.00 am
    TTsTime dueTime(datetime1, EFalse);

	AddTimeScheduleL(KSchTestName1, dueTime, iRef1);
	
    TDateTime datetime2(2000, EJanuary, 1, 10, 0, 2, 0); // 10:00.02 am
    TTsTime dueTime2(datetime2, EFalse);

	AddTimeScheduleL(KSchTestName2, dueTime2, iRef2);
	}

TVerdict CMultiStep::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);
        
	CTefSchsvrBURSuiteSimStepBase::doTestStepPreambleL();        
    
	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

    SetTestStepResult(EPass);
    return TestStepResult();
	}

TVerdict CMultiStep::doTestStepL()
	{
    SetTestStepResult(EFail);
    
	INFO_PRINTF1(_L("Multiple task execution test during BUR"));

	AddTimeSchedulesL();
	
	// Simulate backup
	Simulate_StartBackupL();
	// Set time to 4 seconds before 1st task	
	SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am
	// wait 10 seconds, enough for both tasks to be executed
	User::After(10000000);	
	Simulate_EndBackupL();

	// Check that the schedules have been executed, and been deleted from the store file
	TInt ret;
    ret = iScheduler.DisableSchedule(iRef2.iHandle);
    TESTL(ret==KErrNotFound);
    
    ret = iScheduler.DisableSchedule(iRef1.iHandle);
    TESTL(ret==KErrNotFound);


	AddTimeSchedulesL();
	// Simulate restore
	Simulate_StartRestoreL();
	// Set time to 4 seconds before 1st task	
	SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am
	// wait 10 seconds, enough for both tasks to be executed
	User::After(10000000);	
	Simulate_EndRestoreL(ETrue);

	// Check that the schedules have been executed, and been deleted from the store file
    ret = iScheduler.DisableSchedule(iRef2.iHandle);
    TESTL(ret==KErrNotFound);
    
    ret = iScheduler.DisableSchedule(iRef1.iHandle);
    TESTL(ret==KErrNotFound);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

