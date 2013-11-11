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

#ifndef __DELIMITEDPATH16_H__
#define __DELIMITEDPATH16_H__

/**
	@file DelimitedPath16.h
	Comments :	This file contains the API definition for the classes 
				TDelimitedPathParser16 and CDelimitedPath16. 
	@publishedAll
	@released	
 */

// System includes
//
#include <e32base.h>
#include <delimitedparser16.h>


/**
Dependencies : TDelimitedParserBase16
Comments : Derived class from TDelimitedParserBase providing a class for parsing paths
delimited by '/' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class TDelimitedPathParser16 : public TDelimitedParserBase16
	{
public:	// Methods

	IMPORT_C TDelimitedPathParser16();

	IMPORT_C void Parse(const TDesC16& aPath);

	IMPORT_C void ParseReverse(const TDesC16& aPath);
	};

/**
Dependencies : CDelimitedStringBase16
Comments : Provides functionality to create a delimited path where components of the 
path delimited by '/' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class CDelimitedPath16 : public CDelimitedDataBase16
	{
public:	// Methods

	IMPORT_C static CDelimitedPath16* NewL(const TDesC16& aPath);

	IMPORT_C static CDelimitedPath16* NewLC(const TDesC16& aPath);

	IMPORT_C ~CDelimitedPath16();

	IMPORT_C void InsertAndEscapeCurrentL(const TDesC16& aSegment);

	IMPORT_C void PushAndEscapeBackL(const TDesC16& aSegment);
	
	IMPORT_C void PushAndEscapeFrontL(const TDesC16& aSegment);

private:	// Methods

	CDelimitedPath16();

	void ConstructL(const TDesC16& aPath);

	};

/**
typedef 
@publishedAll
@released
*/
typedef TDelimitedPathParser16 TDelimitedPathParser;

/**
typedef 
@publishedAll
@released
*/
typedef CDelimitedPath16 CDelimitedPath;

#endif	// __DELIMITEDPATH16_H__


