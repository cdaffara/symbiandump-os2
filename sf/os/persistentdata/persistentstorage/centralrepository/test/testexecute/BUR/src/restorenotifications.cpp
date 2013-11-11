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
 @SYMTestSuiteName SYSLIB-CENTRALREPOSITORY-TE_CENTREP_BURSUITE
 @SYMScriptTestEnvironment this script requires Techview ROM for BURTestserver dependency. BURTestserver must be built for this test to run.
*/
#include "restorenotifications.h"
#include "Te_centrep_BURSuiteDefs.h"

TRequestStatus rstChangeStatus1, rstChangeStatus2, rstDeleteStatus;
CRepository* repository1 = NULL, *repository2 = NULL;

CRestoreNotificationsInitialiseStep::~CRestoreNotificationsInitialiseStep()
/**
 * Destructor
 */
	{
	}

CRestoreNotificationsInitialiseStep::CRestoreNotificationsInitialiseStep()

/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KRestoreNotificationsInitialiseStep);
	}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-3415
@SYMTestCaseDesc Restoration Notification Test
@SYMTestPriority High
@SYMTestActions  This test checks that the restoration sends notifications about 
the changed repositories and keys. The test process is:
1. Create repositories.
2. Backup repositories.
3. Modify repositories.
4. Request notifications
5. Restore repositories.
6. Wait for the notifications and check the result of the restore.
Step 4 and step 6 must be run in the same thread for the receiving of notifications.
@SYMTestExpectedResults The notification requesting test step should be notified about 
the changes by the restoration and get the new values of changed keys successfully.
@SYMDEF INC101923 cenrep notifier does not fire when restoring from backup
*/

TVerdict CRestoreNotificationsInitialiseStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EFail);	

	repository1 = CRepository::NewL(KUidBURTestRepository);
	repository2 = CRepository::NewL(KUidBURTestDefaultRepository);

	SetTestStepResult(EPass);
	
	return TestStepResult();
	}

	
CRestoreNotificationsRequestStep::~CRestoreNotificationsRequestStep()
/**
 * Destructor
 */
	{
	}

CRestoreNotificationsRequestStep::CRestoreNotificationsRequestStep()

/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KRestoreNotificationsRequestStep);
	}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-3415
@SYMTestCaseDesc Restoration Notification Test
@SYMTestPriority High
@SYMTestActions  This test checks that the restoration sends notifications about 
the changed repositories and keys. The test process is:
1. Create repositories.
2. Backup repositories.
3. Modify repositories.
4. Request notifications
5. Restore repositories.
6. Wait for the notifications and check the result of the restore.
Step 4 and step 6 must be run in the same thread for the receiving of notifications.
@SYMTestExpectedResults The notification requesting test step should be notified about 
the changes by the restoration and get the new values of changed keys successfully.
@SYMDEF INC101923 cenrep notifier does not fire when restoring from backup
*/

TVerdict CRestoreNotificationsRequestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EFail);	

	TInt r = KErrNone;
	
	TESTL(repository1 != NULL);
	TESTL(repository2 != NULL);

	// request the notification for the key which was changed after the backup 
	// and should be restored.
	r = repository1->NotifyRequest(KNewBackupInt1, rstChangeStatus1);  
	TESTL(r == KErrNone);		

	// request the notification for another key which was changed after the backup 
	// and should be restored.
	r = repository2->NotifyRequest(KOrigBackupMetaInt, rstChangeStatus2); 
	TESTL(r == KErrNone);		

	// request the notification for the key which was marked deleted but created after 
	// the backup. Restoration should delete it.
	r = repository1->NotifyRequest(KBackupDeleteTestInt, rstDeleteStatus); 
	TESTL(r == KErrNone);		
		

	SetTestStepResult(EPass);
	
	return TestStepResult();
	}


/////////////////////////////////
CRestoreNotificationsWaitStep::~CRestoreNotificationsWaitStep()
/**
 * Destructor
 */
	{
	}

CRestoreNotificationsWaitStep::CRestoreNotificationsWaitStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KRestoreNotificationsWaitStep);
	}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-3415
@SYMTestCaseDesc Restoration Notification Test
@SYMTestPriority High
@SYMTestActions  This test checks that the restoration sends notifications about 
the changed repositories and keys. The test process is:
1. Create repositories.
2. Backup repositories.
3. Modify repositories.
4. Request notifications
5. Restore repositories.
6. Wait for the notifications and check the result of the restore.
Step 4 and step 6 must be run in the same thread for the receiving of notifications.
@SYMTestExpectedResults The notification requesting test step should be notified about 
the changes by the restoration and get the new values of changed keys successfully.
@SYMDEF INC101923 cenrep notifier does not fire when restoring from backup
*/

TVerdict CRestoreNotificationsWaitStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EFail);	
    
   	TESTL(repository1 != NULL);
	TESTL(repository2 != NULL);

   	// check for key deleted.
	TInt r = KErrNone;
	TInt intval;

	intval = -1;
	r = repository1->Get(KBackupDeleteTestInt, intval);
	TESTL(r == KErrNotFound);

	User::WaitForRequest(rstDeleteStatus);
	TESTL(rstDeleteStatus==KBackupDeleteTestInt);
	User::WaitForRequest(rstChangeStatus1);
	TESTL(rstChangeStatus1==KNewBackupInt1);

	User::WaitForRequest(rstChangeStatus2);
	TESTL(rstChangeStatus2==KOrigBackupMetaInt);

	// check for key deleted.
	intval = -1;
	r = repository1->Get(KBackupDeleteTestInt, intval);
	TESTL(r == KErrNotFound);
	
	// check for key restored.
	intval = -1;
	r = repository1->Get(KNewBackupInt1, intval);
	TESTL(r == KErrNone);	
	TESTL(intval == KIntBackupValue1);


	// check for key restored.
	intval = -1;
	r = repository2->Get(KOrigBackupMetaInt, intval);
	TESTL(r == KErrNone);	
	TESTL(intval == KOrigBackupMetaIntValue);
	
	SetTestStepResult(EPass);
	
	return TestStepResult();
	}


CRestoreNotificationsReleaseStep::~CRestoreNotificationsReleaseStep()
/**
 * Destructor
 */
	{
	}

CRestoreNotificationsReleaseStep::CRestoreNotificationsReleaseStep()

/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KRestoreNotificationsReleaseStep);
	}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-3415
@SYMTestCaseDesc Restoration Notification Test
@SYMTestPriority High
@SYMTestActions  This test checks that the restoration sends notifications about 
the changed repositories and keys. The test process is:
1. Create repositories.
2. Backup repositories.
3. Modify repositories.
4. Request notifications
5. Restore repositories.
6. Wait for the notifications and check the result of the restore.
Step 4 and step 6 must be run in the same thread for the receiving of notifications.
@SYMTestExpectedResults The notification requesting test step should be notified about 
the changes by the restoration and get the new values of changed keys successfully.
@SYMDEF INC101923 cenrep notifier does not fire when restoring from backup
*/

TVerdict CRestoreNotificationsReleaseStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EFail);	

	if(repository1 != NULL)
		{
		delete repository1;
		}
	if(repository2 != NULL)
		{
		delete repository2;
		}

	SetTestStepResult(EPass);
	
	return TestStepResult();
	}
