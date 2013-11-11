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

#ifndef __LOGSERVDATABASEDRIVER_H__
#define __LOGSERVDATABASEDRIVER_H__

#include <e32base.h>
#include "LogServTaskInterface.h"
#include "LogServBackupObserver.h"

// Classes referenced
class CLogEvent;
class CLogEventType;
class CLogAddEvent;
class CLogMaintenance;
class CLogChangeEvent;
class CLogGetEvent;
class CLogDeleteEvent;
class CLogChangeConfig;
class MLogServDatabaseTransactionInterface;
class MLogServBackupInterface;
class CLogServRecentListManager;
class CLogServResourceInterpreter;
class CLogServViewWindowFetcher;

/**
Implements MLogServTaskInterface and MLogServBackupObserver.
Maintains a list of hitters, i.e. objects that will execute the requested by the client operation - add event, etc.
Every time, when one of the implemented TaskEventAddL(), TaskEventChangeL() etc. methods is called, that method
will read the client data and call the StartL() method of the hitter suitable for that operation.

@see MLogServTaskInterface
@see MLogServBackupObserver
@see CLogAddEvent;
@see CLogGetEvent;
@see CLogChangeEvent;
@see CLogDeleteEvent;
@see CLogChangeConfig;
@see CLogMaintenance;
 
@internalComponent
*/
class CLogServDatabaseDriver : public CBase, public MLogServTaskInterface, public MLogServBackupObserver
	{
public:
	static CLogServDatabaseDriver* NewL(MLogServBackupInterface& aBackupInterface, MLogServDatabaseTransactionInterface& aDatabase, CLogServResourceInterpreter& aResourceInterface, CLogServRecentListManager& aRecentListManager, TInt aHitterPriorities);
	~CLogServDatabaseDriver();

private:
	CLogServDatabaseDriver(MLogServBackupInterface& aBackupInterface, MLogServDatabaseTransactionInterface& aDatabase, CLogServResourceInterpreter& aResourceInterface, CLogServRecentListManager& aRecentListManager, TInt aHitterPriorities);
	void ConstructL();

private: // FROM MLogServBackupObserver
	void BOHandleEventL(TLogServBackupEvent aEvent);

private: // FROM MLogServTaskInterface
	void TaskEventAddL(TRequestStatus& aStatus, CLogEvent& aEvent, const RMessage2& aMessage);
	void TaskEventChangeL(TRequestStatus& aStatus, const CLogEvent& aEvent, const RMessage2& aMessage);
	void TaskEventGetL(TRequestStatus& aStatus, CLogEvent& aEvent, const RMessage2& aMessage);
	void TaskEventDeleteL(TRequestStatus& aStatus, TLogId aId, const RMessage2& aMessage);
	//
	void TaskEventTypeAddL(TRequestStatus& aStatus, const CLogEventType& aEventType);
	void TaskEventTypeGetL(TRequestStatus& aStatus, const CLogEventType*& aEventType, TUid aUid);
	void TaskEventTypeChangeL(TRequestStatus& aStatus, const CLogEventType& aEventType);
	void TaskEventTypeDeleteL(TRequestStatus& aStatus, TUid aType);
	//
	void TaskConfigGetL(TRequestStatus& aStatus, TLogConfig& aConfig);
	void TaskConfigChangeL(TRequestStatus& aStatus, const TLogConfig& aConfig);
	//
	void TaskClearLogL(TRequestStatus& aStatus, const TTime& aDate
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
			, TSimId aSimId
#endif			
			);
	void TaskClearRecentL(TRequestStatus& aStatus, TInt aRecentList
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
			, TSimId aSimId
#endif			
			);
	//
	void TaskMaintenanceStartL(TRequestStatus& aStatus, TBool aPurge);
	//
	void TaskBuildWindowL(TRequestStatus& aStatus, const CLogServViewBase& aView, const TLogTransferWindow& aWindow, const RMessage2& aMessage);
	//
	void TaskCancelCurrent();

private:
	void DestroyHitters();
	void CreateHittersL();

private:
	TInt iHitterPriorities;//The active object priority used for all objects which hit the database
	MLogServBackupInterface& iBackupInterface;//The interface to the backup manager
	MLogServDatabaseTransactionInterface& iDatabase;//Access the database
	CLogServResourceInterpreter& iResourceInterface;//Access resource files
	CLogServRecentListManager& iRecentListManager;//Access recent list
	// The hitters - named as such because they all hit the database in some
	// way (e.g. read or write from/to it).
	CLogAddEvent* iAddEvent;
	CLogGetEvent* iGetEvent;
	CLogChangeEvent* iChangeEvent;
	CLogDeleteEvent* iDeleteEvent;
	CLogChangeConfig* iChangeConfig;
	CLogMaintenance* iMaintainer;
	CLogServViewWindowFetcher* iWindowFetcher;

	};

#endif

