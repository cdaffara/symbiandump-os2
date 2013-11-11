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

#include "LogServBackupManager.h"

// User includes
#include <logcli.h>
#include "logservpanic.h"

// Constants
const TInt KTimerDelay = 10000000; // 10 seconds


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServBackupManager (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServBackupManager::CLogServBackupManager(TInt aPriority)
:	CTimer(aPriority)
	{
	CActiveScheduler::Add(this);
	}

CLogServBackupManager::~CLogServBackupManager()
	{
	__ASSERT_DEBUG(iObservers.Count() == 0, Panic(ELogBackupObserversStillRegistered));
	Cancel();

	// Unregister ourselves with backup server
	if (iBackup)
		{
		if	(iDatabaseName)
			iBackup->DeregisterFile(*iDatabaseName);
		delete iBackup;
		}
	delete iDatabaseName;
	iObservers.Close();
	}

void CLogServBackupManager::ConstructL()
	{
	CTimer::ConstructL();
	}

CLogServBackupManager* CLogServBackupManager::NewL(TInt aPriority)
	{
	CLogServBackupManager* self = new(ELeave) CLogServBackupManager(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServBackupManager::BIObserverAddL(MLogServBackupObserver& aObserver, TLogServBackupPriority aPriority)
	{
	LOGTEXT2("CLogServBackupManager::BIObserverAddL(aPriority=%d)", aPriority);
	TLogBackupNotificationEntry entry(aObserver, aPriority);
	
	// Create orderer which ensures that we place the objects in priority ascending order (i.e. 
	// higher priority gets notified first).
	TLinearOrder<TLogBackupNotificationEntry> orderer(CompareEntries);
	const TInt error = iObservers.InsertInOrderAllowRepeats(entry, orderer);
	User::LeaveIfError(error);

	LOGTEXT("CLogServBackupManager::BIObserverAddL() - end");
	}

void CLogServBackupManager::BIObserverRemove(MLogServBackupObserver& aObserver)
//
//	Removes an observer from the notification queue
//
	{
	const TInt count = iObservers.Count();
	for(TInt i=0; i<count; i++)
		{
		const TLogBackupNotificationEntry& entry = iObservers[i];
		if	(&aObserver == &entry.iObserver)
			{
			iObservers.Remove(i);
			return;
			}
		}
	}

MLogServBackupInterface::TLogServBackupState CLogServBackupManager::BIState() const
	{
	return iState;
	}

TInt CLogServBackupManager::BIErrorValueForCurrentState() const
	{
	TInt error = KErrNone;
	switch(BIState())
		{
	case ELogServBackupStateIdle:
		error = KErrNone;
		break;
	case ELogServBackupStateBackupInProgress:
		error = KErrAccessDenied;
		break;
		}
	//
	LOGTEXT2("CLogServBackupManager::BIErrorValueForCurrentState() = %d", error);
	return error;
	}

void CLogServBackupManager::BISetDatabaseNameL(const TDesC& aDatabaseName)
	{
#ifdef LOGGING_ENABLED
	LOGTEXT3("CLogServBackupManager::BISetDatabaseNameL(%S, isActive: %d)", &aDatabaseName, IsActive());
	if	(iDatabaseName)
		{
		LOGTEXT2("CLogServBackupManager::BISetDatabaseNameL() - currently registered database filename is: %S", iDatabaseName);
		}
	else
		{
		LOGTEXT("CLogServBackupManager::BISetDatabaseNameL() - no file registered with backup interface yet");
		}
#endif

	Cancel();

	HBufC* databaseName = aDatabaseName.AllocLC();

	// If we haven't already created a backup observer, then we need
	// to kick the object back into life again.
	if	(!iBackup)
		{
		LOGTEXT("CLogServBackupManager::BISetDatabaseNameL() - no backup session created");

		// Try and create backup interface synchronously first of all, if that fails
		// then construct as an idle operation
		TRAPD(err, iBackup = CreateBackupL(*databaseName));

		LOGTEXT2("CLogServBackupManager::BISetDatabaseNameL() - backup session creation error: %d", err);
		if	(err != KErrNone)
			After(0);
		}
	else if (iDatabaseName->Compare(aDatabaseName) != KErrNone)
		{
		LOGTEXT3("CLogServBackupManager::BISetDatabaseNameL() - database filename changed from %S to %S", &iDatabaseName, &aDatabaseName);

		// De register the old, register the new
		iBackup->DeregisterFile(*iDatabaseName);
		iBackup->RegisterFileL(aDatabaseName, *this);

		LOGTEXT("CLogServBackupManager::BISetDatabaseNameL() - database re-registration complete");
		}


	delete iDatabaseName;
	iDatabaseName = databaseName;
	CleanupStack::Pop(databaseName);

	LOGTEXT("CLogServBackupManager::BISetDatabaseNameL() - end");
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServBackupManager::RunL()
//
// This method does two things
//
// 1) Keeps trying to create a backup object - which may fail on device 
//    bootup until the ui framework starts the backup server.
//
// 2) Handles the case where the server fails to restart correctly after a backup - it keeps trying
//
	{
	LOGTEXT2("CLogServBackupManager::RunL(%d)", iStatus.Int());

	if	(!iBackup)
		{
		LOGTEXT("CLogServBackupManager::RunL() - trying to create backup object");

		// Keep trying to create backup object
		iBackup = CreateBackupL(*iDatabaseName);

		LOGTEXT("CLogServBackupManager::RunL() - backup object created okay");
		}
	else
		{
		// This branch is executed if we failed to create the backup object on our first
		// attempt in BISetDatabaseNameL
		LOGTEXT("CLogServBackupManager::RunL() - notifying observers about dummy backup ended event");
		NotifyObservers(MLogServBackupObserver::EBackupEnded);
		}

	LOGTEXT("CLogServBackupManager::RunL() - end");
	}

TInt CLogServBackupManager::RunError(TInt aError)
	{
	(void) aError;
	LOGTEXT2("CLogServBackupManager::RunError(%d)", aError);

	// Make sure we don't leak anything, pretend the backup started again - won't fail
	if	(iBackup)
		NotifyObservers(MLogServBackupObserver::EBackupStarting);

	After(KTimerDelay);

	LOGTEXT("CLogServBackupManager::RunError() - end");
	return KErrNone;
	}

TInt CLogServBackupManager::NotifyObservers(MLogServBackupObserver::TLogServBackupEvent aEvent)
//
//	Notify observers of the event. Assumes event queue correctly ordered.
//
	{
	const TInt count = iObservers.Count();
	LOGTEXT3("CLogServBackupManager::NotifyObservers(aEvent = %d) - %d observers", aEvent, count);

	// Depending on the event type, we have to reverse the order of notification.
	// I wish it was possible to do this in a more elegant way... hmm... pointers
	// to member functions?...

	TInt error = KErrNone;
	TRAP(error, 
		//
		switch(aEvent)
			{
		case MLogServBackupObserver::EBackupStarting:
			{
			// Update our state
			iState = ELogServBackupStateBackupInProgress;

			// Notify
			for(TInt i=0; i<count; i++)
				{
				TLogBackupNotificationEntry& entry = iObservers[i];
				entry.iObserver.BOHandleEventL(aEvent);
				}
			}
			break;
		case MLogServBackupObserver::EBackupEnded:
			{
			// Update our state
			iState = ELogServBackupStateIdle;

			// Notify
			for(TInt i=count-1; i>=0; i--)
				{
				TLogBackupNotificationEntry& entry = iObservers[i];
				entry.iObserver.BOHandleEventL(aEvent);
				}
			}
			break;
			}
		);

	LOGTEXT("CLogServBackupManager::NotifyObservers() - end");
	return error;
	}

void CLogServBackupManager::ChangeFileLockL(const TDesC& aFileName, TFileLockFlags aFlags)
	{
	LOGTEXT3("CLogServBackupManager::ChangeFileLockL(%S, aFlags = %d)", &aFileName, aFlags);

#ifdef LOGGING_ENABLED
	if	(aFlags & (MBackupObserver::EReleaseLockReadOnly | MBackupObserver::EReleaseLockNoAccess))
		{
		LOGTEXT("CLogServBackupManager::ChangeFileLockL() - Backup is STARTING");
		}
	else
		{
		LOGTEXT("CLogServBackupManager::ChangeFileLockL() - Backup is ENDING");
		}
#endif

	Cancel();

	// This probably won't ever happen
	if	(iDatabaseName->Compare(aFileName) != 0)
		{
		LOGTEXT("CLogServBackupManager::ChangeFileLockL() - Notification from backup server about the wrong file!");
		User::Leave(KErrNotFound);
		}

	// Work out type of backup event
	MLogServBackupObserver::TLogServBackupEvent event = MLogServBackupObserver::EBackupEnded;
	if	(aFlags & (MBackupObserver::EReleaseLockReadOnly | MBackupObserver::EReleaseLockNoAccess))
		event = MLogServBackupObserver::EBackupStarting;

	// Notify observers
	const TInt error = NotifyObservers(event);
	LOGTEXT2("CLogServBackupManager::ChangeFileLockL() - notifying observers error: %d", error);

	// Shouldn't be any problems when notifying observers
	__ASSERT_DEBUG(!(event == MLogServBackupObserver::EBackupStarting && error != KErrNone), Panic(ELogStartBackupFailure));

	// Handle failure to restart the server after a backup
	if	(error && (event == MLogServBackupObserver::EBackupEnded))
		{
		LOGTEXT("CLogServBackupManager::ChangeFileLockL() - Kicking off server objects again after backup completed (and there was an error)");

		// Make sure we don't leak anything, pretend the backup started again - won't fail
		Cancel();
		NotifyObservers(MLogServBackupObserver::EBackupStarting);
		After(KTimerDelay);
		}

	LOGTEXT("CLogServBackupManager::ChangeFileLockL() - end");
	}

CBaBackupSessionWrapper* CLogServBackupManager::CreateBackupL(const TDesC& aLogDatabaseFileName)
	{
	LOGTEXT("CLogServBackupManager::CreateBackupL()");

	// Create backup session
	CBaBackupSessionWrapper* backup = CBaBackupSessionWrapper::NewL();
	CleanupStack::PushL(backup);

	// Register with the backup server
	backup->RegisterFileL(aLogDatabaseFileName, *this);

	// All done
	CleanupStack::Pop(backup);

	LOGTEXT("CLogServBackupManager::CreateBackupL() - end");
	return backup;
	}

TInt CLogServBackupManager::CompareEntries(const TLogBackupNotificationEntry& aLeft, const TLogBackupNotificationEntry& aRight)
	{
	if	(aLeft.iPriority == aRight.iPriority)
		return 0;
	else if (aLeft.iPriority > aRight.iPriority) // Not what you might expect since higher priority must go first
		return -1;
	return 1;
	}
