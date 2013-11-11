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

#include "tef_efm_bursuitesimstepbase.h"
#include "tef_efm_bursuitedefs.h"

CTefEfmBURSuiteSimStepBase* CTefEfmBURSuiteSimStepBase::NewLC()
	{
	CTefEfmBURSuiteSimStepBase* newInstance = new (ELeave) CTefEfmBURSuiteSimStepBase();
	CleanupStack::PushL( newInstance );
	newInstance->ConstructL();
	return newInstance;	
	}

void CTefEfmBURSuiteSimStepBase::ConstructL()
	{
	iBackupClient = CBaBackupSessionWrapper::NewL();	
	}

CTefEfmBURSuiteSimStepBase::CTefEfmBURSuiteSimStepBase()
	{
	}

CTefEfmBURSuiteSimStepBase::~CTefEfmBURSuiteSimStepBase()
	{
	delete iBackupClient;	
	}

// BUR simulation functions

/** 
 * Use the babackup API to "start" a backup.  
 * Ultimate effect is to cause the correct case statement inside 
 * ChangeFileLockL to be called.
 */
void CTefEfmBURSuiteSimStepBase::Simulate_StartBackupL()
	{
	iBackupClient->CloseFileL( KEfmBackupFileName, MBackupObserver::EReleaseLockReadOnly );		

	// Set BUR property to trigger 
	TInt ret = RProperty::Set(KTestUid , conn::KUidBackupRestoreKey, conn::EBackupBase|conn::EBURBackupPartial);
	if (ret != KErrNone && ret != KErrNotFound)
		{
		User::Leave(ret);
		}
	
	// allow observers to be notified
	User::After(100000);
	}


/** 
 * Use the babackup API to "end" a backup.  
 * Ultimate effect is to cause the correct case statement inside 
 * ChangeFileLockL to be called. 
 */
void CTefEfmBURSuiteSimStepBase::Simulate_EndBackupL( TBool /* aNormal */ )
	{
	iBackupClient->CloseFileL( KEfmBackupFileName, MBackupObserver::ETakeLock );		

	// allow observers to be notified
	User::After(100000);
	}

// Restore type

/** 
 * Use the babackup API to "start" a restore.  
 * Ultimate effect is to cause the correct case statement inside 
 * ChangeFileLockL to be called.
 */
void CTefEfmBURSuiteSimStepBase::Simulate_StartRestoreL()
	{
	iBackupClient->CloseFileL( KEfmBackupFileName, MBackupObserver::EReleaseLockNoAccess );

	// Set BUR property to trigger 
	TInt ret = RProperty::Set(KTestUid , conn::KUidBackupRestoreKey, conn::EBackupBase|conn::EBURRestorePartial);
	if (ret != KErrNone && ret != KErrNotFound)
		{
		User::Leave(ret);
		}

	// allow observers to be notified
	User::After(100000);
	}


/** 
 * Use the babackup API to "end" a restore.  
 * Ultimate effect is to cause the correct case statement inside 
 * ChangeFileLockL to be called.
 */
void CTefEfmBURSuiteSimStepBase::Simulate_EndRestoreL( TBool /* aNormal */ )
	{
	iBackupClient->CloseFileL( KEfmBackupFileName, MBackupObserver::ETakeLock );		
	
	// allow observers to be notified
	User::After(100000);
	}


// Undefined type


// Edge cases

void CTefEfmBURSuiteSimStepBase::Simulate_StartUndefinedL()
	{
	// Use the babackup API to "start" (neither a backup or restore explicitly).  
	// Ultimate effect is to cause the correct case statement inside 
	// CFeatMgrFeatureRegistry::HandleBackupOperationEventL
	// to be called.
	User::Leave( KErrNotSupported );
	}


void CTefEfmBURSuiteSimStepBase::Simulate_EndUndefinedL( TBool /* aNormal */ )
	{
	// Use the babackup API to "end" (neither a backup or restore explicitly).  
	// Ultimate effect is to cause the correct case statement inside 
	// CFeatMgrFeatureRegistry::HandleBackupOperationEventL
	// to be called.
	User::Leave( KErrNotSupported );
	}

/** 
 * CheckRegFileL
 * Open the babackup file to confirm that the file is present
 * this leaves if the KErrNotFound is returned
 */
void CTefEfmBURSuiteSimStepBase::Simulate_CheckRegFileL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile file;
	TInt err = file.Open(fs, KRegistrationFile, EFileRead);
	
	if( err != KErrNone )
		{
		User::Leave(err);
		}

	file.Close();
	CleanupStack::PopAndDestroy(&fs);
	}
