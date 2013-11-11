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

#ifndef __LOGSERVBACKUPINTERFACE_H__
#define __LOGSERVBACKUPINTERFACE_H__

#include <e32std.h>

// Classes referenced
class MLogServBackupObserver;

/**
Typedefs
@internalComponent
*/
typedef TInt TLogServBackupPriority;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> MLogServBackupInterface (header)
/////////////////////////////////////////////////////////////////////////////////////////
/**
@internalComponent
*/
class MLogServBackupInterface
	{
public:
	enum TLogServBackupState
		{
		ELogServBackupStateIdle = 0,
		ELogServBackupStateBackupInProgress
		};

	enum TLogBackupStandardNotificationPriorities
		{
		// The first object that will be told a backup is starting (and the last
		// to be told that a backup has ended).
		ELogBackupPriorityFirstToBeNotified = 1000,

		// The last object to be told that a backup is starting (but the first
		// to be told that it has ended).
		ELogBackupPriorityLastToBeNotified = -1000,

		// Individual object priorities. Not terribly nice really...
		EObjectDatabaseDriver = ELogBackupPriorityFirstToBeNotified,
		EObjectViewLock = EObjectDatabaseDriver - 1,
		EObjectView = EObjectViewLock - 1,
		EObjectClientChangeNotifications = EObjectView - 1,
		EObjectChangeTracker = EObjectClientChangeNotifications -1,

		// Must be last, since it must close the database after everything
		// has stopped using it.
		EObjectDatabaseMarshall = ELogBackupPriorityLastToBeNotified,

		};

public: // Observer interaction
	virtual void BIObserverAddL(MLogServBackupObserver& aObserver, TLogServBackupPriority aPriority) = 0;
	virtual void BIObserverRemove(MLogServBackupObserver& aObserver) = 0;

public: // Enquiry interface
	virtual TLogServBackupState BIState() const = 0;
	virtual TInt BIErrorValueForCurrentState() const = 0;
	void BIValidateStateForDatabaseOperationL();

public: // Misc
	virtual void BISetDatabaseNameL(const TDesC& aDatabaseName) = 0;
	};


#endif

