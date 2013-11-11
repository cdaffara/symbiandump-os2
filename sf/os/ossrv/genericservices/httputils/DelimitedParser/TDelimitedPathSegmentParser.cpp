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

//
//
// Implementation of TDelimitedPathSegmentParser8
//
//

/**
	Constructor.
	
	@since			6.0
*/
EXPORT_C TDelimitedPathSegmentParser8::TDelimitedPathSegmentParser8()
: TDelimitedParserBase8()
	{
	SetDelimiter(TChar(';'));
	}

/**
	This parses the descriptor into path segment parameters from left to right.
						
	@since			6.0
	@param			aPathSegment	A descriptor containing the path segment.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from left to right (EDelimitedDataForward).
*/
EXPORT_C void TDelimitedPathSegmentParser8::Parse(const TDesC8& aPathSegment)
	{
	// Call base class functions
	TDelimitedParserBase8::Parse(aPathSegment);
	}

/**
	This parses the descriptor into path segment parameters from right to left.
						
	@since			6.0
	@param			aPathSegment	A descriptor containing the path segment.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from right to left (EDelimitedDataReverse).
*/
EXPORT_C void TDelimitedPathSegmentParser8::ParseReverse(const TDesC8& aPathSegment)
	{
	// Call base class functions
	TDelimitedParserBase8::ParseReverse(aPathSegment);
	}

//
//
// Implementation of TDelimitedPathSegmentParser16
//
//
/**
	Constructor.
	
	@since			6.0
*/
EXPORT_C TDelimitedPathSegmentParser16::TDelimitedPathSegmentParser16()
: TDelimitedParserBase16()
	{
	SetDelimiter(TChar(';'));
	}

/**
	This parses the descriptor into path segment segments from left to right.
	
	@since			6.0
	@param			aPathSegment	A descriptor containing the path segment.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from left to right (EDelimitedDataFroward).
 */
EXPORT_C void TDelimitedPathSegmentParser16::Parse(const TDesC16& aPathSegment)
	{
	// Call base class functions
	TDelimitedParserBase16::Parse(aPathSegment);
	}
	
/**
	This parses the descriptor into path segment segments from right to left.
	
	@since			6.0
	@param			aPathSegment	A descriptor containing the path segment.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from right to left (EDelimitedDataReverse).
*/
EXPORT_C void TDelimitedPathSegmentParser16::ParseReverse(const TDesC16& aPathSegment)
	{
	// Call base class functions
	TDelimitedParserBase16::ParseReverse(aPathSegment);
	}

