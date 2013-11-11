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
#include "burstep.h"
#include "tef_schsvr_bursuitedefs.h"

CBURStep::~CBURStep()
/**
 * Destructor
 */
	{
	}

CBURStep::CBURStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KBURStep);
	}

TVerdict CBURStep::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);

	CTefSchsvrBURSuiteSimStepBase::doTestStepPreambleL();
	
	_LIT(KBUR, "Operation");
	
	if (!GetStringFromConfig(ConfigSection(), KBUR, iData))
		{
		ERR_PRINTF1(KMissingInputParam);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CBURStep::doTestStepL()
	{
    SetTestStepResult(EFail);
    
	INFO_PRINTF1(_L("Simulating BUR"));
	
	RFs fsSession;
	TInt err = fsSession.Connect();
	
	CleanupClosePushL(fsSession);
	
	_LIT(KBackupStr, "Backup");
	_LIT(KRestoreStr, "Restore");
	_LIT(KBackupDir, "C:\\BackupArchive\\");
	_LIT(KBackupDir2, "C:\\BackupArchive\\DataOwners\\");	
	_LIT(KSchsvrBackupFileName, "C:\\Private\\10005399\\SchedulesBackup.dat");	
	_LIT(KSchsvrBackupFileNameInBackup, "C:\\BackupArchive\\DataOwners\\SchedulesBackup.dat");		
	
	if (iData == KBackupStr)
		{
		// Simulate backup
		Simulate_StartBackupL();
		
		TInt err;
		err = fsSession.MkDir(KBackupDir);
		if (err != KErrAlreadyExists)
			{
			User::LeaveIfError(err);	
			}
		err = fsSession.MkDir(KBackupDir2);
		if (err != KErrAlreadyExists)
			{
			User::LeaveIfError(err);	
			}
		User::LeaveIfError(BaflUtils::CopyFile(fsSession, KSchsvrBackupFileName, KSchsvrBackupFileNameInBackup));
		
		Simulate_EndBackupL();
		
		SetTestStepResult(EPass);		
		}
	else if (iData == KRestoreStr)
		{
		// Simulate restore
		Simulate_StartRestoreL();

		User::LeaveIfError(BaflUtils::CopyFile(fsSession, KSchsvrBackupFileNameInBackup, KSchsvrBackupFileName));

		Simulate_EndRestoreL(ETrue);
		
		SetTestStepResult(EPass);		
		}
	
	CleanupStack::PopAndDestroy(); 
	
	return TestStepResult();
	}

