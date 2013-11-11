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
// This file contains implementation for classes defined in tinternetdate.h
// 
//

/**
 @file tinternetdate.cpp
 @see	tinternetdate.h
*/
 

#include "tinternetdate.h"
#include "inetprottextutils.h"
#include "tinternetdateparser.h"


/**
	Used to set a date which is in a Internet form.
	
	@since			7.0
	@param			aDate	the text form of the date			.
	@leave			KErrCorrupt, if the date is not in the correct format
*/
EXPORT_C void TInternetDate::SetDateL(const TDesC8& aDate)
	{
	TInternetDateParser dateParser;

	TPtrC8 datePtr(aDate);
	// Remove the any leading and trailing whitespaces from the date.
	InetProtTextUtils::RemoveWhiteSpace(datePtr, InetProtTextUtils::ERemoveBoth);

	dateParser.ConvertFromInternetFormL(datePtr, iDateTime);
	}

/**
	Used to access the date/time
	
	@since			7.0
	@return			const TDateTime& The time stored in this object
*/
EXPORT_C const TDateTime& TInternetDate::DateTime()
	{
	return iDateTime;
	}

/**
	Used to access the date/time in a text formate
	
	@since			7.0
	@param			aInternetDateFormat The format of the date. Only RFC1123 dates are supported
	@return			HBufC8* A descriptor containing the date/time in the format 
					specifed by the parameter aInternetDateFormat
	@leave			KErrNoMemory
*/
EXPORT_C HBufC8* TInternetDate::InternetDateTimeL(TInternetDateFormat /*aInternetDateFormat*/)
	{
	TInternetDateParser dateParser;
	return dateParser.ConvertToRfc1123FormL(iDateTime);
	}
