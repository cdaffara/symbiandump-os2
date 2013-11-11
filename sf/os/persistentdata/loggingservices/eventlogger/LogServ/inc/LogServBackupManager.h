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

#ifndef __LOGBACKUP_H__
#define __LOGBACKUP_H__

#include <babackup.h>
#include "LogServBackupInterface.h"
#include "LogServBackupObserver.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServBackupManagerManager (header)
/////////////////////////////////////////////////////////////////////////////////////////
/**
@internalComponent
*/
class CLogServBackupManager : public CTimer, public MBackupObserver, public MLogServBackupInterface
	{
public:
	static CLogServBackupManager* NewL(TInt aPriority = CActive::EPriorityStandard);
	~CLogServBackupManager();

private:
	CLogServBackupManager(TInt aPriority);
	void ConstructL();

public: // From MLogServBackupInterface
	void BIObserverAddL(MLogServBackupObserver& aObserver, TLogServBackupPriority aPriority);
	void BIObserverRemove(MLogServBackupObserver& aObserver);
	//
	TLogServBackupState BIState() const;
	TInt BIErrorValueForCurrentState() const;
	//
	void BISetDatabaseNameL(const TDesC& aDatabaseName);

private: // From CActive
	void RunL();
	TInt RunError(TInt aError);

private: // Internal structs
	struct TLogBackupNotificationEntry
		{
	public:
		inline TLogBackupNotificationEntry(MLogServBackupObserver& aObserver, TLogServBackupPriority aPriority) : iObserver(aObserver), iPriority(aPriority) { };

	public:
		MLogServBackupObserver& iObserver;
		TLogServBackupPriority iPriority;
		};

private:
	TInt NotifyObservers(MLogServBackupObserver::TLogServBackupEvent aEvent);
	void ChangeFileLockL(const TDesC& aFileName,TFileLockFlags aFlags);
	CBaBackupSessionWrapper* CreateBackupL(const TDesC& aLogDatabaseFileName);
	static TInt CompareEntries(const TLogBackupNotificationEntry& aLeft, const TLogBackupNotificationEntry& aRight);

private:
	/**
	 * So we can check we only act upon the correct backup event... essentially
	 * we are not trusting the backup server to do the right thing.
	 */
	HBufC* iDatabaseName;
	
	/**
	 * Interface to the backup server
	 */
	CBaBackupSessionWrapper* iBackup;

	/**
	 * Current state
	 */
	TLogServBackupState iState;

	/**
	 * Backup notification queue
	 */
	RArray<TLogBackupNotificationEntry> iObservers;
	};


#endif
