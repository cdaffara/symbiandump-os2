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

#include "LogViewWindowChangeObserver.h"

// Constants
const TInt KLogViewIgnoreStackGranularity = 3;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewWindowChangeObserver (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogViewWindowChangeObserver::CLogViewWindowChangeObserver(MLogViewChangeObserverInternal& aObserver)
:	iObserver(aObserver), iIgnoreStack(KLogViewIgnoreStackGranularity)
	{
	}

CLogViewWindowChangeObserver::~CLogViewWindowChangeObserver()
	{
	iIgnoreStack.Close();
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindowChangeObserver::IgnoreNextEventL(TLogId aId, TType aType)
	{
	TIgnoreEvent event(aId, aType);
	User::LeaveIfError(iIgnoreStack.Append(event));
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindowChangeObserver::HandleLogViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	const TInt index = Find(aId, ELogEventTypeAdd);
	if	(index == KErrNotFound)
		iObserver.HandleLogViewChangeEventAddedL(aId, aViewIndex, aChangeIndex, aTotalChangeCount);
	else
		iIgnoreStack.Remove(index);
	}

void CLogViewWindowChangeObserver::HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	const TInt index = Find(aId, ELogEventTypeChange);
	if	(index == KErrNotFound)
		iObserver.HandleLogViewChangeEventChangedL(aId, aViewIndex, aChangeIndex, aTotalChangeCount);
	else
		iIgnoreStack.Remove(index);
	}

void CLogViewWindowChangeObserver::HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	const TInt index = Find(aId, ELogEventTypeDelete);
	if	(index == KErrNotFound)
		iObserver.HandleLogViewChangeEventDeletedL(aId, aViewIndex, aChangeIndex, aTotalChangeCount);
	else
		iIgnoreStack.Remove(index);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindowChangeObserver::HandleLogViewChangeEventLogClearedL()
	{
	iObserver.HandleLogViewChangeEventLogClearedL();
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

TInt CLogViewWindowChangeObserver::Find(TLogId aId, TType aType) const
	{
	const TInt count = iIgnoreStack.Count();
	for(TInt i=0; i<count; i++)
		{
		const TIgnoreEvent& event = iIgnoreStack[i];
		if	(event.iId == aId && event.iType == aType)
			return i;
		}
	return KErrNotFound;
	}
