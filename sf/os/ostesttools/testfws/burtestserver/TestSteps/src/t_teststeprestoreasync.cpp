// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// T_TestStepRestore.cpp
// 
//

/**
 @file
 @released
*/
#include "t_teststeprestoreasync.h"

namespace bur_ts
	{
	
	CBURTestStepRestoreAsync* CBURTestStepRestoreAsync::NewL(CBURTestServer& aTestServer)
		/**
		Symbian OS Constructor
		
		@internalComponent
		@released
		
		@param aParent - The parent CBURTestServer of this child test step.
		@return Pointer to a newly created CBURTestStepRestoreAsyncAsync object.
		*/
		{
		CBURTestStepRestoreAsync* self = new (ELeave) CBURTestStepRestoreAsync(aTestServer);
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop();
		return self;
		}
	
	CBURTestStepRestoreAsync::CBURTestStepRestoreAsync(CBURTestServer& aTestServer)
		/**
		C++ Constructor
		
		@internalComponent
		@released
		
		@param aParent - The parent CBURTestServer of this child test step.
		*/
		:CBURTestStepBase(aTestServer)
		{}
	
	void CBURTestStepRestoreAsync::ConstructL()
		/**
		Symbian OS 2nd-phase Constructor
		
		@internalComponent
		@released
		*/
		{
		TInt error;
		
		// Initialise the drive list to empty:
		iDriveList.FillZ();
		
		iMode = ERestoreMode;
		
		User::LeaveIfError(iFs.Connect());
		
		// Install an active scheduler for this test step
		iActiveScheduler = new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(iActiveScheduler);
		
		TRAP(error, iBackupClient = CSBEClient::NewL());
		if (error != KErrNone)
			{
			_LIT(KClientConnection, "SBEClientConn");
			User::Panic(KClientConnection,error);
			}
			
		_LIT(KBURTRestore, "BURTestServerRestore");
		User::RenameThread(KBURTRestore);
		//User::SetCritical(User::ESystemCritical);
		
		SetTestStepName(KBURTestRestoreAsync);
		}
	
	CBURTestStepRestoreAsync::~CBURTestStepRestoreAsync()
		/**
		C++ Destructor
		
		@internalComponent
		@released
		*/
		{
		}

	TVerdict CBURTestStepRestoreAsync::doTestStepPreambleL()
		/**
		Override of base class virtual function
		
		@internalComponent
		@released
		@return TVerdict code
		*/
		{
		SetTestStepResult(EPass);
		return TestStepResult();
		}

	TVerdict CBURTestStepRestoreAsync::doTestStepL()
		/**
		Override of base class pure virtual
		
		@internalComponent
		@released
		@return TVerdict code
		*/
		{
		_LIT(KLog1, "Processing user options...");
		Log(LOG_LEVEL2, KLog1);
		ProcessUserOptionsL();
		
		TBURPartType type = EBURRestoreFull;
			
		iStorageManager->ReadDataOwnersFromDiskL(iDataOwners);
		
		CheckValidRegistrationL();	
			
		if (iIsPartial)
			{
			_LIT(KLog2, "Preparing for PARTIAL restore...");
			Log(LOG_LEVEL2, KLog2);
			PrepareForPartialBURL();
			SetSIDListForPartialL();
			type = EBURRestorePartial;
			}
		else
			{
			_LIT(KLog3, "Preparing for FULL restore...");
			Log(LOG_LEVEL2, KLog3);
			}
			
		// Set restore mode:
		_LIT(KLog4, "Setting restore mode...");
		Log(LOG_LEVEL2, KLog4);
		TRAPD(burmode, SetBURModeAsyncL(type, iBackupIncType));
		if (burmode != KErrNone)
			{
			_LIT(KLog5, "Error setting restore Mode ");
			LogWithNum(LOG_LEVEL3, KLog5, burmode);
			}
		
		//SupplyRegistrationDataL();
		
		// system data
		SupplySystemSnapshotDataL();
		RestoreSystemBaseDataL();
		
		RestoreJavaMidletL();
			
		// confirm system files restored 
		CBURActiveObject* active = CBURActiveObject::NewL();
		CleanupStack::PushL(active);
		iBackupClient->AllSystemFilesRestoredL(active->iStatus);
		active->StartL();
		User::LeaveIfError(active->Error());
		CleanupStack::PopAndDestroy(active);
		
		/** Restore data according to user preferences */
		if (iBackupIncType == EBackupIncrement)
			{
			IncrementalRestoreL();
			}
		else
			{
			BaseRestoreL();
			}
			
		Log(LOG_LEVEL2, _L("Setting backup mode to Normal ..."));
		TRAPD(err2,SetBURModeAsyncL(EBURNormal, ENoBackup));
		if (err2 != KErrNone)
			{
			_LIT(KBURError, "Error setting Backup mode ");
			LogWithNum(LOG_LEVEL3, KBURError, err2);
			}
		
		return TestStepResult();
		}
	
	void CBURTestStepRestoreAsync::BaseRestoreL()
		/**
		@internalComponent
		@released
		*/
		{
		RestoreJavaMidletDataL();
		
		// passive
		SupplyPassiveSnapshotDataL();
		RestorePassiveBaseDataL();
		
		//active
		SupplyActiveSnapshotDataL();
		RestoreActiveBaseDataL();
		}
	
	void CBURTestStepRestoreAsync::IncrementalRestoreL()
		/**
		@internalComponent
		@released
		*/
		{
		RestoreJavaMidletDataL();
		
		// passive
		SupplyPassiveSnapshotDataL();
		RestorePassiveBaseDataL();
		RestorePassiveIncDataL();
		
		
		//active
		SupplyActiveSnapshotDataL();
		RestoreActiveBaseDataL();
		RestoreActiveIncDataL();
		}
		
	
	void CBURTestStepRestoreAsync::SupplyRegistrationDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KRes, "Supplying registration data...");
		Log(LOG_LEVEL2, KRes);
		DoSupplyL(ERegistrationData);
		}
	
	void CBURTestStepRestoreAsync::SupplyPassiveSnapshotDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KSupp, "Supplying passive snapshot data...");
		Log(LOG_LEVEL2, KSupp);
		DoSupplyL(EPassiveSnapshotData);	
		}
	
	void CBURTestStepRestoreAsync::RestorePassiveBaseDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KResBase, "Restoring passive base data...");
		Log(LOG_LEVEL2, KResBase);
		DoSupplyL(EPassiveBaseData);
		}
	
	void CBURTestStepRestoreAsync::RestorePassiveIncDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KPassInc, "Restoring passive incremental data...");
		Log(LOG_LEVEL2, KPassInc);
		DoSupplyL(EPassiveIncrementalData);
		}
	
	void CBURTestStepRestoreAsync::SupplyActiveSnapshotDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KSuppSnap, "Supplying active snapshot data...");
		Log(LOG_LEVEL2, KSuppSnap);
		DoSupplyL(EActiveSnapshotData);
		}
		
	void CBURTestStepRestoreAsync::RestoreActiveBaseDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KActBase, "Restoring active base data...");
		Log(LOG_LEVEL2, KActBase);
		DoSupplyL(EActiveBaseData);
		}
		
	void CBURTestStepRestoreAsync::RestoreActiveIncDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KActIncD, "Restoring active incremental data...");
		Log(LOG_LEVEL2, KActIncD);
		DoSupplyL(EActiveIncrementalData);
		}
		
	void CBURTestStepRestoreAsync::RestoreSystemBaseDataL()
		/**
		@internalComponent
		@released
		*/
		{
		GeneratePIDTransferTypesL(iTransferTypes, ESystemData);
		// go through each of them and check if we have snapshot to supply
		if (iTransferTypes.Count())
			{
			_LIT(KSysBase, "Restoring system base data...");
			Log(LOG_LEVEL2, KSysBase);
			SupplyDataL(iTransferTypes);
			}
		}
		
	void CBURTestStepRestoreAsync::SupplySystemSnapshotDataL()
		/**
		@internalComponent
		@released
		*/
		{
		GeneratePIDTransferTypesL(iTransferTypes, ESystemSnapshotData);
		// go through each of them and check if we have snapshot to supply
		if (iTransferTypes.Count())
			{
			_LIT(KSysSnap, "Supply System Snapshot Data ...");
			Log(LOG_LEVEL2, KSysSnap);
			SupplyDataL(iTransferTypes);
			}
		}
		
	void CBURTestStepRestoreAsync::RestoreJavaMidletL()
		/**
		@internalComponent
		@released
		*/
		{
		GenerateJavaTransferTypesL(iTransferTypes, EJavaMIDlet);
		if (iTransferTypes.Count())
			{
			_LIT(KJMidlet, "Restoring Java Midlets ...");
			Log(LOG_LEVEL2, KJMidlet);
			SupplyDataL(iTransferTypes);
			}
		}
	
	void CBURTestStepRestoreAsync::RestoreJavaMidletDataL()
		/**
		@internalComponent
		@released
		*/
		{
		GenerateJavaTransferTypesL(iTransferTypes, EJavaMIDletData);
		if (iTransferTypes.Count())
			{
			_LIT(KJMidletData, "Restoring Java Midlets Data ...");
			Log(LOG_LEVEL2, KJMidletData);
			SupplyDataL(iTransferTypes);
			}
		}

	TVerdict CBURTestStepRestoreAsync::doTestStepPostambleL()
		/**
		@return - TVerdict code
		Override of base class pure virtual
		*/
		{
		_LIT(KLog, "Number of failures:");
		LogWithNum(LOG_LEVEL3, KLog, iFailures);
		if (iFailures)
			{
			SetTestStepResult(TVerdict(EFail));
			}
		return TestStepResult();
		}
		

	} 	// end namespace
