// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "srvreqs.h"
#include "backup.h"
#include "srvsubsess.h"
#include "log.h"
#include "centralrepositoryinternal.h"
#include <e32def_private.h>

using namespace NCentralRepositoryConstants;

CServerSubSession::CServerSubSession(CServerSession* aSession)
 : iSession(aSession), iInitialised(EFalse)
	{
	}

CServerSubSession::~CServerSubSession()
	{
#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE1("~CServerSubSession() UID: 0x%x\n",uid.iUid);
#endif

	iRepository.Close();
	iFindResult.Close();
	iInitialised = EFalse;
	}

// if session ServiceL Leaves, execution resumes in this method.
// this allows us to panic clients using bad descriptors, to deal with OOM conditions
// and to fail transactions with the correct reason: OOM etc.
void CServerSubSession::ServiceError(TInt aError)
	{
	// ensure any transaction is failed for the reason aError
	iRepository.FailTransaction(aError, KUnspecifiedKey);
	}

TInt CServerSubSession::ServiceL(const RMessage2& aMessage)
	{
	const TClientRequest msg(aMessage);

	struct SAction
		{
		TServerRequest req;
		TInt (CServerSubSession::*groupFuncL)(const TClientRequest&, TServerFunc);
		TServerFunc funcL;
		};

	static const SAction actionTable[] =
		{
			{EInitialise,				&CServerSubSession::GeneralOperationsL,	&CServerSubSession::InitialiseL},
			{ECreateInt,				&CServerSubSession::WriteOperationsL,	&CServerSubSession::CreateIntL},
			{ECreateReal,				&CServerSubSession::WriteOperationsL,	&CServerSubSession::CreateRealL},
			{ECreateString,				&CServerSubSession::WriteOperationsL,	&CServerSubSession::CreateStringL},
			{EDelete,					&CServerSubSession::WriteOperationsL,	&CServerSubSession::DeleteL},
			{EGetInt,					&CServerSubSession::ReadOperationsL,	&CServerSubSession::GetIntL},
			{ESetInt,					&CServerSubSession::WriteOperationsL,	&CServerSubSession::SetIntL},
			{EGetReal,					&CServerSubSession::ReadOperationsL,	&CServerSubSession::GetRealL},
			{ESetReal,					&CServerSubSession::WriteOperationsL,	&CServerSubSession::SetRealL},
			{EGetString,				&CServerSubSession::ReadOperationsL,	&CServerSubSession::GetStringL},
			{ESetString,				&CServerSubSession::WriteOperationsL,	&CServerSubSession::SetStringL},
			{EFind,						&CServerSubSession::ReadOperationsL,	&CServerSubSession::FindL},
			{EFindEqInt,				&CServerSubSession::ReadOperationsL,	&CServerSubSession::FindEqIntL},
			{EFindEqReal,				&CServerSubSession::ReadOperationsL,	&CServerSubSession::FindEqRealL},
			{EFindEqString,				&CServerSubSession::ReadOperationsL,	&CServerSubSession::FindEqStringL},
			{EFindNeqInt,				&CServerSubSession::ReadOperationsL,	&CServerSubSession::FindNeqIntL},
			{EFindNeqReal,				&CServerSubSession::ReadOperationsL,	&CServerSubSession::FindNeqRealL},
			{EFindNeqString,			&CServerSubSession::ReadOperationsL,	&CServerSubSession::FindNeqStringL},
			{EGetFindResult,			&CServerSubSession::ReadOperationsL,	&CServerSubSession::GetFindResultL},
			{ENotifyRequestCheck,		&CServerSubSession::GeneralOperationsL,	&CServerSubSession::NotifyRequestCheck},
			{ENotifyRequest,			&CServerSubSession::GeneralOperationsL,	&CServerSubSession::NotifyRequest},
			{ENotifyCancel,				&CServerSubSession::GeneralOperationsL,	&CServerSubSession::NotifyCancel},
			{ENotifyCancelAll,			&CServerSubSession::GeneralOperationsL,	&CServerSubSession::NotifyCancelAll},
			{EGroupNotifyRequest,		&CServerSubSession::GeneralOperationsL,	&CServerSubSession::GroupNotifyRequest},
			{EGroupNotifyCancel,		&CServerSubSession::GeneralOperationsL,	&CServerSubSession::GroupNotifyCancel},
			{EReset,					&CServerSubSession::ResetOperationsL,	&CServerSubSession::ResetL},
			{EResetAll,					&CServerSubSession::ResetOperationsL,	&CServerSubSession::ResetAllL},
			{ETransactionStart,			&CServerSubSession::GeneralOperationsL,	&CServerSubSession::TransactionStart},
  			{ETransactionCommit,		&CServerSubSession::GeneralOperationsL,	&CServerSubSession::TransactionCommitL},
  			{ETransactionCancel,		&CServerSubSession::GeneralOperationsL,	&CServerSubSession::TransactionCancel},
			{EMove,						&CServerSubSession::WriteOperationsL,	&CServerSubSession::MoveL},
			{ETransactionState,			&CServerSubSession::GeneralOperationsL,	&CServerSubSession::TransactionStateL},
			{ETransactionFail,			&CServerSubSession::GeneralOperationsL,	&CServerSubSession::TransactionFail},
			{EDeleteRange,				&CServerSubSession::WriteOperationsL,	&CServerSubSession::DeleteRangeL},
			{EGetMeta,					&CServerSubSession::ReadOperationsL,	&CServerSubSession::GetMeta}
		};

#ifdef SRVSUBSESS_TRACE
	const TPtrC actionTableNames[] =
		{
			_L("EInitialize"),
			_L("ECreateInt"),
			_L("ECreateReal"),
			_L("ECreateString"),
			_L("EDelete"),
			_L("EGetInt"),
			_L("ESetInt"),
			_L("EGetReal"),
			_L("ESetReal"),
			_L("EGetString"),
			_L("ESetString"),
			_L("EFind"),
			_L("EFindEqInt"),
			_L("EFindEqReal"),
			_L("EFindEqString"),
			_L("EFindNeqInt"),
			_L("EFindNeqReal"),
			_L("EFindNeqString"),
			_L("EGetFindResult"),
			_L("ENotifyRequestCheck"),
			_L("ENotifyRequest"),
			_L("ENotifyCancel"),
			_L("ENotifyCancelAll"),
			_L("EGroupNotifyRequest"),
			_L("EGroupNotifyCancel"),
			_L("EReset"),
			_L("EResetAll"),
			_L("ETransactionStart"),
	  		_L("ETransactionCommit"),
	  		_L("ETransactionCancel"),
			_L("EMove"),
			_L("ETransactionState"),
			_L("ETransactionFail"),
			_L("EDeleteRange"),
			_L("EGetMeta")
		};
#endif

	TInt r;
	TServerRequest fn = static_cast<TServerRequest>(aMessage.Function());

	__ASSERT_ALWAYS(iInitialised || fn==EInitialise, PanicClient(ESessionNotInitialised, msg));
	// In this assert we use (ELastInTable - 1) rather than ELastInTable because EClose is handled in the session
	// rather than the subsession, consiquently the actionTable array is one element shorter than ELastInTable
	__ASSERT_ALWAYS((fn < (ELastInTable)) && (fn >= EInitialise), PanicClient(EBadMessageNumber, msg));

	if (EInitialise != fn)
		{
		iRepository.AccessRepositoryL();
		}
#ifdef SRVSUBSESS_TRACE

	if (EInitialise != fn)
		{
		TUid uid = RepositoryUid();

		__SRVSUBSESS_TRACE2("CServerSubSession::ServiceL - UID: 0x%x %S\n",uid.iUid,&actionTableNames[fn]);
		}
	else
		{
		__SRVSUBSESS_TRACE1("CServerSubSession::ServiceL - UID: 0x?? %S\n",&actionTableNames[fn]);
		}

#endif
	// plus need to check we are initialised
	// this comment removes a false positive from the coverity output.  if fn >= ELastInTable then this code
	// will assert (see above).  but coverity doesn't consider this and therefore complains that there is a
	// posibility that actionTable could be indexed beyond it's length
	//coverity[overrun-local]
	r = (this->*actionTable[fn].groupFuncL)(aMessage, actionTable[fn].funcL);

	return r;
	}

//method allows transactions and notify requests to get through during backup/restore
//process to prevent them from being blocked.
//Transactions being blocked would lead clients to be panicked
//if they were trying to open a second transaction in the same session.
// Notify cancels must be allowed through because they must always succeed.
TInt CServerSubSession::GeneralOperationsL(const TClientRequest& aMessage, TServerFunc aFuncL)
	{
	ASSERT(aFuncL != NULL);
	return (this->*aFuncL)(aMessage);
	}

// method allows read operations to share transaction-related tasks.
// Read operations are allowed only during backup process.
// During restore it fails transactions with KErrLocked and returns KErrAbort
// and if it is a standalone read oparation it rejects it with KErrServerBusy.
TInt CServerSubSession::ReadOperationsL(const TClientRequest& aMessage, TServerFunc aFuncL)
	{
	TInt backupStatus = CRepositoryBackupClient::GetBackupStatus();

	if (iRepository.IsInFailedTransaction())
		{
		return KErrAbort;
		}
	else if (iRepository.IsInTransaction() && (backupStatus == ERestoreInProgress) )
	    {
	    iRepository.FailTransaction(KErrLocked,KUnspecifiedKey);
	    return KErrAbort;
	    }
	else if (backupStatus == ERestoreInProgress)
	    {
	    return KErrServerBusy;
	    }

	ASSERT(aFuncL != NULL);
	return (this->*aFuncL)(aMessage);
	}

// method allows write operations to share transaction-related tasks
// All write operations are not allowed either during backup or restore process.
// If backup or restore is in progress it fails transaction with KErrLocked,
// returns KErrAbort or if it is a standalone operation it returns KErrServerBusy.
TInt CServerSubSession::WriteOperationsL(const TClientRequest& aMessage, TServerFunc aFuncL)
	{
	TInt backupStatus = CRepositoryBackupClient::GetBackupStatus();

	if (iRepository.IsInFailedTransaction())
		{
		return KErrAbort;
		}
	else if (iRepository.IsInTransaction() && (backupStatus != ENoBackupActivty) )
	    {
	    iRepository.FailTransaction(KErrLocked,KUnspecifiedKey);
	    return KErrAbort;
	    }
    else if (backupStatus != ENoBackupActivty)
        {
        return KErrServerBusy;
        }

	// if not already in a transaction, create a temporary concurrent read/write transaction
	const TBool tempTransaction = !iRepository.IsInTransaction();
	if (tempTransaction)
		{
		// concurrent read/write transaction is guaranteed to start
		iRepository.StartTransaction(EConcurrentReadWriteTransaction);
		// to protect against Leaves:
		iRepository.CleanupCancelTransactionPushL();
		}
	else
		{
		if (iRepository.IsInActiveReadTransaction())
			{
			// must be a read/write transaction to continue
			iRepository.AttemptPromoteTransactionToReadWrite();
			// Note we don't check the return value of the above and return it here.
			// Instead we call the next level write function and expect it to have the
			// following code:
			//	if (iRepository.IsInActiveReadTransaction())
			//		{
			//		return iRepository.FailTransaction(KErrLocked, key);
			//		}
			// this ensures CommitTransaction reports the failing key.
			}
		// Note ServiceError will fail the transaction if write operation leaves
		}

	// call the server function
	ASSERT(aFuncL != NULL);
	TInt result = (this->*aFuncL)(aMessage);

	// commit the temporary transaction
	if (tempTransaction)
		{
		CleanupStack::Pop(); // remove cleanup item from earlier
		// absorb result and keyInfo from commit of temporary transaction
		TUint32 tempKeyInfo;
		User::LeaveIfError(iRepository.CommitTransaction(tempKeyInfo));
		}
	return result;
	}

TInt CServerSubSession::ResetOperationsL(const TClientRequest& aMessage, TServerFunc aFuncL)
	{
	// reset operations are not currently supported in transactions
	if (iRepository.IsInTransaction())
		{
		// fail transaction otherwise client may be misled to believe operation worked
		return iRepository.FailTransaction(KErrNotSupported, KUnspecifiedKey);
		}
   //can't reset when backup or restore is in progress
   else if (CRepositoryBackupClient::GetBackupStatus() != ENoBackupActivty)
        {
        return KErrServerBusy;
        }
	ASSERT(aFuncL != NULL);
	return (this->*aFuncL)(aMessage);
	}

TInt CServerSubSession::InitialiseL(const TClientRequest& aMessage)
	{
	__ASSERT_ALWAYS(!iInitialised,
		PanicClient(ESessionAlreadyInitialised, aMessage));
	// We let anyone to open a repository...
	// it's not considered a breach of security to let people know
	// that a repository is there...
	TUid uid = TUid::Uid(aMessage.Int0());

	__SRVSUBSESS_TRACE1("CServerSubSession::InitialiseL UID: 0x%x\n",uid.iUid);

	// Calls iObserver->AccessL internally
	iRepository.OpenL(uid, iNotifier);

	iInitialised = ETrue;

	return KErrNone;
	}

TInt CServerSubSession::CreateIntL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	// cannot make changes in a read transaction - upgrade must have failed due to write lock being used
	if (iRepository.IsInActiveReadTransaction())
		{
		return iRepository.FailTransaction(KErrLocked, key);
		}

	if(KErrNone !=  CheckPolicy(aMessage,iRepository.GetWriteAccessPolicy(key),
			__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSubSession::CreateIntL - Attempt made to create a setting")))
		return iRepository.FailTransaction(KErrPermissionDenied, key);

	TInt val = aMessage.Int1();

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE2("CServerSubSession::CreateIntL UID: 0x%x Key=0x%x\n",uid.iUid,key);
#endif

	TInt r = iRepository.TransactionCreateL(key, val, NULL);

	return r;
	}

TInt CServerSubSession::CreateRealL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	// cannot make changes in a read transaction - upgrade must have failed due to write lock being used
	if (iRepository.IsInActiveReadTransaction())
		{
		return iRepository.FailTransaction(KErrLocked, key);
		}

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetWriteAccessPolicy(key),
			__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::CreateRealL - Attempt made to create a setting")))
		return iRepository.FailTransaction(KErrPermissionDenied, key);

	TReal val;
	TPckg<TReal> p(val);
	aMessage.ReadL(1, p);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE2("CServerSubSession::CreateRealL UID: 0x%x Key=0x%x\n",uid.iUid,key);
#endif
	TInt r = iRepository.TransactionCreateL(key, val, NULL);

	return r;
	}

TInt CServerSubSession::CreateStringL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	// cannot make changes in a read transaction - upgrade must have failed due to write lock being used
	if (iRepository.IsInActiveReadTransaction())
		{
		return iRepository.FailTransaction(KErrLocked, key);
		}

	// sometime: must ensure bad descriptor results in client being panic'd

	// check for descriptor-too-long was previously on the client side,
	// hence test code expects KErrArgument response before KErrPermissionDenied
	TInt length = aMessage.GetDesLengthL(1);
	if (length > KMaxBinaryLength)
		{
		return iRepository.FailTransaction(KErrArgument, key);
		}

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetWriteAccessPolicy(key),
			__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::CreateStringL - Attempt made to create a setting")))
		return iRepository.FailTransaction(KErrPermissionDenied, key);

	TBuf8<KMaxBinaryLength> val;
	aMessage.ReadL(1, val);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE2("CServerSubSession::CreateStringL UID: 0x%x Key=0x%x\n",uid.iUid,key);
#endif

	TInt error = iRepository.TransactionCreateL(key, val, NULL);

	return error;
	}

TInt CServerSubSession::DeleteL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	// cannot make changes in a read transaction - upgrade must have failed due to write lock being used
	if (iRepository.IsInActiveReadTransaction())
		{
		return iRepository.FailTransaction(KErrLocked, key);
		}

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetWriteAccessPolicy(key),
			__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::DeleteL - Attempt made to delete a setting")))
		return iRepository.FailTransaction(KErrPermissionDenied, key);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE2("CServerSubSession::DeleteL UID: 0x%x Key=0x%x\n",uid.iUid,key);
#endif

	TInt r = iRepository.TransactionDeleteL(key);

	return r;
	}

TInt CServerSubSession::GetIntL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetReadAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::GetIntL - Attempt made to read a setting")))
		return KErrPermissionDenied;

	TInt val;
	TInt error = iRepository.Get(key, val);

	if (error == KErrNone)
		{
		TPckg<TInt> p(val);
		aMessage.WriteL(1, p);
		}

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::GetIntL UID: 0x%x Key=0x%x Value=%d\n",uid.iUid,key,val);
		}
	else
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::GetIntL **Failure** UID: 0x%x Key=0x%x\n",uid.iUid,key);
		}
#endif

	return error;
	}

TInt CServerSubSession::SetIntL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	// cannot make changes in a read transaction - upgrade must have failed due to write lock being used
	if (iRepository.IsInActiveReadTransaction())
		{
		return iRepository.FailTransaction(KErrLocked, key);
		}

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetWriteAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::SetIntL - Attempt made to write a setting")))
		return iRepository.FailTransaction(KErrPermissionDenied, key);

	TInt val = aMessage.Int1();
	TInt error = iRepository.TransactionSetL(key, val);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::SetIntL UID: 0x%x Key=0x%x Value=%d\n",uid.iUid,key,val);
		}
	else
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::SetIntL **Failure** UID: 0x%x Key=0x%x Value=%d\n",uid.iUid,key,val);
		}
#endif
	return error;
	}

TInt CServerSubSession::GetRealL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetReadAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::GetRealL - Attempt made to read a setting")))
		return KErrPermissionDenied;

	TReal val;
	TInt error = iRepository.Get(key, val);

	if(error==KErrNone)
		{
		TPckg<TReal> p(val);
		aMessage.WriteL(1, p);
		}

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::GetRealL UID: 0x%x Key=0x%x Value=%d\n",uid.iUid,key,val);
		}
	else
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::GetRealL **Failure** UID: 0x%x Key=0x%x\n",uid.iUid,key);
		}
#endif
	return error;
	}

TInt CServerSubSession::SetRealL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	// cannot make changes in a read transaction - upgrade must have failed due to write lock being used
	if (iRepository.IsInActiveReadTransaction())
		{
		return iRepository.FailTransaction(KErrLocked, key);
		}

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetWriteAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::SetRealL - Attempt made to write a setting")))
		return iRepository.FailTransaction(KErrPermissionDenied, key);

	TReal val;
	TPckg<TReal> p(val);
	aMessage.ReadL(1, p);

	TInt error = iRepository.TransactionSetL(key, val);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::SetRealL UID: 0x%x Key=0x%x Value=%d\n",uid.iUid,key,val);
		}
	else
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::SetRealL **Failure** UID: 0x%x Key=0x%x Value=%d\n",uid.iUid,key,val);
		}
#endif

	return error;
	}

TInt CServerSubSession::GetStringL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetReadAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::GetStringL - Attempt made to read a setting")))
		return KErrPermissionDenied;

	TBuf8<KMaxBinaryLength> val;

	TInt error = iRepository.Get(key, val);

	if(error==KErrNone)
		{
		TInt clientMaxDescriptorLength;
		TPckg<TInt> pInt (clientMaxDescriptorLength);
		aMessage.Read(2, pInt);

		TInt descriptorLength = val.Length();

		// write the descriptor length to aMessage
		TPckg<TInt> p(descriptorLength);
		error = aMessage.Write(2, p);

		if(error==KErrNone)
			{
			if(descriptorLength > clientMaxDescriptorLength)
				{
				// if it is, fill the descriptor up to its max length
				error = aMessage.Write(1, val.Left(clientMaxDescriptorLength));

				// if client-side descriptor is too small to take the value, which it is, returns KErrOverflow
				if(error == KErrNone)
					{
					error = KErrOverflow;
					}
				}
			else
				{
				error = aMessage.Write(1, val);
				}

#ifdef SRVSUBSESS_TRACE
			TUid uid = RepositoryUid();

			if (error == KErrNone)
				{
				__SRVSUBSESS_TRACE3("CServerSubSession::GetStringL UID: 0x%x Key=0x%x Value=%S\n",uid.iUid,key,&val);
				}
			else
				{
				__SRVSUBSESS_TRACE2("CServerSubSession::GetStringL **Failure** UID: 0x%x Key=0x%x\n",uid.iUid,key);
				}
#endif
			}

		// if error is KErrOverflow should not failing transaction
		if ((error != KErrNone) && (error != KErrOverflow))
			{
			// ServiceError will fail transaction
			User::Leave(error);
			}
		}
	return error;
	}

TInt CServerSubSession::SetStringL(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	// cannot make changes in a read transaction - upgrade must have failed due to write lock being used
	if (iRepository.IsInActiveReadTransaction())
		{
		return iRepository.FailTransaction(KErrLocked, key);
		}

	// check for descriptor-too-long was previously on the client side,
	// hence test code expects KErrArgument response before KErrPermissionDenied
	TInt length = aMessage.GetDesLengthL(1);
	if (length > KMaxBinaryLength)
		{
		return iRepository.FailTransaction(KErrArgument, key);
		}

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetWriteAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::SetStringL - Attempt made to write a setting")))
		return iRepository.FailTransaction(KErrPermissionDenied, key);

	TBuf8<KMaxBinaryLength> val;
	aMessage.ReadL(1, val);  // no error if too long, truncated instead

	TInt error = iRepository.TransactionSetL(key, val);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::SetStringL UID: 0x%x Key=0x%x Value=%S\n",uid.iUid,key,&val);
		}
	else
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::SetStringL **Failure** UID: 0x%x Key=0x%x\n",uid.iUid,key);
		}
#endif

	return error;
	}

TInt CServerSubSession::GetMeta(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

	if(KErrNone != CheckPolicy(aMessage,iRepository.GetReadAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::GetMeta - Attempt made to read a setting")))
		return KErrPermissionDenied;

	TUint32 meta;
	TInt error = iRepository.GetMeta(key, meta);

	if (error == KErrNone)
		{
		TPckg<TUint32> p(meta);
		error = aMessage.Write(1, p);
		}

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::GetMeta UID: 0x%x Key=0x%x Meta=0x%x\n",uid.iUid,key,meta);
		}
	else
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::GetMeta **Failure** UID: 0x%x Key=0x%x\n",uid.iUid,key);
		}
#endif

	return error;
	}

TInt CServerSubSession::FindL(const TClientRequest& aMessage)
	{
	// PLATSEC NOTE:
	// There are no read policy checks on FindL.
	// Client is returned the full list of keys whether they have read permission or not.
	// They were able to find this out by brute force using FindL on single setting ranges
	// anyway (because it would return KErrPermissionDenied if it existed, KErrNotFound otherwise).
	// Revealing the existence of settings is not considered a breach of security.
	TKeyFilter keyIdentifier;
	TPckg<TKeyFilter> p(keyIdentifier);
	aMessage.Read(0, p);
	// we reset results first since we do not police GetResult
	// and this way we can guarantee no results are available if access not granted
	iFindResult.Reset();
	
	RArray<TUint32> settingsToSend;
	CleanupClosePushL(settingsToSend);

	TInt error=KErrNone;
	TRAP(error,iRepository.FindL(keyIdentifier.iPartialId, keyIdentifier.iIdMask,settingsToSend,KCentRepFindBufSize,iFindResult));	
	if (error==KErrNone)
		{
		//write back the total number of settingsFound;
		TInt numSettings=settingsToSend.Count()+iFindResult.Count();
		TPtrC8 count(reinterpret_cast<TUint8*>(&numSettings),sizeof(TUint32));
		error=aMessage.Write(2,count);
		if (error==KErrNone)
			{
			TPtrC8 p(reinterpret_cast<TUint8*>(&(settingsToSend[0])), (settingsToSend.Count())*sizeof(TUint32));
			error=aMessage.Write(2,p,4);
			}
	
#ifdef SRVSUBSESS_TRACE
		TUid uid = RepositoryUid();
		if (error == KErrNone)
			{
			__SRVSUBSESS_TRACE4("CServerSubSession::FindL UID: 0x%x Key=0x%x Mask=0x%x Value=%S\n",uid.iUid,keyIdentifier.iPartialId,
					keyIdentifier.iIdMask,&p);
			}
		else
			{
			__SRVSUBSESS_TRACE4("CServerSubSession::FindL (failed write) UID: 0x%x Key=0x%x Mask=0x%x Value=%S\n",uid.iUid,keyIdentifier.iPartialId,
					keyIdentifier.iIdMask,&p);			
			}
#endif
		}
	CleanupStack::PopAndDestroy(); //settingsToSend
	
	if (error != KErrNone)
		{
		iFindResult.Reset();
		if ((error != KErrNone) && (error != KErrNotFound))
			{
			// ServiceError will fail transaction
			User::Leave(error);
			}
		}
	return error;
	}

TInt CServerSubSession::FindEqIntL(const TClientRequest& aMessage)
	{
	TInt val = aMessage.Int1();
	// PlatSec check done in FindValueL
	return FindValueL(aMessage, val);
	}

TInt CServerSubSession::FindEqRealL(const TClientRequest& aMessage)
	{
	TReal val;
	TPckg<TReal> p(val);
	aMessage.Read(1, p);
	// PlatSec check done in FindValueL
	return FindValueL(aMessage, val);
	}

TInt CServerSubSession::FindEqStringL(const TClientRequest& aMessage)
	{
	TBuf8<KMaxBinaryLength> val;
	aMessage.ReadL(1, val);  // no error if too long, truncated instead
	// PlatSec check done in FindValueL
	return FindValueL(aMessage, val);
	}

TInt CServerSubSession::FindNeqIntL(const TClientRequest& aMessage)
	{
	TInt val = aMessage.Int1();
	// PlatSec check done in FindValueL
	return FindValueL(aMessage, val, ENotEqual);
	}

TInt CServerSubSession::FindNeqRealL(const TClientRequest& aMessage)
	{
	TReal val;
	TPckg<TReal> p(val);
	aMessage.Read(1, p);
	// PlatSec check done in FindValueL
	return FindValueL(aMessage, val, ENotEqual);
	}

TInt CServerSubSession::FindNeqStringL(const TClientRequest& aMessage)
	{
	TBuf8<KMaxBinaryLength> val;
	aMessage.ReadL(1, val);  // no error if too long, truncated instead
	// PlatSec check done in FindValueL
	return FindValueL(aMessage, val, ENotEqual);
	}

template <class T>
TInt CServerSubSession::FindValueL(const TClientRequest& aMessage, const T& aVal,TComparison aComparison)
	{
	// IMPORTANT PLATSEC NOTE:
	// MUST return KErrPermissionDenied if read policy of ANY setting in the search range not passed.
	// MUST NOT merely check read policy of matching entries, otherwise it is possible to determine
	// secret values by brute force: Using single-value ranges, cycling through the possible values and
	// confirming a match when it returns KErrPermissionDenied rather than KErrNotFound.
	TKeyFilter keyIdentifier;
	TPckg<TKeyFilter> p(keyIdentifier);
	aMessage.Read(0, p);
	// we reset results first since we do not police GetResult
	// and this way we can guarantee no results are available if access not granted
	iFindResult.Reset();
	RSettingPointerArray settings;
	CleanupClosePushL(settings);
	TInt error = iRepository.FindSettings(keyIdentifier.iPartialId, keyIdentifier.iIdMask, settings);
	if (error == KErrNone)
		{
		//perform the read checking policies first
		TUint32 dummyErrId;
		error=iRepository.CheckPermissions(settings,aMessage,
		__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::FindValueL - Attempt made to search settings"),ETrue,dummyErrId);
		if (error==KErrPermissionDenied)
			iFindResult.Reset();

		//now if everything passes, do perform the FindValue
		if (error==KErrNone)
			{
			TRAP(error,iRepository.FindCompareL(settings,aVal,aComparison,iFindResult));
			if (error==KErrNone)
				{
				const TInt numSettings = iFindResult.Count();
				const TInt numInitial = numSettings > KCentRepFindBufSize ? KCentRepFindBufSize : numSettings;
				RArray<TUint32> settingsToSend;
				CleanupClosePushL(settingsToSend);
				
				//reserve memory for everything that needs to be added to the array
				settingsToSend.ReserveL(numInitial + 1); // the plus one is for the numSettings value
				
				//first append the number of found settings
				settingsToSend.AppendL(numSettings);

				//now append up to KCentRepFindBufSize settings
				for(TInt i = 0; i < numInitial; i++)
					{
					settingsToSend.AppendL(iFindResult[0]);
					iFindResult.Remove(0);
					}
					
				if(iFindResult.Count() == 0)
					{
					iFindResult.Reset();
					}

				//send:
				//1 - the count of total settings found
				//2 - the settings that fit in the buffer allocated for the first IPC 
				TPtrC8 p(reinterpret_cast<TUint8*>(&(settingsToSend[0])), (numInitial+1)*sizeof(TUint32));
				error = aMessage.Write(2, p);

	#ifdef SRVSUBSESS_TRACE
				TUid uid = RepositoryUid();
				
				__SRVSUBSESS_TRACE4("CServerSubSession::FindValueL UID: 0x%x Key=0x%x Mask=0x%x Value=%S\n",uid.iUid,keyIdentifier.iPartialId, 
						keyIdentifier.iIdMask,&p);
	#endif
				CleanupStack::PopAndDestroy(); //settingsToSend		
				}
			}
		}
	if ((error != KErrNone) && (error != KErrNotFound) && (error != KErrPermissionDenied))
		{
		iFindResult.Reset();
		CleanupStack::PopAndDestroy(); //settings

		// ServiceError will fail transaction
		User::Leave(error);
		}
#ifdef SRVSUBSESS_TRACE
	else
		{
		TUid uid = RepositoryUid();

		__SRVSUBSESS_TRACE3("CServerSubSession::FindValueL **Failure** UID: 0x%x Key=0x%x Mask=0x%x\n",uid.iUid,keyIdentifier.iPartialId,
				keyIdentifier.iIdMask);
		}
#endif

	CleanupStack::PopAndDestroy(); //settings
	return error;
	}

TInt CServerSubSession::GetFindResultL(const TClientRequest& aMessage)
	{
	TInt n = iFindResult.Count();	
	if (n==0)
		{
		return KErrNotFound;
		}

  	TPtrC8 p(reinterpret_cast<TUint8*>(&(iFindResult[0])), n*sizeof(TUint32));
  	TInt error = aMessage.Write(0, p);
  	// Free up iFindResult - it's no longer needed
  	iFindResult.Reset();
	// ServiceError will fail transaction

#ifdef SRVSUBSESS_TRACE
  	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::GetFindResultL UID: 0x%x Value=%S\n",uid.iUid,&p);
		}
	else
		{
		__SRVSUBSESS_TRACE1("CServerSubSession::GetFindResultL **Failure** UID: 0x%x\n",uid.iUid);
		}
#endif
	return User::LeaveIfError(error);
	}

TInt CServerSubSession::NotifyRequestCheck(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();
	if(KErrNone != CheckPolicy(aMessage,iRepository.GetReadAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::NotifyRequestCheck - Attempt made to check Notify request")))
		return KErrPermissionDenied;

	TInt error = iRepository.GetPersistentSetting(key) ? KErrNone : KErrNotFound;

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::NotifyRequestCheck UID: 0x%x Key=0x%x\n",uid.iUid,key);
		}
	else
		{
		__SRVSUBSESS_TRACE1("CServerSubSession::NotifyRequestCheck **Failure** UID: 0x%x\n",uid.iUid);
		}
#endif
	return error;
	}

TInt CServerSubSession::NotifyRequest(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();
	if(KErrNone != CheckPolicy(aMessage,iRepository.GetReadAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::NotifyRequest - Attempt made to register for Notify")))
		return KErrPermissionDenied;
	TInt error = iNotifier.AddRequest(key, aMessage);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::NotifyRequest UID: 0x%x Key=0x%x\n",uid.iUid,key);
		}
	else
		{
		__SRVSUBSESS_TRACE1("CServerSubSession::NotifyRequest **Failure** UID: 0x%x\n",uid.iUid);
		}
#endif

	return (error == KErrNone) ? KDontCompleteMessage : error;
	}

TInt CServerSubSession::NotifyCancel(const TClientRequest& aMessage)
	{
	TUint32 key = aMessage.Int0();

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE2("CServerSubSession::NotifyCancel UID: 0x%x Key=0x%x\n",uid.iUid,key);
#endif

	return iNotifier.CancelRequest(key);
	}

TInt CServerSubSession::NotifyCancelAll(const TClientRequest& /*aMessage*/)
	{
#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE1("CServerSubSession::NotifyCancelAll UID: 0x%x\n",uid.iUid);
#endif
	return iNotifier.CancelAllRequests();
	}

TInt CServerSubSession::GroupNotifyRequest(const TClientRequest& aMessage)
	{
	TUint32 partialId = aMessage.Int0();
	TUint32 idMask = aMessage.Int1();
	RSettingPointerArray settings;
	TInt error = iRepository.FindPersistentSettings(partialId, idMask, settings);
	if (error == KErrNone)
		{
		TUint32 dummyErrId;
		error = iRepository.CheckPermissions(settings, aMessage,
			__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::GroupNotifyRequest - Attempt made to register for group Notify"),ETrue,dummyErrId);
		}
	settings.Reset();
	if (error != KErrNone)
		{
#ifdef SRVSUBSESS_TRACE
		TUid uid = RepositoryUid();

		__SRVSUBSESS_TRACE3("CServerSubSession::GroupNotifyRequest **Failure** UID: 0x%x\n PartialId=0x%x Mask=0x%x\n",
				uid.iUid,partialId,idMask);
#endif

		return KErrPermissionDenied;
		}

	error = iNotifier.AddRequest(partialId, idMask, aMessage);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::GroupNotifyRequest UID: 0x%x PartialId=0x%x Mask=0x%x\n",
				uid.iUid,partialId,idMask);
		}
	else
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::GroupNotifyRequest **Failure** UID: 0x%x PartialId=0x%x Mask=0x%x\n",uid.iUid,
				partialId,idMask);
		}
#endif

	return error==KErrNone ? KDontCompleteMessage : error;
	}

TInt CServerSubSession::GroupNotifyCancel(const TClientRequest& aMessage)
	{
	TKeyFilter keyIdentifier;
	TPckg<TKeyFilter> p(keyIdentifier);
	aMessage.Read(0, p);

	RSettingPointerArray settings;
	TInt error = iRepository.FindPersistentSettings(keyIdentifier.iPartialId, keyIdentifier.iIdMask, settings);
	settings.Reset();
	if (error != KErrNone)
		{
#ifdef SRVSUBSESS_TRACE
		TUid uid = RepositoryUid();

		__SRVSUBSESS_TRACE3("CServerSubSession::GroupNotifyCancel **Failure** UID: 0x%x\n PartialId=0x%x Mask=0x%x\n",
				uid.iUid,keyIdentifier.iPartialId, keyIdentifier.iIdMask);
#endif
		return error;
		}

	error = iNotifier.CancelRequest(keyIdentifier.iPartialId, keyIdentifier.iIdMask);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::GroupNotifyCancel UID: 0x%x PartialId=0x%x Mask=0x%x\n",
				uid.iUid,keyIdentifier.iPartialId, keyIdentifier.iIdMask);
		}
	else
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::GroupNotifyCancel **Failure** UID: 0x%x PartialId=0x%x Mask=0x%x\n",uid.iUid,
				keyIdentifier.iPartialId, keyIdentifier.iIdMask);
		}
#endif


	return error;
	}

TInt CServerSubSession::ResetL(const TClientRequest& aMessage)
	{
	// individual setting reset is not yet supported in transactions
	ASSERT(!iRepository.IsInTransaction());
	TUint32 key = aMessage.Int0();
	if(KErrNone != CheckPolicy(aMessage,iRepository.GetWriteAccessPolicy(key),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::ResetL - Attempt made to reset a setting")))
		return iRepository.FailTransaction(KErrPermissionDenied, key);

	TInt error = iRepository.ResetL(key);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::ResetL UID: 0x%x Key=0x%x\n",
				uid.iUid,key);
		}
	else
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::ResetL **Failure** UID: 0x%x Key=0x%x\n",uid.iUid,key);
		}
#endif

	return error;
	}

TInt CServerSubSession::ResetAllL(const TClientRequest& aMessage)
	{
	// factory reset operation is not yet supported in transactions
	ASSERT(!iRepository.IsInTransaction());
	if(KErrNone != CheckPolicy(aMessage,iRepository.GetDefaultWriteAccessPolicy(),
				__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerSession::ResetAllL - Attempt made to reset repository")))
		return KErrPermissionDenied;

	iNotifier.IdReportingOff();
	TInt error = iRepository.ResetAllL();
	iNotifier.IdReportingOn();

	if (error == KErrNone)
		{
		iRepository.CommitChangesL();  // temporary measure
		}

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE1("CServerSubSession::ResetAllL UID: 0x%x\n",uid.iUid);
		}
	else
		{
		__SRVSUBSESS_TRACE1("CServerSubSession::ResetAllL **Failure** UID: 0x%x\n",uid.iUid);
		}
#endif

	return error;
	}

TInt CServerSubSession::MoveL(const TClientRequest& aMessage)
	{
	if (iRepository.IsInActiveReadTransaction())
		{
		TKeyFilter keyIdentifier;
		TPckg<TKeyFilter> p(keyIdentifier);
		aMessage.Read(0, p);

#ifdef SRVSUBSESS_TRACE
		TUid uid = RepositoryUid();

		__SRVSUBSESS_TRACE3("CServerSubSession::MoveL **Failure** In Transaction UID: 0x%x PartialId=0x%x Mask=0x%x\n",
				uid.iUid,keyIdentifier.iPartialId,keyIdentifier.iIdMask);
#endif

		// could not promote to read/write: fail & give the source partialKey as the error key
		return iRepository.FailTransaction(KErrLocked, keyIdentifier.iPartialId);
		}
	
	TUint32 errorKey = KUnspecifiedKey;
	TInt result = iRepository.TransactionMoveL(aMessage, errorKey);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (result == KErrNone)
		{
		__SRVSUBSESS_TRACE1("CServerSubSession::MoveL UID: 0x%x\n",uid.iUid);
		}
	else
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::MoveL **Failure** UID: 0x%x Error=%d\n",uid.iUid,errorKey);
		}
#endif

	if (result != KErrNone && result!=KErrNotFound)
		{
		iRepository.FailTransaction(result, errorKey);
		TPckg<TUint32> p(errorKey);
		aMessage.WriteL(2, p);
		}
	return result;
	}

TInt CServerSubSession::TransactionStart(const TClientRequest& aMessage)
	{
	// check if we are already in a transaction
	TBool inTransactionAlready = iRepository.IsInTransaction();

	// panic client if attempting to start a transaction when already in one
	__ASSERT_ALWAYS(!inTransactionAlready, PanicClient(EStartAlreadyInTransaction, aMessage));

	// if the client has been panicked then we don't want to continue.
	// client session will already have been taken down so don't want to complete the message
	if (inTransactionAlready)
		{
#ifdef SRVSUBSESS_TRACE
		TUid uid = RepositoryUid();

		__SRVSUBSESS_TRACE1("CServerSubSession::TransactionStart **Failure** already in transaction UID=0x%x\n",uid.iUid);
#endif

		return KDontCompleteMessage;
		}

	const TInt mode = aMessage.Int0();

	TInt error = iRepository.StartTransaction(mode);

#ifdef SRVSUBSESS_TRACE

	TUid uid = RepositoryUid();

	// mode strings match transstate.h - if that is updated, this should be as well.
	const TPtrC modeStrings[] =
		{
			_L("ENoTransaction"),                              // 0
			_L("EReadTransaction"),                            // EReadBit     1
			_L("EConcurrentReadWriteTransaction"),             // EWriteBit    2
			_L("EReadWriteTransaction|EAllTransactionBits"),   //EReadBit | EWriteBit 3
			_L("EFailedBit")                                   // 4
		};

	if (error == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::TransactionStart UID: 0x%x Mode=%d (%S)\n",uid.iUid,mode,&modeStrings[mode]);
		}
	else
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::TransactionStart **Failure** UID: 0x%x Mode=%d (%S)\n",uid.iUid,mode,&modeStrings[mode]);
		}
#endif

	return error;
	}

// serves as both rollback and async cancel
TInt CServerSubSession::TransactionCancel(const TClientRequest& /*aMessage*/)
	{
#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE1("CServerSubSession::TransactionCancel UID: 0x%x",uid.iUid);
#endif
	iRepository.CancelTransaction();
	return KErrNone;
	}

TInt CServerSubSession::TransactionCommitL(const TClientRequest& aMessage)
	{
	// check if we are in a transaction
	TBool inTransaction = iRepository.IsInTransaction();

	// panic client if attempting to commit a transaction when we are not in one
	__ASSERT_ALWAYS(inTransaction, PanicClient(ECommitNotInTransaction, aMessage));

	// if the client has been panicked then we don't want to continue.
	// client session will already have been taken down so don't want to complete the message
	if (!inTransaction)
		{
#ifdef SRVSUBSESS_TRACE
		TUid uid = RepositoryUid();

		__SRVSUBSESS_TRACE1("CServerSubSession::TransactionCommitL **Failure** In Transaction UID: 0x%x TransactionKey=%d\n",uid.iUid);
#endif
		return KDontCompleteMessage;
		}

	TUint32 keyInfo = KUnspecifiedKey;
	TInt result = iRepository.CommitTransaction(keyInfo);

	TPckg<TUint32> p(keyInfo);
	aMessage.WriteL(0, p);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	if (result == KErrNone)
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::TransactionCommitL UID: 0x%x TransactionKey=%d\n",uid.iUid,keyInfo);
		}
	else
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::TransactionCommitL **Failure** UID: 0x%x TransactionKey=%d\n",uid.iUid,keyInfo);
		}
#endif
	return result;
	}

TInt CServerSubSession::DeleteRangeL(const TClientRequest& aMessage)
	{
	if (iRepository.IsInActiveReadTransaction())
		{
		// could not promote to read/write: fail & give the partialKey as the error key
		TUint32 partialKey = aMessage.Int0();
		return iRepository.FailTransaction(KErrLocked, partialKey);
		}
	TUint32 errorKey = KUnspecifiedKey;
	TInt result = iRepository.TransactionDeleteRangeL(aMessage, errorKey);
	if (result != KErrNone)
		{
		TPckg<TUint32> p(errorKey);
		aMessage.WriteL(2, p);
		}

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();
	TUint32 partialKey = aMessage.Int0();
	TUint32 keyMask    = aMessage.Int1();

	if (result == KErrNone)
		{
		__SRVSUBSESS_TRACE3("CServerSubSession::DeleteRangeL UID: 0x%x Key=0x%x Mask=0x%x\n",uid.iUid,partialKey,keyMask);
		}
	else
		{
		__SRVSUBSESS_TRACE2("CServerSubSession::DeleteRangeL **Failure** UID: 0x%x ErrorKey=%d\n",uid.iUid,errorKey);
		}
#endif

	return result;
	}

TInt CServerSubSession::TransactionStateL(const TClientRequest& aMessage)
	{
	TInt iState = iRepository.TransactionState();

	TPckg<TInt> p(iState);
	aMessage.WriteL(0, p);

#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE2("CServerSubSession::TransactionStateL UID: 0x%x State=%d\n",uid.iUid,iState);
#endif

	return KErrNone;
	}

TInt CServerSubSession::TransactionFail(const TClientRequest& aMessage)
	{
#ifdef SRVSUBSESS_TRACE
	TUid uid = RepositoryUid();

	__SRVSUBSESS_TRACE1("CServerSubSession::TransactionFail UID: 0x%x",uid.iUid);
#endif

	iRepository.FailTransaction(aMessage.Int0(), KUnspecifiedKey);
	return KErrNone;
	}

//Check the security policy against a RMessage.
TInt CServerSubSession::CheckPolicy(const TClientRequest& msg,
								 const TSecurityPolicy& aPolicy,
								 const char *aDiagnostic)
	{
	return msg.CheckPolicy(aPolicy,aDiagnostic) ? KErrNone : KErrPermissionDenied;
	}
