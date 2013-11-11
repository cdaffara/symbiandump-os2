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

#ifndef __LOGSERVOPERATIONFACTORY_H__
#define __LOGSERVOPERATIONFACTORY_H__

#include <e32base.h>
#include "LogServDefs.h"
#include "LogCliServShared.h"

// Classes referenced
class CLogPackage;
class CLogServViewBase;
class CLogServOperationBase;
class MLogServTaskInterface;
class MLogServBackupInterface;
class MLogServOperationManager;
class MLogServDatabaseTransactionInterface;

/**
A factory class, used for creation of operation objects and queueing them for execution.

Operation objects. Every operation class derives from CLogServOperationBase.

@see CLogServOperationBase

@see CLogServOpEventAdd
@see CLogServOpEventGet
@see CLogServOpEventChange
@see CLogServOpEventDelete
@see CLogServOpTypeAdd
@see CLogServOpTypeGet
@see CLogServOpTypeChange
@see CLogServOpTypeDelete
@see CLogServOpClearLog
@see CLogServOpClearRecent
@see CLogServOpConfigGet
@see CLogServOpConfigChange
@see CLogServOpMaintenance

@see CLogServOpViewSetup
@see CLogServOpViewEventRemove
@see CLogServOpViewClearDuplicates
@see CLogServOpViewSetFlags
@see CLogServOpViewWindowFetcher

View objects.Every view class derives from CLogServViewBase.

@see CLogServViewBase

@see CLogServViewEvent
@see CLogServViewRecent
@see CLogServViewDuplicate

The class also offers a factory method for views creation - NewViewL().
All created objects accept a reference to the MLogServOperationManager object and add themselves to the
operations queue.

@see MLogServOperationManager

@internalComponent
*/
class LogServFactory
{
public:
	static CLogServOperationBase* NewOperationL(const TLogClientServerData& aCliServData,
												MLogServTaskInterface& aTaskInterface,
												MLogServOperationManager& aOperationManager,
												const RMessage2& aMessage,
												CLogPackage& aLogPackage,
												TLogServSessionId aSessionId
												);

	static CLogServOperationBase* NewViewOperationL(const TLogClientServerData& aCliServData,
												MLogServTaskInterface& aTaskInterface,
												MLogServOperationManager& aOperationManager,
												const RMessage2& aMessage,
												CLogPackage& aLogPackage,
												TLogServSessionId aSessionId,
												CLogServViewBase& aView
												);


	static CLogServViewBase* NewViewL(TLogViewType aType,
									  TLogViewId aId,
									  MLogServDatabaseTransactionInterface& aDatabase,
									  MLogServBackupInterface& aBackupInterface,
									  CLogPackage& aPackage,
									  const RMessage2& aMessage
									  );
};

#endif

