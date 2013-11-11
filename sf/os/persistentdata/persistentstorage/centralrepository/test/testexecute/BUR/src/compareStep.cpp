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
#include "compareStep.h"
#include "Te_centrep_BURSuiteDefs.h"
#include "Te_centrep_BURSuiteStepBase.h"

CcompareStep::~CcompareStep()
/**
 * Destructor
 */
	{
	}

CcompareStep::CcompareStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KcompareStep);
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

TVerdict CcompareStep::doTestStepL()
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

	TInt numOfSettings;
	// get iteration number from ini file
	TInt bRet = GetIntFromConfig(ConfigSection(), KIniNumOfSettings, numOfSettings);
	
	TESTL(bRet==1);

	// Check that there are the correct number of entries in the repository after restore

	RArray<TUint32> foundIds;

	TInt r = KErrNone;
	TInt leaveCode = KErrNone;
	TRAP(leaveCode, r = repository->FindL(0, 0, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	
	TESTL(r==KErrNone);

	INFO_PRINTF2(_L("Number of Keys: %d\n"), foundIds.Count());
 	TESTL(foundIds.Count()==numOfSettings);

	
	foundIds.Reset();

	// Check that the values are backed up correctly

	TInt i = 0;

	// Check original values

	r = repository->Get(KOrigNoMetaInt, i);
	TESTL(r==KErrNone);
	TESTL(i==KOrigNoMetaIntValue);	

	r = repository->Get(KOrigZeroMetaInt, i);
	// Not backed up but merged from ROM file
	TESTL(r==KErrNone);

	r = repository->Get(KOrigBackupMetaInt, i);
	TESTL(r==KErrNone);
	TESTL(i==KOrigBackupMetaIntValue);	

	// Check newly created values
	
	r = repository->Get(KNewBackupInt1, i);
	TESTL(r==KErrNone);
	TESTL(i==KIntBackupValue1);
	r = repository->Get(KNewBackupInt2, i);
	TESTL(r==KErrNone);
	TESTL(i==KIntBackupValue2);
	r = repository->Get(KNewBackupInt3, i);
	TESTL(r==KErrNone);
	TESTL(i==KIntBackupValue3);
	r = repository->Get(KNewBackupInt4, i);
	TESTL(r==KErrNone);
	TESTL(i==KIntBackupValue4);

	TReal y = 0;

	r = repository->Get(KNewBackupReal1, y);
	TESTL(r==KErrNone);
	TESTL(y==KRealBackupValue1);
	r = repository->Get(KNewBackupReal2, y);
	TESTL(r==KErrNone);
	TESTL(y==KRealBackupValue2);
	r = repository->Get(KNewBackupReal3, y);
	TESTL(r==KErrNone);
	TESTL(y==KRealBackupValue3);
	r = repository->Get(KNewBackupReal4, y);
	TESTL(r==KErrNone);
	TESTL(y==KRealBackupValue4);
	
	TBuf8<20> str8;
	
	r = repository->Get(KNewBackupString81, str8);
	TESTL(r==KErrNone);
	TESTL(str8==KString8BackupValue1);
	r = repository->Get(KNewBackupString82, str8);
	TESTL(r==KErrNone);
	TESTL(str8==KString8BackupValue2);
	r = repository->Get(KNewBackupString83, str8);
	TESTL(r==KErrNone);
	TESTL(str8==KString8BackupValue3);
	r = repository->Get(KNewBackupString84, str8);
	TESTL(r==KErrNone);
	TESTL(str8==KString8BackupValue4);

	TBuf<20> str;
	
	r = repository->Get(KNewBackupString161, str);
	TESTL(r==KErrNone);
	TESTL(str==KString16BackupValue1);
	r = repository->Get(KNewBackupString162, str);
	TESTL(r==KErrNone);
	TESTL(str==KString16BackupValue2);
	r = repository->Get(KNewBackupString163, str);
	TESTL(r==KErrNone);
	TESTL(str==KString16BackupValue3);
	r = repository->Get(KNewBackupString164, str);
	TESTL(r==KErrNone);
	TESTL(str==KString16BackupValue4);

	// Check that settings deleted BEFORE backup are handled properly
	

	// Setting deleted and IS marked for backup - should NOT be re-instated
	r=repository->Get(KBackupDeleteTestInt, i);
	TESTL(r==KErrNotFound) ;	

	// Setting deleted and NOT marked for backup - should NOT have it's original
	// value restored - will either be not present or have a modified value!
	TInt expectedResult ;
	bRet = GetIntFromConfig(ConfigSection(), KIniExpectedGetResult, expectedResult);
	TInt expectedValue ;
	bRet = GetIntFromConfig(ConfigSection(), KIniExpectedGetValue, expectedValue);
	r=repository->Get(KNoBackupDeleteTestInt, i);
	TESTL(r==expectedResult) ;
	if (r == KErrNone) 
		{
		TESTL(i == expectedValue) ;
		}
	
	CleanupStack::PopAndDestroy(repository);

	// Repeat for "Default" repository used to test that a repository which has not
	// been modified (and hence has no repository file on the "C" drive is correctly
	// handled - This should always contain just the original ROM default values.
	
	User::LeaveIfNull(repository = CRepository::NewLC(KUidBURTestDefaultRepository));

	numOfSettings = KNumDefaultBackupSettings;
	

	// Check that there are the correct number of entries in the repository after restore
	foundIds.Reset();

	r = KErrNone;
	leaveCode = KErrNone;
	TRAP(leaveCode, r = repository->FindL(0, 0, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	
	TESTL(r==KErrNone);

	INFO_PRINTF2(_L("Number of Keys: %d\n"), foundIds.Count());
 	TESTL(foundIds.Count()==numOfSettings);

	
	foundIds.Reset();

	// Check that the values are backed up correctly

	i = 0;

	// Check original values

	r = repository->Get(KOrigNoMetaInt, i);
	TESTL(r==KErrNone);
	TESTL(i==KOrigNoMetaIntValue);	

	r = repository->Get(KOrigZeroMetaInt, i);
	// Not backed up but merged from ROM file
	TESTL(r==KErrNone);

	r = repository->Get(KOrigBackupMetaInt, i);
	TESTL(r==KErrNone);
	TESTL(i==KOrigBackupMetaIntValue);	

	CleanupStack::PopAndDestroy(repository);
    SetTestStepResult(EPass);	

	return TestStepResult();
	}
