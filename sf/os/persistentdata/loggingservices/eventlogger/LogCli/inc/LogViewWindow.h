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

#ifndef __LOGVIEWWINDOW_H__
#define __LOGVIEWWINDOW_H__

// System includes
#include <e32base.h>

// User includes
#include <logwrap.h>
#include "LogServShared.h"
#include "LogViewWindowFetcher.h"
#include "LogViewWindowLockObserver.h"
#include "LogViewChangeObserverInternal.h"

// Classes referenced
class CLogClient;
class CLogPackage;
class CLogChangeDefinition;
class CLogFilterList;
class MLogViewChangeObserver;
class CLogViewSetupClientOp;
class CLogViewWindowFetcher;
class CLogViewWindowLockObserver;
class CLogViewWindowChangeObserver;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewWindow (header)
/////////////////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CLogViewWindow) : public CLogActive, public MLogViewChangeObserverInternal, public MLogViewWindowFetcherObserver, public MLogViewWindowLockObserver
/**
@internalComponent
*/
	{
public:
	CLogViewWindow(RLogSession& aSession, TLogViewId aViewId, TInt aWindowSize, MLogViewChangeObserver* aCascadeObserver, TInt aPriority);
	~CLogViewWindow();
	void ConstructL(CLogPackage& aPackage);

public:

	/**
	 * Prepare the view
	 */
	TInt Setup(const CLogFilterList& aFilterList, TInt aParam, TLogFilterConstructionType aFilterConstructionType);

	/**
	 * Move the view position
	 */
	TBool NavigateL(TLogNavigation aNavigate, TRequestStatus& aStatus);

	/**
	 * Remove the specified log event from the window if its present
	 */
	void RemoveFromWindowIfPresentL(TLogId aId);

	/**
	 * Access the change observer (needed to construct the view)
	 */
	MLogViewChangeObserverInternal& ChangeObserver();

	/**
	 * Access the event at the current position
	 */
	const CLogEvent& CurrsorEvent() const;

	/** gets iViewRecordCount */
	inline TInt ViewRecordCount() const;

private: // FROM MLogViewWindowLockObserver
	void HandleWindowLockStatusChangeL(TLogViewLockStatus aStatus);

private: // FROM MLogViewWindowFetcherObserver
	void HandleFetchedWindowItemL(TInt aItemIndex, CLogEvent* aEvent);

public: // FROM MLogViewChangeObserver
	void HandleLogViewChangeEventAddedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
	void HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
	void HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);

private: // FROM MLogViewChangeObserverInternal
	void HandleLogViewChangeEventLogClearedL();

private: // FROM CLogActive
	void DoRunL();
	void DoCancel();
	void DoComplete(TInt& aComplete);

private:
	enum TWindowState
		{
		EStateIdle = 0,
		EStateFetchingWindow,
		EStateProcessingWindow,
		EStateNavigateWithinWindow,
		EStateLocked
		};

private:
	void StateHandleWindowFetchStarting();
	void StateHandleWindowFetchedL();
	void StateHandleNavigation();

private:
	void CompleteRequest(TInt aCompletionCode = KErrNone);
	void CalculateWindowForCursorPosition(TInt aCursorPosition, TLogWindow& aWindow) const;
	void Reset();
	void SilentCancel();
	void RefetchL(const TLogWindow& aWindow, TInt aCursor);
	void ChangeState(TWindowState aNewState);
	void RemoveEvent(TInt aIndex);
	TInt CalculateCursorPosition(TLogNavigation aNavigate) const;
	TInt FindEvent(TLogId aId);
	TInt FindAndRemoveEvent(TLogId aId);

private:
	RLogSession& iSession;
	const TLogViewId iViewId;
	const TInt iWindowSize;
	MLogViewChangeObserver* iCascadeObserver;
	RPointerArray<CLogEvent> iEvents;

	TWindowState iState;
	TLogWindowAndCursor iWindow;
	TInt iViewRecordCount;

	CLogViewSetupClientOp* iWindowPreparer;
	CLogViewWindowFetcher* iWindowFetcher;
	CLogViewWindowLockObserver* iWindowLockObserver;
	CLogViewWindowChangeObserver* iWindowChangeObserver;
	};


inline 
TInt CLogViewWindow::ViewRecordCount() const
	{
	return iViewRecordCount;
	}

#endif
