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
 @SYMTestSuiteName SYSLIB-CENTRALREPOSITORY-TE_CENTREP_BURSUITE
 @SYMScriptTestEnvironment this script requires Techview ROM for BURTestserver dependency. BURTestserver must be built for this test to run.
*/
#include "initailiseStep.h"
#include "Te_centrep_BURSuiteDefs.h"

CinitailiseStep::~CinitailiseStep()
/**
 * Destructor
 */
	{
	}

CinitailiseStep::CinitailiseStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KinitailiseStep);
	}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-0131
@SYMTestCaseDesc Test Backup & Restore functionality, when repository unchanged
@SYMTestPriority High
@SYMTestActions  Create repository, backup, restore, check values
@SYMTestExpectedResults The test must not fail
@SYMPREQ 752 Uniform settings access for remote provision and settings sharing
@SYMPREQ 908 System file corruption recoverability
*/

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-0132
@SYMTestCaseDesc Test Backup & Restore functionality, when repository corrupt
@SYMTestPriority High
@SYMTestActions  Create repository, backup, corrupt, restore, check values
@SYMTestExpectedResults The test must not fail
@SYMPREQ 752 Uniform settings access for remote provision and settings sharing
@SYMPREQ 908 System file corruption recoverability
*/

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
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-0134
@SYMTestCaseDesc Test Backup & Restore functionality, when repository deleted
@SYMTestPriority High
@SYMTestActions  Create repository, backup, delete, restore, check values
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

TVerdict CinitailiseStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EFail);	

	// Existance of caching functionality makes all test steps fail because file delete operations
	// outside of CenRep's knowledge assumes non-caching behaviour.
	// We wait until the cache is empty here to correct the behaviour.
	User::After(KDefaultEvictionTimeout+950000);

 	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidBURTestRepository));

	TInt r;

	// Create values to be backed up

	r = repository->Create(KNewBackupInt1, KIntBackupValue1);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupInt2, KIntBackupValue2);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupInt3, KIntBackupValue3);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupInt4, KIntBackupValue4);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupReal1, KRealBackupValue1);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupReal2, KRealBackupValue2);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupReal3, KRealBackupValue3);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupReal4, KRealBackupValue4);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString81, KString8BackupValue1);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString82, KString8BackupValue2);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString83, KString8BackupValue3);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString84, KString8BackupValue4);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString161, KString16BackupValue1);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString162, KString16BackupValue2);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString163, KString16BackupValue3);
	TESTL(r==KErrNone);
	r = repository->Create(KNewBackupString164, KString16BackupValue4);
	TESTL(r==KErrNone);

	// Create values not to be backed up

	r = repository->Create(KNewInt1, KIntValue1);
	TESTL(r==KErrNone);
	r = repository->Create(KNewInt2, KIntValue2);
	TESTL(r==KErrNone);
	r = repository->Create(KNewInt3, KIntValue3);
	TESTL(r==KErrNone);
	r = repository->Create(KNewInt4, KIntValue4);
	TESTL(r==KErrNone);
	r = repository->Create(KNewReal1, KRealValue1);
	TESTL(r==KErrNone);
	r = repository->Create(KNewReal2, KRealValue2);
	TESTL(r==KErrNone);
	r = repository->Create(KNewReal3, KRealValue3);
	TESTL(r==KErrNone);
	r = repository->Create(KNewReal4, KRealValue4);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString81, KString8Value1);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString82, KString8Value2);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString83, KString8Value3);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString84, KString8Value4);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString161, KString16Value1);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString162, KString16Value2);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString163, KString16Value3);
	TESTL(r==KErrNone);
	r = repository->Create(KNewString164, KString16Value4);
	TESTL(r==KErrNone);

	// Delete key not marked for backup
	r = repository->Delete(KNoBackupDeleteTestInt);
	TESTL(r==KErrNone);

	// Delete key marked for backup	
	r = repository->Delete(KBackupDeleteTestInt) ;
	TESTL(r==KErrNone);
	
	// Check that keys deleted in initialise step have really gone.
	TInt value ;
	r=repository->Get(KNoBackupDeleteTestInt, value);
	TESTL(r==KErrNotFound) ;
	
	r=repository->Get(KBackupDeleteTestInt, value);
	TESTL(r==KErrNotFound) ;
	
	CleanupStack::PopAndDestroy(repository);

	SetTestStepResult(EPass);
	
	return TestStepResult();
	}

