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
// 
// 
//

/**
 @file UriUtilsCommon.h
 @publishedAll
 @released
*/

#ifndef __URIUTILSCOMMON_H__
#define __URIUTILSCOMMON_H__

// System includes
//
#include <e32base.h>

/** Error Base value */
const TInt KUriUtilsErrorBase		= -5000;

/**	
enum TUriError
Enum defining the error and panic codes used in the UriUtils components.
@publishedAll
@released
@since 6.0
*/
enum TUriUtilsError
	{
	/** Error specifying the use of an invalid component index */
	KUriUtilsErrBadComponentIndex			= KUriUtilsErrorBase,
	/** Error specifying the use of a base uri path is not absolute and not empty */
	KUriUtilsErrBadBasePath					= KUriUtilsErrorBase - 1,
	/** Error specifying that a buffer is not big enough to append a component and delimiters */
	KUriUtilsErrBufferOverflow				= KUriUtilsErrorBase - 2,
	/** Error specifying the use of an supported parsing mode */
	KUriUtilsErrBadDelimitedParserMode		= KUriUtilsErrorBase - 3,
	/** Error specifying that the delimiting character is not set */
	KUriUtilsErrNoDelimiter					= KUriUtilsErrorBase - 4,
	/** Error specifying that the data has not been parsed */
	KUriUtilsErrNotParsed					= KUriUtilsErrorBase - 5,
	/** Error specifying that a 16-bit character was found in data to be escape encode */
	KUriUtilsErr16BitChar					= KUriUtilsErrorBase - 6,
	/** Error specifying that a badly formed escape triple was found in data being escape decoded */
	KUriUtilsErrBadEscapeTriple				= KUriUtilsErrorBase - 7,
	/** Error specifying that charconv was unable to convert the input data */
	KUriUtilsCannotConvert					= KUriUtilsErrorBase - 8,
	/** Error specifying that there	was empty data */
	KUriUtilsErrEmptyData					= KUriUtilsErrorBase - 9,
	/** Error specifying that there is no character representation of an integer */
	KUriUtilsErrNoIntegerInData			= KUriUtilsErrorBase - 10,
	/** Error code specifying an invalid uri */
	KUriUtilsErrInvalidUri					= KUriUtilsErrorBase - 11,
	/** Error code for unsupported escaping mode */
	KUriUtilsErrBadEscapeMode				= KUriUtilsErrorBase - 12,
	/** Error code for unsupported text whitespace removal mode */
	KUriUtilsErrBadTextRemoveMode			= KUriUtilsErrorBase - 13,
	/**	Quoted string could not be decoded as it is malformed. */
	KUriUtilsErrDecodeMalformedQuotedString	= KUriUtilsErrorBase - 14,
	/** Error code specifying an invalid scheme */
	KUriUtilsErrInvalidScheme				= KUriUtilsErrorBase - 15,
	/** Error code specifying an invalid username and/or password */
	KUriUtilsErrInvalidUserInfo				= KUriUtilsErrorBase - 16,
	/** Error code specifying an invalid host */
	KUriUtilsErrInvalidHost					= KUriUtilsErrorBase - 17,
	/** Error code specifying an invalid port */
	KUriUtilsErrInvalidPort					= KUriUtilsErrorBase - 18,
	/** Error code specifying an invalid path */
	KUriUtilsErrInvalidPath					= KUriUtilsErrorBase - 19,
	/** Error code specifying an invalid sip parameter */
	KUriUtilsErrInvalidParam				= KUriUtilsErrorBase - 20,
	/** Error code specifying an invalid query */
	KUriUtilsErrInvalidQuery				= KUriUtilsErrorBase - 21,
	/** Error code specifying an invalid sip headers */
	KUriUtilsErrInvalidHeaders				= KUriUtilsErrorBase - 22,
	/** Error code specifying an invalid fragment */
	KUriUtilsErrInvalidFragment				= KUriUtilsErrorBase - 23,
	/** Error code specifying that schemes are not equivalent */
	KUriUtilsErrDifferentScheme				= KUriUtilsErrorBase - 24, 
	/** Error code specifying that usernames and passwords are not equivalent */
	KUriUtilsErrDifferentUserInfo			= KUriUtilsErrorBase - 25,
	/** Error code specifying that hosts are not equivalent */
	KUriUtilsErrDifferentHost				= KUriUtilsErrorBase - 26,
	/** Error code specifying that ports are not equivalent */
	KUriUtilsErrDifferentPort				= KUriUtilsErrorBase - 27,
	/** Error code specifying that paths are not equivalent */
	KUriUtilsErrDifferentPath				= KUriUtilsErrorBase - 28,
	/** Error code specifying that queries are not equivalent */
	KUriUtilsErrDifferentQuery				= KUriUtilsErrorBase - 30,
	/** Error code specifying that fragments are not equivalent */
    KUriUtilsErrDifferentFragment			= KUriUtilsErrorBase - 32,
	/** Error code specifying that a non-numeric character was found in data */
    KUriUtilsErrMalformedInteger			= KUriUtilsErrorBase - 33
	};

// Constants
//
/** Scheme Delimiter */
const TInt KSchemeDelimiter		= ':';
/** Userinfo Delimiter */
const TInt KUserinfoDelimiter	= '@';
/** Equals Separator */
const TInt KUserPwdSeparator	= ':';
/** close Brace */
const TInt KEqualsSeparator		= '=';
/** Slash Delimiter */
const TInt KPortDelimiter		= ':';
/** close Brace */
const TInt KSlashDelimiter		= '/';
/** Fragment Delimiter */
const TInt KQueryDelimiter		= '?';
/** close Brace */
const TInt KFragmentDelimiter	= '#';
/** param delimiter */
const TInt KParamDelimiter		= ';';
/** open Brace */
const TInt KIPv6UriOpenBrace	= '[';
/** close Brace */
const TInt KIPv6UriCloseBrace	= ']';

#endif	// __URIUTILSCOMMON_H__
