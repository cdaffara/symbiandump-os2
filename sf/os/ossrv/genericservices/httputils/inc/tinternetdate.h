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
// This file contains the class TInternetDate
// 
//

/**
 @file tinternetdate.h
 @publishedAll
 @released
*/
  

#ifndef		__TINTERNETDATE_H__
#define		__TINTERNETDATE_H__

#include <e32std.h>

/**
Stores dates in universal time and provides parsing of internet style dates 
into TDateTime and RFC 1123 (updates RFC 822) dates.

Supports parsing of the following date formats:
Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036
Sun Nov  6 08:49:37 1994       ; ANSI C's asctime() format

In case of Date Formats with offsets "Mon, 21 Jan 2002 23:00:00 +0210" , equivalent 
time in UTC can be determined by substracting the offset from the local time (RFC 3339).

All dates are store relative to Universal Time and not local time. 

@publishedAll
@released
@since 7.0
*/
class TInternetDate
	{
public:	
/** 
	Enum defining internet date formats.
	@publishedAll
	@released
	@since 9.1	
 */
	enum TInternetDateFormat
		{
		/** 
			Rfc 1123 Compliant date format 
		*/
		ERfc1123Format
		};

	inline TInternetDate();


	inline TInternetDate(TDateTime aUniversalTime);

	IMPORT_C void SetDateL(const TDesC8& aDate);

	IMPORT_C const TDateTime& DateTime();

	IMPORT_C HBufC8* InternetDateTimeL(TInternetDateFormat aInternetDateFormat);

private:
	/**
	A date and time object
	*/
	TDateTime iDateTime;
	};


/**
	Intended Usage:	Default Constructor. Used when setting dates that are in a text format using SetDateL
	
	@since			7.0	.	
*/
inline TInternetDate::TInternetDate()
	{}

/**
	Intended Usage:	Constructor. Used when setting a date that you later want parsed into an internet text format
	
	@since			7.0
	@param			aUniversalTime	a time that should correspond to Universal Time.
*/
inline TInternetDate::TInternetDate(TDateTime aUniversalTime) :
	iDateTime(aUniversalTime)
	{}


#endif	//	__TINTERNETDATE_H__
