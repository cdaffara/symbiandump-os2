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
// T_TestStepBackup.cpp
// 
//

/**
 @file
 @released
*/

#include "t_teststepbackupasync.h"


namespace bur_ts
	{
			
	CBURTestStepBackupAsync* CBURTestStepBackupAsync::NewL(CBURTestServer& aTestServer)
		/**
		Symbian OS Constructor
		
		@internalComponent
		@released
		
		@param aParent - The parent CBURTestServer of this child test step.
		@return Pointer to a newly created CBURTestStepBackup object.
		*/
		{
		CBURTestStepBackupAsync* self = new (ELeave) CBURTestStepBackupAsync(aTestServer);
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop();
		return self;
		}

	CBURTestStepBackupAsync::CBURTestStepBackupAsync(CBURTestServer& aTestServer)
		/**
		C++ Constructor
		
		@internalComponent
		@released
		
		@param aParent - The parent CBURTestServer of this child test step.
		*/
		: CBURTestStepBase(aTestServer), iDataTransferHandler(NULL)
		{}
	
	void CBURTestStepBackupAsync::ConstructL()
		/**
		Symbian OS 2nd-phase Constructor
		
		@internalComponent
		@released
		*/
		{
		TInt error;
		
		// Initialise the drive list to empty
		iDriveList.FillZ();
		
		User::LeaveIfError(iFs.Connect());
		
		// Install an active scheduler for this test step
		iActiveScheduler = new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(iActiveScheduler);
		
		TRAP(error, iBackupClient = CSBEClient::NewL());
		if (error != KErrNone)
			{
			_LIT(KClientConnection, "SBEClientConnection");
			User::Panic(KClientConnection,error);
			}
			
		_LIT(KBURTBackup, "BURTestServerBackup");
		User::RenameThread(KBURTBackup);
		//User::SetCritical(User::ESystemCritical);
		
		SetTestStepName(KBURTestBackupAsync);
		}
	
	CBURTestStepBackupAsync::~CBURTestStepBackupAsync()
		/**
		C++ Destructor
		
		@internalComponent
		@released
		*/
		{
		delete iDataTransferHandler;
		}

	TVerdict CBURTestStepBackupAsync::doTestStepPreambleL()
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

	TVerdict CBURTestStepBackupAsync::doTestStepL()
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
		
		// Get a list of data owners:
		PopulateListOfDataOwnersAsyncL();
		
		
		TBURPartType type = EBURBackupFull;
		
		if (iIsPartial)
			{
			_LIT(KLog2, "Preparing for PARTIAL backup...");
			Log(LOG_LEVEL2, KLog2);
			type = EBURBackupPartial;
			PrepareForPartialBURL();
			SetSIDListForPartialL();
			}
		else
			{
			_LIT(KLog3, "Preparing for FULL backup...");
			Log(LOG_LEVEL2, KLog3);
			}
		
		CheckValidRegistrationL();
		
		_LIT(KLog4, "Saving Data Owners...");
		Log(LOG_LEVEL2, KLog4);
		SaveDataOwners();
		
		Log(LOG_LEVEL2, _L("Setting backup mode..."));
		TRAPD(err,SetBURModeAsyncL(type, iBackupIncType));
		if (err != KErrNone)
			{
			_LIT(KBURError, "Error setting Backup mode ");
			Log(LOG_LEVEL3, KBURError);
			}
		
		iDataTransferHandler = CAsyncBackupTransferHandler::NewL(this);
		Log(LOG_LEVEL2, _L("Starting backup..."));
		
		//BackupRegistrationDataL();
		
		BackupPublicDataL();
		
		BackupJavaMidletsL();
		BackupJavaMidletsDataL();
		
		// Backup data according to user preferences
		if (iBackupIncType == EBackupIncrement)
			{
			IncrementalBackupL();
			}
		else
			{
			BaseBackupL();
			}
			
		Log(LOG_LEVEL2, _L("Setting backup mode to Normal ..."));
		TRAPD(err2,SetBURModeL(EBURNormal, ENoBackup));
		if (err2 != KErrNone)
			{
			_LIT(KBURError, "Error setting Backup mode ");
			Log(LOG_LEVEL3, KBURError);
			}
		
		
		return TestStepResult();
		}
	
	void CBURTestStepBackupAsync::BaseBackupL()
		/**
		@internalComponent
		@released
		*/
		{
		// system data
		BackupSystemBaseDataL();
		BackupSystemSnapshotDataL();
		
		//active
		BackupActiveBaseDataL();
		BackupActiveSnapshotDataL();
		
		//passive
		BackupPassiveBaseDataL();
		BackupPassiveSnapshotDataL();
		}
	
	void CBURTestStepBackupAsync::IncrementalBackupL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KInc, "Backup mode: INCREMENTAL");
		Log(LOG_LEVEL2, KInc);
		//active
		SupplySystemSnapshotDataL();
		SupplyActiveSnapshotDataL();
		SupplyPassiveSnapshotDataL();
		
		CBURActiveObject* active = CBURActiveObject::NewL();
		CleanupStack::PushL(active);
		TRAPD(err,iBackupClient->AllSnapshotsSuppliedL(active->iStatus));
		if (err == KErrNone)
			{
			active->StartL();
			}
		
		if (err != KErrNone || active->Error() != KErrNone)
			{
			_LIT(KSnapErr, "Error Supplying Snapshot Data ");
			LogWithNum(LOG_LEVEL3, KSnapErr, err);
			}
		else
			{
			_LIT(KSnap, "All Snapshots Supplied");
			Log(LOG_LEVEL3, KSnap);
			}
		
		CleanupStack::PopAndDestroy(active);
		
		// system data
		BackupSystemBaseDataL();
		BackupSystemSnapshotDataL();
		
		// active
		BackupActiveIncDataL();
		BackupActiveSnapshotDataL();
		
		//passive
		BackupPassiveIncDataL();
		BackupPassiveSnapshotDataL();
		}
		
	void CBURTestStepBackupAsync::BackupSystemBaseDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KLog, "Backup System Data ...");
		Log(LOG_LEVEL2, KLog);
		GeneratePIDTransferTypesL(iTransferTypes, ESystemData);
		DoBackupL();
		}
		
	void CBURTestStepBackupAsync::SupplySystemSnapshotDataL()
		/**
		@internalComponent
		@released
		*/
		{
		GeneratePIDTransferTypesL(iTransferTypes, ESystemSnapshotData);
		// go through each of them and check if we have snapshot to supply
		if (iTransferTypes.Count())
			{
			_LIT(KLog, "Supply System Snapshot Data ...");
			Log(LOG_LEVEL2, KLog);
			SupplyDataL(iTransferTypes);
			}
		}
		
	void CBURTestStepBackupAsync::BackupSystemSnapshotDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KLog, "Backup System Snapshot Data ...");
		Log(LOG_LEVEL2, KLog);
		GeneratePIDTransferTypesL(iTransferTypes, ESystemSnapshotData);
		DoBackupL();
		}
	
	
	void CBURTestStepBackupAsync::BackupRegistrationDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KReg, "Backup registration data...");
		Log(LOG_LEVEL2, KReg);
		GenerateSIDTransferTypesL(iTransferTypes, ERegistrationData);
		DoBackupL();
		}
	
	void CBURTestStepBackupAsync::BackupPassiveSnapshotDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KPassSnap, "Backup passive snapshot data...");
		Log(LOG_LEVEL2, KPassSnap);
		GenerateSIDTransferTypesL(iTransferTypes, EPassiveSnapshotData);
		DoBackupL();
		}
	
	void CBURTestStepBackupAsync::SupplyPassiveSnapshotDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KSupp, "Supplying passive snapshot data...");
		Log(LOG_LEVEL2, KSupp);
		DoSupplyL(EPassiveSnapshotData);
		}

	void CBURTestStepBackupAsync::BackupPassiveBaseDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KPassBase, "Backup passive base data...");
		Log(LOG_LEVEL2, KPassBase);
		GenerateSIDTransferTypesL(iTransferTypes, EPassiveBaseData);
		DoBackupL();
		}
	
	void CBURTestStepBackupAsync::BackupPassiveIncDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KPassInc, "Backup passive incremental data...");
		Log(LOG_LEVEL2, KPassInc);
		GenerateSIDTransferTypesL(iTransferTypes, EPassiveIncrementalData);
		DoBackupL();
		}
	
	void CBURTestStepBackupAsync::BackupActiveSnapshotDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KActSnap, "Backup active snapshot data...");
		Log(LOG_LEVEL2, KActSnap);
		DoActiveBackupL(EActiveSnapshotData);
		}
	
	void CBURTestStepBackupAsync::SupplyActiveSnapshotDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KSuppActSnap,"Supplying active snapshot data...");
		Log(LOG_LEVEL2, KSuppActSnap);
		DoSupplyL(EActiveSnapshotData);
		}
	
	void CBURTestStepBackupAsync::BackupActiveBaseDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KActBase, "Backup active base data...");
		Log(LOG_LEVEL2, KActBase);
		DoActiveBackupL(EActiveBaseData);
		}

	void CBURTestStepBackupAsync::BackupActiveIncDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KActInc, "Backup active incremental data...");
		Log(LOG_LEVEL2, KActInc);
		DoActiveBackupL(EActiveIncrementalData);
		}
		
	void CBURTestStepBackupAsync::BackupPublicDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KPublic, "Backup Public Data...");
		Log(LOG_LEVEL2, KPublic);
		RPointerArray<CSBGenericDataType> array;
		TRAP_IGNORE(GeneratePublicTransferTypesL(array));
		TRAP_IGNORE(DoPublicBackupL(array));
		array.ResetAndDestroy();
		array.Close();
		}
		
	void CBURTestStepBackupAsync::DoPublicBackupL(RPointerArray<CSBGenericDataType>& aTransferTypes)
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KNote, "Public data is Not saved it just for Testing purposes to show the list of files got from the device!");
		Log(LOG_LEVEL2, KNote);
		RFileArray array;
		CleanupClosePushL(array);
		TInt length = iDriveList.Length();
		TInt count = aTransferTypes.Count();
		for (TInt driveCount = 0; driveCount < length && count; driveCount++)
			{
			if (iDriveList[driveCount])
				{
				for (TInt i =0; i < count; i++)
					{
					CBURActiveObject* active = CBURActiveObject::NewL();
					CleanupStack::PushL(active);
					iBackupClient->PublicFileListL(TDriveNumber(driveCount), *aTransferTypes[i], array, active->iStatus);
					active->StartL();
					User::LeaveIfError(active->Error());
					CleanupStack::PopAndDestroy(active);
					
					TInt fileCount = array.Count();
					_LIT(KFoundCount, "Found number of files: ");
					LogWithNum(LOG_LEVEL3, KFoundCount, fileCount);
					
					for (TInt j = 0; j < fileCount; j++)
						{
						_LIT(KFile, "File: ");
						LogWithText(LOG_LEVEL4, KFile, array[j].iName); 
						}
					
					array.Reset();
					}
				}
			}		
		CleanupStack::PopAndDestroy(&array);
		}
	
	void CBURTestStepBackupAsync::DoBackupL()
		/**
		Core backup method to carry out the transfer as well as the storage of 
		data to the archive.
		
		@internalComponent
		@released
		
		@param aDataType - Type of data to be backed up.
		*/
		{
		if (iTransferTypes.Count())
			{
			// STEP 2 - Start the data request handler to backup data:
			_LIT(KLog2, "Requesting backup data for IDs per drive...");
			Log(LOG_LEVEL3, KLog2);
			iDataTransferHandler->StartL();
			// Log success:
			if (iDataTransferHandler->Success())
				{
				_LIT(KLog3, "Operation finished with no errors");
				Log(LOG_LEVEL3, KLog3);
				} //if
			else
				{
				iFailures++;
				_LIT(KLog4, "Operation failed with errors ");
				Log(LOG_LEVEL3, KLog4);
				SetTestStepResult(TVerdict(EFail));
				} //else
			} //if 
		else 
			{
			_LIT(KLogNoTrans, "Nothing to do ");
			Log(LOG_LEVEL3, KLogNoTrans);
			} //else
		}
		
		
	void CBURTestStepBackupAsync::DoActiveBackupL(TTransferDataType aTransferDataType)
		/**
		@internalComponent
		@released
		*/
		{
		RTransferTypeArray transferTypes;
		GenerateSIDTransferTypesL(transferTypes, aTransferDataType);
		if (transferTypes.Count())
			{
			TRAPD(error,
			for (TInt i=0; i < KRetries;)
				{
				CheckSIDStatusL(transferTypes, iTransferTypes);													
				if (iTransferTypes.Count()) // dataowners ready
					{
					DoBackupL();
					} 
				else if (transferTypes.Count()) // data owners not ready
					{
					User::After(KDelay);
					i++;
					}
				else // finished with all sids
					{
					break;
					}
				} // for
				); //TRAPD
			if (transferTypes.Count())
				{
				iFailures++;
				_LIT(KLogNoTrans, "***Error: Some Data Owners were Not Ready or Failed to Connect");
				Log(LOG_LEVEL3, KLogNoTrans);
				} //if
			transferTypes.ResetAndDestroy();
			
			User::LeaveIfError(error);
			}
		else 
			{
			_LIT(KLogNoTrans, "Nothing to do ");
			Log(LOG_LEVEL3, KLogNoTrans);
			}

		}
		
	void CBURTestStepBackupAsync::BackupJavaMidletsL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KMidlet, "Backup midlets...");
		Log(LOG_LEVEL2, KMidlet);
		GenerateJavaTransferTypesL(iTransferTypes, EJavaMIDlet);
		DoBackupL();
		}
		
	void CBURTestStepBackupAsync::BackupJavaMidletsDataL()
		/**
		@internalComponent
		@released
		*/
		{
		_LIT(KMidletData, "Backup midlets data...");
		Log(LOG_LEVEL2, KMidletData);
		GenerateJavaTransferTypesL(iTransferTypes, EJavaMIDletData);
		DoBackupL();
		}
	
	TVerdict CBURTestStepBackupAsync::doTestStepPostambleL()
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
	
	}	// end namespace
