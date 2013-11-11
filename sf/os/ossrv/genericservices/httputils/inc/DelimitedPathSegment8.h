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

#ifndef __DELIMITEDPATHSEGMENT8_H__
#define __DELIMITEDPATHSEGMENT8_H__

/**
	@file DelimitedPathSegment8.h
	Comments :	This file contains the API definition for the classes TDelimitedPathSegmentParser8
				and CDelimitedPathSegment8. 
	@publishedAll
	@released
 */

// System includes
//
#include <e32base.h>
#include <delimitedparser8.h>


/**
Dependencies : TDelimitedParserBase8
Comments : Derived class from TDelimitedParserBase providing a class for parsing 
path segments delimited by a ';' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class TDelimitedPathSegmentParser8 : public TDelimitedParserBase8
	{
public:	// Methods


	IMPORT_C TDelimitedPathSegmentParser8();

	IMPORT_C void Parse(const TDesC8& aPathSegment);

	IMPORT_C void ParseReverse(const TDesC8& aPathSegment);
	};

/**
Dependencies : CDelimitedStringBase8
Comments : Provides functionality to create a delimited path segment where components of the 
path segment delimited by '/' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class CDelimitedPathSegment8 : public CDelimitedDataBase8
	{
public:	// Methods

	IMPORT_C static CDelimitedPathSegment8* NewL(const TDesC8& aPathSegment);

	IMPORT_C static CDelimitedPathSegment8* NewLC(const TDesC8& aPathSegment);

	IMPORT_C ~CDelimitedPathSegment8();

	IMPORT_C void InsertAndEscapeCurrentL(const TDesC8& aParam);

	IMPORT_C void PushAndEscapeFrontL(const TDesC8& aParam);

	IMPORT_C void PushAndEscapeBackL(const TDesC8& aParam);

private:	// Methods

	CDelimitedPathSegment8();

	void ConstructL(const TDesC8& aPathSegment);

	};

#endif // __DELIMITEDPATHSEGMENT8_H__
