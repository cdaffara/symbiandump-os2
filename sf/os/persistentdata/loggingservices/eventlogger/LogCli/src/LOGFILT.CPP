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

#include <logcli.h>
#include <logwraplimits.h>
#include "LogCliServShared.h"
#include "logcntdef.h"
#include "logclipanic.h"

const TInt KLogFilterListGranuality = 10;

//**********************************
// CLogFilter
//**********************************

/** Creates a new filter object.

All fields in the new instance are initialised to default values, so that 
if none of the fields are changed, the filter has no effect on the selection 
of events in view.

@return A pointer to the new event object. */
EXPORT_C CLogFilter* CLogFilter::NewL()
	{
	CLogFilter* self = new(ELeave)CLogFilter;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CLogFilter::CLogFilter()
: iDurationType(KLogNullDurationType), iContact(KLogNullContactId), iFlags(KLogNullFlags),
iStartTime(TTime(0)), iEndTime(TTime(0))
	{
	}

void CLogFilter::ConstructL()
	{
	iRemoteParty = HBufC::NewL(KLogMaxRemotePartyLength);
	iDirection = HBufC::NewL(KLogMaxDirectionLength);
	iStatus = HBufC::NewL(KLogMaxStatusLength);
	iNumber = HBufC::NewL(KLogMaxNumberLength);
	}

/** Frees all resource owned by the object prior to its destruction. */
EXPORT_C CLogFilter::~CLogFilter()
	{
	delete iRemoteParty;
	delete iDirection;
	delete iStatus;
	delete iNumber;
	}

/** Makes a copy of a filter.

@param aFilter The filter object to be copied. */
EXPORT_C void CLogFilter::Copy(const CLogFilter& aFilter)
	{
	SetEventType(aFilter.EventType());
	SetDurationType(aFilter.DurationType());
	SetContact(aFilter.Contact());
	SetRemoteParty(aFilter.RemoteParty());
	SetDirection(aFilter.Direction());
	SetStatus(aFilter.Status());
	SetNumber(aFilter.Number());
	SetNullFields(aFilter.NullFields());

	ClearFlags(KLogFlagsMask);
	SetFlags(aFilter.Flags());
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
	SetSimId(aFilter.SimId());
#endif
	}

void CLogFilter::InternalizeL(RReadStream& aStream)
	{
	TBuf<KLogMaxDateLength> buf;
	aStream >> iEventType;
	aStream >> iDurationType;
	aStream >> iContact;
	InternalizeBufL(aStream, iRemoteParty);
	InternalizeBufL(aStream, iDirection);
	InternalizeBufL(aStream, iStatus);
	InternalizeBufL(aStream, iNumber);
	aStream >> iNullFields;
	aStream >> iFlags;
	aStream >> buf;
	iStartTime.Parse(buf);
	buf.FillZ();
	aStream >> buf;
	iEndTime.Parse(buf);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
	aStream >> iSimId;
#endif
	}

void CLogFilter::InternalizeBufL(RReadStream& aStream, HBufC*& aDes)
	{
	TPtr ptr(aDes->Des());
	HBufC* temp = HBufC::NewL(aStream, ptr.MaxLength());
	ptr.Zero();
	ptr.Copy(*temp);
	delete temp;
	}

void CLogFilter::ExternalizeL(RWriteStream& aStream) const
	{
	TBuf<KLogMaxDateLength> buf;  
	aStream << iEventType;
	aStream << iDurationType;
	aStream << iContact;
	aStream << *iRemoteParty;
	aStream << *iDirection;
	aStream << *iStatus;
	aStream << *iNumber;
	aStream << iNullFields;
	aStream << iFlags;
	if (iStartTime != TTime(0))
		iStartTime.FormatL(buf, LogUtils::DateFormatForLocale());
	aStream << buf;
	buf.FillZ();
	if (iEndTime != TTime(0))
		iEndTime.FormatL(buf, LogUtils::DateFormatForLocale());
	aStream << buf;
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
	aStream << iSimId;
#endif
	}

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM

/**
Sets the short Id of the SIM card that will be used by the filter.

@param aSimId SIM card short Id;  
*/
EXPORT_C void CLogFilter::SetSimId(TSimId aSimId)
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is defined
	iSimId = aSimId;
	}

/**
Returns the short Id of the SIM card that is used by the filter.

@return SIM card short Id;  
*/
EXPORT_C TSimId CLogFilter::SimId() const
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is defined
	return iSimId;
	}

#else//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM

#pragma BullseyeCoverage off

/**
Not supported. 
*/
EXPORT_C void CLogFilter::SetSimId(TSimId)
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is not defined
	__ASSERT_ALWAYS(0, ::Panic(ELogDualSimNotSupported));
	}

/**
Not supported. 
*/
EXPORT_C TSimId CLogFilter::SimId() const
	{//Compiled when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM is not defined
	__ASSERT_ALWAYS(0, ::Panic(ELogDualSimNotSupported));
	return 0;
	}

#pragma BullseyeCoverage on

#endif//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM

//**********************************
// CLogFilterList
//**********************************

/** Constructs a flat array of pointers to const CLogFilter objects. */
EXPORT_C CLogFilterList::CLogFilterList()
: CArrayPtrFlat<const CLogFilter>(KLogFilterListGranuality)
	{
	}

/** Creates a copy of this set of event view filters.

@return A pointer to the new copy of the set of event view filters. */
EXPORT_C CLogFilterList* CLogFilterList::CopyL() const
	{
	CLogFilterList* newList = CopyLC();
	CleanupStack::Pop();
	return newList;
	}

/** Creates a copy of this set of event view filters and puts a pointer to the 
copy onto the cleanup stack.

@return A pointer to the new copy of the set of event view filters. */
EXPORT_C CLogFilterList* CLogFilterList::CopyLC() const
	{
	CLogFilterList* newList = new(ELeave)CLogFilterList;
	CleanupStack::PushL(newList);
	if (Count())
		{
		newList->ResizeL(Count(), NULL);
		Mem::Copy(newList->Back(0), Back(0), Count() * sizeof(const CLogFilter*));	
		}
	return newList;
	}

void CLogFilterList::InternalizeL(RReadStream& aStream)
	{
	ResetAndDestroy();
	TInt count;
	count = aStream.ReadInt32L();
	while(count--)
		{
		CLogFilter* filter = CLogFilter::NewL();
		CleanupStack::PushL(filter);
		filter->InternalizeL(aStream);
		AppendL(filter); 
		CleanupStack::Pop(filter);
		}
	}

void CLogFilterList::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(Count());	
	for(TInt index = 0; index < Count(); index++)
		{
		At(index)->ExternalizeL(aStream);
		}
	}
