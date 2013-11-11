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

#ifndef CLIREP_H
#define CLIREP_H

#include <centralrepository.h>
#include "centralrepositoryinternal.h"

const TInt32 KCentRepFindWithLenghtBufSize = NCentralRepositoryConstants::KCentRepFindBufSize + 1;

class RRepositorySubSession;
/** Client-side session.
@internalComponent
*/
class RRepositorySession : public RSessionBase
	{
public:
	RRepositorySession();
	TInt Connect();
	inline TInt IncrementSubSessionCounter();
	inline TInt DecrementSubSessionCounter();
	
#if defined(__CENTREP_SERVER_PERFTEST__) || defined(__CENTREP_SERVER_MEMTEST__) || defined(__CENTREP_SERVER_CACHETEST__)
	TInt SendReceive(TInt aFunction) const;
	TInt SendReceive(TInt aFunction, const TIpcArgs& aArgs) const;
	void SendReceive(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus) const;
#endif

private:
	TInt iSubSessionCounter;
	};

/** Client-side subSession.
@internalComponent
*/
class RRepositorySubSession : public RSubSessionBase
	{
public:
	TInt Open(RRepositorySession* aSession,TInt aFunction,const TIpcArgs& aArgs);
	void Close();
	TInt SendReceive(TInt aFunction) const;
	TInt SendReceive(TInt aFunction, const TIpcArgs& aArgs) const;
	void SendReceive(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus) const;
private:
	RRepositorySession* iSession;
	};


/** Implementation of CRepository.
@internalComponent
*/
NONSHARABLE_CLASS(CClientRepository) : public CRepository
	{
	friend class CRepository;
public:
	static CClientRepository* NewLC(TUid aRepositoryUid);

	~CClientRepository();

	TInt Create(TUint32 aId, TInt aVal);			// TInt
	TInt Create(TUint32 aId, const TReal& aVal);	// TReal
	TInt Create(TUint32 aId, const TDesC8& aVal);	// TDesC8
	TInt Create(TUint32 aId, const TDesC16& aVal);	// TDesC16

	TInt Delete(TUint32 aId);
	TInt Delete(TUint32 aPartialKey, TUint32 aMask, TUint32 &aErrorKey);

	TInt Get(TUint32 aId, TInt& aVal);
	TInt Set(TUint32 aId, TInt aVal);

	TInt Get(TUint32 aId, TReal& aVal);
	TInt Set(TUint32 aId, const TReal& aVal);

	TInt Get(TUint32 aId, TDes8& aVal);
	TInt Get(TUint32 aId, TDes8& aVal, TInt& aActualLen);
	TInt Set(TUint32 aId, const TDesC8& aVal);
	
	TInt Get(TUint32 aId, TDes& aVal);
	TInt Get(TUint32 aId, TDes& aVal, TInt& aActualLen);
	TInt Set(TUint32 aId, const TDesC& aVal);

	TInt GetMeta(TUint32 aId, TUint32& aMeta);

	TInt Move(TUint32 aSourcePartialId, TUint32 aTargetPartialId, TUint32 aIdMask, TUint32 &aErrorId) ;
	
	TInt FindL(TUint32 aPartialId, TUint32 aIdMask, RArray<TUint32>& aFoundIds);

	TInt FindEqL(TUint32 aPartialId, TUint32 aIdMask, TInt aVal, RArray<TUint32>& aFoundIds);
	TInt FindEqL(TUint32 aPartialId, TUint32 aIdMask, const TReal& aVal, RArray<TUint32>& aFoundIds);
	TInt FindEqL(TUint32 aPartialId, TUint32 aIdMask, const TDesC8& aVal, RArray<TUint32>& aFoundIds);
	TInt FindEqL(TUint32 aPartialId, TUint32 aIdMask, const TDesC& aVal, RArray<TUint32>& aFoundIds);

	TInt FindNeqL(TUint32 aPartialId, TUint32 aIdMask, TInt aVal, RArray<TUint32>& aFoundIds);
	TInt FindNeqL(TUint32 aPartialId, TUint32 aIdMask, const TReal& aVal, RArray<TUint32>& aFoundIds);
	TInt FindNeqL(TUint32 aPartialId, TUint32 aIdMask, const TDesC8& aVal, RArray<TUint32>& aFoundIds);
	TInt FindNeqL(TUint32 aPartialId, TUint32 aIdMask, const TDesC& aVal, RArray<TUint32>& aFoundIds);

	TInt NotifyRequest(TUint32 aId, TRequestStatus& aStatus);
	TInt NotifyRequest(TUint32 aPartialId, TUint32 aIdMask, TRequestStatus& aStatus);
	TInt NotifyCancel(TUint32 aId);
	TInt NotifyCancel(TUint32 aPartialId, TUint32 aIdMask);
	TInt NotifyCancelAll();

	TInt Reset();
	TInt Reset(TUint32 aId);

	TInt StartTransaction(TTransactionMode aMode);
	void StartTransaction(TTransactionMode aMode, TRequestStatus& aStatus);
	TInt CommitTransaction(TUint32& aKeyInfo);
	void CommitTransaction(TDes8& aKeyInfo, TRequestStatus& aStatus);
	void CancelTransaction();
	void CleanupCancelTransactionPushL();
	void FailTransaction();
	void CleanupFailTransactionPushL();
	TInt TransactionState();
	
private:
	CClientRepository();
	void ConstructL(TUid aRepositoryUid);
	TInt GetFindResult(const TFixedArray<TUint32, KCentRepFindWithLenghtBufSize>& aUids, RArray<TUint32>& aFoundIds);
	RRepositorySession* Session();
private:
	RRepositorySubSession* iSubSession;
	TInt iClientErr;
	};

#endif // CLIREP_H

