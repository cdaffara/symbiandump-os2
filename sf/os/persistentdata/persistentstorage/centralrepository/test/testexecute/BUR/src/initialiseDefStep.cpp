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
#include "initialiseDefStep.h"
#include "Te_centrep_BURSuiteDefs.h"

CinitialiseDefStep::~CinitialiseDefStep()
/**
 * Destructor
 */
	{
	}

CinitialiseDefStep::CinitialiseDefStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KinitialiseDefStep);
	}

/*
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-1387
@SYMTestCaseDesc CenRep restore does not work if only some settings from the rep. are backed up
@SYMTestPriority Crticial
@SYMTestActions Test that partial restore merges with existing data in private data cage or ROM
@SYMTestExpectedResults The test must not fail
@SYMDEF INC067079 
@SYMTestSuiteName te_centrep_BURSuite BURTestServer
*/

TVerdict CinitialiseDefStep::doTestStepL()
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

	TInt r;

	// Modify values before back up, to be able to test them against restored
	// values later

	r = repository->Set(KModifyBackupInt1, KIntBackupValue1);
	TESTL(r==KErrNone);

	r = repository->Set(KModifyBackupString1, KString16BackupValue1);
	TESTL(r==KErrNone);
	
	CleanupStack::PopAndDestroy(repository);

	SetTestStepResult(EPass);
	
	return TestStepResult();
	}

