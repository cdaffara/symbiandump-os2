/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "util.h"
#include <centralrepository.h>

_LIT(KDaylightSavingsMinFormat, "%F%Y0224:");
_LIT(KDaylightSavingsMaxFormat, "%F%Y0924:");

void Util::SetAppropriateTimezoneL()
	{
	TUid repUid = {0x1020e4d3};
	CRepository* rep = CRepository::NewLC(repUid);
	
	// Set the date format to European
	User::LeaveIfError(rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction));
	User::LeaveIfError(rep->Set(101, EDateEuropean)); // 101 is the date format reg entry
	TUint32 keys(0);
	User::LeaveIfError(rep->CommitTransaction(keys));
	
	CleanupStack::PopAndDestroy(rep);
	
	TExtendedLocale locale;
	locale.LoadSystemSettings();
	locale.GetLocale()->SetDateFormat(EDateEuropean);
	User::LeaveIfError(locale.SaveSystemSettings());
	}

TBool Util::DaylightSavingsAppliesL(const TTime& utc)
	{
	
	// This algorithm needs the first day of the week to be monday
	
	TDay oldStart;
	
	TLocale set;
	oldStart = set.StartOfWeek();
	set.SetStartOfWeek(EMonday);
	set.Set();
	
	TBuf<9> min;
	TBuf<9> max;
	
	utc.FormatL(min, KDaylightSavingsMinFormat);
	utc.FormatL(max, KDaylightSavingsMaxFormat);
	
	// Get times representing the first/last possible day of this 
	// year that daylight savings time change could change on
	
	TTime timeMin;
	User::LeaveIfError(timeMin.Set(min));
	TTime timeMax;
	User::LeaveIfError(timeMax.Set(max));

	// Find the last sunday in the respective months
	
	TTimeIntervalDays addMin(6 - timeMin.DayNoInWeek());
	TTimeIntervalDays addMax(6 - timeMax.DayNoInWeek());
	
	timeMin += addMin;
	timeMax += addMax;
	
	// The change happens at 1AM.
	TTimeIntervalHours hour(1);
	timeMin += hour;
	timeMax += hour;
	
	// Now we know which day the change occurs on.
	// Compare it to what the UTC is.

	TBool result = ((timeMin <= utc) && (timeMax > utc));
	
	// reset the first week day
	set.SetStartOfWeek(oldStart);
	set.Set();
	
	return result;
	
	}
