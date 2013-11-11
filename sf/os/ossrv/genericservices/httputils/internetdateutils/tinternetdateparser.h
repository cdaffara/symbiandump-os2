// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definition of internal classes used to parse TInternetDate data
// 
//

/**
 @file tinternetdateparser.h
*/


#ifndef		__TINTERNETDATEPARSER_H__
#define		__TINTERNETDATEPARSER_H__

#include <e32std.h>

/**
	Used as a support class of TInternetDate. This provides the implementation of parsing

	Supports parsing of the following date formats:
	Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
    Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036
    Sun Nov  6 08:49:37 1994       ; ANSI C's asctime() format

	All dates are store relative to Universal Time and not local time. 

	@internalAll
	@since 7.0
*/
class TInternetDateParser
	{
public:
	inline TInternetDateParser();
		
	void ConvertFromInternetFormL(const TDesC8& aInternetTextDateTime, TDateTime& aDateTime);

	HBufC8* ConvertToRfc1123FormL(const TDateTime& aDateTime);
private:

	void ConvertFromRfc1123And850FormL(const TDesC8& aRfc1123DateTime, TDateTime& aDateTime);
	void ConvertFromAscTimeFormL(const TDesC8& aAscTimeDateTime, TDateTime& aDateTime);
	TInt ParseRfcDateDayMonthYearL(const TDesC8& aDateTimeText, TDateTime& aDateTime);
	TMonth GetMonthL(const TDesC8& aMonthText);
	void ParseHHMMSSTimeL(const TDesC8& aDateTimeText, TDateTime& aDateTime);
	void ParseTimeOffsetL(const TDesC8& aTimeOffset, TTimeIntervalMinutes& aOffsetMinutes);
	void ParseHHMMTimeOffsetL(const TDesC8& aTimeOffset, TTimeIntervalMinutes& aOffsetMinutes);
	};


inline TInternetDateParser::TInternetDateParser()
	{}



#endif	//	__TINTERNETDATEPARSER_H__
