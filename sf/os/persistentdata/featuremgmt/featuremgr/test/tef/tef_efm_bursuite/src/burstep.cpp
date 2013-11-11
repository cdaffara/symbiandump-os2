// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/

#include "burstep.h"


CBURStep::~CBURStep()
	{
	}

CBURStep::CBURStep()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KBURStep);
	}

/**
 * Test to see that the registration file is present. If it is then the function does a 
 * backup/restore depending on which option was used when running this teststep.
 */
TVerdict CBURStep::doTestStepL()
	{
	CTefEfmBURSuiteSimStepBase* simulate = CTefEfmBURSuiteSimStepBase::NewLC();
	SetTestStepResult( EFail );    // assume test setp fails
	simulate->Simulate_CheckRegFileL();
	RFs fsSession;
	User::LeaveIfError( fsSession.Connect() );
	CleanupClosePushL( fsSession );
	
	// Simulate a backup
	if( iData == KBackupStr )
		{
		INFO_PRINTF1(_L("Simulating Backup of BUR"));
		simulate->Simulate_StartBackupL();
		SetTestStepResult( CTefEfmBURSuiteStepBase::CopyFeatureFileBackupL() );
		simulate->Simulate_EndBackupL();
		}

	// Simulate a restore
	else if( iData == KRestoreStr )
		{
		INFO_PRINTF1(_L("Simulating Restore of BUR"));
		simulate->Simulate_StartRestoreL();
		SetTestStepResult( CTefEfmBURSuiteStepBase::CopyFeatureFileRestoreL() );
		simulate->Simulate_EndRestoreL( ETrue );
		}
	
	CleanupStack::PopAndDestroy(2); 
	
	return TestStepResult();
	}


/**
 * Sets the test to fail and gets the operation from the ini file based on the string 
 * used in the script file.
 * If it's a backup, then create the dir structure required.
 * This assumes that a "restore" already has a directory structure, and 
 * is called directly after a backup has taken place.
 */
TVerdict CBURStep::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);
    
	_LIT(KBUR, "Operation");
	
	if (!GetStringFromConfig(ConfigSection(), KBUR, iData))
		{
		ERR_PRINTF1(KMissingInputParam);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	if( iData == KBackupStr )
		{
		SetTestStepResult( CreateDirStructureL() );
		}
	else if( iData == KRestoreStr )
		{
		// Else don't create anything and just set a pass
	    SetTestStepResult(EPass);
		}
	// else fail
	
	return TestStepResult();
	}


/** 
 * If it's a restore, then cleanup the dir structure.
 * This assumes that a "backup" doesn't want to cleanup the directory 
 * structure, since a "restore" is called immediately after a "backup" has 
 * taken place.
 */
TVerdict CBURStep::doTestStepPostambleL()
	{

	SetTestStepResult(EFail);

	if( iData == KRestoreStr )
		{
		SetTestStepResult( CleanUpTestFilesL() );
		}
	else if( iData == KBackupStr )
		{
		// Else don't cleanup anything and just set a pass
	    SetTestStepResult(EPass);
		}
	// else fail
	
	return TestStepResult();
	}
