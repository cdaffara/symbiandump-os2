// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "logservsession.h"
#include "LogNotify.h"
#include "LogServServer.h"
#include "logservpanic.h"
#include "LogServView.h"
#include "LogServOperationBase.h"
#include "LogServBackupInterface.h"
#include "LogServOperationFactory.h"
#include "LogServOperationManager.h"
#include "LogServDatabaseChangeInterface.h"
#include "LogServDatabaseChangeDefinition.h"
#include "LogServSqlStrings.h"

// Constants
const TInt KLogViewListGranuality = 5;
const TInt KLogServPendingGlobalChangesArrayGranularity = 3;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServSession (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServSession::CLogServSession(TLogServSessionId aSessionId, 
                                 MLogServSessionLifetimeObserver& aObserver, 
                                 MLogServBackupInterface& aBackupInterface, 
                                 MLogServTaskInterface& aTaskInterface, 
                                 MLogServOperationManager& aOperationManager, 
                                 MLogServDatabaseChangeInterface& aChangeInterface, 
                                 MLogServDatabaseTransactionInterface& aDatabase) :
    iSessionId(aSessionId), 
    iObserver(aObserver), 
    iBackupInterface(aBackupInterface), 
    iTaskInterface(aTaskInterface), 
    iOperationManager(aOperationManager), 
    iChangeInterface(aChangeInterface), 
    iDatabase(aDatabase), 
    iViewList(KLogViewListGranuality), 
    iPendingGlobalChanges(KLogServPendingGlobalChangesArrayGranularity)
	{
	iObserver.SLOHandleEvent(Id(), MLogServSessionLifetimeObserver::ELogServSessionEventCreated); 
	}

CLogServSession::~CLogServSession()
	{
	LOGTEXT3("CLogServSession::~CLogServSession() - client logging off: %S, %d", &iClientThreadName, iSessionId);

	// Set second parameter to not complete the message, otherwise if the client has died a KERN-EXEC 44 will be generated.
	iOperationManager.OMCancel(iSessionId, EFalse);
	
	delete iPackage;
	delete iNotify;

	iChangeInterface.DCIRequestChangeNotificationsCancel(*this);
	//
	iViewList.ResetAndDestroy();
	iViewList.Close();
	iPendingGlobalChanges.Close();
	//
	iObserver.SLOHandleEvent(Id(), MLogServSessionLifetimeObserver::ELogServSessionEventDestroyed); 

	LOGTEXT2("CLogServSession::~CLogServSession() - client dead %d", iSessionId);
	LOGTEXT("");
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServSession::DCOHandleGlobalChangeEventL(const TLogServDatabaseChangeDefinition& aChange)
	{
	if	(iExtendedNotificationRequested)
		User::LeaveIfError(iPendingGlobalChanges.Append(aChange));
	ExtendedNotifyCompleteL(KErrNone);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
void CLogServSession::CreateL()
	{
	iPackage = CLogPackage::NewL();

	// Request change notifications
	iChangeInterface.DCIRequestChangeNotificationsL(*this);
	}

//
//  Handle a client request.
//
//  Leaving is handled by CLogServ::RunError() which reports the error code to the client
//  Note: operation add themselves to the server active queue in their constructor
//  The server active object owns them and deletes them when necessary
//  So the iffy looking code below won't actually leak memory
//
void CLogServSession::ServiceL(const RMessage2& aMessage)
	{
	LOGTEXT3("CLogServSession::ServiceL(%S, %d)", &iClientThreadName, aMessage.Function());

	switch(aMessage.Function())
		{
	/**
	 * Overall operation management
	 */
	case ELogOperationGetResult:
	case ELogOperationCancel:
	case ELogOperationInitiate:
		ServiceOperationFunctionL(aMessage);
		break;

	/**
	 * Notification related
	 */
	case ELogNotify:
		{
		if	(!iNotify)
			{
			iNotify = CLogNotify::NewL(iBackupInterface, iChangeInterface, Server().Priority());

		#ifdef LOGGING_ENABLED
			iNotify->SetClientThreadName(iClientThreadName);
		#endif
			}
		
		if	(!iNotify->IsActive())
			{
			const TTimeIntervalMicroSeconds32 delayTime = reinterpret_cast<TInt>(aMessage.Ptr0());
			
			//Validate time value
			if(delayTime.Int() < 0)
				{
				User::Leave(KErrArgument);
				}
			iNotify->Notify(delayTime, aMessage);
			}
		else
			::PanicClientL(aMessage, ELogAlreadyActive19);
		break;
		}
	case ELogNotifyCancel:
		{
		if	(iNotify)
			iNotify->Cancel();
		aMessage.Complete(KErrCancel);
		break;
		}
	case ELogNotifyExtended:
		{
		if	(iExtendedNotificationMessage == RMessage2())
			{
			iExtendedNotificationMessage = aMessage;
			iExtendedNotificationRequested = ETrue;
			}
		else
			::PanicClientL(aMessage, ELogExtendedMessageAlreadyActive);
		break;
		}
	case ELogNotifyExtendedCancel:
		{
        if	(iExtendedNotificationMessage != RMessage2())
			ExtendedNotifyCompleteL(KErrCancel);
		//
		iExtendedNotificationRequested = EFalse;
		iPendingGlobalChanges.Reset();
		iPendingGlobalChanges.GranularCompress();
		//
		aMessage.Complete(KErrCancel);
		break;
		}

	/**
	 * View functionality
	 */
	case ELogViewCreate:
	case ELogViewDelete:
	case ELogViewCount:
	case ELogViewOperationInitiate:
	case ELogViewChangeNotificationsRequest:
	case ELogViewChangeNotificationsCancel:
	case ELogViewFetchChanges:
	case ELogViewNotifyLockStatusChange:
	case ELogViewNotifyLockStatusChangeCancel:
		ServiceViewFunctionL(aMessage);
		break;

	/** 
	 * Misc. (debug) functionality
	 */
#ifdef _DEBUG
	case ELogMakeTransient:
		{
		Server().MakeTransient((TBool)aMessage.Ptr0());
		aMessage.Complete(KErrNone);
		break;
		}
	case ELogIsServerReady:
		{
		const TBool serverReady = (iBackupInterface.BIErrorValueForCurrentState() == KErrNone);
		aMessage.Complete(serverReady);
		break;
		}
	case ELogSetHeapFail:
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::TAllocFail(aMessage.Int0()),aMessage.Int1());
		aMessage.Complete(KErrNone);
		break;
		}
#endif// _DEBUG
	/**
	 * Unknown operations
	 */
	default:
		::PanicClientL(aMessage, ELogIllegalFunction);
		break;
		}
	}

/**
If aError is KErrBadDescriptor, then panic the client, else - default error handling.
KErrBadDescriptor error may be thrown from "message write" operations, if the client supplied a bad
descriptor to the server.
*/
void CLogServSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	if(aError == KErrBadDescriptor)
		{
		//The __LOGPANIC_CLIENT() macro cannot be used here because it calls a leaving function. A leaving call
		//from a leaving call will terminate the server.
		aMessage.Panic(KLogServ, ELogBadDescriptor);
		}
	CSession2::ServiceError(aMessage, aError);
	}
	
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

CLogServServer& CLogServSession::Server() const
	{
	return *static_cast<CLogServServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


CLogServViewBase& CLogServSession::ViewByIdL(TUint32 aViewId)
	{
	const TInt index = ViewPositionById(aViewId);
	User::LeaveIfError(index);
	return *iViewList[index];
	}

TInt CLogServSession::ViewPositionById(TUint32 aViewId) const
	{
	const TInt count = iViewList.Count();
	for(TInt i=0; i<count; i++)
		{
		const CLogServViewBase& view = *iViewList[i];
		if	(view.ViewId() == aViewId)
			return i;
		}
	return KErrNotFound;
	}


void CLogServSession::ReadClientServerDataL(TLogClientServerData& aClientServerData,
		const RMessage2 &aMessage, TInt aMinOperation, TInt aMaxOperation)
	{

	TInt length = aMessage.GetDesLengthL(0);
	if(length != sizeof(TLogClientServerData))
		{
		User::Leave(KErrBadDescriptor);
		}
	
	TPckg<TLogClientServerData> pData(aClientServerData);
	aMessage.ReadL(0, pData);
	
	//Validate Operation Type
	if((aClientServerData.iOperationType < aMinOperation) ||
			(aClientServerData.iOperationType > aMaxOperation))
		{
		User::Leave(KErrArgument);
		}
	}

void CLogServSession::ServiceViewFunctionL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
	case ELogViewCreate:
		{
		iViewList.ReserveL(iViewList.Count() + 1);
		// Type is first parameter, view id is second
		const TLogViewId id = static_cast<TLogViewId>(aMessage.Int0());
		const TLogViewType type = static_cast<TLogViewType>(aMessage.Int1());
		
		//Validate type value
		if((type < ELogViewTypeEvent)||(type > ELogViewTypeDuplicate))
			{
			User::Leave(KErrArgument);
			}
		
		// Ask the factory to create it
		CLogServViewBase* view = LogServFactory::NewViewL(type, id, iDatabase, iBackupInterface, *iPackage, aMessage);
		TInt err = iViewList.Append(view);
        __ASSERT_ALWAYS(err == KErrNone, Panic(ELogArrayReserved));
		aMessage.Complete(err);
		break;
		}
	case ELogViewDelete:
		{
		const TLogViewId id = static_cast<TLogViewId>(aMessage.Int0());
		TInt indexAndError = ViewPositionById(id);
		if	(indexAndError >= 0)
			{
			delete iViewList[indexAndError];
			iViewList.Remove(indexAndError);
			indexAndError = KErrNone;
			}
		aMessage.Complete(indexAndError);
		break;
		}
	case ELogViewCount:
		{
		const TLogViewId id = static_cast<TLogViewId>(aMessage.Int0());
		CLogServViewBase& view = ViewByIdL(id);
		const TInt count = view.Count();
		aMessage.Complete(count);
		break;
		}
	case ELogViewOperationInitiate:
		{
		// We don't allow any operations during a backup
		iBackupInterface.BIValidateStateForDatabaseOperationL();
		//
		TLogClientServerData clientServerData;
		ReadClientServerDataL(clientServerData,aMessage, 
				ELogOperationViewSetup, ELogOperationViewWindowFetch);

		//
		const TLogViewId id = static_cast<TLogViewId>(aMessage.Int1());
		CLogServViewBase& view = ViewByIdL(id);

		// Create operation. Operations are owned by the operation manager (they are added to a queue)
		// when the objects are created, so this does not leak any memory.
		CLogServOperationBase* operation = LogServFactory::NewViewOperationL(clientServerData, iTaskInterface, iOperationManager, aMessage, *iPackage, iSessionId, view);
		(void) operation;
		break;
		}
	case ELogViewChangeNotificationsRequest:
		{
		const TLogViewId id = static_cast<TLogViewId>(aMessage.Int0());
		CLogServViewBase& view = ViewByIdL(id);
		view.RequestChangeNotifications(aMessage);
		break;
		}
	case ELogViewChangeNotificationsCancel:
		{
		const TLogViewId id = static_cast<TLogViewId>(aMessage.Int0());
		CLogServViewBase& view = ViewByIdL(id);
		view.RequestChangeNotificationsCancel();
		aMessage.Complete(KErrNone);
		break;
		}
	case ELogViewFetchChanges:
		{
		const TLogViewId id = static_cast<TLogViewId>(aMessage.Int0());
		CLogServViewBase& view = ViewByIdL(id);
		view.RequestChangesL(aMessage);
		aMessage.Complete(KErrNone);
		break;
		}
	case ELogViewNotifyLockStatusChange:
		{
		const TLogViewId id = static_cast<TLogViewId>(aMessage.Int0());
		CLogServViewBase& view = ViewByIdL(id);
		view.RequestLockStatusChanges(aMessage);
		break;
		}
	case ELogViewNotifyLockStatusChangeCancel:
		{
		const TLogViewId id = static_cast<TLogViewId>(aMessage.Int0());
		CLogServViewBase& view = ViewByIdL(id);
		view.RequestLockStatusChangesCancel();
		aMessage.Complete(KErrNone);
		break;
		}
		}
	}

void CLogServSession::ServiceOperationFunctionL(const RMessage2& aMessage)
	{
	// We don't allow any operations during a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();
	//
	TLogClientServerData clientServerData;
	ReadClientServerDataL(clientServerData,aMessage, ELogOperationEventAdd,ELogOperationMaintain);
	//
	const TInt function = aMessage.Function();
	//
	const TLogOperationId& operationId = clientServerData.iOperationId;
	#ifdef LOGGING_ENABLED
	const TLogOperationType& operationType = clientServerData.iOperationType;
	#endif
	LOGTEXT4("CLogServSession::ServiceOperationFunctionL() - Operation function for client %S: Id: %d, Type: %d", &iClientThreadName, operationId, operationType);
	//
	switch(function)
		{
	case ELogOperationGetResult:
		LOGTEXT("CLogServSession::ServiceOperationFunctionL() - getting result");
		iOperationManager.OMGetResultL(operationId, iSessionId, aMessage);
		aMessage.Complete(KErrNone);
		break;
	case ELogOperationCancel:
		LOGTEXT("CLogServSession::ServiceOperationFunctionL() - cancelling");
		iOperationManager.OMCancel(operationId, iSessionId, ETrue);
		aMessage.Complete(KErrCancel);
		break;
	case ELogOperationInitiate:
		LOGTEXT("CLogServSession::ServiceOperationFunctionL() - initiating");
		// Create operation. Operations are owned by the operation manager (they are added to a queue)
		// when the objects are created, so this does not leak any memory.
		CLogServOperationBase* operation = LogServFactory::NewOperationL(clientServerData, iTaskInterface, iOperationManager, aMessage, *iPackage, iSessionId);
		(void) operation;
		break;
		}
	LOGTEXT("CLogServSession::ServiceOperationFunctionL() - end");
	}

void CLogServSession::ExtendedNotifyCompleteL(TInt aCompletionCode)
	{
	const TInt count = iPendingGlobalChanges.Count();
    if	(iExtendedNotificationMessage != RMessage2() && (count || aCompletionCode < KErrNone))
		{
		if	(aCompletionCode >= KErrNone && count)
			{
			const TLogServDatabaseChangeDefinition& change = iPendingGlobalChanges[0];
			//
			const TPckgBuf<TInt> pContext(change.iChangeType.iUid);
			const TPckg<TInt> pParam1(change.iChangeParam1);
			const TPckg<TInt> pParam2(change.iChangeParam2);
			const TPckg<TInt> pParam3(change.iChangeParam3);
			//
			iExtendedNotificationMessage.WriteL(0, pContext);
			iExtendedNotificationMessage.WriteL(1, pParam1);
			iExtendedNotificationMessage.WriteL(2, pParam2);
			iExtendedNotificationMessage.WriteL(3, pParam3);
			//
			iPendingGlobalChanges.Remove(0);
			}
		//
		iExtendedNotificationMessage.Complete(aCompletionCode);
		}
	}
