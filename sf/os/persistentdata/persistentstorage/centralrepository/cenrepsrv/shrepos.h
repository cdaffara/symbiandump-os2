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

#ifndef SHREPOS_H
#define SHREPOS_H

#include <e32base.h>
#include "obsrvr_noc.h"
#include "srvres.h"
#include "srvdefs.h"
#include "setting.h"
#include "clientrequest.h"
#include "panic.h"
#include "transstate.h"
#include "inifile.h"
#include "rstrepos.h"
#include "datatype.h"
#include "heaprepos.h"

class CRepositoryTransactor;

class CSharedRepository : public CBase
	{
	friend class CObservable;
	friend class CServerRepository;
	friend class TConvToolTester;

public:
	static CSharedRepository* NewL(TUid aUid);
	~CSharedRepository();

	TUid Uid() const;
	
	TInt CommitChanges(TCentRepLocation aLocation = EPersists);
	void CommitChangesL(TCentRepLocation aLocation = EPersists)
		{
		User::LeaveIfError(CommitChanges(aLocation));
		}

	void CreateL(TServerSetting& s, TSettingsAccessPolicy* &aPolicy, TBool aFirstLoad, TBool aSingleMetaFound=EFalse);
	
	void SetMetaDataOnRead(TServerSetting& aSetting, TBool aSingleMetaFound);
	
	void SetMetaDataOnCreate(TServerSetting& aNewSetting, TUint32* aMeta);

	TInt DeleteAndPersist(TUint32 aId);
	TInt DeleteNoPersist(TUint32 aId);

	TInt ResetNoPersistL(TServerSetting& aSetting);
	void ResetAndPersistL(TServerSetting& aSetting);
	TInt ResetAllNoPersistL(CSharedRepository& aRepository);
	
	const TSecurityPolicy& GetFallbackReadAccessPolicy(TUint32 aId);
	inline const TSecurityPolicy& GetReadAccessPolicy(const TServerSetting& aSetting);
	inline const TSecurityPolicy& GetReadAccessPolicy(TUint32 aId);
	inline const TSecurityPolicy& GetDefaultReadAccessPolicy();

	const TSecurityPolicy& GetFallbackWriteAccessPolicy(TUint32 aId);
	inline const TSecurityPolicy& GetWriteAccessPolicy(const TServerSetting& aSetting);
	inline const TSecurityPolicy& GetWriteAccessPolicy(TUint32 aId);
	inline const TSecurityPolicy& GetDefaultWriteAccessPolicy();

	TSettingsAccessPolicy* GetFallbackAccessPolicy(TUint32 aId
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,TBool aSkipSingle=EFalse
#endif	
	);
															
	TInt StartTransaction(CRepositoryTransactor& aTransactor, TInt aMode);
	TInt CommitTransaction(CRepositoryTransactor& aTransactor, TUint32& aKeyInfo);
	TInt FailTransaction(CRepositoryTransactor& aTransactor, TInt aError, TUint32 aErrorKey);
	void FailAllTransactions(const CRepositoryTransactor* aExcludeTransactor);
	TInt AttemptPromoteTransactionToReadWrite(CRepositoryTransactor& aTransactor);
	TBool IsTransactionActive();

	void	MergeL(CSharedRepository& aInstallRep, TMergeType aMergeType);
#ifndef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	TInt FindLocationForFileL(TCentRepLocation& aLocation,TUid aUid,const TCentRepFileType aType) const;
#endif	
	
	void	HandleUpdateMergeL(TTime aInstallFileTimeStamp, CSharedRepository& aInstallRep);
	void	HandleDeleteMergeL(CSharedRepository& aRomRep);
	void	SetInstallTime(TTime aInstallTime);

	inline TUid Owner() {return iSimRep->Owner(); }
	inline void WriteBackupStream(RWriteStream& aStream) const;
	inline void WriteDeletedSettingsStream(RWriteStream& aStream) const;
	inline void InternalizeL(RReadStream& aStream, CRestoredRepository& aRstRepos) ;
	void ExternalizeCre(RWriteStream& aStream) const;
	void InternalizeCreL(RReadStream& aStream) ;
#ifdef	SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	void InternalizeCreL(RReadStream& aStream,TUint8& aCreVersion);
#endif		
	TInt ReloadContentL(CIniFileIn& aIniFile, TBool aFirstLoad = EFalse);
#ifdef CENTREP_CONV_TOOL	
	void DoCommitChangesToIniFileL(const TDesC& aOutFileName
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,TUint32 aCreVersion
#endif	
	);
#endif	
	inline void SetSize(TInt aSize);
	inline TInt Size() const;

private:

	void	ResetContent();
	void	RestoreConsistencyL();
	void 	DoRestoreConsistencyL();
	TInt	CreateRepositoryFromCreFileL(TCentRepLocation aLocation);
	TInt	ReadSettingSavePolicyL(CIniFileIn& aFile,TServerSetting& aSetting, TSettingsAccessPolicy* &aPolicy, TBool& aSingleMetaFound);
	TInt DoCommitTransactionSettings(CRepositoryTransactor& aTransactor, TUint32& aKeyInfo);
	void Notify(TUint32 aVal) const;
	
	RSettingsArray& GetSettings();
	CSharedRepository();	
	void ConstructL(TUid aUid);

#ifdef FOTA_UNIT_TESTING	
public:
	void SetUid(TUid aUid)
		{
		iSimRep->SetUid(aUid);
		}
#endif
	CHeapRepository* iSimRep;

	TInt32 iSize; //approximate size(in memory) of the repository after internalisation
	TBool iNotificationState;//Notification can be made if iNotificationState is non-zero, 
	                         //otherwise the notification is disabled
	                         
	//iInconsistentData data flag will be set at the beginning of CommitChangesL()
	//and reset at the end of it. The flag is used to implement lazy-load approach for the
	//repository - if CommitChangesL() fails, the in-memory representation of the repository 
	//won't match the updated by CommitChangesL() repository (ini) file. The repository
	//consistency has to be restored before any next call of CSharedRepository methods.
	TBool iInconsistentData;	                         
	};

/**
@internalTechnology
Base class for CServerRepository and other objects that may be in transactions with the
shared repository. Contains a double queue link so the shared repository can build a queue
of them. In order to use transactions in the shared repository, the object needs to supply a
CRepositoryTransactor - either itself or a member variable. This makes it possible to
prevent one transactor from closing another's transaction.
Stores transaction state and changes made during the transaction.
Also stores RMessage2 while pending async start or commit, and handles its completion.
*/
class CRepositoryTransactor : public CBase
	{
	// shared repository may set private transaction state, but not derived classes
	friend class CSharedRepository;
	// for the _FOFF macro to work
	friend class CObservable::TSharedRepositoryInfo;
	friend class CObservable;
public:

	inline TInt TransactionState() const
		{
		return iTransactionState;
		}
	inline TInt TransactionMode() const
		{
		return iTransactionState & EAllTransactionModeBits;
		}
	inline TBool IsInTransaction() const
		{
		return iTransactionState != ENoTransaction;
		}
	inline TBool IsInActiveTransaction() const
		{
		// transaction is active if no bits apart from the (non-zero) mode are set
		return (iTransactionState != ENoTransaction) && (TransactionMode() == iTransactionState);
		}
	inline TBool IsInActiveReadTransaction() const
		{
		return iTransactionState == EReadTransaction;
		}
	inline TBool IsInActiveConcurrentReadWriteTransaction() const
		{
		return (iTransactionState == EConcurrentReadWriteTransaction);
		}
	inline TBool IsInActiveExclusiveReadWriteTransaction() const
		{
		return (iTransactionState == EReadWriteTransaction);
		}
	inline TBool IsInActiveReadWriteTransaction() const
		{
		return (iTransactionState == EReadWriteTransaction)
			|| (iTransactionState == EConcurrentReadWriteTransaction);
		}
	inline TBool IsInFailedTransaction() const
		{
		return (iTransactionState & EFailedBit) != 0;
		}

protected:
	inline void ClearSettings()
		{
		iTransactionSettings.Reset();
		}

private:
	inline void SetFailed(TInt aError, TUint32 aErrorKey)
		{
		// don't want to fail more than once
		ASSERT(IsInActiveTransaction());
		ASSERT(aError != KErrNone); // must fail for a reason
		iTransactionState |= EFailedBit;
		ClearSettings();
		iTransactionResult = aError;
		iTransactionErrorKey = aErrorKey;
		}
	inline void PromoteToExclusiveReadWrite()
		{
		ASSERT(iTransactionState == EReadTransaction);
		iTransactionState = EReadWriteTransaction;
		ClearSettings();
		}
	inline void AddToQueue(TSglQue<CRepositoryTransactor>& aTransactors, TInt aMode)
		{
		ASSERT(!IsInTransaction());
		// if the following ASSERT fails, transactor is already in a queue
		ASSERT(iLink.iNext == NULL);
		aTransactors.AddLast(*this);
		iTransactionState = aMode;
		// check adding to queue in an active state
		ASSERT(IsInActiveTransaction());
		ClearSettings();
		// clear failure reasons
		iTransactionResult = KErrNone;
		iTransactionErrorKey = KUnspecifiedKey;
		}
	inline void Deque()
		{
		iTransactionState = ENoTransaction;
		ClearSettings();
		delete iIniRep;
		iIniRep = NULL;
		}

protected:
	// derived classes are free to change transaction settings.
	// these can be made persistent by committing write transaction
	RSettingsArray iTransactionSettings;

	// 'Create' needs to check the default (INI) file to see if the
	// key position has a setting defined. If that is the case,
	// the new setting uses meta defined for the original setting
	// in the INI file.
	// Store data read from INI here so that we only read the INI
	// once per transaction.
	CSharedRepository* iIniRep;

private:
	// A queue link used by the CSharedRepository the CTransactor is working on
	TSglQueLink iLink;
	// integer encoding transaction state using definition of TTransactionState
	TInt iTransactionState;
	// result to be returned by commit if transaction failed earlier
	TInt iTransactionResult;
	// if transaction failed earlier, key or partial key involved in the original error,
	// or KUnspecifiedKey if could not be attributed to just one.
	TUint32 iTransactionErrorKey;
	};

#include "shrepos.inl"

#endif // SHREPOS_H
