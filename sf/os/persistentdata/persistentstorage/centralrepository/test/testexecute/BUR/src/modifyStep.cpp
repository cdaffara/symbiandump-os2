// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "modifyStep.h"
#include "Te_centrep_BURSuiteDefs.h"

CmodifyStep::~CmodifyStep()
/**
 * Destructor
 */
	{
	}

CmodifyStep::CmodifyStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KmodifyStep);
	}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-0133
@SYMTestCaseDesc Test Backup & Restore functionality, when repository modified
@SYMTestPriority High
@SYMTestActions  Create repository, backup, modify, restore, check values
@SYMTestExpectedResults The test must not fail
@SYMPREQ 752 Uniform settings access for remote provision and settings sharing
@SYMPREQ 908 System file corruption recoverability
*/

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

TVerdict CmodifyStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EFail);	

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidBURTestRepository));

	TInt r;
	
	// Create new keys

	r = repository->Create(KNewBackupInt, KIntBackupValue);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupReal, KRealBackupValue);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString8, KString8BackupValue);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString16, KString16BackupValue);
	TESTL(r==KErrNone);
	r = repository->Create(KNewInt, KIntValue);
	TESTL(r==KErrNone);
	r = repository->Create(KNewReal, KRealValue);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString8, KString8Value);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString16, KString16Value);
	TESTL(r==KErrNone);

	// Modify current entries

	r = repository->Set(KNewInt1, KInt1_UpdatedValue);
	TESTL(r==KErrNone);
	r = repository->Set(KNewReal1, KReal1_UpdatedValue);
	TESTL(r==KErrNone);
	
	// Delete keys

	r = repository->Delete(KNewBackupString161);
	TESTL(r==KErrNone);
	r = repository->Delete(KNewBackupString162);
	TESTL(r==KErrNone);
	r = repository->Delete(KNewBackupString163);
	TESTL(r==KErrNone);
	r = repository->Delete(KNewBackupString164);
	TESTL(r==KErrNone);

	// Re-instate deleted keys
	TInt value ;
	r=repository->Create(KNoBackupDeleteTestInt, KNoBackupDeleteTestIntModifiedValue);
	TESTL(r==KErrNone) ;
	r=repository->Get(KNoBackupDeleteTestInt, value);
	TESTL(r==KErrNone) ;
	TESTL(value==KNoBackupDeleteTestIntModifiedValue) ;
		
	r=repository->Create(KBackupDeleteTestInt, KBackupDeleteTestIntModifiedValue);
	TESTL(r==KErrNone) ;
	r=repository->Get(KBackupDeleteTestInt, value);
	TESTL(r==KErrNone) ;
	TESTL(value==KBackupDeleteTestIntModifiedValue) ;
	
	// Modify keys to be backed up
	r=repository->Set(KNewBackupInt1, KIntBackupValue1 + 1);
	TESTL(r==KErrNone);
	r=repository->Get(KNewBackupInt1, value);
	TESTL(r==KErrNone) ;
	TESTL(value==KIntBackupValue1 + 1) ;
	
	CleanupStack::PopAndDestroy(repository);


	// Repeat for "Default" repository used to test that a repository which has not
	// been modified (and hence has no repository file on the "C" drive is correctly
	// handled.
	User::LeaveIfNull(repository = CRepository::NewLC(KUidBURTestDefaultRepository));

	
	// Modify current entries
	r = repository->Set(KOrigBackupMetaInt, KOrigBackupMetaIntValue + 1);
	TESTL(r==KErrNone);

	// Delete keys
	r = repository->Delete(KOrigNoMetaInt);
	TESTL(r==KErrNone);

	CleanupStack::PopAndDestroy(repository);	
  
    SetTestStepResult(EPass);
    
	return TestStepResult();
	}
