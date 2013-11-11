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

#include <delimitedquery8.h>
#include <delimitedquery16.h>
#include <escapeutils.h>

//
//
// Implemetation of CDelimitedQuery8
//
//

/**
	Static factory constructor. Uses two phase construction and leaves nothing on 
	the CleanupStack.
	
	@since			6.0
	@param			aQuery	A descriptor with the initial query.
	@return			A pointer to created object.
	@post			Nothing left on the CleanupStack.
 */
EXPORT_C CDelimitedQuery8* CDelimitedQuery8::NewL(const TDesC8& aQuery)
	{
	CDelimitedQuery8* self = NewLC(aQuery);
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to 
	created object on the CleanupStack.
	
	@since			6.0
	@param			aQuery	A descriptor with the initial query.
	@return			A pointer to created object.
	@post			Pointer to created object left of CleanupStack.
 */
EXPORT_C CDelimitedQuery8* CDelimitedQuery8::NewLC(const TDesC8& aQuery)
	{
	CDelimitedQuery8* self = new (ELeave) CDelimitedQuery8;
	CleanupStack::PushL(self);
	self->ConstructL(aQuery);
	return self;
	}

/**
	Destructor.
	
	@since			6.0
 */
EXPORT_C CDelimitedQuery8::~CDelimitedQuery8()
	{
	}

/**
	Escape encodes segment then inserts the escaped version in a position before the 
	current parsed segment. The new segment should only contain a single query segment, 
	as any query delimiters in the segment will be converted to an escape triple. The 
	parser is left in a state where its current segment is the same one as before the 
	insertion.
	
	@since			6.0
	@param			aSegment	A descriptor with the unescaped query segment.
	@pre 			The query must have been initially parsed.
	@post			The query will have been extended to include the new segment. The 
	current segment will remain as the one before the insertion.
 */
EXPORT_C void CDelimitedQuery8::InsertAndEscapeCurrentL(const TDesC8& aSegment)
	{
	// Create escaped version of the segment
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aSegment, EscapeUtils::EEscapeQuery);
	CleanupStack::PushL(escaped);

	// Insert the segment
	InsertCurrentL(*escaped);

	// Cleanup
	CleanupStack::PopAndDestroy(escaped);
	}

/**
	Escape encodes segment then inserts the escaped version at the back of the query. 
	The new segment should only contain a single query segment, as any query delimiters 
	in the segment will be converted to an escape triple. The parser is left in a state 
	where its current segment is the same one as before the insertion.
	
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the unescaped query segment.
	@pre 			The delimiter must have been set. 
	@post			The query will have been extended to include the new segment.
 */
EXPORT_C void CDelimitedQuery8::PushAndEscapeBackL(const TDesC8& aSegment)
	{
	// Create escaped version of the segment
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aSegment, EscapeUtils::EEscapeQuery);
	CleanupStack::PushL(escaped);

	// Insert the segment
	PushBackL(*escaped);

	// Cleanup
	CleanupStack::PopAndDestroy(escaped);
	}

/**
	Escape encodes segment then inserts the escaped version at the front of the query. 
	The new segment should only contain a single query segment, as any query delimiters 
	in the segment will be converted to an escape triple. The parser is left in a state 
	where its current segment is the same one as before the insertion.
	
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the unescaped query segment.
	@pre 			The delimiter must have been set. 
	@post			The query will have been extended to include the new segment.
 */
EXPORT_C void CDelimitedQuery8::PushAndEscapeFrontL(const TDesC8& aSegment)
	{
	// Create escaped version of the segment
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aSegment, EscapeUtils::EEscapeQuery);
	CleanupStack::PushL(escaped);

	// Insert the segment
	PushFrontL(*escaped);

	// Cleanup
	CleanupStack::PopAndDestroy(escaped);
	}

/**
	Constructor. First phase of two-phase construction method. Does non-allocating 
	construction.
	
	@since			6.0
 */
CDelimitedQuery8::CDelimitedQuery8()
: CDelimitedDataBase8()
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully 
	construct the object.
	
	@since			6.0
	@param			aQuery	A descriptor with the initial query.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed.
 */
void CDelimitedQuery8::ConstructL(const TDesC8& aQuery)
	{
	// Call base class ConstructL()
	CDelimitedDataBase8::ConstructL(aQuery);

	// Set the delimiter to '&'
	SetDelimiter(TChar('&'));
	}

//
//
// Implemetation of CDelimitedQuery16
//
//

/**
	Static factory constructor. Uses two phase construction and leaves nothing on the 
	CleanupStack.
						
	@since			6.0
	@param			aQuery	A descriptor with the initial query.
	@return			A pointer to created object.
	@post			Nothing left on the CleanupStack.
 */
EXPORT_C CDelimitedQuery16* CDelimitedQuery16::NewL(const TDesC16& aQuery)
	{
	CDelimitedQuery16* self = NewLC(aQuery);
	CleanupStack::Pop(self);
	return self;
	}


/**
	Static factory constructor. Uses two phase construction and leaves a pointer to created object on the CleanupStack.
						
	@since			6.0
	@param			aQuery	A descriptor with the initial query.
	@return			A pointer to created object.
	@post			Pointer to created object left of CleanupStack.
 */
EXPORT_C CDelimitedQuery16* CDelimitedQuery16::NewLC(const TDesC16& aQuery)
	{
	CDelimitedQuery16* self = new (ELeave) CDelimitedQuery16;
	CleanupStack::PushL(self);
	self->ConstructL(aQuery);
	return self;
	}

/**
	Destructor.
	
	@since			6.0
 */
EXPORT_C CDelimitedQuery16::~CDelimitedQuery16()
	{
	}
	
/**
	Escape encodes segment then inserts the escaped version in a position before the 
	current parsed segment. The new segment should only contain a single query segment, 
	as any query delimiters in the segment will be converted to an escape triple. The 
	parser is left in a state where its current segment is the same one as before the 
	insertion.
						
	@since			6.0
	@param			aSegment	A descriptor with the unescaped query segment.
	@pre 			The query must have been initially parsed.
	@post			The query will have been extended to include the new segment. The 
	current segment will remain as the one before the insertion.
 */
EXPORT_C void CDelimitedQuery16::InsertAndEscapeCurrentL(const TDesC16& aSegment)
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
	Escape encodes segment then inserts the escaped version at the back of the query. 
	The new segment should only contain a single query segment, as any query delimiters 
	in the segment will be converted to an escape triple. The parser is left in a state 
	where its current segment is the same one as before the insertion.
						
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the unescaped query segment.
	@pre 			The delimiter must have been set. 
	@post			The query will have been extended to include the new segment.
*/
EXPORT_C void CDelimitedQuery16::PushAndEscapeBackL(const TDesC16& aSegment)
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
	Escape encodes segment then inserts the escaped version at the front of the query. 
	The new segment should only contain a single query segment, as any query delimiters 
	in the segment will be converted to an escape triple. The parser is left in a state 
	where its current segment is the same one as before the insertion.
						
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the unescaped query segment.
	@pre 			The delimiter must have been set. 
	@post			The query will have been extended to include the new segment.
 */
EXPORT_C void CDelimitedQuery16::PushAndEscapeFrontL(const TDesC16& aSegment)
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
CDelimitedQuery16::CDelimitedQuery16()
: CDelimitedDataBase16()
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully 
	construct the object.
						
	@since			6.0
	@param			aQuery	A descriptor with the initial query.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed.
 */
void CDelimitedQuery16::ConstructL(const TDesC16& aQuery)
	{
	// Call base class ConstructL()
	CDelimitedDataBase16::ConstructL(aQuery);

	// Set the delimiter to '&'
	SetDelimiter(TChar('&'));
	}

