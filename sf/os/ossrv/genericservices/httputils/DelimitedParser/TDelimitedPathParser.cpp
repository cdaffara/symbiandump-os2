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

//
//
// Implementation of TDelimitedPathParser8
//
//
/**
	Constructor.
	
	@since			6.0
 */
EXPORT_C TDelimitedPathParser8::TDelimitedPathParser8()
: TDelimitedParserBase8()
	{
	SetDelimiter(TChar('/'));
	}
	
/**
	This parses the descriptor into path segments from left to right.
	
	@since			6.0
	@param			aPath	A descriptor containing the path.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from left to right (EDelimitedDataForward).
 */
EXPORT_C void TDelimitedPathParser8::Parse(const TDesC8& aPath)
	{
	// Call base class functions
	TDelimitedParserBase8::Parse(aPath);
	}

/**
	This parses the descriptor into path segments from right to left.
	
	@since			6.0
	@param			aPath	A descriptor containing the path.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from right to left (EDelimitedDataReverse).
 */
EXPORT_C void TDelimitedPathParser8::ParseReverse(const TDesC8& aPath)
	{
	// Call base class functions
	TDelimitedParserBase8::ParseReverse(aPath);
	}

//
//
// Implementation of TDelimitedPathParser16
//
//

/**
	Constructor.
	
	@since			6.0
*/
EXPORT_C TDelimitedPathParser16::TDelimitedPathParser16()
: TDelimitedParserBase16()
	{
	SetDelimiter(TChar('/'));
	}

/**
	This parses the descriptor into path segments from left to right.
	
	@since			6.0
	@param			aPath	A descriptor containing the path.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from left to right (EDelimitedDataFroward).
*/
EXPORT_C void TDelimitedPathParser16::Parse(const TDesC16& aPath)
	{
	// Call base class functions
	TDelimitedParserBase16::Parse(aPath);
	}

/**
	This parses the descriptor into path segments from right to left.
	
	@since			6.0
	@param			aPath	A descriptor containing the path.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from right to left (EDelimitedDataReverse).
*/
EXPORT_C void TDelimitedPathParser16::ParseReverse(const TDesC16& aPath)
	{
	// Call base class functions
	TDelimitedParserBase16::ParseReverse(aPath);
	}
