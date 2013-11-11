/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file
 @released
*/

#ifndef __T_BUR_TEST_STEP_BASE_H__
#define __T_BUR_TEST_STEP_BASE_H__

#include <test/testexecutestepbase.h>
#include <connect/sbeclient.h>
#include "t_burcommon.h"
#include "t_burtestserver.h"
#include "t_storagemanager.h"

using namespace conn;

namespace bur_ts
	{
	
	const TInt KRetries = 3;
	const TInt KDelay = 10000000; //10 sec
	
	// Forward declaration
	class CStorageManager;
	
	class CBURTestStepBase : public CTestStep
		/**
		Base class for all test cases to be used by BURTestServer.
		It provides common functionality to backup and restore, such as supplying data
		from the archive, setting backup/restore mode, getting list of data owners, etc.
		
		This class may not be directly instantiated. All derived classes must supply 
		implementations of all pure virtual functions inherited from CTestStep 
		(from TestExecute Framework), namely:
		
		virtual TVerdict doTestStepPreambleL();\n
		virtual TVerdict doTestStepPostambleL();\n
		virtual TVerdict doTestStepL();
		
		The logging methods provide an interface to the text execute logging utility.
		
		@released
		@internalComponent
		*/
		{
	protected:
		// No need for 2-phase construction here and this is an abstract class.
		CBURTestStepBase(CBURTestServer& aTestServer);
		~CBURTestStepBase();
		
	public:
		// Utility function to convert user input into a TDriveList object
		static TDriveList StringToDriveListL(TPtrC apText);
		
		// For logging to the TestExecute logger:
		void Log(TUint aIndentationLevel, const TDesC& aLog);
		void LogWithText(TUint aIndentationLevel, const TDesC& aLog, const TDesC& aText);
		void LogWithText8(TUint aIndentationLevel, const TDesC& aLog, const TDesC8& aText);
		void LogWithNum(TUint aIndentationLevel, const TDesC& aLog, TInt aNum);
		void LogWithSID(TUint aIndentationLevel, const TDesC& aLog, TSecureId aSecureId);
		void LogWithChar(TUint aIndentationLevel, const TDesC& aLog, TChar aChar);
		
		// Inline methods:
		
		inline RFs& Fs();
		inline CSBEClient* BackupClient() const;
		inline CStorageManager* StorageManager() const;
		inline const RTransferTypeArray& TransferTypes() const;
		
	public:
		RSIDArray iSidArray;
		
	private:
		HBufC* GetIndentation(TInt aLevel);
		
	protected:
		// Methods required by both CTestStepBackup as well CTestStepRestore:
		void ProcessUserOptionsL();
		
		void PrepareForPartialBURL();
		
		void PopulateListOfDataOwnersL();
		
		void SetBURModeL(TBURPartType aBURPartType, TBackupIncType aBackupIncType);
		
		void CreateStorageManagerL();
		
		void GenerateSIDTransferTypesL(RTransferTypeArray& aTransferTypes, TTransferDataType aDataType);
		
		void GeneratePIDTransferTypesL(RTransferTypeArray& aTransferTypes, TPackageDataType aPkgDataType = ESystemData);
		
		void GenerateJavaTransferTypesL(RTransferTypeArray& aTransferTypes, TJavaTransferType aJavaTransferType);
		
		void SupplyDataL(RTransferTypeArray& aTransferTypes);
		
		void SaveDataOwners();
		
		void CheckSIDStatusL(RTransferTypeArray& aTransferTypes, RTransferTypeArray& aReadyArray);
		
		TSecureId ExtractIDL(CDataOwnerInfo& aDataOwner);
		
		void DoSupplyL(TTransferDataType aDataType);
		
		HBufC* ExtractJavaIDL(CDataOwnerInfo& aDataOwner);
		
		void CheckValidRegistrationL();
		
		void SetSIDListForPartialL();
		
		void GeneratePublicTransferTypesL(RPointerArray<CSBGenericDataType>& aTransferTypes);
		
		//async methods
		void PopulateListOfDataOwnersAsyncL();
		
		void SetBURModeAsyncL(TBURPartType aBURPartType, TBackupIncType aBackupIncType);
		
		
	protected:
		/** Test server the test step belongs to: */
		CBURTestServer& iParentTestServer;
		
		/** Data owners */
		RDataOwnerArray iDataOwners;
		
		/** Backup server client */
		CSBEClient* iBackupClient;
		
		/** User defined parameters read in by TestExecute config parser */
		TFileName iBackupDirName;
		TBackupIncType iBackupIncType;
		TBool iIsPartial;
		TDriveList iDriveList;
		
		/** Number of failures */
		TInt iFailures;
		
		/** Storage handler for archiving backup data */
		CStorageManager* iStorageManager;
		
		/** Active scheduler for this test step's thread */
		CActiveScheduler* iActiveScheduler;
		
		/** File server session for use by all derived classes */
		RFs iFs;
		
		/** Array of suite Hashes */
		RArray<TPtrC> iJidArray;
		
		/** Restore or Backup ? */
		TBackupMode iMode;
		
		/** waited for active to become ready? */
		TBool iWaitedForActive;
		
		/** Array of transfer types */
		RTransferTypeArray iTransferTypes;
		};
		
	// INLINE METHODS:
	inline RFs& CBURTestStepBase::Fs()
		/**
		Provides access to the central file server session.
		
		@released
		@internalComponent
		
		@return An RFs file server session.
		*/
		{
		return iFs; 
		}
	
	inline CSBEClient* CBURTestStepBase::BackupClient() const
		/**
		Provides access to the backup client.
		
		@released
		@internalComponent
		
		@return A pointer to the backup client.
		*/
		{
		return iBackupClient;
		}
	
	inline CStorageManager* CBURTestStepBase::StorageManager() const
		/**
		Provides access to the stroage manager.
		
		@released
		@internalComponent
		
		@return A pointer to the storage manager.
		*/
		{
		return iStorageManager;
		}
		
	inline const RTransferTypeArray& CBURTestStepBase::TransferTypes() const
		/**
		Provides access to the Transfer Types.
		
		@released
		@internalComponent
		
		@return A reference to the transfer types.
		*/
		{
		return iTransferTypes;
		}

	/**
	A Simple Active Object class for simulating Asyncrnous calls.
	*/	
	class CBURActiveObject : public CActive
		{
	public:
		static CBURActiveObject* NewL();
		~CBURActiveObject();
		
		void StartL();
		TInt Error();
		
	protected:
		void DoCancel();
		void RunL();
		TInt RunError(TInt aError);
		
	private:
		CBURActiveObject();
		void ConstructL();
		
	private:
		CActiveSchedulerWait* iActiveScheduler;
		TInt iError;
		};
	
	} // end of namespace

#endif // __T_BUR_TEST_STEP_BASE_H__


