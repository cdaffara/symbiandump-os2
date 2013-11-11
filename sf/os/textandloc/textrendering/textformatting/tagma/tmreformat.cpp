/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <tagma.h>
#include <bidi.h>
#include "tmreformat.h"

/**
Create a new reformat object, leaving it on the cleanup stack.

@param aTextLayout The text layout object that is looking after the document that needs reformatting. 
We use this to query the existing formatted range.
@param aFormatParam Restrictions on the formatting operation
@param aReformatParam Information about the reformat operation: which section of the document has changed
*/
CTmReformat* CTmReformat::NewLC(CTmTextLayout& aTextLayout, const TTmFormatParamBase& aFormatParam, 
		const TTmReformatParam& aReformatParam)
	{
	CTmReformat* self = new(ELeave) CTmReformat(aTextLayout, aFormatParam, aReformatParam);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self; 
	}
	
CTmReformat::CTmReformat(CTmTextLayout& aTextLayout, const TTmFormatParamBase& aFormatParam, 
		const TTmReformatParam& aReformatParam)
		:iTextLayout(aTextLayout), iFormatParam(aFormatParam),
		iReformatParam(aReformatParam)
	{
	}

void CTmReformat::ConstructL()
	{
	iOldStartBDContext = new(ELeave) TBidirectionalContext;
	iOldEndBDContext = new(ELeave) TBidirectionalContext;
	iNewEndBDContext= new(ELeave) TBidirectionalContext;
	iNewCode = new(ELeave) CTmCode;
	}

CTmReformat::~CTmReformat()
	{
	delete iOldStartBDContext;
	delete iOldEndBDContext;
	delete iNewEndBDContext;
	delete iNewCode;
	}

/**
Generate the new bytecode to be spliced into the text layout's bytecode. 

First it will generate new code for the section of document containing the actual edit, plus a line
either side (with the exception of two special cases where it's explicitly told to reformat whole
paragraphs).

Then it will look at the resulting bytecode, and:
- if the edit has changed bidirectional state above the new code, it will extend the edit upwards
to the start of the paragraph containing it

- if the edit has changed line endings or bidirectional state below the new code, then it will extend 
the edit downwards to the end of the paragraph containing it

The exception to this is:

- if the current formatted band ends in the middle of a paragraph and the edit would extend
to the end of the current formatted band, we will abandon any attempt to generate new
bytecode, effectively telling the caller to delete everything from the start of the edit to the end
of the band. The reason for this is that otherwise we run the risk of generating partial lines.

Once this function has been called, the caller can get the new bytecode and information about the 
range it occupies and the range it replaces.
*/
void CTmReformat::ReformatL()
	{
	TBool formatToParagraphStart = EFalse;
	TBool formatToParagraphEnd = EFalse;
	if(iReformatParam.iParInvalid)
		{
		formatToParagraphEnd = ETrue;
		}
	if(iReformatParam.iParFormatChanged)
		{
		formatToParagraphStart = ETrue;
		formatToParagraphEnd = ETrue;
		}
		
	GetReformatRangeL(formatToParagraphStart, formatToParagraphEnd);
	GenerateNewCodeL(formatToParagraphStart, formatToParagraphEnd);
	
	TBool extendReformatRange = EFalse;
	if((!ReformatRangeExtendsToParagraphStart()) && (BidirectionalityChangedAtStart()))
		{
		formatToParagraphStart = ETrue;
		extendReformatRange = ETrue;
		}
	if((!ReformatRangeExtendsToParagraphEnd()) && 
		(LineEndingsChangedAtEnd() || (BidirectionalityChangedAtEnd())))
		{
		formatToParagraphEnd = ETrue;
		extendReformatRange = ETrue;
		}
	if(BidirectionalityChangedAtStart() || BidirectionalityChangedAtEnd())
		{
		iBidirectionalStateChanged = ETrue;
		}
	if (extendReformatRange)
		{
		GetReformatRangeL(formatToParagraphStart, formatToParagraphEnd);
		GenerateNewCodeL(formatToParagraphStart, formatToParagraphEnd);
		}
	}

/**
Returns the bytecode we generated. This must be spliced into CTmTextLayout's current bytecode.
*/
CTmCode& CTmReformat::NewCode()
	{
	return *iNewCode;
	}
	
/**
This structure gives the caller information about the range to be replaced by the section of bytecode
we just generated. 

For example, if we have a document and delete a section of it, this function will give us information
about the position and dimensions of the deleted section.
*/
const CTmFormatContext::TInfo& CTmReformat::FormatInfo() const
	{
	return iFormatInfo;
	}

/**
This structure gives the caller information about the section of bytecode we just generated. 

For example, if we have a document and delete a section of it, this function will give us information
about the position of the deleted section. If we inserted some text, it will also tells us the 
dimensions of the new text.
*/
const TTmByteCodeFinder::TInfo& CTmReformat::ReformatRange() const
	{
	return iReformatRange;
	}

/**
This function uses iReformatParam to get information about the section of the document to reformat.
Its operation is also parameterised by aToParagraphStart and aToParagraphEnd, which tell it whether 
to extend the range it retrieves to the start and/or end of the paragraph containing the start/end 
of the edit.
*/
void CTmReformat::GetReformatRangeL(TBool aToParagraphStart, TBool aToParagraphEnd)
	{
//by default, the range is just the stuff that's been edited
	TInt startChar = iReformatParam.iStartChar;
	TInt endChar = iReformatParam.iStartChar + iReformatParam.iOldLength;
//get the range into iReformatRange
	TTmInterpreterParam interpreterParam(iTextLayout);
	TTmByteCodeFinder finder(interpreterParam, startChar, endChar);
	TInt max_extra_lines = aToParagraphEnd ? iReformatParam.iMaxExtraLines : KMaxTInt;
	if (!finder.FindByteCode(aToParagraphStart, aToParagraphEnd, max_extra_lines, iReformatRange, iOldStartBDContext, iOldEndBDContext))
		{
		TmPanic(EFormatNotFound);	
		}
	}
	
void CTmReformat::GenerateNewCodeL(TBool aToParagraphStart, TBool aToParagraphEnd)
	{
	TTmFormatParam formatParam(iFormatParam);
	formatParam.iStartChar = iReformatRange.iStartInfo.iStart;
	formatParam.iEndChar = iReformatRange.iEndInfo.iEnd + iReformatParam.iNewLength - iReformatParam.iOldLength;
	formatParam.iLineInPar = iReformatRange.iStartInfo.iLineInPar;
	iNewCode->Reset();
/*
If we were asked to format to the end of the paragraph,
but the end of the reformat range does not actually end on a paragraph boundary, 
then we should abandon the reformat, effectively marking all the text after the 
edit start as invalid, and regenerating the bytecode afresh from the document source. 

This is because otherwise, under such conditions, we we risk formatting a partial
line at the end of the range.

To do this we:
- set iReformatRange (which represents the section of bytecode to be replaced) to extend 
from the start of the edit range to the end of the formatted range
- set iFormatInfo (which represents the section of bytecode with which to replace iReformatRange
to have an extent of zero

Then the calling code will replace iReformatRange with iFormatInfo, as usual, and thus will 
effectively delete everything after the start of the edit. Then it will add new sections of 
formatting from the document source to ensure we have enough to fill the visible band
*/
//set extent of iFormatInfo to zero
	iFormatInfo.iWidth = 0;
	iFormatInfo.iHeight = 0;
	iFormatInfo.iFirstLineEndChar = iReformatRange.iStartInfo.iStart;
	iFormatInfo.iLastLineStartChar = iReformatRange.iStartInfo.iStart;
	iFormatInfo.iEndChar = iReformatRange.iStartInfo.iStart;
// Set the context character  for this line from the previous line found in the byte code.
// This is then passed on the the CTmFormatContext::FormatL() function to be stored in the CTmTextLayout object.
// Every new line that is about to be formatted/reformatted takes the context from CTmTextLayout.
	iFormatInfo.iContextCharPerLine = iReformatRange.iContextCharPerLine;
//set the new BD state at end == the BD state at the start of the old reformat range
	*iNewEndBDContext = *iOldStartBDContext;
	if ((aToParagraphEnd || (iReformatRange.iEndInfo.iEnd == iTextLayout.EndChar()))
		&& (!ReformatRangeExtendsToParagraphEnd()))
		{
//set iReformatRange to extend from edit start to end of formatted band
		TTmInterpreterParam interpreterParam(iTextLayout);
		TTmByteCodeFinder finder(interpreterParam, iReformatParam.iStartChar, iTextLayout.EndChar()-1);
		if (!finder.FindByteCode(aToParagraphStart, EFalse, KMaxTInt, iReformatRange, NULL, NULL))
			{
			TmPanic(EFormatNotFound);	
			}			
		}
	else
		{
		CTmFormatContext::FormatL(*(iTextLayout.Source()), formatParam, *iNewCode, iFormatInfo,&iTextLayout, 
		iOldStartBDContext, iNewEndBDContext, &iEndOfPreviousLine);	
		//if we actually did format to the end of a paragraph, 
		//the new bidirectional state is the default
		if(ReformatRangeExtendsToParagraphEnd())
			{
			iNewEndBDContext->Reset();
			}	
		}
	}

TBool CTmReformat::BidirectionalStateChanged()
	{
	return iBidirectionalStateChanged;
	}

/**
Tells us whether the edit changed the bidirectional state before the edit point.
*/
TBool CTmReformat::BidirectionalityChangedAtStart()
	{
	return !(iOldStartBDContext->ContextMatches(iEndOfPreviousLine));
	}

/**
Returns the bidirectional state at the end of the newly generated bytecode
*/
const TBidirectionalContext& CTmReformat::BidirectionalStateAtEnd() const
	{
	return *iNewEndBDContext;
	}

/**
Tells us whether the edit changed the bidirectional state after the edit point.
*/	
TBool CTmReformat::BidirectionalityChangedAtEnd()
	{ 
	return !(*(iNewEndBDContext) == *(iOldEndBDContext));
	}

/**
Tells us whether the start of the range to replace is on a paragraph boundary.
*/	
TBool CTmReformat::ReformatRangeExtendsToParagraphStart() const
	{
	return iReformatRange.iStartInfo.iFlags & TTmLineInfo::EParStart;
	}
	
/**
Tells us whether the end of the range to replace is on a paragraph boundary.
*/		
TBool CTmReformat::ReformatRangeExtendsToParagraphEnd() const
	{
	return iReformatRange.iEndInfo.iFlags & TTmLineInfo::EParEnd;
	}	

/**
Tells us whether line endings after the edit point have changed.

The algorithm here is a bit non-obvious. It is:
- take the first character position of the last line in the reformatted range, before the reformat
	-plus the difference between the new size and the old size
-compare to the new character position of the last line in the range

For example, suppose this is the contents of the band:

line 1: 	t  h  s  i  s
position:   1  2  3  4  5

line 2:		a  t  e  s  t
position:   6  7  8  9  10

-line 2 start position == 6

-now insert an "i" at position 3

-suppose this does not change the format of line 2:

line 1: 	t  h  i  s  i  s
position:   1  2  3  4  5  6

line 2:		a  t  e  s  t
position:   7  8  9  10 11

-line 2 new start position ==7

(old line2 start) + ((new length)-(old length)) = 6 + (1-0) = 7 = new start position

-suppose it does change the format of line 2, by pushing the last char off the end of line 1:

line 1: 	t  h  i  s  i
position:   1  2  3  4  5

line 2:		s  a  t  e  s  t
position:   6  7  8  9  10 11

-line 2 new start position ==6

(old line2 start) + ((new length)-(old length)) = 6 + (1-0) = 7 != new start position
*/
TBool CTmReformat::LineEndingsChangedAtEnd() const
	{
	TInt lastLineStartCharBeforeEdit =  
		iReformatRange.iEndInfo.iStart + iReformatParam.iNewLength - iReformatParam.iOldLength;
	return lastLineStartCharBeforeEdit != iFormatInfo.iLastLineStartChar;	
	}
	
