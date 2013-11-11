/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "TXTSTD.H"
#include "ParseLst.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ParseLstTraces.h"
#endif



//++ sort out definitive hash includes


CParserList::CParserItem::CParserItem(MParser* aParser, const TUint aTagIndex)
: iParser(aParser),
  iTagIndex(aTagIndex)
	{
	}


// Panic the process with UikParse as the category.
void CParserList::Panic(TParserListPanic aPanic) const
	{
	OstTraceExt2( TRACE_FATAL, DUP1_CPARSERLIST_PANIC, "CParserList::Panic;this=%x;aPanic=%x", (unsigned int)this, ( TUint )&( aPanic ) );
	_LIT(panicStr, "ParseLst");
	User::Panic(panicStr, aPanic);
	}


CParserList::CParserList()
: iParserList(4)
	{
	iHighestIndex = 0;
	iNumberInList = 0;
	}


CParserList::~CParserList()
	{
	iParserList.ResetAndDestroy();	// Reset the list and destroy the CParserItems
	iParserList.Close();			// but leave the parsers and free the resources
	}


// Add parser to appropriate list
void CParserList::ActivateAParserL(MParser* aParser, const TBool aDefaultParser)
	{
	// Check the index isn't rolling over
	// N.B. This is somewhat unlikely. Since index is effectively a 29-bit
	// uint (I'm using the top 3 bits as flags) this allows 268435456 parsers
	// to be installed. (Assuming we deinstall almost as many, as we went
	// along, to avoid OOM.) At 50 per sec that's 20 years continuous action!
	if (iHighestIndex >= EMaxParserIndex)
	    {
	    OstTrace0( TRACE_DUMP, CPARSERLIST_ACTIVATEAPARSERL, "EParserIndexRollover" );
	    }
	__ASSERT_DEBUG((iHighestIndex < EMaxParserIndex), Panic(EParserIndexRollover));
	if (!aParser)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CPARSERLIST_ACTIVATEAPARSERL, "EParserNullPtr" );
	    }
	__ASSERT_DEBUG(aParser, Panic(EParserNullPtr));
	TUint parserIndex = iHighestIndex + 1;
	// Adjust flags to describe parser
	if (aParser->ReformatOnRecognise())
		parserIndex |= static_cast<TUint>( EReformatOnRecogniseFlag );
	if (aParser->ReformatOnRollover())
		parserIndex |= EReformatOnRolloverFlag;
	if (aDefaultParser)
		parserIndex |= EDefaultParserFlag;
	// Create a parser item
	TInt result;
	CParserItem* parserItem = new (ELeave) CParserItem(aParser, parserIndex);
	++iHighestIndex;
	if (aDefaultParser)
		{
		CleanupStack::PushL(parserItem);
		result = iParserList.Append(parserItem);
		if (result) // We couldn't get it in the main list
			User::Leave(result);
		CleanupStack::Pop();
		}
	else
		{
		// Look for the end of the specific parsers and the start of the default set
		TInt count;
		for (count = 0; count < iNumberInList; count++)
			{
			if (DefaultParser(iParserList[count]->TagIndex()))
				break;
			}
		CleanupStack::PushL(parserItem);
		result = iParserList.Insert(parserItem, count);
		if (result) // We couldn't get it in the main list
			User::Leave(result);
		CleanupStack::Pop();
		}
	iNumberInList++;
	}


// Activate an individual parser
void CParserList::ActivateParserL(MParser* aParser)
	{
	ActivateAParserL(aParser, EFalse);
	}


// N.B. We can't just delete CParserList and zero TLS when we get back
// to a list with no specific or default parsers because there could be
// an EText left with a local ptr to it.
void CParserList::DeactivateParser(MParser* aParser)
	{
	if (!iNumberInList)
	    {
	    OstTrace0( TRACE_DUMP, CPARSERLIST_DEACTIVATEPARSER, "EParserNoneActive" );
	    }
	__ASSERT_DEBUG(iNumberInList, Panic(EParserNoneActive));
	if (!aParser)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CPARSERLIST_DEACTIVATEPARSER, "EParserNullPtr" );
	    }
	__ASSERT_DEBUG(aParser, Panic(EParserNullPtr));
	// Run thru list till find entry we need
	TInt count;
	for (count = 0; count < iNumberInList; count++)
		{
		if (iParserList[count]->Parser() == aParser)
			{
			delete iParserList[count];
			iParserList.Remove(count);
			break;
			}
		}
	if (count >= iNumberInList)
	    {
	    OstTrace0( TRACE_DUMP, DUP2_CPARSERLIST_DEACTIVATEPARSER, "EParserInstanceNotActive" );
	     }
	__ASSERT_DEBUG((count < iNumberInList), Panic(EParserInstanceNotActive));
	iNumberInList--;
	}


// Activate a parser as one of the default set
void CParserList::ActivateDefaultParserL(MParser* aParser)
	{
	ActivateAParserL(aParser, ETrue);
	}


//++ Put comment here
void CParserList::DeactivateParserDefaults()
	{
	if (iNumberInList)
		{
		// Take them out of the list
		while (iNumberInList && DefaultParser(iParserList[iNumberInList - 1]->TagIndex()))
			{
			// Tell the parser to free itself
			iParserList[iNumberInList - 1]->Parser()->Release();
			// Delete the item that refers to it
			delete iParserList[iNumberInList - 1];
			// Remove the entry from the list
			iParserList.Remove(iNumberInList - 1);
			iNumberInList--;
			}
		}
	}


// Called by EText to scan an area of text
TBool CParserList::ParseThisText(CRichText& aTextObj,TInt aStartScan,TInt aScanLength,TInt& aStartOfTags,TInt& aLength)
	{
	TBool foundSomething = EFalse;
	TCharFormatX format;
	TCharFormatXMask varies;
	TPtrC ptr;
	TInt endRange;

	// Scan either side of the range in case part of some tagged text was deleted.
	if (aStartScan > 0)
		{
		aStartScan--;
		aScanLength++;
		}
	if (aStartScan + aScanLength < aTextObj.DocumentLength())
		aScanLength++;

	if (iNumberInList && aScanLength)
		{
		aStartOfTags = aStartScan + aScanLength;
		aLength = 0;
		for (TInt count = 0; count < iNumberInList; count++)
			{
			// For each parser in the list
			TUint tagIndex = iParserList[count]->TagIndex();
			TInt pos;
			MParser* parser = iParserList[count]->Parser();
			TInt localStartScan = aStartScan;
			TInt localScanLength = aScanLength;
			// Start by removing existing tags for this parser. This ensures
			// that old tags that have been invalidated by subsequent editing
			// are removed. Any that are still valid will be replaced.
			aTextObj.GetExtendedCharFormat(format, varies, localStartScan, aScanLength);

			if (format.iParserTag || varies.AttribIsSet(EAttParserTag))
				{
				// At least some of the object contains a non-zero tag - go through it
				// Are we starting part way through a tag?
				aTextObj.GetExtendedCharFormat(format, varies, localStartScan, 1);
				if (format.iParserTag == tagIndex)
					{
					// The first char of this range has the current parsers tag
					// so we need to check backwards for the start of that tag
					for (pos = localStartScan; pos > 0; pos--)
						{
						aTextObj.GetExtendedCharFormat(format, varies, pos - 1, 1);
						if (format.iParserTag != tagIndex)
							break;
						}
					// Since we are going to remove a tag starting from here
					// we need to allow this area to be rescanned
					localScanLength += localStartScan - pos;
					localStartScan = pos;
					}
				// What about off the end?
				aTextObj.GetExtendedCharFormat(format, varies, localStartScan + localScanLength - 1, 1);
				if (format.iParserTag == tagIndex)
					{
					// The last char of this range has the current parsers tag
					// so we need to check forwards for the end of that tag
					pos = localStartScan + localScanLength;
					TInt end = aTextObj.DocumentLength();
					while (pos < end)
						{
						aTextObj.GetTextAndExtendedFormat(ptr, format, pos);
						if (format.iParserTag != tagIndex)
							break;
						pos += ptr.Length();
						}
					// Adjust scan length
					localScanLength = pos - localStartScan;
					}
				pos = localStartScan;
				while (pos < localStartScan + localScanLength)
					{
					// Run along the scan range
					aTextObj.GetTextAndExtendedFormat(ptr, format, pos);
					if (format.iParserTag == tagIndex)
						{
						// Remove this tag
						format.iParserTag = 0;
						varies.ClearAll();
						varies.SetAttrib(EAttParserTag);
						TRAPD(leaveCode, aTextObj.ApplyExtendedCharFormatL(format, varies, pos, ptr.Length()));
						if (leaveCode==KErrNone)
							foundSomething = ETrue;	// We are removing a tag
						if (aLength)
							{
							if (pos < aStartOfTags)
								{
								aLength += aStartOfTags - pos;
								aStartOfTags = pos;
								}
							if (pos + ptr.Length() > aStartOfTags + aLength)
								aLength = pos + ptr.Length() - aStartOfTags;
							}
						else
							{
							aStartOfTags = pos;
							aLength = ptr.Length();
							}
						}
					pos += ptr.Length();
					}
				}
			endRange = localStartScan + localScanLength;

			// For this parser, run through text looking for changes
			TBool allowBack = ETrue;
			for (;;)	// Run forever
				{
				TInt localStartTag = aTextObj.DocumentLength();
				TInt localTagLength = 0;
				TInt result = parser->ParseThisText(aTextObj, allowBack, localStartScan, localScanLength, localStartTag, localTagLength);
				if (!result)
					break;
				if (!allowBack && (localStartTag < localStartScan))
				    {
				    OstTrace0( TRACE_DUMP, CPARSERLIST_PARSETHISTEXT, "EParserIgnoringAllowFlag" );
				    }
				__ASSERT_DEBUG(allowBack || (localStartTag >= localStartScan), Panic(EParserIgnoringAllowFlag));
				TInt startNewTag = localStartTag;
				TInt lengthNewTag = localTagLength;
				aTextObj.GetExtendedCharFormat(format, varies, localStartTag, localTagLength);
				if (format.iParserTag || varies.AttribIsSet(EAttParserTag))
					{
					// At least some of this area contains a non-zero tag - go through it
					pos = localStartTag;
					TBool higher = EFalse;
					while (pos < localStartTag + localTagLength)
						{
						aTextObj.GetTextAndExtendedFormat(ptr, format, pos);
						if (format.iParserTag && (MaskedTag(format.iParserTag) < MaskedTag(tagIndex)))
							{
							// A higher precedence tag is already here so we can't
							// insert our tag - let's see how far it goes
							TUint tag = format.iParserTag;	// Stash tag before overwriting it
							TInt len = aTextObj.DocumentLength();
							while (pos < len)
								{
								aTextObj.GetTextAndExtendedFormat(ptr, format, pos);
								if (format.iParserTag != tag)
									break;
								pos += ptr.Length();
								}
							result = EFalse;
							startNewTag = pos;
							lengthNewTag = 0;
							break;
							}
						// If there aren't any higher precedence tags in here then this
						// will save us having to go through again if there aren't any
						// lower precedence tags either
						if (MaskedTag(format.iParserTag) >= MaskedTag(tagIndex))
							higher = ETrue;
						pos += ptr.Length();
						}
					if (higher)
						{
						// There are lower or equal precedence tags in this range
						// Do they extend back off the start?
						aTextObj.GetExtendedCharFormat(format, varies, localStartTag, 1);
						if (format.iParserTag)
							{
							// need to check backwards
							TUint tag = format.iParserTag;	// Stash tag before overwriting it
							for (pos = localStartTag; pos > 0; pos--)
								{
								aTextObj.GetExtendedCharFormat(format, varies, pos - 1, 1);
								if (format.iParserTag != tag)
									break;
								}
							localTagLength += localStartTag - pos;
							localStartTag = pos;
							}
						// What about off the end?
						pos = localStartTag + localTagLength;
						aTextObj.GetExtendedCharFormat(format, varies, pos - 1, 1);
						if (format.iParserTag)
							{
							// need to check forwards
							TUint tag = format.iParserTag;	// Stash tag before overwriting it
							TInt len = aTextObj.DocumentLength();
							while (pos < len)
								{
								aTextObj.GetTextAndExtendedFormat(ptr, format, pos);
								if (format.iParserTag != tag)
									break;
								pos += ptr.Length();
								}
							localTagLength = pos - localStartTag;
							}

						// Remove all tags in this area - they all have lower precedence
						format.iParserTag = 0;
						varies.ClearAll();
						varies.SetAttrib(EAttCharLanguage);
						TRAPD(leaveCode, aTextObj.ApplyExtendedCharFormatL(format, varies, localStartTag, localTagLength));
						if (leaveCode==KErrNone)
							foundSomething = ETrue;	// We are removing a tag
						}
					}

				if (result)
					{
					// Format tag this area with tagIndex
					format.iParserTag = tagIndex;
					varies.ClearAll();
					varies.SetAttrib(EAttParserTag);
					// Use the original length, not the possibly expanded version
					TRAPD(leaveCode, aTextObj.ApplyExtendedCharFormatL(format, varies, startNewTag, lengthNewTag));
					if (leaveCode==KErrNone)
						foundSomething = ETrue;		// We are applying a tag
					if (aLength)
						{
						if (localStartTag < aStartOfTags)
							{
							aLength += aStartOfTags - localStartTag;
							aStartOfTags = localStartTag;
							}
						if (localStartTag + localTagLength > aStartOfTags + aLength)
							aLength = localStartTag + localTagLength - aStartOfTags;
						}
					else
						{
						aStartOfTags = localStartTag;
						aLength = localTagLength;
						}
					}
				// Jump over
				localScanLength -= startNewTag + lengthNewTag - localStartScan;
				localStartScan = startNewTag + lengthNewTag;	// Adjust start of next scan run
				if (localStartScan >= endRange)	// Have we reached the end of the range yet?
					break;
				allowBack = EFalse;
				}
			}
		}

	return foundSomething;
	}


// given a tag, fetch a ptr to the parser - or null
MParser* CParserList::ParserWithThisTag(const TUint aTagIndex) const
	{
	MParser* parser = NULL;
	for (TInt count = 0; count < iNumberInList; count++)
		{
		if (aTagIndex == iParserList[count]->TagIndex())
			{
			parser = iParserList[count]->Parser();
			break;
			}
		}
	return parser;
	}

// given a ptr to a parser, fetch a tag - or zero
TUint CParserList::TagForThisParser(const MParser *const aParser) const
	{
	TUint tagIndex = 0;
	for (TInt count = 0; count < iNumberInList; count++)
		{
		if (aParser == iParserList[count]->Parser())
			{
			tagIndex = iParserList[count]->TagIndex();
			break;
			}
		}
	if (!tagIndex)
	    {
	    OstTrace0( TRACE_DUMP, CPARSERLIST_TAGFORTHISPARSER, "EParserNoSuchTag" );
	    }
	__ASSERT_DEBUG(tagIndex, Panic(EParserNoSuchTag));
	return tagIndex;
	}


CParserData::CParserData(TInt aEndParse):
	iStartParse(0),
	iEndParse(aEndParse)
	{
	// Get parser data
	iActiveParserList = (CParserList*)Dll::Tls();
	iLastKnownCursor = -1;
	if (iActiveParserList)
		iActiveParserList->iRefCount++;
	}


CParserData::~CParserData()
	{
	if (iActiveParserList)
		{
		iActiveParserList->iRefCount--;
		if ((iActiveParserList->iRefCount == 0) && (iActiveParserList->iNumberInList == 0))
			{
			Dll::FreeTls();
			delete iActiveParserList;
			}
		}
	}


// Merge the specified range, which may have changed length, into the current range.
// aOldLength is the # of chars deleted and to be removed from the range
// aNewLength is the # of chars inserted and to be added to the range
void CParserData::MergeRange(TInt aStart,TInt aOldLength,TInt aNewLength)
	{
	if (iStartParse == -1) // no current range
		{
		iStartParse = aStart;
		iEndParse = aStart + aNewLength;
		}
	else
		{
		if (aStart < iStartParse)
			iStartParse = aStart;
		if (aStart + aOldLength > iEndParse)
			iEndParse = aStart + aOldLength;
		iEndParse += aNewLength - aOldLength;
		}
	}
