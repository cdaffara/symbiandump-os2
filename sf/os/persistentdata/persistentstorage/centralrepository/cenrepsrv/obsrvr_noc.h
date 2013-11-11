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
 
#ifndef OBSRVR_H
#define OBSRVR_H

#include <e32base.h>

class CSharedRepository;
class CServerRepository;
class CRepositoryTransactor;

#include "inifile.h"

#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
class CHeapRepository;
/**
This simple T class is used to store information regarding
the keyspace entry in the rofs list with respect to the
extension used and the mount id order
*/
class TRofsFlag
{
public:
	TRofsFlag(const TDesC& aExt,TUint8 aMountId):iFlag(aMountId)
		{
		if (aExt.Compare(_L("txt"))==0)
			{
			iFlag |=0x80000000;
			}
		}

	static TInt CompareFlag(const TRofsFlag& aFlag1, const TRofsFlag& aFlag2)
		{
		return (aFlag1.iFlag & 0xFF)-(aFlag2.iFlag & 0xFF);
		}

public:
	//the 16 bit flag in format of |8bit cre/txt flag|8bit mount_id|
	//obviously this is assuming the format of the mangled file
	//granulatiry currently set to default of 8 which is reasonable
	//MSB=1 indicates this is txt
	TUint32 iFlag;	
};

/**
This class is used to store information about potentially multiple
files(in diffeernt rofs) used for a centrep keyspace
*/
class TMultiRofsList
	{
public:
	TMultiRofsList(TUid aUid):iRepUid(aUid){}
	
public:
	//repository uid
	TUid iRepUid;
	//list of the mount ids together with the extension(cre/txt)
	RArray<TRofsFlag> iMountFlagList;		
	};
#endif	

class MObserver
	{
public:
	virtual void Notify(TUint32 aId) = 0;
	};

class CObservable : public CBase
	{
public:
	static CObservable* NewLC();
	
	~CObservable();

	void AddObserverL(TUid aUid, CServerRepository* aRepositoryPointer);
	void RemoveObserver(TUid aUid, CServerRepository* aRepositoryPointer, TInt aInMemoryIndex);

	void Notify(TUid aUid, TUint32 aVal) const;
	
	void ReleaseTransactionLock(CRepositoryTransactor& aTransactor,TUid aRepositoryUid);
	void CancelTransaction(CRepositoryTransactor& aTransactor,TUid aRepositoryUid);	
	TInt FindOpenRepository(TUid aUid) const;
	void RemoveOpenRepository(CSharedRepository* aRepository);
	
	inline CSharedRepository*& GetOpenRepository(TInt aIndex) 
		{
		ASSERT((aIndex>=0)&&(aIndex<iOpenRepositories.Count()));
		return iOpenRepositories[aIndex];
		}
	inline void AddOpenRepositoryL(CSharedRepository* aRepository)
		{
		iOpenRepositories.AppendL(aRepository);
		}
	
	TInt FindConnectedRepository(TUid aUid) const;
			
#ifdef CACHE_OOM_TESTABILITY
	void CloseiOpenRepositories();
	void CloseiRepositoryInfo();
	void Reset();
	TBool iTrapOOMOnOpen;
#endif	
	
	CSharedRepository* AccessL(TUid aUid, TBool aFailIfNotFound = ETrue);
	void LoadRepositoryLC(TUid aUid, TBool aFailIfNotFound, CSharedRepository*& aRepository, CIniFileIn::TIniFileOpenMode aIniFileOpenMode);

	/**
	@internalTechnology
	This is the class/structure which keeps shared non-persisted repository data 
	*/
	class TSharedRepositoryInfo
		{
	public:
		TSharedRepositoryInfo(TUid aUid);
		TUid iRepositoryUid;
		// all transactors in the order added
		TSglQue<CRepositoryTransactor> iTransactors;
		// combined read/write lock for pessimistic transactions.
		// -1 if in EReadWriteTransaction, otherwise non-negative number of EReadTransactions.
		TInt iPessimisticTransactionLockCount;
		// count of active concurrent read/write transactions:
		TInt iNumActiveConcurrentReadWriteTransactions;
		

		};

	TSharedRepositoryInfo* SharedRepositoryInfo(TUid aUid);		
	void AddSharedRepositoryInfoL(TUid aUid);
	void RemoveSharedRepositoryInfo(TUid aUid);
	TInt CreateRepositoryL(CSharedRepository* aRepository, TCentRepLocation aLocation);
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	void ProcessMultiRofsListL(const CDir& aList);	
	static TInt CompareUid(const TMultiRofsList& aUid1, const TMultiRofsList& aUid2);		
	RArray<TMultiRofsList>& GetMultiRofsList()
		{
		return iMultiRofsUidList;	
		}
#endif		
	
	/**
	@internalTechnology
	This is the class/structure which keeps notification-related data 
	*/
	class TRepositoryObserverInfo
		{
	public:
		TUid iRepositoryUid;
		CServerRepository* iRepositoryPointer; 
		};
	RArray<TRepositoryObserverInfo> iObservers;		
private:
	static TInt ObserverSortOrder(const TRepositoryObserverInfo &aRepository1, const TRepositoryObserverInfo &aRepository2);
	static TInt InfoSortOrder(const TSharedRepositoryInfo &aRepository1, const TSharedRepositoryInfo &aRepository2);
	static TInt CompareTUidValues(TInt aUid1, TInt aUid2);
	
	TInt ReadIniFileL(CSharedRepository*& aRepository,TCentRepLocation aLocation);
	TInt CreateRepositoryL(CSharedRepository*& aRepository, CIniFileIn::TIniFileOpenMode aIniFileOpenMode);
	TInt ReadSettingsL(CIniFileIn* aIniFileIn, CSharedRepository* aRep);

	TInt FindRepositoryInfo(TUid aUid);
	void RefreshTransactorAccessPolicies(CSharedRepository* aRepository, const TInt offset);
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	//Multi ROFS stuff
	void ConstructL();
	void OverrideSettingL(TServerSetting& aBaseSetting,const TServerSetting& aOvSetting,CSharedRepository* aCoreRepository,TBool aNewOv,TBool aNewOvIndivPolicy);	
	void MergeRepositoryL(CSharedRepository* aCoreRepository,CHeapRepository* aOverrideRepository);
	void MergeMultiRofsL(TBool aCoreInitialized,CSharedRepository* aCoreRepository,const RArray<TRofsFlag>& aOverridingFileList);
#endif
	
private:
	enum {KGranularity=8};	
	RPointerArray<CSharedRepository> iOpenRepositories;	
	RPointerArray<TSharedRepositoryInfo> iRepositoryInfo;
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	RArray<TMultiRofsList> iMultiRofsUidList;
#endif		
	};

#endif // OBSRVR_H
