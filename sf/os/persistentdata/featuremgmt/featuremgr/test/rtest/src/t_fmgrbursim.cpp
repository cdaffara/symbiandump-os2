// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_fmgrbursim.h"
#include "t_fmgrburdefs.h"

CFeatMgrBURSim* CFeatMgrBURSim::NewLC()
	{
	CFeatMgrBURSim* newInstance = new (ELeave) CFeatMgrBURSim();
	CleanupStack::PushL( newInstance );
	newInstance->ConstructL();
	return newInstance;	
	}

void CFeatMgrBURSim::ConstructL()
	{
	iBackupClient = CBaBackupSessionWrapper::NewL();
	}

CFeatMgrBURSim::CFeatMgrBURSim()
	{
	}

CFeatMgrBURSim::~CFeatMgrBURSim()
	{
	delete iBackupClient;	
	}

// BUR simulation functions

/** 
 * Use the babackup API to "start" a backup.  
 * Ultimate effect is to cause the correct case statement inside 
 * ChangeFileLockL to be called.
 */
void CFeatMgrBURSim::Simulate_StartBackupL()
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
void CFeatMgrBURSim::Simulate_EndBackupL( TBool /* aNormal */ )
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
void CFeatMgrBURSim::Simulate_StartRestoreL()
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
void CFeatMgrBURSim::Simulate_EndRestoreL( TBool /* aNormal */ )
	{
	iBackupClient->CloseFileL( KEfmBackupFileName, MBackupObserver::ETakeLock );		
	
	// allow observers to be notified
	User::After(100000);
	}


// Undefined type


// Edge cases

void CFeatMgrBURSim::Simulate_StartUndefinedL()
	{
	// Use the babackup API to "start" (neither a backup or restore explicitly).  
	// Ultimate effect is to cause the correct case statement inside 
	// CFeatMgrFeatureRegistry::HandleBackupOperationEventL
	// to be called.
	User::Leave( KErrNotSupported );
	}


void CFeatMgrBURSim::Simulate_EndUndefinedL( TBool /* aNormal */ )
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
void CFeatMgrBURSim::Simulate_CheckRegFileL()
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
