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

#include <delimitedparser8.h>
#include <delimitedparser16.h>
#include "DelimitedParserInternal.h"
#include <uriutilscommon.h>

// Panic category
//
_LIT(KDelimitedParserPanicCategory,"DELIM-PARSER"); 

//
//
// Implementation of TDelimitedParserBase8
//
//

/**
	Constructor.
	
	@since			6.0
*/
EXPORT_C TDelimitedParserBase8::TDelimitedParserBase8()
: iDataDes(0,0), iCurrentSegment(0,0), iNextSegmentPos(-1), iMode(EDelimitedDataNotParsed), iDelimiter(0)
	{
	}

/**
	Resets the internal pointer position to the start or end or the descriptor
	depending on whether the decriptor is parsing mode.
	
	@warning		There will be a KUriUtilsErrBadDelimitedParserMode panic if the data mode has 
	not been correctly set.
 */
EXPORT_C void TDelimitedParserBase8::Reset() const
	{
	iNextSegmentPos = InitialDelimiterPosition(iDataDes, iMode);
	}

/**
	Retrieves the current segment and then parses the data to the next one.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set.
	@since			6.0
	@param			aSegment	This is an output argument that is set to the current segment.
	@return			A error value of KErrNotFound if there is no current segment. The 
	value KErrNone if there is a current segment.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse(). 
	@post			The current segment is updated to the next one.
*/
EXPORT_C TInt TDelimitedParserBase8::GetNext(TPtrC8& aSegment) const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Check that there is a segment
	if( iNextSegmentPos == KErrNotFound )
		{
		// There is no segment
		return KErrNotFound;
		}
	// There is one - set aSegment
	aSegment.Set(iCurrentSegment);
	// Parse the next segment
	iNextSegmentPos = FindNextSegment(iNextSegmentPos);
	return KErrNone;
	}

/**
	Parses to the next segment.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set.
	@since			6.0
	@return			A error value of KErrNotFound if there is no current segment. The 
	value KErrNone if there is a current segment.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse(). 
	@post			The current segment is updated to the next one.
 */	
EXPORT_C TInt TDelimitedParserBase8::Inc() const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Check that there is a segment
	if( iNextSegmentPos == KErrNotFound )
		{
		// There is no segment
		return KErrNotFound;
		}
	// Parse the next segment
	iNextSegmentPos = FindNextSegment(iNextSegmentPos);
	return KErrNone;
	}

/**
	Parses back to the previous segment.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set.
	@since			6.0
	@return			A error value of KErrNotFound if the current segment is the initial 
	segment. The value KErrNone if the data has been parsed to the previous segment.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse(). 
	@post			If the parse was successful then the  current segment is updated 
	to the previous one. Otherwise there is no change.
*/
EXPORT_C TInt TDelimitedParserBase8::Dec() const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Find position of previous delimiter 
	TInt prev = PrevDelimiterPosition(iDataDes, iNextSegmentPos, iDelimiter, iMode);

	// Get the previous segment
	if( FindPrevSegment(prev) == KErrNotFound )
		{
		// There is no previous segment - set to start of data
		return KErrNotFound;
		}
	// Update next segment position
	iNextSegmentPos = prev;
	return KErrNone;
	}

/**
	Retrieves the current segment.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set.
	@since			6.0
	@param			aSegment	This is an output argument that is set to the current segment.
	@return			A error value of KErrNotFound if there is no current segment. The 
	value KErrNone if there is a current segment.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse(). 
*/
EXPORT_C TInt TDelimitedParserBase8::Peek(TPtrC8& aSegment) const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Check that there is a segment
	if( iNextSegmentPos == KErrNotFound )
		{
		// There is no segment
		return KErrNotFound;
		}
	// There is one - set aSegment
	aSegment.Set(iCurrentSegment);
	return KErrNone;
	}

/**
	Indicates whether the end of the data has been reached and there are no more segments 
	to parse.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set.
	@since			6.0
	@return			A boolean value of ETrue if the end of the data has been reached,
	or EFalse if there are more segements to parse.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse(). 
*/
EXPORT_C TBool TDelimitedParserBase8::Eos() const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));

	TBool eos = iNextSegmentPos == KErrNotFound ? ETrue : EFalse;
	return eos;
	}

/**
	Checks for a delimiter at the front (left) of the data.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@return			A boolean of value ETrue if there is a front delimiter, or EFalse 
	if there is no front delimiter.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse(). 
*/
EXPORT_C TBool TDelimitedParserBase8::FrontDelimiter() const
	{
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	return (iDataDes.Locate(iDelimiter) == 0);
	}

/**
	Checks for a delimiter at the back (right) of the data.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set.
	@since			6.0
	@return			A boolean of value ETrue if there is a back delimiter, or EFalse 
	if there is no back delimiter.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse(). 
*/
EXPORT_C TBool TDelimitedParserBase8::BackDelimiter() const
	{
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	TInt delimiterPos = iDataDes.LocateReverse(iDelimiter);
	if( delimiterPos == KErrNotFound )
		return EFalse;
	return (delimiterPos == iDataDes.Length() - 1);
	}

/**
	Retrieves the descriptor reference with the data
	
	@since			6.0
	@return			A const descriptor reference with the data.
*/
EXPORT_C const TDesC8& TDelimitedParserBase8::Des() const
	{
	return iDataDes;
	}

/**
	Gives the remainder of the data from (and including) the current segment. Any other segments 
	that have parsed through are not included.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set.
	@since			6.0
	@param			aRemainder	This is an output argument that is set to the remaining data.
	@return			An error value of KErrNotFound if there is no remaining data, or 
	value of KErrNone if there is remaining data.
	@pre 			The data must have been initially parsed by Parse() or ParseReverse().
 */
EXPORT_C TInt TDelimitedParserBase8::Remainder(TPtrC8& aRemainder) const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Check to see if there is a segment left
	if( iNextSegmentPos == KErrNotFound )
		{
		// There is no segment
		return KErrNotFound;
		}
	// Find the previous delimiter -> the start of the current segment
	TInt prev = PrevDelimiterPosition(iDataDes, iNextSegmentPos, iDelimiter, iMode);

	// Need to see which direction the parsing is going to set the remainder
	switch(iMode)
		{
	case EDelimitedDataForward:
		{
		aRemainder.Set(iDataDes.Right(iDataDes.Length() - prev));
		} break;
	case EDelimitedDataReverse:
		{
		aRemainder.Set(iDataDes.Left(prev));
		} break;
	default:
		// Bad mode!
		User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrBadDelimitedParserMode);
		break;
		}
	return KErrNone;
	}

/**
	This parses the data into segments from left to right.

	@warning		There will be a KDelimitedParserErrNoDelimiter panic if the delimiter
	has not been set.
	@since			6.0
	@param			aData	A descriptor containing the data.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from left to right (EDelimitedDataFroward).
 */
EXPORT_C void TDelimitedParserBase8::Parse(const TDesC8& aData)
	{
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Initialise data for EForward direction
	iMode = EDelimitedDataForward;
	DoParse(aData);
	}

/**
	This parses the data into segments from lright to left.

	@warning		There will be a KDelimitedParserErrNoDelimiter panic if the delimiter
	has not been set.
	@since			6.0
	@param			aData	A descriptor containing the data.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from right to left (EDelimitedDataReverse).
 */
EXPORT_C void TDelimitedParserBase8::ParseReverse(const TDesC8& aData)
	{
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Initialise data for EReverse direction
	iMode = EDelimitedDataReverse;
	DoParse(aData);
	}

/**
	Sets the delimiting character.
	
	@since			6.0
	@param			aDelimiter	The delimiting character.
	@post			The delimiting character is set.
*/
EXPORT_C void TDelimitedParserBase8::SetDelimiter(TChar aDelimiter)
	{
	iDelimiter = aDelimiter;
	}

/**
	Initialises the parsing of the data.
	
	@since			6.0
	@param			aData	A descriptor reference with the data.
	@pre 			The delimiting character has been set.
	@post			The data descriptor is set to the input argument. The current 
	segment refers to the initial segment of the data.
*/
void TDelimitedParserBase8::DoParse(const TDesC8& aData)
	{
	// Reset the segment information, then set the new Data - set pointer to NULL and length to zero
	iCurrentSegment.Set(NULL,0);
	iDataDes.Set(aData);

	// Check that there is a string!
	if( iDataDes.Length() == 0 )
		{
		// No string - ensure functionality blocked for this descriptor
		iNextSegmentPos = KErrNotFound;
		return;
		}
	// Find the segment - search from initial start position
	iNextSegmentPos = FindNextSegment(InitialDelimiterPosition(iDataDes, iMode));
	}

/**
	Finds the next segment from the given start position.
	
	@since			6.0
	@param			aStartPos	The position from where to start the search for the
	next segment.
	@return			The position of delimiter after the specified start position, or 
	an error value of KErrNotFound if no more delimiters are found.
*/
TInt TDelimitedParserBase8::FindNextSegment(TInt aStartPos) const
	{
	// Find position of next delimiter
	TInt next = NextDelimiterPosition(iDataDes, aStartPos, iDelimiter, iMode);

	if( next != KErrNotFound )
		{
		TInt startPos = next < aStartPos ? next : aStartPos;
		TInt endPos = next < aStartPos ? aStartPos : next;
		if( iDataDes[startPos] == iDelimiter )
			{
			// Move past delimiter
			++startPos;
			}
		TInt length = endPos - startPos;
		iCurrentSegment.Set(iDataDes.Mid(startPos, length));
		}
	return next;
	}

/**
	Finds the previous segment from the given start position.
	
	@since			6.0
	@param			aStartPos	The position from where to start the search for the
					previous segment.
	@return			The position of delimiter before the specified start position, or 
	an error value of KErrNotFound if no more delimiters are found.
*/
TInt TDelimitedParserBase8::FindPrevSegment(TInt aStartPos) const
	{
	// Find position of previous delimiter 
	TInt prev = PrevDelimiterPosition(iDataDes, aStartPos, iDelimiter, iMode);
	
	if( prev != KErrNotFound )
		{
		TInt startPos = prev < aStartPos ? prev : aStartPos;
		TInt endPos = prev < aStartPos ? aStartPos : prev;
		if( iDataDes[startPos] == iDelimiter )
			{
			// Move past delimiter
			++startPos;
			}
		TInt length = endPos - startPos;
		iCurrentSegment.Set(iDataDes.Mid(startPos, length));
		}
	return prev;
	}

//
//
// Implementation of CDelimitedDataBase8
//
//

/**
	Destructor.
	
	@since			6.0
*/
EXPORT_C CDelimitedDataBase8::~CDelimitedDataBase8()
	{
	delete iDataBuf;
	}

/**
	Inserts the new segment in a position before the current parsed	segment. The new 
	segment can be made up of several segments and have delimiters at either extreme. 
	The insert functionality will ensure that there is always a delimiter at the front 
	of the new segment. The parser is left in a state where its current segment is the 
	same one as before the insertion.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not been 
	parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@param			aSegment	A descriptor with the new segment to be inserted.
	@pre 			The data must have been initially parsed by Parse() or ParseReverse().
	@post			The data will have been extended to include the new segment. The 
	current segment will remain as the one before the insertion.
*/
EXPORT_C void CDelimitedDataBase8::InsertCurrentL(const TDesC8& aSegment)
	{
	__ASSERT_ALWAYS(iParser.iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	DoInsertL(aSegment);
	}

/**
	Removes the current segment. After removing the segment, the parser's new current segment 
	will be the next segment. If the last segment is the one that is removed then the parser 
	will be set to the end of the data.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set.
	@since			6.0
	@pre 			The data must have been initially parsed by Parse() or ParseReverse().
	@post			The data will have been reduced to exclude the removed segment.
	The current segment will be set to what was the next segment. If the removed segment was 
	the last segment, the parser is at the end of the data.
*/
EXPORT_C void CDelimitedDataBase8::RemoveCurrentL()
	{
	__ASSERT_ALWAYS(iParser.iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	DoRemoveL();
	}

/**
	Adds a new segment to the end of the data. The new segment can be made up of several segments 
	and have  delimiters at either extreme. The insert functionality will ensure that there is 
	always a delimiter at the front of the new segment. The data must re-parsed to ensure that the 
	parser is valid.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the new segment to be inserted.
	@pre 			The delimiter must have been set. 
	@post			The data will have been extended to include the new segment.
*/
EXPORT_C void CDelimitedDataBase8::PushBackL(const TDesC8& aSegment)
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Parse the string in reverse direction - sets last segment as current
	iParser.ParseReverse(*iDataBuf);

	// Insert the segment
	DoInsertL(aSegment);

	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Removes the last segment from the data. The data must be re-parsed to ensure that the parser is valid.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data will have been reduced to exclude the last segment. 
*/
EXPORT_C void CDelimitedDataBase8::PopBackL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Parse the string in reverse direction - sets last segment as current
	iParser.ParseReverse(*iDataBuf);

	// Remove the current segment
	DoRemoveL();

	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Adds a new segment to the front of the data. The new segment can be made up of several segments 
	and have delimiters at either extreme. The insert functionality will ensure that there is always 
	a delimiter at the front of the new segment. The data must re-parsed to ensure that the parser 
	is valid.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.					
	@since			6.0
	@param			aSegment	A descriptor with the new segment to be inserted.
	@pre 			The delimiter must have been set. 
	@post			The data will have been extended to include the new segment.
*/
EXPORT_C void CDelimitedDataBase8::PushFrontL(const TDesC8& aSegment)
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Parse the string in forward direction - sets first segment as current
	iParser.Parse(*iDataBuf);

	// Insert the segment
	DoInsertL(aSegment);

	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Removes the first segment from the data. The data must be re-parsed to ensure that the parser is valid.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data will have been reduced to exclude the last segment. 
*/
EXPORT_C void CDelimitedDataBase8::PopFrontL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Parse the string in forward direction - sets first segment as current
	iParser.Parse(*iDataBuf);

	// Remove the current segment
	DoRemoveL();

	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Removes the front delimiter (if exists) from the data.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data might have been reduced to exclude the front delimiter. 
*/
EXPORT_C void CDelimitedDataBase8::TrimFrontDelimiterL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Search for delimiter
	if( iParser.FrontDelimiter() )
		{
		// Remove front delimiter and update member data
		SetDataL(iParser.iDataDes.Right(iParser.iDataDes.Length() - 1));
		}
	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Adds a delimiter to the front of the data (if it doesn't exist).
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data might have been extended to include a front delimiter. 
*/
EXPORT_C void CDelimitedDataBase8::AddFrontDelimiterL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	if( !iParser.FrontDelimiter() )
		{
		// Create a new buffer of correct size
		HBufC8* buf = HBufC8::NewL(iParser.iDataDes.Length() + 1);
		TPtr8 str = buf->Des();

		// Append a delimiter, then append the current string
		str.Append(iParser.iDelimiter);
		str.Append(iParser.iDataDes);

		// Set buffer to this new string
		SetData(buf);
		}
	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Removes the back delimiter (if exists) from the data.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data might have been reduced to exclude the front delimiter. 
*/
EXPORT_C void CDelimitedDataBase8::TrimBackDelimiterL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Search for delimiter
	if( iParser.BackDelimiter() )
		{
		// Remove back delimiter and update member data
		SetDataL(iParser.iDataDes.Left(iParser.iDataDes.Length() - 1));
		}
	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Adds a delimiter to the back of the data (if it doesn't exist).
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has 
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data might have been extended to include a front delimiter. 
*/
EXPORT_C void CDelimitedDataBase8::AddBackDelimiterL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	if( !iParser.BackDelimiter() )
		{
		// Create a new buffer of correct size
		HBufC8* buf = HBufC8::NewL(iParser.iDataDes.Length() + 1);
		TPtr8 str = buf->Des();

		// Append the current string, then append a delimiter
		str.Append(iParser.iDataDes);
		str.Append(iParser.iDelimiter);

		// Set buffer to this new string
		SetData(buf);
		}
	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	This parses the data into segments from left to right.
	
	@warning		There will be a KDelimitedParserErrNoDelimiter panic if the delimiter
	has not been set.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of 
	parsing is set from left to right (EDelimitedDataFroward).
*/
EXPORT_C void CDelimitedDataBase8::Parse()
	{
	// This call will panic with KUriUtilsErrNoDelimiter if the delimiter is not set
	iParser.Parse(*iDataBuf);
	}

/**
	This parses the string into segments from right to left.
	
	@since			6.0
	@pre 			The delimiter must have been set. Will get a KDelimitedParserErrNoDelimiter panic if 
	the delimiter has not been initialized.
	@post			The current segment is the leftmost segment and the direction of parsing is right to left.
*/
EXPORT_C void CDelimitedDataBase8::ParseReverse()
	{
	// This call will panic with KUriUtilsErrNoDelimiter if the delimiter is not set
	iParser.ParseReverse(*iDataBuf);
	}

/**
	Retrieves a const reference to the delimited data parser. 
	
	@since			6.0
	@return			A const reference to the delimited data parser. 
*/
EXPORT_C const TDelimitedParserBase8& CDelimitedDataBase8::Parser() const
	{
	return iParser;
	}

/**
	Sets the delimiting character.
	
	@since			6.0
	@param			aDelimiter	The delimiting character.
	@post			The delimiting character is updated.
*/
EXPORT_C void CDelimitedDataBase8::SetDelimiter(TChar aDelimiter)
	{
	iParser.SetDelimiter(aDelimiter);
	}

/**
	Constructor. First phase of two-phase construction method. Does non-allocating construction.
						
	@since			6.0
*/
EXPORT_C CDelimitedDataBase8::CDelimitedDataBase8()
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully construct 
	the object.
						
	@since			6.0
	@param			aData	A descriptor with the initial string.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed.
*/
EXPORT_C void CDelimitedDataBase8::ConstructL(const TDesC8& aData)
	{
	// Create copy of string and set descriptor in the parser
	SetDataL(aData);
	}

/**
	Inserts the new segment in a position before the current segment. The new segment can be made up 
	of several segments and have delimiters at either extreme. The insert functionality will ensure 
	that there is always a delimiter at the front of the new segment. The parser will be left in a 
	state where its current segment is the same one as before the insertion.
						
	@since			6.0
	@param			aSegment	The descriptor with the segment to be inserted.
	@pre 			The string must have been parsed.
	@post			The string will have been extended to include the new segment. The current segment will
	remain as the one before the insertion.
*/
void CDelimitedDataBase8::DoInsertL(const TDesC8& aSegment)
	{
	// Get previous delimiter to split the current string into prefix and suffix to the new segment
	TInt prevPos = PrevDelimiterPosition(iParser.iDataDes, iParser.iNextSegmentPos, iParser.iDelimiter, iParser.iMode);
	TPtrC8 prefix = iParser.iDataDes.Left(prevPos);

	TInt suffixLength = iParser.iDataDes.Length() - prevPos;
	TPtrC8 suffix = iParser.iDataDes.Right(suffixLength);
	if( suffixLength && suffix[0] == iParser.iDelimiter )
		{
		// Remove front delimiter on suffix
		suffix.Set(suffix.Right(--suffixLength));
		}	

	// Check for delimiters
	TPtrC8 segment = aSegment;
	TInt segmentLength = segment.Length();
	TBool segmentBackDelimiter = (segmentLength && segment[segmentLength - 1] == iParser.iDelimiter);
	if( segmentBackDelimiter )
		{
		// Remove back delimiter from the segment
		segment.Set(segment.Left(--segmentLength));
		}
	if( segmentLength && segment[0] == iParser.iDelimiter )
		{
		// Remove front delimiter from the segment
		segment.Set(segment.Right(--segmentLength));
		}

	// Check if a back delimiter is needed - NOTE always add a front delimiter
	TInt extra = 1;
	TBool needBackDelimiter = EFalse;
	if( suffix.Length() || segmentBackDelimiter )
		{
		++extra;
		needBackDelimiter = ETrue;
		}
	// Create space for new string
	HBufC8* buf = HBufC8::NewL(prevPos + segmentLength + suffixLength + extra);
	TPtr8 str = buf->Des();

	// Form the new string
	str.Append(prefix);
	str.Append(iParser.iDelimiter);
	str.Append(segment);
	if( needBackDelimiter )
		str.Append(iParser.iDelimiter);
	str.Append(suffix);

	// Update string data
	SetData(buf);

	// Check to see if the internal parser object (iParser) has been parsed
	// (can tell if it has if the data pointer in iCurrentSegment is not NULL)
	// If so update iCurrentSegment to ensure that iParser remains valid
	if( iParser.iCurrentSegment.Ptr() )
		{
		// Ensure parser is in correct position and current segment is correct
		iParser.iNextSegmentPos = prevPos;
		if( iParser.iMode == EDelimitedDataForward )
			{
			// Move iterator to delimiter before iCurrentSegment - length of segment + a delimiter
			iParser.iNextSegmentPos += segmentLength + 1;
			}
		// Get the next segment
		iParser.iNextSegmentPos = iParser.FindNextSegment(iParser.iNextSegmentPos);
		}
	}

/**
	Removes the current segment. After removing the segment, the parser's new current segment will be 
	the next segment. If the last segment is the one that is removed then the parser will be set to the 
	end of the data.
						
	@since			6.0
	@pre 			The data must have been parsed.
	@post			The data will have been reduced to exclude the removed data. The 
	current segment is set to what was the next segment. If the removed segment was 
	the last segment, the parser is at the end of the data.
*/
void CDelimitedDataBase8::DoRemoveL()
	{
	// Check if there is anything to remove
	if( iParser.iDataDes.Length() == 0 )
		{
		return;
		}
	// Find the previous delimiter
	TInt prev = PrevDelimiterPosition(iParser.iDataDes, iParser.iNextSegmentPos, iParser.iDelimiter, iParser.iMode);

	// Set up the start and end position of current segment
	TInt endPos = iParser.iNextSegmentPos;
	TInt startPos = iParser.iNextSegmentPos;
	if( prev < iParser.iNextSegmentPos )
		startPos = prev;
	else
		endPos = prev;

	// Ok, get the prefix and suffix parts
	TPtrC8 prefix = iParser.iDataDes.Left(startPos);
	TInt suffixLength = iParser.iDataDes.Length() - endPos;
	TPtrC8 suffix = iParser.iDataDes.Right(suffixLength);

	// Create the space
	HBufC8* buf = HBufC8::NewL(startPos + suffixLength);
	TPtr8 str = buf->Des();

	// Form the new string
	str.Append(prefix);
	str.Append(suffix);

	// Update string data
	SetData(buf);

	// Ensure parser is in correct position
	iParser.iNextSegmentPos = iParser.FindNextSegment(startPos);
	}

/**
	Updates internal data buffer with the new data. Creates a copy of the new data.
						
	@since			6.0
	@param			aData	A descriptor with the new string.
	@post			The internal data buffer now contains a copy of the new data and the 
	parser is set to the new data.
*/
void CDelimitedDataBase8::SetDataL(const TDesC8& aData)
    {
	// Cleanup old data and set new
	HBufC8* buf =  aData.AllocL();
	SetData(buf);
	}

/**
	Sets internal data buffer and parser. Cleans up the old data and uses the data buffer. 
	The parser is set to the new data.						
	
	@since			6.0
	@param			aDataBuf	A pointer to a decriptor buffer with the new data.
	@post			The internal data buffer now points to the new buffer and the parser 
	is set to the data in the new buffer..
*/
void CDelimitedDataBase8::SetData(HBufC8* aDataBuf)
	{
	delete iDataBuf;
	iDataBuf = aDataBuf;
	iParser.iDataDes.Set(*iDataBuf);
	}

//
//
// Implementation of TDelimitedParserBase16
//
//
/**
	Constructor.
	
	@since			6.0
 */
EXPORT_C TDelimitedParserBase16::TDelimitedParserBase16()
: iDataDes(0,0), iCurrentSegment(0,0), iNextSegmentPos(-1), iMode(EDelimitedDataNotParsed), iDelimiter(0)
	{
	}

/**
	Resets the internal pointer position to the start or end or the descriptor depending 
	on whether the decriptor is parsing mode.
	
	@warning		There will be a KUriUtilsErrBadDelimitedParserMode panic if the data mode has 
	not been correctly set.
 */
EXPORT_C void TDelimitedParserBase16::Reset() const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	iNextSegmentPos = InitialDelimiterPosition(iDataDes, iMode);
	}
/**
	Retrieves the current segment and then parses the data to the next one.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not 
	been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@param			aSegment	This is an output argument that is set to the current segment.
	@return			A error value of KErrNotFound if there is no current segment. The
	value KErrNone if there is a current segment.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse().
	@post			The current segment is updated to the next one.
 */
EXPORT_C TInt TDelimitedParserBase16::GetNext(TPtrC16& aSegment) const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Check that there is a segment
	if( iNextSegmentPos == KErrNotFound )
		{
		// There is no segment
		return KErrNotFound;
		}
	// There is one - set aSegment
	aSegment.Set(iCurrentSegment);
	// Parse the next segment
	iNextSegmentPos = FindNextSegment(iNextSegmentPos);
	return KErrNone;
	}
	
/**
	Parses to the next segment.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not 
	been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@return			A error value of KErrNotFound if there is no current segment. The
	value KErrNone if there is a current segment.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse().
	@post			The current segment is updated to the next one.
 */
EXPORT_C TInt TDelimitedParserBase16::Inc() const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Check that there is a segment
	if( iNextSegmentPos == KErrNotFound )
		{
		// There is no segment
		return KErrNotFound;
		}
	// Parse the next segment
	iNextSegmentPos = FindNextSegment(iNextSegmentPos);
	return KErrNone;
	}
	
/**
	Parses back to the previous segment.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not 
	been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@return			A error value of KErrNotFound if the current segment is the initial
	segment. The value KErrNone if the data has been parsed to the previous segment.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse().
	@post			If the parse was successful then the  current segment is updated
	to the previous one. Otherwise there is no change.
 */
EXPORT_C TInt TDelimitedParserBase16::Dec() const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Find position of previous delimiter 
	TInt prev = PrevDelimiterPosition(iDataDes, iNextSegmentPos, iDelimiter, iMode);

	// Get the previous segment
	if( FindPrevSegment(prev) == KErrNotFound )
		{
		// There is no previous segment - set to start of data
		return KErrNotFound;
		}
	// Update next segment position
	iNextSegmentPos = prev;
	return KErrNone;
	}

/**
	Retrieves the current segment.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not 
	been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@param			aSegment	This is an output argument that is set to the current segment.
	@return			A error value of KErrNotFound if there is no current segment. The
					value KErrNone if there is a current segment.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse().
 */
EXPORT_C TInt TDelimitedParserBase16::Peek(TPtrC16& aSegment) const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Check that there is a segment
	if( iNextSegmentPos == KErrNotFound )
		{
		// There is no segment
		return KErrNotFound;
		}
	// There is one - set aSegment
	aSegment.Set(iCurrentSegment);
	return KErrNone;
	}
	
/**
	Indicates whether the end of the data has been reached and there are no more segments to parse.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not 
	been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@return			A boolean value of ETrue if the end of the data has been reached,
	or EFalse if there are more segements to parse.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse().
 */
EXPORT_C TBool TDelimitedParserBase16::Eos() const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));

	TBool eos = iNextSegmentPos == KErrNotFound ? ETrue : EFalse;
	return eos;
	}
	
/**
	Checks for a delimiter at the front (left) of the data.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not 
	been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@return			A boolean of value ETrue if there is a front delimiter, or EFalse
	if there is no front delimiter.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse().
 */
EXPORT_C TBool TDelimitedParserBase16::FrontDelimiter() const
	{
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	return (iDataDes.Locate(iDelimiter) == 0);
	}

/**
	Checks for a delimiter at the back (right) of the data.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not 
	been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@return			A boolean of value ETrue if there is a back delimiter, or EFalse
	if there is no back delimiter.
	@pre 			The string must have been initially parsed by Parse() or ParseReverse().
 */
EXPORT_C TBool TDelimitedParserBase16::BackDelimiter() const
	{
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	TInt delimiterPos = iDataDes.LocateReverse(iDelimiter);
	if( delimiterPos == KErrNotFound )
		return EFalse;
	return (delimiterPos == iDataDes.Length() - 1);
	}
	
/**
	Retrieves the descriptor reference with the data
	
	@since			6.0
	@return			A const descriptor reference with the data.
 */
EXPORT_C const TDesC16& TDelimitedParserBase16::Des() const
	{
	return iDataDes;
	}
	
/**
	Gives the remainder of the data from (and including) the current segment. Any other segments that 
	have parsed through are not included.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not been parsed, 
	and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@param			aRemainder	This is an output argument that is set to the remaining data.
	@return			An error value of KErrNotFound if there is no remaining data, or value of KErrNone 
	if there is remaining data.
	@pre 			The data must have been initially parsed by Parse() or ParseReverse().
 */
EXPORT_C TInt TDelimitedParserBase16::Remainder(TPtrC16& aRemainder) const
	{
	__ASSERT_ALWAYS(iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Check to see if there is a segment left
	if( iNextSegmentPos == KErrNotFound )
		{
		// There is no segment
		return KErrNotFound;
		}
	// Find the previous delimiter -> the start of the current segment
	TInt prev = PrevDelimiterPosition(iDataDes, iNextSegmentPos, iDelimiter, iMode);

	// Need to see which direction the parsing is going to set the remainder
	switch(iMode)
		{
	case EDelimitedDataForward:
		{
		aRemainder.Set(iDataDes.Right(iDataDes.Length() - prev));
		} break;
	case EDelimitedDataReverse:
		{
		aRemainder.Set(iDataDes.Left(prev));
		} break;
	default:
		// Bad mode!
		User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrBadDelimitedParserMode);
		break;
		}
	return KErrNone;
	}
	
/**
	This parses the data into segments from left to right.
	
	@warning		There will be a KDelimitedParserErrNoDelimiter panic if the delimiter
	has not been set.
	@since			6.0
	@param			aData	A descriptor containing the data.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of
	parsing is set from left to right (EDelimitedDataFroward).
 */
EXPORT_C void TDelimitedParserBase16::Parse(const TDesC16& aData)
	{
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Initialise data for EForward direction
	iMode = EDelimitedDataForward;
	DoParse(aData);
	}

/**
	This parses the data into segments from lright to left.
	
	@warning		There will be a KDelimitedParserErrNoDelimiter panic if the delimiter
	has not been set.
	@since			6.0
	@param			aData	A descriptor containing the data.
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of
	parsing is set from right to left (EDelimitedDataReverse).
 */
EXPORT_C void TDelimitedParserBase16::ParseReverse(const TDesC16& aData)
	{
	__ASSERT_ALWAYS(iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Initialise data for EReverse direction
	iMode = EDelimitedDataReverse;
	DoParse(aData);
	}

/**
	Sets the delimiting character.
	
	@since			6.0
	@param			aDelimiter	The delimiting character.
	@post			The delimiting character is set.
*/
EXPORT_C void TDelimitedParserBase16::SetDelimiter(TChar aDelimiter)
	{
	iDelimiter = aDelimiter;
	}

/**
	Initialises the parsing of the data.
	
	@since			6.0
	@param			aData	A descriptor reference with the data.
	@pre 			The delimiting character has been set.
	@post			The data descriptor is set to the input argument. The current segment 
	refers to the initial segment of the data.
 */
void TDelimitedParserBase16::DoParse(const TDesC16& aData)
	{
	// Reset the segment information, then set the new Data - set pointer to NULL and length to zero
	iCurrentSegment.Set(NULL,0);
	iDataDes.Set(aData);

	// Check that there is a string!
	if( iDataDes.Length() == 0 )
		{
		// No string - ensure functionality blocked for this descriptor
		iNextSegmentPos = KErrNotFound;
		return;
		}
	// Find the segment - search from initial start position
	iNextSegmentPos = FindNextSegment(InitialDelimiterPosition(iDataDes, iMode));
	}

/**
	Finds the next segment from the given start position.
	
	@since			6.0
	@param			aStartPos	The position from where to start the search for the
	next segment.
	@return			The position of delimiter after the specified start position, or
	an error value of KErrNotFound if no more delimiters are found.
 */
TInt TDelimitedParserBase16::FindNextSegment(TInt aStartPos) const
	{
	// Find position of next delimiter
	TInt next = NextDelimiterPosition(iDataDes, aStartPos, iDelimiter, iMode);

	if( next != KErrNotFound )
		{
		TInt startPos = next < aStartPos ? next : aStartPos;
		TInt endPos = next < aStartPos ? aStartPos : next;
		if( iDataDes[startPos] == iDelimiter )
			{
			// Move past delimiter
			++startPos;
			}
		TInt length = endPos - startPos;
		iCurrentSegment.Set(iDataDes.Mid(startPos, length));
		}
	return next;
	}
	
/**
	Finds the previous segment from the given start position.
	
	@since			6.0
	@param			aStartPos	The position from where to start the search for the
	previous segment.
	@return			The position of delimiter before the specified start position, or
	an error value of KErrNotFound if no more delimiters are found.
*/
TInt TDelimitedParserBase16::FindPrevSegment(TInt aStartPos) const
	{
	// Find position of previous delimiter 
	TInt prev = PrevDelimiterPosition(iDataDes, aStartPos, iDelimiter, iMode);
	
	if( prev != KErrNotFound )
		{
		TInt startPos = prev < aStartPos ? prev : aStartPos;
		TInt endPos = prev < aStartPos ? aStartPos : prev;
		if( iDataDes[startPos] == iDelimiter )
			{
			// Move past delimiter
			++startPos;
			}
		TInt length = endPos - startPos;
		iCurrentSegment.Set(iDataDes.Mid(startPos, length));
		}
	return prev;
	}

//
//
// Implementation of CDelimitedDataBase16
//
//

/**
	Destructor.
	
	@since			6.0
 */
EXPORT_C CDelimitedDataBase16::~CDelimitedDataBase16()
	{
	delete iDataBuf;
	}

/**
	Inserts the new segment in a position before the current parsed	segment. The new segment can be 
	made up of several segments and have delimiters at either extreme. The insert functionality will 
	ensure that there is always a delimiter at the front of the new segment. The parser is left in a 
	state where its current segment is the same one as before the insertion.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not been parsed, 
	and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@param			aSegment	A descriptor with the new segment to be inserted.
	@pre 			The data must have been initially parsed by Parse() or ParseReverse().
	@post			The data will have been extended to include the new segment. The current segment 
	will remain as the one before the insertion.
 */
EXPORT_C void CDelimitedDataBase16::InsertCurrentL(const TDesC16& aSegment)
	{
	__ASSERT_ALWAYS(iParser.iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	DoInsertL(aSegment);
	}
	
/**
	Removes the current segment. After removing the segment, the parser's new current segment will be the 
	next segment. If the last segment is the one that is removed then the parser will be set to the end of 
	the data.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not been parsed, and 
	a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set.
	@since			6.0
	@pre 			The data must have been initially parsed by Parse() or ParseReverse().
	@post			The data will have been reduced to exclude the removed segment.	The current segment will 
	be set to what was the next segment. If the removed segment was the last segment, the parser is at the end 
	of the data.
 */
EXPORT_C void CDelimitedDataBase16::RemoveCurrentL()
	{
	__ASSERT_ALWAYS(iParser.iMode != EDelimitedDataNotParsed, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNotParsed));
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	DoRemoveL();
	}

/**
	Adds a new segment to the end of the data. The new segment can be made up of several segments and have  
	delimiters at either extreme. The insert functionality will ensure that there is always a delimiter at 
	the front of the new segment. The data must re-parsed to ensure that the parser is valid.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not been parsed, and a 
	KDelimitedParserErrNoDelimiter panic if the delimiter has not been set. A re-parse is required to ensure 
	that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the new segment to be inserted.
	@pre 			The delimiter must have been set.
	@post			The data will have been extended to include the new segment.
 */
EXPORT_C void CDelimitedDataBase16::PushBackL(const TDesC16& aSegment)
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Parse the string in reverse direction - sets last segment as current
	iParser.ParseReverse(*iDataBuf);

	// Insert the segment
	DoInsertL(aSegment);

	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Removes the last segment from the data. The data must be re-parsed to ensure that the parser is valid.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not been parsed, 
	and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set. A re-parse is required 
	to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data will have been reduced to exclude the last segment.
 */
EXPORT_C void CDelimitedDataBase16::PopBackL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Parse the string in reverse direction - sets last segment as current
	iParser.ParseReverse(*iDataBuf);

	// Remove the current segment
	DoRemoveL();

	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}


/**
	Adds a new segment to the front of the data. The new segment can be made up of several segments and have  
	delimiters at either extreme. The insert functionality will ensure that there is always a delimiter at 
	the front of the new segment. The data must re-parsed to ensure that the parser is valid.
							
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not been parsed, and 
	a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set. A re-parse is required to ensure 
	that the parser is valid.
	@since			6.0
	@param			aSegment	A descriptor with the new segment to be inserted.
	@pre 			The delimiter must have been set.
	@post			The data will have been extended to include the new segment.
 */
EXPORT_C void CDelimitedDataBase16::PushFrontL(const TDesC16& aSegment)
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Parse the string in forward direction - sets first segment as current
	iParser.Parse(*iDataBuf);

	// Insert the segment
	DoInsertL(aSegment);

	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Removes the first segment from the data. The data must be re-parsed to ensure that the parser is valid.
						
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has	not been 
	parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set. A re-parse 
	is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data will have been reduced to exclude the last segment.
 */
EXPORT_C void CDelimitedDataBase16::PopFrontL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Parse the string in forward direction - sets first segment as current
	iParser.Parse(*iDataBuf);

	// Remove the current segment
	DoRemoveL();

	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}


/**
	Removes the front delimiter (if exists) from the data.
	
	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data might have been reduced to exclude the front delimiter.
 */
EXPORT_C void CDelimitedDataBase16::TrimFrontDelimiterL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Search for delimiter
	if( iParser.FrontDelimiter() )
		{
		// Remove front delimiter and update member data
		SetDataL(iParser.iDataDes.Right(iParser.iDataDes.Length() - 1));
		}
	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Adds a delimiter to the front of the data (if it doesn't exist).

	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not
	been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set. 
	A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data might have been extended to include a front delimiter.
 */
EXPORT_C void CDelimitedDataBase16::AddFrontDelimiterL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	if( !iParser.FrontDelimiter() )
		{
		// Create a new buffer of correct size
		HBufC16* buf = HBufC16::NewL(iParser.iDataDes.Length() + 1);
		TPtr16 str = buf->Des();

		// Append a delimiter, then append the current string
		str.Append(iParser.iDelimiter);
		str.Append(iParser.iDataDes);

		// Set buffer to this new string
		SetData(buf);
		}
	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Removes the back delimiter (if exists) from the data.

	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has
	not been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not 
	been set. A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data might have been reduced to exclude the front delimiter.
 */
EXPORT_C void CDelimitedDataBase16::TrimBackDelimiterL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	// Search for delimiter
	if( iParser.BackDelimiter() )
		{
		// Remove back delimiter and update member data
		SetDataL(iParser.iDataDes.Left(iParser.iDataDes.Length() - 1));
		}
	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}

/**
	Adds a delimiter to the back of the data (if it doesn't exist).

	@warning		There will be a KDelimitedParserErrNotParsed panic if the data has not 
	been parsed, and a KDelimitedParserErrNoDelimiter panic if the delimiter has not been set. 
	A re-parse is required to ensure that the parser is valid.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The data might have been extended to include a front delimiter.
 */
EXPORT_C void CDelimitedDataBase16::AddBackDelimiterL()
	{
	__ASSERT_ALWAYS(iParser.iDelimiter != 0, User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrNoDelimiter));

	if( !iParser.BackDelimiter() )
		{
		// Create a new buffer of correct size
		HBufC16* buf = HBufC16::NewL(iParser.iDataDes.Length() + 1);
		TPtr16 str = buf->Des();

		// Append the current string, then append a delimiter
		str.Append(iParser.iDataDes);
		str.Append(iParser.iDelimiter);

		// Set buffer to this new string
		SetData(buf);
		}
	// Make sure that a re-parse is required
	iParser.iMode = EDelimitedDataNotParsed;
	}
	
/**
	This parses the data into segments from left to right.

	@warning		There will be a KDelimitedParserErrNoDelimiter panic if the delimiter
	has not been set.
	@since			6.0
	@pre 			The delimiter must have been set.
	@post			The current segment is the leftmost segment and the direction of
	parsing is set from left to right (EDelimitedDataFroward).
 */
EXPORT_C void CDelimitedDataBase16::Parse()
	{
	// This call will panic with KUriUtilsErrNoDelimiter if the delimiter is not set
	iParser.Parse(*iDataBuf);
	}

/**
	This parses the string into segments from right to left.
	
	@since			6.0
	@pre 			The delimiter must have been set. Will get a KDelimitedParserErrNoDelimiter panic if
	the delimiter has not been initialized.
	@post			The current segment is the leftmost segment and the direction of parsing is right to left.
 */
EXPORT_C void CDelimitedDataBase16::ParseReverse()
	{
	// This call will panic with KUriUtilsErrNoDelimiter if the delimiter is not set
	iParser.ParseReverse(*iDataBuf);
	}
	
/**
	Retrieves a const reference to the delimited data parser.

	@since			6.0
	@return			A const reference to the delimited data parser.
 */
EXPORT_C const TDelimitedParserBase16& CDelimitedDataBase16::Parser() const
	{
	return iParser;
	}
	
/**
	Sets the delimiting character.
	
	@since			6.0
	@param			aDelimiter	The delimiting character.
	@post			The delimiting character is updated.
 */
EXPORT_C void CDelimitedDataBase16::SetDelimiter(TChar aDelimiter)
	{
	iParser.SetDelimiter(aDelimiter);
	}

/**
	Constructor. First phase of two-phase construction method. Does non-allocating construction.
	
	@since			6.0
 */
EXPORT_C CDelimitedDataBase16::CDelimitedDataBase16()
	{
	}
	
/**
	Second phase of two-phase construction method. Does any	allocations required to fully construct 
	the object.
						
	@since			6.0
	@param			aData	A descriptor with the initial string.
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed.
 */
EXPORT_C void CDelimitedDataBase16::ConstructL(const TDesC16& aData)
	{
	// Create copy of string and set descriptor in the parser
	SetDataL(aData);
	}
	
/**
	Inserts the new segment in a position before the current segment. The new segment can be made up of 
	several segments and have delimiters at either extreme. The insert functionality will ensure that 
	there is always a delimiter at the front of the	new segment. The parser will be left in a state where 
	its current segment is the same one as before the insertion.
						
	@since			6.0
	@param			aSegment	The descriptor with the segment to be inserted.
	@pre 			The string must have been parsed.
	@post			The string will have been extended to include the new segment. The current segment will
	remain as the one before the insertion.
 */
void CDelimitedDataBase16::DoInsertL(const TDesC16& aSegment)
	{
	TInt prevPos = PrevDelimiterPosition(iParser.iDataDes, iParser.iNextSegmentPos, iParser.iDelimiter, iParser.iMode);
	TPtrC16 prefix = iParser.iDataDes.Left(prevPos);

	TInt suffixLength = iParser.iDataDes.Length() - prevPos;
	TPtrC16 suffix = iParser.iDataDes.Right(suffixLength);
	if( suffixLength && suffix[0] == iParser.iDelimiter )
		{
		// Remove front delimiter on suffix
		suffix.Set(suffix.Right(--suffixLength));
		}	

	// Check for delimiters...
	TPtrC16 segment = aSegment;
	TInt segmentLength = segment.Length();
	// Check the last character in segment
	TBool segmentBackDelimiter = (segmentLength && segment[segmentLength - 1] == iParser.iDelimiter);
	if( segmentBackDelimiter )
		{
		// Remove back delimiter from the segment
		segment.Set(segment.Left(--segmentLength));
		}
	// Check the first character in segment...
	if( segmentLength && segment[0] == iParser.iDelimiter )
		{
		// Remove front delimiter from the segment
		segment.Set(segment.Right(--segmentLength));
		}

	// Check if a back delimiter is needed - NOTE always add a front delimiter
	TInt extra = 1;
	TBool needBackDelimiter = EFalse;
	if( suffix.Length() || segmentBackDelimiter )
		{
		++extra;
		needBackDelimiter = ETrue;
		}
	// Create space for new string
	HBufC16* buf = HBufC16::NewL(prevPos + segmentLength + suffixLength + extra);
	TPtr16 str = buf->Des();

	// Form the new string
	str.Append(prefix);
	str.Append(iParser.iDelimiter);
	str.Append(segment);
	if( needBackDelimiter )
		str.Append(iParser.iDelimiter);
	str.Append(suffix);

	// Update string data
	SetData(buf);

	// Check to see if the internal parser object (iParser) has been parsed
	// (can tell if it has if the data pointer in iCurrentSegment is not NULL)
	// If so update iCurrentSegment to ensure that iParser remains valid
	if( iParser.iCurrentSegment.Ptr() )
		{
		// Ensure parser is in correct position and current segment is correct
		iParser.iNextSegmentPos = prevPos;
		if( iParser.iMode == EDelimitedDataForward )
			{
			// Move iterator to delimiter before iCurrentSegment - length of segment + a delimiter
			iParser.iNextSegmentPos += segmentLength + 1;
			}
		// Get the next segment
		iParser.iNextSegmentPos = iParser.FindNextSegment(iParser.iNextSegmentPos);
		}
	}

/**
	Removes the current segment. After removing the segment, the parser's new current segment will be the 
	next segment. If the last segment is the one that is removed then the parser will be set to the end of 
	the data.
						
	@since			6.0
	@pre 			The data must have been parsed.
	@post			The data will have been reduced to exclude the removed data. The current segment 
	is set to what was the next segment. If the removed segment was	the last segment, the parser is 
	at the end of the data.
 */
void CDelimitedDataBase16::DoRemoveL()
	{
	// Check if there is anything to remove
	if( iParser.iDataDes.Length() == 0 )
		{
		return;
		}
	// Find the previous delimiter
	TInt prev = PrevDelimiterPosition(iParser.iDataDes, iParser.iNextSegmentPos, iParser.iDelimiter, iParser.iMode);

	// Set up the start and end position of current segment
	TInt endPos = iParser.iNextSegmentPos;
	TInt startPos = iParser.iNextSegmentPos;
	if( prev < iParser.iNextSegmentPos )
		startPos = prev;
	else
		endPos = prev;

	// Ok, get the prefix and suffix parts
	TPtrC16 prefix = iParser.iDataDes.Left(startPos);
	TInt suffixLength = iParser.iDataDes.Length() - endPos;
	TPtrC16 suffix = iParser.iDataDes.Right(suffixLength);

	// Create the space
	HBufC16* buf = HBufC16::NewL(startPos + suffixLength);
	TPtr16 str = buf->Des();

	// Form the new string
	str.Append(prefix);
	str.Append(suffix);

	// Update string data
	SetData(buf);

	// Ensure parser is in correct position
	iParser.iNextSegmentPos = iParser.FindNextSegment(startPos);
	}

/**
	Updates internal data buffer with the new data. Creates a copy of the new data.
						
	@since			6.0
	@param			aData	A descriptor with the new string.
	@post			The internal data buffer now contains a copy of the new data and the
	parser is set to the new data.
 */
void CDelimitedDataBase16::SetDataL(const TDesC16& aData)
	{
	// Cleanup old data and set new
	HBufC16* buf =  aData.AllocL();
	SetData(buf);
	}
	
/**
	Sets internal data buffer and parser. Cleans up the old data and uses the data buffer. The 
	parser is set to the new data.
						
	@since			6.0
	@param			aDataBuf	A pointer to a decriptor buffer with the new data.
	@post			The internal data buffer now points to the new buffer and the parser
	is set to the data in the new buffer..
 */
void CDelimitedDataBase16::SetData(HBufC16* aDataBuf)
	{
	delete iDataBuf;
	iDataBuf = aDataBuf;
	iParser.iDataDes.Set(*iDataBuf);
	}

//
//
// Implementation of LOCAL functions
//
//

/**
	Finds the position of the next delimiter in the data.
	
	@since			6.0
	@param			aData		A descriptor with the delimited data.
	@param			aStartPos	The position from where to start the search for the delimiter.
	@param			aDelimiter	The delimiting character.
	@param			aMode		The parsing mode.
	@return			The position of delimiter after the specified start position, or 
	an error value of KErrNotFound if no more delimiters are found.	
	@pre			None
	@post			Unspecified
 */
template<class TDesCType>
TInt NextDelimiterPosition(const TDesCType& aData, TInt aStartPos, TInt aDelimiter, TDelimitedDataParseMode aMode)
	{
	if( aStartPos == KErrNotFound )
		{
		// Have got to the end - initialise the iterator
		return InitialDelimiterPosition(aData, aMode);
		}
	TInt next = KErrNotFound;
	switch( aMode )
		{
	case EDelimitedDataForward:
		{
		// Search parsed string for next delimiter
		next = LeftDelimiterPosition(aData, aStartPos, aDelimiter);
		} break;
	case EDelimitedDataReverse:
		{
		// Search parsed string for next delimiter
		next = RightDelimiterPosition(aData, aStartPos, aDelimiter);
		} break;
	default:
		// Bad mode!
		User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrBadDelimitedParserMode);
		break;
		}
	return next;
	}


/**
	Finds the position of the previous delimiter in the data from the given start position.
	
	@since			6.0
	@param			aData		A descriptor with the delimited data.
	@param			aStartPos	The position from where to start the search for the delimiter.
	@param			aDelimiter	The delimiting character.
	@param			aMode		The parsing mode.
	@return			The position of delimiter before the specified start position, or 
	an error value of KErrNotFound if no more delimiters are found.	
	@pre			None
	@post			Unspecified
 */
template<class TDesCType>
TInt PrevDelimiterPosition(const TDesCType& aData, TInt aStartPos, TInt aDelimiter, TDelimitedDataParseMode aMode)
	{
	// Switch modes, then find the next delimiter, switch back
	TDelimitedDataParseMode mode = aMode == EDelimitedDataForward ? EDelimitedDataReverse : EDelimitedDataForward;
	return NextDelimiterPosition(aData, aStartPos, aDelimiter, mode);
	}

/**
	Finds the position of the delimiter to the right of the given start position.
	
	@since			6.0
	@param			aData		A descriptor with the delimited data.
	@param			aStartPos	The position from where to start the search for the delimiter.
	@param			aDelimiter	The delimiting character.
	@return			The position of delimiter to the right of the specified start position, or 
	an error value of KErrNotFound if no more delimiters are found.	
	@pre			None
	@post			Unspecified
 */
template<class TDesCType>
TInt RightDelimiterPosition(const TDesCType& aData, TInt aStartPos, TInt aDelimiter)
	{
	// Find position of right-most delimiter in the descriptor data to left of aStartPos
	if( aStartPos == 0 )
		{
		// There is no data
		return KErrNotFound;
		}
	TInt rightDelimiterPos = aData.Left(aStartPos).LocateReverse(aDelimiter);

	// See if a delimiter was found
	if( rightDelimiterPos == KErrNotFound )
		{
		// No - start of string delimits
		rightDelimiterPos = 0;
		}
	return rightDelimiterPos;
	}

/**
	Finds the position of the delimiter to the left of the given start position.
	
	@since			6.0
	@param			aData		A descriptor with the delimited data.
	@param			aStartPos	The position from where to start the search for the delimiter.
	@param			aDelimiter	The delimiting character.
	@return			The position of delimiter to the left of the specified start position, or 
	an error value of KErrNotFound if no more delimiters are found.	
	@pre			None
	@post			Unspecified
 */
template<class TDesCType>
TInt LeftDelimiterPosition(const TDesCType& aData, TInt aStartPos, TInt aDelimiter)
	{
	// Find position of left-most delimiter in the descriptor data to right of aStartPos
	const TInt length = aData.Length();
	TInt rightLength = length - aStartPos;
	if( rightLength == 0 )
		{
		// There is no data
		return KErrNotFound;
		}
	// Ok there is some string to search - remove delimiter
	--rightLength;
	TInt leftDelimiterPos = aData.Right(rightLength).Locate(aDelimiter);

	// See if a delimiter was found
	if( leftDelimiterPos == KErrNotFound )
		{
		// No - end of string delimits
		leftDelimiterPos = length;
		}
	else
		{
		// Offset the delimiter found - include delimiter that was removed
		leftDelimiterPos += aStartPos + 1;
		}
	return leftDelimiterPos;
	}

/**
	Retrieves the initial position for searching delimited data for a given parsing mode.
	
	@since			6.0
	@param			aData		A descriptor with the delimited data.
	@param			aMode		The parsing mode.
	@return			The initial position for parsing the data.
	@pre			None
	@post			Unspecified
 */
template<class TDesCType>
TInt InitialDelimiterPosition(const TDesCType& aData, TDelimitedDataParseMode aMode)
//
// Initialises iNextSegmentPos
	{
	TInt initPos = KErrNotFound;
	switch( aMode )
		{
	case EDelimitedDataForward:
		{
		// Search parsed string for next delimiter
		initPos = 0;
		} break;
	case EDelimitedDataReverse:
		{
		// Search parsed string for next delimiter
		initPos = aData.Length();
		} break;
	default:
		// Bad mode!
		User::Panic(KDelimitedParserPanicCategory, KUriUtilsErrBadDelimitedParserMode);
		break;
		}
	return initPos;
	}

