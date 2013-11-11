// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGWRAP_H__
#define __LOGWRAP_H__

#include <e32base.h>
#include <f32file.h>
#include <barsc2.h> // For CResourceFile
#include <d32dbms.h>
#include <logwrapconst.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS 
	#include <logwraplimits.h>
	#include "logcntdef.h"
#endif

#ifndef __WINC__
#include <logwrap.rsg>
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
/** 
Contact item ID. These are used to uniquely identify contact items within a contacts database.
@publishedAll
@released 
*/
typedef TInt32 TLogContactItemId;

#endif

/**
@publishedAll
@released
*/
typedef TUint32	TLogViewId;


const TLogFlags KLogNullFlags = 0;
const TLogFlags KLogFlagsMask = 0xF;


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS 
/**
@internalAll
*/
const TInt KLogActiveDoNotCompleteIfActive = KMaxTInt-1;
#endif

//SimId typedef. Probably the final version will import it from different header. 
typedef TUint32 TSimId;
//"Null" SimId value.
const TSimId KLogNullSimId = 0;

//**********************************
// CLogActive
//**********************************

/** 
Common active object behaviour.

This is an abstract class that defines common active object behaviour for 
the log engine classes.
@publishedAll
@released
*/
class CLogActive : public CActive
	{
public:
	IMPORT_C ~CLogActive();
	//
protected:
	IMPORT_C CLogActive(TInt aPriority);
	IMPORT_C void Queue(TRequestStatus& aStatus);
	IMPORT_C void Complete(TInt aStatus);
	IMPORT_C void DoCancel();
	//
private:
	//
	IMPORT_C void RunL();
	virtual void DoRunL()=0;
	IMPORT_C virtual void DoComplete(TInt&);
	//
private:
	TRequestStatus* iReport;
	};

//**********************************
// CLogEvent
//**********************************

/** 
Encapsulates the details of an event.

Where time is used, it must be specified as UTC rather than local time.

@publishedAll
@released
*/
class CLogEvent : public CBase
	{
public:
	IMPORT_C static CLogEvent* NewL();
	IMPORT_C ~CLogEvent();
	//
public:
	inline TLogId Id() const;
	inline void SetId(TLogId aId);
	//
	inline TUid EventType() const;
	inline void SetEventType(TUid aId);
	//
	inline const TDesC& RemoteParty() const;
	inline void SetRemoteParty(const TDesC& aRemote);
	//
	inline const TDesC& Direction() const;
	inline void SetDirection(const TDesC& aDirection);
	//
	inline const TTime& Time() const;
	inline void SetTime(const TTime& aTime);
	//
	inline TLogDurationType DurationType() const;
	inline void SetDurationType(TLogDurationType aDurationType);
	//
	inline TLogDuration Duration() const;
	inline void SetDuration(TLogDuration aDuration);
	//
	inline const TDesC& Status() const;
	inline void SetStatus(const TDesC& aStatus);
	//
	inline const TDesC& Subject() const;
	inline void SetSubject(const TDesC& aSubject);
	//
	inline const TDesC& Number() const;
	inline void SetNumber(const TDesC& aNumber);
	//
	inline TLogContactItemId Contact() const;
	inline void SetContact(TLogContactItemId  aContact);
	//
	inline TLogLink Link() const;
	inline void SetLink(TLogLink aLink);
	//
	inline const TDesC& Description() const;
	inline void SetDescription(const TDesC& aDescription);
	//
	inline TLogFlags Flags() const;
	inline void SetFlags(TLogFlags aFlags);
	inline void ClearFlags(TLogFlags aFlags);
	//
	inline const TDesC8& Data() const;
	IMPORT_C void SetDataL(RReadStream& aStream, TInt aLen);
	IMPORT_C void SetDataL(const TDesC8& aData);
	//
	IMPORT_C void CopyL(const CLogEvent& aEvent);

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	//
	IMPORT_C void SetSimId(TSimId aSimId);
	IMPORT_C TSimId SimId() const;
	//
private:
	CLogEvent();
	void ConstructL();
	void InternalizeBufL(RReadStream& aStream, HBufC*& aDes);
	//
private:
	TLogId iId;
	TUid iEventType;
	TTime iTime;
	TLogDurationType iDurationType;
	TLogDuration iDuration;
	TLogContactItemId  iContact;
	TLogLink iLink;
	TLogFlags iFlags;
	HBufC* iDescription;
	HBufC* iRemoteParty;
	HBufC* iDirection;
	HBufC* iStatus;
	HBufC* iSubject;
	HBufC* iNumber;
	HBufC8* iData;
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TSimId	iSimId;
#endif	
	};

//**********************************
// CLogBase
//**********************************

/** 
Base class for the log engine implementation.

This class defines the interface that client code uses to perform simple actions 
on the log database.

The class provides default implementations for the event handling functions. 
The Log Engine, i.e. an instance of CLogClient, is derived from this class 
and provides implementations for the event handling functions.

The class also deals with some aspects of resource file handling.

An instance of this class is constructed by the log wrapper, CLogWrapper, 
if the phone does not implement logging.

An instance of this class is never constructed by clients.

@see CLogClient
@see CLogWrapper
@publishedAll
@released
*/
class CLogBase : public CLogActive
	{
public:
	IMPORT_C CLogBase(TInt aPriority);
	IMPORT_C ~CLogBase();
	//
	IMPORT_C virtual void GetEvent(CLogEvent& aEvent, TRequestStatus& aStatus);
	IMPORT_C virtual void AddEvent(CLogEvent& aEvent, TRequestStatus& aStatus);
	IMPORT_C virtual void ChangeEvent(const CLogEvent& aEvent, TRequestStatus& aStatus);
	IMPORT_C virtual void DeleteEvent(TLogId aId, TRequestStatus& aStatus);
	//
	IMPORT_C virtual TInt GetString(TDes& aString, TInt aId) const;
	//
protected:
	IMPORT_C void LoadResourcesL(RFs& aFs);
	inline const CResourceFile* ResourceFile() const;
	//
private:
	void DoRunL();
	//
private:
	CResourceFile* iResourceFile;
	
	//
private:
	IMPORT_C virtual void CLogBase_Reserved1();
	TAny* CLogBase_Reserved;
	};

//**********************************
// CLogWrapper
//**********************************

/** 
The log wrapper.

As some UI variants do not use a Log Engine, the log wrapper provides a consistent 
interface to clients that need to work, without being changed, with multiple 
UI variants.

The log wrapper is provided in its own DLL, the logwrap.dll.

If a Log Engine is not installed on a phone, the log wrapper creates an instance 
of the CLogBase class, whose event handling requests complete with KErrNotSupported 
and whose functions return KErrNotSupported.

If a Log Engine is installed on a phone, the log wrapper creates an instance 
of the CLogClient class, provided in a separate DLL, the logcli.dll . The 
CLogClient class derives from CLogBase and provides implementations for the 
event handling requests and functions.

@see CLogBase
@see CLogClient
@publishedAll
@released
*/
class CLogWrapper : public CBase
	{
public:
	IMPORT_C static CLogWrapper* NewL(RFs& aFs, TInt aPriority = CActive::EPriorityStandard);
	IMPORT_C ~CLogWrapper();
	//
	inline CLogBase& Log();
	IMPORT_C TBool ClientAvailable() const;
	//
private:
	CLogWrapper();
	void ConstructL(RFs& aFs, TInt aPriority);
	//
private:
	CLogBase* iBase;
	};

#include <logwrap.inl>
#include <logeng.h>

#endif
