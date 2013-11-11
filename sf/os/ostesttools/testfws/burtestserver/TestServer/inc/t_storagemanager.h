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

#ifndef __STORAGE_MANAGER_H__
#define __STORAGE_MANAGER_H__

#include <e32cmn.h>
#include <s32file.h>
#include "t_burteststepbase.h"

using namespace conn;

namespace bur_ts
	{
	// Forward declaration
	class CBURTestStepBase;
	
	class CStorageManager : public CBase
		/**
		Handles storage management for both backup and restore.
		
		@released
		@internalComponent
		*/
		{
	public:
		static CStorageManager* NewL(const TFileName& aBackupDirName, TDriveList aDriveList, CBURTestStepBase* aTestStep);
		~CStorageManager();
		
		// Stores data in the archive during a backup operation
		void ArchiveDataL(CSBGenericTransferType* aTransferType, const TDesC8& aData, TBool aFinished);
		
		// Retrieves data from the archive during a restore operation
		void RetrieveDataL(CSBGenericTransferType* aTransferType, TDes8& aData, TBool& aFinished, TInt aIndex = 0);
		
		// generates transfer type array by parsing archive directory
		//void GenerateSIDTransferTypeArrayL(RTransferTypeArray& aArray, TTransferDataType aDataType);
		
		// returns total number of files stored in the directory for Incrementals
		TInt TotalIncFiles(CSBSIDTransferType* aTransferType);
		
		void SaveDataOwnerL(CDataOwnerInfo& aDataOwner);
		
		void ReadDataOwnersFromDiskL(RDataOwnerArray& aArray);
		
		void GetFileNameL(CSBGenericTransferType* aTransferType, TFileName& aFileName, TBool aBackup, TInt aIndex = 1);
		
		TBool IsFileExists(TFileName& aFileName);
		
		void Reset();
		
	private:
		CStorageManager(const TFileName& aBackupDirName, TDriveList aDriveList, CBURTestStepBase* aTestStep);
		void ConstructL();
		
		// Following methods implement the backup archive naming scheme:
		void GetSIDFileNameL(CSBSIDTransferType* aTransferType, TFileName& aFileName);
		void GetSIDPrivateDirName(TChar aDrive, TSecureId aSeID, TFileName& aFileName) const;
		
		void GetPIDFileNameL(CSBPackageTransferType* aTransferType, TFileName& aFileName);
		void GetJavaFileNameL(CSBJavaTransferType* aTransferType, TFileName& aFileName);
		
		void GetJavaPrivateDirName(TChar aDrive, const TDesC& aSuiteHash, TFileName& aFileName);
		
		void GenerateIncNumber(TInt aIndex, TDes& aDes);
		
		void WriteToDiskL(TFileName& aFileName, const TDesC8& aData);
		
		void ReadFromDiskL(TFileName& aFileName, TDes8& aData, TBool& aFinished);
		
	private:
		/** The name of the backup directory */
		TFileName iBackupDirName;
		
		/** List of drives being backed up */
		TDriveList iDriveList;
		
		/** Parent test step for callbacks to various logging methods */
		CBURTestStepBase* iTestStep;
		
		/** Last file it which was written to */
		TFileName iLastFile;
		
		/** firstSupply */
		TBool iFirstSupply;
		
		/** bytes read */
		TInt iBytesRead;
		};
	}

#endif // __STORAGE_MANAGER_H__

