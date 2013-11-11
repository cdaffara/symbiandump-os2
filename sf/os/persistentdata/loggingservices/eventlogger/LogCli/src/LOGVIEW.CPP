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

#include <logview.h>

// User includes
#include <logwrap.h>
#include <logcli.h>
#include "logclipanic.h"
#include "logservcli.h"
#include "logpackage.h"
#include "logclientop.h"
#include "LogViewObserver.h"
#include "LogViewWindow.h"

// Constants
const TInt KLogDefaultWindowSize = 10;

//**********************************
// CLogView
//**********************************

CLogView::CLogView(CLogClient& aClient, TInt aPriority/* = CActive:EPriorityStandard*/)
: CLogActive(aPriority), iClient(aClient)
	{
	// Get the view id
	iViewId = iClient.Session().AllocateIdView();
	}

EXPORT_C CLogView::~CLogView()
/** Frees all resources owned by this object prior to its destruction. In particular, 
any outstanding asynchronous request is cancelled. */
	{
	Cancel();

	delete iEvent;
	delete iPackage;
	delete iMaintain;
	delete iLogViewObserver;
	delete iWindow;

	// Delete the view
	iClient.Session().Send(ELogViewDelete, TIpcArgs(iViewId)); // Not much we can do if this goes wrong
	}

void CLogView::ConstructL(TInt aType, MLogViewChangeObserver* aObserver)
	{
	iType = aType;
	iLogViewChangeObserver = aObserver;
	iPackage = CLogPackage::NewL();
	iEvent = CLogEvent::NewL();
	iMaintain = new(ELeave)CLogMaintainClientOp(iClient.Session(), *iPackage, Priority());

	PrepareViewChildrenL();
	}

void CLogView::NotifyLogServerTerminatedL()
	{
	PrepareViewChildrenL();
	}

void CLogView::PrepareViewChildrenL()
	{
	// Construct the view in the server
	TIpcArgs aArgs;
	aArgs.Set(0, iViewId);
	aArgs.Set(1, iType);

	User::LeaveIfError(iClient.Session().Send(ELogViewCreate, aArgs));
	CLogViewWindow* window = new(ELeave)
		CLogViewWindow(iClient.Session(), iViewId, KLogDefaultWindowSize,
		iLogViewChangeObserver, CActive::EPriorityIdle);
	
	delete iWindow;
	iWindow = window;
	iWindow->ConstructL(*iPackage);

	// The Log view observer receives all events from the log server. It then cascades them to an object owned by
	// the log window. In turn this object (CLogViewWindowChangeObserver) cascades them back to the log window.
	// Finally, the log window passes them back up to the client of the log engine, i.e aObserver.
	CLogViewObserver* observer = CLogViewObserver::NewL(*this, iClient, iWindow->ChangeObserver(), iViewId, Priority());
	delete iLogViewObserver;
	iLogViewObserver = observer;
	}

EXPORT_C TBool CLogView::FirstL(TRequestStatus& aStatus)
/** Moves the current position in the view to the first event. The first event 
is the most recent event.

This is an asynchronous request.

@param aStatus The request status. On request completion, contains:KErrNone, 
if the position in the view has been successfully moved; otherwise, one of 
the other system wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. */
	{
	return NavigateL(ELogNavigateFirst, aStatus);
	}

EXPORT_C TBool CLogView::LastL(TRequestStatus& aStatus)
/** Moves the current position in the view to the last event. The last event is 
the oldest event.

This is an asynchronous request.

@param aStatus The request status. On request completion, contains:KErrNone, 
if the position in the view has been successfully moved; otherwise, one of 
the other system wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. */
	{
	return NavigateL(ELogNavigateLast, aStatus);
	}

EXPORT_C TBool CLogView::NextL(TRequestStatus& aStatus)
/** Moves the current position in the view to the next event. The next event is 
always older than the current event, i.e. next implies movement in the first 
to last direction.

@param aStatus The request status. On request completion, contains:KErrNone, 
if the position in the view has been successfully moved; otherwise, one of 
the other system wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. */
	{
	return NavigateL(ELogNavigateForwards, aStatus);
	}

EXPORT_C TBool CLogView::PreviousL(TRequestStatus& aStatus)
/** Moves the current position in the view to the previous event. The previous 
event is always more recent than the current event, i.e. previous implies 
movement in the last to first direction.

@param aStatus The request status. On request completion, contains:KErrNone, 
if the position in the view has been successfully moved; otherwise, one of 
the other system wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. */
	{
	return NavigateL(ELogNavigateBackwards, aStatus);
	}

EXPORT_C TInt CLogView::CountL()
/** Gets the number of events in the view.

@return The number of events in the view. */
	{
	// Just return zero if the view isn't setup
	if	(!IsValid() && !LogViewRecordCount())
		{
		return 0;
		}

	// Ask the server the number of events in this view
	const TInt count = iClient.Session().Send(ELogViewCount, TIpcArgs(iViewId));

	User::LeaveIfError(count);
	return count;
	}

EXPORT_C void CLogView::SetFlagsL(TLogFlags aFlags)
/**
@capability Note For built-in event types, the required capability level is defined in
the event type's write access policy. */
	{
	// To preserve the same server side interface as an operation
	TPckgBuf<TLogClientServerData> data;
	data().iOperationType = ELogOperationViewSetFlags;
	data().iOperationId = KLogNullOperationId;
	//
	User::LeaveIfError(iClient.Session().Send(ELogViewOperationInitiate, TIpcArgs(&data,iViewId,aFlags)));
	}

TBool CLogView::NavigateL(TInt aPosition, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(ELogAlreadyActive1));
	if (!IsValid() && !LogViewRecordCount())
		{
		return EFalse;
		}
	//
	const TLogNavigation navigationPosition = static_cast<TLogNavigation>(aPosition);
	const TBool requestIssued = iWindow->NavigateL(navigationPosition, iStatus);
	//
	if	(requestIssued)
		{
		Queue(aStatus);
		SetActive();
		// iValid is false if at time of SetFilterL there is no event.
		// If iWindow->NavigateL returns ETrue then there are always
		// events in the view.
		iValid = ETrue;
		}
	//
	return requestIssued;
	}

void CLogView::DoRunL()
	{
	if	(iStatus.Int() == KErrNone)
		ReadEventFromWindowL();
	}

void CLogView::DoCancel()
	{
	iWindow->Cancel();
	CLogActive::DoCancel();
	}

void CLogView::ReadEventFromWindowL()
	{
	CLogEvent* event = CLogEvent::NewL();
	delete iEvent;
	iEvent = event;
	iEvent->CopyL(iWindow->CurrsorEvent());
	}

void CLogView::ReawaitForChanges()
	{
	iLogViewObserver->Cancel();
	iLogViewObserver->RequestChanges();
	}

//
// The CLogViewWindow member has an observer to receive changes
// in events. If events are added to database after SetFilterL then
// iWindow would know about them.
TInt CLogView::LogViewRecordCount() const
	{
	if (iWindow)
		{
		return iWindow->ViewRecordCount();
		}
	return 0;
	}
















//**********************************
// CLogViewEvent
//**********************************

CLogViewEvent::CLogViewEvent(CLogClient& aClient, TInt aPriority)
: CLogView(aClient, aPriority)
	{
	}

EXPORT_C CLogViewEvent::~CLogViewEvent()
/** Frees all resources owned by the object prior to its destruction. In particular, 
any outstanding asynchronous request is cancelled */
	{
	Cancel();
	}

void CLogViewEvent::ConstructL(MLogViewChangeObserver* aObserver)
	{
	CLogView::ConstructL(ELogViewTypeEvent, aObserver);
	}

EXPORT_C CLogViewEvent* CLogViewEvent::NewL(CLogClient& aClient, TInt aPriority)
	{
	CLogViewEvent* self = new(ELeave)CLogViewEvent(aClient, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CLogViewEvent* CLogViewEvent::NewL(CLogClient& aClient, MLogViewChangeObserver& aObserver, TInt aPriority)
	{
	CLogViewEvent* self = new(ELeave)CLogViewEvent(aClient, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(&aObserver);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C TBool CLogViewEvent::SetFilterL(const CLogFilter& aFilter, TRequestStatus& aStatus)
/** Initialises or refreshes the event view defined by the specified filter.

The view can only be used after the request completes successfully.

@param aFilter The filter. 
@param aStatus The request status. On request completion, contains:KErrNone, 
if the view has been successfully initialised or refreshed;one of the other 
system wide error codes, otherwise. 
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability Note None required.*/
	{
	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);
	list->AppendL(&aFilter);
	TBool result = SetFilterL(*list, aStatus);
	CleanupStack::PopAndDestroy(); // list
	return result;
	}

EXPORT_C TBool CLogViewEvent::SetFilterL(const CLogFilterList& aFilterList, TRequestStatus& aStatus)
/** Initialises or refreshes the event view defined by the set of specified filters.

The view can only be used after the request completes successfully.

@param aFilterList The set of filters. 
@param aStatus The request status. On request completion, contains:KErrNone, 
if the view has been successfully initialised or refreshed;one of the other 
system wide error codes, otherwise. 
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability Note None required.  */
	{
	// Start maintenance of the database - ignore errors
	iMaintain->Start();

	// Package the parameters
	iPackage->SetLogFilterListL(aFilterList);

	// Setup the view
	const TInt count = iWindow->Setup(aFilterList, 0, ELogFilterConstructFilterByFilterFieldByField);
	User::LeaveIfError(count);
	iValid = count;
	ReawaitForChanges();

	if(count)
		{
		// Get the required event
		iValid = FirstL(aStatus);
		return iValid;
		}
	return EFalse;
	}

EXPORT_C TBool CLogViewEvent::SetFilterParseFilterByFilterL(const CLogFilterList& aFilterList, TRequestStatus& aStatus)
/**
@capability Note None required.  */
	{
	// Start maintenance of the database - ignore errors
	iMaintain->Start();

	// Package the parameters
	iPackage->SetLogFilterListL(aFilterList);

	// Setup the view
	const TInt count = iWindow->Setup(aFilterList, 0, ELogFilterConstructFieldByFieldFilterByFilter);
	User::LeaveIfError(count);
	iValid = count;
	ReawaitForChanges();

	if(count)
		{
		// Get the required event
		iValid = FirstL(aStatus);
		return iValid;
		}
	return EFalse;
	}























//**********************************
// CLogViewRecent
//**********************************

CLogViewRecent::CLogViewRecent(CLogClient& aClient, TInt aPriority)
: CLogView(aClient, aPriority)
	{
	}

EXPORT_C CLogViewRecent::~CLogViewRecent()
/** Frees resources owned by the object priot to its destruction. */
	{
	Cancel();
	delete iRemove;
	}

void CLogViewRecent::ConstructL(MLogViewChangeObserver* aObserver)
	{
	// Construct the view
	CLogView::ConstructL(ELogViewTypeRecent, aObserver);

	iRemove = new(ELeave) CLogViewRemoveEventClientOp(iClient.Session(), *iPackage, Priority());
	}

EXPORT_C CLogViewRecent* CLogViewRecent::NewL(CLogClient& aClient, TInt aPriority)
	{
	CLogViewRecent* self = new(ELeave) CLogViewRecent(aClient, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CLogViewRecent* CLogViewRecent::NewL(CLogClient& aClient, MLogViewChangeObserver& aObserver, TInt aPriority)
	{
	CLogViewRecent* self = new(ELeave) CLogViewRecent(aClient, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(&aObserver);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C TBool CLogViewRecent::SetRecentListL(TLogRecentList aList, TRequestStatus& aStatus)
/** Initialises or refreshes the view for the specified recent event list. This 
is an asynchronous request.

On successful completion, the view is positioned at the first, i.e. most recent, 
event within the recent event list.

@param aList The recent event list specifier. A value of KLogNullRecentList 
indicates that the view is to include events from all of the recent event 
lists.
@param aStatus The request status. On request completion, contains:KErrNone, 
if the view has been successfully initialised or refreshed; otherwise, one 
of the other system wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability Note None required.  */
	{
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	TBool result = SetRecentListL(aList, *filter, aStatus);
	CleanupStack::PopAndDestroy(); // filter
	return result;
	}

EXPORT_C TBool CLogViewRecent::SetRecentListL(TLogRecentList aList, const CLogFilter& aFilter, TRequestStatus& aStatus)
/** Initialises or refreshes the view for the specified recent event list, conforming 
to the specified filter. This is an asynchronous request.

On successful completion, the view is positioned at the first, i.e. most recent, 
event in the recent event list.

@param aList The recent event list specifier. A value of KLogNullRecentList 
indicates that the view is to include events from all of the recent event 
lists.
@param aFilter The filter.
@param aStatus The request status. On request completion, contains:KErrNone, 
if the view has been successfully initialised or refreshed; otherwise, one 
of the other system wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability Note None required.  */
	{
	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);
	list->AppendL(&aFilter);
	TBool result = SetRecentListL(aList, *list, aStatus);
	CleanupStack::PopAndDestroy(); // list
	return result;
	}

EXPORT_C TBool CLogViewRecent::SetRecentListL(TLogRecentList aList, const CLogFilterList& aFilterList, TRequestStatus& aStatus)
/** Initialises or refreshes the view for the specified recent event list, conforming 
to the set of specified filters. This is an asynchronous request.

On successful completion, the view is positioned at the first, i.e. most recent, 
event in the recent event list.

@param aList The recent event list specifier. A value of KLogNullRecentList 
indicates that the view is to include events from all of the recent event 
lists.
@param aFilterList The set of filters.
@param aStatus The request status. On request completion, contains:KErrNone, 
if the view has been successfully initialised or refreshed; otherwise, one 
of the other system wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability Note None required.  */
	{
	// Start maintenance of the database - ignore errors
	iMaintain->Start();

	// Package the parameters
	iPackage->SetLogFilterListL(aFilterList);

	// Setup the view
	TInt count = iWindow->Setup(aFilterList, aList, ELogFilterConstructFilterByFilterFieldByField);
	User::LeaveIfError(count);
	iValid = count;
	ReawaitForChanges();

	// Initialise list ids
	iRecentList = aList;
	iCurrentList = aList;

	// This receives the current recent list id from the server
	iCurrentListBuf() = iCurrentList;
	iData = &iCurrentListBuf;

	if(count)
		{
		// Get the required event
		iValid = FirstL(aStatus);
		return iValid;
		}
	return EFalse;
	}

EXPORT_C void CLogViewRecent::RemoveL(TLogId aId)
/** Removes the event with the specified unique event ID from the view. This does 
not delete the event from the main event log.

@param aId The unique event ID. 
@capability WriteDeviceData  */
	{
	User::LeaveIfError(iRemove->Start(iViewId, aId));
	iWindow->RemoveFromWindowIfPresentL(aId);
	}

EXPORT_C TBool CLogViewRecent::RemoveL(TRequestStatus& aStatus)
/** Removes the current event from its recent event list. This is an asynchronous 
request.

This does not delete the event from the main event log.

The function moves the current position in the view to the first, i.e. most 
recent, event.

Note that removing a recent event from a recent event list also removes all 
of its duplicates.

@param aStatus The request status. On request completion, contains:KErrNone, 
if the view has been successfully initialised or refreshed; otherwise, one 
of the other system wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability WriteDeviceData  */
	{
	__ASSERT_DEBUG(IsValid(), Panic(ELogNotValid1));
	RemoveL(Event().Id());
	return FirstL(aStatus);
	}

EXPORT_C TBool CLogViewRecent::DuplicatesL(CLogViewDuplicate& aView, TRequestStatus& aStatus)
/** Refreshes the specified duplicate event view with the duplicates of the current 
event in the recent event list view. This is an asynchronous request.

On successful completion, the view is positioned at the first, i.e. most recent, 
event within the view.

@param aView The duplicates view to be refreshed.
@param aStatus The request status. On request completion, contains:KErrNone, 
if the view has been successfully refreshed; otherwise, one of the other system 
wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability Note None required.  */
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(ELogAlreadyActive5));
	return IsValid() && aView.SetEventL(Event().Id(), aStatus);
	}

EXPORT_C TBool CLogViewRecent::DuplicatesL(CLogViewDuplicate& aView, const CLogFilter& aFilter, TRequestStatus& aStatus)
/** Refreshes the specified duplicate event view with the duplicates of the current 
event in the recent event list view and conforming to the specified filter. 
This is an asynchronous request.

On successful completion, the view is positioned at the first, i.e. most recent, 
event within the view.

@param aView The duplicates view to be refreshed.
@param aFilter The filter.
@param aStatus The request status. On request completion, contains:KErrNone, 
if the view has been successfully refreshed; otherwise, one of the other system 
wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability Note None required.*/
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(ELogAlreadyActive6));
	return IsValid() && aView.SetEventL(Event().Id(), aFilter, aStatus);
	}

EXPORT_C TBool CLogViewRecent::DuplicatesL(CLogViewDuplicate& aView, const CLogFilterList& aFilterList, TRequestStatus& aStatus)
/** Refreshes the specified duplicate event view with the duplicates of the current 
event in the recent event list view and conforming to the set of specified 
filters. This is an asynchronous request.

On successful completion, the view is positioned at the first, i.e. most recent, 
event within the view.

@param aView The duplicates view to be refreshed.
@param aFilterList The set of filters.
@param aStatus The request status. On request completion, contains:KErrNone, 
if the view has been successfully refreshed; otherwise, one of the other system 
wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability Note None required.*/
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(ELogAlreadyActive7));
	return IsValid() && aView.SetEventL(Event().Id(), aFilterList, aStatus);
	}

EXPORT_C void CLogViewRecent::ClearDuplicatesL()
/**
@capability WriteDeviceData  */
	{
	__ASSERT_DEBUG(IsValid(), Panic(ELogNotValid3));

	// To preserve the same server side interface as an operation
	TPckgBuf<TLogClientServerData> data;
	data().iOperationType = ELogOperationViewClearDuplicates;
	data().iOperationId = KLogNullOperationId;
	//
	User::LeaveIfError(iClient.Session().Send(ELogViewOperationInitiate, TIpcArgs(&data,iViewId)));
	}

void CLogViewRecent::DoRunL()
	{
	// To fetch the event
	ReadEventFromWindowL();

	// A fix to maintain source compatibility
	iCurrentList = iCurrentListBuf();
	}


















//**********************************
// CLogViewDuplicate
//**********************************

CLogViewDuplicate::CLogViewDuplicate(CLogClient& aClient, TInt aPriority)
: CLogView(aClient, aPriority)
	{
	}

EXPORT_C CLogViewDuplicate::~CLogViewDuplicate()
	{
	Cancel();
	delete iRemove;
	}

void CLogViewDuplicate::ConstructL(MLogViewChangeObserver* aObserver)
	{
	CLogView::ConstructL(ELogViewTypeDuplicate, aObserver);

	iRemove = new(ELeave) CLogViewRemoveEventClientOp(iClient.Session(), *iPackage, Priority());
	}

EXPORT_C CLogViewDuplicate* CLogViewDuplicate::NewL(CLogClient& aClient, TInt aPriority)
	{
	CLogViewDuplicate* self = new(ELeave)CLogViewDuplicate(aClient, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CLogViewDuplicate* CLogViewDuplicate::NewL(CLogClient& aClient, MLogViewChangeObserver& aObserver, TInt aPriority)
	{
	CLogViewDuplicate* self = new(ELeave) CLogViewDuplicate(aClient, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(&aObserver);
	CleanupStack::Pop(self);
	return self;
	}

TBool CLogViewDuplicate::SetEventL(TLogId aId, TRequestStatus& aStatus)
	{
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	TBool result = SetEventL(aId, *filter, aStatus);
	CleanupStack::PopAndDestroy(); // filter
	return result;
	}

TBool CLogViewDuplicate::SetEventL(TLogId aId, const CLogFilter& aFilter, TRequestStatus& aStatus)
	{
	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);
	list->AppendL(&aFilter);
	TBool result = SetEventL(aId, *list, aStatus);
	CleanupStack::PopAndDestroy(); // list
	return result;
	}

TBool CLogViewDuplicate::SetEventL(TLogId aId, const CLogFilterList& aFilterList, TRequestStatus& aStatus)
	{
	// Start maintenance of the database - ignore errors
	iMaintain->Start();

	// Package the parameters
	iPackage->SetLogFilterListL(aFilterList);

	// Setup the view
	TInt count = iWindow->Setup(aFilterList, aId, ELogFilterConstructFilterByFilterFieldByField);
	User::LeaveIfError(count);
	iValid = count;
	iSourceId = aId;
	ReawaitForChanges();

	if(count)
		{
		// Get the required event
		iValid = FirstL(aStatus);
		return iValid;
		}
	return EFalse;
	}

EXPORT_C void CLogViewDuplicate::RemoveL(TLogId aId)
/** Removes the event with the specified unique event ID from the view. This does 
not delete the event from the main event log.

@param aId The unique event ID. 
@capability WriteDeviceData  */
	{
	// Note: Duplicate views reset themselves
	__ASSERT_DEBUG(IsValid(), Panic(ELogNotValid2));
	User::LeaveIfError(iRemove->Start(iViewId, aId));
	iWindow->RemoveFromWindowIfPresentL(aId);
	}

EXPORT_C TBool CLogViewDuplicate::RemoveL(TRequestStatus& aStatus)
/** Removes the current event from the duplicate list. This is an asynchronous 
request.

This does not delete the event from the main event log.

The function moves the current position in the view to the first, i.e. most 
recent, event.

@param aStatus The request status. On request completion, contains: KErrNone, 
if the view has been successfully initialised or refreshed; otherwise, one 
of the other system wide error codes.
@return ETrue, if the function has successfully issued the asynchronous request. 
EFalse, if there are no events in the view. 
@capability WriteDeviceData  */
	{
	RemoveL(Event().Id());
	return FirstL(aStatus);
	}
