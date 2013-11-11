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
// Defines TScheduleEntry hierachy: 
// - const classes which tell you the next time they're due
// - CSchedule is an aggregate of TScheduleEntry objects
// 
//

#if !defined(__SCHENTRY_H__)
#define __SCHENTRY_H__

//System Includes
#include <e32base.h>

//User Includes
#include "SCHINFO.H"

/**
TScheduleEntry
@internalComponent
*/
class TScheduleEntry
	{
	friend class ScheduleEntryFactory;
	
public:
	//returns the next due time for this schedule entry.
	const TTsTime& NextScheduledTime(const TTsTime& aTime);
	const TTsTime& DueTime() const;
	const TScheduleEntryInfo2& Info() const;

	static TInt Offset();

protected:
	TScheduleEntry(TScheduleEntryInfo2& aInfo);	
private:	
	virtual void CalculateNextPossibleRunDate(TTime& aTime) const = 0 ;

private:	
	TSglQueLink iLink; // queue management
	TScheduleEntryInfo2 iEntryInfo; 
	TTsTime iDueTime;
	};

/**
@internalComponent
*/
class ScheduleEntryFactory
	{
public:
	static TScheduleEntry* CreateL(TScheduleEntryInfo2& aInfo);
	};


#endif
