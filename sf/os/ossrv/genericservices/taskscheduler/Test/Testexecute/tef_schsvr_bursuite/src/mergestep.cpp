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
#include "mergestep.h"
#include "tef_schsvr_bursuitedefs.h"
#include <bautils.h>

_LIT(KSchsvrBackupFileName, "C:\\Private\\10005399\\SchedulesBackup.dat");	
_LIT(KSchsvrBackupFileNameInBackup, "C:\\BackupArchive\\DataOwners\\SchedulesBackup.dat");	

CMergeStep::~CMergeStep()
/**
 * Destructor
 */
	{
	}

CMergeStep::CMergeStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KMergeStep);
	}

void CMergeStep::AddTimeSchedulesL()
	{
	_LIT(KSchTestName1, "Multiple Persistent Schedule Test1");				

	//Add one persistent schedule that will not expire during BUR
    TDateTime datetime1(2000, EJanuary, 2, 10, 0, 0, 0); // 10:00.00 am
    TTsTime dueTime(datetime1, EFalse);	
	AddTimeScheduleL(KSchTestName1, dueTime, iRef1);
	}
	
void CMergeStep::AddTimeSchedulesAfterBackupL()
	{
	_LIT(KSchTestName1, "Multiple Persistent Schedule Test2");				
	_LIT(KSchTestName2, "Multiple Transient Schedule Test3");

	//Add one persistent schedule that will not expire during BUR
    TDateTime datetime1(2000, EJanuary, 3, 10, 0, 0, 0); // 10:00.00 am
    TTsTime dueTime(datetime1, EFalse);	
	AddTimeScheduleL(KSchTestName1, dueTime, iRef2);
	
	//Add one transient schedule that will not expire during BUR
    TDateTime datetime2(2000, EJanuary, 4, 9, 0, 2, 0); // 09:00.00 am
    TTsTime dueTime2(datetime2, EFalse);
	AddTransientTimeScheduleL(KSchTestName2,dueTime2,iRef3);
	}	

TVerdict CMergeStep::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);
        
	CTefSchsvrBURSuiteSimStepBase::doTestStepPreambleL();        
    
	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

    SetTestStepResult(EPass);
    return TestStepResult();
	}

TVerdict CMergeStep::doTestStepL()
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	
	CleanupClosePushL(fsSession);	
	
    SetTestStepResult(EFail);
    
	INFO_PRINTF1(_L("Multiple task execution test during BUR"));

	AddTimeSchedulesL();
	
	// Simulate backup
	Simulate_StartBackupL();
	// Set time
	SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am
	User::LeaveIfError(BaflUtils::CopyFile(fsSession, KSchsvrBackupFileName, KSchsvrBackupFileNameInBackup));	
	Simulate_EndBackupL();

	AddTimeSchedulesAfterBackupL();

	// Check that the schedules are still there
	TInt ret;
	TScheduleType type;
	ret = iScheduler.GetScheduleTypeL(iRef1.iHandle,type);
    TESTL(ret==KErrNone);
    ret = iScheduler.GetScheduleTypeL(iRef2.iHandle,type);
    TESTL(ret==KErrNone);
    ret = iScheduler.GetScheduleTypeL(iRef3.iHandle,type);
    TESTL(ret==KErrNone);    
    
	// Simulate restore
	Simulate_StartRestoreL();
	User::After(10000000);
	User::LeaveIfError(BaflUtils::CopyFile(fsSession, KSchsvrBackupFileNameInBackup, KSchsvrBackupFileName));	
	Simulate_EndRestoreL(ETrue);

	// Now check the schedules following the restore
	// check that the persistent schedule added before backup will be restored
	ret = iScheduler.GetScheduleTypeL(iRef1.iHandle,type);
    TESTL(ret==KErrNone);
    // persistent schedule created after backup will no longer be there
    ret = iScheduler.GetScheduleTypeL(iRef2.iHandle,type);
    TESTL(ret==KErrNotFound);    
    // transient schedule created after backup will still be there
    ret = iScheduler.GetScheduleTypeL(iRef3.iHandle,type);
    TESTL(ret==KErrNone);

	CleanupStack::PopAndDestroy();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

