// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGCLI_H__
#define __LOGCLI_H__

// System includes
#include <f32file.h>
#include <d32dbms.h>
#include <logwrap.h>

// Classes referenced
class CLogClientObserver;
class MLogClientChangeObserver;


//**********************************
// CLogEventType
//**********************************

/** Encapsulates the details of an event type.

An event type is used to associate an event with a user-readable description 
and other configuration information related to the event.

Event types are identified by UID.

@see CLogEvent::EventType()
@see CLogEvent::SetEventType()
@publishedAll 
@released
*/
class CLogEventType : public CBase
	{
public:
	IMPORT_C static CLogEventType* NewL();
	IMPORT_C ~CLogEventType();
	//
public:
	inline TUid Uid() const;
	inline void SetUid(TUid aUid);
	//
	inline const TDesC& Description() const;
	inline void SetDescription(const TDesC& aDescription);
	//
	inline TBool LoggingEnabled() const;
	inline void SetLoggingEnabled(TBool aEnable);
	//
	IMPORT_C void Copy(const CLogEventType& aType);

	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	//
private:
	CLogEventType();
	void ConstructL();
	void InternalizeBufL(RReadStream& aStream, HBufC*& aDes);
	//
private:
	TUid iUid;
	HBufC* iDescription;
	TBool iLoggingEnabled;
	};

//**********************************
// TLogConfig
//**********************************

/** Encapsulates Log Engine configuration data.

@see CLogClient::GetConfig()
@see CLogClient::ChangeConfig()
@publishedAll
@released
*/
class TLogConfig
	{
public:
	IMPORT_C TLogConfig();
	//
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	//
public:
	/** The maximum number of events that the event log holds.
	
	If the number of event log entries exceeds this value, the oldest event is 
	deleted from the log. */
	TLogSize iMaxLogSize;
	
	/** The maximum number of events that a recent event list holds.
	
	If the number of recent event list entries exceeds this value, the oldest 
	event is removed from the list. Note, however, that the event is not deleted 
	from the event log. */
	TLogRecentSize iMaxRecentLogSize;
	
	/** The maximum age, in seconds, for which events can be retained in the event log.
	
	Events older than this are automatically deleted from the event log. 
	
	Purging of events by age can be disabled by setting this value to 0. */
	TLogAge iMaxEventAge;
	};

//**********************************
// CLogFilter
//**********************************

/** Specifies the conditions that events must satisfy to appear in a view.

In general, a filter is used to construct the WHERE clause of an SQL 
statement based on the content of the filter's fields. The filter's fields 
correspond to the event properties in a log event detail object, i.e. an instance 
of the CLogEvent class.

Where a filter uses time to extract one or more events from the log, this must be 
specified as UTC rather than local time.

@see CLogEvent
@see CLogViewEvent
@see CLogViewRecent
@see CLogViewDuplicate
@publishedAll
@released
*/
class CLogFilter : public CBase
	{
public:
	IMPORT_C static CLogFilter* NewL();
	IMPORT_C ~CLogFilter();
	//
public:
	inline TUid EventType() const;
	inline void SetEventType(TUid aType);
	//
	inline const TDesC& RemoteParty() const;
	inline void SetRemoteParty(const TDesC& aRemote);
	//
	inline const TDesC& Direction() const;
	inline void SetDirection(const TDesC& aDirection);
	//
	inline TLogDurationType DurationType() const;
	inline void SetDurationType(TLogDurationType aType);
	//
	inline const TDesC& Status() const;
	inline void SetStatus(const TDesC& aStatus);
	//
	inline TLogContactItemId  Contact() const;
	inline void SetContact(TLogContactItemId  aContact);
	//
	inline const TDesC& Number() const;
	inline void SetNumber(const TDesC& aNumber);
	//
	inline TLogFlags Flags() const;
	inline void SetFlags(TLogFlags aFlags);
	inline void ClearFlags(TLogFlags aFlags);
	//
	inline TUint32 NullFields() const;
	inline void SetNullFields(TUint32 aFields);
	//
	inline const TTime& StartTime() const;
	inline void SetStartTime(const TTime& aStartTime);	
	//
	inline const TTime& EndTime() const;
	inline void SetEndTime(const TTime& aEndTime);
	//
	IMPORT_C void Copy(const CLogFilter& aFilter);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	//
	IMPORT_C void SetSimId(TSimId aSimId);
	IMPORT_C TSimId SimId() const;
	//
private:
	CLogFilter();
	void ConstructL();
	void InternalizeBufL(RReadStream& aStream, HBufC*& aDes);
	//
private:
	TUid iEventType;
	HBufC* iRemoteParty;
	HBufC* iDirection;
	TLogDurationType iDurationType;
	HBufC* iStatus;
	TLogContactItemId  iContact;
	HBufC* iNumber;
	TLogFlags iFlags;
	TUint32 iNullFields;
	TTime iStartTime;
	TTime iEndTime;
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
	TSimId iSimId;
#endif
	};

/** A set of event view filters.

Specifically, this is an array of pointers to const CLogFilter objects and 
is derived from CArrayPtrFlat<const CLogFilter>.

@see CLogFilter
@see CLogViewEvent
@see CLogViewRecent
@publishedAll
@released
*/
class CLogFilterList : public CArrayPtrFlat<const CLogFilter>
	{
public:
	IMPORT_C CLogFilterList();
	IMPORT_C CLogFilterList* CopyL() const;
	IMPORT_C CLogFilterList* CopyLC() const;
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	};

//**********************************
// CLogClient
//**********************************

class CLogView;
class RLogSession;
class CLogPackage;
class CLogAddEventClientOp;
class CLogChangeEventClientOp;
class CLogGetEventClientOp;
class CLogDeleteEventClientOp;
class CLogAddTypeClientOp;
class CLogChangeTypeClientOp;
class CLogGetTypeClientOp;
class CLogDeleteTypeClientOp;
class CLogGetConfigClientOp;
class CLogChangeConfigClientOp;
class CLogClearLogClientOp;
class CLogClearRecentClientOp;

/** Log Engine implementation.

It creates a shared session on the log database and allows log viewers to 
retrieve events from the log.

The class also provides log administration functions.

Wherever time values are used by this class, they must be specified as UTC rather
than local time. 

@see CLogWrapper
@publishedAll
@released
*/
class CLogClient : public CLogBase
	{
public:
	IMPORT_C static CLogClient* NewL(RFs& aFs, TInt aPriority = CActive::EPriorityStandard);
	IMPORT_C ~CLogClient();
	//
public:
	IMPORT_C void AddEvent(CLogEvent& aEvent, TRequestStatus& aStatus);
	IMPORT_C void GetEvent(CLogEvent& aEvent, TRequestStatus& aStatus);
	IMPORT_C void ChangeEvent(const CLogEvent& aEvent, TRequestStatus& aStatus);
	IMPORT_C void DeleteEvent(TLogId aId, TRequestStatus& aStatus);

	IMPORT_C void AddEventType(const CLogEventType& aType, TRequestStatus& aStatus);
	IMPORT_C void GetEventType(CLogEventType& aType, TRequestStatus& aStatus);
	IMPORT_C void ChangeEventType(const CLogEventType& aType, TRequestStatus& aStatus);
	IMPORT_C void DeleteEventType(TUid aId, TRequestStatus& aStatus);
	IMPORT_C void GetConfig(TLogConfig& aConfig, TRequestStatus& aStatus);
	IMPORT_C void ChangeConfig(const TLogConfig& aConfig, TRequestStatus& aStatus);
	IMPORT_C void ClearLog(const TTime& aDate, TRequestStatus& aStatus);
	IMPORT_C void ClearLog(TInt aRecentList, TRequestStatus& aStatus);
	IMPORT_C void ClearLog(const TTime& aDate, TSimId aSimid, TRequestStatus& aStatus);
	IMPORT_C void ClearLog(TInt aRecentList, TSimId aSimid, TRequestStatus& aStatus);
	//
	IMPORT_C void NotifyChange(TTimeIntervalMicroSeconds32 aDelay, TRequestStatus& aStatus);
	IMPORT_C void NotifyChangeCancel();
	//
	IMPORT_C void SetGlobalChangeObserverL(MLogClientChangeObserver* aObserver);
	//
	IMPORT_C TInt GetString(TDes& aString, TInt aId) const;
	RLogSession& Session() const;
	//
private:
	CLogClient(RFs& aFs, TInt aPriority);
	void ConstructL();
	void DoGetStringL(TDes& aString, TInt aId) const;
	//
	void DoRunL();
	void DoCancel();
	//
private:
	RLogSession* iSession;
	CLogPackage* iPackage;
	CLogAddEventClientOp* iAddEvent;
	CLogChangeEventClientOp* iChangeEvent;
	CLogGetEventClientOp* iGetEvent;
	CLogDeleteEventClientOp* iDeleteEvent;
	CLogAddTypeClientOp* iAddType;
	CLogChangeTypeClientOp* iChangeType;
	CLogGetTypeClientOp* iGetType;
	CLogDeleteTypeClientOp* iDeleteType;
	CLogGetConfigClientOp* iGetConfig;
	CLogChangeConfigClientOp* iChangeConfig;
	CLogClearLogClientOp* iClearLog;
	CLogClearRecentClientOp* iClearRecent;
	RFs& iFs;
	CLogClientObserver* iChangeObserver;
	//
private:
	IMPORT_C void CLogBase_Reserved1();
	};

#include <logcli.inl>

#endif
