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

#ifndef __LOGSERVSESSION_H__
#define __LOGSERVSESSION_H__

#include <logcli.h>
#include "LogServDefs.h"
#include "LogServDatabaseChangeObserver.h"

// Classes referenced
class TLogClientServerData;
class MLogServSessionLifetimeObserver;
class MLogServTaskInterface;
class MLogServOperationManager;
class MLogServBackupInterface;
class MLogServDatabaseChangeInterface;
class MLogServDatabaseTransactionInterface;
class CLogServServer;
class CLogNotify;
class CLogServViewBase;

/**
Handles client requests such as "add event", "change event", etc.  
and dispatches them for execution to the hitters.
"Hitter" is an active object which class type is one of (CLogActive derived class): 
 - CLogAddEvent;
 - CLogGetEvent;
 - CLogChangeEvent;
 - CLogDeleteEvent;
 - CLogChangeConfig;
 - CLogMaintenance;
 - CLogServViewWindowFetcher;
  
Every client request is processed asynchronously and consists of two IPC calls:
 - ELogOperationInitiate - the client initiates asynchronous operation that will be executed by 
   one of the hitters;
 - ELogOperationGetResult - after the hitter completes the execution of the requested operation and
   if there is a result for the client, the client can collect the result issuing this IPC request;

The client requests processing is relatively complex and I hope the following "add event" step-by-step
example can clarify how the LogEng server works.
 1. LogEng client issues "add event" request
 2. The request is handled by CLogServSession::ServiceL()
 3. CLogServSession::ServiceL() calls CLogServSession::ServiceOperationFunctionL() where the IPC command 
    is identified as ELogOperationInitiate and a new operation is created by calling
 4. LogServFactory::NewOperationL(). Here the client request type is identified as ELogOperationEventAdd
    and an instance of CLogServOpEventAdd class is created
 5. The CLogServOpEventAdd class derives from CLogServOperationBase. The CLogServOperationBase constructor
    will add the just created operation to a queue of pending operations maintained by an instance of the
    CLogServOperationQueue class, which implements the MLogServOperationManager interface.
    The CLogServOpEventAdd instance is added to the queue by calling 
    MLogServOperationManager::OMOperationQueueAdd().
 6. The CLogServOperationQueue instance is an active object. 
    CLogServOperationQueue::OMOperationQueueAdd() completes itsef and calls SetActive().
    The execution control is returned to the server side session object. Later, when the active scheduler
    takes the execution control the CLogServOperationQueue::RunL() will be called.
 7. CLogServOperationQueue::RunL() will pick up the next pending operation from the queue and calls the
    operation's StartL() method - CLogServOpEventAdd::StartL().
 8. CLogServOpEventAdd::StartL() reads the client "add event" data from the message object and calls
    MLogServTaskInterface::TaskEventAddL() passing the client data as call arguments.
 9. MLogServTaskInterface, as the class name states, is an interface class implemented by the 
    CLogServDatabaseDriver class.
10. CLogServDatabaseDriver::TaskEventAddL() will call the StartL() method of the hitter - 
    CLogAddEvent::StartL().
11. CLogAddEvent::StartL() will complete itself and call SetActive().
12. The next time when the active scheduler takes the execution control, it will call 
    CLogActive::RunL() --> CLogAddEvent::DoRunL(). And the "add event" request will be executed
13. The LogEng client then can complete the "add event" request by calling the server using the 
    ELogOperationGetResult IPC code. CLogServOperationQueue::OMGetResultL() will retrieve the result of the
    operation and destroy the "add event" oeration. 
    
@see LogServFactory    
@see CLogServOpEventAdd
@see MLogServOperationManager
@see CLogServOperationQueue
@see MLogServTaskInterface
@see CLogServDatabaseDriver
@see CLogActive
@see CLogAddEvent

@internalComponent
*/
class CLogServSession : public CSession2, public MLogServDatabaseChangeObserver
	{
public:
	CLogServSession(TLogServSessionId aSessionId, 
                    MLogServSessionLifetimeObserver& aObserver, 
                    MLogServBackupInterface& aBackupInterface, 
                    MLogServTaskInterface& aTaskInterface, 
                    MLogServOperationManager& aOperationManager, 
                    MLogServDatabaseChangeInterface& aChangeInterface, 
                    MLogServDatabaseTransactionInterface& aDatabase);
	~CLogServSession();

    inline TLogServSessionId Id() const;
    CLogServServer& Server() const;
	
private:
	void DCOHandleGlobalChangeEventL(const TLogServDatabaseChangeDefinition& aChange);//FROM MLogServDatabaseChangeObserver
	void CreateL();
	virtual void ServiceL(const RMessage2& aMessage);
	virtual void ServiceError(const RMessage2& aMessage,TInt aError);
	CLogServViewBase& ViewByIdL(TLogViewId aViewId);
	TInt ViewPositionById(TLogViewId aViewId) const;
    void ReadClientServerDataL(TLogClientServerData& aClientServerData, 
    		const RMessage2& aMessage, TInt aMinOperation, TInt aMaxOperation);
	void ServiceViewFunctionL(const RMessage2& aMessage);
	void ServiceOperationFunctionL(const RMessage2& aMessage);
	void ExtendedNotifyCompleteL(TInt aCompletionCode);

private:
	TLogServSessionId iSessionId;
	MLogServSessionLifetimeObserver& iObserver;
	MLogServBackupInterface& iBackupInterface;
	MLogServTaskInterface& iTaskInterface;
	MLogServOperationManager& iOperationManager;
	MLogServDatabaseChangeInterface& iChangeInterface;
	MLogServDatabaseTransactionInterface& iDatabase;
	CLogPackage* iPackage;
	CLogNotify* iNotify;
	RPointerArray<CLogServViewBase> iViewList;
	RMessage2 iExtendedNotificationMessage;
	RArray<TLogServDatabaseChangeDefinition> iPendingGlobalChanges;
	TBool iExtendedNotificationRequested;
#ifdef LOGGING_ENABLED
	TName iClientThreadName;
#endif

	};

inline TLogServSessionId CLogServSession::Id() const
	{
	return iSessionId;
	}

#endif//__LOGSERVSESSION_H__
