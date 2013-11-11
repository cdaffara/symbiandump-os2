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

#include <delimitedpathsegment8.h>
#include <delimitedpathsegment16.h>
#include <escapeutils.h>

//
//
// Implemetation of CDelimitedPathSegment8
//
//

/**
	Static factory constructor. Uses two phase construction and leaves nothing on 
	the CleanupStack.
	
	@since			6.0
	@param			aPathSegment	A descriptor with the initial path segment.
	@return			A pointer to created object.
	@post			Nothing left on the CleanupStack.
*/
EXPORT_C CDelimitedPathSegment8* CDelimitedPathSegment8::NewL(const TDesC8& aPathSegment)
	{
	CDelimitedPathSegment8* self = NewLC(aPathSegment);
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to 
	created object on the CleanupStack.
	
	@since			6.0
	@param			aPathSegment	A descriptor with the initial path segment.
	@return			A pointer to created object.
	@post			Pointer to created object left of CleanupStack.
*/
EXPORT_C CDelimitedPathSegment8* CDelimitedPathSegment8::NewLC(const TDesC8& aPathSegment)
	{
	CDelimitedPathSegment8* self = new (ELeave) CDelimitedPathSegment8;
	CleanupStack::PushL(self);
	self->ConstructL(aPathSegment);
	return self;
	}

/**
	Destructor.
	
	@since			6.0
*/
EXPORT_C CDelimitedPathSegment8::~CDelimitedPathSegment8()
	{
	}

/**

	Escape encodes the parameter then inserts the escaped version in a position before the 
	current parsed parameter. The new parameter should only contain a single path segment 
	parameter, as any parameter delimiters in the parameter will be converted to an escape 
	triple. The parser is left in a state where its current parameter is the same one as before 
	the insertion.
						
	@since			6.0
	@param			aParam	A descriptor with the unescaped path segment parameter
	@pre 			The path segment must have been initially parsed.
	@post			The path segment will have been extended to include the new 
	parameter. The current segment will remain as the one before the insertion.
*/
EXPORT_C void CDelimitedPathSegment8::InsertAndEscapeCurrentL(const TDesC8& aParam)
	{
	// Create escaped version of the parameter
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aParam, EscapeUtils::EEscapePath);
	CleanupStack::PushL(escaped);

	// Insert the segment
	InsertCurrentL(*escaped);

	// Cleanup
	CleanupStack::PopAndDestroy(escaped);
	}

/**
	Escape encodes the parameter then inserts the escaped version at the front of the 
	path segment. The new parameter should only contain a single path segment parameter, 
	as any parameter delimiters in the parameter will be converted to an escape triple. 
	The parser is left in a state where its current parameter is the same one as before 
	the insertion.
						
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aParam	A descriptor with the unescaped path segment parameter
	@pre 			The path segment must have been initially parsed.
	@post			The path segment will have been extended to include the new 
	parameter. The current segment will remain as the one before the insertion.
*/
EXPORT_C void CDelimitedPathSegment8::PushAndEscapeBackL(const TDesC8& aParam)
	{
	// Create escaped version of the parameter
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aParam, EscapeUtils::EEscapePath);
	CleanupStack::PushL(escaped);

	// Insert the segment
	PushBackL(*escaped);

	// Cleanup
	CleanupStack::PopAndDestroy(escaped);
	}

/**
	Escape encodes the parameter then inserts the escaped version at the back of the 
	path segment. The new parameter should only contain a single path segment parameter, 
	as any parameter delimiters in the parameter will be converted to an escape triple. 
	The parser is left in a state where its current parameter is the same one as before 
	the insertion.
						
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aParam	A descriptor with the unescaped path segment parameter.
	@pre 			The path segment must have been initially parsed.
	@post			The path segment will have been extended to include the new 
	parameter. The current segment will remain as the one before the insertion.
*/
EXPORT_C void CDelimitedPathSegment8::PushAndEscapeFrontL(const TDesC8& aParam)
	{
	// Create escaped version of the parameter
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aParam, EscapeUtils::EEscapePath);
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
CDelimitedPathSegment8::CDelimitedPathSegment8()
: CDelimitedDataBase8()
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to 
	fully construct the object.
						
	@since			6.0
	@param			aPathSegment	A descriptor with the initial path segment.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed.
*/
void CDelimitedPathSegment8::ConstructL(const TDesC8& aPathSegment)
	{
	// Call base class ConstructL()
	CDelimitedDataBase8::ConstructL(aPathSegment);

	// Set the delimiter to ';'
	SetDelimiter(TChar(';'));
	}

//
//
// Implemetation of CDelimitedPathSegment16
//
//

/**
	Static factory constructor. Uses two phase construction and leaves nothing on the 
	CleanupStack.
						
	@since			6.0
	@param			aPathSegment	A descriptor with the initial path segment.
	@return			A pointer to created object.
	@post			Nothing left on the CleanupStack.
 */
EXPORT_C CDelimitedPathSegment16* CDelimitedPathSegment16::NewL(const TDesC16& aPathSegment)
	{
	CDelimitedPathSegment16* self = NewLC(aPathSegment);
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to created 
	object on the CleanupStack.
						
	@since			6.0
	@param			aPathSegment	A descriptor with the initial path segment.
	@return			A pointer to created object.
	@post			Pointer to created object left of CleanupStack.
*/
EXPORT_C CDelimitedPathSegment16* CDelimitedPathSegment16::NewLC(const TDesC16& aPathSegment)
	{
	CDelimitedPathSegment16* self = new (ELeave) CDelimitedPathSegment16;
	CleanupStack::PushL(self);
	self->ConstructL(aPathSegment);
	return self;
	}

/**
	Destructor.
	
	@since			6.0
*/
EXPORT_C CDelimitedPathSegment16::~CDelimitedPathSegment16()
	{
	}
	
/**
	Escape encodes the parameter then inserts the escaped version in a position before the 
	current parsed parameter. The new parameter should only contain a single path segment 
	parameter, as any parameter delimiters in the parameter will be converted to an escape 
	triple. The parser is left in a state where its current parameter is the same one as before 
	the insertion.
						
	@since			6.0
	@param			aParam	A descriptor with the unescaped path segment parameter
	@pre 			The path segment must have been initially parsed.
	@post			The path segment will have been extended to include the new 
	parameter. The current segment will remain as the one before the insertion.
*/
EXPORT_C void CDelimitedPathSegment16::InsertAndEscapeCurrentL(const TDesC16& aParam)
	{
	// Need to convert to utf8 first
	HBufC8* utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aParam);
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
	Escape encodes the parameter then inserts the escaped version at the back of the path 
	segment. The new parameter should only contain a single path segment parameter, as any 
	parameter delimiters in the parameter will be converted to an escape triple. The parser 
	is left in a state where its current parameter is the same one as before the insertion.
						
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aParam	A descriptor with the unescaped path segment parameter.
	@pre 			The path segment must have been initially parsed.
	@post			The path segment will have been extended to include the new 
	parameter. The current segment will remain as the one before the insertion.
 */
EXPORT_C void CDelimitedPathSegment16::PushAndEscapeBackL(const TDesC16& aParam)
	{
	// Need to convert to utf8 first
	HBufC8* utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aParam);
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
	Escape encodes the parameter then inserts the escaped version at the front of the 
	path segment. The new parameter should only contain a single path segment parameter, 
	as any parameter delimiters in the parameter will be converted to an escape triple. 
	The parser is left in a state where its current parameter is the same one as before 
	the insertion.
						
	@warning		A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aParam	A descriptor with the unescaped path segment parameter
	@pre 			The path segment must have been initially parsed.
	@post			The path segment will have been extended to include the new 
	parameter. The current segment will remain as the one before the insertion.
 */
EXPORT_C void CDelimitedPathSegment16::PushAndEscapeFrontL(const TDesC16& aParam)
	{
	// Need to convert to utf8 first
	HBufC8* utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aParam);
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
	Constructor. First phase of two-phase construction method. Does non-allocating
	 construction.
						
	@since			6.0
 */
CDelimitedPathSegment16::CDelimitedPathSegment16()
: CDelimitedDataBase16()
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to 
	fully construct the object.
						
	@since			6.0
	@param			aPathSegment	A descriptor with the initial path segment.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed.
 */
void CDelimitedPathSegment16::ConstructL(const TDesC16& aPathSegment)
	{
	// Call base class ConstructL()
	CDelimitedDataBase16::ConstructL(aPathSegment);

	// Set the delimiter to ';'
	SetDelimiter(TChar(';'));
	}

