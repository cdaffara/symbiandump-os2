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

#ifndef __DELIMITEDQUERY8_H__
#define __DELIMITEDQUERY8_H__

/**
	@file DelimitedQuery8.h
	Comments :	This file contains the API definition for the classes TDelimitedQueryParser8
				and CDelimitedQuery8. 
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
querys delimited by a '&' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class TDelimitedQueryParser8 : public TDelimitedParserBase8
	{
public:	// Methods

	IMPORT_C TDelimitedQueryParser8();

	IMPORT_C void Parse(const TDesC8& aQuery);

	IMPORT_C void ParseReverse(const TDesC8& aQuery);
	};

/**
Dependencies : CDelimitedStringBase8
Comments : Provides functionality to create a delimited query where components of the 
query delimited by '&' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class CDelimitedQuery8 : public CDelimitedDataBase8
	{
public:	// Methods

	IMPORT_C static CDelimitedQuery8* NewL(const TDesC8& aQuery);

	IMPORT_C static CDelimitedQuery8* NewLC(const TDesC8& aQuery);

	IMPORT_C ~CDelimitedQuery8();

	IMPORT_C void InsertAndEscapeCurrentL(const TDesC8& aSegment);

	IMPORT_C void PushAndEscapeFrontL(const TDesC8& aSegment);

	IMPORT_C void PushAndEscapeBackL(const TDesC8& aSegment);

private:	// Methods

	CDelimitedQuery8();

	void ConstructL(const TDesC8& aQuery);

	};

#endif	// __DELIMITEDQUERY8_H__
