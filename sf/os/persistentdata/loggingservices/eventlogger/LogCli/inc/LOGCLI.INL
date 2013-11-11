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
// CLogEventType
// 
//

/** Gets the UID identifying the event type.

@return The UID. */
inline TUid CLogEventType::Uid() const
	{
	return iUid;
	}

/** Sets the UID identifying the event type.

@param aUid The Uid. */
inline void CLogEventType::SetUid(TUid aUid)
	{
	iUid = aUid;
	}

/** Gets the human readable and localised name of the event type.

@return A non-modifiable descriptor containing the name. */
inline const TDesC& CLogEventType::Description() const
	{
	return *iDescription;
	}

/** Sets the human readable and localised name of the event type.

For example, Email.

@param aDescription The name. */
inline void CLogEventType::SetDescription(const TDesC& aDescription)
	{
	iDescription->Des().Copy(aDescription);
	}

/** Determines whether logging for this event type is enabled.

@return ETrue, if logging is enabled. EFalse, if logging is disabled. */
inline TBool CLogEventType::LoggingEnabled() const
	{
	return iLoggingEnabled;
	}

/** Enables or disables logging for this event type.

Note that disabling the logging of an event type causes a CLogClient::AddEventL() 
request for an event of that type to complete with an appropriate error.

@param aEnable ETrue, to enable logging for this event type.EFalse, to disable 
logging for this event type.
@see CLogClient::AddEvent() */
inline void CLogEventType::SetLoggingEnabled(TBool aEnable)
	{
	iLoggingEnabled = aEnable;
	}

//**********************************
// CLogFilter
//**********************************

/** Gets the event type used by the filter.

@return The event type UID.
@see CLogEvent::EventType() */
inline TUid CLogFilter::EventType() const
	{
	return iEventType;
	}

/** Sets the event type to be used by the filter.

@param aType The event type UID.
@see CLogEvent::SetEventType() */
inline void CLogFilter::SetEventType(TUid aType)
	{
	iEventType = aType;
	}

/** Gets the remote party string used by the filter.

@return A non-modifiable descriptor containing the remote party string.
@see CLogEvent::RemoteParty() */
inline const TDesC& CLogFilter::RemoteParty() const
	{
	return *iRemoteParty;
	}

/** Sets the remote party string to be used by the filter.

@param aRemote The remote party string.
@see CLogEvent::SetRemoteParty() */
inline void CLogFilter::SetRemoteParty(const TDesC& aRemote)
	{
	iRemoteParty->Des().Copy(aRemote);
	}

/** Gets the direction string used by the filter.

@return A non-modifiable descriptor containing the direction string.
@see CLogEvent::Direction() */
inline const TDesC& CLogFilter::Direction() const
	{
	return *iDirection;
	}

/** Sets the direction string to be used by the filter.

@param aDirection The direction string.
@see CLogEvent::SetDirection() */
inline void CLogFilter::SetDirection(const TDesC& aDirection)
	{
	iDirection->Des().Copy(aDirection);
	}

/** Gets the duration type used by the filter.

@return The duration type.
@see CLogEvent::DurationType() */
inline TLogDurationType CLogFilter::DurationType() const
	{
	return iDurationType;
	}

/** Sets the duration type to be used by the filter.

@param aType The duration type.
@see CLogEvent::SetDurationType() */
inline void CLogFilter::SetDurationType(TLogDurationType aType)
	{
	iDurationType = aType;
	}

/** Gets the delivery status used by the filter.

@return A non-modifiable descriptor containing the delivery status.
@see CLogEvent::Status() */
inline const TDesC& CLogFilter::Status() const
	{
	return *iStatus;
	}

/** Sets the delivery status to be used by the filter.

@param aStatus The delivery status.
@see CLogEvent::SetStatus() */
inline void CLogFilter::SetStatus(const TDesC& aStatus)
	{
	iStatus->Des().Copy(aStatus);
	}

/** Gets the contact ID used by the filter.

@return The contact ID.
@see CLogEvent::Contact() */
inline TLogContactItemId  CLogFilter::Contact() const
	{
	return iContact;
	}

/** Sets the contact ID to be used by the filter.

@param aContact The contact ID.
@see CLogEvent::SetContact() */
inline void CLogFilter::SetContact(TLogContactItemId  aContact)
	{
	iContact = aContact;
	}

/** Gets the phone number used by the filter.

@return A non-modifiable descriptor containing the phone number.
@see CLogEvent::Number() */
inline const TDesC& CLogFilter::Number() const
	{
	return *iNumber;
	}

/** Sets the phone number to be used by the filter.

@param aNumber The phone number.
@see CLogEvent::SetNumber() */
inline void CLogFilter::SetNumber(const TDesC& aNumber)
	{
	iNumber->Des().Copy(aNumber);
	}

/** Identifies the fields defined as NULL in the filter. These are the fields that 
the filter expects to be NULL in the database. A NULL field is one that has 
no value.

@return Bit values defining the null fields.
@see SetNullFields() */
inline TUint32 CLogFilter::NullFields() const
	{
	return iNullFields;
	}

/** Defines NULL fields to the filter. These are the fields that the filter expects 
to be NULL in the database. A NULL field is one that has no value.

Each field is identified by a bit value. The bit values may be ORd together 
in any combination.
 
Note that setting a flag and explicitly setting a value for the corresponding 
field generates an OR condition in the SQL WHERE clause.

@param aFields Bit values defining the null fields. These are one or more 
of the following values defined in logwrap.hrh: ELogContactField, ELogDirectionField, 
ELogDurationTypeField, ELogEventTypeField, ELogNumberField, ELogRemotePartyField, 
ELogStatusField, ELogFlagsField, ELogSubjectField, ELogLinkField, ELogDataField, ELogSimIdField. */
inline void CLogFilter::SetNullFields(TUint32 aFields)
	{
	iNullFields = aFields;
	}

/** Gets the UTC start time used by the filter.

@return The UTC start time. */
inline const TTime& CLogFilter::StartTime() const
	{
	return iStartTime;
	}

/** Sets the specified UTC start time to be used by the filter.

@param aStartTime The UTC start time to be set. */
inline void CLogFilter::SetStartTime(const TTime& aStartTime)
	{
	iStartTime = aStartTime; 
	}
	
/** Gets the UTC end time used by the filter.

@return The UTC end time. */
inline const TTime& CLogFilter::EndTime() const
	{
	return iEndTime;
	}

/** Sets the specified UTC end time to be used by the filter.

@param aEndTime The UTC end time to be set. */
inline void CLogFilter::SetEndTime(const TTime& aEndTime)
	{
	iEndTime = aEndTime;
	}

/** Gets the flags used by the filter.

@return The flags. */
inline TLogFlags CLogFilter::Flags() const
	{
	return iFlags;
	}

/** Sets the specified flags to be used by the filter.

@param aFlags The flags to be set. */
inline void CLogFilter::SetFlags(TLogFlags aFlags)
	{
	__ASSERT_DEBUG(aFlags <= KLogFlagsMask, User::Invariant());
	iFlags = (TLogFlags)(iFlags | aFlags);
	}

/** Clears the specified flags to be used by the filter. 

The constant KLogFlagsMask can be used to clear all the flags.

@param aFlags The flags to be cleared */
inline void CLogFilter::ClearFlags(TLogFlags aFlags)
	{
	__ASSERT_DEBUG(aFlags <= KLogFlagsMask, User::Invariant());
	iFlags = (TLogFlags)(iFlags & ~aFlags);
	}
