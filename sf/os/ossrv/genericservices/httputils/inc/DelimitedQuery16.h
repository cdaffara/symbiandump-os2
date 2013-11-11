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

#ifndef __DELIMITEDQUERY16_H__
#define __DELIMITEDQUERY16_H__

/**
	@file DelimitedQuery16.h
	Comments :	This file contains the API definition for the classes TDelimitedQueryParser16
				and CDelimitedQuery16.
	@publishedAll
	@released
 */

// System includes
//
#include <e32base.h>
#include <delimitedparser16.h>


/**
Dependencies : TDelimitedParserBase16
Comments : Derived class from TDelimitedParserBase providing a class for parsing querys
delimited by '&' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class TDelimitedQueryParser16 : public TDelimitedParserBase16
	{
public:	// Methods

	IMPORT_C TDelimitedQueryParser16();

	IMPORT_C void Parse(const TDesC16& aQuery);

	IMPORT_C void ParseReverse(const TDesC16& aQuery);
	};

/**
Dependencies : CDelimitedStringBase16
Comments : Provides functionality to create a delimited query where components of the 
query delimited by '&' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class CDelimitedQuery16 : public CDelimitedDataBase16
	{
public:	// Methods

	IMPORT_C static CDelimitedQuery16* NewL(const TDesC16& aQuery);

	IMPORT_C static CDelimitedQuery16* NewLC(const TDesC16& aQuery);

	IMPORT_C ~CDelimitedQuery16();

	IMPORT_C void InsertAndEscapeCurrentL(const TDesC16& aSegment);

	IMPORT_C void PushAndEscapeFrontL(const TDesC16& aSegment);

	IMPORT_C void PushAndEscapeBackL(const TDesC16& aSegment);

private:	// Methods

	CDelimitedQuery16();

	void ConstructL(const TDesC16& aQuery);

	};

/**
typedef 
@publishedAll
@released
*/
typedef TDelimitedQueryParser16	TDelimitedQueryParser;

/**
typedef 
@publishedAll
@released
*/
typedef CDelimitedQuery16		CDelimitedQuery;

#endif	// __DELIMITEDQUERY16_H__
