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

#include "LogServServer.h"
#include "logservsession.h"
#include "logservpanic.h"
#include "LogServShutdownTimer.h"
#include "LogServDatabaseMarshall.h"
#include "LogServDatabaseDriver.h"
#include "LogServBackupManager.h"
#include "LogServOperationQueue.h"
#include "LogServResourceInterpreter.h"
#include "LogServDatabaseChangeTracker.h"
#include "LogServRecentListManager.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServServer (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServServer::CLogServServer()
:	CServer2(CActive::EPriorityStandard, ESharableSessions)
	{
	}

CLogServServer::~CLogServServer()
	{
	LOGTEXT("CLogServServer::~CLogServServer()");
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();

	for (CSession2* session = iter++; session != NULL; session = iter++)
		{
		delete session;
		}

	// Order is important
	delete iOperationQueue;
	//
	delete iDatabaseDriver;
	delete iChangeTracker;
	delete iDatabaseMarshall;
	//
	delete iRecentListManager;
	delete iResourceInterpreter;
	//
	delete iBackupManager;
	delete iShutdownTimer;
	iFsSession.Close();

	LOGTEXT("CLogServServer::~CLogServServer() - end");
	}

void CLogServServer::ConstructL()
	{
	LOGTEXT("CLogServServer::ConstructL() - about to connect to file server");
	User::LeaveIfError(iFsSession.Connect());

	LOGTEXT("CLogServServer::ConstructL() - about to start server");
	StartL(KLogServerFullName);

	// Low level objects
	LOGTEXT("CLogServServer::ConstructL() - about to create shutdown timer");
	iShutdownTimer = CLogServShutdownTimer::NewL(Priority() + 1);

	LOGTEXT("CLogServServer::ConstructL() - about to create backup manager");
	iBackupManager = CLogServBackupManager::NewL(Priority());

	LOGTEXT("CLogServServer::ConstructL() - about to create resource interpreter");
	iResourceInterpreter = CLogServResourceInterpreter::NewL(iFsSession);

	LOGTEXT("CLogServServer::ConstructL() - about to create recent list manager");
	iRecentListManager = CLogServRecentListManager::NewL(*iResourceInterpreter, R_LOG_RECENT);

	// Medium level objects
	LOGTEXT("CLogServServer::ConstructL() - about to create database marshall");
	iDatabaseMarshall = CLogServDatabaseMarshall::NewL(iFsSession, *iResourceInterpreter, *iBackupManager);

	LOGTEXT("CLogServServer::ConstructL() - about to create change tracker");
	iChangeTracker = CLogServDatabaseChangeTracker::NewL(*iDatabaseMarshall, *iBackupManager, Priority() + 1);

	// Setup the pointer to the change tracker from within the database marshall (since its underlying
	// mixin provides a means of submitting change events)
	LOGTEXT("CLogServServer::ConstructL() - about to set change interface for database marshall ");
	iDatabaseMarshall->SetChangeInterface(*iChangeTracker);

	// These objects are quite high level... i.e. they use the engine objects above
	LOGTEXT("CLogServServer::ConstructL() - about to create database driver");
	iDatabaseDriver = CLogServDatabaseDriver::NewL(*iBackupManager, *iDatabaseMarshall, *iResourceInterpreter, *iRecentListManager, Priority() - 1);

	LOGTEXT("CLogServServer::ConstructL() - about to create operation queue");
	iOperationQueue = CLogServOperationQueue::NewL(*iDatabaseDriver, Priority());

	// Ensure that the server still exits even if the 1st client fails to connect
	// Only shutdown the server if it's transient - by default it isn't, but keeping
	// this code here in case this changes
	if	(iTransient)
		iShutdownTimer->Start();

	LOGTEXT("CLogServServer::ConstructL() - end");
	}

CLogServServer* CLogServServer::NewLC()
	{
	CLogServServer* self = new(ELeave) CLogServServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServServer::SLOHandleEvent(TLogServSessionId /*aId*/, TLogServSessionEvent aEvent)
	{
	switch(aEvent)
		{
	case ELogServSessionEventCreated:
		iShutdownTimer->Cancel();
		++iSessionCount;
		break;
	case ELogServSessionEventDestroyed:
		// Only shutdown the server if it's transient - by default it isn't
		if	(--iSessionCount == 0 && iTransient)
			iShutdownTimer->Start();
		break;
		}

	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
CSession2* CLogServServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
//
//	Create a new client session. This should really check the version number.
//
	{
	CLogServServer* self = const_cast<CLogServServer*>(this);
	const TLogServSessionId sessionId = ++self->iNextFreeSessionId;

	LOGTEXT2("CLogServServer::NewSessionL() - session id is: %d", sessionId);
	//
	CLogServSession* session = new(ELeave) CLogServSession(sessionId, 
												*self,
												*iBackupManager,
												*iDatabaseDriver,
												*iOperationQueue,
												*iChangeTracker,
												*iDatabaseMarshall);
	LOGTEXT("CLogServServer::NewSessionL() - end");
	return session;
	}

#ifdef _DEBUG
void CLogServServer::MakeTransient(TBool aTransient)
	{
	iTransient = aTransient;
	}
#endif//_DEBUG

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

