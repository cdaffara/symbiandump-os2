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

#ifndef __LOGVIEWWINDOWCHANGEOBSERVER_H__
#define __LOGVIEWWINDOWCHANGEOBSERVER_H__

// System includes
#include <e32base.h>

// User includes
#include "LogViewChangeObserverInternal.h"

// Classes referenced
class RLogSession;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewWindowChangeObserver (header)
/////////////////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CLogViewWindowChangeObserver) : public CBase, public MLogViewChangeObserverInternal
/**
@internalComponent
*/
	{
public:
	enum TType
		{
		ELogEventTypeAdd = 0,
		ELogEventTypeChange,
		ELogEventTypeDelete
		};

public:
	CLogViewWindowChangeObserver(MLogViewChangeObserverInternal& aObserver);
	~CLogViewWindowChangeObserver();

public:

	/**
	 * Request that the next change notification corresponding to the specified
	 * log id and change type is ignored.
	 */
	void IgnoreNextEventL(TLogId aId, TType aType);

private: // FROM MLogViewChangeObserver
	void HandleLogViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
	void HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
	void HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);

private: // FROM MLogViewChangeObserverInternal
	void HandleLogViewChangeEventLogClearedL();

private:
	TInt Find(TLogId aId, TType aType) const;

private:
	class TIgnoreEvent
		{
	public:
		inline TIgnoreEvent(TLogId aId, TType aType) : iId(aId), iType(aType) { }

	public:
		TLogId iId;
		TType iType;
		};
	
private:
	MLogViewChangeObserverInternal& iObserver;
	RArray<TIgnoreEvent> iIgnoreStack;
	};


#endif
