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

#ifndef __LOGNOTIFY_H__
#define __LOGNOTIFY_H__

#include <logcli.h>
#include "LogServBackupObserver.h"
#include "LogServDatabaseChangeObserver.h"

// Classes referenced
class MLogServBackupInterface;
class MLogServDatabaseChangeInterface;

/**
Uses the previous classes to provide change notifications
@internalComponent
*/
class CLogNotify : public CTimer, public MLogServBackupObserver, public MLogServDatabaseChangeObserver
	{
public:
	static CLogNotify* NewL(MLogServBackupInterface& aBackupInterface, MLogServDatabaseChangeInterface& aChangeInterface, TInt aPriority);
	~CLogNotify();

private:
	CLogNotify(MLogServBackupInterface& aBackupInterface, MLogServDatabaseChangeInterface& aChangeInterface, TInt aPriority);
	void ConstructL();

public:
	void Notify(TTimeIntervalMicroSeconds32 aDelay, const RMessage2& aMessage);


private: // FROM MLogServDatabaseChangeObserver
	void DCOHandleChangeEventsL(const CLogChangeDefinition& aChanges);

private: // FROM MLogServBackupObserver
	void BOHandleEventL(TLogServBackupEvent aEvent);

public: // FROM CActive
	void Cancel();

private: // FROM CActive
	void RunL();
	void DoCancel();

private: // Internal
	void CheckForChanges();
	void CompleteClientRequest(TInt aCompletionCode);

private:

	/**
	 * So this object knows when a backup is starting. Have to cancel the delay timer
	 * during this period.
	 */
	MLogServBackupInterface& iBackupInterface;

	/**
	 * The change interface
	 */
	MLogServDatabaseChangeInterface& iChangeInterface;

	/**
	 * Counters for change tracking
	 */
	TUint iLastCommit;
	TUint iCommit;

	/**
	 * The client-side outstanding notification request
	 */

	RMessagePtr2 iMessagePtr;

	/**
	 * Used as a flag to indicate whether the client-side message should
	 * be completed
	 */
	TBool iCompleteClientMessage;

#ifdef LOGGING_ENABLED
public:
	TName iClientThreadName;
	inline void SetClientThreadName(const TDesC& aName) { iClientThreadName = aName; }
#endif
	};

#endif
