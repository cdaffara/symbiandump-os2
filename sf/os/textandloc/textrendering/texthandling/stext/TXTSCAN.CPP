/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TXTETEXT.H"
#include "TXTSTD.H"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTSCANTraces.h"
#endif



EXPORT_C TInt CPlainText::CharPosOfParagraph(TInt& aLength,TInt aParaOffset)const
/** Finds the length and the start position of a paragraph identified by its 
paragraph number the first paragraph is numbered zero.

Notes:

if aParaOffset is invalid, (equal to or greater than the total number of 
paragraphs), the function's return value is EScanEndOfData

@param aLength On return contains the length of the specified paragraph. 
@param aParaOffset The paragraph number. The first paragraph is numbered zero. 
@return The document position of the first character in the paragraph. */
	{
	__TEST_INVARIANT;

	TInt startPos=aLength=0;
	TUint scanMask=(EScanToUnitStart);
	aLength=ScanParas(startPos,scanMask);
	for (TInt offset=1;offset<=aParaOffset;offset++)
		{
		if (startPos<=EScanEndOfData)
			return EScanEndOfData;
		aLength=ScanParas(startPos,scanMask);
		}
	if (startPos==EScanEndOfData)
		startPos=iByteStore->Size()/sizeof(TText);
	return startPos-aLength;
	}

EXPORT_C TInt CPlainText::ParagraphNumberForPos(TInt& aPos)const
/** Gets the number of the paragraph which contains a document position. 
Paragraph numbering begins at zero.

@param aPos A document position. Must be valid or a panic occurs. On return, 
contains the document position of the first character in the paragraph in 
which it is located. 
@return The number of the paragraph containing the specified document position.
The first paragraph is numbered zero. */
	{
	__TEST_INVARIANT;

	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_PARAGRAPHNUMBERFORPOS, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));
	TUint scanMask=(EScanBackwards|EScanStayIfBoundary|EScanToUnitStart);
	ScanParas(aPos,scanMask);
	// aPos now holds the 1st char of the paragraph.
	TInt paraOffset=0,currentPos=0;
	scanMask=EScanToUnitStart;
	while (currentPos<aPos)
		{// Move forwards, counting paragraphs until we get to the current position.
		paraOffset++;
		ScanParas(currentPos,scanMask);
		}
	return paraOffset;
	}

EXPORT_C void CPlainText::GetWordInfo(TInt aCurrentPos,TInt& aStartPos,TInt& aLength,
									  TBool aPictureIsDelimiter,TBool aPunctuationIsDelimiter)const
/** Gets the start position and length of the word that contains the document 
position specified.

@param aCurrentPos A document position. Must be valid or a panic occurs. 
@param aStartPos On return, the document position of the first character in 
the word containing the position aCurrentPos.
@param aLength On return, the length of the word containing document position 
aCurrentPos.
@param aPictureIsDelimiter ETrue if picture characters should be treated as 
word delimiters, false if not. For example, when navigating text, this might 
be EFalse, but when spell checking, it might be ETrue. 
@param aPunctuationIsDelimiter ETrue if punctuation characters should be treated 
as word delimiters, EFalse if not. */
	{
	__TEST_INVARIANT;

	if (aCurrentPos<0 || aCurrentPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_GETWORDINFO, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aCurrentPos>=0 && aCurrentPos<=DocumentLength(),Panic(ECharPosBeyondDocument));
	aStartPos=aLength=aCurrentPos;
	// First find the start of the word.
	TUint scanMask=(EScanBackwards|
					EScanStayIfBoundary|
					EScanToUnitStart);
	if (aPictureIsDelimiter)
		scanMask|=EScanPictureIsDelimiter;
	if (aPunctuationIsDelimiter)
		scanMask|=EScanPunctuationIsDelimiter;
	ScanWords(aStartPos,scanMask);
	// Next find the end of the word.
	scanMask=(EScanStayIfBoundary|EScanToUnitEnd);
	if (aPictureIsDelimiter)
		scanMask|=EScanPictureIsDelimiter;
	if (aPunctuationIsDelimiter)
		scanMask|=EScanPunctuationIsDelimiter;
	ScanWords(aLength,scanMask);
	aLength-=aStartPos;
	if (aLength+aStartPos>(DocumentLength()+1))
	    {
	    OstTrace0( TRACE_DUMP, DUP2_CPLAINTEXT_GETWORDINFO, "ECharPosBeyondDocument" );
	    }
	__ASSERT_DEBUG(aLength+aStartPos<=(DocumentLength()+1),Panic(ECharPosBeyondDocument));

	__TEST_INVARIANT;
	}

EXPORT_C TInt CPlainText::ToParagraphStart(TInt& aPos) const
/** Gets the document position of the start of the paragraph containing the 
specified document position.

@param aPos A document position. Must be valid or a panic occurs. On return, 
contains the document position of the first character in the paragraph in 
which it is located.
@return The number of characters skipped in scanning to the start of the 
paragraph. */
	{
	__TEST_INVARIANT;
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_TOPARAGRAPHSTART, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));

	int skipped = 0;
	while (aPos > 0)
		{
		TPtrC8 p = iByteStore->BackPtr(aPos * sizeof(TText));
		const TText* start = (TText*)p.Ptr();
		const TText* end = start + p.Length() / sizeof(TText);
		while (end > start)
			{
			end--;
			if (*end == EParagraphDelimiter)
				return skipped;
			aPos--;
			skipped++;
			}
		}
	return skipped;
	}

EXPORT_C TInt CPlainText::WordCount()const
/** Gets a count of the number of words in the text object.

@return The number of words in the text object. */
	{return CountUnits(EUnitIsWord);}

EXPORT_C TInt CPlainText::ParagraphCount()const
/** Gets a count of the number of paragraphs in the text object.

Note: the paragraph delimiter which terminates every text object means this 
function always returns a count of at least one.

@return The number of paragraphs contained in the text object. */
	{//return CountUnits(EUnitIsParagraph);}
	TInt pos=0,unitCount=0;
	TUint scanMask=(EScanToUnitStart);
	while (pos!=EScanEndOfData)
		{
		unitCount++;
		ScanParas(pos,scanMask);
		}
	return unitCount;
	}

TInt CPlainText::CountUnits(TUnitOfText aContext)const
// Returns the number of units in the document, where
// units are defined by the parameter aContext.
//
	{
	SScanData scanData;
	TInt pos=0,unitCount=0;
	TUint scanMask=(EScanToUnitStart|EScanJoinDelimiters|EScanStayIfBoundary);
	InitScanControl(pos,scanMask,aContext,scanData);
	ScanUnit(pos,aContext,scanData);
	// The above code skips over leading white space
	scanData.scanMask &=~ EScanStayIfBoundary;
	while(pos!=EScanEndOfData)
		{
		unitCount++;
		ScanUnit(pos,aContext,scanData);
		}//	Count complete, so return pos to the beginning of the data
	pos=0;
	return unitCount;
	}




EXPORT_C TInt CPlainText::ScanWords(TInt& aPos,TUint& aScanMask)const
/** Scans from a document position to the beginning or end of a word. The 
destination is determined by a scan mask. The scan can either be forwards 
(the default) or backwards, and the destination may be the first or last 
character in the word containing the position, or the first character in 
the next word.

Note: If an attempt is made to scan beyond the end of text delimiter, on return, 
aPos is set to EScanEndOfData  and the function's return value indicates 
the number of characters skipped in passing the end of text delimiter.

@param aPos The document position from which to scan. Must be valid, or a 
panic occurs. On return, contains the new document position. 
@param aScanMask The scan mask. See the enumeration whose values begin with 
CPlainText::EScanBackwards. 
@return The number of characters skipped to reach the new document position. */
	{
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_SCANWORDS, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));
	SScanData scanData;
	InitScanControl(aPos,aScanMask,EUnitIsWord,scanData);
	return ScanUnit(aPos,EUnitIsWord,scanData);
	}

EXPORT_C TInt CPlainText::ScanParas(TInt& aPos,TUint& aScanMask)const
/** Scans from a document position to the beginning or end of a paragraph. The 
destination is determined by a scan mask. The scan can either be forwards 
(the default) or backwards, and the destination may be the first or last 
character in the paragraph containing the position, or the first character in 
the next paragraph. 

Note: if an attempt is made to scan beyond the end of text delimiter, on return, 
aPos is set to EScanEndOfData ) and the function's return value indicates 
the number of characters skipped in passing the end of text delimiter.

@param aPos The document position to scan from. Must be valid or a panic occurs. 
On return, contains the new document position. 
@param aScanMask The scan mask. See the enumeration whose values begin with 
CPlainText::EScanBackwards. 
@return The number of characters skipped to reach the new document position. */
	{
	if (aPos<0 || aPos>DocumentLength())
	    {
	    OstTrace0( TRACE_FATAL, CPLAINTEXT_SCANPARAS, "ECharPosBeyondDocument" );
	    }
	__ASSERT_ALWAYS(aPos>=0 && aPos<=DocumentLength(),Panic(ECharPosBeyondDocument));
	SScanData scanData;
	InitScanControl(aPos,aScanMask,EUnitIsParagraph,scanData);
	return ScanUnit(aPos,EUnitIsParagraph,scanData);
	}


void CPlainText::InitScanControl(TInt& aPos,TUint& aScanMask,TUnitOfText aContext,SScanData& aScanData)const
// Initialises the control segment of the bitmask
// used to determine when scan has completed.
// Calls the initialisation of aScanData.
//
	{
	aScanData.pos=aPos;
	aScanData.scanMask=0;
	aScanData.scanMask|=aScanMask;
	if (aScanData.scanMask & EScanStayIfBoundary)
		{//Scan one unit the other way first.
			aScanData.scanMask ^= EScanBackwards;  // Alter scan direction.
			InitScanData(aScanData.pos,aScanData);				
			TestForDelimiter(aContext,aScanData);
			aScanData.scanMask ^= EScanBackwards;  // Restore original scan direction.
		}
	InitScanData(aScanData.pos,aScanData);
	if(!aPos && (aScanData.scanMask&(EScanStayIfBoundary|EScanBackwards))==EScanStayIfBoundary)
		{//At the start of the buffer and scanning forwards and ScanStayIfBoundary set.
		aScanData.scanMask &=~aScanData.EInsideUnit;
		}
	else 
		{
		TestForDelimiter(aContext,aScanData);
		if (aScanData.scanMask & aScanData.EIsDelimiter)
			aScanData.scanMask &=~ aScanData.EInsideUnit;
		else
 			aScanData.scanMask |= aScanData.EInsideUnit;
		}
	if (aScanData.scanMask & EScanBackwards)
		{//Set stop flags relative to scan direction
		if (aScanData.scanMask &  EScanToUnitStart)
			aScanData.scanMask |= aScanData.EStopEnd;
		if (aScanData.scanMask &  EScanToUnitEnd)
			aScanData.scanMask |= aScanData.EStopBegin;
		}
	else
		{//Set stop flags relative to scan direction
		if (aScanData.scanMask &  EScanToUnitStart)
			aScanData.scanMask |= aScanData.EStopBegin;
		if (aScanData.scanMask &  EScanToUnitEnd)
			aScanData.scanMask |= aScanData.EStopEnd;
		}
	}


void CPlainText::InitScanData(TInt aPos,SScanData& aScanData)const
// Initialises the necessary elements of the scan structure, for ScanXxx 
// methods.  These are used to track scan progress.
//
	{
	aScanData.pos=aPos;
	aScanData.oldPos=aPos;
	aScanData.currentSegLen=0;
	if (aScanData.scanMask & EScanBackwards)
		{//Scanning backwards.
		aScanData.delta=-1;
		aScanData.totalBufLen=aPos;
		}
	else
		{//Scanning forwards.
		aScanData.delta=+1;
		aScanData.totalBufLen=((iByteStore->Size()/sizeof(TText))-aScanData.pos);
		}
	}	


TInt CPlainText::ScanUnit(TInt& aPos,TUnitOfText aContext,SScanData& aScanData)const
// Scan from position aPos, by one unit.
// Update aPos to the current position, and return the number
// of characters skipped or EScanEndOfData if at the end of the buffer.
//
	{
	while (TestForDelimiter(aContext,aScanData))
		{//More data has been read
		if (aScanData.scanMask & aScanData.EInsideUnit)
			{
			if (aScanData.scanMask & aScanData.EIsDelimiter)
				{
				aScanData.scanMask &=~aScanData.EInsideUnit;
				if (aScanData.scanMask & aScanData.EStopEnd)
					break;
				}
			 }
		else
			{
			if (aScanData.scanMask & aScanData.EIsDelimiter)
				{
				if (!(aScanData.scanMask & EScanJoinDelimiters))
					break;
				}
	   		else
				{
				aScanData.scanMask |= aScanData.EInsideUnit;
				if (aScanData.scanMask & aScanData.EStopBegin)
					break;
				}
			}
		}
	TInt charsSkipped=(aScanData.scanMask & EScanBackwards)? aPos-aScanData.oldPos : 
										aScanData.oldPos-aPos;
	aPos=(aScanData.oldPos<(TInt)(iByteStore->Size()/sizeof(TText)))? aScanData.oldPos : EScanEndOfData;
	return charsSkipped;
	}


TBool CPlainText::TestForDelimiter(TUnitOfText aContext,SScanData& aScanData) const
/** Return ETrue if a character was read successfully.
Set EIsDelimiter flag if the character read
is a delimiter of the appropriate type.*/	
    {
	aScanData.scanMask |= aScanData.EIsDelimiter;
	aScanData.oldPos = aScanData.pos;
	TChar c;
	if (!GetChar(aScanData,c))
		return FALSE;

	TBool found = FALSE;
	if (aContext == EUnitIsParagraph)
		found = c == EParagraphDelimiter;
	else if (aContext == EUnitIsWord)
		{
		if (c == EParagraphDelimiter ||
			c == ELineBreak ||
			c == EPageBreak ||
			c == ESpace ||
			c == ETabCharacter)
			found = TRUE;
		if (!found && (aScanData.scanMask & EScanPictureIsDelimiter))
			{
			if (c == EPictureCharacter)
				found = TRUE;
			}
		if (!found)
			{
			TChar::TCategory cat = c.GetCategory();
			if (cat == TChar::EZsCategory) // it's a space
				found = TRUE;
			if (!found &&
				(aScanData.scanMask & EScanPunctuationIsDelimiter) &&
				(cat & TChar::EPunctuationGroup))
				{
				if (c != EHyphenMinus &&
					c != EApostrophe &&
					c != EPotentialHyphen &&
					c != EHyphen && 
					c != ERightSingleQuote)
					found = TRUE;
				}
			}
		}

	if (found)
		aScanData.scanMask |= aScanData.EIsDelimiter;
	else
		aScanData.scanMask &= ~aScanData.EIsDelimiter;
	return TRUE;
	}


TBool CPlainText::GetChar(SScanData& aScanData,TChar& aChar)const
// If there is one, write the next character to aChar, and return ETrue,
// otherwise return EFalse.
//
	{
	TPtrC view(_S("a"));  // Dummy value cos no default constructor.
	if (!aScanData.currentSegLen)	
		{//First time in, or new segment.
		if (!aScanData.totalBufLen)
			{//No data left to read.
			return EFalse;	
			}
		if (aScanData.delta < 0)
			{//Scanning backwards
			TPtrC8 tempView=iByteStore->BackPtr(aScanData.pos*sizeof(TText));
			view.Set((TText*)tempView.Ptr(),tempView.Length()/sizeof(TText));
			}
		else
			{//Scanning forwards
			TPtrC8 tempView=iByteStore->Ptr(aScanData.pos*sizeof(TText));
			view.Set((TText*)tempView.Ptr(),tempView.Length()/sizeof(TText));
			}//Ptr now returns a TDes.
		aScanData.currentSegLen=(TInt)Min(aScanData.totalBufLen,(TInt)view.Length());
		aScanData.totalBufLen-=aScanData.currentSegLen;
		aScanData.buf=(TText*)view.Ptr();
		if (aScanData.delta < 0)
			aScanData.buf+=aScanData.currentSegLen-1;
		}
	aChar=*(aScanData.buf);  // aChar takes the character just read.
	aScanData.buf+=aScanData.delta;  // pText increments correctly for both ASCII and Unicode
	aScanData.pos+=aScanData.delta;
	aScanData.currentSegLen-=1;  // len holds the number of chars left, for ASCII and Unicode
	return ETrue;
	}


