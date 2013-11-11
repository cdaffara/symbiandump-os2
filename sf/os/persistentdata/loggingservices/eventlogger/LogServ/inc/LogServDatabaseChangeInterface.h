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

#ifndef __LOGSERVDATABASECHANGEINTERFACE_H__
#define __LOGSERVDATABASECHANGEINTERFACE_H__

#include "LogServDatabaseChangeObserver.h"
#include "LogChangeDefinition.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> MLogServDatabaseChangeInterface (header)
/////////////////////////////////////////////////////////////////////////////////////////
/**
@internalComponent
*/
class MLogServDatabaseChangeInterface
	{
/////////////////////////////////////////////////////////////////////////////////////////
public: // ENUMERATIONS
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
public: // FROM MLogServDatabaseChangeInterface
/////////////////////////////////////////////////////////////////////////////////////////

	/**
	 * Submit some contextual information to the change interface so that it can
	 * intelligently notify observers about a batch of updates
	 */
	virtual void DCISubmitChangedEventContextL(TLogDatabaseChangeType aType, TLogId aEventId) = 0;

	/**
	 * Submit some global information about a change. This isn't specific to an event
	 * but may include changes such as 'a backup is starting/ending' or 'the log
	 * has been cleared' etc.
	 */
	virtual void DCISubmitGlobalChangeContextL(TUid aChangeType, TInt aContextParam1 = 0, TInt aContextParam2 = 0, TInt aContextParam3 = 0) = 0;

/////////////////////////////////////////////////////////////////////////////////////////
public: // FROM MLogServDatabaseChangeInterface
/////////////////////////////////////////////////////////////////////////////////////////

	/** 
	 * Request change notifications
	 */
	virtual void DCIRequestChangeNotificationsL(MLogServDatabaseChangeObserver& aObserver) = 0;

	/**
	 * Cancel change notification request
	 */
	virtual void DCIRequestChangeNotificationsCancel(MLogServDatabaseChangeObserver& aObserver) = 0;
	};


#endif
