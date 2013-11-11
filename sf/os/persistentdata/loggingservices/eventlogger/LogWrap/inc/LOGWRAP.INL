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
// CLogEvent
// 
//

inline TLogId CLogEvent::Id() const
/** Gets the unique event ID associated with this log event.

Unique event IDs are allocated by the Log Engine when an event is added to 
the log.

@return The unique event ID.
@see CLogClient::GetEvent()
@see CLogClient::AddEvent()
@see CLogClient::ChangeEvent()
@see CLogClient::DeleteEvent() */
	{
	return iId;
	}

inline void CLogEvent::SetId(TLogId aId)
/** Sets the unique event ID.

@param aId The unique event ID. */
	{
	iId = aId;
	}

inline TUid CLogEvent::EventType() const
/** Gets the type of this log event. Event types are identified by a UID.

@return The event type UID.
@see CLogEvent::Description() */
	{
	return iEventType;
	}

inline void CLogEvent::SetEventType(TUid aId)
/** Sets the type of this log event. Event types are identified by a UID.

@param aId The event type UID.
@see CLogEvent::Description() */
	{
	iEventType = aId;
	}

inline const TDesC& CLogEvent::RemoteParty() const
/** Gets the remote party associated with this event.

@return A reference to a non-modifiable descriptor containing the remote party. */
	{
	return *iRemoteParty;
	}

inline void CLogEvent::SetRemoteParty(const TDesC& aRemote)
/** Sets the remote party associated with this event. This describes the destination 
of an outgoing event or the source of an incoming event.

The remote party is represented by a string.

The string is copied into a pre-allocated heap descriptor that has a maximum 
length of KLogMaxRemoteLength. If the length of the specified descriptor is 
greater than KLogMaxRemoteLength, then the data is truncated.

@param aRemote The remote party. */
	{
	TPtr ptr(iRemoteParty->Des());
	iRemoteParty->Des().Copy(aRemote.Ptr(), Min(aRemote.Length(), ptr.MaxLength()));
	}

inline const TDesC& CLogEvent::Direction() const
/** Gets the direction of the call represented by this event. The direction of 
a call means incoming, outgoing etc.

The direction is represented by a string.

@return A reference to a non-modifiable descriptor containing the string representing 
the direction of the call. */
	{
	return *iDirection;
	}

inline void CLogEvent::SetDirection(const TDesC& aDirection)
/** Sets the direction of the call represented by this event. The direction of 
a call means incoming, outgoing etc.

The direction is represented by a string. Standard strings are available in 
the log wrapper resource file and can be accessed through the resource IDs: 
R_LOG_DIR_IN, R_LOG_DIR_OUT, R_LOG_DIR_IN_ALT, R_LOG_DIR_OUT_ALT, R_LOG_DIR_FETCHED 
and R_LOG_DIR_MISSED.

The string is copied into a pre-allocated heap descriptor that has a maximum 
length of KLogMaxDirectionLength. If the length of the specified descriptor 
is greater than KLogMaxDirectionLength, then the data is truncated.

@param aDirection The readable name.
@see CLogClient::GetString() */
	{
	TPtr ptr(iDirection->Des());
	ptr.Copy(aDirection.Ptr(), Min(aDirection.Length(), ptr.MaxLength()));
	}

inline const TTime& CLogEvent::Time() const
/** Gets the UTC time that this event was created.

@return The time of the event. */
	{
	return iTime;
	}

inline void CLogEvent::SetTime(const TTime& aTime)
/** Sets the UTC time that this event was created.

Note that this field is used when ordering events in a view; changing this 
value may change the position of the event in a view.

@param aTime The UTC time of the event. */
	{
	iTime = aTime;
	}

inline TLogDurationType CLogEvent::DurationType() const
/** Gets the duration type.

This applies meaning to the idea of a duration. 

The duration type is implemented as a UI variant-specific enumeration. The 
following duration types are generic: KLogDurationNone, KLogDurationValid 
and KLogDurationData.

@return The duration type. */
	{
	return iDurationType;
	}

inline void CLogEvent::SetDurationType(TLogDurationType aDurationType)
/** Sets the duration type. This applies meaning to the idea of a duration.

@param aDurationType The duration type. */
	{
	iDurationType = aDurationType;
	}

inline TLogDuration CLogEvent::Duration() const
/** Gets the duration of the event.

@return The duration of the event, expressed as the number of seconds since 
the time of the event. */
	{
	return iDuration;
	}

inline void CLogEvent::SetDuration(TLogDuration aDuration)
/** Sets the duration of the event.

@param aDuration The duration of the event, expressed as the number of seconds 
since the time of the event. */
	{
	iDuration = aDuration;
	}

inline const TDesC& CLogEvent::Status() const
/** Gets the delivery status of this event.

@return A reference to a non-modifiable descriptor containing the delivery 
status. */
	{
	return *iStatus;
	}

inline void CLogEvent::SetStatus(const TDesC& aStatus)
/** Sets the delivery status of this event.

The delivery status is represented by a string. Standard strings are available 
in the log wrapper resource file and can be accessed through the resource 
IDs: R_LOG_DEL_PENDING, R_LOG_DEL_SENT, R_LOG_DEL_FAILED, R_LOG_DEL_NONE, 
R_LOG_DEL_DONE and R_LOG_DEL_NOT_SENT.

The string is copied into a pre-allocated heap descriptor that has a maximum 
length of KLogMaxStatusLength. If the length of the specified descriptor is 
greater than KLogMaxStatusLength, then the data is truncated.

@param aStatus The delivery status.
@see CLogClient::GetString() */
	{
	TPtr ptr(iStatus->Des());
	ptr.Copy(aStatus.Ptr(), Min(aStatus.Length(), ptr.MaxLength()));
	}

inline const TDesC& CLogEvent::Subject() const
/** Gets the subject of this event.

@return A reference to a non-modifiable descriptor containing the subject. */
	{
	return *iSubject;
	}

inline void CLogEvent::SetSubject(const TDesC& aSubject)
/** Sets the subject of this event.

The subject is represented by a string.

The string is copied into a pre-allocated heap descriptor that has a maximum 
length of KLogMaxSubjectLength. If the length of the specified descriptor 
is greater than KLogMaxSubjectLength, then the data is truncated.

@param aSubject The subject. */
	{
	TPtr ptr(iSubject->Des());
	ptr.Copy(aSubject.Ptr(), Min(aSubject.Length(), ptr.MaxLength()));
	}

inline const TDesC& CLogEvent::Number() const
/** Gets the phone number associated with the event.

@return A reference to a non-modifiable descriptor containing the phone number. */
	{
	return *iNumber;
	}

inline void CLogEvent::SetNumber(const TDesC& aNumber)
/** Sets the phone number associated with this event. This is used when the number 
cannot be stored in any other field.

The number is specified as a string and is copied into a pre-allocated heap 
descriptor that has a maximum length of KLogMaxNumberLength. If the length 
of the specified descriptor is greater than KLogMaxNumberLength, then the 
number is truncated.

@param aNumber The number. */
	{
	TPtr ptr(iNumber->Des());
	ptr.Copy(aNumber.Ptr(), Min(aNumber.Length(), ptr.MaxLength()));
	}

inline TLogContactItemId  CLogEvent::Contact() const
/** Gets the contact ID associated with the event.

@return The contact ID. */
	{
	return iContact;
	}

inline void CLogEvent::SetContact(TLogContactItemId  aContact)
/** Sets the contact ID associated with the event.

@param aContact The contact ID. */
	{
	iContact = aContact;
	}

inline TLogLink CLogEvent::Link() const
/** Gets the link value.

@return The link value. */
	{
	return iLink;
	}

inline void CLogEvent::SetLink(TLogLink aLink)
/** Sets the link value.

The link can be used to relate this event to an entity in another application. 
For example, it can be used to associate the event with the call ID or the 
message ID for emails and faxes.

@param aLink The link value. */
	{
	iLink = aLink;
	}

inline const TDesC& CLogEvent::Description() const
/** Gets the human readable name describing the event type.

Note that this is set automatically by the Log Engine.

@return A reference to a non-modifiable descriptor containing the readable 
name. */
	{
	return *iDescription;
	}

inline void CLogEvent::SetDescription(const TDesC& aDescription)
	{
	TPtr ptr(iDescription->Des());
	ptr.Copy(aDescription.Ptr(), Min(aDescription.Length(), ptr.MaxLength()));
	}

inline const TDesC8& CLogEvent::Data() const
/** Gets event specific data.

@return A reference to a non-modifiable descriptor containing the data that 
is specific to the event.KNullDesC8, if there is no data. */
	{
	if (iData)
		return *iData;
	else
		return KNullDesC8;
	}

inline TLogFlags CLogEvent::Flags() const
/** Gets the flags set for this event.

@return The flags. */
	{
	return iFlags;
	}

inline void CLogEvent::SetFlags(TLogFlags aFlags)
/** Sets the specified flags for this event.

The function does not change any of the other flag bit settings.

Only the low order 4 bits are stored in the Log Engine Database; the constant 
KLogFlagsMask can be used as a mask.

@param aFlags The flags to be set. */
	{
	__ASSERT_DEBUG(aFlags <= KLogFlagsMask, User::Invariant());
	iFlags = (TLogFlags)(iFlags | aFlags);
	}

inline void CLogEvent::ClearFlags(TLogFlags aFlags)
/** Clears the specified flags for this event.

The function does not change any of the other flag bit settings.

The constant KLogFlagsMask may be used to clear all the flags in an event.

@param aFlags The flags to be cleared. */
	{
	__ASSERT_DEBUG(aFlags <= KLogFlagsMask, User::Invariant());
	iFlags = (TLogFlags)(iFlags & ~aFlags); 
	}

//**********************************
// CLogBase
//**********************************

inline const CResourceFile* CLogBase::ResourceFile() const
	{
	return iResourceFile;
	}

inline CLogBase& CLogWrapper::Log()
/** Gets a reference to the Log Engine object.

@return If the Log Engine is installed, this is a reference to a CLogClient 
object. If there is no Log Engine, this is a reference to an instance of the 
Log Engine base class, CLogBase.
@see CLogClient */
	{
	return *iBase;
	}
