// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SCHINFO.H"
#include "SCHEXEC.H"

/**
Persists flag value, used by TScheduleState2
@internalComponent
*/
const TUint32 KPersists = 0x00000001;


/**
IsEnabled flag value, used by TScheduleState2
@internalComponent
*/
const TUint32 KIsEnabled = 0x00000002;


EXPORT_C TTaskInfo::TTaskInfo(TInt aTaskId, TName& aName, TInt aPriority, TInt aRepeat)
				:iRepeat(aRepeat), iTaskId(aTaskId), iName(aName), iPriority(aPriority)
/** Constructor taking the specified parameters.

@param aTaskId The task Id.
@param aName The name of the task.
@param aPriority The task priority.
@param aRepeat How often the task is to be repeated */
	{
	} 

EXPORT_C TTaskInfo::TTaskInfo()
/** Default constructor. */
	{
	}

EXPORT_C TTaskInfo& TTaskInfo::operator=(const TTaskInfo& aTaskInfo)
	{
	Mem::Copy(this,&aTaskInfo,sizeof(*this));
	return *this;
	}

EXPORT_C void TTaskInfo::ExternalizeL(RWriteStream& aWriteStream) const
/** Externalises an object of this class to a write stream.
	
	The presence of this function means that the standard templated operator<<() 
	can be used to externalise objects of this class.
	
	@param aStream Stream to which the object should be externalised. */
	{
	aWriteStream << iName;
	aWriteStream.WriteInt32L(iTaskId);
	aWriteStream.WriteInt32L(iRepeat);
	aWriteStream.WriteInt32L(iPriority);
	}

EXPORT_C void TTaskInfo::InternalizeL(RReadStream& aReadStream)
	/** Internalises an object of this class from a read stream.
	
	The presence of this function means that the standard templated operator>>() 
	can be used to internalise objects of this class.
	
	Note that the function has assignment semantics. It replaces the old value 
	of the object with a new value read from the read stream.
	
	@param aStream Stream from which the object is to be internalised. */
	{
	aReadStream >> iName;
	iTaskId = aReadStream.ReadInt32L();
	iRepeat = aReadStream.ReadInt32L();
	iPriority = aReadStream.ReadInt32L();
	}

//
/**
Externalizes the ScheduleEntryInfo
@internalComponent only used by server
*/
void TScheduleEntryInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iValidityPeriod.Int());
	aStream.WriteInt32L(iInterval);
	aStream.WriteInt8L(iIntervalType);
	TInt64 asInt = iStartTime.Int64();
	aStream.WriteInt32L(I64LOW(asInt));
	aStream.WriteInt32L(I64HIGH(asInt));
	}

/**
Internalizes the ScheduleEntryInfo
@internalComponent only used by server
*/
void TScheduleEntryInfo::InternalizeL(RReadStream& aStream)
	{
	iValidityPeriod = aStream.ReadInt32L();
	iInterval = aStream.ReadInt32L();
	iIntervalType = TIntervalType(aStream.ReadInt8L());
	TInt64 asInt;
	TInt lo;
	TInt hi;
	lo=aStream.ReadInt32L();
	hi=aStream.ReadInt32L();
	asInt = MAKE_TINT64(hi,lo);
	iStartTime = asInt;
	}



/**
TScheduleEntryInfo2 Default constructor. 
It sets the object's members data to the following default values.
iIntervalType : EHourly
iStartTime : UTC time set to 0
iInterval : 0
iValidityPeriod : 0
*/
EXPORT_C TScheduleEntryInfo2::TScheduleEntryInfo2() :
		iInterval(0),
		iIntervalType(TIntervalType(0)),
		iValidityPeriod(0),
		iReserved(NULL)
	{
		
	}
	
/**
Copy constructor for TScheduleEntryInfo2
Sets the parameter's data to this object.
@param aEntryInfo The TScheduleEntryInfo2 object to be copied
*/	
EXPORT_C TScheduleEntryInfo2::TScheduleEntryInfo2(const TScheduleEntryInfo2& aEntryInfo)
	{
	*this = aEntryInfo;
	}

/**
TScheduleEntryInfo2 constructor taking the specified parameters.

@param aStartTime The first time that the entry will cause execution of tasks
@param aIntervalType Defines the type of time-frame relative to which execution of tasks is timed; 
for example, EHourly implies relative to the current hour, EDaily implies 
relative to the current day
@param aInterval The interval between execution of tasks
For a schedule entry interval to be valid, it should be greater than or equal to 1
@param aIntervalMinutes The period for which the entry is valid
*/	
EXPORT_C TScheduleEntryInfo2::TScheduleEntryInfo2(const TTsTime& aStartTime, TIntervalType aIntervalType, TInt aInterval, TTimeIntervalMinutes aValidityPeriod)
	{
	iStartTime = aStartTime;
	iIntervalType = aIntervalType;
	iInterval = aInterval;
	iValidityPeriod = aValidityPeriod ;
	iReserved = NULL;
	}

/**
Returns the Interval Type
@return The type of interval used between due times for this schedule entry.
The type of interval used may be EHourly, EDaily, EMonthly or EYearly.
@see TIntervalType
*/
EXPORT_C TIntervalType TScheduleEntryInfo2::IntervalType() const
	{
	return iIntervalType;	
	}

/**
Sets the type of interval used between due times for this schedule entry.
The type of interval used may be EHourly, EDaily, EMonthly or EYearly.
@param aIntervalType The type of interval to be used.
@see TIntervalType
*/	
EXPORT_C void TScheduleEntryInfo2::SetIntervalType(TIntervalType aIntervalType)
	{
	iIntervalType = aIntervalType;	
	}

/**
Returns the first time at which the entry will cause execution of tasks.
@return Start time - this TTsTime value may be either UTC or local time based.
Entries with local time based start times will remain at that local time regardless of
timezone or DST changes (ie. will float). Entries with UTC based start times, will
remain at the given UTC time (will not float).
@see TTsTime
*/
EXPORT_C const TTsTime& TScheduleEntryInfo2::StartTime() const
	{
	return iStartTime;	
	}

/**
Sets the first time the entry will cause execution of tasks.
@param aStartTime This TTsTime value may be either UTC or local time based.
If this is a local time based value, the schedule entry will remain
at that local time regardless of timezone and DST changes (ie. it will float)
If the value is UTC based, the schedule entry will remain at that UTC time (will not float).
@see TTsTime
*/
EXPORT_C void TScheduleEntryInfo2::SetStartTime(const TTsTime& aStartTime)
	{
	iStartTime = aStartTime;
	}
	
/**
Returns the interval between execution of tasks.
@return Interval between execution of tasks. 
For a schedule entry interval to be valid, it should be greater than or equal to 1.
@see TScheduleEntryInfo2::SetInterval
*/	
EXPORT_C TInt TScheduleEntryInfo2::Interval() const
	{
	return 	iInterval;
	}

/**
Sets the interval between execution of tasks.
The way that this value is interpreted depends on the value of iIntervalType. 
For example, if the interval is 2 and iIntervalType has a value of EMonthly, 
then the interval is 2 months. 
@param aInterval For a schedule entry interval to be valid, it should be greater than or equal to 1.
*/	
EXPORT_C void TScheduleEntryInfo2::SetInterval(TInt aInterval)
	{
	iInterval = aInterval;	
	}

/**
Return the period for which the entry is valid.
After the validity period has expired, tasks associated with the entry will 
not be eligible for execution
@return TTimeIntervalMinutes
*/
EXPORT_C TTimeIntervalMinutes TScheduleEntryInfo2::ValidityPeriod() const
	{
	return iValidityPeriod;	
	}
	
/**
Sets the period for which the entry is valid.
After the validity period has expired, tasks associated with the entry will 
not be eligible for execution
@param aValidityPeriod, the period for which the entry is valid
@see TTimeIntervalMinutes
*/	
EXPORT_C void TScheduleEntryInfo2::SetValidityPeriod(TTimeIntervalMinutes aValidityPeriod)
	{
	iValidityPeriod = aValidityPeriod;	
	}


/**
Non exported constructor accepting a TScheduleEntryInfo parameter
This constructor is provided for use with the deprecated APIs.
This will assume home time in order to maintain backwards compatibility and will create a #
TScheduleEntryInfo2 object with a local time based start time.
@param aEntryInfo Entry info of deprecated type TScheduleEntryInfo
@see TScheduleEntryInfo
*/
TScheduleEntryInfo2::TScheduleEntryInfo2(const TScheduleEntryInfo& aEntryInfo)
	{	
	iStartTime.SetLocalTime(aEntryInfo.iStartTime);
	iIntervalType = aEntryInfo.iIntervalType;
	iInterval = aEntryInfo.iInterval;
	iValidityPeriod = aEntryInfo.iValidityPeriod ;	
	}


/**
Externalises an object of this class to a write stream.
The presence of this function means that the standard templated operator<<() 
can be used to externalise objects of this class.
	
@param aStream Stream to which the object should be externalised.
@internalComponent only used by server
*/
void TScheduleEntryInfo2::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iStartTime;
	aStream.WriteInt32L(iIntervalType);	
	aStream.WriteInt32L(iInterval);
	aStream.WriteInt32L(iValidityPeriod.Int());	
	}

	
/**
Internalises an object of this class from a read stream.
The presence of this function means that the standard templated operator>>() 
can be used to internalise objects of this class.
		
@param aStream Stream from which the object is to be internalised.
@internalComponent only used by server
*/	
void TScheduleEntryInfo2::InternalizeL(RReadStream& aStream)
	{
	aStream >> iStartTime;
	iIntervalType = TIntervalType(aStream.ReadInt32L());
	iInterval = aStream.ReadInt32L();
	iValidityPeriod = aStream.ReadInt32L();
	}
	
/**
Calls ProcessOffsetEvent() on TScheduleEntryInfo's start time member
@see TTsTime::ProcessOffsetEvent
@internalComponent only used by the server
*/
void TScheduleEntryInfo2::ProcessOffsetEvent()
	{
	iStartTime.ProcessOffsetEvent();
	}

/**
Assignment operator for TScheduleEntryInfo2
@see Mem::Copy
*/
EXPORT_C TScheduleEntryInfo2& TScheduleEntryInfo2::operator=(const TScheduleEntryInfo2& aEntryInfo)
	{
	Mem::Copy(this,&aEntryInfo,sizeof(*this));
	return *this;
	}
	
/**
Default Constructor for TScheduleState2.
By default, this state: has an empty string name, is non persistent, non enabled and its due time is set to zero.
*/
EXPORT_C TScheduleState2::TScheduleState2():
	iName(_L("")),
	iFlags(0),
	iReserved(NULL)
	{
		
	}

/**
Copy constructor for TScheduleState2
@param aScheduleState The TScheduleState2 object to be copied
*/	
EXPORT_C TScheduleState2::TScheduleState2(const TScheduleState2& aScheduleState)
	{
	*this = aScheduleState;
	}

/**
Constructor taking the specified parameters.
@param	aName The name of the schedule
@param 	aDueTime The time when the schedule is next due.
@param 	aPersists Boolean to indicate whether the schedule is persistent or not.
if a schedule is persistent, its lifetime is not limited to the lifetime of 
the tasks associated with it.
If a schedule is transient, it is created together with a new scheduled task, 
and is destroyed when the task is destroyed
@param 	aEnabled Boolean to indicate whether the shedule is enabled or not.
*/	
EXPORT_C TScheduleState2::TScheduleState2(const TName& aName, const TTsTime& aDueTime, TBool aPersists, TBool aEnabled)
	{
	iName = aName;
	iDueTime = aDueTime;
	SetPersists(aPersists);
	SetEnabled(aEnabled);
	iReserved = NULL;
	
	}
		
/**
@return  The name of the schedule
*/
EXPORT_C const TName& TScheduleState2::Name() const
	{
	return iName;	
	}
	
/**
Sets the name of the schedule
@param aName The name of the schedule
*/	
EXPORT_C void TScheduleState2::SetName(const TName& aName)
	{
	iName = aName;	
	}


/**
Returns the time when the schedule is next due.
@return The time when the schedule is next due. This time could be either home time (for floating schedules) or UTC time.
@see TTsTime
*/
EXPORT_C const TTsTime& TScheduleState2::DueTime() const
	{
	return iDueTime;	
	}

	
/**
Sets the time when the schedule is next due.
@param aDueTime The time when the schedule is next due. This time could be either home time (for floating schedules) or UTC time.
@see TTsTime
*/	
EXPORT_C void TScheduleState2::SetDueTime(const TTsTime& aDueTime)
	{
	iDueTime = aDueTime;
	}


/**
Returns a boolean whether this schedule perists or not.
@return Etrue if this schedule persists, EFalse if this schedule doen't persist.
*/
EXPORT_C TBool TScheduleState2::Persists() const
	{
	return iFlags & KPersists ? ETrue: EFalse;		
	}

	

/**
Sets a boolean whether this schedule perists or not.
@param aPersists Etrue if this schedule persits, EFalse if this schedule doen't persist.
*/	
EXPORT_C void TScheduleState2::SetPersists(TBool aPersists)
	{
	if(aPersists )
		iFlags |= KPersists;
	else
		iFlags &= ~KPersists;
	}


/**
Returns information on whether this schedule is enabled or not.
@return Etrue if the schedule is enabled, EFalse id the schedule is not enabled.
*/
EXPORT_C TBool TScheduleState2::Enabled() const
	{
	return iFlags & KIsEnabled ? ETrue: EFalse;	
	}

	
/**
Sets information on whether this schedule is enabled or not.
@param aEnabled 
*/	
EXPORT_C void TScheduleState2::SetEnabled(TBool aEnabled)
	{
	if(aEnabled )
		iFlags |= KIsEnabled;
	else
		iFlags &= ~KIsEnabled;	
	}

/**
Assigns a TScheduleState2 to this object.
@see Mem::Copy
*/
EXPORT_C TScheduleState2& TScheduleState2::operator=(const TScheduleState2& aScheduleState)
	{
	Mem::Copy(this,&aScheduleState,sizeof(*this));
	return *this;
	}

/**
A constructor for TScheduleState that takes a TScheduleState2 parameter,
for use with the deprecated APIs. All TScheduleStates created will have 
local time based iDueTime data members.
@internalComponent
*/
TScheduleState::TScheduleState(const TScheduleState2& aScheduleState2)
	{
	iName = aScheduleState2.Name();	
	iPersists = aScheduleState2.Persists();
	iEnabled = aScheduleState2.Enabled();
	
	// iDueTime is local time based for backwards compatibility
	iDueTime = aScheduleState2.DueTime().GetLocalTime();
	}
