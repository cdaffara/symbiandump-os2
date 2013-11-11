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
// Defines TTsTime class, to represent UTC and  Local Time for use within Task Scheduler
// 
//


#ifndef __SCHTIME_H__
#define __SCHTIME_H__

// System includes
#include <e32std.h>

//Forward declarations
class RWriteStream;
class RReadStream;

/**
In Task Scheduler TTsTime is used to represent time as either UTC or Local Time.
It is used by many of the Task Scheduler API's and also used internally within Task Scheduler.
This class is not expected to be stored by Task Scheduler clients.

It provides EXPORTed APIs for constructing, setting and getting UTC and Local Time.

Internally the object always holds time as UTC (using the data member iUTC) irrespective of 
whether the object is local time based or UTC based. 

If the object is local time based iOffset will be set to the system TimeZone/DST offset.
When UTC based iOffset will always be 0. 

Therefore:

When representing UTC:
	iUTC contains the UTC time
	iOffSet is set to 0
	iFlags, bit 0 is set to 1

When representing Local Time:
	iUTC contains the home time minus the TimeZone/DST offset
	iOffSet contains the TimeZone/DST offset 
	iFlags, bit 0 is set to 0

If an instance of this class is created using the default constructor then:
	iUTC is set to 0
	iOffSet is set to 0 
	iFlags, bit 0 is set to 1 (indicating UTC time)

@publishedAll
@released
*/
class TTsTime
	{
public:
	
	IMPORT_C TTsTime();
	
	IMPORT_C TTsTime(const TTime& aTime, TBool aIsUtc);
	
	IMPORT_C void SetLocalTime(const TTime& aLocalTime);
	
	IMPORT_C const TTime GetLocalTime(); 

	IMPORT_C TTime GetLocalTime() const; 

	IMPORT_C void SetUtcTime(const TTime& aUtcTime);
	
	IMPORT_C const TTime& GetUtcTime(); 

	IMPORT_C const TTime& GetUtcTime() const;
	
	IMPORT_C TBool IsUtc() const;
	
	IMPORT_C TTsTime& operator=(const TTsTime& aTsTime);
	
	IMPORT_C TTsTime(const TTsTime& aTTsTime);
 	
public:
	// APIs for use within the Task Scheduler server

	void ExternalizeL(RWriteStream& aStream) const;
	
	void InternalizeL(RReadStream& aStream);
	
	void ProcessOffsetEvent();

	inline TTimeIntervalSeconds GetOffset();
	
			

private:

	TTime DetermineLocalTime() const;

	/**
	This object always stores time as UTC irrespective of whether the object is home time or UTC based.
	*/
	TTime iUtcTime;
	
	/**
	If the object is UTC based then this will always be 0. 	If home time based then this will contain the value
	of system TimeZone/DST offset at the time that the object was created or last updated.
	*/
	TTimeIntervalSeconds iOffset;
	
	/**
	Bit 0 is set to 0 when UTC based, Bit 0 is set to 1 when home time based, Bit1-Bit31 are reserved for future use.
	*/
	TUint32 iFlags;

	};
	
/**
This method must only be used by Task Scheduler itself as it returns raw offset data
that can become out of date if system Timezone/DST changes occur.
@internalComponent
*/
// Not for Client Use , Only to be used Internally.
inline TTimeIntervalSeconds TTsTime::GetOffset()
	{
	return iOffset;
	}

#endif
