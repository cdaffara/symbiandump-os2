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

// System includes
#include <bautils.h>
#include <barsc2.h> // For CResourceFile
#include <barsread2.h> // For RResourceReader
#include <logwrap.h>

// User includes
#include <logcli.h>
#include "logservcli.h"
#include "LogServShared.h"
#include "logpackage.h"
#include "logclipanic.h"
#include "logclientop.h"
#include "LogClientObserver.h"

//**********************************
// TLogConfig
//**********************************

/** Sets the:

maximum age of events to zero

maximum number of events to appear in the log to zero

maximum number of events to appear in a recent event list to zero */
EXPORT_C TLogConfig::TLogConfig()
: iMaxLogSize(0), iMaxRecentLogSize(0), iMaxEventAge(0)
	{
	}

void TLogConfig::InternalizeL(RReadStream& aStream)
	{
	aStream >> iMaxLogSize;
	aStream >> iMaxRecentLogSize;
	aStream >> iMaxEventAge;
	}

void TLogConfig::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iMaxLogSize;
	aStream << iMaxRecentLogSize;
	aStream << iMaxEventAge;
	}

//**********************************
// CLogClient
//**********************************

EXPORT_C CLogClient* CLogClient::NewL(RFs& aFs, TInt aPriority/* = CActive::EPriorityStandard*/)
	{
	CLogClient* self = new(ELeave)CLogClient(aFs, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

void CLogClient::ConstructL()
	{
	// Load resources
	LoadResourcesL(iFs);

	iSession = new(ELeave)RLogSession;
	User::LeaveIfError(iSession->Connect());
	iPackage = CLogPackage::NewL();

	iAddEvent = new(ELeave)CLogAddEventClientOp(*iSession, *iPackage, Priority());
	iChangeEvent = new(ELeave)CLogChangeEventClientOp(*iSession, *iPackage, Priority());
	iGetEvent = new(ELeave)CLogGetEventClientOp(*iSession, *iPackage, Priority());
	iDeleteEvent = new(ELeave)CLogDeleteEventClientOp(*iSession, *iPackage, Priority());
	iAddType = new(ELeave)CLogAddTypeClientOp(*iSession, *iPackage, Priority());
	iChangeType = new(ELeave)CLogChangeTypeClientOp(*iSession, *iPackage, Priority());
	iGetType = new(ELeave)CLogGetTypeClientOp(*iSession, *iPackage, Priority());
	iDeleteType = new(ELeave)CLogDeleteTypeClientOp(*iSession, *iPackage, Priority());
	iGetConfig = new(ELeave)CLogGetConfigClientOp(*iSession, *iPackage, Priority());
	iChangeConfig = new(ELeave)CLogChangeConfigClientOp(*iSession, *iPackage, Priority());
	iClearLog = new(ELeave)CLogClearLogClientOp(*iSession, *iPackage, Priority());
	iClearRecent = new(ELeave)CLogClearRecentClientOp(*iSession, *iPackage, Priority());
	}

/** Frees all resources owned by the Log Engine object prior to its destruction. 
In particular, any outstanding asynchronous request is cancelled, the database, 
the database session and the resource file are all closed. */
EXPORT_C CLogClient::~CLogClient()
	{
	Cancel();

	delete iChangeObserver;
	if (iSession && iSession->Handle())
		{
		// Complete change notification
		NotifyChangeCancel();
		iSession->Close();
		}

	delete iSession;
	delete iPackage;
	delete iAddEvent;
	delete iChangeEvent;
	delete iGetEvent;
	delete iDeleteEvent;
	delete iAddType;
	delete iChangeType;
	delete iGetType;
	delete iDeleteType;
	delete iGetConfig;
	delete iChangeConfig;
	delete iClearLog;
	delete iClearRecent;
	}

CLogClient::CLogClient(RFs& aFs, TInt aPriority)
: CLogBase(aPriority), iFs(aFs)
	{
	}

/** Adds an event to the log database. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

@param aEvent A log event detail object containing the attributes of the event 
to be added. The Log Engine sets the unique event ID, the UTC time and the event 
description, replacing any supplied values. The caller must ensure that this 
object remains in existence and valid until the request is complete.
@param aStatus The request status. On request completion,contains: KErrNone, 
if the event has been successfully added to the log database; KErrNotFound, 
if the event type is not registered with the Log Engine; KErrNotSupported, 
if the logging of events of this type has been disabled; otherwise, one of 
the other system wide error codes.
@capability Note For built-in event types, the required capability level is defined in
the event type's write access policy.
@see CLogEventType::SetLoggingEnabled() */
EXPORT_C void CLogClient::AddEvent(CLogEvent& aEvent, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iAddEvent->Start(aEvent, iStatus);
	SetActive();
	}

/** Changes the details of an existing event. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

Note that it is not possible to change the event type using this function.

@param aEvent The event detail object containing the attributes of the event 
to be changed. Before calling the function, this object must contain the appropriate 
unique event ID; if no unique event ID is set, the function raises a LogCli 
13 panic. The caller must ensure that this object remains in existence and 
valid until the request is complete.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful; otherwise, one of the other system wide error codes.
@capability Note For built-in event types, the required capability level is defined in
the event type's write access policy.
@see TLogId */
EXPORT_C void CLogClient::ChangeEvent(const CLogEvent& aEvent, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iChangeEvent->Start(aEvent, iStatus);
	SetActive();
	}

/** Gets the details of the specified event. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

@param aEvent A reference to a log event detail object. Before calling the 
function, this object must contain the appropriate unique event ID; if no 
unique event ID is set, the function raises a LogServ 50 panic. The caller 
must ensure that this object remains in existence and valid until the request 
is complete. On successful completion of the request, it contains the appropriate 
log event detail.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful; otherwise, one of the other system wide error codes.
@capability Note For built-in event types, the required capability level is defined in
the event type's read access policy.
@see TLogId */
EXPORT_C void CLogClient::GetEvent(CLogEvent& aEvent, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iGetEvent->Start(aEvent, iStatus);
	SetActive();
	}

/** Deletes the event with the specified unique event ID, from the main event log.

@param aId The unique event ID of the event to be deleted. This must not be 
the null unique event ID, KLogNullId, otherwise the function raises a LogCli 
13 panic.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful; otherwise, one of the other system wide error codes. 
@capability Note For built-in event types, the required capability level is defined in
the event type's write access policy.
*/
EXPORT_C void CLogClient::DeleteEvent(TLogId aId, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iDeleteEvent->Start(aId, iStatus);
	SetActive();
	}

/** Registers a new event type. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

@param aType The event type detail object containing the attributes of the 
event type to be registered. The caller must ensure that this object remains 
in existence and valid until the request is complete.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful; otherwise, one of the other system wide error codes.
@capability WriteDeviceData
@see TUid */
EXPORT_C void CLogClient::AddEventType(const CLogEventType& aType, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iAddType->Start(aType, iStatus);
	SetActive();
	}

/** Gets the details of an event type. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

@param aType A reference to an event type detail object. Before calling the 
function, this object must contain the UID identifying the event type; if 
no UID is set, the function raises a LogCli 13 panic. The caller must ensure 
that this object remains in existence and valid until the request is complete. 
On successful completion of the request, it contains the appropriate event 
type detail.
@param aStatus The request status. On request completion, contains: KErrNone, 
if successful; otherwise one of the other system wide error codes.
@capability Note None required.
@see TUid */
EXPORT_C void CLogClient::GetEventType(CLogEventType& aType, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iGetType->Start(aType, iStatus);
	SetActive();
	}

/** Changes the details of an existing event type. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

@param aType The event type detail object containing the attributes of the 
event type to be changed. Before calling the function, this object must contain 
the UID identifying the event type; if no UID is set, the function raises 
a LogCli 13 panic. The caller must ensure that this object remains in existence 
and valid until the request is complete.
@param aStatus The request status. On request completion, contains: KErrNone, 
if successful; otherwise, one of the other system wide error codes.
@capability WriteDeviceData
@see TUid */
EXPORT_C void CLogClient::ChangeEventType(const CLogEventType& aType, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iChangeType->Start(aType, iStatus);
	SetActive();
	}

/** Removes an existing event type. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

Note that this function does not remove events from the event log, so it is 
possible to have events in the log that are of an unknown type. This function 
allows an event type associated with a component to be removed when that component 
is uninstalled.

@param aId The UID of the event type to be deleted.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful; otherwise, one of the other system wide error codes. 
@capability WriteDeviceData
*/
EXPORT_C void CLogClient::DeleteEventType(TUid aId, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iDeleteType->Start(aId, iStatus);
	SetActive();
	}

/** Gets the Log Engine configuration. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

@param aConfig A reference to a Log Engine configuration object. The caller 
must ensure that this object remains in existence and valid until the request 
is complete. On successful completion of the request, it contains the Log 
Engine configuration data.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful; otherwise, one of the other system wide error codes. 
@capability Note None required.
*/
EXPORT_C void CLogClient::GetConfig(TLogConfig& aConfig, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iGetConfig->Start(aConfig, iStatus);
	SetActive();
	}

/** Changes the Log Engine configuration. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

@param aConfig The new configuration values for the Log Engine.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful; otherwise, one of the other system wide error codes. 
@capability WriteDeviceData */
EXPORT_C void CLogClient::ChangeConfig(const TLogConfig& aConfig, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iChangeConfig->Start(aConfig, iStatus);
	SetActive();
	}

/** Clears all events from the main event log that occurred before the specified 
date and time. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

@param aDate The UTC date and time.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful; otherwise, one of the other system wide error codes. 
@capability WriteDeviceData  */
EXPORT_C void CLogClient::ClearLog(const TTime& aDate, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iClearLog->Start(aDate, iStatus);
	SetActive();
	}

/** Clears the specified recent event list. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

@param aRecentList Identifies the recent event list to be cleared. The value 
KlogNullRecentList indicates that all recent event lists are to be cleared.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful; otherwise, one of the other system wide error codes. 
@capability WriteDeviceData  */
EXPORT_C void CLogClient::ClearLog(TInt aRecentList, TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iClearRecent->Start((TLogRecentList)aRecentList, iStatus);
	SetActive();
	}

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

/** 
Clears all events from the main event log that occurred before the specified 
date and time and logged with the supplied SIM short Id. This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

Note: If KLogNullSimId is passed as value of aSimId parameter, then the method will produce the same result as the
           ClearLog() method without SimId parameter - all events occured before  the specified data will be deleted,
	disregarding the SimId event property.

@param aDate   The UTC date and time.
@param aSimId  SIM card short Id.
@param aStatus The request status. On request completion, contains:KErrNone, 
               if successful; otherwise, one of the other system wide error codes. 
@capability WriteDeviceData  
*/
EXPORT_C void CLogClient::ClearLog(const TTime& aDate, TSimId aSimId, TRequestStatus& aStatus)
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is defined
	Queue(aStatus);
	iClearLog->Start(aDate, iStatus, aSimId);
	SetActive();
	}

/** 
Clears the specified recent event list from events with the specified SIM short Id. 
This is an asynchronous request.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

Note: If KLogNullSimId is passed as value of aSimId parameter, then the method will produce the same result as the
           ClearLog() method without SimId parameter - all events from the specified event list will be cleared,
	disregarding the SimId event property.

@param aRecentList Identifies the recent event list to be cleared. The value 
                   KlogNullRecentList indicates that all recent event lists are to be cleared.
@param aSimId      SIM card short Id.
@param aStatus     The request status. On request completion, contains:KErrNone, 
                   if successful; otherwise, one of the other system wide error codes. 
@capability WriteDeviceData  
*/
EXPORT_C void CLogClient::ClearLog(TInt aRecentList, TSimId aSimId, TRequestStatus& aStatus)
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is defined
	Queue(aStatus);
	iClearRecent->Start((TLogRecentList)aRecentList, iStatus, aSimId);
	SetActive();
	}

#else //SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM

#pragma BullseyeCoverage off

/**
Not supported. 
*/
EXPORT_C void CLogClient::ClearLog(const TTime&, TSimId, TRequestStatus&)
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is not defined
	__ASSERT_ALWAYS(0, ::Panic(ELogDualSimNotSupported));
	}

/**
Not supported. 
*/
EXPORT_C void CLogClient::ClearLog(TInt, TSimId, TRequestStatus&)
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is not defined
	__ASSERT_ALWAYS(0, ::Panic(ELogDualSimNotSupported));
	}

#pragma BullseyeCoverage on

#endif//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM

/** Requests notification of changes to the Log Engine database. This is an asynchronous 
request.

The function requires the caller to specify a minimum time that must elapse 
before this notification request can complete. The Log Engine buffers all 
changes that occur during this time; the request, then completes after this 
minimum time period has elapsed. If no changes occur within this time period, 
then the request completes when the next change to the database occurs.

There must be no asynchronous request outstanding when this function is called, 
otherwise the function raises a LogCli 0 panic.

Note that once a notification request has completed, this function must be 
called again to get further change notifications.

@param aDelay The minimum time, in microseconds, that elapses before the notification 
request can complete.
@param aStatus The request status. On request completion, contains:KErrNone, 
if successful;KErrCancel, if an outstanding notification request is cancelled; 
otherwise, one of the other system wide error codes. 
@capability Note None required.
*/
EXPORT_C void CLogClient::NotifyChange(TTimeIntervalMicroSeconds32 aDelay, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;

	iSession->Send(ELogNotify, TIpcArgs(aDelay.Int()), aStatus);
}

/** Cancels any outstanding notification request for changes to Log Engine database.

This function can be called even if there is no outstanding notification request. 
@capability Note None required  */
EXPORT_C void CLogClient::NotifyChangeCancel()
	{
	iSession->Send(ELogNotifyCancel, TIpcArgs());
	}

/**
@capability Note None required
*/
EXPORT_C void CLogClient::SetGlobalChangeObserverL(MLogClientChangeObserver* aObserver)
	{
	delete iChangeObserver;
	iChangeObserver = NULL;
	//
	if	(aObserver)
		{
		iChangeObserver = CLogClientObserver::NewL(*this, *aObserver, Priority());
		}
	}

/** Gets a standard string from the specified resource in logwrap.dll resource 
file.

The function can be used to populate some of the event fields in a CLogEvent 
object before creating or changing an event.

Note that TLogString is a modifiable buffer descriptor that is guaranteed 
to be large enough to contain all standard strings used in the Log Engine; 
pass an instance of this type to this function.

@param aString A modifiable descriptor into which the string is copied.
@param aId The resource id.
@return KErrNone, if successful; otherwise, one of the other system wide error 
codes.
@capability Note None required.
@see TLogString */
EXPORT_C TInt CLogClient::GetString(TDes& aString, TInt aId) const
	{
	aString.Zero();
	TRAPD(err, DoGetStringL(aString, aId));
	return err;
	}

void CLogClient::DoGetStringL(TDes& aString, TInt aId) const
	{
	RResourceReader reader;
#ifdef _DEBUG
	const CResourceFile* rcFile = ResourceFile();
	__ASSERT_DEBUG(rcFile != NULL, Panic(ELogNullRcFile));
#endif
	reader.OpenLC(ResourceFile(), aId);
	aString.Copy(reader.ReadTPtrCL());
	CleanupStack::PopAndDestroy();			// reader
	}

void CLogClient::DoCancel()
	{
	LOGTEXT("CLogClient::DoCancel()");

	iAddEvent->Cancel();
	iChangeEvent->Cancel();
	iGetEvent->Cancel();
	iDeleteEvent->Cancel();
	iAddType->Cancel();
	iChangeType->Cancel();
	iGetType->Cancel();
	iDeleteType->Cancel();
	iGetConfig->Cancel(); 
	iChangeConfig->Cancel();
	iClearLog->Cancel();
	iClearRecent->Cancel();

	CLogBase::DoCancel();
	LOGTEXT("CLogClient::DoCancel() - end");
	}

void CLogClient::DoRunL()
	{
	LOGTEXT2("CLogClient::DoRunL(%d)", iStatus.Int());
	User::LeaveIfError(iStatus.Int());
	LOGTEXT("CLogClient::DoRunL() - end");
	}

RLogSession& CLogClient::Session() const
	{
	return *iSession;
	}

#pragma BullseyeCoverage off

EXPORT_C void CLogClient::CLogBase_Reserved1()
	{
	}

#pragma BullseyeCoverage on
