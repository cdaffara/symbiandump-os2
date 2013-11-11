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

#include <e32base.h>

//
//
//	Implementation of LOCAL functions - helper functions
//
//

template<class HBufCType, class TPtrCType, class TDelimitedParserType>
TInt DoDelimitedDataParsingL(const TDelimitedParserType& aParser, VA_LIST& aSegments)
	{
	// Loop through the list
	TPtrCType parseSegment;
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aSegments);
	while( pExpected )
		{
		// Get the parsed segment - test GetNext()
		if( aParser.GetNext(parseSegment) == KErrNotFound )
			{
			// Should have found something - error!
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Got the segment - compare against expected
		if( parseSegment.Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Ok, test Dec()...
		if( aParser.Dec() != KErrNone )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Test Peek()
		if( aParser.Peek(parseSegment) == KErrNotFound )
			{
			// Should have found something - error!
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Got the segment - compare against expected
		if( parseSegment.Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Ok, test Inc()...
		if( aParser.Inc() != KErrNone )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Ok get next expected segment
		CleanupStack::PopAndDestroy(pExpected);	// pExpected
		pExpected = GetNextArgLC<HBufCType>(aSegments);
		}
	// Got to the end - should parse no more segments
	if( aParser.GetNext(parseSegment) != KErrNotFound )
		{
		// Should not have found something - error!
		return KErrNotFound;
		}
	// Ok, test Dec()...
	if( aParser.Dec() != KErrNone )
		{
		return KErrNotFound;
		}
	// Test Peek()
	if( aParser.Peek(parseSegment) == KErrNotFound )
		{
		// Should have found something - error!
		return KErrNotFound;
		}
	// Ok, test Inc()...
	if( aParser.Inc() != KErrNone )
		{
		return KErrNotFound;
		}
	// Ok, test Eos()
	if( !aParser.Eos() != KErrNone )
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class HBufCType>
HBufCType* GetNextArgLC(VA_LIST& aList)
	{
	const TDesC* arg = VA_ARG(aList, const TDesC*);
	if( arg == NULL )
		{
		return NULL;
		}
	HBufCType* buf = HBufCType::NewLC(arg->Length());
	buf->Des().Copy(*arg);
	return buf;
	}

template<class TDelimitedParserType>
TInt DoDelimiterPresenceTest(const TDelimitedParserType& aParser, TBool aFrontDelim, TBool aBackDelim)
	{
	TBool frontDelim = aParser.FrontDelimiter();
	if( (!frontDelim && aFrontDelim) | (frontDelim && !aFrontDelim) )
		{
		return KErrNotFound;
		}
	TBool backDelim = aParser.BackDelimiter();
	if( (!backDelim && aBackDelim) | (backDelim && !aBackDelim) )
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class HBufCType, class TPtrCType, class TDelimitedParserType>
TInt DoDelimitedDataRemainderTestL(const TDelimitedParserType& aParser, VA_LIST& aRemainders)
	{
	// Loop through the list
	TPtrCType parseRemainder;
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aRemainders);
	while( pExpected )
		{
		// Get the remaining data
		if( aParser.Remainder(parseRemainder) == KErrNotFound )
			{
			// Should have found something - error!
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Got the remainder - compare against expected
		if( parseRemainder.Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Ok, parse the to the next segment...
		if( aParser.Inc() != KErrNone )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Ok get next expected remainder
		CleanupStack::PopAndDestroy(pExpected);	// pExpected
		pExpected = GetNextArgLC<HBufCType>(aRemainders);
		}
	// Got to the end - should get no remainder
	if( aParser.Remainder(parseRemainder) != KErrNotFound )
		{
		// Should not have found something - error!
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class TDelimitedParserType, class TDesCType>
TInt DoDelimitedDataDes(const TDelimitedParserType& aParser, const TDesCType& aDelimitedData)
	{
	if( aParser.Des().Compare(aDelimitedData) != 0)
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPushFrontL(CDelimitedDataType* aParser, VA_LIST& aData)
	{
	// Loop through the list
	TInt error = KErrNone;
	HBufCType* pData = GetNextArgLC<HBufCType>(aData);
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aData);
	while( pExpected && pData )
		{
		// Push front
		TRAP(error, aParser->PushFrontL(*pData));
		if( error != KErrNone )
			{
			CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
			return error;
			}
		// Check against expected
		if( aParser->Parser().Des().Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
			return KErrNotFound;
			}
		// Ok get next set of data
		CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
		pData = GetNextArgLC<HBufCType>(aData);
		pExpected = GetNextArgLC<HBufCType>(aData);
		}
	return KErrNone;
	}

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPushBackL(CDelimitedDataType* aParser, VA_LIST& aData)
	{
	// Loop through the list
	TInt error = KErrNone;
	HBufCType* pData = GetNextArgLC<HBufCType>(aData);
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aData);
	while( pExpected && pData )
		{
		// Push front
		TRAP(error, aParser->PushBackL(*pData));
		if( error != KErrNone )
			{
			CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
			return error;
			}
		// Check against expected
		if( aParser->Parser().Des().Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
			return KErrNotFound;
			}
		// Ok get next set of data
		CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
		pData = GetNextArgLC<HBufCType>(aData);
		pExpected = GetNextArgLC<HBufCType>(aData);
		}
	return KErrNone;
	}

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPopFrontL(CDelimitedDataType* aParser, VA_LIST& aData)
	{
	// Loop through the list
	TInt error = KErrNone;
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aData);
	while( pExpected )
		{
		// Push front
		TRAP(error, aParser->PopFrontL());
		if( error != KErrNone )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return error;
			}
		// Check against expected
		if( aParser->Parser().Des().Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Ok get next set of data
		CleanupStack::PopAndDestroy(pExpected);	// pExpected
		pExpected = GetNextArgLC<HBufCType>(aData);
		}
	return KErrNone;
	}

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPopBackL(CDelimitedDataType* aParser, VA_LIST& aData)
	{
	// Loop through the list
	TInt error = KErrNone;
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aData);
	while( pExpected )
		{
		// Push front
		TRAP(error, aParser->PopBackL());
		if( error != KErrNone )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return error;
			}
		// Check against expected
		if( aParser->Parser().Des().Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Ok get next set of data
		CleanupStack::PopAndDestroy(pExpected);	// pExpected
		pExpected = GetNextArgLC<HBufCType>(aData);
		}
	return KErrNone;
	}

template<class CDelimitedDataType>
TInt DoDelimitedDataAddAndTrimFrontAndBackDelimiterL(CDelimitedDataType* aParser)
	{
	TInt error = KErrNone;
	// Add front delimiter
	TRAP(error, aParser->AddFrontDelimiterL());
	if( error != KErrNone )
		{
		return KErrNotFound;
		}
	if( !aParser->Parser().FrontDelimiter() )
		{
		return KErrNotFound;
		}
	// Trim front delimiter
	TRAP(error, aParser->TrimFrontDelimiterL());
	if( error != KErrNone )
		{
		return KErrNotFound;
		}
	if( aParser->Parser().FrontDelimiter() )
		{
		return KErrNotFound;
		}
	// Add back delimiter
	TRAP(error, aParser->AddBackDelimiterL());
	if( error != KErrNone )
		{
		return KErrNotFound;
		}
	if( !aParser->Parser().BackDelimiter() )
		{
		return KErrNotFound;
		}
	// Trim back delimiter
	TRAP(error, aParser->TrimBackDelimiterL());
	if( error != KErrNone )
		{
		return KErrNotFound;
		}
	if( aParser->Parser().BackDelimiter() )
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class CDelimitedDataType>
TInt DoDelimitedDataParseToPosition(CDelimitedDataType* aParser, TInt aStartPos)
	{
	// Parse to correct place - initial parse must have been done
	TInt i=0;
	while( i < aStartPos && !aParser->Parser().Eos() )
		{
		aParser->Parser().Inc();
		++i;
		}
	if( i != aStartPos )
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class HBufCType, class TPtrCType, class CDelimitedDataType>
TInt DoDelimitedDataInsertL(CDelimitedDataType* aParser, VA_LIST& aData)
	{
	// Loop through the list and insert!
	TInt error = KErrNone;
	HBufCType* pData = GetNextArgLC<HBufCType>(aData);
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aData);
	while( pExpected && pData )
		{
		// Get copy of current segment
		TPtrCType seg;
		aParser->Parser().Peek(seg);
		HBufCType* segCopy = seg.AllocLC();
		// Insert current
		TRAP(error, aParser->InsertCurrentL(*pData));
		if( error != KErrNone )
			{
			CleanupStack::PopAndDestroy(3, pData);	// pData, pExpected, segCopy
			return error;
			}
		// Check against expected
		if( aParser->Parser().Des().Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(3, pData);	// pData, pExpected, segCopy
			return KErrNotFound;
			}
		// Check that the current segment hasn't changed
		aParser->Parser().Peek(seg);
		if( segCopy->Compare(seg) != 0 )
			{
			CleanupStack::PopAndDestroy(3, pData);	// pData, pExpected, segCopy
			return KErrNotFound;
			}
		// Ok get next set of data
		CleanupStack::PopAndDestroy(3, pData);	// pData, pExpected, segCopy
		pData = GetNextArgLC<HBufCType>(aData);
		pExpected = GetNextArgLC<HBufCType>(aData);
		}
	return KErrNone;
	}

template<class HBufCType, class TPtrCType, class CDelimitedDataType>
TInt DoDelimitedDataRemoveL(CDelimitedDataType* aParser, VA_LIST& aData)
	{
	// Loop through the list
	TInt error = KErrNone;
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aData);
	while( pExpected )
		{
		// Remove current
		TRAP(error, aParser->RemoveCurrentL());
		if( error != KErrNone )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return error;
			}
		// Check against expected
		if( aParser->Parser().Des().Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(pExpected);	// pExpected
			return KErrNotFound;
			}
		// Ok get next set of data
		CleanupStack::PopAndDestroy(pExpected);	// pExpected
		pExpected = GetNextArgLC<HBufCType>(aData);
		}
	return KErrNone;
	}

template<class HBufCType, class TPtrCType, class CDelimitedDataType>
TInt DoDelimitedDataInsertAndEscapeL(CDelimitedDataType* aParser, VA_LIST& aData)
	{
	// Loop through the list and insert!
	TInt error = KErrNone;
	HBufCType* pData = GetNextArgLC<HBufCType>(aData);
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aData);
	while( pExpected && pData )
		{
		// Get copy of current segment
		TPtrCType seg;
		aParser->Parser().Peek(seg);
		HBufCType* segCopy = seg.AllocLC();
		// Insert current
		TRAP(error, aParser->InsertAndEscapeCurrentL(*pData));
		if( error != KErrNone )
			{
			CleanupStack::PopAndDestroy(3, pData);	// pData, pExpected, segCopy
			return error;
			}
		// Check against expected
		if( aParser->Parser().Des().Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(3, pData);	// pData, pExpected, segCopy
			return KErrNotFound;
			}
		// Check that the current segment hasn't changed
		aParser->Parser().Peek(seg);
		if( segCopy->Compare(seg) != 0 )
			{
			CleanupStack::PopAndDestroy(3, pData);	// pData, pExpected, segCopy
			return KErrNotFound;
			}
		// Ok get next set of data
		CleanupStack::PopAndDestroy(3, pData);	// pData, pExpected, segCopy
		pData = GetNextArgLC<HBufCType>(aData);
		pExpected = GetNextArgLC<HBufCType>(aData);
		}
	return KErrNone;
	}

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPushAndEscapeFrontL(CDelimitedDataType* aParser, VA_LIST& aData)
	{
	// Loop through the list
	TInt error = KErrNone;
	HBufCType* pData = GetNextArgLC<HBufCType>(aData);
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aData);
	while( pExpected && pData )
		{
		// Push front
		TRAP(error, aParser->PushAndEscapeFrontL(*pData));
		if( error != KErrNone )
			{
			CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
			return error;
			}
		// Check against expected
		if( aParser->Parser().Des().Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
			return KErrNotFound;
			}
		// Ok get next set of data
		CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
		pData = GetNextArgLC<HBufCType>(aData);
		pExpected = GetNextArgLC<HBufCType>(aData);
		}
	return KErrNone;
	}

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPushAndEscapeBackL(CDelimitedDataType* aParser, VA_LIST& aData)
	{
	// Loop through the list
	TInt error = KErrNone;
	HBufCType* pData = GetNextArgLC<HBufCType>(aData);
	HBufCType* pExpected = GetNextArgLC<HBufCType>(aData);
	while( pExpected && pData )
		{
		// Push front
		TRAP(error, aParser->PushAndEscapeBackL(*pData));
		if( error != KErrNone )
			{
			CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
			return error;
			}
		// Check against expected
		if( aParser->Parser().Des().Compare(*pExpected) != 0 )
			{
			CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
			return KErrNotFound;
			}
		// Ok get next set of data
		CleanupStack::PopAndDestroy(2, pData);	// pData, pExpected
		pData = GetNextArgLC<HBufCType>(aData);
		pExpected = GetNextArgLC<HBufCType>(aData);
		}
	return KErrNone;
	}
