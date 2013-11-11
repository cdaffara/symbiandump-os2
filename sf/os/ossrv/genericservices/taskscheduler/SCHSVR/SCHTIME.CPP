// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32def.h>
#include <s32strm.h>
#include "SCHTIME.H"
#include "SCHEXEC.H"

/**
UTC/Local Time flag value
@internalComponent
*/
const TUint32 KIsUtc = 0x00000001;

/** 
Default constructor for TTsTime.
This constructor initialises its member data to zero.
By default this sets it to be UTC based.
*/ 
EXPORT_C TTsTime::TTsTime():
	iUtcTime(0), 
	iOffset(0), 
	iFlags(KIsUtc)
	{
	}

/**
Copy constructor for TTsTime
@param aTTsTime value to duplicate
*/	
EXPORT_C TTsTime::TTsTime(const TTsTime& aTTsTime)
	{
	*this = aTTsTime;
	}
		
/** 
Constructs a TTsTime with a TTime. 
If the iIsUtc is ETrue, then TTsTime is UTC based time, if iIsUtc is EFalse 
then TTsTime is local time based. This constructor will update all the member data as appropriate.	
@param aTime The TTime value
@param aIsUtc is Etrue when aTime is UTC and EFalse when aTime is local time.
*/	
EXPORT_C TTsTime::TTsTime(const TTime& aTime, TBool aIsUtc)
	{
	// if the time passed as argument is UTC based
	if(aIsUtc)
		SetUtcTime(aTime);
		
	// if the time passed is local time	
	else
		SetLocalTime(aTime);
	}
	
/**
Sets this object to a local time based value updating its parameters as appropriate.
@param aLocalTime The local time to be set. 
*/
EXPORT_C void TTsTime::SetLocalTime(const TTime& aLocalTime)
	{
	iOffset = User::UTCOffset(); // sets the current UTC offset to iOffset
	iUtcTime = aLocalTime - iOffset; // converts the given aLocalTime to a UTC time value
	iFlags &= ~KIsUtc;    //set the Bit 0 to zero for Local Time
	}
		
/** 
Sets this object to a UTC time based value updating its parameters as appropriate.
@param aUtcTime The UTC time to be set
*/
EXPORT_C void TTsTime::SetUtcTime(const TTime& aUtcTime)
	{
	iUtcTime = aUtcTime; 
	iOffset = 0; 
	iFlags |= KIsUtc;    //set the Bit 0 to one as TTsTime is UTC
	}
		
/** 
This function returns a home time value.
@return Retrieves time from object in local time
*/
EXPORT_C const TTime TTsTime::GetLocalTime()
	{
	return DetermineLocalTime();
	}
	
/** 
This function returns a home time value.
@return Retrieves time from object in local time
*/
EXPORT_C TTime TTsTime::GetLocalTime() const
	{
	return DetermineLocalTime();
	}	
		
/**
This function returns a UTC value.
@return Returns the UTC time value.
*/ 
EXPORT_C const TTime& TTsTime::GetUtcTime()
	{
	return iUtcTime;
	}
	
/**
This function returns a UTC value.
@return Returns the UTC time value.
*/ 
EXPORT_C const TTime& TTsTime::GetUtcTime() const
	{
	return iUtcTime;
	}
	
/** 
@return ETrue, if TTsTime object is UTC and EFalse if TTsTime object is local time
*/
EXPORT_C TBool TTsTime::IsUtc() const
	{
	return iFlags & KIsUtc ? ETrue: EFalse;
	}

	
/**
This function returns a home time value.
If UTC based returns UTC time + User::UTCOffset(). If local time based returns
local time of the object.
@internalComponent
*/
TTime TTsTime::DetermineLocalTime() const
	{
	TTime localTime;
	if(IsUtc())
		localTime = iUtcTime+User::UTCOffset();
	else
		localTime = iUtcTime+iOffset;
	
	return localTime;
	}
	
/** 
This class does not explicitly update iOffSet if the system TimeZone/DST offset changes.
When called this API will update the object if the system TimeZone/DST offset has changed. 
Keeping the offset in objects of this class correct is the responsibility of the 
Task Scheduler server.This applies to local time objects only. UTC objects will be unaffected.
@internalComponent
*/	
void TTsTime::ProcessOffsetEvent() 
	{
	if(iFlags & KIsUtc)
		return;
			
	TTimeIntervalSeconds kernelOffset = User::UTCOffset();
	
	// check if there was an offset event happened without TTsTime being updated
	// localTime will always be the addition of UTC time and kernel offset
	if (iOffset != kernelOffset)
		{	
		// If TTsTime is home time and there was a time zone change then TTsTime needs to be update.
		// If not then TTsTime remains the same, as UTC times are the same everywhere.
		iUtcTime = (iUtcTime+iOffset) - kernelOffset; 
		iOffset = kernelOffset; 
		}
	}
/** 
Overloaded assignment operator for TTsTime
@param aTsTime time value to copy
*/
EXPORT_C TTsTime& TTsTime::operator=(const TTsTime& aTsTime)
	{
	Mem::Copy(this,&aTsTime,sizeof(*this));
	return *this;	
	}
	
/** 
Uses the standard template operator>>() to internalize the object from aStream.
Additionally this method will ensure that the object is updated in the event that 
the system TimeZone/DST offset changed since it was externalised.
@internalComponent
*/
void TTsTime::InternalizeL(RReadStream& aStream)  
	{
	TInt32 offset;
	TInt64 utcTime;
	
	// Updates TTsTime if local time based and the system Timezone/DST change occurred.
	ProcessOffsetEvent();
				
	aStream >> utcTime;
	iUtcTime = TTime(utcTime);
	aStream >> offset;
	iOffset = offset;
	aStream >> iFlags;	
	}
	
/** 
Uses the standard template operator>>() to externalize the object to aStream
@internalComponent
*/
void TTsTime::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iUtcTime.Int64();
	aStream.WriteInt32L(iOffset.Int());
	aStream.WriteUint32L(iFlags);
	}
