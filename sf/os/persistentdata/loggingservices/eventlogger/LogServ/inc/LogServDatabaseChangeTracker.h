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

#ifndef __LOGSERVDATABASECHANGETRACKER_H__
#define __LOGSERVDATABASECHANGETRACKER_H__

#include <d32dbms.h>
#include "LogServBackupObserver.h"
#include "LogServDatabaseChangeInterface.h"
#include "LogServDatabaseChangeDefinition.h"

// Classes referenced
class CLogChangeDefinition;
class MLogServBackupInterface;
class MLogServDatabaseTransactionInterface;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServDatabaseChangeTracker (header)
/////////////////////////////////////////////////////////////////////////////////////////
/**
@internalComponent
*/
class CLogServDatabaseChangeTracker : public CActive, public MLogServBackupObserver, public MLogServDatabaseChangeInterface
	{
/////////////////////////////////////////////////////////////////////////////////////////
public:
/////////////////////////////////////////////////////////////////////////////////////////
	static CLogServDatabaseChangeTracker* NewL(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, TInt aPriority);
	~CLogServDatabaseChangeTracker();

/////////////////////////////////////////////////////////////////////////////////////////
private: // INTERNAL CONSTRUCT
/////////////////////////////////////////////////////////////////////////////////////////
	CLogServDatabaseChangeTracker(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, TInt aPriority);
	void ConstructL();

/////////////////////////////////////////////////////////////////////////////////////////
private: // FROM MLogServDatabaseChangeInterface
/////////////////////////////////////////////////////////////////////////////////////////
	void DCISubmitChangedEventContextL(TLogDatabaseChangeType aType, TLogId aEventId);
	void DCISubmitGlobalChangeContextL(TUid aChangeType, TInt aContextParam1, TInt aContextParam2, TInt aContextParam3);
	//
	void DCIRequestChangeNotificationsL(MLogServDatabaseChangeObserver& aObserver);
	void DCIRequestChangeNotificationsCancel(MLogServDatabaseChangeObserver& aObserver);

/////////////////////////////////////////////////////////////////////////////////////////
private: // FROM MLogServBackupObserver
/////////////////////////////////////////////////////////////////////////////////////////
	void BOHandleEventL(TLogServBackupEvent aEvent);

/////////////////////////////////////////////////////////////////////////////////////////
private: // FROM CActive
/////////////////////////////////////////////////////////////////////////////////////////
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

/////////////////////////////////////////////////////////////////////////////////////////
private: // INTERNAL METHODS
/////////////////////////////////////////////////////////////////////////////////////////
	void Request();
	void NotifyObserversL();
	void NotifyGlobalChangeEventsL();
	static TBool IdleNotifyGlobalChangeEvents(TAny* aSelf);

/////////////////////////////////////////////////////////////////////////////////////////
private: // MEMBER DATA
/////////////////////////////////////////////////////////////////////////////////////////

	/**
	 * Watches the database for change events
	 */
	MLogServDatabaseTransactionInterface& iDatabase;

	/**
	 * Watches the database for change events
	 */
	MLogServBackupInterface& iBackupInterface;

	/**
	 * Watches the database for change events
	 */
	RDbNotifier iNotifier;

	/**
	 * Observers
	 */
	RPointerArray<MLogServDatabaseChangeObserver> iObservers;

	/**
	 * Track changes
	 */
	CLogChangeDefinition* iChanges;

	/**
	 * Used to ensure any global changes are pushed to clients
	 * only after all other active object operations are complete.
	 */
	CIdle* iIdler;

	/**
	 * A queue of global change events
	 */
	RArray<TLogServDatabaseChangeDefinition> iGlobalChanges;
	};


#endif
