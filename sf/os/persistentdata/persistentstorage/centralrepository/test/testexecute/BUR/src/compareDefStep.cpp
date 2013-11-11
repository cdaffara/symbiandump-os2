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
#include "compareDefStep.h"
#include "Te_centrep_BURSuiteDefs.h"
#include "Te_centrep_BURSuiteStepBase.h"

CcompareDefStep::~CcompareDefStep()
/**
 * Destructor
 */
	{
	}

CcompareDefStep::CcompareDefStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KcompareDefStep);
	}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-1387
@SYMTestCaseDesc CenRep restore does not work if only some settings from the rep. are backed up
@SYMTestPriority Crticial
@SYMTestActions Test that partial restore merges with existing data in private data cage or ROM
@SYMTestExpectedResults The test must not fail
@SYMDEF INC067079 
*/

TVerdict CcompareDefStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EFail);	

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidBURDefTestRepository));

	// Check that all entries in the ROM repository are restored

	RArray<TUint32> foundIds;

	TInt r = KErrNone;
	TInt leaveCode = KErrNone;
	TRAP(leaveCode, r = repository->FindL(0, 0, foundIds));
	if (leaveCode != KErrNone)
		r = leaveCode ;
	
	TESTL(r==KErrNone);

	INFO_PRINTF2(_L("Number of Keys: %d\n"), foundIds.Count());
	TESTL(foundIds.Count()==KNumDefBackupSettings);
	
	foundIds.Reset();

	// Check that the values are correctly merged after restore 

	TInt i = 0;

	r = repository->Get(KModifyBackupInt1, i);
	TESTL(r==KErrNone);
	TESTL(i==KIntBackupValue1);

	r = repository->Get(KDefaultBackupInt1, i);
	TESTL(r==KErrNone);
	TESTL(i==KIntDefaultValue1);
	
	TBuf<20> str;
	
	r = repository->Get(KModifyBackupString1, str);
	TESTL(r==KErrNone);
	TESTL(str==KString16BackupValue1);

	r = repository->Get(KDefaultBackupString1, str);
	TESTL(r==KErrNone);
	TESTL(str==KString16DefaultValue1);

	CleanupStack::PopAndDestroy(repository);

    SetTestStepResult(EPass);	

	return TestStepResult();
	}
