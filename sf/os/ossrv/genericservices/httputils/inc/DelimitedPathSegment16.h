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

#ifndef __DELIMITEDPATHSEGMENT16_H__
#define __DELIMITEDPATHSEGMENT16_H__

/**
	@file DelimitedPathSegment16.h
	Comments :	This file contains the API definition for the classes TDelimitedPathSegmentParser16
				and CDelimitedPathSegment16.
	@publishedAll
	@released 
 */

// System includes
//
#include <e32base.h>
#include <delimitedparser16.h>


/**
Dependencies : TDelimitedParserBase16
Comments : Derived class from TDelimitedParserBase providing a class for parsing path segments
delimited by '/' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class TDelimitedPathSegmentParser16 : public TDelimitedParserBase16
	{
public:	// Methods

	IMPORT_C TDelimitedPathSegmentParser16();

	IMPORT_C void Parse(const TDesC16& aPathSegment);

	IMPORT_C void ParseReverse(const TDesC16& aPathSegment);
	};

/**
Dependencies : CDelimitedStringBase16
Comments : Provides functionality to create a delimited path segment where components of the 
path segment delimited by '/' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class CDelimitedPathSegment16 : public CDelimitedDataBase16
	{
public:	// Methods

	IMPORT_C static CDelimitedPathSegment16* NewL(const TDesC16& aPathSegment);

	IMPORT_C static CDelimitedPathSegment16* NewLC(const TDesC16& aPathSegment);

	IMPORT_C ~CDelimitedPathSegment16();

	IMPORT_C void InsertAndEscapeCurrentL(const TDesC16& aParam);

	IMPORT_C void PushAndEscapeBackL(const TDesC16& aParam);
	
	IMPORT_C void PushAndEscapeFrontL(const TDesC16& aParam);

private:	// Methods

	CDelimitedPathSegment16();

	void ConstructL(const TDesC16& aPathSegment);

	};

/**
typedef 
@publishedAll
@released
*/
typedef TDelimitedPathSegmentParser16	TDelimitedPathSegmentParser;

/**
typedef 
@publishedAll
@released
*/
typedef CDelimitedPathSegment16			CDelimitedPathSegment;

#endif	// __DELIMITEDPATHSEGMENT16_H__
