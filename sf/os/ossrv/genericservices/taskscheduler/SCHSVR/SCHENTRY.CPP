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

// User includes
#include "SCHENTRY.H"
#include "SchLogger.h"

//	
NONSHARABLE_CLASS(TScheduleEntryHourly) : public TScheduleEntry
	{
public:
	TScheduleEntryHourly(TScheduleEntryInfo2& aInfo);
	void CalculateNextPossibleRunDate(TTime& aTime) const;
	};

NONSHARABLE_CLASS(TScheduleEntryDaily) : public TScheduleEntry
	{
public:
	TScheduleEntryDaily(TScheduleEntryInfo2& aInfo);
	void CalculateNextPossibleRunDate(TTime& aTime) const;
	};

NONSHARABLE_CLASS(TScheduleEntryMonthly) : public TScheduleEntry
	{
public:
	TScheduleEntryMonthly(TScheduleEntryInfo2& aInfo);
	void CalculateNextPossibleRunDate(TTime& aTime) const;
	};

NONSHARABLE_CLASS(TScheduleEntryYearly) : public TScheduleEntry
	{
public:
	TScheduleEntryYearly(TScheduleEntryInfo2& aInfo);
	void CalculateNextPossibleRunDate(TTime& aTime) const;
	};
//	

TScheduleEntry* ScheduleEntryFactory::CreateL(TScheduleEntryInfo2& aInfo)
	{
	switch (aInfo.IntervalType())
		{
		case EHourly:
			{
			return new(ELeave) TScheduleEntryHourly(aInfo);
			}
		case EDaily:
			{
			return new(ELeave) TScheduleEntryDaily(aInfo);
			}
		case EMonthly:
			{
			return new(ELeave) TScheduleEntryMonthly(aInfo);
			}
		case EYearly:
			{
			return new(ELeave) TScheduleEntryYearly(aInfo);
			}
		}
	User::Leave(KErrArgument);
	return NULL;//never gets to here!!
	}

//	
//TScheduleEntry (generic code)
TScheduleEntry::TScheduleEntry(TScheduleEntryInfo2& aInfo)
:	iEntryInfo(aInfo)
	{
	TTsTime time;
	time.SetUtcTime(Time::MaxTTime());
	iDueTime = time;
	}

const TTsTime& TScheduleEntry::DueTime() const
	{
	return iDueTime;
	}

TInt TScheduleEntry::Offset()
	{
	return (_FOFF(TScheduleEntry, iLink));
	}

const TScheduleEntryInfo2& TScheduleEntry::Info() const
	{
	return iEntryInfo;
	}
/**
returns the next due time for this schedule entry.  aTime is the minimum time
for which this due time should be.*/
const TTsTime& TScheduleEntry::NextScheduledTime(const TTsTime& aTime)
	{
	// update start time in case of any timezone/DST changes
	// do this before calculating next scheduled time so that it is taken into
	// account in the calculation
	iEntryInfo.ProcessOffsetEvent();
	
	// if start time = max time then algorithm below doesnt work
	// so we need to jump out of it here.
	if (iEntryInfo.StartTime().GetUtcTime() == Time::MaxTTime() 
		|| iEntryInfo.StartTime().GetLocalTime() == Time::MaxTTime() 
		|| aTime.GetUtcTime() < (iEntryInfo.StartTime().GetUtcTime() + iEntryInfo.ValidityPeriod())) 
		iDueTime = iEntryInfo.StartTime();
	else
		{
		// Work out when this schedule entry can next run, by adding interval steps
		// Due times should be UTC based or local time based, 
		// in accordance with the start time set by the user
		TTime nextDueTime;

		if (iEntryInfo.StartTime().IsUtc())
			{
			nextDueTime = iEntryInfo.StartTime().GetUtcTime();
			while (aTime.GetUtcTime() > (nextDueTime + iEntryInfo.ValidityPeriod()))	
				CalculateNextPossibleRunDate(nextDueTime);
			
			iDueTime.SetUtcTime(nextDueTime);
			}
		else	//is hometime based
			{
			nextDueTime = iEntryInfo.StartTime().GetLocalTime();
			while (aTime.GetLocalTime() > (nextDueTime + iEntryInfo.ValidityPeriod()))	
				CalculateNextPossibleRunDate(nextDueTime);
			
			iDueTime.SetLocalTime(nextDueTime);
			}
		}
	return iDueTime;	
	}


//subclass-specific code...
TScheduleEntryHourly::TScheduleEntryHourly(TScheduleEntryInfo2& aInfo)
			:TScheduleEntry(aInfo)
	{
	}

void TScheduleEntryHourly::CalculateNextPossibleRunDate(TTime& aTime) const
	{
	aTime+=TTimeIntervalHours(Info().Interval());
	}

TScheduleEntryDaily::TScheduleEntryDaily(TScheduleEntryInfo2& aInfo)
			:TScheduleEntry(aInfo)
	{
	}

void TScheduleEntryDaily::CalculateNextPossibleRunDate(TTime& aTime) const
	{
	aTime+=TTimeIntervalDays(Info().Interval());
	}

TScheduleEntryMonthly::TScheduleEntryMonthly(TScheduleEntryInfo2& aInfo)
			:TScheduleEntry(aInfo)
	{
	}

void TScheduleEntryMonthly::CalculateNextPossibleRunDate(TTime& aTime) const
	{
	aTime+=TTimeIntervalMonths(Info().Interval());
	}

TScheduleEntryYearly::TScheduleEntryYearly(TScheduleEntryInfo2& aInfo)
			:TScheduleEntry(aInfo)
	{
	}

void TScheduleEntryYearly::CalculateNextPossibleRunDate(TTime& aTime) const
	{
	aTime+=TTimeIntervalYears(Info().Interval());
	}
