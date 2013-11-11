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

#include "srvdefs.h"
#include "srvres.h"
#include "shrepos.h"
#include "srvrepos_noc.h"
#include "obsrvr_noc.h"
#include "srvparams.h"

CSharedRepository* CSharedRepository::NewL(TUid aUid)
	{
	CSharedRepository* self = new(ELeave) CSharedRepository();
	CleanupStack::PushL(self);
	self->ConstructL(aUid);
	CleanupStack::Pop(self);
	
	// debug check that CRepository::TTransactionMode modes match those used internally
	// from CRepositoryTransactor: internal state logic relies on this
	// there should be a better location for these checks...
	ASSERT(CRepository::EReadTransaction == static_cast<CRepository::TTransactionMode>(EReadTransaction));
	ASSERT(CRepository::EConcurrentReadWriteTransaction == static_cast<CRepository::TTransactionMode>(EConcurrentReadWriteTransaction));
	ASSERT(CRepository::EReadWriteTransaction == static_cast<CRepository::TTransactionMode>(EReadWriteTransaction));
	
	return self;
	}
	
void CSharedRepository::ConstructL(TUid aUid)
	{
	iSimRep = CHeapRepository::NewL(aUid);
	}

CSharedRepository::CSharedRepository() : iNotificationState(ETrue)
	{
	}
	
CSharedRepository::~CSharedRepository()
	{
	if (iSimRep)
		{
		delete iSimRep;
		}
	}

TUid CSharedRepository::Uid() const
	{
	return iSimRep->Uid();
	}

/**
Stores the repository in-memory content to the related repository file on drive C.
If the operation fails, the in-memory content won't match the content of 
the repository file (which will be kept as it was before the CommitChangesL() call).
In order to keep the consistency, the in-memory repository content is deleted now
and restored later, on the next repository operation.
*/
TInt CSharedRepository::CommitChanges(TCentRepLocation aLocation)
	{
	iInconsistentData=ETrue;
	
	HBufC* filePath(NULL);	   
	TRAPD(err,TServerResources::CreateRepositoryFileNameL(filePath,iSimRep->Uid(),aLocation,ECre));	
	if (err!=KErrNone)
		{
		iSimRep->ResetContent();
		return err;		
		}
	
	// should not be committing while transactions are still active
	ASSERT(!IsTransactionActive());
	TInt ret=iSimRep->CommitChanges(TServerResources::iFs,TServerResources::iPersistsVersion,*filePath);
	if (ret==KErrNone)
		{
		iInconsistentData=EFalse;	
		}
	delete filePath;
	return ret;
	}

// merge transaction settings (which may include entries flagged as deleted), persist and notify
// private method relies on calling code to ensure it is permitted to make changes here.
// if this method is committing any changes, it cancels all other sessions' transactions
TInt CSharedRepository::DoCommitTransactionSettings(CRepositoryTransactor& aTransactor, TUint32& aKeyInfo)
	{
	aKeyInfo = KUnspecifiedKey;
	if (0 == aTransactor.iTransactionSettings.Count())
		{
		aKeyInfo = 0; // == number of settings modified
		return KErrNone; // nothing to do
		}
	TInt error = iSimRep->SettingsArray().MergeArray(aTransactor.iTransactionSettings, iSimRep->DeletedSettingsArray(), ETransactionMerge);
	TInt numChanges = aTransactor.iTransactionSettings.Count();
	if (numChanges == 0)
		{
		if (error == KErrNone)
			{
			aKeyInfo = 0; // no changes
			}
		// no changes were made, so the internal cache is still valid.
		// This could be because there were no changes: empty list, only deletes on
		// non-existent items (a setting created and deleted in the transaction),
		// or because of error, such as failure of an initial realloc failure.
		return error;
		}
	if (error != KErrNone)
		{
		// the repository is corrupted. Dump it for lazy loading later
		ResetContent();
		
		// mark cache as inconsistent so it is reloaded.
		iInconsistentData = ETrue;	
		return error;	
		}
	if (error == KErrNone)
		{
		// changes have been made: fail all other sessions' transactions so we can commit
		FailAllTransactions(/*aExcludeTransactor=*/&aTransactor);
		error = CommitChanges(); // this already calls ResetContent() in case of failure
		}
	if (error == KErrNone)
		{
		// settings are now persistent on disk: we can now notify about the changes
		// following will notify about objects that are created and deleted in the transaction
		// this could be made faster by having a multiple Notify method.
		// That would also allow Notify messages to be more descriptive - ranges of Keys
		for (TInt i = 0; i < numChanges; i++)
			{
			Notify(aTransactor.iTransactionSettings[i].Key());
			}
		aKeyInfo = /*reinterpret_cast<TUint32>*/numChanges;
		}
	return error;
	}

void CSharedRepository::SetMetaDataOnRead(TServerSetting& aSetting, TBool aSingleMetaFound)
	{
	iSimRep->SetMetaDataOnRead(aSetting, aSingleMetaFound);
	}

void CSharedRepository::SetMetaDataOnCreate(TServerSetting& aNewSetting, TUint32* aMeta)
	{
	if(aMeta)
		{
		aNewSetting.SetMeta(*aMeta);
		}
	else
		{
		// No metadata specified. First check for a matching "range" default
		// metadata setting
		TSettingsDefaultMeta* defaultMeta = iSimRep->RangeMetaArray().Find(aNewSetting.Key());
		if (defaultMeta)
			{
			aNewSetting.SetMeta(defaultMeta->GetDefaultMetadata());
			}
		else
			{
			// Range value not found, try for a repository default
			aNewSetting.SetMeta(iSimRep->DefaultMeta());
			}
		}	
	}

void CSharedRepository::CreateL(TServerSetting& aSetting, TSettingsAccessPolicy*& aPolicy, TBool aFirstLoad, TBool aSingleMetaFound)
	{
	User::LeaveIfError(iSimRep->Create(aSetting, aPolicy, aSingleMetaFound));
	if (!aFirstLoad)
		{
		Notify(aSetting.Key());
		}
	}

// deletes an individual setting in the shared repository and makes it persistent
// if changes are made, all sessions' transactions are failed
TInt CSharedRepository::DeleteAndPersist(TUint32 aId)
	{
	TServerSetting* s = iSimRep->SettingsArray().Find(aId);
	if(!s)
		return KErrNotFound;
	iSimRep->SettingsArray().Remove(aId);
	
	// removed a setting, so must fail all sessions' transactions before commit possible
	FailAllTransactions(/*aExcludeTransactor=*/NULL);
	TInt error = CommitChanges();
	if (error == KErrNone)
		{
		Notify(aId);
		}
	return error;
	}

// deletes an individual setting without making it persistent
// must not be called while any sessions are in transactions
TInt CSharedRepository::DeleteNoPersist(TUint32 aId)
	{
	// should only be calling this if no transactions are active
	ASSERT(!IsTransactionActive());
	TServerSetting* s = iSimRep->SettingsArray().Find(aId);
	if(!s)
		return KErrNotFound;

	iSimRep->SettingsArray().Remove(aId);
	return KErrNone;	
	}

TInt CSharedRepository::ResetNoPersistL(TServerSetting& aSetting)
	{
	TServerSetting* s = iSimRep->SettingsArray().Find(aSetting.Key());
	if ((!s) || (*s != aSetting))
		{
		if (s)
			{
			// save access policy of setting
			TSettingsAccessPolicy* policy=s->AccessPolicy();
			s->Transfer(aSetting);
			// restore access policy of setting
			s->SetAccessPolicy(policy);
			}
		else
			{
			TServerSetting setting;
			setting.Transfer(aSetting);
			setting.SetAccessPolicy(GetFallbackAccessPolicy(setting.Key()));
			setting.PushL();
			iSimRep->SettingsArray().OrderedInsertL(setting);
			
			TInt index = iSimRep->DeletedSettingsArray().FindInUnsignedKeyOrder(aSetting.Key());
			if (index != KErrNotFound)
				iSimRep->DeletedSettingsArray().Remove(index);
	
			setting.Pop();
			}
		}
	else
		{
		return KErrGeneral;
		}
	return KErrNone;
	}

// if changes are made, all sessions' transactions are failed
void CSharedRepository::ResetAndPersistL(TServerSetting& aSetting)
	{
	if (ResetNoPersistL(aSetting) == KErrNone)
		{
		// changed a setting, so must fail all sessions' transactions
		// before commit possible
		FailAllTransactions(/*aExcludeTransactor=*/NULL);
		CommitChangesL();
		Notify(aSetting.Key());
		}
	}

TInt CSharedRepository::ResetAllNoPersistL(CSharedRepository& aNewContent)
	{
	// mark cache as inconsistent in case Reset fails, so it is reloaded.
	iInconsistentData=ETrue;

	// should not change repository while transactions in progress: should fail them first
	ASSERT(!IsTransactionActive());
	TInt newCount = (aNewContent.iSimRep)->SettingsArray().Count();
	TInt count = iSimRep->SettingsArray().Count();

	TInt newIndex = 0;
	TInt index = 0;

	while(newIndex<newCount && index<count)
		{
		const TServerSetting& newSetting = (aNewContent.iSimRep)->SettingsArray()[newIndex];
		const TServerSetting& setting = iSimRep->SettingsArray()[index];

		TUint32 newKey = newSetting.Key();
		TUint32 key = setting.Key();

		if(newKey<key)
			{
			Notify(newKey);
			newIndex++;
			}
		else if(newKey==key)
			{
			if(newSetting!=setting)
				{
				Notify(key);
				}
			newIndex++;
			index++;
			}
		else if(newKey>key)
			{
			Notify(key);
			index++;
			}
		}

	while(newIndex<newCount)
		{
		Notify((aNewContent.iSimRep)->SettingsArray()[newIndex++].Key());
		}
		
	while(index<count)
		{
		Notify(iSimRep->SettingsArray()[index++].Key());
		}

	// Replace current settings with settings read from ROM, this 
	// will leave settings pointing to new single policies
	iSimRep->SettingsArray().AdoptL((aNewContent.iSimRep)->SettingsArray());

	// Reset policy pointers to point at this repositories policies
	newCount=iSimRep->SettingsArray().Count();
	for(TInt i=0; i<newCount;i++)
		{
		(iSimRep->SettingsArray())[i].SetAccessPolicy(NULL);
		TUint32 key = (iSimRep->SettingsArray())[i].Key();
		(iSimRep->SettingsArray())[i].SetAccessPolicy(GetFallbackAccessPolicy(key));
		}	
	
	iSimRep->DeletedSettingsArray().Reset();

	iInconsistentData=EFalse;
	return KErrNone;
	}
	
// returns the read security policy used if there is no per-setting policy at aId
const TSecurityPolicy& CSharedRepository::GetFallbackReadAccessPolicy(TUint32 aId)
	{
	return iSimRep->GetFallbackReadAccessPolicy(aId);
	}

// returns the write security policy used if there is no per-setting policy at aId
const TSecurityPolicy& CSharedRepository::GetFallbackWriteAccessPolicy(TUint32 aId)
	{
	return iSimRep->GetFallbackWriteAccessPolicy(aId);
	}

// Get pointer to security policy that applies to a given setting
TSettingsAccessPolicy* CSharedRepository::GetFallbackAccessPolicy(TUint32 aId
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,TBool aSkipSingle
#endif	
	)
	{
	return iSimRep->GetFallbackAccessPolicy(aId
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,aSkipSingle
#endif	
	);
	}


TInt CSharedRepository::ReadSettingSavePolicyL(CIniFileIn& aFile,TServerSetting& aSetting, TSettingsAccessPolicy*& aPolicy, TBool& aSingleMetaFound)
	{
	return iSimRep->ReadSettingSavePolicyL(aFile, aSetting, aPolicy, aSingleMetaFound);
	}	

// Merge settings in this->iSettings with the iSettings of aMergeRep
// During an intsall/upgrade event aMergeRep will be created from the installed file
// During an upinstall event aMergeRep will be created from the ROM file
void CSharedRepository::MergeL(CSharedRepository& aMergeRep, TMergeType aMergeType)
	{
	// Process settings from main section - this updates values only
	User::LeaveIfError(GetSettings().MergeArray(aMergeRep.GetSettings(), iSimRep->DeletedSettingsArray(), aMergeType));

	//if the merging is due to a ROM Flash, we need to copy over both the NEW ROM keypsace global properties
	//(default access policies/metadata and range policies/metadata), individual policies, we then need to ensure
	//that the settings point at the correct individual policies and metadata.
	if (aMergeType==ERomFlash)
		{
		//copy the default/range/individual policy
		iSimRep->SetDefaultPolicy(aMergeRep.iSimRep->GetDefaultAccessPolicy());
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
		iSimRep->GetDefaultAccessPolicy().iHighKey=aMergeRep.iSimRep->GetDefaultAccessPolicy().iHighKey;
		iSimRep->GetDefaultAccessPolicy().iKeyMask=aMergeRep.iSimRep->GetDefaultAccessPolicy().iKeyMask;		
#endif		
		iSimRep->RangePolicyArray().Reset();
		TInt count=aMergeRep.iSimRep->RangePolicyArray().Count();
		iSimRep->RangePolicyArray().ReserveL(count);
		for (TInt i=0;i<count;i++)
			{
			iSimRep->RangePolicyArray().AppendL(aMergeRep.iSimRep->RangePolicyArray()[i]);
			}
		iSimRep->SinglePolicyArray().ResetAndDestroy();
		count=aMergeRep.iSimRep->SinglePolicyArray().Count();
		iSimRep->SinglePolicyArray().ReserveL(count);
		for (TInt i=0;i<count;i++)
			{
			iSimRep->SinglePolicyArray().AppendL(aMergeRep.iSimRep->SinglePolicyArray()[i]);
			}
		//now need to reset the aMergeRep single policies so it is not going to destroy the
		//individual policies as ownership has been transferred
		aMergeRep.iSimRep->SinglePolicyArray().Reset();
		
		//copy the default/range metadata
		iSimRep->SetDefaultMeta(aMergeRep.iSimRep->DefaultMeta());
		iSimRep->RangeMetaArray().Reset();
		count=aMergeRep.iSimRep->RangeMetaArray().Count();
		iSimRep->RangeMetaArray().ReserveL(count);		
		for (TInt i=0;i<count;i++)
			{
			iSimRep->RangeMetaArray().AppendL(aMergeRep.iSimRep->RangeMetaArray()[i]);
			}
		
		//set the timestamp,owner etc
		iSimRep->SetTimeStamp(aMergeRep.iSimRep->TimeStamp());
		iSimRep->SetOwner(aMergeRep.iSimRep->Owner());
		}

	// Update all access policies and meta
	for(TInt i=0; i<iSimRep->SettingsArray().Count();i++)
		{
		TServerSetting& setting= iSimRep->SettingsArray()[i];
		setting.SetAccessPolicy(GetFallbackAccessPolicy(setting.Key()));
		}	
	}
	
// Save timestamp of installed file 
void CSharedRepository::SetInstallTime(TTime aInstallTime)
	{
	iSimRep->SetTimeStamp(aInstallTime);
	}
	
// Handle creation or upgrade of file in install directory
void CSharedRepository::HandleUpdateMergeL(TTime aInstallFileTimeStamp, CSharedRepository& aInstallRep)
	{			
	MergeL(aInstallRep, ESWIUpgradeMerge);
	
	SetInstallTime(aInstallFileTimeStamp);	// Set merge timestamp		
	CommitChangesL();						// Commit changes to write system drive file

	// settings are now persistent on disk: we can now notify about the changes
	for (TInt i = 0; i < (aInstallRep.iSimRep)->SettingsArray().Count(); i++)
		{
		Notify((aInstallRep.iSimRep)->SettingsArray()[i].Key());
		}
	}
	
// Handle merge activity due to an uninstall
void CSharedRepository::HandleDeleteMergeL(CSharedRepository& aRomRep)
	{
	MergeL(aRomRep, ESWIDowngradeMerge);
	
	SetInstallTime(0);						// Reset timestamp			
	CommitChangesL();						// Commit changes to write system drive file
	
	// settings are now persistent on disk: we can now notify about the changes
	for (TInt i = 0; i < (aRomRep.iSimRep)->SettingsArray().Count(); i++)
		{
		Notify((aRomRep.iSimRep)->SettingsArray()[i].Key());
		}
	}
	

#ifdef CENTREP_CONV_TOOL
/**
Statement "iInconsistentData = ETrue;" must be the first statement in the method,
"iInconsistentData = EFalse;" must be the last. It is used for lasy-load implementation
for the repository and solves the problem that if CommitChangesL() fails the in-memory
repository data won't match the repository data, stored in the file.
This routine is being retained for testing purposes
*/
void CSharedRepository::DoCommitChangesToIniFileL(const TDesC& aOutFileName
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,TUint32 aCreVersion
#endif
	)
	{
	iInconsistentData=ETrue;

	// should not be committing while transactions are still active
	ASSERT(!IsTransactionActive());

	iSimRep->DoCommitChangesToIniFileL(TServerResources::iFs,aOutFileName
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,aCreVersion
#endif	
	);
	
	iInconsistentData = EFalse;
	}
#endif //CENTREP_CONV_TOOL
	
/**
The method reloads the repository content from a repository file.
The current repository must be emptied (or must be empty already) before the call is made.
@param aIniFile A reference to CIniFileIn object, which will be used to load
				the repository content.
@param aFirstLoad is used to indicate whether the file is reloaded for first time, this is used to prevent
notification if not needed. For example file loading for merging purpose will not result in notification
@return KErrCorrupt Corrupted repository file.
		KErrNone	The repository content was seccessfully loaded into memory.
		KErrNotFound Setting not found in the file.
@leave System-wide error codes.
@leave KErrGeneral It's probably a programmer's error - current CSharedRepository 
				   object is partially initialised.
*/
TInt CSharedRepository::ReloadContentL(CIniFileIn& aIniFile, TBool aFirstLoad)
	{
	// Preconditions - CHeapRepository object should be an empty one.
	if(!iSimRep->IsEmpty())
		{
		User::Leave(KErrGeneral);
		}
	TInt err = iSimRep->ReloadContentExceptSettingsL(aIniFile);
	if(err == KErrCorrupt)
		{
		return err;
		}
	CleanupClosePushL(iSimRep->RangeMetaArray());
	CleanupClosePushL(iSimRep->RangePolicyArray());
	
	// Settings
	TServerSetting setting;
	TSettingsAccessPolicy* policy;
	TBool singleMetaFound;
	TCleanupItem tc(CHeapRepository::SinglePoliciesCleanup, &(iSimRep->SinglePolicyArray()));
	CleanupStack::PushL(tc);	
	CleanupClosePushL(iSimRep->SettingsArray());	
	while((err = ReadSettingSavePolicyL(aIniFile, setting, policy, singleMetaFound)) == KErrNone)
		{
		setting.PushL();
		if(iSimRep->SettingsArray().IsDefault())
			{
			setting.SetClean();			
			}
		CreateL(setting, policy, aFirstLoad, singleMetaFound);
		setting.Pop();
		}
	if(err == KErrNotFound)			
		{
		err = KErrNone;
		}
	if (err == KErrNone)
		{
		CleanupStack::Pop(4,&(iSimRep->RangeMetaArray()));		
		}
	else
		{
		CleanupStack::PopAndDestroy(4,&(iSimRep->RangeMetaArray()));		
		}
	return err;
	}

/**
Resets current repository data - actually all of them, which may be loaded from
the related ini file.
The iUid data member value is kept as it was at the moment of creation of 
CSharedRepository object.
*/
void CSharedRepository::ResetContent()
	{
	iSimRep->ResetContent();
	}

/**
This function is used to restore the notification, which was temporary disabled
when making RestoreConsistencyL() call.
@param aNotificationState It points to CObservable::iNotificationState data member, which 
						 controls the notification state - active or disabled.
@internalComponent
*/
static void RestoreNotification(void* aNotificationState)
	{
	TBool* notificationState = static_cast <TBool*> (aNotificationState);
	*notificationState = ETrue;
	}

/**
The method reloads the repository content from the related ini file if previous
CommitChangesL() has not completed successfully.
*/
void CSharedRepository::RestoreConsistencyL()
	{
	//Do nothing if previous CommitChangesL() completed successfully.
	if (!iInconsistentData)
		{
		return;
		}
	//Reset current repository data	
	ResetContent();
	//Disable notifications
	TCleanupItem restoreNotification(&RestoreNotification, &iNotificationState);
	CleanupStack::PushL(restoreNotification);
	iNotificationState = EFalse;
	//Reload the repository content from the related ini file
	DoRestoreConsistencyL();
	//Activate notifications
	CleanupStack::PopAndDestroy();//restoreNotification
	
	TCentRepLocation location = EPersists;
	HBufC* persistsTmpFilePath(NULL);
    //allocates memory on the heap
    TServerResources::CreateRepositoryFileNameLC(persistsTmpFilePath,iSimRep->Uid(),location,ETmp);
	// Remove any .tmp file
	// If a debug build - record error
	TInt fileDeleteErr=TServerResources::iFs.Delete(*persistsTmpFilePath);
	if ((fileDeleteErr != KErrNone) && (fileDeleteErr != KErrNotFound))
		{
		#ifdef _DEBUG
		RDebug::Print(_L("CHeapRepository::RestoreConsistencyL - Failed to delete file. Error = %d"), fileDeleteErr);
		#endif
		}

	CleanupStack::PopAndDestroy(persistsTmpFilePath);
	
	iInconsistentData=EFalse;
	}

/**
The method reloads the repository content from the related cre or ini file.
@leave System-wide error codes
*/	
void CSharedRepository::DoRestoreConsistencyL()
	{
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	//note that the function below already handles the deletion of any corrupt file
	//in  non-rom location
	TInt ret=TServerResources::iObserver->CreateRepositoryL(this,EPersists);
	if (ret==KErrNotFound)
		{
		ret=TServerResources::iObserver->CreateRepositoryL(this,EInstall);
		if (ret==KErrNotFound)
			{	
			ret=TServerResources::iObserver->CreateRepositoryL(this,ERom);
			User::LeaveIfError(ret);
			}
		}
#else
	TCentRepLocation location;
	
	TInt err = FindLocationForFileL(location,iSimRep->Uid(),ECre);
	if (err != KErrNotFound)
		{
		User::LeaveIfError(CreateRepositoryFromCreFileL(location));
		return;
		}
		
	User::LeaveIfError(FindLocationForFileL(location,iSimRep->Uid(),EIni));
	
	HBufC* fileName(NULL);
    TServerResources::CreateRepositoryFileNameLC(fileName,iSimRep->Uid(),location,EIni);	
 
	CIniFileIn* iniFile = NULL;
	err = CIniFileIn::NewLC(TServerResources::iFs,iniFile,*fileName);
	if (err==KErrCorrupt && location!=ERom)
		{
		User::LeaveIfError(TServerResources::iFs.Delete(*fileName));
		}
	User::LeaveIfError(err);
	

	err = ReloadContentL(*iniFile);
	User::LeaveIfError(err);
	
	CleanupStack::PopAndDestroy(iniFile); //iniFile 
	CleanupStack::PopAndDestroy(fileName);	//fileName
#endif
	}


/**
This method looks for and sets a location for a given repository.
It is based on EAuto mode thus it goes through all locations in the
same order (EPersists - EInstall - ERom)  

@param aLocation - returns a location for a repository
@param aUid - id of a repository which location should be found
@param aType - repository file type (.txt or .cre) 
@return KErrNone if aLocation succesfully set for a given repository,
		KErrNotFound if a repository was not found in any locations.

@internalTechnology
*/
#ifndef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
TInt CSharedRepository::FindLocationForFileL(TCentRepLocation& aLocation,TUid aUid,const TCentRepFileType aType) const
    { 		
	if(TServerResources::CentrepFileExistsL(aUid, EPersists, aType))
	    {
	     aLocation = EPersists;
		 return KErrNone;
	    }
	    
	if(TServerResources::CentrepFileExistsL(aUid, EInstall, aType))
		{
	     aLocation = EInstall;
		 return KErrNone;
	    }
	    
	if(TServerResources::CentrepFileExistsL(aUid, ERom, aType))
		{
		aLocation = ERom;
		return KErrNone;
		}

	return KErrNotFound;
    }
#endif

TInt CSharedRepository::CreateRepositoryFromCreFileL( TCentRepLocation aLocation)
	{
	// Get file path name from location
    HBufC* filePath(NULL);
    TServerResources::CreateRepositoryFileNameLC(filePath,iSimRep->Uid(), aLocation,ECre);
    // Trap errors from repository creation so we can delete corrupt repositories
	TRAPD(error, iSimRep->CreateRepositoryFromCreFileL(TServerResources::iFs,*filePath));
	if(error!=KErrNone && error!=KErrNotFound && error!=KErrNoMemory)
		{
		error=KErrCorrupt;
		// store wasn't quite what we were expecting - can't return an error, can't leave
		// so all we can do is close the file, tidy up as best we can, and return corrupt 
		if (aLocation != ERom)
			{
			// If a debug build - record error
			TInt fileDeleteErr=TServerResources::iFs.Delete(*filePath);
			if (fileDeleteErr != KErrNone)
				{
				#ifdef _DEBUG
				RDebug::Print(_L("CSharedRepository::CreateRepositoryFromCreFileL - Failed to delete file. Error = %d"), fileDeleteErr);
				#endif
				}

			}
		}
	else if( error==KErrNoMemory)
		{
		User::Leave(KErrNoMemory);
		}
	CleanupStack::PopAndDestroy(filePath);
	return error;
	}

/** Attempts to start a transaction.
Guaranteed to succeed (return KErrNone) for EConcurrentReadWriteTransaction mode only.
@param aTransactor transactor attempting to start transaction
@param aMode type of transaction to be started
@pre transactor is not in a transaction
@return KErrNone if the transaction is started, KErrLocked if read/write locks prevented that
type of transaction from starting now, and KErrArgument for invalid aMode.
@post On returning KErrNone, transaction is started and read/write locks are obtained for it
in the shared repository. Any other return: transaction has not started.
*/
TInt CSharedRepository::StartTransaction(CRepositoryTransactor& aTransactor, TInt aMode)
	{
	// session can only be in one transaction
	ASSERT(!aTransactor.IsInTransaction());
	
  	CObservable::TSharedRepositoryInfo* shrepinfo = TServerResources::iObserver->SharedRepositoryInfo(iSimRep->Uid());
  	ASSERT(shrepinfo);
	switch (aMode)
		{
		case EConcurrentReadWriteTransaction:
			// can always start this type of transaction
			shrepinfo->iNumActiveConcurrentReadWriteTransactions++;
			break;
		case EReadTransaction:
			// negative lock means there is an active EReadWriteTransaction
			if (shrepinfo->iPessimisticTransactionLockCount < 0)
				{
				ASSERT(shrepinfo->iPessimisticTransactionLockCount == -1); // sanity check
				return KErrLocked;
				}
			// when non-negative lock equals number of active EReadTransactions.
			shrepinfo->iPessimisticTransactionLockCount++;
			break;
		case EReadWriteTransaction:
			// lock is zero if there are no active pessimistic transactions
			if (shrepinfo->iPessimisticTransactionLockCount != 0)
				{
				return KErrLocked;
				}
			// lock value of -1 means the exclusive EReadWriteTransaction is active
			shrepinfo->iPessimisticTransactionLockCount = -1;
			break;
		default:
			// not a valid transaction mode
			return KErrArgument;
		}
	aTransactor.AddToQueue(shrepinfo->iTransactors, aMode);
	return KErrNone;
	}

/**	Commit transaction
@return KErrNone on success, or error code.
@param aKeyInfo 
	on success (return KErrNone): aKeyInfo returns number of modified settings;
	on failure (other error code): KUnspecifiedKey
@pre transactor is in a transaction.
@post transactor is not in a transaction
*/
TInt CSharedRepository::CommitTransaction(CRepositoryTransactor& aTransactor, TUint32& aKeyInfo)
	{
	// calling code should have panicked the client if not in a transaction
	ASSERT(aTransactor.IsInTransaction());
	TInt result = aTransactor.iTransactionResult;
	if (aTransactor.IsInFailedTransaction())
		{
		ASSERT(result != KErrNone);
		aKeyInfo = aTransactor.iTransactionErrorKey;
		}
	else
		{
		ASSERT(result == KErrNone);
		ASSERT(aTransactor.iTransactionErrorKey == KUnspecifiedKey);
		aKeyInfo = 0;
		// must release locks otherwise shared repository will not commit changes
		// failed transactions have already released their locks
		TServerResources::iObserver->ReleaseTransactionLock(aTransactor,iSimRep->Uid());
		}
	
	// transactions that haven't made any changes can be closed at any time
	if (aTransactor.IsInActiveReadWriteTransaction() &&
		(aTransactor.iTransactionSettings.Count() > 0))
		{
		result = DoCommitTransactionSettings(aTransactor, aKeyInfo);
		}

	// transaction is complete - remove from queue
  	CObservable::TSharedRepositoryInfo* shrepinfo = TServerResources::iObserver->SharedRepositoryInfo(iSimRep->Uid());
  	ASSERT(shrepinfo);
	shrepinfo->iTransactors.Remove(aTransactor);
	//Remove the link to the next transaction
	aTransactor.iLink.iNext = NULL;
	aTransactor.Deque();

	return result;
	}

TInt CSharedRepository::FailTransaction(CRepositoryTransactor& aTransactor, TInt aError, TUint32 aErrorKey)
	{
	ASSERT(aError != KErrNone); // must fail for a reason
	if (aTransactor.IsInActiveTransaction())
		{
		// locks cannot be removed from a failed transaction, so release before failing
		TServerResources::iObserver->ReleaseTransactionLock(aTransactor,iSimRep->Uid());
		aTransactor.SetFailed(aError, aErrorKey);
		}
	return aError; // to allow "return FailTransaction(error, errorKey);" - error written once
	}

/** Fails all active transactions - except for the optional aExcludeTransactor, releasing locks.
All transactions are failed with reason "KErrLocked" meaning they are "locked out".
This should only be done to allow another agent to change values in the repository.
Beware that all concurrent read/write transactions that are failed with KErrLocked are
expected to retry the transactions straight afterwards - must be careful to allow their
retry strategy to be successful.
*/
void CSharedRepository::FailAllTransactions(const CRepositoryTransactor* aExcludeTransactor)
	{
  	CObservable::TSharedRepositoryInfo* shrepinfo = TServerResources::iObserver->SharedRepositoryInfo(iSimRep->Uid());
  	ASSERT(shrepinfo);
	TSglQueIter<CRepositoryTransactor> transIter(shrepinfo->iTransactors);  	
	CRepositoryTransactor* transactor;
	while ((transactor = transIter++) != NULL)
		{
		if (transactor != aExcludeTransactor)
			{
			FailTransaction(*transactor, KErrLocked, KUnspecifiedKey);
			}
		}
	}

/** must currently be in active Read transaction. Does not fail
transaction here if promotion to read/write failed.
@return KErrNone if promoted, KErrLocked if not
*/
TInt CSharedRepository::AttemptPromoteTransactionToReadWrite(CRepositoryTransactor& aTransactor)
	{
	// transactor should currently be in an active read transaction
	ASSERT(aTransactor.IsInActiveReadTransaction());

  	CObservable::TSharedRepositoryInfo* shrepinfo = TServerResources::iObserver->SharedRepositoryInfo(iSimRep->Uid());
  	ASSERT(shrepinfo);
	// sanity check: must only be pessimistic reads active
	ASSERT(shrepinfo->iPessimisticTransactionLockCount > 0);
	// can promote only if there are no other active read transactions:
	if (1 == shrepinfo->iPessimisticTransactionLockCount)
		{
		// may only promote to exclusive read/write as it has the same commit semantics
		// as Read transaction: concurrent R/W must wait for reads to finish first.
		aTransactor.PromoteToExclusiveReadWrite();
		// lock value of -1 means the exclusive EReadWriteTransaction is active
		shrepinfo->iPessimisticTransactionLockCount = -1;
		return KErrNone;
		}
	return KErrLocked;
	}


	
void CSharedRepository::ExternalizeCre(RWriteStream& aStream) const
	{
	iSimRep->ExternalizeCre(TServerResources::iPersistsVersion,aStream);
	}

void CSharedRepository::InternalizeCreL(RReadStream& aStream)
	{
	iSimRep->InternalizeCreL(aStream);
	}
	
#ifdef	SYMBIAN_CENTREP_SUPPORT_MULTIROFS
void CSharedRepository::InternalizeCreL(RReadStream& aStream,TUint8& aCreVersion)
	{
	iSimRep->InternalizeCreL(aStream,aCreVersion);
	}
#endif	

void CSharedRepository::Notify(TUint32 aVal) const
	{
	if(iNotificationState)
		{
		TServerResources::iObserver->Notify(iSimRep->Uid(), aVal);
		}
	}

TBool CSharedRepository::IsTransactionActive()
	{
  	CObservable::TSharedRepositoryInfo* shrepinfo = TServerResources::iObserver->SharedRepositoryInfo(iSimRep->Uid());
  	if (shrepinfo)
  		{
		return (shrepinfo->iPessimisticTransactionLockCount != 0) ||
			(shrepinfo->iNumActiveConcurrentReadWriteTransactions > 0);
  		}
  	return EFalse;  		
	}

RSettingsArray& CSharedRepository::GetSettings()
	{
	return iSimRep->SettingsArray();
	}
