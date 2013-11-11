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

#include <delimitedpath8.h>
#include <delimitedpath16.h>
#include <escapeutils.h>

//
//
// Implemetation of CDelimitedPath8
//
//

/**
	Static factory constructor. Uses two phase construction and leaves 
	nothing on the CleanupStack. 
	
	@param			aPath	A descriptor with the initial path.
	@return			A pointer to the newly created object. 
 */
EXPORT_C CDelimitedPath8* CDelimitedPath8::NewL(const TDesC8& aPath)
	{
	CDelimitedPath8* self = NewLC(aPath);
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer 
	to created object on the CleanupStack. 

	@param			aPath	A descriptor with the initial path.
	@return			A pointer to the newly created object.
 */
EXPORT_C CDelimitedPath8* CDelimitedPath8::NewLC(const TDesC8& aPath)
	{
	CDelimitedPath8* self = new (ELeave) CDelimitedPath8;
	CleanupStack::PushL(self);
	self->ConstructL(aPath);
	return self;
	}

/**
	Destructor. 
*/
EXPORT_C CDelimitedPath8::~CDelimitedPath8()
	{
	}

/**
	Escape encodes the segment then inserts the escaped version in a position before 
	the current parsed segment. The new segment should only contain a single path segment, 
	as any path delimiters in the segment will be converted to an escape triple. 
	The parser is left in a state where its current segment is the same one as before the insertion.

	@pre	The path must have been initially parsed.
	@param	aSegment	A descriptor with the unescaped path segment.
	@post	The path will have been extended to include the new segment. 
			The current segment will remain as the one before the insertion.
 */
EXPORT_C void CDelimitedPath8::InsertAndEscapeCurrentL(const TDesC8& aSegment)
	{
	// Create escaped version of the segment
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aSegment, EscapeUtils::EEscapePath);
	CleanupStack::PushL(escaped);

	// Insert the segment
	InsertCurrentL(*escaped);

	// Cleanup
	CleanupStack::PopAndDestroy(escaped);
	}

/**
	Escape encodes the segment then inserts the escaped version at the back of the path. 
	The new segment should only contain a single path segment, as any path delimiters in 
	the segment will be converted to an escape triple. The parser is left in a state 
	where its current segment is the same one as before the insertion. 

	@pre	The delimiter must have been set.
	@param	aSegment	A descriptor with the unescaped path segment.
	@post	The path will have been extended to include the new segment
 */
EXPORT_C void CDelimitedPath8::PushAndEscapeBackL(const TDesC8& aSegment)
	{
	// Create escaped version of the segment
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aSegment, EscapeUtils::EEscapePath);
	CleanupStack::PushL(escaped);

	// Insert the segment
	PushBackL(*escaped);

	// Cleanup
	CleanupStack::PopAndDestroy(escaped);
	}

/**
	Escape encodes the segment then inserts the escaped version at the front of the path. 
	The new segment should only contain a single path segment, as any path delimiters in 
	the segment will be converted to an escape triple. The parser is left in a state where 
	its current segment is the same one as before the insertion. 

	@pre	The delimiter must have been set. 
	@param	aSegment	A descriptor with the unescaped path segment.
	@post	The path will have been extended to include the new segment
 */
EXPORT_C void CDelimitedPath8::PushAndEscapeFrontL(const TDesC8& aSegment)
	{
	// Create escaped version of the segment
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aSegment, EscapeUtils::EEscapePath);
	CleanupStack::PushL(escaped);

	// Insert the segment
	PushFrontL(*escaped);

	// Cleanup
	CleanupStack::PopAndDestroy(escaped);
	}

/**
	Constructor.
*/
CDelimitedPath8::CDelimitedPath8()
: CDelimitedDataBase8()
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully construct 
	the object.

	@param			aPath	A descriptor with the initial string.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed.
*/
void CDelimitedPath8::ConstructL(const TDesC8& aPath)
	{
	// Call base class ConstructL()
	CDelimitedDataBase8::ConstructL(aPath);

	// Set the delimiter to '/'
	SetDelimiter(TChar('/'));
	}

//
//
// Implemetation of CDelimitedPath16
//
//

/**
	Static factory constructor. Uses two phase construction and leaves nothing on 
	the CleanupStack.
						
	@since			6.0
	@param			aPath	A descriptor with the initial path.
	@return			A pointer to created object.
	@post			Nothing left on the CleanupStack.
 */
EXPORT_C CDelimitedPath16* CDelimitedPath16::NewL(const TDesC16& aPath)
	{
	CDelimitedPath16* self = NewLC(aPath);
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to 
	created object on the CleanupStack.
						
	@since			6.0
	@param			aPath	A descriptor with the initial path.
	@return			A pointer to created object.
	@post			Pointer to created object left of CleanupStack.
*/
EXPORT_C CDelimitedPath16* CDelimitedPath16::NewLC(const TDesC16& aPath)
	{
	CDelimitedPath16* self = new (ELeave) CDelimitedPath16;
	CleanupStack::PushL(self);
	self->ConstructL(aPath);
	return self;
	}

/**
	Destructor.
	
	@since			6.0
*/
EXPORT_C CDelimitedPath16::~CDelimitedPath16()
	{
	}

/**
	Escape encodes the segment then inserts the escaped version in a position before the 
	current parsed segment. The new segment should only contain a single path segment, as 
	any path delimiters in the segment will be converted to an escape triple. The parser 
	is left in a state where its current segment is the same one as before the insertion.
						
	@since			6.0
	@param			aSegment	A descriptor with the unescaped path segment.
	@pre 			The path must have been initially parsed.
	@post			The path will have been extended to include the new segment. The 
	current segment will remain as the one before the insertion.
*/
EXPORT_C void CDelimitedPath16::InsertAndEscapeCurrentL(const TDesC16& aSegment)
	{
	// Need to convert to utf8 first
	HBufC8* utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aSegment);
	CleanupStack::PushL(utf8);

	// Create escaped version of component
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(*utf8, EscapeUtils::EEscapePath);
	CleanupStack::PushL(escaped);

	// Convert back to 16-bits
	HBufC16* converted = HBufC16::NewLC(escaped->Length());
	converted->Des().Copy(*escaped);

	// Insert the segment
	InsertCurrentL(*converted);

	// Cleanup
	CleanupStack::PopAndDestroy(3, utf8);	// utf8, escaped, converted
	}

/**
	Escape encodes the segment then inserts the escaped version at the back of the path. 
	The new segment should only contain a single path segment, as any path delimiters in 
	the segment will be converted to an escape triple. The parser is left in a state where 
	its current segment is the same one as before the insertion.
						
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the unescaped path segment.
	@pre 			The delimiter must have been set. 
	@post			The path will have been extended to include the new segment.
*/
EXPORT_C void CDelimitedPath16::PushAndEscapeBackL(const TDesC16& aSegment)
	{
	// Need to convert to utf8 first
	HBufC8* utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aSegment);
	CleanupStack::PushL(utf8);

	// Create escaped version of component
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(*utf8, EscapeUtils::EEscapePath);
	CleanupStack::PushL(escaped);

	// Convert back to 16-bits
	HBufC16* converted = HBufC16::NewLC(escaped->Length());
	converted->Des().Copy(*escaped);

	// Insert the segment
	PushBackL(*converted);

	// Cleanup
	CleanupStack::PopAndDestroy(3, utf8);	// utf8, escaped, converted
	}

/**
	Escape encodes the segment then inserts the escaped version at the front of the path. 
	The new segment should only contain a single path segment, as any path delimiters in 
	the segment will be converted to an escape triple. The parser is left in a state where 
	its current segment is the same one as before the insertion.
						
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the unescaped path segment.
	@pre 			The delimiter must have been set. 
	@post			The path will have been extended to include the new segment.
*/
EXPORT_C void CDelimitedPath16::PushAndEscapeFrontL(const TDesC16& aSegment)
	{
	// Need to convert to utf8 first
	HBufC8* utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aSegment);
	CleanupStack::PushL(utf8);

	// Create escaped version of component
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(*utf8, EscapeUtils::EEscapePath);
	CleanupStack::PushL(escaped);

	// Convert back to 16-bits
	HBufC16* converted = HBufC16::NewLC(escaped->Length());
	converted->Des().Copy(*escaped);

	// Insert the segment
	PushFrontL(*converted);

	// Cleanup
	CleanupStack::PopAndDestroy(3, utf8);	// utf8, escaped, converted
	}

/**
	Constructor. First phase of two-phase construction method. Does non-allocating construction.
						
	@since			6.0
*/
CDelimitedPath16::CDelimitedPath16()
: CDelimitedDataBase16()
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully construct 
	the object.
						
	@since			6.0
	@param			aPath	A descriptor with the initial path.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed.
*/
void CDelimitedPath16::ConstructL(const TDesC16& aPath)
	{
	// Call base class ConstructL()
	CDelimitedDataBase16::ConstructL(aPath);

	// Set the delimiter to '/'
	SetDelimiter(TChar('/'));
	}
