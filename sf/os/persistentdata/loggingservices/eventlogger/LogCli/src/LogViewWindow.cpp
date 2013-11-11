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

#include "LogViewWindow.h"

// User includes
#include "logclientop.h"
#include "logclipanic.h"
#include "LogViewWindowChangeObserver.h"

// Constants
const TInt KWindowSlideSize = 2;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewWindow (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogViewWindow::CLogViewWindow(RLogSession& aSession, TLogViewId aViewId, TInt aWindowSize, MLogViewChangeObserver* aCascadeObserver, TInt aPriority)
:	CLogActive(aPriority), iSession(aSession), iViewId(aViewId), iWindowSize(aWindowSize), iCascadeObserver(aCascadeObserver), iEvents(aWindowSize)
	{
	Reset();
	}

CLogViewWindow::~CLogViewWindow()
	{
	Cancel();
	//
	delete iWindowChangeObserver;
	delete iWindowPreparer;
	delete iWindowFetcher;
	delete iWindowLockObserver;
	//
	iEvents.ResetAndDestroy();
	iEvents.Close();
	}

void CLogViewWindow::ConstructL(CLogPackage& aPackage)
	{
	iWindowPreparer = new(ELeave) CLogViewSetupClientOp(iSession, aPackage, CActive::EPriorityStandard);
	//
	iWindowFetcher = new(ELeave) CLogViewWindowFetcher(iSession, iViewId, *this, CActive::EPriorityIdle + 1);
	iWindowFetcher->ConstructL();
	//
	iWindowLockObserver = new(ELeave) CLogViewWindowLockObserver(iSession, iViewId, *this, CActive::EPriorityHigh);
	iWindowLockObserver->ConstructL();
	//
	iWindowChangeObserver = new(ELeave) CLogViewWindowChangeObserver(*this);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

TInt CLogViewWindow::Setup(const CLogFilterList& aFilterList, TInt aParam, TLogFilterConstructionType aFilterConstructionType)
	{
	Reset();
	iViewRecordCount = iWindowPreparer->Start(iViewId, aFilterList, aParam, aFilterConstructionType);
	return iViewRecordCount;
	}

TBool CLogViewWindow::NavigateL(TLogNavigation aNavigate, TRequestStatus& aStatus)
	{
	// Check why we're already active
	switch(iState)
		{
	case EStateIdle:
	case EStateLocked:
		__ASSERT_DEBUG(!IsActive(), Panic(ELogWindowStateMachineNavigationError1));
		break;
	case EStateFetchingWindow:
	case EStateProcessingWindow:
	case EStateNavigateWithinWindow:
		// We're trying to catch up with some changes.. use the requested
		// window as the new window, but refetch everything.
		__ASSERT_DEBUG(IsActive(), Panic(ELogWindowStateMachineNavigationError2));
		SilentCancel();
		iWindow = iWindowFetcher->RequestedWindow();
		break;
	default:
		break;
		}

	// Perform boundary checks
	TInt cursorPos = CalculateCursorPosition(aNavigate);
	if	(cursorPos < 0 || cursorPos >= iViewRecordCount)
		{
		// Can't navigate to the specified position
		return EFalse;
		}

	// Check whether the cursor position falls within the view window 
	if	(iWindow.iValid && iWindow.Contains(cursorPos))
		{
		// Can return event from window
		CompleteRequest(cursorPos);
		ChangeState(EStateNavigateWithinWindow);
		}
	else
		{
		// Have to fetch a new window. Work it out...
		TLogWindowAndCursor window;
		CalculateWindowForCursorPosition(cursorPos, window);
		window.iCursorPosition = cursorPos;
		iWindowFetcher->PrepareToFetchWindowL(window);
		//		
		CompleteRequest(KErrNone);
		ChangeState(EStateFetchingWindow);
		}

	// Okay to (try to) navigate here
	Queue(aStatus);
	return ETrue;
	}

void CLogViewWindow::RemoveFromWindowIfPresentL(TLogId aId)
	{
	TInt index = FindEvent(aId);
	if	(index >= 0)
		{
		// Ignore next window removal event (since we're proactively removing
		// the event from the client side without server prompting).
		iWindowChangeObserver->IgnoreNextEventL(aId, CLogViewWindowChangeObserver::ELogEventTypeDelete);

		// Delete item	
		RemoveEvent(index);

		// Map onto full view position
		index += iWindow.iLower;

		// Update window - when removing the last event from the window, we must
		// ensure we mark the window as invalid.
		if	(iWindow.AdjustForItemDeletion(index) == TLogWindowAndCursor::EWindowAffected && iEvents.Count() == 0)
			{
			iWindow.Reset();
			}
		}
	//
	--iViewRecordCount;
	}

MLogViewChangeObserverInternal& CLogViewWindow::ChangeObserver()
	{
	__ASSERT_ALWAYS(iWindowChangeObserver, Panic(ELogWindowNoChangeObserver));
	return *iWindowChangeObserver;
	}

const CLogEvent& CLogViewWindow::CurrsorEvent() const
	{
	// Map the cursor position to fall within the window
	const TInt mappedIndex = iWindow.WindowIndexFromCursorPosition();
	const TInt count = iEvents.Count();
	__ASSERT_ALWAYS(mappedIndex >= 0 && mappedIndex < count, Panic(ELogWindowCursorCalculationOutOfBounds));
	return *iEvents[mappedIndex];
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindow::HandleWindowLockStatusChangeL(TLogViewLockStatus aStatus)
	{
	switch(aStatus)
		{
	case ELogViewWindowLocked:
		SilentCancel();
		iState = EStateLocked;
		break;
	case ELogViewWindowOpen:
		if (iState == EStateLocked)
			iState = EStateIdle; 
		break;
	default:
		break;
		}

	// The window is never valid after a change in lock status
	iWindow.iValid = EFalse;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindow::HandleFetchedWindowItemL(TInt /*aItemIndex*/, CLogEvent* aEvent)
	{
	// IMPROVEMENT: could use aItemIndex as the insertion point?
	User::LeaveIfError(iEvents.Append(aEvent));
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindow::HandleLogViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	///////////////////////////////////////
	// USE CASE 1:
	///////////////////////////////////////
	// Cursor position:  *
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item X is added => 
	// 
	// Cursor position:    *
	// View Index:       0 1 2 3 4 5 6 7
	// View Contents:    X A B C D E F G
	// 
	///////////////////////////////////////
	// USE CASE 2:
	///////////////////////////////////////
	// Cursor position:              *
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item X is added => 
	// 
	// Cursor position:                *
	// View Index:       0 1 2 3 4 5 6 7
	// View Contents:    X A B C D E F G
	// 
	///////////////////////////////////////
	// USE CASE 3:
	///////////////////////////////////////
	// Cursor position:    *          
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item X is added => 
	// 
	// Cursor position:      *          
	// View Index:       0 1 2 3 4 5 6 7
	// View Contents:    X A B C D E F G
	// 
	///////////////////////////////////////
	// USE CASE 4:
	///////////////////////////////////////
	// Cursor position:    *          
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, change item Z so that it now appears in the view
	// 
	// Cursor position:    *          
	// View Index:       0 1 2 3 4 5 6 7
	// View Contents:    A B C Z D E F G
	// 
	///////////////////////////////////////
	// USE CASE 5:
	///////////////////////////////////////
	// Cursor position:        *      
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, change item Z so that it now appears in the view
	// 
	// Cursor position:          *   
	// View Index:       0 1 2 3 4 5 6 7
	// View Contents:    A B C Z D E F G
	// 
	///////////////////////////////////////
	//RDebug::Print(_L("CLogViewWindow::HandleLogViewChangeEventAddedL()   - aId: %3d, aViewIndex: %3d, aChangeIndex: %3d, aTotalChangeCount: %d, cursorPos: %3d, window: (%3d, %3d), viewCount: %3d, iState: %d"), aId, aViewIndex, aChangeIndex, aTotalChangeCount, iWindow.iCursorPosition, iWindow.iLower, iWindow.iUpper, iViewRecordCount, iState);

	// Must compare against the current window (if this object is idle) or against
	// the window fetcher's window if it is in the process of making a fetch
	TLogWindowAndCursor currentWindow;
	switch(iState)
		{
	case EStateFetchingWindow:
	case EStateProcessingWindow:
		currentWindow = iWindowFetcher->RequestedWindow();
		break;
	case EStateNavigateWithinWindow:
	case EStateIdle:
	case EStateLocked:
		currentWindow = iWindow;
		break;
		}

	// If the addition took place after the window, then we do nothing (except update the
	// total view record count)
	if	(aViewIndex <= currentWindow.iUpper)
		{
		// If the addition took place before our window, or at the very start, then we simply need to adjust 
		// the window & cursor position. If the window fetcher was active then this will affect the window
		// being fetched - in this case, we MUST refetch the whole window.
		if	(iState == EStateIdle && aViewIndex <= currentWindow.iLower)
			{
			iWindow.AdjustForItemAddition(aViewIndex);
			}
		else
			{
			// If the addition took place within the window, then we have to refetch the window
			TBool refetch = ETrue;
			TLogWindowAndCursor newWindow;
			//
			switch(iState)
				{
			case EStateIdle:
				newWindow = iWindow;
				break;
			case EStateNavigateWithinWindow:
				// Since we complete our own request status with the new cursor position, 
				// we can use that here as the desired cursor position after the fetch
				newWindow = iWindow;
				newWindow.iCursorPosition = iStatus.Int();
				break;
			case EStateFetchingWindow:
			case EStateProcessingWindow:
				newWindow = iWindowFetcher->RequestedWindow();
				break;
			case EStateLocked:
				// Don't need to do anything. When the view is unlocked, we refetch anyway
				refetch = EFalse;
				break;
			default:
				break;
				}
			//
			if	(refetch)
				{
				newWindow.AdjustForItemAddition(aViewIndex);
				RefetchL(newWindow, newWindow.iCursorPosition);
				}
			}
		}

	// Increase the total view size
	++iViewRecordCount;

	if	(iCascadeObserver)
		iCascadeObserver->HandleLogViewChangeEventAddedL(aId, aViewIndex, aChangeIndex, aTotalChangeCount);
	}

void CLogViewWindow::HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	//RDebug::Print(_L("CLogViewWindow::HandleLogViewChangeEventChangedL() - aId: %3d, aViewIndex: %3d, aChangeIndex: %3d, aTotalChangeCount: %d, cursorPos: %3d, window: (%3d, %3d), viewCount: %3d, iState: %d"), aId, aViewIndex, aChangeIndex, aTotalChangeCount, iWindow.iCursorPosition, iWindow.iLower, iWindow.iUpper, iViewRecordCount, iState);

	// Must compare against the current window (if this object is idle) or against
	// the window fetcher's window if it is in the process of making a fetch
	TLogWindowAndCursor currentWindow;
	switch(iState)
		{
	case EStateFetchingWindow:
	case EStateProcessingWindow:
		currentWindow = iWindowFetcher->RequestedWindow();
		break;
	case EStateNavigateWithinWindow:
	case EStateIdle:
	case EStateLocked:
		currentWindow = iWindow;
		break;
	default:
		break;
		}

	// If the event that changed was within the view, then we have to refetch it
	if	(currentWindow.Contains(aViewIndex))
		{
		switch(iState)
			{
		case EStateIdle:
			RefetchL(iWindow, iWindow.iCursorPosition);
			break;
		case EStateNavigateWithinWindow:
			// Since we complete our own request status with the new cursor position, 
			// we can use that here as the desired cursor position after the fetch
			RefetchL(iWindow, iStatus.Int());
			break;
		case EStateFetchingWindow:
		case EStateProcessingWindow:
			RefetchL(iWindowFetcher->RequestedWindow(), iWindowFetcher->RequestedWindow().iCursorPosition);
			break;
		case EStateLocked:
			// Don't need to do anything. When the view is unlocked, we refetch anyway
			break;
		default:
			break;
			}
		}

	if	(iCascadeObserver)
		iCascadeObserver->HandleLogViewChangeEventChangedL(aId, aViewIndex, aChangeIndex, aTotalChangeCount);
	}

void CLogViewWindow::HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	///////////////////////////////////////
	// USE CASE 1:
	///////////////////////////////////////
	// Cursor position:            *
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item 5 is deleted => 
	// 
	// Cursor position:            *
	// View Index:       0 1 2 3 4 5
	// View Contents:    A B C D E G
	// 
	///////////////////////////////////////
	// USE CASE 2:
	///////////////////////////////////////
	// Cursor position:            *
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item 4 is deleted => 
	// 
	// Cursor position:          *
	// View Index:       0 1 2 3 4 5
	// View Contents:    A B C D F G
	// 
	///////////////////////////////////////
	// USE CASE 3:
	///////////////////////////////////////
	// Cursor position:              *
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item 6 is deleted => 
	// 
	// Cursor position:            *
	// View Index:       0 1 2 3 4 5
	// View Contents:    A B C D E F
	// 
	///////////////////////////////////////
	// USE CASE 4:
	///////////////////////////////////////
	// Cursor position:  *            
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item 6 is deleted => 
	// 
	// Cursor position:  *
	// View Index:       0 1 2 3 4 5
	// View Contents:    B C D E F G
	// 
	///////////////////////////////////////
	//RDebug::Print(_L("CLogViewWindow::HandleLogViewChangeEventDeletedL() - aId: %3d, aViewIndex: %3d, aChangeIndex: %3d, aTotalChangeCount: %d, cursorPos: %3d, window: (%3d, %3d), viewCount: %3d, iState: %d"), aId, aViewIndex, aChangeIndex, aTotalChangeCount, iWindow.iCursorPosition, iWindow.iLower, iWindow.iUpper, iViewRecordCount, iState);

	// Must compare against the current window (if this object is idle) or against
	// the window fetcher's window if it is in the process of making a fetch
	TLogWindowAndCursor currentWindow;
	switch(iState)
		{
	case EStateFetchingWindow:
	case EStateProcessingWindow:
		currentWindow = iWindowFetcher->RequestedWindow();
		break;
	case EStateNavigateWithinWindow:
	case EStateIdle:
	case EStateLocked:
		currentWindow = iWindow;
		break;
	default:
		break;
		}

	// Does the change alter our current window in any way?
	if	(aViewIndex <= currentWindow.iUpper)
		{
		TBool refetch = ETrue;
		TLogWindowAndCursor newWindow;
		//
		switch(iState)
			{
		case EStateIdle:
			FindAndRemoveEvent(aId);

			// When removing the last event from the window, we must ensure we mark
			// the window as invalid.
			if	(iWindow.AdjustForItemDeletion(aViewIndex) == TLogWindowAndCursor::EWindowAffected && iEvents.Count() == 0)
				{
				iWindow.Reset();
				}

			refetch = EFalse;
			break;
		case EStateNavigateWithinWindow:
			newWindow = iWindow;
			newWindow.iCursorPosition = iStatus.Int();
			break;
		case EStateFetchingWindow:
		case EStateProcessingWindow:
			newWindow = iWindowFetcher->RequestedWindow();
			break;
		case EStateLocked:
			// Don't need to do anything. When the view is unlocked, we refetch anyway
			refetch = EFalse;
			break;
		default:
			break;
			}
		//
		if	(refetch)
			{
			newWindow.AdjustForItemDeletion(aViewIndex);
			RefetchL(newWindow, newWindow.iCursorPosition);
			}
		}

	// Reduce the total view size
	--iViewRecordCount;

	if	(iCascadeObserver)
		iCascadeObserver->HandleLogViewChangeEventDeletedL(aId, aViewIndex, aChangeIndex, aTotalChangeCount);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindow::HandleLogViewChangeEventLogClearedL()
	{
	Cancel();
	Reset();

	// This event type is not cascaded to the client of the log engine
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindow::DoRunL()
	{
	switch(iState)
		{
	case EStateFetchingWindow:
		StateHandleWindowFetchStarting();
		break;
	case EStateProcessingWindow:
		StateHandleWindowFetchedL();
		break;
	case EStateNavigateWithinWindow:
		StateHandleNavigation();
		break;
	case EStateIdle:
	case EStateLocked:
	default:
		break;
		}
	}

void CLogViewWindow::DoCancel()
	{
	if(iState == EStateProcessingWindow)
		{
		iWindowFetcher->Cancel();
		}
	CLogActive::DoCancel();
	}

void CLogViewWindow::DoComplete(TInt& aComplete)
	{
	switch(iState)
		{
	default:
	case EStateIdle:
		break;
	case EStateNavigateWithinWindow:
		if	(aComplete < KErrNone)
			{
			// Reset to known state
			iState = EStateIdle;
			}
		break;
	case EStateFetchingWindow:
	case EStateProcessingWindow:
		if	(aComplete < KErrNone)
			{
			// Reset to known state
			iState = EStateIdle;
			iWindow = iWindowFetcher->RequestedWindow();
			}
		break;
	case EStateLocked:
		aComplete = KErrAccessDenied;
		break;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindow::StateHandleWindowFetchStarting()
	{
	iEvents.ResetAndDestroy();
	iWindowFetcher->Start(iStatus);
	ChangeState(EStateProcessingWindow);
	SetActive();
	}

void CLogViewWindow::StateHandleWindowFetchedL()
	{
	const TInt count = iEvents.Count();
	const TInt expected = iWindowFetcher->RequestedWindow().Range();
	if	(expected != count)
		User::Leave(KErrGeneral);
	//	
	iWindow = iWindowFetcher->RequestedWindow();
	CompleteRequest(iWindow.iCursorPosition);
	ChangeState(EStateNavigateWithinWindow);
	}

void CLogViewWindow::StateHandleNavigation()
	{
	const TInt cursorPos = iStatus.Int();
	__ASSERT_ALWAYS(iWindow.Contains(cursorPos), Panic(ELogWindowNavigationOutsideWindow));
	iWindow.iCursorPosition = cursorPos;
	ChangeState(EStateIdle);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindow::CompleteRequest(TInt aCompletionCode)
	{
	if	(!IsActive() || (IsActive() && iStatus == KRequestPending))
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, aCompletionCode);
		if	(!IsActive())
			SetActive();
		}
	}

void CLogViewWindow::CalculateWindowForCursorPosition(TInt aCursorPosition, TLogWindow& aWindow) const
	{
	aWindow.iLower = Max(0, aCursorPosition - KWindowSlideSize - 1);
	aWindow.iUpper = Min(iViewRecordCount - 1, aWindow.iLower + iWindowSize - 1);
	}

void CLogViewWindow::Reset()
	{
	Cancel();
	//
	iViewRecordCount = 0;
	iWindow.Reset();
	iEvents.ResetAndDestroy();
	iState = EStateIdle;
	}

void CLogViewWindow::SilentCancel()
	{
	iWindowFetcher->SilentCancel();
	}

void CLogViewWindow::RefetchL(const TLogWindow& aWindow, TInt aCursor)
	{
	SilentCancel();
	//
	TLogWindowAndCursor newWindow(aWindow, aCursor);
	iWindowFetcher->PrepareToFetchWindowL(newWindow);
	CompleteRequest(KErrNone);
	ChangeState(EStateFetchingWindow);
	}

void CLogViewWindow::ChangeState(TWindowState aNewState)
	{
	iState = aNewState;
	}

void CLogViewWindow::RemoveEvent(TInt aIndex)
	{
	CLogEvent* event = iEvents[aIndex];
	delete event;
	iEvents.Remove(aIndex);
	}

TInt CLogViewWindow::CalculateCursorPosition(TLogNavigation aNavigate) const
	{
	TInt position = 0;
	switch(aNavigate)
		{
	case ELogNavigateForwards:
		position = iWindow.iCursorPosition + 1;
		break;
	case ELogNavigateBackwards:
		position = iWindow.iCursorPosition - 1;
		break;
	case ELogNavigateFirst:
		position = 0;
		break;
	case ELogNavigateLast:
		position = iViewRecordCount - 1;
		break;
	default:
		break;
		}
	return position;
	}

TInt CLogViewWindow::FindEvent(TLogId aId)
	{
	const TInt count = iEvents.Count();
	for(TInt i=0; i<count; i++)
		{
		CLogEvent* event = iEvents[i];
		if	(event->Id() == aId)
			return i;
		}
	return KErrNotFound;
	}

TInt CLogViewWindow::FindAndRemoveEvent(TLogId aId)
	{
	TInt index = FindEvent(aId);
	if	(index >= 0)
		{
		RemoveEvent(index);
		index += iWindow.iLower;
		}
	return index;
	}
