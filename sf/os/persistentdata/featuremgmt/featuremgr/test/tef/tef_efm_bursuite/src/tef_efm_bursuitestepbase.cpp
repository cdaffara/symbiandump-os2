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

#include "tef_efm_bursuitestepbase.h"
#include "tef_efm_bursuitedefs.h"
#include "bautils.h"
#include "f32file.h"


CTefEfmBURSuiteStepBase::CTefEfmBURSuiteStepBase()
	{
	}

CTefEfmBURSuiteStepBase::~CTefEfmBURSuiteStepBase()
	{
	}

/*
 * Create the directory structure this occurs when ever a test starts. 
 * If the directory structure is not created then tests can not complete. 
 */
TVerdict CTefEfmBURSuiteStepBase::doTestStepPreambleL()
	{
	SetTestStepResult( CreateDirStructureL() );
	return TestStepResult();
	}

/*
 * Clean up the test Files this occurs when ever a test starts. 
 */
TVerdict CTefEfmBURSuiteStepBase::doTestStepPostambleL()
	{
	if( EFail == TestStepResult() )
		{
		CleanUpTestFilesL();
		}
	else
		{
		SetTestStepResult( CleanUpTestFilesL() );
		}
	return TestStepResult();
	}

// -------------------------
// public


/**
 *  Erase the feature file using the standard functions 
 */
TVerdict CTefEfmBURSuiteStepBase::DeleteDefaultBURFeatureFile( const TDesC & aFeatureFileName )
	{
	TInt delete_err( KErrNone );
	RFs fsSession;
	SetTestStepResult( EFail );

	if (KErrNone == fsSession.Connect() )
		{		
		// Delete the feature file
		delete_err = BaflUtils::DeleteFile( fsSession, aFeatureFileName );
		if( KErrNone != delete_err )
			{
			INFO_PRINTF2( _L("Error while trying to delete %S"), &aFeatureFileName );
			INFO_PRINTF2( _L("BaflUtils::DeleteFile error is %d"), delete_err );
			// not a success so we continue and report this error.
			SetTestStepResult( EFail );
			}
		else
			{
			// delete file worked, set a pass
			SetTestStepResult( EPass );
			} 
		}
	
	return TestStepResult();
	}

// -------------------------
// protected

/**
 * Performs a backup by copying the file to the backup location
 */
TVerdict CTefEfmBURSuiteStepBase::CopyFeatureFileBackupL()
	{		
	TInt err( KErrNone );
	RFs fsSession;
	SetTestStepResult( EFail );
	
	User::LeaveIfError( fsSession.Connect() );
	CleanupClosePushL( fsSession );
	
	err = BaflUtils::CopyFile( fsSession, KEfmBackupFileName, KEfmBackupFileNameInBackup );
	if( KErrNone != err )
		{
		INFO_PRINTF2( _L("Error while trying to backup from %S"), &KEfmBackupFileName );
		INFO_PRINTF2( _L("Error while trying to backup to %S"), &KEfmBackupFileNameInBackup );
		ERR_PRINTF2( _L("BaflUtils::CopyFile error is %d"), err );
		// test was not a success so we continue and report this error.
		SetTestStepResult( EFail );
		}
	else
		{
		// test passed
		SetTestStepResult( EPass );
		}
	
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}



/**
 * Performs a restore by copying the file from the backup to it's normal place.
 */
TVerdict CTefEfmBURSuiteStepBase::CopyFeatureFileRestoreL()
	{
	TInt err( KErrNone );
	RFs fsSession;
	SetTestStepResult( EFail );
	
	User::LeaveIfError( fsSession.Connect() );
	CleanupClosePushL( fsSession );
	err = BaflUtils::CopyFile( fsSession, KEfmBackupFileNameInBackup, KEfmBackupFileName );
	
	if( KErrNone != err )
		{
		INFO_PRINTF2( _L("Error while trying to backup from %S"), &KEfmBackupFileNameInBackup );
		INFO_PRINTF2( _L("Error while trying to backup to %S"), &KEfmBackupFileName );
		ERR_PRINTF2( _L("BaflUtils::CopyFile error is %d"), err );
		// test was not a success so we continue and report this error.
		}
	else
		{
		// test passed
		SetTestStepResult( EPass );
		}
	
	CleanupStack::PopAndDestroy();

	return TestStepResult();
	}


// ------------------------- ------------------------- 
// setup and cleanup functions

/**
 * Creates the direcory structure required for the backup to take place. 
 * Will leave if an error occurs other then KErrNone or KErrAlreadyExists as this 
 * indicates the directory structure has not been created. 
 */
TVerdict CTefEfmBURSuiteStepBase::CreateDirStructureL()
	{
	TInt err( KErrNone );
	RFs fsSession;
	SetTestStepResult( EFail );
	
	User::LeaveIfError( fsSession.Connect() );
	CleanupClosePushL( fsSession );
		
	err = fsSession.MkDir( KBackupDir );
	
	if ((KErrNone != err) && (KErrAlreadyExists != err))
		{
		ERR_PRINTF2( _L("Can not create back up directory %S"), &KBackupDir );
		// catastrophic failure, can't continue any further with test, so leave.
		User::LeaveIfError(err);
		}
	
	err = fsSession.MkDir( KBackupDir2 );
	
	if ((KErrNone != err) && (KErrAlreadyExists != err))
		{
		ERR_PRINTF2( _L("Can not create back up directory %S"), &KBackupDir );
		// catastrophic failure, can't continue any further with test, so leave.
		User::LeaveIfError(err);
		}
	
	SetTestStepResult( EPass );	
	CleanupStack::PopAndDestroy();

	return TestStepResult();
	}



/**
 * Removes the folder and files used for backup. 
 */
TVerdict CTefEfmBURSuiteStepBase::CleanUpTestFilesL()
	{
	RFs fsSession;

	User::LeaveIfError( fsSession.Connect() );
	CleanupClosePushL( fsSession );

	CFileMan* fileMan = CFileMan::NewL(fsSession);
	CleanupStack::PushL(fileMan);
	
	TInt err = fileMan->RmDir(KBackupDir);
	if( KErrNone !=  err )
		{
		ERR_PRINTF2( _L("Unable to do cleanup on %S"), &KBackupDir );
		// catastrophic failure, can't continue any further with test, so leave.
		User::LeaveIfError(err);	
		}
	
	CleanupStack::PopAndDestroy( fileMan );
	CleanupStack::PopAndDestroy( &fsSession ); 

	return EPass;
	}






