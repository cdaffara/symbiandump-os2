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

#ifndef __LOGSERVVIEWCHANGEMANAGER_H__
#define __LOGSERVVIEWCHANGEMANAGER_H__

#include <logcli.h>
#include "LogChangeDefinition.h"
#include "LogCliServShared.h"
#include "LogServBackupObserver.h"
#include "LogServDatabaseChangeObserver.h"

// Classes referenced
class MLogServDatabaseChangeInterface;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServViewBase (header)
/////////////////////////////////////////////////////////////////////////////////////////
/**
@internalComponent
*/
class CLogServViewChangeManager : public CBase
	{
public:
	static CLogServViewChangeManager* NewL(MLogServDatabaseChangeInterface& aChangeInterface);
	~CLogServViewChangeManager();

private:
	CLogServViewChangeManager(MLogServDatabaseChangeInterface& aChangeInterface);
	void ConstructL();

public: // CHANGE TRANSACTION

	/**
	 *
	 */
	void ChangeTransactionPrepare();

	/**
	 *
	 */
	void ChangeTransactionSubmitL(TLogId aId, TLogDatabaseChangeType aType, TInt aViewIndex);

	/**
	 *
	 */
	void ChangeTransactionCommitL();


public: // CHANGE DELIVERY

	/**
	 * Deliver the next available batch of changes. Panics if there
	 * aren't any.
	 */
	void DeliverChangesL(const RMessage2& aMessage);

public: // CHANGE REQUEST MANAGEMENT

	/**
	 * The client has requested change notifications
	 */
	void RequestChangeNotifications(const RMessage2& aMessage);

	/**
	 * The client has cancelled a previous change notification request
	 */
	void RequestChangeNotificationsCancel();

private:
	void NotifyClient();
	void CompleteClientChangeMessage(TInt aCompletionCode);

private:
	MLogServDatabaseChangeInterface& iChangeInterface;

	/**
	 * Completed when the view contents change
	 */
	RMessagePtr2 iClientSideChangeMessage;

	/**
	 * A buffer which contain the list of changed items
	 * for the current change transaction
	 */
	CLogChangeDefinition* iTransientChangeDefinition;

	/**
	 * All the change 'packages' which are outstanding for this client
	 */
	RPointerArray<CBufBase> iPendingChanges;

	};

#endif
