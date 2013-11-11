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
// This file contains the implementation of internal classes used to parse TInternetDate data
// 
//

/**
 @file tinternetdateparser.cpp
 @see tinternetdateparser.h
*/

#include "tinternetdateparser.h"
#include "inetprottextutils.h"
#include <e32base.h>

//constants
//
const TInt KRfc1123DateTimeLength = 29;	// Sun, 06 Nov 1994 08:49:37 GMT
const TInt KHHMMSSFormatLength=8;		// HH:MM:SS

// Long form day names, 'weekday' in RFC850
//
static const TText8* const KInternetWeekDays[] = 
	{
	_S8("Monday"),
	_S8("Tuesday"),
	_S8("Wednesday"),
	_S8("Thursday"),
	_S8("Friday"),
	_S8("Saturday"),
	_S8("Sunday")
	};

// Month names, 'month' in RFC1123 & RFC850
//
static const TText8* const KInternetMonths[] = 
	{
	_S8("Jan"),
	_S8("Feb"),
	_S8("Mar"),
	_S8("Apr"),
	_S8("May"),
	_S8("Jun"),
	_S8("Jul"),
	_S8("Aug"),
	_S8("Sep"),
	_S8("Oct"),
	_S8("Nov"),
	_S8("Dec")
	};

/**
	Converts from TDateTime to Internet RFC1123 Style date strings
	
	@since			7.0
	@param			aDateTime The date/time to be parsed
	@leave			KErrNoMemory
	@return			HBufC8* aDescriptor containing a RFC1123 style date. 
*/
HBufC8* TInternetDateParser::ConvertToRfc1123FormL(const TDateTime& aDateTime)
	{
	const TChar KSpace = ' ';

	HBufC8* dateTime = HBufC8::NewLC(KRfc1123DateTimeLength);
	TPtr8 dateTimePtr = dateTime->Des();
	// Append the wkDay
	TTime time(aDateTime);
	TInt wkDay = time.DayNoInWeek();
	TPtrC8 wkDayText(KInternetWeekDays[wkDay]);
	dateTimePtr.Append(wkDayText.Left(3));


	// Append the day number 
	_LIT8(KDayNumberFormat, ", %02d");
	TInt dayNumber = aDateTime.Day() + 1;
	dateTimePtr.AppendFormat(KDayNumberFormat, dayNumber);

	// Append month
	dateTimePtr.Append(KSpace);
	TInt month = aDateTime.Month();
	dateTimePtr.Append(TPtrC8(KInternetMonths[month]));

	// Append year
	_LIT8(KYearDigitFormat, " %4d");
	TInt year = aDateTime.Year();
	dateTimePtr.AppendFormat(KYearDigitFormat, year);

	// Append Time HH:MM:SS

	dateTimePtr.Append(KSpace);
	_LIT8(KHHMMSSFormat, "%02d:%02d:%02d GMT");
	dateTimePtr.AppendFormat(KHHMMSSFormat, aDateTime.Hour(),aDateTime.Minute(),aDateTime.Second());

	CleanupStack::Pop(dateTime);
	return dateTime;
	}


/**
	Used to parse internet text date/time into a TDateTime
	
	@since			7.0
	@param			aInternetTextDateTime The date/time in a text format
	@param			aDateTime The result of parsing is placed here
	@leave			KErrNoMemory, KErrCorrupt If the format of the descriptor is not valid
*/	
void TInternetDateParser::ConvertFromInternetFormL(const TDesC8& aInternetTextDateTime, TDateTime& aDateTime)
	{
	const TChar KComma = ',';
	TInt colonLoc = aInternetTextDateTime.Locate(KComma);
	switch (colonLoc)
		{
	case -1:	// ANSI C asctime() format: Sun Nov  6 08:49:37 1994
		ConvertFromAscTimeFormL(aInternetTextDateTime, aDateTime);
		break;
	case 3:		// RFC 1123 Style Date: Sun, 06 Nov 1994 08:49:37 GMT
	case 6:		// RFC 850 Style Date: Sunday, 06-Nov-94 08:49:37 GMT
	case 7:		
	case 8:	
	case 9:		
		{
		TPtrC8 dateTime(aInternetTextDateTime.Right(aInternetTextDateTime.Length() -(colonLoc+1))); // Remove "<day>,"
		ConvertFromRfc1123And850FormL(dateTime, aDateTime);	
		}
		break;
	default:
		User::Leave(KErrCorrupt);
		};
	}

/**
	@param			aRfcDateTime 
	@param			aDateTime The result of parsing is placed here
 */	
void TInternetDateParser::ConvertFromRfc1123And850FormL(const TDesC8& aRfcDateTime, TDateTime& aDateTime)
	{
	TPtrC8 dateTime(aRfcDateTime);
	InetProtTextUtils::RemoveWhiteSpace(dateTime, InetProtTextUtils::ERemoveBoth);
	TInt textLeft = ParseRfcDateDayMonthYearL(dateTime, aDateTime);
	if (textLeft <= 0)
		User::Leave(KErrCorrupt);
	dateTime.Set(dateTime.Right(textLeft));
	InetProtTextUtils::RemoveWhiteSpace(dateTime, InetProtTextUtils::ERemoveLeft);
	ParseHHMMSSTimeL(dateTime, aDateTime);
	textLeft= dateTime.Length() - KHHMMSSFormatLength;
	if (textLeft > 1) // Must still have leading space
		{
		dateTime.Set(dateTime.Right(textLeft));
		InetProtTextUtils::RemoveWhiteSpace(dateTime, InetProtTextUtils::ERemoveLeft);
		TTimeIntervalMinutes timeOffset = 0;
		ParseTimeOffsetL(dateTime, timeOffset);

		if (timeOffset.Int() > 0 || timeOffset.Int() < 0)
			{
			TTime time(aDateTime);
			time -= timeOffset;
			aDateTime = time.DateTime();
			}
		}
	else
		User::Leave(KErrCorrupt); // Must include a timezone or timeoffset
	}

/**
	@param			aTimeOffset 
	@param			aOffsetMinutes 
 */
void TInternetDateParser::ParseHHMMTimeOffsetL(const TDesC8& aTimeOffset, TTimeIntervalMinutes& aOffsetMinutes)
	{
	TInt offsetTextLength=aTimeOffset.Length();
	if (offsetTextLength != 5) // +0000 or -0000
		User::Leave(KErrCorrupt);

	TPtrC8 timeOffset(aTimeOffset.Right(offsetTextLength-1));
	
	TInt num=0;
	if (InetProtTextUtils::ConvertDescriptorToInt(timeOffset, num) !=4)
		User::Leave(KErrCorrupt); // not 0000

	TInt minutes = num % 100;
	TInt hours = num / 100;
	TInt offsetMinutes=0;
	if (minutes < 60 && hours < 24)
		offsetMinutes = minutes + (hours * 60);
	
	if (aTimeOffset[0] == '-')
		offsetMinutes *= -1;

	aOffsetMinutes = offsetMinutes;
	}

/**
	@param			aTimeOffset 
	@param			aOffsetMinutes 
 */
void TInternetDateParser::ParseTimeOffsetL(const TDesC8& aTimeOffset, TTimeIntervalMinutes& aOffsetMinutes)
	{
	TInt offsetMinutes=0;
	TInt offsetTextLength = aTimeOffset.Length();
	if (offsetTextLength > 1)
		{
		// check second letter to see if UT, GMT
		// and to differentiate between Standard and Daylight Savings
		switch (aTimeOffset[1])					
			{
		case 'M' : // GMT
		case 'T' : // UT
			{
			return;
			}
		case 'S': // Standard Time (Daylight Savings Time is one hour ahead);
			offsetMinutes -= 60;
			break;
		case 'D' : // Daylight Savings Time
			break;
		default:
			// Must be either corrupt or an explicit HHMM +/- offset
			ParseHHMMTimeOffsetL(aTimeOffset, aOffsetMinutes);
			return;
			};

		// Check first letter to find out if its Eastern, Central, Mountain or Pacific Time
		switch (aTimeOffset[0])
			{
		case 'E' : // EST or EDT -4
			offsetMinutes -=240;
			break;
		case 'C' : // CST or CDT -5
			offsetMinutes -=300;
			break;
		case 'M' : // MST or MDT -6
			offsetMinutes -= 360;
			break;
		case 'P' : // PST or PDT -7
			offsetMinutes -=420;
			break;
		default:
			// either the format is invalid or it is a non standard 3 letter timezone
			// in either case 
			return;
			};
		}
	else // This must be a military single letter format time zone
		// According to RFC2882 These Military Timezones are obsolete. Also since A can be either +1 or -1 depending on the specification
		// it should be ignored unless there is additional Out of band Information
		{
		return; // Do nothing
		}

	aOffsetMinutes = offsetMinutes;
	}

/**
	@param			aDateTimeText 
	@param			aDateTime 
 */
void TInternetDateParser::ParseHHMMSSTimeL(const TDesC8& aDateTimeText, TDateTime& aDateTime)
	{
	if (aDateTimeText.Length() < KHHMMSSFormatLength)  // Must be at least HH:MM:SS
		User::Leave(KErrCorrupt);
	TInt num=0;
	TInt consumed = InetProtTextUtils::ConvertDescriptorToInt(aDateTimeText, num);	
	if (consumed != 2 || num < 0 || num > 23)
		User::Leave(KErrCorrupt);
	TPtrC8 dateTime(aDateTimeText.Right(aDateTimeText.Length() -3)); // Remove HH:
	aDateTime.SetHour(num);

	consumed = InetProtTextUtils::ConvertDescriptorToInt(dateTime, num);
	if (consumed != 2 || num < 0 || num > 59)
		User::Leave(KErrCorrupt);
	dateTime.Set(dateTime.Right(dateTime.Length() -3)); // Remove MM:
	aDateTime.SetMinute(num);

	consumed = InetProtTextUtils::ConvertDescriptorToInt(dateTime, num);
	if (consumed != 2 || num < 0 || num > 59)
		User::Leave(KErrCorrupt);
	aDateTime.SetSecond(num);
	}
	
/**
	@param			aDateTimeText 
	@param			aDateTime 
	@return
 */
TInt TInternetDateParser::ParseRfcDateDayMonthYearL(const TDesC8& aDateTimeText, TDateTime& aDateTime)
	{
	// Parse RFC 1123 and RFC 850 style date month year parts
	// 06 Nov 1994	(RFC 1123)
	// 06-Nov-94	(RFC 850) In this case if YY < 50 then treat as 20YY otherwise 19YY
	TInt day =0;
	TInt consumed = InetProtTextUtils::ConvertDescriptorToInt(aDateTimeText,day);
	if ((consumed !=1 && consumed != 2) || aDateTimeText.Length() <= 2)
		User::Leave(KErrCorrupt);

	--day; // Symbian OS month days start from 0

	// Skip over DD and separator
	TPtrC8 dateTimeText(aDateTimeText.Right(aDateTimeText.Length()-(consumed+1)));
	TMonth month = GetMonthL(dateTimeText);
	// Skip over MMM and separator
	const TInt KConsumedMonthLength = 4;
	TInt rightLength = dateTimeText.Length() - KConsumedMonthLength ;
	if( rightLength < 0)
		{
		User::Leave(KErrCorrupt);	
		}
	dateTimeText.Set(dateTimeText.Right(rightLength)); 

	// Now get year
	TInt year;
	consumed = InetProtTextUtils::ConvertDescriptorToInt(dateTimeText, year);
	if (consumed != 4 && consumed !=2)
		User::Leave(KErrCorrupt);

	// check for YY and adjust
	if (consumed == 2)
		year += year > 50 ? 1900 : 2000;

	if (aDateTime.Set(year, month, day, 0, 0, 0, 0) != KErrNone)
		User::Leave(KErrCorrupt);

	return dateTimeText.Length() - (consumed +1);
	}

/**
	@param			aMonthText 
	@return 
 */
TMonth TInternetDateParser::GetMonthL(const TDesC8& aMonthText)
	{
	if (aMonthText.Length() < 3)
		User::Leave(KErrCorrupt);
	
	switch (aMonthText[0])
		{
	case 'J':		// Jan, Jun, Jul
		return (aMonthText[1] == 'a' ? EJanuary : aMonthText[2] == 'n' ? EJune : EJuly);	
	case 'F':		// Feb
		return EFebruary;
	case 'M':		// Mar, May
		return (aMonthText[2] == 'r' ? EMarch : EMay);
	case 'A':		// Apr, Aug
		return (aMonthText[1] == 'p' ? EApril : EAugust);
	case 'S':		// Sep
		return ESeptember;
	case 'O':		// Oct
		return EOctober;
	case 'N':		// Nov
		return ENovember;
	case 'D':		// Dec
		return EDecember;
	default:
		User::Leave(KErrCorrupt);
		};
	return EJanuary;
	}


/**
	@param			aAscTimeDateTime 
	@param			aDateTime
 */
void TInternetDateParser::ConvertFromAscTimeFormL(const TDesC8& aAscTimeDateTime, TDateTime& aDateTime)
	{
	// Convert a date in ascTime format ( Sun Nov  6 08:49:37 1994 )

	const TChar KSpace = ' ';

	// First remove the wkDay
	TPtrC8 dateTime(aAscTimeDateTime);
	InetProtTextUtils::RemoveWhiteSpace(dateTime, InetProtTextUtils::ERemoveBoth);
	TInt endOfWkDayLoc = dateTime.Locate(KSpace);
	if (endOfWkDayLoc != 3) // Must be following a wkDay "Sun "
		User::Leave(KErrCorrupt);
	dateTime.Set(dateTime.Right(dateTime.Length() - 3));
	InetProtTextUtils::RemoveWhiteSpace(dateTime, InetProtTextUtils::ERemoveLeft);
	// Now parse the month then skip over MMM
	TMonth month = GetMonthL(dateTime);
	dateTime.Set(dateTime.Right(dateTime.Length() - 3));
	InetProtTextUtils::RemoveWhiteSpace(dateTime, InetProtTextUtils::ERemoveLeft);
	// Now parse the monthDay
	TInt day = 0;
	TInt consumed = InetProtTextUtils::ConvertDescriptorToInt(dateTime, day);
	if (consumed < 0)
		User::Leave(KErrCorrupt);

	dateTime.Set(dateTime.Right(dateTime.Length() - consumed));
	InetProtTextUtils::RemoveWhiteSpace(dateTime, InetProtTextUtils::ERemoveLeft);

	// Now get the time
	ParseHHMMSSTimeL(dateTime, aDateTime);
	dateTime.Set(dateTime.Right(dateTime.Length() - KHHMMSSFormatLength));	
	// Now the year
	InetProtTextUtils::RemoveWhiteSpace(dateTime, InetProtTextUtils::ERemoveLeft);
	TInt year;
	consumed = InetProtTextUtils::ConvertDescriptorToInt(dateTime, year);
	if (consumed !=4)
		User::Leave(KErrCorrupt);

	if (aDateTime.Set(year, month, day-1, aDateTime.Hour(), aDateTime.Minute(), aDateTime.Second(), 0) != KErrNone)
		User::Leave(KErrCorrupt);
	}
