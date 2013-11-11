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

#ifndef __LOGSERVSERVER_H__
#define __LOGSERVSERVER_H__

#include <logcli.h>
#include "LogCliServShared.h"
#include "LogServSessionLifetimeObserver.h"

// Classes referenced
class CLogServRecentListManager;
class CLogServOperationQueue;
class CLogServDatabaseDriver;
class CLogServDatabaseMarshall;
class CLogServResourceInterpreter;
class CLogServBackupManager;
class CLogServShutdownTimer;
class CLogServDatabaseChangeTracker;

/**
The LogEng server class. Implements the MLogServSessionLifetimeObserver interface.
Owns the following objects (private data members): 
 - Shutdown timer (CLogServShutdownTimer instance);
 - Backup manager (CLogServBackupManager instance);
 - Resource interpreter (CLogServResourceInterpreter instance);
 - CLogServDatabaseMarshall instance;
 - CLogServDatabaseChangeTracker instance;
 - CLogServDatabaseDriver instance;
 - CLogServOperationQueue instance;

@see MLogServSessionLifetimeObserver
@see CLogServShutdownTimer 
@see CLogServBackupManager 
@see CLogServResourceInterpreter 
@see CLogServDatabaseMarshall 
@see CLogServDatabaseChangeTracker 
@see CLogServDatabaseDriver 
@see CLogServOperationQueue 
@internalComponent
*/
class CLogServServer : public CServer2, public MLogServSessionLifetimeObserver
	{
public:
	static CLogServServer* NewLC();
	~CLogServServer();
    void MakeTransient(TBool aTransient);

private:
	CLogServServer();
	void ConstructL();
    // FROM MLogServSessionLifetimeObserver
	void SLOHandleEvent(TLogServSessionId aId, TLogServSessionEvent aEvent);
    // FROM CServer
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

private:
	TInt iSessionCount;//Number of connected sessions
	TBool iTransient;//Whether the server is transient or not
	TLogServSessionId iNextFreeSessionId;//The next id to assign to any new session
	TLogOperationId iNextFreeOperationId;//The next free id to assign to an operation
	TLogViewId iNextFreeViewId;//The next free id to assign to a view
	RFs iFsSession;
	CLogServShutdownTimer* iShutdownTimer;
	CLogServBackupManager* iBackupManager;
	CLogServResourceInterpreter* iResourceInterpreter;
	CLogServRecentListManager* iRecentListManager;
	CLogServDatabaseMarshall* iDatabaseMarshall;
	CLogServDatabaseChangeTracker* iChangeTracker;
	CLogServDatabaseDriver* iDatabaseDriver;
	CLogServOperationQueue* iOperationQueue;
	};

#endif
