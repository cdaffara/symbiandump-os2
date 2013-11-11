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

#include "clirep.h"
#include <e32math.h>
#include "srvparams.h"
#include "srvreqs.h"

using namespace NCentralRepositoryConstants;

RRepositorySession* CClientRepository::Session()
	{
	return static_cast<RRepositorySession*>(Dll::Tls());
	}
	
CClientRepository* CClientRepository::NewLC(TUid aRepositoryUid)
	{
	CClientRepository* rep = new(ELeave) CClientRepository();
	CleanupStack::PushL(rep);
	rep->ConstructL(aRepositoryUid);
	return rep;
	}

void CClientRepository::ConstructL(TUid aRepositoryUid)
	{
	RRepositorySession* session = Session();
	
	if(session == NULL)
		{
		session = new (ELeave) RRepositorySession();
		CleanupStack::PushL(session);
		User::LeaveIfError(Dll::SetTls(session));
		CleanupStack::Pop(session);
		User::LeaveIfError(session->Connect());
		}
	else
		{
		session->IncrementSubSessionCounter();
		}

	iSubSession = new (ELeave) RRepositorySubSession();
	User::LeaveIfError(iSubSession->Open(session, EInitialise, TIpcArgs(aRepositoryUid.iUid)));
	}

CClientRepository::CClientRepository()
	{
	}

CClientRepository::~CClientRepository()
	{
	if(iSubSession)
		{
		iSubSession->Close();
		delete iSubSession;
		}
	RRepositorySession* session = Session();
	if(session && session->DecrementSubSessionCounter() == 0)
		{
		//The last subSesssion is closed. Time to close the session.
		session->Close();
		delete session;
		Dll::FreeTls();
		//SetSession(NULL);
		}
	}

TInt CClientRepository::Create(TUint32 aId, TInt aVal)
	{
	return iSubSession->SendReceive(ECreateInt, TIpcArgs(aId, aVal));
	}

TInt CClientRepository::Create(TUint32 aId, const TReal& aVal)
	{
	TPckg<TReal> p(aVal);
	return iSubSession->SendReceive(ECreateReal, TIpcArgs(aId, &p));
	}
	
TInt CClientRepository::Create(TUint32 aId, const TDesC8& aVal)
	{
	return iSubSession->SendReceive(ECreateString, TIpcArgs(aId, &aVal));
	}

TInt CClientRepository::Create(TUint32 aId, const TDesC16& aVal)
	{
	TPtrC8 ptr8((const TUint8*)aVal.Ptr(), aVal.Size());
	return iSubSession->SendReceive(ECreateString, TIpcArgs(aId, &ptr8));
	}

TInt CClientRepository::Delete(TUint32 aId)
	{
	return iSubSession->SendReceive(EDelete, TIpcArgs(aId));
	}
	
TInt CClientRepository::Delete(TUint32 aPartialKey, TUint32 aMask, TUint32 &aErrorKey)
	{
	aErrorKey = KUnspecifiedKey; // set in case not filled by server
	TPckg<TUint32> p(aErrorKey);
	return iSubSession->SendReceive(EDeleteRange, TIpcArgs(aPartialKey, aMask, &p));
	}

TInt CClientRepository::Get(TUint32 aId, TInt& aVal)
	{
	TPckg<TInt> p(aVal);
	return iSubSession->SendReceive(EGetInt, TIpcArgs(aId, &p));
	}

TInt CClientRepository::Set(TUint32 aId, TInt aVal)
	{
	return iSubSession->SendReceive(ESetInt, TIpcArgs(aId, aVal));
	}

TInt CClientRepository::Get(TUint32 aId, TReal& aVal)
	{
	TPckg<TReal> p(aVal);
	return iSubSession->SendReceive(EGetReal, TIpcArgs(aId, &p));
	}

TInt CClientRepository::Set(TUint32 aId, const TReal& aVal)
	{
	TPckg<TReal> p(aVal);
	return iSubSession->SendReceive(ESetReal, TIpcArgs(aId, &p));
	}

TInt CClientRepository::Get(TUint32 aId, TDes8& aVal)
	{
	TPckg<TInt> p(aVal.MaxLength());
	return iSubSession->SendReceive(EGetString, TIpcArgs(aId, &aVal, &p));
	}

TInt CClientRepository::Get(TUint32 aId, TDes8& aVal, TInt& aActualLen)
	{
	aActualLen = aVal.MaxLength();
	TPckg<TInt> p(aActualLen);
	return iSubSession->SendReceive(EGetString, TIpcArgs(aId, &aVal, &p));
	}

TInt CClientRepository::Set(TUint32 aId, const TDesC8& aVal)
	{
	return iSubSession->SendReceive(ESetString, TIpcArgs(aId, &aVal));
	}

TInt CClientRepository::Get(TUint32 aId, TDes& aVal)
	{
	TPtr8 ptr8((TUint8*)aVal.Ptr(), 0, aVal.MaxSize());
	
	TPckg<TInt> p(ptr8.MaxLength());
	
	TInt r = iSubSession->SendReceive(EGetString, TIpcArgs(aId, &ptr8, &p));

	if(r==KErrNone || r==KErrOverflow)
		{
		TInt len = ptr8.Length();
		// note the following handles the case where client is getting an odd-length 8-bit
		// descriptor into 16-bit aVal. Round up length and ensure the extra byte is zero.
		if(len&1)
			{
			ptr8.SetLength(len+1);   // set the length before trying to write the value			
			ptr8[len] = 0;
			}
		aVal.SetLength((len + 1)/2);
		}

	return r;
	}
	
TInt CClientRepository::Get(TUint32 aId, TDes& aVal, TInt& aActualLen)
	{
	TPtr8 ptr8((TUint8*)aVal.Ptr(), 0, aVal.MaxSize());

	aActualLen = ptr8.MaxLength();
	TPckg<TInt> p(aActualLen);

	TInt r = iSubSession->SendReceive(EGetString, TIpcArgs(aId, &ptr8, &p));

	if(r==KErrNone || r==KErrOverflow)
		{
		TInt len = ptr8.Length();
		// note the following handles the case where client is getting an odd-length 8-bit
		// descriptor into 16-bit aVal. Round up length and ensure the extra byte is zero.
		if(len&1)
			{
			ptr8.SetLength(len+1);   // set the length before trying to write the value			
			ptr8[len] = 0;
			}
		aVal.SetLength((len + 1)/2);
		aActualLen = ((aActualLen + 1)/2);
		}

	return r;
	}

TInt CClientRepository::Set(TUint32 aId, const TDesC& aVal)
	{
	TPtrC8 ptr8((const TUint8*)aVal.Ptr(), aVal.Size());
	return iSubSession->SendReceive(ESetString, TIpcArgs(aId, &ptr8));
	}

TInt CClientRepository::GetMeta(TUint32 aId, TUint32& aMeta)
	{
	TPckg<TUint32> p(aMeta);
	return iSubSession->SendReceive(EGetMeta, TIpcArgs(aId, &p));
	}

TInt CClientRepository::Move(TUint32 aSourcePartialId, TUint32 aTargetPartialId,
                             TUint32 aIdMask, TUint32 &aErrorId)
	{
	aErrorId = KUnspecifiedKey; // set in case not filled by server
	TPckg<TUint32> p(aErrorId);
	TKeyFilter srcKeyIdentifier = {aSourcePartialId, aIdMask};
	TKeyFilter tgtKeyIdentifier = {aTargetPartialId, aIdMask};
	TPckg<TKeyFilter> pSrc(srcKeyIdentifier);
	TPckg<TKeyFilter> pTrg(tgtKeyIdentifier);
	
	TInt r = iSubSession->SendReceive(EMove, TIpcArgs(&pSrc, &pTrg, &p));
	
	return r;
	}
	
//Calls FailTransaction if it Leaves. This is the pattern for all client-side failure of
//operations valid in transactions.
TInt CClientRepository::FindL(TUint32 aPartialId, TUint32 aIdMask,
	RArray<TUint32>& aFoundIds)
	{
    CleanupFailTransactionPushL();
	aFoundIds.Reset();	
	
	TFixedArray<TUint32, KCentRepFindWithLenghtBufSize> uids;
	TUint32* start = uids.Begin();
	TPtr8 ptr(reinterpret_cast<TUint8*>(start), uids.Count() * uids.Length());
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);

	TInt r = iSubSession->SendReceive(EFind, TIpcArgs(&pIdentifier, 0, &ptr));

	if(r == KErrNone)
		{
		r = GetFindResult(uids, aFoundIds);
		if (r==KErrNoMemory)
			User::LeaveNoMemory();
		}
		
    CleanupStack::Pop();
	
	return r;
	}

//Calls FailTransaction if it Leaves. This is the pattern for all client-side failure of
//operations valid in transactions.
TInt CClientRepository::FindEqL(TUint32 aPartialId, TUint32 aIdMask, TInt aVal,
	RArray<TUint32>& aFoundIds)
	{
    CleanupFailTransactionPushL();
	aFoundIds.Reset();	
	
	TFixedArray<TUint32, KCentRepFindWithLenghtBufSize> uids;
	TUint32* start = uids.Begin();
	TPtr8 ptr(reinterpret_cast<TUint8*>(start), uids.Count() * uids.Length());
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);

	TInt r = iSubSession->SendReceive(EFindEqInt, TIpcArgs(&pIdentifier, aVal, &ptr));

	if(r == KErrNone)
		{
		r = GetFindResult(uids, aFoundIds);
		if (r==KErrNoMemory)
			User::LeaveNoMemory();
		}
		
    CleanupStack::Pop();
	
	return r;
	}

//Calls FailTransaction if it Leaves. This is the pattern for all client-side failure of
//operations valid in transactions.
TInt CClientRepository::FindEqL(TUint32 aPartialId, TUint32 aIdMask,
	const TReal& aVal, RArray<TUint32>& aFoundIds)
	{
    CleanupFailTransactionPushL();
	aFoundIds.Reset();	
	
	TPckg<TReal> pVal(aVal);
	TFixedArray<TUint32, KCentRepFindWithLenghtBufSize> uids;
	TUint32* start = uids.Begin();
	TPtr8 ptr(reinterpret_cast<TUint8*>(start), uids.Count() * uids.Length());
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);

	TInt r = iSubSession->SendReceive(EFindEqReal, TIpcArgs(&pIdentifier, &pVal, &ptr));

	if(r == KErrNone)
		{
		r = GetFindResult(uids, aFoundIds);
		if (r==KErrNoMemory)
			User::LeaveNoMemory();
		}
		
    CleanupStack::Pop();
	
	return r;
	}

//Calls FailTransaction if it Leaves. This is the pattern for all client-side failure of
//operations valid in transactions.
TInt CClientRepository::FindEqL(TUint32 aPartialId, TUint32 aIdMask,
	const TDesC8& aVal, RArray<TUint32>& aFoundIds)
	{
    CleanupFailTransactionPushL();
	aFoundIds.Reset();	
	
	TFixedArray<TUint32, KCentRepFindWithLenghtBufSize> uids;
	TUint32* start = uids.Begin();
	TPtr8 ptr(reinterpret_cast<TUint8*>(start), uids.Count() * uids.Length());
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);

	TInt r = iSubSession->SendReceive(EFindEqString, TIpcArgs(&pIdentifier, &aVal, &ptr));

	if(r == KErrNone)
		{
		r = GetFindResult(uids, aFoundIds);
		if (r==KErrNoMemory)
			User::LeaveNoMemory();
		}
		
    CleanupStack::Pop();
	
	return r;
	}

//Calls FailTransaction if it Leaves. This is the pattern for all client-side failure of
//operations valid in transactions.
TInt CClientRepository::FindEqL(TUint32 aPartialId, TUint32 aIdMask,
	const TDesC& aVal, RArray<TUint32>& aFoundIds)
	{
    CleanupFailTransactionPushL();
	aFoundIds.Reset();	
	
	TPtrC8 pVal((const TUint8*)aVal.Ptr(), aVal.Length()*2);
	TFixedArray<TUint32, KCentRepFindWithLenghtBufSize> uids;
	TUint32* start = uids.Begin();
	TPtr8 ptr(reinterpret_cast<TUint8*>(start), uids.Count() * uids.Length());
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);

	TInt r = iSubSession->SendReceive(EFindEqString, TIpcArgs(&pIdentifier, &pVal, &ptr));

	if(r == KErrNone)
		{
		r = GetFindResult(uids, aFoundIds);
		if (r==KErrNoMemory)
			User::LeaveNoMemory();
		}
		
    CleanupStack::Pop();
	
	return r;
	}

//Calls FailTransaction if it Leaves. This is the pattern for all client-side failure of
//operations valid in transactions.
TInt CClientRepository::FindNeqL(TUint32 aPartialId, TUint32 aIdMask,
	TInt aVal, RArray<TUint32>& aFoundIds)
	{
    CleanupFailTransactionPushL();
	aFoundIds.Reset();	
	
	TFixedArray<TUint32, KCentRepFindWithLenghtBufSize> uids;
	TUint32* start = uids.Begin();
	TPtr8 ptr(reinterpret_cast<TUint8*>(start), uids.Count() * uids.Length());
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);

	TInt r = iSubSession->SendReceive(EFindNeqInt, TIpcArgs(&pIdentifier, aVal, &ptr));

	if(r == KErrNone)
		{
		r = GetFindResult(uids, aFoundIds);
		if (r==KErrNoMemory)
			User::LeaveNoMemory();
		}
		
    CleanupStack::Pop();
	
	return r;
	}

//Calls FailTransaction if it Leaves. This is the pattern for all client-side failure of
//operations valid in transactions.
TInt CClientRepository::FindNeqL(TUint32 aPartialId, TUint32 aIdMask,
	const TReal& aVal, RArray<TUint32>& aFoundIds)
	{
    CleanupFailTransactionPushL();
	aFoundIds.Reset();	
	
	TPckg<TReal> pVal(aVal);
	TFixedArray<TUint32, KCentRepFindWithLenghtBufSize> uids;
	TUint32* start = uids.Begin();
	TPtr8 ptr(reinterpret_cast<TUint8*>(start), uids.Count() * uids.Length());
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);

	TInt r = iSubSession->SendReceive(EFindNeqReal, TIpcArgs(&pIdentifier, &pVal, &ptr));

	if(r == KErrNone)
		{
		r = GetFindResult(uids, aFoundIds);
		if (r==KErrNoMemory)
			User::LeaveNoMemory();
		}
		
    CleanupStack::Pop();
	
	return r;
	}

//Calls FailTransaction if it Leaves. This is the pattern for all client-side failure of
//operations valid in transactions.
TInt CClientRepository::FindNeqL(TUint32 aPartialId, TUint32 aIdMask,
	const TDesC8& aVal, RArray<TUint32>& aFoundIds)
	{
    CleanupFailTransactionPushL();
	aFoundIds.Reset();	
	
	TFixedArray<TUint32, KCentRepFindWithLenghtBufSize> uids;
	TUint32* start = uids.Begin();
	TPtr8 ptr(reinterpret_cast<TUint8*>(start), uids.Count() * uids.Length());
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);

	TInt r = iSubSession->SendReceive(EFindNeqString, TIpcArgs(&pIdentifier, &aVal, &ptr));

	if(r == KErrNone)
		{
		r = GetFindResult(uids, aFoundIds);
		if (r==KErrNoMemory)
			User::LeaveNoMemory();
		}
		
    CleanupStack::Pop();
	
	return r;
	}

//Calls FailTransaction if it Leaves. This is the pattern for all client-side failure of
//operations valid in transactions.
TInt CClientRepository::FindNeqL(TUint32 aPartialId, TUint32 aIdMask,
	const TDesC& aVal, RArray<TUint32>& aFoundIds)
	{
    CleanupFailTransactionPushL();
	aFoundIds.Reset();	
	
	TPtrC8 pVal((const TUint8*)aVal.Ptr(), aVal.Length()*2);
	TFixedArray<TUint32, KCentRepFindWithLenghtBufSize> uids;
	TUint32* start = uids.Begin();
	TPtr8 ptr(reinterpret_cast<TUint8*>(start), uids.Count() * uids.Length());
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);

	TInt r = iSubSession->SendReceive(EFindNeqString, TIpcArgs(&pIdentifier, &pVal, &ptr));

	if(r == KErrNone)
		{
		r = GetFindResult(uids, aFoundIds);
		if (r==KErrNoMemory)
			User::LeaveNoMemory();
		}
		
    CleanupStack::Pop();
	
	return r;
	}

/** Private helper function for all the Find~L functions.
No need to call FailTransaction since all the methods that call this method calls
FailTransaction prior to this method.
@internalComponent
*/
TInt CClientRepository::GetFindResult(const TFixedArray<TUint32, KCentRepFindWithLenghtBufSize>& aUids, RArray<TUint32>& aFoundIds)
	{
	iClientErr = KErrNone;	
	const TUint32 numFound = aUids[0];
	const TUint32 numInitial = numFound > KCentRepFindBufSize ? KCentRepFindBufSize : numFound;
	const TUint32 numFinal = numFound > KCentRepFindBufSize ? numFound - KCentRepFindBufSize : 0;
	
	for(TUint32 i = 1; i <= numInitial; i++)
		{
		//initialise client error first
		iClientErr=aFoundIds.Append(aUids[i]);
		if (iClientErr!=KErrNone)
			return iClientErr;		
		}
	
	if(numFinal)
		{
		TAny* tempBuf = User::Alloc(numFinal * sizeof(TUint32));
		if (tempBuf==NULL)
			{
			return KErrNoMemory;
			}
		TPtr8 p(static_cast<TUint8*>(tempBuf), numFinal * sizeof(TUint32));
		TInt r = iSubSession->SendReceive(EGetFindResult, TIpcArgs(&p));
		if (r == KErrNone)
			{ 
			for(TUint32 i = 0; i < numFinal; i++)
				{
				iClientErr=aFoundIds.Append(static_cast<const TUint32*>(tempBuf)[i]);
				if (iClientErr!=KErrNone)
					{
					User::Free(tempBuf);
					return iClientErr;
					}
				}
			}
		User::Free(tempBuf);
		}		
	return iClientErr;
	}

TInt CClientRepository::NotifyRequest(TUint32 aId, TRequestStatus& aStatus)
	{
	TInt r = iSubSession->SendReceive(ENotifyRequestCheck, TIpcArgs(aId));
	if(r==KErrNone)
		iSubSession->SendReceive(ENotifyRequest, TIpcArgs(aId), aStatus);
	return r;
	}

TInt CClientRepository::NotifyCancel(TUint32 aId)
	{
	return iSubSession->SendReceive(ENotifyCancel, TIpcArgs(aId));
	}

TInt CClientRepository::NotifyCancelAll()
	{
	return iSubSession->SendReceive(ENotifyCancelAll);
	}

TInt CClientRepository::NotifyRequest(TUint32 aPartialId, TUint32 aIdMask,
	TRequestStatus& aStatus)
	{
	iSubSession->SendReceive(EGroupNotifyRequest,
		TIpcArgs(aPartialId, aIdMask), aStatus);
	return KErrNone;
	}

TInt CClientRepository::NotifyCancel(TUint32 aPartialId, TUint32 aIdMask)
	{
	TKeyFilter keyIdentifier = {aPartialId, aIdMask};
	TPckg<TKeyFilter> pIdentifier(keyIdentifier);
	
	return iSubSession->SendReceive(EGroupNotifyCancel, TIpcArgs(&pIdentifier));
	}

TInt CClientRepository::Reset()
	{
	return iSubSession->SendReceive(EResetAll);
	}

TInt CClientRepository::Reset(TUint32 aId)
	{
	return iSubSession->SendReceive(EReset, TIpcArgs(aId));
	}

TInt CClientRepository::StartTransaction(TTransactionMode aMode)
	{
	return iSubSession->SendReceive(ETransactionStart, TIpcArgs(aMode));
	}

void CClientRepository::StartTransaction(TTransactionMode aMode, TRequestStatus& aStatus)
	{
	iSubSession->SendReceive(ETransactionStart, TIpcArgs(aMode), aStatus);
	}

TInt CClientRepository::CommitTransaction(TUint32& aKeyInfo)
	{
	// set to KUnspecifiedKey in case failure happens before setting in server
	aKeyInfo = KUnspecifiedKey;
	TPckg<TUint32> p(aKeyInfo);
	return iSubSession->SendReceive(ETransactionCommit, TIpcArgs(&p));
	}

void CClientRepository::CommitTransaction(TDes8& aKeyInfo, TRequestStatus& aStatus)
	{
	// set to KUnspecifiedKey in case failure happens before setting in server
	aKeyInfo.Copy(TPckg<TUint32>(KUnspecifiedKey));
	iSubSession->SendReceive(ETransactionCommit, TIpcArgs(&aKeyInfo), aStatus);
	}
	
void CClientRepository::CancelTransaction()
	{
    iSubSession->SendReceive(ETransactionCancel);
	}

static void CancelTransactionCleanupOperation(TAny* aRepository)
    {
    static_cast<CClientRepository*>(aRepository)->CancelTransaction();
    }

// So CancelTransaction is called in case of Leave. Must pop with CleanupStack::Pop() or similar
void CClientRepository::CleanupCancelTransactionPushL()
    {
    CleanupStack::PushL(TCleanupItem(CancelTransactionCleanupOperation, this));
    }
    
void CClientRepository::FailTransaction()
	{
	if (iClientErr==KErrNone)
		iSubSession->SendReceive(ETransactionFail,TIpcArgs(KErrAbort));
	else
		iSubSession->SendReceive(ETransactionFail,TIpcArgs(iClientErr));
	//reset the internal client code
	iClientErr=KErrNone;
	}
	
// So FailTransaction is called in case of Leave. Must pop with CleanupStack::Pop() or similar
static void FailTransactionCleanupOperation(TAny* aRepository)
    {
    static_cast<CClientRepository*>(aRepository)->FailTransaction();
    }
	
void CClientRepository::CleanupFailTransactionPushL()
	{
	CleanupStack::PushL(TCleanupItem(FailTransactionCleanupOperation, this));
	}

TInt CClientRepository::TransactionState()
	{
	TInt iValue;
	
	TPckg<TInt> p(iValue);
	
	iSubSession->SendReceive(ETransactionState, TIpcArgs(&p));
	
	return iValue;
	}

TInt RRepositorySubSession::Open(RRepositorySession* aSession,TInt aFunction,const TIpcArgs& aArgs)
	{
	iSession = aSession;
	return(CreateSubSession(*aSession, aFunction, aArgs));
	}
	
void RRepositorySubSession::Close()
	{
	RSubSessionBase::CloseSubSession(EClose);
	}

TInt RRepositorySubSession::SendReceive(TInt aFunction) const
	{
	return RSubSessionBase::SendReceive(aFunction);
	}
	
TInt RRepositorySubSession::SendReceive(TInt aFunction, const TIpcArgs& aArgs) const
	{
	return RSubSessionBase::SendReceive(aFunction, aArgs);
	}
	
void RRepositorySubSession::SendReceive(TInt aFunction, const TIpcArgs& aArgs, TRequestStatus& aStatus) const
	{
	RSubSessionBase::SendReceive(aFunction, aArgs, aStatus);
	}

inline TInt RRepositorySession::IncrementSubSessionCounter()
	{
	return ++iSubSessionCounter;
	}
	
RRepositorySession ::RRepositorySession()
	:iSubSessionCounter(1)
	{
	}
	
inline TInt RRepositorySession::DecrementSubSessionCounter()
	{
	ASSERT(iSubSessionCounter > 0);
	return --iSubSessionCounter;
	}

#if defined(__CENTREP_SERVER_PERFTEST__) || defined(__CENTREP_SERVER_MEMTEST__) || defined(__CENTREP_SERVER_CACHETEST__)
TInt RRepositorySession::SendReceive(TInt aFunction) const
	{
	return RSessionBase::SendReceive(aFunction);
	}

TInt RRepositorySession::SendReceive(TInt aFunction,
	const TIpcArgs& aArgs) const
	{
	return RSessionBase::SendReceive(aFunction, aArgs);
	}

void RRepositorySession::SendReceive(TInt aFunction, const TIpcArgs& aArgs,
	TRequestStatus& aStatus) const
	{
	RSessionBase::SendReceive(aFunction, aArgs, aStatus);
	}
#endif

LOCAL_C TInt StartServer();

TInt RRepositorySession::Connect()
	{
   	const TVersion KVersion(KServerMajorVersion, KServerMinorVersion,
   		KServerBuildVersion);
   	TInt retry = 2;
   	TInt err = KErrGeneral;
   	// Use unlimited message slots as we can call subscribe multiple times per
   	// session.
   	TInt numMessageSlots = -1; 
   	for(;;)
   		{
   		// Try to create a new session with the server.
   		err = CreateSession(KServerName, KVersion, numMessageSlots);
   		if((err != KErrNotFound) && (err != KErrServerTerminated))
   			break; //completed
   		// Server not running, try to start it.
   		if(--retry==0)
			break; // Failed.
   		err = StartServer();
   		if((err != KErrNone) && (err != KErrAlreadyExists))
			break;	// Launched server
   		}
   	return err;
	}

//
// Start the server process or thread
//
LOCAL_C TInt StartServer()
	{
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);
	//
	// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt r=server.Create(KServerImg,KNullDesC,serverUid);

	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

