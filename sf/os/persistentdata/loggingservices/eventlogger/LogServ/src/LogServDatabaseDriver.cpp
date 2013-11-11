// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <logclientchangeobserver.h>
#include "LogServDatabaseDriver.h"
#include "LOGMAIN.H"
#include "LOGADD.H"
#include "LOGCHNGE.H"
#include "LOGGET.H"
#include "LOGDEL.H"
#include "LOGQUERY.H"
#include "logservpanic.h"
#include "LogServCacheConfig.h"
#include "LogServCacheTypes.h"
#include "LogServBackupInterface.h"
#include "LogServViewWindowFetcher.h"
#include "LogServRecentListManager.h"
#include "LogServDatabaseTransactionInterface.h"
#include "LogServDatabaseChangeInterface.h"
#include "LogServSqlStrings.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServDatabaseDriver (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServDatabaseDriver::CLogServDatabaseDriver(MLogServBackupInterface& aBackupInterface, MLogServDatabaseTransactionInterface& aDatabase, CLogServResourceInterpreter& aResourceInterface, CLogServRecentListManager& aRecentListManager, TInt aHitterPriorities)
:	iHitterPriorities(aHitterPriorities), iBackupInterface(aBackupInterface), iDatabase(aDatabase), iResourceInterface(aResourceInterface), iRecentListManager(aRecentListManager)
	{
	}

CLogServDatabaseDriver::~CLogServDatabaseDriver()
	{
	iBackupInterface.BIObserverRemove(*this);
	DestroyHitters();
	}

void CLogServDatabaseDriver::ConstructL()
	{
	iBackupInterface.BIObserverAddL(*this, MLogServBackupInterface::EObjectDatabaseDriver);
	//
	CreateHittersL();
	}

CLogServDatabaseDriver* CLogServDatabaseDriver::NewL(MLogServBackupInterface& aBackupInterface, MLogServDatabaseTransactionInterface& aDatabase, CLogServResourceInterpreter& aResourceInterface, CLogServRecentListManager& aRecentListManager, TInt aHitterPriorities)
	{
	CLogServDatabaseDriver* self = new(ELeave) CLogServDatabaseDriver(aBackupInterface, aDatabase, aResourceInterface, aRecentListManager, aHitterPriorities);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServDatabaseDriver::BOHandleEventL(TLogServBackupEvent aEvent)
	{
	switch(aEvent)
		{
	case EBackupStarting:
		LOGTEXT("CLogServDatabaseDriver::BOHandleEventL() - Backup STARTING");
		DestroyHitters();
		break;
	case EBackupEnded:
		LOGTEXT("CLogServDatabaseDriver::BOHandleEventL() - Backup ENDING");
		CreateHittersL();
		break;
	default:
		__ASSERT_DEBUG(EFalse, User::Invariant());
		break;
		}

	LOGTEXT("CLogServDatabaseDriver::BOHandleEventL() - end");
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServDatabaseDriver::TaskEventAddL(TRequestStatus& aStatus, CLogEvent& aEvent, const RMessage2& aMessage)
//
// Add an event to the database
//
	{
	LOGTEXT("CLogServDatabaseDriver::TaskEventAddL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	// Get the recent list for this event
	const CLogServRecentList* list = iRecentListManager.GetRecentList(aEvent);

	// Do the work
	iAddEvent->StartL(aEvent, list, aStatus, aMessage);

	LOGTEXT("CLogServDatabaseDriver::TaskEventAddL() - end");
	}

void CLogServDatabaseDriver::TaskEventChangeL(TRequestStatus& aStatus, const CLogEvent& aEvent, const RMessage2& aMessage)
//
// Change an event in the database
//
	{
	LOGTEXT("CLogServDatabaseDriver::TaskEventChangeL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	// Get the recent list for this event
	const CLogServRecentList* list = iRecentListManager.GetRecentList(aEvent);
	
	//

	// Do the work
	iChangeEvent->StartL(aEvent, list, aStatus, aMessage);

	LOGTEXT("CLogServDatabaseDriver::TaskEventChangeL() - end");
	}

void CLogServDatabaseDriver::TaskEventGetL(TRequestStatus& aStatus, CLogEvent& aEvent, const RMessage2& aMessage)
//
// Get an event from the database
//
	{
	LOGTEXT("CLogServDatabaseDriver::TaskEventGetL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	// Do the work
	iGetEvent->StartL(aEvent, aStatus, aMessage);

	LOGTEXT("CLogServDatabaseDriver::TaskEventGetL() - end");
	}

void CLogServDatabaseDriver::TaskEventDeleteL(TRequestStatus& aStatus, TLogId aId, const RMessage2& aMessage)
//
// Delete an event from the database
//
	{
	LOGTEXT("CLogServDatabaseDriver::TaskEventDeleteL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	iDeleteEvent->StartL(aId, aStatus, aMessage);

	LOGTEXT("CLogServDatabaseDriver::TaskEventDeleteL() - end");
	}

void CLogServDatabaseDriver::TaskEventTypeAddL(TRequestStatus& aStatus, const CLogEventType& aEventType)
	{
	LOGTEXT("CLogServDatabaseDriver::TaskEventTypeAddL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();
	iDatabase.DTICacheTypes().AddTypeL(aEventType);
	//
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);

	LOGTEXT("CLogServDatabaseDriver::TaskEventTypeAddL() - end");
	}

void CLogServDatabaseDriver::TaskEventTypeGetL(TRequestStatus& aStatus, const CLogEventType*& aEventType, TUid aUid)
	{
	LOGTEXT("CLogServDatabaseDriver::TaskEventTypeGetL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	const TLogServCacheTypeEntry& entry = iDatabase.DTICacheTypes().FindByUid(aUid);
	aEventType = entry.iEventType;
	if(entry.iEventTypeId == KLogNullTypeId)
		{
		User::Leave(KErrNotFound);
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);

	LOGTEXT("CLogServDatabaseDriver::TaskEventTypeGetL() - end");
	}

void CLogServDatabaseDriver::TaskEventTypeChangeL(TRequestStatus& aStatus, const CLogEventType& aEventType)
	{
	LOGTEXT("CLogServDatabaseDriver::TaskEventTypeChangeL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();
	const TInt error = iDatabase.DTICacheTypes().ChangeType(aEventType);
	//
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, error);

	LOGTEXT("CLogServDatabaseDriver::TaskEventTypeChangeL() - end");
	}

void CLogServDatabaseDriver::TaskEventTypeDeleteL(TRequestStatus& aStatus, TUid aType)
	{
	LOGTEXT("CLogServDatabaseDriver::TaskEventTypeDeleteL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();
	const TInt error = iDatabase.DTICacheTypes().DeleteType(aType);
	//
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, error);

	LOGTEXT("CLogServDatabaseDriver::TaskEventTypeDeleteL() - end");
	}

void CLogServDatabaseDriver::TaskConfigGetL(TRequestStatus& aStatus, TLogConfig& aConfig)
	{
	LOGTEXT("CLogServDatabaseDriver::TaskConfigGetL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();
	aConfig = iDatabase.DTICacheConfig().Config();
	//
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);

	LOGTEXT("CLogServDatabaseDriver::TaskConfigGetL() - end");
	}

void CLogServDatabaseDriver::TaskConfigChangeL(TRequestStatus& aStatus, const TLogConfig& aConfig)
	{
	LOGTEXT("CLogServDatabaseDriver::TaskConfigChangeL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	iChangeConfig->StartL(aConfig, aStatus);

	LOGTEXT("CLogServDatabaseDriver::TaskConfigChangeL() - end");
	}

// aDate is expected to be UTC
void CLogServDatabaseDriver::TaskClearLogL(TRequestStatus& aStatus, const TTime& aDate
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		, TSimId aSimId
#endif		
		)
	{
	LOGTEXT("CLogServDatabaseDriver::TaskClearLogL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	// Format the date.
	TBuf<KLogMaxDateLength> dateString;
	aDate.FormatL(dateString, LogUtils::DateFormatForLocale());
	// Get list of events to purge
	TheSql.Format(KLogSqlDeleteOldestString, &dateString);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	if(aSimId != KLogNullSimId)
		{
		TheSql.Append(KLogAnd);
		TheSql.AppendFormat(KLogUNumberPredicate, &KLogFieldEventSimId, aSimId);
		}
#endif	
	iDatabase.DTIBeginWithRollBackProtectionLC();
	TInt rc = iDatabase.DTIExecuteSql(TheSql);
	User::LeaveIfError(rc);
	if(rc > 0)
	    {
		// Add log cleared events. First is for the window code, which receives messages
		// through the old API. The 2nd is for clients of the log engine
		iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeLogCleared, KLogNullId);
		iDatabase.DTIChangeInterface().DCISubmitGlobalChangeContextL(KLogClientChangeEventLogCleared);
		}
	iDatabase.DTICommitAndCancelRollbackProtectionL();
	// Complete the request
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);

	LOGTEXT("CLogServDatabaseDriver::TaskClearLogL() - end");
	}

void CLogServDatabaseDriver::TaskClearRecentL(TRequestStatus& aStatus, TInt aRecentList
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		, TSimId aSimId
#endif		
		)
	{
	LOGTEXT("CLogServDatabaseDriver::TaskClearRecentL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	TheSql.Copy(KLogSqlSelectAllRecent);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TBool whereAdded = EFalse;
#endif	
	// Get events to clear from recent lists
	if(aRecentList != KLogNullRecentList)
		{
		TheSql.Append(KLogWhere);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		whereAdded = ETrue;
#endif		
		TheSql.AppendFormat(KLogNumberPredicate, &KLogFieldEventRecentString, aRecentList);
		}
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	//..and clear from recent lists with specific SimId
	if(aSimId != KLogNullSimId)
		{
		if(whereAdded)
			{
			TheSql.Append(KLogAnd);
			}
		else
			{
			TheSql.Append(KLogWhere);
			}
		TheSql.AppendFormat(KLogUNumberPredicate, &KLogFieldEventSimId, aSimId);
		}
#endif	
	
    RLogDbView view;
    view.PrepareLC(iDatabase.DTIDatabase(), TheSql);
	
	if(view.FirstL())
		{
        static TDbColNo colId = 0;
        static TDbColNo colRecent = 0;
        static TDbColNo colDuplicate = 0;
        if(colId == 0)
            {
            CDbColSet* cs = view.ColSetL();
            colId = cs->ColNo(KLogFieldIdString);
            colRecent = cs->ColNo(KLogFieldEventRecentString);
            colDuplicate = cs->ColNo(KLogFieldEventDuplicateString);
            delete cs;
            }
		iDatabase.DTIBeginWithRollBackProtectionLC();
		// Iterate through the events
		do
			{
			// Get current event id
			view.GetL();
			const TLogId id = view.ColInt32(colId);
			// Make the change
			view.UpdateL();
			view.SetColNullL(colRecent);
			view.SetColNullL(colDuplicate);
			view.PutL();
			// This is a "hidden" change. It may affect the contents of a view, but the actual event hasn't changed
			iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChangedHidden, id);
			}
		while(view.NextL());
		iDatabase.DTICommitAndCancelRollbackProtectionL();
		}
    CleanupStack::PopAndDestroy(&view);

	// Complete the request
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);

	LOGTEXT("CLogServDatabaseDriver::TaskClearRecentL() - end");
	}

void CLogServDatabaseDriver::TaskMaintenanceStartL(TRequestStatus& aStatus, TBool aPurge)
//
//	Start database maintenance process
//
	{
	LOGTEXT("CLogServDatabaseDriver::TaskMaintenanceStartL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	iMaintainer->Start(aPurge, aStatus);

	LOGTEXT("CLogServDatabaseDriver::TaskMaintenanceStartL() - end");
	}

void CLogServDatabaseDriver::TaskBuildWindowL(TRequestStatus& aStatus, const CLogServViewBase& aView, const TLogTransferWindow& aWindow, const RMessage2& aMessage)
//
//	Build a client window
//
	{
	LOGTEXT("CLogServDatabaseDriver::TaskBuildWindowL()");

	// Handle when we're doing a backup
	iBackupInterface.BIValidateStateForDatabaseOperationL();

	iWindowFetcher->StartL(aStatus, aView, aWindow, aMessage); 

	LOGTEXT("CLogServDatabaseDriver::TaskBuildWindowL() - end");
	}

void CLogServDatabaseDriver::TaskCancelCurrent()
//
//	Cancel all active tasks
//
	{
	LOGTEXT("CLogServDatabaseDriver::TaskCancelCurrent()");

	if	(iAddEvent)
		iAddEvent->Cancel();
	if	(iChangeEvent)
		iChangeEvent->Cancel();
	if	(iGetEvent)
		iGetEvent->Cancel();
	if	(iDeleteEvent)
		iDeleteEvent->Cancel();
	if	(iChangeConfig)
		iChangeConfig->Cancel();
	if	(iWindowFetcher)
		iWindowFetcher->Cancel();

	LOGTEXT("CLogServDatabaseDriver::TaskCancelCurrent() - end");
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServDatabaseDriver::DestroyHitters()
	{
	LOGTEXT("CLogServDatabaseDriver::DestroyHitters()");

	TaskCancelCurrent();
	//
	delete iMaintainer;
	iMaintainer = NULL;
	delete iAddEvent;
	iAddEvent = NULL;
	delete iChangeEvent;
	iChangeEvent = NULL;
	delete iGetEvent;
	iGetEvent = NULL;
	delete iDeleteEvent;
	iDeleteEvent = NULL;
	delete iChangeConfig;
	iChangeConfig = NULL;
	delete iWindowFetcher;
	iWindowFetcher = NULL;

	LOGTEXT("CLogServDatabaseDriver::DestroyHitters() - end");
	}

void CLogServDatabaseDriver::CreateHittersL()
	{
	LOGTEXT("CLogServDatabaseDriver::CreateHittersL()");

	iMaintainer = CLogMaintenance::NewL(iDatabase, iHitterPriorities);
	iAddEvent = CLogAddEvent::NewL(iDatabase, iHitterPriorities);
	iChangeEvent = CLogChangeEvent::NewL(iDatabase, iHitterPriorities);
	iGetEvent = CLogGetEvent::NewL(iDatabase, iHitterPriorities);
	iDeleteEvent = CLogDeleteEvent::NewL(iDatabase, iHitterPriorities);
	iChangeConfig = CLogChangeConfig::NewL(iDatabase, iRecentListManager, iHitterPriorities);
	iWindowFetcher = CLogServViewWindowFetcher::NewL(iDatabase, iHitterPriorities);

	LOGTEXT("CLogServDatabaseDriver::CreateHittersL() - end");
	}
