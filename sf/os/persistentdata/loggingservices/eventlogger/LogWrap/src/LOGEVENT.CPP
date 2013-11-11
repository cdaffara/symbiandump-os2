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

#include <logwrap.h>
#include <logwraplimits.h>
#include "logcntdef.h"
#include "s32strm.h"
#include "LOGPANIC.H"

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

/**
This is the current version number of the event data, after the introduction of the "dual SIM support". 
New event properties in this version:
 - TInt    iVersion;
 - TSimId	iSimId;
 
The data versioning was introduced together with the "dual SIM support" changes where a new data member was
added to the CLogEvent class. 
The idea is to keep InternalizeL()/ExternalizeL() functionality binary compatibly in case of the unlikely event
CLogEvent data gets persisted to a file for example.

The old format of the externalized event data is:
 <id><type>.....<data length>[<optional: data>]
 
The new format of the externalized event data is:
 <id><type>.....<uint: data version><data length>[<optional: data>]<simId>
 
In case if CLogEvent::InternalizeL() reads from a stream with old event data, then:
1) The data vesrion is read first as an integer. 
2) If the data version is negative then this is an event data with "dual SIM support",
   because the version number of that is positive integer above the KMaxTInt constant
   (interpreted as negative one if read as an integer from the stream)
3) If the data version is positive then this is an event data without "dual SIM support".   
 
@internalComponent 
*/
const TUint KLogEventDataVersion = 0x80000002U;

#endif//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

EXPORT_C CLogEvent* CLogEvent::NewL()
/** Creates a new log event detail object.

@return Pointer to the new log event detail object. */
	{
	CLogEvent* self = new(ELeave)CLogEvent;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CLogEvent::CLogEvent() :
	iId(KLogNullId), 
	iEventType(KNullUid), 
	iDurationType(KLogNullDurationType), 
	iDuration(KLogNullDuration), 
	iContact(KLogNullContactId), 
	iLink(KLogNullLink), 
	iFlags(KLogNullFlags)
	{
	}

void CLogEvent::ConstructL()
	{
	iDescription = HBufC::NewL(KLogMaxDescriptionLength);
	iRemoteParty = HBufC::NewL(KLogMaxRemotePartyLength);
	iDirection = HBufC::NewL(KLogMaxDirectionLength);
	iStatus = HBufC::NewL(KLogMaxStatusLength);
	iSubject = HBufC::NewL(KLogMaxSubjectLength);
	iNumber = HBufC::NewL(KLogMaxNumberLength);
	}

EXPORT_C CLogEvent::~CLogEvent()
/** Frees all resources owned by the log event detail object. */
	{
	delete iDescription;
	delete iRemoteParty;
	delete iDirection;
	delete iStatus;
	delete iSubject;
	delete iNumber;
	delete iData;
	}

EXPORT_C void CLogEvent::SetDataL(const TDesC8& aData)
/** Sets event specific data.

The data can be used for any purpose. The data is copied into a heap descriptor 
allocated by this function; the amount of data is only limited by the available 
memory.

@param aData The event specific data. */
	{
	if (iData)
		{
		if (iData->Des().MaxLength() < aData.Length())
			iData = iData->ReAllocL(aData.Length());
		}
	else
		{
		if (aData.Length() > 0)
			iData = HBufC8::NewL(aData.Length());
		else
			return;
		}

	iData->Des().Copy(aData);
	}

EXPORT_C void CLogEvent::SetDataL(RReadStream& aStream, TInt aLen)
/** Sets event specific data from the specified stream.

The data can be used for any purpose. The data is copied into a heap descriptor 
allocated by this function; the amount of data is only limited by the available 
memory.

@param aStream The stream containing the event specific data.
@param aLen The length of data to be read from the stream. */
	{
	HBufC8* buf = NULL;

	if (aLen > 0)
		{
		buf = HBufC8::NewLC(aLen);
		buf->Des().SetLength(aLen);

		TPtr8 ptr(buf->Des());
		aStream.ReadL(ptr, aLen);

		CleanupStack::Pop(); // buf
		}

	delete iData;
	iData = buf;
	}

EXPORT_C void CLogEvent::CopyL(const CLogEvent& aEvent)
/** Makes a copy of the specified log event.

@param aEvent The log event to be copied. */
	{
	// Set data first as this is the only function that can leave
	// If this function fails nothing will be changed
	SetDataL(aEvent.Data());

	SetId(aEvent.Id());
	SetEventType(aEvent.EventType());
	SetTime(aEvent.Time());
	SetDurationType(aEvent.DurationType());
	SetDuration(aEvent.Duration());
	SetContact(aEvent.Contact());
	SetLink(aEvent.Link());
	SetDescription(aEvent.Description());
	SetRemoteParty(aEvent.RemoteParty());
	SetDirection(aEvent.Direction());
	SetStatus(aEvent.Status());
	SetSubject(aEvent.Subject());
	SetNumber(aEvent.Number());

	ClearFlags(KLogFlagsMask);
	SetFlags(aEvent.Flags());
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
	SetSimId(aEvent.SimId());
#endif
	}

EXPORT_C void CLogEvent::InternalizeL(RReadStream& aStream)
	{
	iId = (TLogId) aStream.ReadInt32L();
	iEventType.iUid = aStream.ReadInt32L();

	TInt64 time; 
	aStream >> time;
	iTime = time;

	iDurationType = (TLogDurationType) aStream.ReadInt8L();
	iDuration = (TLogDuration) aStream.ReadUint32L();
	iContact = (TLogContactItemId ) aStream.ReadInt32L();
	iLink = (TLogLink) aStream.ReadUint32L();
	iFlags = (TLogFlags) aStream.ReadUint8L();

	InternalizeBufL(aStream, iDescription);
	InternalizeBufL(aStream, iRemoteParty);
	InternalizeBufL(aStream, iDirection);
	InternalizeBufL(aStream, iStatus);
	InternalizeBufL(aStream, iSubject);
	InternalizeBufL(aStream, iNumber);

	TInt dataLen = aStream.ReadInt32L();
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
	TUint dataVersion = 0;
	if(dataLen < 0)
		{
		//This is CLogEvent data version with dual SIM support.
		//The next property is the data length.
		dataVersion = (TUint)dataLen;
		dataLen = aStream.ReadInt32L();
		}
#endif
	HBufC8* temp = NULL;
	if(dataLen != 0)
		{
		temp = HBufC8::NewL(aStream, dataLen);
		}
	delete iData;
	iData = temp;
	
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
	if(dataVersion >= KLogEventDataVersion)
		{
		aStream >> iSimId;
		}
#endif
	}

void CLogEvent::InternalizeBufL(RReadStream& aStream, HBufC*& aDes)
	{
	TPtr ptr(aDes->Des());
	HBufC* temp = HBufC::NewL(aStream, ptr.MaxLength());
	ptr.Copy(*temp);
	delete temp;
	}

EXPORT_C void CLogEvent::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iId);
	aStream.WriteInt32L(iEventType.iUid);
	aStream << iTime.Int64();
	aStream.WriteInt8L(iDurationType);
	aStream.WriteUint32L(iDuration);
	aStream.WriteInt32L(iContact);
	aStream.WriteUint32L(iLink);
	aStream.WriteInt8L(iFlags);
	aStream << *iDescription;
	aStream << *iRemoteParty;
	aStream << *iDirection;
	aStream << *iStatus;
	aStream << *iSubject; 
	aStream << *iNumber;

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
	aStream.WriteUint32L(KLogEventDataVersion);
#endif
	
	TInt dataLength = 0;
	if(iData)
		{
		dataLength = iData->Length();
		}
	aStream.WriteInt32L(dataLength);
	if	(iData && dataLength)
		{
		aStream << *iData;
		}
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
	aStream << iSimId;
#endif
	}

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

/**
Sets the short Id of the SIM card being used.

@param aSimId SIM card short Id;  
*/
EXPORT_C void CLogEvent::SetSimId(TSimId aSimId)
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is defined
	iSimId = aSimId;
	}

/**
Returns the short Id of the SIM card that was used.

@return SIM card short Id;  
*/
EXPORT_C TSimId CLogEvent::SimId() const
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is defined
	return iSimId;
	}

#else//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM

#pragma BullseyeCoverage off

/**
Not supported.
*/
EXPORT_C void CLogEvent::SetSimId(TSimId)
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is not defined
	__ASSERT_ALWAYS(0, ::Panic(ELogDualSimNotSupported));
	}

/**
Not supported.
*/
EXPORT_C TSimId CLogEvent::SimId() const
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is not defined
	__ASSERT_ALWAYS(0, ::Panic(ELogDualSimNotSupported));
	return 0;
	}

#pragma BullseyeCoverage on

#endif//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
