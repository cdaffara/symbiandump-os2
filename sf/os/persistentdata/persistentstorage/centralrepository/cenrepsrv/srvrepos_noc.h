// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef SRVREPOS_H
#define SRVREPOS_H

#include <e32base.h>
#include <s32file.h>
#ifdef SYMBIAN_BAFL_SYSUTIL
#include <bafl/sysutil.h>
#endif
#include "shrepos.h"
#include "setting.h"
#include "clientrequest.h"
#include "sessnotf.h"
#include "operations.h"
#include "datatype.h"

class MObserver;
class CRestoredRepository;

class CServerRepository : public CRepositoryTransactor, public MOperationLogic
	{
public:
	void OpenL(TUid aUid, MObserver& aObserver, TBool aFailIfNotFound = ETrue);
	void HandleOpenMergeL();
	void Close();
	void RestoreNotify(const CRestoredRepository& aRstRepos);
	TInt RFSRepositoryL();
#ifdef SYMBIAN_BAFL_SYSUTIL
	static void CheckROMReflashL();
#endif
	static void RFSAllRepositoriesL();

	inline TInt StartTransaction(TInt aMode)
		{
		return iRepository->StartTransaction(*this, aMode);
		}
	inline TInt CommitTransaction(TUint32& aKeyInfo)
		{
		return iRepository->CommitTransaction(*this, aKeyInfo);
		}
	inline void CancelTransaction() // serves as both rollback and async cancel
		{
		TServerResources::iObserver->CancelTransaction(*this,iUid);
		}
	void CleanupCancelTransactionPushL();
	inline TInt FailTransaction(TInt aError, TUint32 aErrorKey)
		{
		// returns aError to allow "return FailTransaction(error, errorKey);" - error written once
		return iRepository->FailTransaction(*this, aError, aErrorKey);
		}

	inline void FailAllTransactions()
		{
		iRepository->FailAllTransactions(NULL);
		}
	/** must currently be in active Read transaction. Does not fail
	transaction here if promotion to read/write failed.
	@return KErrNone if promoted, KErrLocked if not
	*/
	inline TInt AttemptPromoteTransactionToReadWrite()
		{
		ASSERT(IsInActiveReadTransaction());
		return iRepository->AttemptPromoteTransactionToReadWrite(*this);
		}

	inline TServerSetting* GetPersistentSetting(TUint32 aId)
		{
		return iRepository->GetSettings().Find(aId);
		}
	inline TServerSetting* GetTransactionSetting(TUint32 aId)
		{
		return iTransactionSettings.Find(aId);
		}

	template <class T> 
	TInt TransactionCreateL(TUint32 aKey, const T& aVal, TUint32* aMeta);

	template <class T>
	TInt TransactionSetL(TUint32 aKey, const T& aVal);

	inline TInt TransactionDeleteL(TUint32 aId)
		{
		// all write operations now done in a transaction
		ASSERT(IsInActiveReadWriteTransaction());
		TRAPD(err,DeleteSettingL(aId));
		if (err==KErrNoMemory)
			User::LeaveNoMemory();
		return err;
		}
		
	TInt TransactionMoveL(const TClientRequest& aMessage, TUint32& aErrorKey);
	TInt TransactionDeleteRangeL(const TClientRequest& aMessage, TUint32& aErrId);

	TInt CheckPermissions(RSettingPointerArray& aSettings, const TClientRequest& aMessage, const char* aDiagnostic,TBool aReadPolicy,TUint32& aErrId);
	
	TInt CheckAccessPolicyBeforeMoving(const TClientRequest& aMessage, const TServerSetting* aSourceSetting, TUint32 aSourceKey, 
							const TServerSetting* aTargetSetting, TUint32 aTargetKey, TUint32& aErrorKey);
								
	TInt CheckMovePermissions(const RSettingPointerArray& aSourceSettings,const TClientRequest& aMessage,TUint aSourceToTarget,TUint32& aErrorKey)
		{
		TInt error=KErrNone;
		for (TInt i=0;i<aSourceSettings.Count() && error==KErrNone;i++)	
			{
			ASSERT(aSourceSettings[i]);
			TServerSetting* sourceSetting = aSourceSettings[i];
			TUint32 sourceKey = sourceSetting->Key();
			TUint32 targetKey = sourceKey ^ aSourceToTarget;
			TServerSetting* targetSetting = GetSetting(targetKey);
		
			error = CheckAccessPolicyBeforeMoving(aMessage, sourceSetting, sourceKey, targetSetting, targetKey, aErrorKey);		
			}
		return error;
		}

	inline TInt FindPersistentSettings(TUint32 aPartialId, TUint32 aIdMask, RSettingPointerArray& aMatches) const
		{
		// guarantees to reset RSettingPointerArray in case of error
		return iRepository->GetSettings().Find(aPartialId, aIdMask, aMatches);
		}
	inline TInt FindTransactionSettings(TUint32 aPartialId, TUint32 aIdMask, RSettingPointerArray& aMatches) const
		{
		// guarantees to reset RSettingPointerArray in case of error
		return iTransactionSettings.Find(aPartialId, aIdMask, aMatches);
		}

	inline void CommitChangesL(TCentRepLocation aLocation = EPersists) const
		{
		iRepository->CommitChangesL(aLocation);
		}

	TInt ResetL(TUint32 aId);
	TInt ResetAllL();

	inline const TSecurityPolicy& GetReadAccessPolicy(const TServerSetting& aSetting) const
		{
		return iRepository->GetReadAccessPolicy(aSetting);
		}
	inline const TSecurityPolicy& GetReadAccessPolicy(TUint32 aId) const
		{
		return iRepository->GetReadAccessPolicy(aId);
		}
	inline const TSecurityPolicy& GetDefaultReadAccessPolicy() const
		{
		return iRepository->GetDefaultReadAccessPolicy();
		}

	inline const TSecurityPolicy& GetFallbackWriteAccessPolicy(TUint32 aId) const
		{
		return iRepository->GetFallbackWriteAccessPolicy(aId);
		}

	inline const TSecurityPolicy& GetWriteAccessPolicy(const TServerSetting& aSetting) const
		{
		return iRepository->GetWriteAccessPolicy(aSetting);
		}
	inline const TSecurityPolicy& GetWriteAccessPolicy(TUint32 aId) const
		{
		return iRepository->GetWriteAccessPolicy(aId);
		}
	inline const TSecurityPolicy& GetDefaultWriteAccessPolicy() const
		{
		return iRepository->GetDefaultWriteAccessPolicy();
		}

	void HandleSWIUpdateL(TUid aUid, TTime aModified, CSessionNotifier& aNotifier);
	void HandleSWIDeleteL(TUid aUid, CSessionNotifier& aNotifier);

	void StoreRepositoryContentsL(CStreamStore & aStore, TStreamId & aSettingStreamId, TStreamId & aDeletedSettingsStreamId) const;
	void StoreRepositorySettingValuesL(CStreamStore& aStore, TStreamId & aSettingStreamId) const;	
	void RestoreRepositoryContentsL(CStreamStore& aStore, TStreamId aSettingStreamId, TStreamId aDeletedSettingsStreamId, CRestoredRepository& aRstRepos);
	void RestoreRepositorySettingValuesL(CStreamStore& aStore, TStreamId aSettingStreamId, CRestoredRepository& aRstRepos);

#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	void ResetFromIniFileL(TUint32 aId,CIniFileIn::TIniFileOpenMode aIniFileOpenMode,TBool& aKeyFound);
#else	
	void ResetFromIniFileL(TUint32 aId,TCentRepLocation aLocation,TBool& aKeyFound);
#endif	
	
	void LoadIniRepL(CIniFileIn::TIniFileOpenMode aMode);
	TBool GetMetaFromIni(TUint32 aKey, TUint32& aMeta);
	
	void RestoreInstallRepositoryL(TUid aUid, CStreamStore& aStore, TStreamId& aSettingStreamId, CRestoredRepository& aRstRepos); 
	void BackupInstallRepositoryL(TUid aUid, CStreamStore& aStore, TStreamId& aSettingStreamId); 


#ifdef CACHE_OOM_TESTABILITY
	TInt SizeiRepository()
		{
		return iRepository->Size();
		}
#endif	
	
	inline MObserver* Notifier()
		{
		ASSERT(iNotifier);
		return iNotifier;
		}
	
	inline void AccessRepositoryL()
		{
		iRepository = TServerResources::iObserver->AccessL(iUid);
		}

private:
	enum TPersistedRepActions {ECenRepReflash,ECenRepReset};
	
private:
	static void ProcessPersistsRepositoriesL(TPersistedRepActions aRomFlashOrReset);
	static void CacheRomVersionL(const TDesC& aFilename, TDesC8& aVersion);
	TInt HandleReflashofRepositoryL();

	//--------------------virtual functions from MOperation----------------------------
public:
	void GetSingleMeta(TUint aKey,TUint32& aMeta)
		{
		TUint32 metaFromIni;
		if (GetMetaFromIni(aKey, metaFromIni))
			{
			aMeta=~KMetaDefaultValue & metaFromIni;
			}
		else
			{
			//First check for a matching "range" default
			TSettingsDefaultMeta* defaultMeta = iRepository->iSimRep->RangeMetaArray().Find(aKey);
			if (defaultMeta)
				{
				aMeta=defaultMeta->GetDefaultMetadata();
				}
			else
				{
				// Range value not found, try for a repository default
				aMeta=iRepository->iSimRep->DefaultMeta();
				}			
			}		
		}
		
	TSettingsAccessPolicy* GetFallbackAccessPolicy(TUint32 aId) const
		{
		return iRepository->GetFallbackAccessPolicy(aId);		
		}	

	RSettingsArray& GetWritableSettingList()
		{
		return iTransactionSettings;
		}

	TServerSetting* GetSetting(TUint aKey)
		{
		// try to be most efficient when no transaction changes
		if ((iTransactionSettings.Count() > 0) && IsInActiveReadWriteTransaction())
			{
			TServerSetting* s = GetTransactionSetting(aKey);
			if (s)
				{
				return (s);
				}
			}
		return GetPersistentSetting(aKey);
		}

	/** Returns pointer array of settings whose keys match the partial key and mask. Combines
	settings from the persistent list with those in the transaction, with priority given to the
	latter, including settings flagged as deleted eliminating the corresponding entry from the 
	persistent settings (plus themselves so the final list has no settings flagged as deleted in it).
	Can also call this method when not in a transaction.
	In case of error, aMatches may contain entries and must be Reset.
	*/
	TInt FindSettings(TUint32 aSourcePartialKey,TUint32 aMask,RSettingPointerArray& aOutputArray) const
		{
		TInt error = FindPersistentSettings(aSourcePartialKey, aMask, aOutputArray);
		// try to be most efficient when no transaction changes
		if ((iTransactionSettings.Count() > 0) && IsInActiveReadWriteTransaction() && (KErrNone == error))
			{
			RSettingPointerArray transactionSettings;
			error = FindTransactionSettings(aSourcePartialKey, aMask, transactionSettings);
			if (error == KErrNone)
				{
				error = RSettingsArray::Merge(aOutputArray, transactionSettings);
				}
			transactionSettings.Reset();
			}
		return error;
		}
	
	//---------------------end of virtual functions-------------------------------------------	
#ifndef FOTA_UNIT_TESTING
private:
#endif
	CSharedRepository* iRepository;
	MObserver* iNotifier;	
	TUid iUid;
	};

template <class T>
TInt CServerRepository::TransactionSetL(TUint32 aKey, const T& aVal)
	{
	// all write operations now done in a transaction
	ASSERT(IsInActiveReadWriteTransaction());
	
	TRAPD(error,SetSettingL(aKey,aVal));
	if (error != KErrNone)
		{
		FailTransaction(error, aKey);
		}
	return error;
	}
	

template <class T>
TInt CServerRepository::TransactionCreateL(TUint32 aKey, const T& aVal, TUint32* aMeta)
	{
	// all write operations now done in a transaction
	ASSERT(IsInActiveReadWriteTransaction());
	
	TRAPD(err,CreateSettingL(aKey,aVal,aMeta));
	if (err==KErrAlreadyExists)
		{
		return FailTransaction(KErrAlreadyExists,aKey);
		}
	User::LeaveIfError(err);
	
	return KErrNone;
	}

#endif // SRVREPOS_H


