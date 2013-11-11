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
//

/**
 @file
 @released
*/


#include "t_storagemanager.h"
#include "e32cons.h"
#include "e32panic.h"

namespace bur_ts
	{
	CStorageManager* CStorageManager::NewL(const TFileName& aBackupDirName, TDriveList aDriveList, CBURTestStepBase* aTestStep)
		/**
		Symbian OS Constructor
		
		@internalComponent
		@released
		
		@param aBackupDirName - Directory to which all backup files are to be stored in 
			(or read from during a restore operation).
		@param aDriveList - Drives that are being backed up. One subdirectory under per 
			drive will be generated under iBackupDirName.
		@param aTestStep - A pointer to a CBURTestStepBackup or CBURTestStepRestore that 
			owns this object.
		
		@return Pointer to a newly created CStorageManager object.
		*/
		{
		CStorageManager* self = new (ELeave) CStorageManager(aBackupDirName, aDriveList, aTestStep);
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop();
		return self;
		}
	
	CStorageManager::CStorageManager(const TFileName& aBackupDirName, TDriveList aDriveList, CBURTestStepBase* aTestStep)
		/**
		C++ Constructor
		
		@internalComponent
		@released
		
		@param aBackupDirName - Directory to which all backup files are to be stored in 
				(or read from during a restore operation).
		@param aDriveList - Drives that are being backed up. One subdirectory under per 
			drive will be generated under iBackupDirName.
		@param aTestStep - A pointer to a CBURTestStepBackup or CBURTestStepRestore that 
			owns this object.
		*/
		:iBackupDirName(aBackupDirName), iDriveList(aDriveList), iTestStep(aTestStep), iLastFile(KNullDesC), iFirstSupply(ETrue), iBytesRead(0) 		
		{}
	
	void CStorageManager::ConstructL()
		/**
		Symbian OS 2nd phase Constructor
		
		@internalComponent
		@released
		*/
		{
		}
	
	CStorageManager::~CStorageManager()
		/**
		C++ Destructor
	 	
		@internalComponent
		@released
		*/
		{}
	
	void CStorageManager::ArchiveDataL(CSBGenericTransferType* aTransferType, const TDesC8& aData, TBool aFinished)
		/**
		Stores data to the archive for the given transfer type
		
		@internalComponent
		@released
		
		@param aTransferType - A CSBGenericTransferType* containing information about transfer type
		@param aData - Data to be stored
		
		*/
		{
		__ASSERT_DEBUG(aTransferType != NULL, User::Panic(KBURTestPanicText, ENullTargetPointer));
		
		TFileName filename;
		
		if (iFirstSupply && aFinished) //no partial data
			{
			GetFileNameL(aTransferType, filename, ETrue);		
			}
		else if (iFirstSupply && !aFinished) // we start to write
			{
			GetFileNameL(aTransferType, filename, ETrue);
			iFirstSupply = EFalse;
			}
		else if (!iFirstSupply && !aFinished) // we continue
			{
			filename = iLastFile;
			}
		else if (!iFirstSupply && aFinished) // we finish
			{
			filename = iLastFile;
			iFirstSupply = ETrue;
			}
	
		// Handle the error case where no name valid was generated!
		__ASSERT_DEBUG(filename != _L(""), User::Panic(KBURTestPanicText, KErrBURTestInvalidFileName));
		
		WriteToDiskL(filename, aData);
		
		}
	
	void CStorageManager::RetrieveDataL(CSBGenericTransferType* aTransferType, TDes8& aData, TBool& aFinished, TInt aIndex)
		/**
		Retrieves data from the backup archive for the given transfer type.
		
		@internalComponent
		@released
		
		@param aTransferType - A CSBGenericTransferType* containing information about transfer type
		@param aErrorCode - Error code to be set on completion
		@param aIndex - Index of the increment
		
		@return Pointer to the retrieved data on the heap
		
		*/
		{
		__ASSERT_DEBUG(aTransferType != NULL, User::Panic(KBURTestPanicText, ENullTargetPointer));
		
		TFileName filename;
		GetFileNameL(aTransferType, filename, EFalse, aIndex);
		
		ReadFromDiskL(filename, aData, aFinished);		
		}
		
	void CStorageManager::Reset()
	/**
	Resets last name and first supply variables
	*/
		{
		iLastFile = KNullDesC;
		iFirstSupply = ETrue;
		iBytesRead = 0;
		}
		
	void CStorageManager::GetFileNameL(CSBGenericTransferType* aTransferType, TFileName& aFileName, TBool aBackup, TInt aIndex)
		/**
		Creates the name of the file depending on its type
	
		@intenalComponent
		@released
	
		@param aTransferType - A CSBGenericTransferType contains information about transfer type
		@param aFileName - reference to a TFileName
		@param aBackup - is it Backup or Restore
		@param aIndex - index of increment (currently not supported)
		
		*/
		{
		__ASSERT_DEBUG(aTransferType != NULL, User::Panic(KBURTestPanicText, ENullTargetPointer));
		TSBDerivedType derivedType = aTransferType->DerivedTypeL();
		
		if (derivedType == ESIDTransferDerivedType)
			{
			CSBSIDTransferType* type = CSBSIDTransferType::NewL(aTransferType);
			CleanupStack::PushL(type);
				
			GetSIDFileNameL(type, aFileName);
		
			TTransferDataType  dataType = type->DataTypeL();
		
			if (dataType == EActiveIncrementalData || dataType == EPassiveIncrementalData )
				{
				aFileName.Append(KIncrement);
				if (aBackup)
					{
					GenerateIncNumber(TotalIncFiles(type)+1, aFileName);
					//GenerateIncNumber(TotalIncFiles(type)+1, aFileName);
					}
				else
					{
					GenerateIncNumber(aIndex, aFileName);
					}
				aFileName.Append(KBackupExtn);	
				}
			
			CleanupStack::PopAndDestroy(type);
			}
				
		else if (derivedType == EPackageTransferDerivedType)
			{
			CSBPackageTransferType* pkgType = CSBPackageTransferType::NewL(aTransferType);
			CleanupStack::PushL(pkgType);
				
			GetPIDFileNameL(pkgType, aFileName);
			
			CleanupStack::PopAndDestroy(pkgType);
 			}
 		else if (derivedType == EJavaTransferDerivedType)
 			{
 			CSBJavaTransferType* javaType = CSBJavaTransferType::NewL(aTransferType);
 			CleanupStack::PushL(javaType);
 			
 			GetJavaFileNameL(javaType, aFileName);
 			
 			CleanupStack::PopAndDestroy(javaType);
 			}
		}
		
	void CStorageManager::GetPIDFileNameL(CSBPackageTransferType* aTransferType, TFileName& aFileName)
		/**
		Generates file name for the PID
		
		@param aTransferType - A CSBPackageTransferType contains info about Package Type
		@param aFileName - Reference to a filename
		
		*/
		{
		__ASSERT_DEBUG(aTransferType != NULL, User::Panic(KBURTestPanicText, ENullTargetPointer));
		TUid id;
		TChar drive;
		TDriveNumber driveNumber;
		TPackageDataType pkgType;
		
		id = aTransferType->PackageIdL();
		pkgType = aTransferType->DataTypeL();
		driveNumber = aTransferType->DriveNumberL();
		
		//Get the drive letter
		iTestStep->Fs().DriveToChar(driveNumber, drive);
		
		switch(pkgType)
			{
			case ESystemData:
				GetSIDPrivateDirName(drive, id, aFileName);
				aFileName.Append(KData);
				aFileName.Append(KBackupExtn);
				break;
			case ESystemSnapshotData:
				GetSIDPrivateDirName(drive, id, aFileName);
				aFileName.Append(KSnapshot);
				aFileName.Append(KBackupExtn);
				break;
			}
		}
		
	void CStorageManager::GetJavaFileNameL(CSBJavaTransferType* aTransferType, TFileName& aFileName)
		/**
		Concatenates the name of the backup file from the information found in aTransferType.
		
		
		@param 	aTransferType - A CSBJavaTransferType* containing information about the 
					type of file name that needs be returned.
			
		@param TFilename of the backup file generated in line with the 
				implemented naming scheme
		*/
		{
		const TDesC& suiteHash = aTransferType->SuiteHashL();
		TDriveNumber driveNumber = aTransferType->DriveNumberL();
		TJavaTransferType javaType = aTransferType->DataTypeL();
		
		TChar drive;
		
		iTestStep->Fs().DriveToChar(driveNumber, drive);
		
		// we can't create a TSecureID from suiteHash therefore need to duplicate GetSIDPrivateDir Method
		GetJavaPrivateDirName(drive, suiteHash, aFileName);
		
		switch(javaType)
			{
			case EJavaMIDlet:
				aFileName.Append(KMidlet);
				aFileName.Append(KBackupExtn);
				break;
			case EJavaMIDletData:
				aFileName.Append(KData);
				aFileName.Append(KBackupExtn);
				break;
			}
		}
	
	void CStorageManager::GetSIDFileNameL(CSBSIDTransferType* aTransferType, TFileName& aFileName)
		/**
		Concatenates the name of the backup file from the information found in aTransferType.
		
		@internalComponent
		@released
		
		@param 	aTransferType - A CSBSIDTransferType* containing information about the 
				type of file name that needs be returned.
		
		@param TFilename of the backup file generated in line with the 
				implemented naming scheme
				
		*/
		{
		__ASSERT_DEBUG(aTransferType != NULL, User::Panic(KBURTestPanicText, ENullTargetPointer));
		TSecureId sid;
		TChar drive;
		TDriveNumber driveNumber;
		TTransferDataType dataType;
		
		sid = aTransferType->SecureIdL();
		dataType = aTransferType->DataTypeL();
		driveNumber = aTransferType->DriveNumberL();
		
		// Get the drive letter
		iTestStep->Fs().DriveToChar(driveNumber, drive);
		
		switch(dataType)
			{
			case ERegistrationData:
				{
				GetSIDPrivateDirName(drive, sid, aFileName);
				aFileName.Append(KRegFilesDir);
				aFileName.AppendNumUC(sid, EHex);
				aFileName.Append(KBackupExtn);
				break;
				}
				// No need to create this directory, as it's done in SetupDirs():
			case EPassiveSnapshotData:
				{
				GetSIDPrivateDirName(drive, sid, aFileName);
				aFileName.Append(KPassiveDataDir);
				aFileName.Append(KSnapshotDataDir);
				aFileName.Append(KSnapshot);
				aFileName.Append(KBackupExtn);
				break;
				}
			case EPassiveBaseData:
				{
				GetSIDPrivateDirName(drive, sid, aFileName);
				aFileName.Append(KPassiveDataDir);
				aFileName.Append(KBaseDataDir);
				aFileName.Append(KData);
				aFileName.Append(KBackupExtn);
				break;
				} 
			case EPassiveIncrementalData :
				{
				GetSIDPrivateDirName(drive, sid, aFileName);
				aFileName.Append(KPassiveDataDir);
				aFileName.Append(KIncrementalDataDir);
				break;
				}
			case EActiveSnapshotData:
				{
				GetSIDPrivateDirName(drive, sid, aFileName);
				aFileName.Append(KActiveDataDir);
				aFileName.Append(KSnapshotDataDir);
				aFileName.Append(KSnapshot);
				aFileName.Append(KBackupExtn);
				break;
				}
			case EActiveBaseData:
				{
				GetSIDPrivateDirName(drive, sid, aFileName);
				aFileName.Append(KActiveDataDir);
				aFileName.Append(KBaseDataDir);
				aFileName.Append(KData);
				aFileName.Append(KBackupExtn);
				break;
				}
			case EActiveIncrementalData:
				{
				GetSIDPrivateDirName(drive, sid, aFileName);
				aFileName.Append(KActiveDataDir);
				aFileName.Append(KIncrementalDataDir);
				break;
				}
			default:
				{
				User::Leave(KErrArgument);
				break;
				}
			}
		
		}
	
	
	void CStorageManager::GetSIDPrivateDirName(TChar aDrive, TSecureId aSID, TFileName& aFileName) const
		/**
		Gets the name of the directory where the private data is to stored:\n
		\<backup directory>\\KPrivateDirName\\<aSID>\\
		
		@internalComponent
		@released
		
		@param aDrive - The drive letter of the drive where data resides.
		@param aSID - SID the data belonds to.
		
		@param aFileName - Directory name of the given SID's private directory, generated in 
				line with the implemented naming scheme.
		*/
		{
		aFileName = iBackupDirName;
		aFileName.AppendNumUC(aSID, EHex);
		aFileName.Append(KSlash);
		aFileName.Append(aDrive);
		aFileName.Append(KSlash);
		}
		
	void CStorageManager::GetJavaPrivateDirName(TChar aDrive, const TDesC& aSuiteHash, TFileName& aFileName)
		/**
		Gets the name of the directory where the private data is to stored:\n
		\<backup directory>\\KPrivateDirName\\<aSID>\\
		
		@internalComponent
		@released
		
		@param aDrive - The drive letter of the drive where data resides.
		@param aSuiteHash - Suite Hash the data belonds to.
		
		@param aFileName name of the given suite hash private directory, generated in line with the implemented naming scheme.
		*/
		{
		aFileName = iBackupDirName;
		aFileName.Append(aSuiteHash);
		aFileName.Append(KSlash);
		aFileName.Append(aDrive);
		aFileName.Append(KSlash);
		}
		
	TInt CStorageManager::TotalIncFiles(CSBSIDTransferType* aTransferType)
		/**
		Returns number of Files in the directory for SID TransferType
		
		@param aTransferType - CSBSIDTransferType* pointer
		*/
		{
		__ASSERT_DEBUG(aTransferType != NULL, User::Panic(KBURTestPanicText, ENullTargetPointer));
		CDir* dir = NULL;
		TFileName filename;
		GetSIDFileNameL(aTransferType, filename);
		TInt err = iTestStep->Fs().GetDir(filename, KEntryAttMaskSupported, ESortByName, dir);
		if (err != KErrNone)
			{
			return 0;
			}
		return dir->Count();
		}
			
	void CStorageManager::GenerateIncNumber(TInt aIndex, TDes& aDes)
		/**
		Generates number from int to string in range 0 - 999
		
		@param aIndex - Number
		@param aDes - A Descriptor
		*/
	
		{
		if (aIndex <= 9)
			{
			aDes.AppendNum(0);
			aDes.AppendNum(0);
			aDes.AppendNum(aIndex);
			}
		else if (aIndex <= 99)
			{
			aDes.AppendNum(0);
			aDes.AppendNum(aIndex);
			}
		else if (aIndex <= 999)
			{
			aDes.AppendNum(aIndex);
			}
		else
			{
			_LIT(KPanic1, "Number greater then 999 ");
			User::Panic(KPanic1, KErrNotSupported);
			}
		}

		
	void CStorageManager::SaveDataOwnerL(CDataOwnerInfo& aDataOwner)
		/**
		Saves data for DataOwner
		
		@param aDataOwner - DataOwner to be saved
		
		*/

		{		
		CDir* dir = NULL;
		TFileName filename;
		filename.Append(iBackupDirName);
		filename.Append(KDataOwnerDir);
		iTestStep->Fs().MkDirAll(filename);
		_LIT(KPid, "pid");
		_LIT(KSid, "sid");
		_LIT(KJid, "jid");
		
		
		TSBDerivedType type;
		type = aDataOwner.Identifier().DerivedTypeL();
		
		if (type == ESIDDerivedType)
			{
			CSBSecureId* sid = CSBSecureId::NewL(&(aDataOwner.Identifier()));
			CleanupStack::PushL(sid);
			filename.Append(KSid);
			filename.AppendNumUC(sid->SecureIdL(), EHex);
			CleanupStack::PopAndDestroy(sid);
			}
		else if (type == EPackageDerivedType)
			{
			CSBPackageId* pid = CSBPackageId::NewL(&(aDataOwner.Identifier()));
			CleanupStack::PushL(pid);
			filename.Append(KPid);
			filename.AppendNumUC(pid->PackageIdL().iUid, EHex);
			CleanupStack::PopAndDestroy(pid);
			}
		else if (type == EJavaDerivedType)
			{
			CSBJavaId* jid = CSBJavaId::NewL(&(aDataOwner.Identifier()));
			CleanupStack::PushL(jid);
			filename.Append(KJid);
			filename.Append(jid->SuiteHashL());
			CleanupStack::PopAndDestroy(jid);
			}
		
		filename.Append(KBackupExtn);
		//HBufC8* data = HBufC8::NewLC(aDataOwner.Size());
		
		HBufC8* data = aDataOwner.ExternaliseLC();
		WriteToDiskL(filename, *data);
		CleanupStack::PopAndDestroy(data);
		delete dir;
		dir = NULL;
		}
		
	void CStorageManager::WriteToDiskL(TFileName& aFile, const TDesC8& aData)
		/**
		Writes data to disk
		
		@param aFile - file to write to
		@param aData - data to write
		
		*/
		{
		RFile file;
		// Place on the cleanup stack:
		CleanupClosePushL(file);
		TInt error = iTestStep->Fs().MkDirAll(aFile);
		
		if (error == KErrAlreadyExists || error == KErrNone) // directory exists
			{
			if (iLastFile == aFile) // more data needs appended to already open file
				{
				error = file.Open(iTestStep->Fs(), aFile, EFileWrite);	
				}
			else 
				{
				error = file.Replace(iTestStep->Fs(), aFile, EFileWrite);
				}
			}
		if (error != KErrNone)
			{
			CleanupStack::PopAndDestroy(&file); // file
			User::Leave(error)	;
			}
		
		TInt size = NULL;
		file.Size(size);
		error = file.Write(size, aData);
		file.Flush();
		
		iLastFile = aFile;	
			
		CleanupStack::PopAndDestroy(&file); // file
		User::LeaveIfError(error);
		}
		
	void CStorageManager::ReadFromDiskL(TFileName& aFile, TDes8& aData, TBool& aFinished)
		/**
		Read data from a disk
		
		@param aFile - file to read
		@param aData - reference to a buffer to put the data from a file
		
		*/
		{
		RFile file;
		CleanupClosePushL(file);
				
		TInt err = file.Open(iTestStep->Fs(), aFile, EFileRead);
		if (err != KErrNone)
			{
			CleanupStack::PopAndDestroy(&file);
			User::Leave(err);
			}
		
		TInt size;
		file.Size(size);
		
		TInt availableSpace = aData.MaxLength() - aData.Length();
		
		if (availableSpace - (size - iBytesRead) >= 0)
			{
			aFinished = ETrue;
			}
		else
			{
			aFinished = EFalse;	
			}
		
		err = file.Read(iBytesRead, aData);
		file.Flush();
		CleanupStack::PopAndDestroy(&file);
		
		User::LeaveIfError(err);
		
		if (aFinished)
			{
			iBytesRead = 0;
			}
		else
			{
			iBytesRead += availableSpace;
			}
		}
		
	void CStorageManager::ReadDataOwnersFromDiskL(RDataOwnerArray& aArray)
		/**
		Reads Data Owners from a storage
		
		@param aArray - RDataOwnerArray will contain dataowners found in storage
		
		*/

		{
		aArray.ResetAndDestroy();
		CDir* dir = NULL;
		TFileName filename = iBackupDirName;
		filename.Append(KDataOwnerDir);
		User::LeaveIfError(iTestStep->Fs().GetDir(filename, KEntryAttMaskSupported, ESortByName, dir));
		TInt totalFiles = dir->Count();
		
		for (TInt index = 0; index < totalFiles; index++)
			{
			TFileName file = filename;
			TEntry entry = (*dir)[index];
			file.Append(entry.iName);
			HBufC8* data = HBufC8::NewLC(entry.iSize);
			TPtr8 pData = data->Des();
			TBool ignore;
			TRAPD(err, ReadFromDiskL(file, pData, ignore));
			if (err != KErrNone)
				{
				_LIT(KLog1, "Error opening file: ");
				iTestStep->LogWithText(LOG_LEVEL2, KLog1, file);
				CleanupStack::PopAndDestroy(data);
				continue;
				}
			CDataOwnerInfo* dataOwner = CDataOwnerInfo::NewL(pData);
			CleanupStack::PushL(dataOwner);
			aArray.AppendL(dataOwner);
			CleanupStack::Pop(dataOwner);
			CleanupStack::PopAndDestroy(data);
			}
		}
	
	TBool CStorageManager::IsFileExists(TFileName& aFileName)
		/**
		Is File Exists in the File System
		
		@param aFileName File Name of the file to check
		
		@return ETrue if exists
		*/
		{
		TEntry entry;
		TInt err = iTestStep->Fs().Entry(aFileName, entry);
		if (err == KErrNone)
			{
			return ETrue;
			}
		else
			{
			return EFalse;
			}
		}
	
	}	// end namespace
