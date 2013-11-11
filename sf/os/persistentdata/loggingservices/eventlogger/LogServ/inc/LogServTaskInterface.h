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

#ifndef __LOGSERVTASKINTERFACE_H__
#define __LOGSERVTASKINTERFACE_H__

#include <logwrap.h>

// Classes referenced
class TLogConfig;
class TLogTransferWindow;
class CLogEvent;
class CLogEventType;
class CLogServViewBase;

/**
An interface class for starting a LogEng operation.
Every time when CLogServOperationQueue::RunL() takes the execution control, an operation will be picked up
from the pending queue and the StartL() method of the operation - called, for example: 
CLogServOpEventAdd::StartL(). The operation's StartL() method will read the client data and calls an
appropriate method from this interface - TaskEventAddL() for example. 
 
@see CLogServOperationQueue 

@internalComponent
*/
class MLogServTaskInterface
{
public:
	/** 
	 * Event tasks
	 */
	virtual void TaskEventAddL(TRequestStatus& aStatus, CLogEvent& aEvent, const RMessage2& aMessage) = 0;
	virtual void TaskEventChangeL(TRequestStatus& aStatus, const CLogEvent& aEvent, const RMessage2& aMessage) = 0;
	virtual void TaskEventGetL(TRequestStatus& aStatus, CLogEvent& aEvent, const RMessage2& aMessage) = 0;
	virtual void TaskEventDeleteL(TRequestStatus& aStatus, TLogId aId, const RMessage2& aMessage) = 0;

	/** 
	 * Event type tasks
	 */
	virtual void TaskEventTypeAddL(TRequestStatus& aStatus, const CLogEventType& aEventType) = 0;
	virtual void TaskEventTypeGetL(TRequestStatus& aStatus, const CLogEventType*& aEventType, TUid aUid) = 0;
	virtual void TaskEventTypeChangeL(TRequestStatus& aStatus, const CLogEventType& aEventType) = 0;
	virtual void TaskEventTypeDeleteL(TRequestStatus& aStatus, TUid aType) = 0;

	/**
	 * Configuration tasks
	 */
	virtual void TaskConfigGetL(TRequestStatus& aStatus, TLogConfig& aConfig) = 0;
	virtual void TaskConfigChangeL(TRequestStatus& aStatus, const TLogConfig& aConfig) = 0;

	/**
	 * Clearing tasks
	 */
	virtual void TaskClearLogL(TRequestStatus& aStatus, const TTime& aDate
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
			, TSimId aSimId
#endif			
			) = 0;
	virtual void TaskClearRecentL(TRequestStatus& aStatus, TInt aRecentList
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
			, TSimId aSimId
#endif
			) = 0;

	/**
	 * Maintenance tasks
	 */
	virtual void TaskMaintenanceStartL(TRequestStatus& aStatus, TBool aPurge) = 0;

	/**
	 * Window building tasks
	 */ 
	virtual void TaskBuildWindowL(TRequestStatus& aStatus, const CLogServViewBase& aView, 
                                  const TLogTransferWindow& aWindow, const RMessage2& aMessage) = 0;

	/**
	 * Cancellation
	 */
	virtual void TaskCancelCurrent() = 0;
};


#endif
