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

//
//
// Implementation of TDelimitedQueryParser8
//
//
/**
	Constructor.
	
	@since			6.0
 */
EXPORT_C TDelimitedQueryParser8::TDelimitedQueryParser8()
: TDelimitedParserBase8()
	{
	SetDelimiter(TChar('&'));
	}

/**
	This parses the descriptor into query segments from left to right.
	
	@since			6.0
	@param			aQuery	A descriptor containing the query.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from left to right (EDelimitedDataForward).
 */
EXPORT_C void TDelimitedQueryParser8::Parse(const TDesC8& aQuery)
	{
	// Call base class functions
	TDelimitedParserBase8::Parse(aQuery);
	}

/**
	This parses the descriptor into query segments from right to left.
	
	@since			6.0
	@param	aQuery	A descriptor containing the query.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from right to left (EDelimitedDataReverse).
 */
EXPORT_C void TDelimitedQueryParser8::ParseReverse(const TDesC8& aQuery)
	{
	// Call base class functions
	TDelimitedParserBase8::ParseReverse(aQuery);
	}

//
//
// Implementation of TDelimitedQueryParser16
//
//
/**
	Constructor.
	
	@since			6.0
 */
EXPORT_C TDelimitedQueryParser16::TDelimitedQueryParser16()
: TDelimitedParserBase16()
	{
	SetDelimiter(TChar('&'));
	}

/**
	This parses the descriptor into query segments from left to right.
	
	@since			6.0
	@param			aQuery	A descriptor containing the query.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from left to right (EDelimitedDataFroward).
 */
EXPORT_C void TDelimitedQueryParser16::Parse(const TDesC16& aQuery)
	{
	// Call base class functions
	TDelimitedParserBase16::Parse(aQuery);
	}

/**
	This parses the descriptor into query segments from right to left.
	
	@since			6.0
	@param			aQuery	A descriptor containing the query.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from right to left (EDelimitedDataReverse).
 */
EXPORT_C void TDelimitedQueryParser16::ParseReverse(const TDesC16& aQuery)
	{
	// Call base class functions
	TDelimitedParserBase16::ParseReverse(aQuery);
	}

