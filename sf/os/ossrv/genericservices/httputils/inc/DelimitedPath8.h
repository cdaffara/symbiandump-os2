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

#ifndef __DELIMITEDPATH8_H__
#define __DELIMITEDPATH8_H__

/**
	@file DelimitedPath8.h
	Comments :	This file contains the API definition for the classes 
				TDelimitedPathParser8 and CDelimitedPath16. 
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
paths delimited by a '/' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class TDelimitedPathParser8 : public TDelimitedParserBase8
	{
public:	// Methods

	IMPORT_C TDelimitedPathParser8();

	IMPORT_C void Parse(const TDesC8& aPath);

	IMPORT_C void ParseReverse(const TDesC8& aPath);
	};

/**
Dependencies : CDelimitedStringBase8
Comments : Provides functionality to create a delimited path where components of the 
path delimited by '/' as defined in RFC2396.
@publishedAll
@released
@since 6.0
*/
class CDelimitedPath8 : public CDelimitedDataBase8
	{
public:	// Methods

/**
	Static factory constructor. Uses two phase construction and leaves nothing
	on the CleanupStack.
	@since			6.0
	@param			aPath	A descriptor with the initial path.
	@return			A pointer to created object.
	@post			Nothing left on the CleanupStack.
 */
	IMPORT_C static CDelimitedPath8* NewL(const TDesC8& aPath);

/**
	Static factory constructor. Uses two phase construction and leaves a
	pointer to created object on the CleanupStack.
	@since			6.0
	@param			aPath	A descriptor with the initial path.
	@return			A pointer to created object.
	@post			Pointer to created object left of CleanupStack.
 */
	IMPORT_C static CDelimitedPath8* NewLC(const TDesC8& aPath);

/**
	Destructor.
	@since			6.0
 */
	IMPORT_C ~CDelimitedPath8();

/**
	Escape encodes the segment then inserts the escaped version in a 
	position before the current parsed segment. The new segment should only contain a
	single path segment, as any path delimiters in the segment will be converted to an
	escape triple. The parser is left in a state where its current segment is the same
	one as before the insertion.
	@since			6.0
	@param			aSegment	A descriptor with the unescaped path segment.
	@pre 			The path must have been initially parsed.
	@post			The path will have been extended to include the new segment. The 
	current segment will remain as the one before the insertion.
 */
	IMPORT_C void InsertAndEscapeCurrentL(const TDesC8& aSegment);

/**
	Escape encodes the segment then inserts the escaped version at 
	the front of the path. The new segment should only contain a single path segment, 
	as any path delimiters in the segment will be converted to an escape triple. The 
	parser is left in a state where its current segment is the same one as before 
	the insertion.
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the unescaped path segment.
	@pre 			The delimiter must have been set. 
	@post			The path will have been extended to include the new segment.
 */
	IMPORT_C void PushAndEscapeFrontL(const TDesC8& aSegment);

/**
	Escape encodes the segment then inserts the escaped version at 
	the back of the path. The new segment should only contain a single path segment, 
	as any path delimiters in the segment will be converted to an escape triple. The 
	parser is left in a state where its current segment is the same one as before 
	the insertion.
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the unescaped path segment.
	@pre 			The delimiter must have been set. 
	@post			The path will have been extended to include the new segment.
 */
	IMPORT_C void PushAndEscapeBackL(const TDesC8& aSegment);

private:	// Methods

/**
	Constructor. First phase of two-phase construction method. Does
	non-allocating construction.
	@since			6.0
 */
	CDelimitedPath8();

/**
	Second phase of two-phase construction method. Does any allocations required
	to fully construct the object.
	@since			6.0
	@param			aPath	A descriptor with the initial path.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed.
 */
	void ConstructL(const TDesC8& aPath);

	};

#endif	// __DELIMITEDPATH8_H__
