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
* The CTmTextLayout class: TAGMA's simplest text class. It holds the layout of rectangular piece of text.
*
*/


#include "TmLayoutImp.h"
#include "TmHighlightSource.h"
#include "TMINTERP.H"
#include "TmChunkInterp.h"
#include <txtfrmat.h>
#include "TmTextDrawExt.h"
#include <frmtlay.h>
#include "tmreformat.h"
#include "frmUtils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TMLAYOUTTraces.h"
#endif


#define UNUSED_VAR(a) a = a

/**
Construct a TTmFormatParamBase object. Set iWrapWidth, iMaxHeight and iMaxLines to 'infinity' (KMaxTInt),
iFlags to 0 (wrapping off, no truncation with ellipsis, etc.), and iEllipsis to the ordinary ellipsis character, U+2026.
*/
EXPORT_C TTmFormatParamBase::TTmFormatParamBase():
	iWrapWidth(KMaxTInt),
	iMaxHeight(KMaxTInt),
	iMaxLines(KMaxTInt),
	iFlags(0),
	iEllipsis(0x2026)
	{
	}

/**
Construct a TTmFormatParam object, setting iStartChar to 0, iEndChar to 'infinity' (KMaxTInt) and iLineInPar to 0.
*/
EXPORT_C TTmFormatParam::TTmFormatParam():
	iStartChar(0),
	iEndChar(KMaxTInt),
	iLineInPar(0)
	{
	}

/**
Construct a TTmFormatParam object with its base class part (TTmFormatParamBase) set to aBase, and setting iStartChar to 0,
iEndChar to 'infinity' (KMaxTInt) and iLineInPar to 0.
*/
EXPORT_C TTmFormatParam::TTmFormatParam(const TTmFormatParamBase& aBase):
	TTmFormatParamBase(aBase),
	iStartChar(0),
	iEndChar(KMaxTInt),
	iLineInPar(0)
	{
	}

/**
Construct a TTmReformatParam object, setting everything to 0 or FALSE apart from iMaxExtraLines, which is
set to 'infinity' (KMaxTInt).
*/
EXPORT_C TTmReformatParam::TTmReformatParam():
	iStartChar(0), 
	iOldLength(0),
	iNewLength(0),
	iMaxExtraLines(KMaxTInt),
	iParFormatChanged(FALSE),
	iParInvalid(FALSE)
	{
	}

/**
Construct a TTmReformatResult object, setting iHeightChange to 0 and iUnchangedTop and iUnformattedStart to
'infinity' (KMaxTInt).
*/
EXPORT_C TTmReformatResult::TTmReformatResult():
	iHeightChange(0),
	iUnchangedTop(KMaxTInt),
	iUnformattedStart(KMaxTInt)
	{
	}

/**
Construct an empty object with no text and zero width and height.
It is legal to call all CTmTextLayout functions on an empty object,
with the exception of AddParL and FormatL, which panic with ENoSource.
*/
EXPORT_C CTmTextLayout::CTmTextLayout() :
	iCurrentContextChar(NULL)
	{
	// do nothing
	}

/**
The destructor.
*/
EXPORT_C CTmTextLayout::~CTmTextLayout()
	{
	delete iBdStateAtEnd;
	}

/**
Set the text, discarding any existing layout information. The text from aParam.iStartChar to aParam.iEndChar is
extracted from aSource and formatted.
*/
EXPORT_C void CTmTextLayout::SetTextL(MTmSource& aSource,const TTmFormatParam& aParam)
	{
	if (aParam.iMaxHeight <= 0 ||
		aParam.iStartChar < 0 || aParam.iEndChar < aParam.iStartChar ||
		aParam.iLineInPar < 0)
		TmPanic(EBadArg);
		
	if (iBdStateAtEnd)
		{
		iBdStateAtEnd->Reset();
		}
	else
		{
		iBdStateAtEnd = new(ELeave) TBidirectionalContext;
		}

		
	iSource = &aSource;
	iCode.Reset();
	iStartChar = aParam.iStartChar;
	CTmFormatContext::TInfo info;
	if (aParam.iFlags & TTmFormatParamBase::EWrap
		&& (aParam.iWrapWidth < 1 || aParam.iMaxHeight < 1))
		{
		// Illegal parameters, so we should not waste time formatting.
		// We will assume that good parameters will be along later.
		iHeight = 0;
		iEndChar = iStartChar;
		iWidth = 0;
		}
	else
		{
		//Get line context character from previous line if there is
		TTmDocPosSpec startDocPos(aParam.iStartChar - 1, static_cast<TTmDocPosSpec::TType>(1));
		TTmInterpreter interpreter(TTmInterpreterParam(*this));
		TBool lineFound = interpreter.DocPosToLine(startDocPos);
		if (lineFound)
			info.iContextCharPerLine = interpreter.LineContextCharChar();
		else
			info.iContextCharPerLine = NULL;
		
		CTmFormatContext::FormatL(aSource,aParam,iCode,info,this,NULL,iBdStateAtEnd, NULL);
		iHeight = info.iHeight;
		iEndChar = info.iEndChar;
		iWidth = 0;
		AdjustWidth(aParam,info.iWidth);
		}
	}

/**
Format part of the text that has changed and indicate whether further formatting needs to be done.

The backing text must remain constant during background formatting. What this means is that
if background formatting is enabled (TTmReformatParam::iMaxExtraLines is some small number, causing
formatting to end mid-way with TTmReformatResult::iUnformattedStart less than KMaxTInt), so that text
can be formatted bit by bit by a higher-level object, as is done by CTextView, the backing text must not
change between the series of calls to this function that make up a single reformatting act.
*/
EXPORT_C void CTmTextLayout::FormatL(const TTmFormatParamBase& aParam,const TTmReformatParam& aReformatParam,
									 TTmReformatResult& aResult)
	{
	if (!iSource)
		TmPanic(ENoSource);

	CTmReformat* reformat = CTmReformat::NewLC(*this, aParam, aReformatParam);
	reformat->ReformatL();
	CTmCode* new_code = &(reformat->NewCode());
	TTmByteCodeFinder::TInfo oldCodeInfo = reformat->ReformatRange();
	CTmFormatContext::TInfo newCodeInfo = reformat->FormatInfo();
	TBool bd_changed = reformat->BidirectionalStateChanged();
	/*
	Find the lines that have changed and need to be redisplayed.
	If aParagraphFormatChanged is true at least one paragraph format has changed,
	so the redraw rectangle includes the whole of the paragraphs affected.

	Also redraw the whole paragraph if background formatting is needed, or if the bidirectional state has changed.
	*/
	if (aReformatParam.iParFormatChanged || aResult.iUnformattedStart < KMaxTInt || bd_changed)
		{
		aResult.iRedrawRect = oldCodeInfo.iBounds;
		aResult.iRedrawRect.iBr.iY = aResult.iRedrawRect.iTl.iY + newCodeInfo.iHeight;
		}
	else
		{
		TTmInterpreterParam p(*this);
		p.iCodeStart = oldCodeInfo.iStartCodePos;
		p.iCodeEnd = oldCodeInfo.iEndCodePos;
		p.iTextStart = oldCodeInfo.iStartInfo.iStart;
		TTmInterpreter old_code_interpreter(p);
		p.iByteCode = new_code;
		p.iCodeStart = 0;
		p.iCodeEnd = new_code->Size();
		TTmInterpreter new_code_interpreter(p);
		old_code_interpreter.CalculateRedrawRect(new_code_interpreter,aReformatParam.iStartChar,
												 aReformatParam.iOldLength,aReformatParam.iNewLength,aResult.iRedrawRect);
		aResult.iRedrawRect.Move(0,oldCodeInfo.iBounds.iTl.iY);
		}

	// Delete the old bytecode and insert the new.
	iCode.ChangeL(oldCodeInfo.iStartCodePos,oldCodeInfo.iEndCodePos,*new_code);

	// Adjust the band's bounds.
	iHeight += newCodeInfo.iHeight - oldCodeInfo.iBounds.Height();
	
	//if the range we are replacing includes the end of the current formatted range,
	//then the end BD state pointer may be invalidated, so we need to update it
	if(oldCodeInfo.iEndInfo.iEnd == iEndChar)
		{
		*iBdStateAtEnd = reformat->BidirectionalStateAtEnd();
		}
	// Adjust the band's width if necessary.
	AdjustWidth(aParam,newCodeInfo.iWidth);

	// Adjust the band's length.
	TInt lengthOfOldRange = oldCodeInfo.iEndInfo.iEnd - oldCodeInfo.iStartInfo.iStart;
	TInt lengthOfNewRange = newCodeInfo.iEndChar - oldCodeInfo.iStartInfo.iStart;
	iEndChar += lengthOfNewRange - lengthOfOldRange;
	// Determine the change in height of the reformatted text.
	aResult.iHeightChange = newCodeInfo.iHeight - oldCodeInfo.iBounds.Height();

	/*
	Set the top of the unchanged text, and if not all text has been formatted
	remove the first unformatted line from the redraw rect.
	*/
	if (aResult.iUnformattedStart < KMaxTInt)
		{
		TTmLineInfo info;
		TTmDocPosSpec pos(aResult.iUnformattedStart,TTmDocPosSpec::ETrailing);
		DocPosToLine(pos,info);
		aResult.iRedrawRect.iBr.iY = info.iOuterRect.iTl.iY;
		ParNumberToLine(info.iParNumber,KMaxTInt,info);
		aResult.iUnchangedTop = info.iOuterRect.iBr.iY - aResult.iHeightChange;
		}
	else
		aResult.iUnchangedTop = aResult.iRedrawRect.iBr.iY - aResult.iHeightChange;

	CleanupStack::PopAndDestroy(reformat);
	
	Invariant();
	}

/**
Format a new paragraph immediately before or after the band and add it to the formatted text.
If the text starts or ends in the middle of the paragraph, complete the paragraph.
Return TRUE if text was successfully added, or FALSE if there is no more text to add, or
the new text would have exceeded the maximum height allowed. Place the height of the new text in aHeightIncrease.
Place the increase in the number of complete paragraphs (0 or 1) in aParagraphsIncrease.

Note that this function will not necessarily add a whole paragraph to the band: if the client has restricted
the amount of formatting to add by setting the relevant values in aParam, then these restrictions will
be respected with the result that less than a paragraph of formatting may be generated.

This is the case, for example, when CTextLayout adds text below the band. 
*/
EXPORT_C TBool CTmTextLayout::AddParL(const TTmFormatParamBase& aParam,TBool aAtStart,TInt& aHeightIncrease,
									  TInt& aParagraphsIncrease)
	{	
	if (!iSource)
		TmPanic(ENoSource);

	aHeightIncrease = 0;
	aParagraphsIncrease = 0;
	int start_char = 0;
	int end_char = 0;
	int deleted_height = 0;
	TTmFormatParam format_param(aParam);

	TBidirectionalContext* pStartBd = NULL;
	TBidirectionalContext* pEndBd = NULL;
	TBidirectionalEndOfLineContext* pEOLContext = NULL;		
	
	if (aAtStart)
		{
		if (iStartChar == 0)
			return FALSE;
		start_char = iSource->ParagraphStart(iStartChar - 1);
		end_char = iSource->ParagraphEnd(iStartChar - 1);
		if (end_char > iStartChar)
			{
			if (end_char > iEndChar)
				end_char = iEndChar;
			DeletePar(aParam,TRUE,KMaxTInt,deleted_height);
			}
		else
			aParagraphsIncrease = 1;
		}
	else
		{
		if (iEndChar > iSource->DocumentLength())
			return FALSE;
		start_char = iEndChar;
		end_char = iSource->ParagraphEnd(iEndChar);
		
		pStartBd = iBdStateAtEnd;
		pEndBd = iBdStateAtEnd;
			
		TInt lastLineNumber = Lines()-1;
		if(lastLineNumber >= 0)
			{
			TTmLineInfo lastLine;
			TBool found = LineNumberToLine(lastLineNumber, lastLine);
			ASSERT(found);
			if(!(lastLine.iFlags & TTmLineInfo::EParEnd))
				{
				format_param.iLineInPar = lastLine.iLineInPar+1;
				}
			}				
						
		aParagraphsIncrease = 1; 
		}

	CTmCode* new_code = new(ELeave) CTmCode;
	CleanupStack::PushL(new_code);
	format_param.iStartChar = start_char;
	format_param.iEndChar = end_char;
	int code_pos = aAtStart ? 0 : iCode.Size();
	CTmFormatContext::TInfo info;
	
	//Get line context character from previous line if there is
	TTmDocPosSpec startDocPos(format_param.iStartChar - 1, static_cast<TTmDocPosSpec::TType>(1));
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	TBool lineFound = interpreter.DocPosToLine(startDocPos);
	if (lineFound)
		info.iContextCharPerLine = interpreter.LineContextCharChar();
	else
		info.iContextCharPerLine = NULL;
	
	CTmFormatContext::FormatL(*iSource,format_param,*new_code,info,this,
		pStartBd, pEndBd, pEOLContext);

	// Insert the new code if any.
	aHeightIncrease = info.iHeight;
	if (aHeightIncrease > 0)
		{
		iCode.ChangeL(code_pos,code_pos,*new_code);

		iHeight += aHeightIncrease;
		if (aAtStart)
			iStartChar = start_char;
		else
			iEndChar = info.iEndChar;

		// Adjust the band's width if necessary.
		AdjustWidth(aParam,info.iWidth);
		}
	//if we did actually format to the paragraph end, reset the bd state
	if(!aAtStart)
		{
		if(iEndChar == format_param.iEndChar)
			{
			iBdStateAtEnd->Reset();
			}
		}
	CleanupStack::PopAndDestroy();	// new code
	aHeightIncrease -= deleted_height;
	
	Invariant();
	
	return aHeightIncrease > 0;
	}
	
TBool CTmTextLayout::LastLine(TTmLineInfo& aLine)
	{
	TInt lastLineNumber = Lines()-1;
	if(lastLineNumber >= 0)
		{
		TTmLineInfo lastLine;
		TBool found = LineNumberToLine(lastLineNumber, aLine);
		ASSERT(found);
		return ETrue;
		}
	return EFalse;	
	}

void CTmTextLayout::SetCurrentContextChar(TUint aContextChar)
	{
	iCurrentContextChar = aContextChar;
	}
	
TUint CTmTextLayout::GetCurrentContextChar() const
	{
	return iCurrentContextChar;
	}
/**
Extend the formatted range downwards. That is, starting at the end of the current 
formatted range, generate some new formatting and append it to the end of the range.
@param aParam 
This controls how much new formatting to generate. We will generate formatting up whichever comes first of:
- the character position specified in TTmFormatParam::iEndChar
- the height specified (in pixels) in TTmFormatParamBase::iMaxHeight
- the number of lines specified in TTmFormatParamBase::iMaxLines
We replace the values specified in TTmFormatParam::iStartChar and TmFormatParam::iLineInPar
@pre The object must have been initialised with an MTmSource, or it will panic.
*/
EXPORT_C void CTmTextLayout::ExtendFormattingDownwardsL(TTmFormatParam& aParam)
	{
	if (!iSource)
		TmPanic(ENoSource);

	aParam.iStartChar = iEndChar;

	if (iEndChar > iSource->DocumentLength())
		return;
	if (aParam.iStartChar > aParam.iEndChar)
		return;
		
	TBidirectionalContext* pStartBd = iBdStateAtEnd;
	TBidirectionalContext* pEndBd = iBdStateAtEnd;
	TBidirectionalEndOfLineContext* pEOLContext = NULL;		

	//if there is a last line (i.e. if there is any formatting at all)
	//and the last line is not a paragraph boundary, 
	//then initialise the line number				
	TTmLineInfo lastLine;
	TBool found = LastLine(lastLine);
	if(found && !(lastLine.iFlags & TTmLineInfo::EParEnd))
		{
		aParam.iLineInPar = lastLine.iLineInPar+1;
		}				
	
	//Generate the additional formatting
	CTmCode* newCode = new(ELeave) CTmCode;
	CleanupStack::PushL(newCode);

	TInt newCodePos = iCode.Size();
	CTmFormatContext::TInfo info;
	
	//Get line context character from previous line if there is
	TTmDocPosSpec startDocPos(aParam.iStartChar - 1, static_cast<TTmDocPosSpec::TType>(1));
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	TBool lineFound = interpreter.DocPosToLine(startDocPos);
	if (lineFound)
		info.iContextCharPerLine = interpreter.LineContextCharChar();
	else
		info.iContextCharPerLine = NULL;
	
	CTmFormatContext::FormatL(*iSource,aParam,*newCode,info,this,pStartBd, pEndBd, pEOLContext);

	// Insert the new code if any.
	if (info.iHeight > 0)
		{
		iCode.ChangeL(newCodePos,newCodePos,*newCode);
		iHeight += info.iHeight;
		iEndChar = info.iEndChar;
		AdjustWidth(aParam,info.iWidth);
		}
	//if we did actually format to the paragraph end, reset the bd state
	found = LastLine(lastLine);
	if(found && (lastLine.iFlags & TTmLineInfo::EParEnd))
		{
		iBdStateAtEnd->Reset();
		}

	CleanupStack::PopAndDestroy();	// new code
	Invariant();	
	}

/**
Delete the format for the first or last paragraph if its height does not exceed aMaxDeletedHeight.
Return TRUE if a paragraph was successfully deleted. Place the height of the deleted paragraph in aHeightDecrease.
*/
EXPORT_C TBool CTmTextLayout::DeletePar(const TTmFormatParamBase& aParam,TBool aAtStart,TInt aMaxDeletedHeight,
										TInt& aHeightDecrease)
	{
	aHeightDecrease = 0;
	if (!iSource || iStartChar >= iEndChar)
		return FALSE;

	int pos = aAtStart ? iStartChar : iEndChar - 1;
	TTmByteCodeFinder finder(TTmInterpreterParam(*this), pos, pos);
	TTmByteCodeFinder::TInfo info;
	if (!finder.FindByteCode(TRUE,TRUE,KMaxTInt,info))
		TmPanic(EFormatNotFound);
	if (info.iBounds.Height() > aMaxDeletedHeight)
		return FALSE;

	iCode.Delete(info.iStartCodePos,info.iEndCodePos - info.iStartCodePos);
	if (aAtStart)
		iStartChar = info.iEndInfo.iEnd;
	else
		iEndChar = info.iStartInfo.iStart;
	aHeightDecrease = info.iBounds.Height();
	iHeight -= aHeightDecrease;

	// Adjust the band's width if necessary.
	AdjustWidth(aParam,0);
	
	Invariant();
	
	return TRUE;
	}
	
/** 
Delete as many lines of formatting as possible from the end of the formatted range, up to 
the maximum height specified in aMaxDeletedHeight. The height decrease is placed in aHeightDecrease. 
*/
EXPORT_C void CTmTextLayout::DeleteFormattingFromEndL(const TTmFormatParamBase& aParam, TInt aMaxDeletedHeight, TInt& aHeightDecrease)
	{
	aHeightDecrease = 0;
	if (!iSource || iStartChar >= iEndChar)
		return;
	TBidirectionalContext* bdStart = new(ELeave) TBidirectionalContext;
	CleanupStack::PushL(bdStart);
	TBidirectionalContext* bdEnd = new(ELeave) TBidirectionalContext;
	CleanupStack::PushL(bdEnd);
	TTmByteCodeFinder::TInfo info;
	TTmByteCodeFinder sectionFinder(TTmInterpreterParam(*this), iStartChar, iStartChar);
	TBool found = sectionFinder.FindByteCodeAtEnd(info, aMaxDeletedHeight, iHeight, bdStart, bdEnd);
	if (!found)
		{
		CleanupStack::PopAndDestroy(2);
		}
	else
		{
		//should use an assignment operator here instead
		delete iBdStateAtEnd;
		iBdStateAtEnd = bdStart;
		CleanupStack::PopAndDestroy(bdEnd);
		CleanupStack::Pop(bdStart);
		iCode.Delete(info.iStartCodePos, info.iEndCodePos - info.iStartCodePos);
		iEndChar = info.iStartInfo.iStart;
		aHeightDecrease = info.iBounds.Height();
		iHeight -= aHeightDecrease;
		// Adjust the band's width if necessary.
		AdjustWidth(aParam,0);
		Invariant();
		}	
	}

/**
Reset the object to its empty state; no source or formatting, height and width of 0, start and end char both 0.
*/
EXPORT_C void CTmTextLayout::Clear()
	{
	if(iBdStateAtEnd)
		iBdStateAtEnd->Reset();
	iSource = NULL;
	iCode.Reset();
	iWidth = 0;
	iHeight = 0;
	iStartChar = 0;
	iEndChar = 0;

	Invariant();
	}

/**
Draw the text.

@param aGc
	Graphics context to draw to.
@param aTopLeft
	The origin of the formatted text in the coordinates of aGc.
@param aClipRect
	The clipping rectangle in the coordinates of aGc. Nowhere outside this
	rectangle will be drawn.
@param aDocBackground
	The default document background colour, which is used for regions not drawn
	by any custom background drawing routine and not covered by any text. If
	null, no background is drawn in these places.
@param aDrawParBackground
	If false no default background is drawn on those areas covered by text.
	Custom background drawers are invoked regardless of this argument.
*/

EXPORT_C void CTmTextLayout::DrawLayout(CGraphicsContext& aGc,
	const TPoint& aTopLeft,const TRect& aClipRect,
	const TLogicalRgb* aDocBackground,TBool aDrawParBackground) const
	{
	DrawLayout(aGc, aTopLeft, aClipRect, aDocBackground, aDrawParBackground, NULL, NULL);
	}

/**
Draw the text.

@param aGc
	Graphics context to draw to.
@param aTopLeft
	The origin of the formatted text in the coordinates of aGc.
@param aClipRect
	The clipping rectangle in the coordinates of aGc. Nowhere outside this
	rectangle will be drawn.
@param aDocBackground
	The default document background colour, which is used for regions not drawn
	by any custom background drawing routine and not covered by any text. If
	null, no background is drawn in these places.
@param aDrawParBackground
	If false no default background is drawn on those areas covered by text.
	Custom background drawers are invoked regardless of this argument.
@param aHighlight
	The current selection. If null, no selection is drawn.
@param aHighlightExtensions
	If not null, the selection highlight has its edges moved by the specified
	number of pixels
*/

EXPORT_C void CTmTextLayout::DrawLayout(CGraphicsContext& aGc,
	const TPoint& aTopLeft,const TRect& aClipRect,
	const TLogicalRgb* aDocBackground,TBool aDrawParBackground,
	const TCursorSelection* aHighlight,
	const TTmHighlightExtensions* aHighlightExtensions) const
	{
	if (iSource)
		{
		RTmDrawingInterpreter interpreter(*iSource, aGc,
			TTmInterpreterParam(*this), aTopLeft, aClipRect, aDocBackground,
			aDrawParBackground ? RTmDrawingInterpreter::EDrawParBackground : 0,
			aHighlight, aHighlightExtensions);
		interpreter.Draw();
		interpreter.Close();
		}
	}

/**
Draw the background, using the custom background graphics implemented by overriding MTmSource::DrawBackground if any:
aTopLeft is the top-left corner of the text, aRect is the portion of the background to be drawn, and aBackground is
the default background colour. This function is used by higher-level classes for drawing backgrounds outside the ordinary
text area consistently with the ordinary background.
*/
EXPORT_C void CTmTextLayout::DrawBackground(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
											const TLogicalRgb& aBackground) const
	{
	if (iSource)
		{
		RTmDrawingInterpreter interpreter(*iSource, aGc, TTmInterpreterParam(*this),
			aTopLeft, aClipRect, &aBackground, 0, NULL, NULL);
		interpreter.DrawBackground(aClipRect, aBackground);
		interpreter.Close();
		}
	}

EXPORT_C void CTmTextLayout::InvertLayout(CGraphicsContext& aGc,
	const TPoint& aTopLeft, TInt aStartDocPos,TInt aEndDocPos) const
	{
	TTmHighlightExtensions highlightProperties;
	highlightProperties.SetAll(0);
	InvertLayout(aGc,aTopLeft,aStartDocPos,aEndDocPos,highlightProperties,0,0);
	}

/**
Analogous to InvertLayout(CGraphicsContext, const TPoint&, TInt, TInt) but with
highlight extensions applied.
@see InvertLayout(CGraphicsContext, const TPoint&, TInt, TInt)
@publishedPartner
@released
*/
EXPORT_C void CTmTextLayout::InvertLayout(CGraphicsContext& aGc,
	const TPoint& aTopLeft, TInt aStartDocPos, TInt aEndDocPos,
	const TTmHighlightExtensions& aHighlightExtensions,
	TInt aHighlightStartDocPos, TInt aHighlightEndDocPos) const
	{
	// note: Assumes that RWindow::BeginRedraw() has been called by the caller.

	if (NULL == iSource)
		{
		return;
		}

	TRect clipRect;
	GetUpdateBoundingRect(aStartDocPos, aEndDocPos, aTopLeft, clipRect);

	if (aStartDocPos < aHighlightStartDocPos)
		{
		aHighlightStartDocPos = aStartDocPos;
		}
	
	if (aEndDocPos > aHighlightEndDocPos)
		{
		aHighlightEndDocPos = aEndDocPos;
		}

	TCursorSelection selection;
	selection.SetSelection(aHighlightStartDocPos, aHighlightEndDocPos);

	TRect boundingRect;
	GetUpdateBoundingRect(aStartDocPos, aEndDocPos, aTopLeft, boundingRect);
	aHighlightExtensions.AbsExtendRect(boundingRect);
	boundingRect.Intersection(clipRect);
	if (! boundingRect.IsEmpty())
		{
		RTmDrawingInterpreter interpreter(*iSource, aGc, TTmInterpreterParam(*this),
			aTopLeft, boundingRect, NULL, ETrue, &selection, &aHighlightExtensions, ETrue);
		interpreter.Draw();
		interpreter.Close();
		}
	}

/**
Calculate the bounding rectangle of the region that must be redrawn to redraw the given document section.
@param aStartDocPos
	The section starting position
@param aEndDocPos
	The section ending position
@param aTopLeft
	Textview position in graphics context coordinates. The resulting region can be used
	to clip the drawing in that graphics context.
@param aBoundingRect
	The resulting bounding rectangle, that is the smallest rectangle that should be redrawn
	to redraw the specified document section.
*/
EXPORT_C void CTmTextLayout::GetUpdateBoundingRect(TInt aStartDocPos, TInt aEndDocPos,
		const TPoint& aTopLeft, TRect& aBoundingRect) const
	{
	aBoundingRect = TRect(TRect::EUninitialized);

	if (! iSource || aEndDocPos <= aStartDocPos)
		{
		return;
		}

	RTmBoundingRectInterpreter interpreter(*iSource, *this);
	TRect rect(TRect::EUninitialized); // set by call to RTmBoundingRectInterpreter::FirstRect
	TTmDocPos start(aStartDocPos, TRUE);
	TTmDocPos end(aEndDocPos,FALSE);
	TBool found = interpreter.FirstRect(start.iPos, end.iPos, rect);
	while (found)
		{
		if (! rect.IsEmpty())
			{
			rect.Move(aTopLeft);
			if(aBoundingRect.IsEmpty())
				{
				aBoundingRect = rect;
				}
			else
				{
				aBoundingRect.BoundingRect(rect);
				}
			}

		found = interpreter.NextRect(rect);
		}
	interpreter.Close();
	}

EXPORT_C void CTmTextLayout::HighlightSection(CGraphicsContext& aGc,
	const TPoint& aTopLeft, TInt aStartDocPos, TInt aEndDocPos, const TRect& aClipRect) const
	{
	TTmHighlightExtensions highlightProperties;
	highlightProperties.SetAll(0);
	HighlightSection(aGc,aTopLeft,aStartDocPos,aEndDocPos,aClipRect,highlightProperties,0,0);
	}

/**
Analogous to HighlightSection(CGraphicsContext&, const TPoint&, TInt, TInt,
const TRect&) but with highlight extensions applied.
@see HighlightSecion(CGraphicsContext&, const TPoint&, TInt, TInt, const
TRect&)
@publishedPartner
@released
*/
EXPORT_C void CTmTextLayout::HighlightSection(CGraphicsContext& aGc,
	const TPoint& aTopLeft, TInt aStartDocPos, TInt aEndDocPos,
	const TRect& aClipRect, const TTmHighlightExtensions& aHighlightExtensions,
	TInt aHighlightStartDocPos, TInt aHighlightEndDocPos) const
	{
	if (0 == aHighlightStartDocPos && 0 == aHighlightEndDocPos)
		{
		aHighlightStartDocPos = aStartDocPos;
		aHighlightEndDocPos = aEndDocPos;
		}
	else
		{
		if (aStartDocPos < aHighlightStartDocPos)
			{
			aHighlightStartDocPos = aStartDocPos;
			}
		
		if (aEndDocPos > aHighlightEndDocPos)
			{
			aHighlightEndDocPos = aEndDocPos;
			}
		}

	DrawSection(aGc, aTopLeft, aStartDocPos, aEndDocPos, aClipRect, aHighlightExtensions,
		aHighlightStartDocPos, aHighlightEndDocPos);
	}


EXPORT_C void CTmTextLayout::DrawSection(CGraphicsContext& aGc,
	const TPoint& aTopLeft, TInt aStartDocPos, TInt aEndDocPos, const TRect& aClipRect) const
	{
	TTmHighlightExtensions highlightProperties;
	highlightProperties.SetAll(0);
	DrawSection(aGc,aTopLeft,aStartDocPos,aEndDocPos,aClipRect,highlightProperties,0,0);
	}
	
/**
Analogous to DrawSection(CGraphicsContext&, const TPoint&, TInt, TInt, const
TRect&) but with highlight extensions applied.
@see DrawSection(CGraphicsContext&, const TPoint&, TInt, TInt, const TRect&)
@publishedPartner
@released
*/
EXPORT_C void CTmTextLayout::DrawSection(CGraphicsContext& aGc,
	const TPoint& aTopLeft, TInt aStartDocPos, TInt aEndDocPos,
	const TRect& aClipRect, const TTmHighlightExtensions& aHighlightExtensions,
	TInt aHighlightStartDocPos, TInt aHighlightEndDocPos) const
	{
	// note: Assumes that RWindow::BeginRedraw() has been called by the caller.

	TCursorSelection selection;
	selection.SetSelection(aHighlightStartDocPos, aHighlightEndDocPos);

	TRect boundingRect;
	GetUpdateBoundingRect(aStartDocPos, aEndDocPos, aTopLeft, boundingRect);
	if (! boundingRect.IsEmpty())
		{
		aHighlightExtensions.AbsExtendRect(boundingRect);
		boundingRect.Intersection(aClipRect);
		if (! boundingRect.IsEmpty())
			{
			DrawLayout(aGc, aTopLeft, boundingRect, NULL, ETrue, &selection, &aHighlightExtensions);
			}
		}
	}

/**
Finds a document position and returns information about it and the line it is
in.
@param aDocPos The position to find.
@param aPosInfo Returns information on the found position.
@param aLineInfo Returns information on the line containing the position found.
@return
	True if the position was found within the formatted text. False if the
	position was not in the formatted text.
*/
EXPORT_C TBool CTmTextLayout::FindDocPos(const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aPosInfo, TTmLineInfo& aLineInfo) const
	{
	if (!iSource)
		return FALSE;
	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));
	TBool found = interpreter.FindDocPos(aDocPos,aPosInfo);
	aLineInfo = interpreter.LineInfo();
	interpreter.Close();
	return found;
	}

/**
Find a document position and return information about it and the line it is in. Return FALSE if the document
position is outside the formatted text. The document position to be found is passed in aDocPos and the information
about the position found is returned in aPosInfo. The line information is returned in aLineInfo.
@deprecated 7.0s
*/
EXPORT_C TBool CTmTextLayout::FindDocPos(const TTmDocPos& aDocPos,TTmPosInfo& aPosInfo,TTmLineInfo& aLineInfo) const
	{
	TTmDocPosSpec dp = aDocPos;
	TTmPosInfo2 pi;
	TBool r = FindDocPos(dp, pi, aLineInfo);
	aPosInfo = pi;
	return r;
	}

/**
Finds a document position and returns information about it and the line it is
in.
@param aDocPos The position to find.
@param aPosInfo Returns information on the found position.
@param aLineInfo Returns information on the line containing the position found.
@param aSubscript Returns the subscript height value.
@return
	True if the position was found within the formatted text. False if the
	position was not in the formatted text.
*/
EXPORT_C TBool CTmTextLayout::FindDocPos(const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aPosInfo, TTmLineInfo& aLineInfo, TInt& aSubscript) const
	{
	if (!iSource)
		return FALSE;
	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));
	TBool found = interpreter.FindDocPos(aDocPos,aPosInfo);
	aLineInfo = interpreter.LineInfo();
	aSubscript = interpreter.Subscript();
	interpreter.Close();
	return found;
	}

/**
Finds a document position (i.e. the edge between two adjacent characters) by
x-y position relative to the top-left corner of the formatted text.
@param aXyPos Position to find.
@param aPosInfo Returns information on the position closest to aXyPos.
@param aLineInfo Returns information on the line containing aXyPos.
@return
	True if the Y-position is within the formatted text, or false otherwise.
*/
EXPORT_C TBool CTmTextLayout::FindXyPos(const TPoint& aXyPos,
	TTmPosInfo2& aPosInfo, TTmLineInfo& aLineInfo) const
	{
	if (!iSource)
		return FALSE;
	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));
	TBool found = interpreter.FindXyPos(aXyPos,aPosInfo);
	aLineInfo = interpreter.LineInfo();
	interpreter.Close();
	return found;
	}

/**
Find an x-y position, relative to the top-left corner of the text, and return information about it and the line it is in.
Return FALSE if the position is outside the formatted text. The document position to be found is passed in aDocPos and
the information about the position found is returned in aPosInfo. The line information is returned in aLineInfo.
@deprecated 7.0s
*/
EXPORT_C TBool CTmTextLayout::FindXyPos(const TPoint& aXyPos,TTmPosInfo& aPosInfo,TTmLineInfo& aLineInfo) const
	{
	TTmPosInfo2 pi;
	TBool r = FindXyPos(aXyPos, pi, aLineInfo);
	aPosInfo = pi;
	return r;
	}

/**
Return the number of formatted lines.
*/
EXPORT_C TInt CTmTextLayout::Lines() const
	{
	if (!iSource)
		return 0;
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	return interpreter.Lines();
	}

/**
Return the number of formatted paragraphs.
*/
EXPORT_C TInt CTmTextLayout::Paragraphs() const
	{
	if (!iSource)
		return 0;
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	return interpreter.Paragraphs();
	}

/**
Returns information (including line number and formatting information) of the
line containing a given document position.
@param aDocPos Position to find.
@param aLineInfo
	Returns information about the line containing aDocPos if aDocPos is within
	the formatted text, or an undefined value if not.
@return True if the aDocPos was within the formatted text.
*/
EXPORT_C TBool CTmTextLayout::DocPosToLine(const TTmDocPosSpec& aDocPos,
	TTmLineInfo& aLineInfo) const
	{
	if (!iSource)
		return FALSE;
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	TBool found = interpreter.DocPosToLine(aDocPos);
	aLineInfo = interpreter.LineInfo();
	return found;
	}

/**
Retrieve information about the line containing a certain document position.
Put information about the line containing aDocPos in aLineInfo. Return FALSE if aDocPos is not in the formatted text.
@deprecated 7.0s
*/
EXPORT_C TBool CTmTextLayout::DocPosToLine(const TTmDocPos& aDocPos,TTmLineInfo& aLineInfo) const
	{
	TTmDocPosSpec dp = aDocPos;
	return DocPosToLine(dp, aLineInfo);
	}

/**
Retrieve information about a line, referring to the line by its line number.

Put information about the line numbered aLineNumber, counting the first formatted line as 0, in aLineInfo.
Return FALSE if aLineNumber is not formatted.
*/
EXPORT_C TBool CTmTextLayout::LineNumberToLine(TInt aLineNumber,TTmLineInfo& aLineInfo) const
	{
	if (!iSource)
		return FALSE;
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	TBool found = interpreter.LineNumberToLine(aLineNumber);
	aLineInfo = interpreter.LineInfo();
	return found;
	}

/**
Retrieve information about a line, referring to the line by its paragraph number and line number within the paragraph.

Put information about line aLineNumber in paragraph aParNumber, counting the first formatted paragraph as 0,
in aLineInfo. Return FALSE if aParNumber is not formatted. If aLineInPar is greater than the number of lines in
the paragraph the last line in the paragraph is returned.
*/
EXPORT_C TBool CTmTextLayout::ParNumberToLine(TInt aParNumber,TInt aLineInPar,TTmLineInfo& aLineInfo) const
	{
	if (!iSource)
		return FALSE;
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	TBool found = interpreter.ParNumberToLine(aParNumber,aLineInPar);
	aLineInfo = interpreter.LineInfo();
	return found;
	}

/**
Retrieve information about a line containing a certain y coordinate.

Put information about the line containing the y coordinate aYPos, relative to 0 as the top of the formatted text, in
aLineInfo. Return FALSE if aYPos is outside all formatted lines.
*/
EXPORT_C TBool CTmTextLayout::YPosToLine(TInt aYPos,TTmLineInfo& aLineInfo) const
	{
	if (!iSource)
		return FALSE;
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	TBool found = interpreter.YPosToLine(aYPos);
	aLineInfo = interpreter.LineInfo();
	return found;
	}


/**
Retrieve the text as displayed on a particular line.

The text will be returned in display order, after all transformations have been
applied to the backing text. These transformations include bidirectional
reordering (making Arabic and Hebrew go right to left, etc.), ligation
(combining adjacent characters into a single glyph), suppressing control
characters, and converting characters to visible forms like visible spaces if
necessary.

Precondition: SetTextL must have been called on this object before
GetDisplayedText can be called.

Please note that any text that has passed through a shaper (for example
Devanagari text) will be replaced in the returned text with the Unicode
Replacement Character 0xFFFD. This is because the shaper produces glyph codes,
not Unicode characters, and this is because Unicode does not contain all the
ligatures and glyph variants required to display such scripts correctly.

@param aLineNumber The line to fetch the display text of.
@param aText A buffer into which the display text is placed.
@param aNeeded
	Returns the number of characters in the displayed line. The text is
	truncated if aText is not big enough, aNeeded returns how large the buffer
	would need to be to return the whole line.
@return
	EFalse if the operation is impossible, perhaps because aLineNumber
is too large to be within the text.
@since 7.0
*/
EXPORT_C TBool CTmTextLayout::GetDisplayedTextL(TInt aLineNumber, TDes& aText,
	TInt& aNeeded) const
	{
	if (!iSource)
	    {
	    OstTrace0( TRACE_FATAL, CTMTEXTLAYOUT_GETDISPLAYEDTEXTL, "ENoSource" );
	    }
	__ASSERT_ALWAYS(iSource, TmPanic(ENoSource));
	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));
	CleanupClosePushL(interpreter);
	TBool result = interpreter.GetDisplayedTextL(aLineNumber, aText,0, aNeeded);
	CleanupStack::PopAndDestroy();
	return result;
	}

/**
Retrieve the text as displayed on a particular line. Return FALSE if the operation is impossible (e.g., because no
MTmSource has yet been assigned). The text is returned in aText. The number of characters needed, which may be greater
than the size of aText, is placed in aNeeded.

The text will be returned in display order, after all transformations have been applied to the backing text. These
transformations include bidirectional reordering (making Arabic and Hebrew go right to left, etc.), ligation (combining
adjacent characters into a single glyph), suppressing control characters, and converting characters to visible forms like
visible spaces if necessary.
@deprecated 7.0 It can fail due to environmental conditions, and should therefore be leavable. Use GetDisplayedTextL instead.
*/
EXPORT_C TBool CTmTextLayout::GetDisplayedText(TInt aLineNumber,TDes& aText,TInt& aNeeded) const
	{
	if (!iSource)
		return FALSE;
	TBool result = EFalse;
	TRAPD(err, result = GetDisplayedTextL(aLineNumber, aText, aNeeded));
    UNUSED_VAR(err);
	return result;
	}

/**
Get the size of the minimal bounding box of the text when formatted to the specified wrap width.
The width may be greater than aWrapWidth. To find the minimum width for the text,
pass 0 for aWrapWidth. To find the minimum height, pass KMaxTInt for aWrapWidth.
*/
EXPORT_C void CTmTextLayout::GetMinimumLayoutSizeL(TInt aWrapWidth,TSize& aSize) const
	{
		GetMinimumLayoutSizeL(aWrapWidth,ETrue,aSize);
	}

/**
Get the size of the minimal bounding box of the text when formatted to the specified wrap width.
The width may be greater than aWrapWidth. To find the minimum width for the text,
pass 0 for aWrapWidth. To find the minimum height, pass KMaxTInt for aWrapWidth.
Use aAllowLegalLineBreaksOnly to set whether or not illegal line breaks should be considered
when determining aSize.
*/
EXPORT_C void CTmTextLayout::GetMinimumLayoutSizeL(TInt aWrapWidth,TBool aAllowLegalLineBreaksOnly,TSize& aSize) const
	{
	if (!iSource)
		{
		aSize.iWidth = aSize.iHeight = 0;
		return;
		}
	TTmFormatParam param;
	param.iWrapWidth = aWrapWidth;
	param.iFlags = TTmFormatParam::EWrap;
	if(aAllowLegalLineBreaksOnly)
		param.iFlags |= TTmFormatParam::ELegalLineBreaksOnly;
	param.iStartChar = iStartChar;
	param.iEndChar = iEndChar;
	CTmCode* code = new(ELeave) CTmCode;
	CleanupStack::PushL(code);
	CTmFormatContext::TInfo info;
	
	//Get line context character from previous line if there is
	TTmDocPosSpec startDocPos(param.iStartChar - 1, static_cast<TTmDocPosSpec::TType>(1));
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	TBool lineFound = interpreter.DocPosToLine(startDocPos);
	if (lineFound)
		info.iContextCharPerLine = interpreter.LineContextCharChar();
	else
		info.iContextCharPerLine = NULL;
	
	CTmFormatContext::FormatL(*iSource,param,*code,info,const_cast<CTmTextLayout*>(this));
	CleanupStack::PopAndDestroy(); // code
	aSize.iWidth = info.iWidth;
	aSize.iHeight = info.iHeight;
	}

/**
Find the width of the widest line in the specified vertical range. The width is the 'inner width'; that of the text
itself not including indents or margins.
*/
EXPORT_C TInt CTmTextLayout::WidthOfWidestLine(TInt aTop,TInt aBottom) const
	{
	if (!iSource)
		return 0;
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	return interpreter.WidthOfWidestLine(aTop, aBottom);
	}

EXPORT_C void CTmTextLayout::HorizontalExtremes(
	TInt &aLeft, TInt &aRight, TInt aTopY, TInt aBottomY) const
	{
	if (!iSource)
		{
		aLeft = KMaxTInt;
		aRight = KMinTInt;
		return;
		}
	TTmInterpreter interpreter(TTmInterpreterParam(*this));
	interpreter.HorizontalExtremes(aLeft, aRight, aTopY, aBottomY);
	}

/**
Finds information about the visual position closest to the point specified. If
there are two positions at this place that represent the ends of blocks that
are not logically contiguous, different positions representing the ends of
these blocks are returned. there is an ambiguity at this position, the document
positions that represent the ends of the different blocks that meet here are
returned.
@param aXyPos
	The position to find.
@param aPosLeft
	The position returned. If this position is ambiguous, this is the position
	attached to the block on the left.
@param aPosRight
	The position returned. If this position is ambiguous, this is the position
	attached to the block on the right. If it is not ambiguous, aPosLeft ==
	aPosRight.
@param aLineInfo
	Information about the line containing aPosLeft and aPosRight.
@return
	ETrue if the position was found. EFalse otherwise. If EFalse, aPosLeft,
	aPosRight and aLineInfo are undefined.
*/
EXPORT_C TBool CTmTextLayout::FindXyPosWithDisambiguation(
	const TPoint& aXyPos,
	TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight,
	TTmLineInfo& aLineInfo) const
	{
	if (!iSource)
		return EFalse;

	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));
	if (!interpreter.YPosToLine(aXyPos.iY))
		return EFalse;
	aLineInfo = interpreter.LineInfo();

	RTmGraphemeEdgeIterator edgeIterator;
	edgeIterator.Begin(interpreter);
	TTmVisualDocPos pos;

	edgeIterator.FindXPos(aXyPos.iX, pos);
	aPosLeft = pos.LeftEdge();
	aPosRight = pos.RightEdge();

	edgeIterator.Close();
	interpreter.Close();
	return pos.Ambiguity() != TTmVisualDocPos::ENotFound;
	}

/**
Finds the position after the horizontally spacing element after aStart. A
horizontally spacing element is a spacing glyph (such as a letter or number)
plus all its combining accent, or other spacing character such as a tab or
carriage return. This function will also skip any zero-width characters that
precede the next horizontally spacing element.
@param aStart
	Starting position. The returned position will be greater than this.
@return
	The position of the first trailing edge beyond aStart not visually
	coincident with the leading edge of aStart, or KErrNotFound if there is
	none, or it is not within formatted text.
*/
EXPORT_C TInt CTmTextLayout::FindNextPos(TInt aStart) const
	{
	if (!iSource)
		return KErrNotFound;

	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));

	if (!interpreter.DocPosToLine(TTmDocPos(aStart + 1, EFalse)))
		return KErrNotFound;

	RTmGraphemeEdgeIterator edgeIterator;
	edgeIterator.Begin(interpreter);

	TInt pos = edgeIterator.NextPosition(aStart);

	edgeIterator.Close();
	interpreter.Close();
	return pos;
	}

/**
Finds the position before the horizontally spacing element before aStart.
Zero-width characters immediately before aStart are also skipped.
@param aStart
	Starting position. The returned position will be greater than this.
@return
	The position of the first trailing edge beyond aStart not visually
	coincident with the leading edge of aStart, or KErrNotFound if there is
	none, or it is not within formatted text.
@see FindNextPos
*/
EXPORT_C TInt CTmTextLayout::FindPreviousPos(TInt aStart) const
	{
	if (!iSource)
		return KErrNotFound;

	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));

	if (!interpreter.DocPosToLine(TTmDocPos(aStart - 1, ETrue)))
		return KErrNotFound;

	RTmGraphemeEdgeIterator edgeIterator;
	edgeIterator.Begin(interpreter);

	TInt pos = edgeIterator.PreviousPosition(aStart);

	edgeIterator.Close();
	interpreter.Close();
	return pos;
	}

/**
Finds information about the position visually to the left of the position
specified, as long as it is on the same line. If there are two positions at
this place that represent the ends of blocks that are not logically contiguous,
different positions representing the ends of these blocks are returned.
@param aDocPos
	The position to find.
@param aPosLeft
	The position returned. If this position is ambiguous, this is the position
	attached to the block on the left.
@param aPosRight
	The position returned. If this position is ambiguous, this is the position
	attached to the block on the right. If it is not ambiguous, aPosLeft ==
	aPosRight.
@return
	ETrue if the position was found on the same line as aDocPos. EFalse
	otherwise. If EFalse, aPosLeft and aPosRight are undefined.
*/
EXPORT_C TBool CTmTextLayout::GetNextPosLeftWithDisambiguation(
	const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight) const
	{
	if (!iSource)
		return EFalse;

	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));
	if (!interpreter.DocPosToLine(aDocPos))
		return EFalse;

	RTmGraphemeEdgeIterator edgeIterator;
	edgeIterator.Begin(interpreter);
	TTmPosInfo2 closest;
	TTmVisualDocPos next;

	edgeIterator.FindEdgeLeftwards(aDocPos, closest, next);
	aPosLeft = next.LeftEdge();
	aPosRight = next.RightEdge();

	edgeIterator.Close();
	interpreter.Close();
	return next.Ambiguity() != TTmVisualDocPos::ENotFound;
	}

/**
Finds information about the position visually to the right of the position
specified, as long as it is on the same line. If there are two positions at
this place that represent the ends of blocks that are not logically contiguous,
different positions representing the ends of these blocks are returned.
@param aDocPos
	The position to find.
@param aPosLeft
	The position returned. If this position is ambiguous, this is the position
	attached to the block on the left.
@param aPosRight
	The position returned. If this position is ambiguous, this is the position
	attached to the block on the right. If it is not ambiguous, aPosLeft ==
	aPosRight.
@return
	ETrue if the position was found on the same line as aDocPos. EFalse
	otherwise. If EFalse, aPosLeft and aPosRight are undefined.
*/
EXPORT_C TBool CTmTextLayout::GetNextPosRightWithDisambiguation(
	const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight) const
	{
	if (!iSource)
		return EFalse;

	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));
	if (!interpreter.DocPosToLine(aDocPos))
		return EFalse;

	RTmGraphemeEdgeIterator edgeIterator;
	edgeIterator.Begin(interpreter);
	TTmPosInfo2 closest;
	TTmVisualDocPos next;

	edgeIterator.FindEdgeRightwards(aDocPos, closest, next);
	aPosLeft = next.LeftEdge();
	aPosRight = next.RightEdge();

	edgeIterator.Close();
	interpreter.Close();
	return next.Ambiguity() != TTmVisualDocPos::ENotFound;
	}

/**
Find the nearest document position to the current one in the visual direction
specified. This only works if the document position is within formatted text.
False is returned if this condition is not met.
@param aDocPos Start position.
@param aInfo
	Returns the position found, or an undefined value if aDocPos is not in
	formatted text or is already at the end of the line in the direction being
	searched.
@param aToLeft True if the direction to search in is leftwards.
@return True if the next position was found (see the description of aInfo).
*/
EXPORT_C TBool CTmTextLayout::GetNextVisualCursorPos(
	const TTmDocPosSpec& aDocPos, TTmPosInfo2& aInfo, TBool aToLeft) const
	{
	TTmPosInfo2 dummy;
	return aToLeft?
		GetNextPosLeftWithDisambiguation(aDocPos, dummy, aInfo)
		: GetNextPosRightWithDisambiguation(aDocPos, aInfo, dummy);
	}

/**
Get the next cursor position in visual order. This function is useful for text that contains Arabic, Hebrew, or
other right-to-left languages because it allows the cursor to be moved right or left on the display whatever the
order of the backing text.

The cursor position to the left or right of aDocPos is found, depending on the value of aToLeft. Information about
the resulting position is returned in aInfo. The function returns TRUE if successful, FALSE if not. A FALSE return value
means that there is no text, or aDocPos is outside the formatted text, or aDocPos cannot move in the required direction
because it is at the end of the line.
@deprecated 7.0s
*/
EXPORT_C TBool CTmTextLayout::GetNextVisualCursorPos(const TTmDocPos& aDocPos,TTmPosInfo& aInfo,TBool aToLeft) const
	{
	TTmDocPosSpec dp = aDocPos;
	TTmPosInfo2 pi;
	TBool r = GetNextVisualCursorPos(dp, pi, aToLeft);
	aInfo = pi;
	return r;
	}

/**
Returns the position at the left or right extreme of the line containing aPos.
@param aToRight EFalse for finding the left hand end, ETrue for the right.
@param aExtreme Returns the document position at the end requested.
@return
	EFalse if the call failed either because the position was unformatted or
	because the line contained no text.
*/
EXPORT_C TBool CTmTextLayout::LineExtreme(const TTmDocPosSpec& aPos,
	TBool aToRight, TTmDocPos& aExtreme) const
	{
	TTmPosInfo2 posInfo;
	TTmLineInfo lineInfo;
	if (!FindDocPos(aPos, posInfo, lineInfo))
		return EFalse;
	TPoint extreme(
		aToRight? lineInfo.iInnerRect.iBr.iX : lineInfo.iInnerRect.iTl.iX,
		lineInfo.iInnerRect.iTl.iY);
	if (!FindXyPos(extreme, posInfo, lineInfo))
		return EFalse;
	aExtreme = posInfo.iDocPos;
	return ETrue;
	}

/**
Returns the chunks of text adjacent to aPos.
@param aLeft Returns a description of the chunk on the left of the cursor.
@param aRight Returns a description of the chunk on the right.
@return
	EFalse if the call failed because the line is not formatted or contains
	no text.
*/
EXPORT_C TBool CTmTextLayout::FindAdjacentChunks(const TTmDocPosSpec& aPos,
	CTmTextLayout::TTmChunkDescription& aLeft,
	CTmTextLayout::TTmChunkDescription& aRight) const
	{
	// Following block of code is used to check whether a doc position is shared by two text chunks. 
	TBool overlapped = EFalse;
	TTmChunkInterpreter tempCi(*this);
	if (!tempCi.ChunkSetToLeftOfLine(aPos))
		return EFalse;
	TTmChunkInterpreter::TContainment contain1 = tempCi.ChunkContainsPos(aPos);
	TTmChunkInterpreter::TContainment contain2;
	while(tempCi.ChunkNext())
		{
		contain2 = tempCi.ChunkContainsPos(aPos);
		if ( (contain1 == TTmChunkInterpreter::EMiddleOfChunk && contain2 != TTmChunkInterpreter::ENotInChunk)
				|| (contain1 != TTmChunkInterpreter::ENotInChunk && contain2 == TTmChunkInterpreter::EMiddleOfChunk) )
			{
			overlapped = ETrue;
			break;
			}
		if ( contain1 != TTmChunkInterpreter::ENotInChunk )
			break;
		contain1 = contain2;
		}
	// end of checking...

	TTmChunkInterpreter ci(*this);
	if (!ci.ChunkSetToLeftOfLine(aPos))
		return EFalse;
	aLeft.iStart = -1;
	if (aPos.iPos == ci.LineInfo().iStart
		&& aPos.iType == TTmDocPosSpec::ETrailing
		&& !(ci.LineInfo().iFlags & TTmLineInfo::EParRightToLeft))
		{
		aRight.iStart = ci.ChunkStart();
		aRight.iEnd = ci.ChunkEnd();
		aRight.iRightToLeft = ci.ChunkRightToLeft();
		return ETrue;
		}
	TTmChunkInterpreter::TContainment containment = ci.ChunkContainsPos(aPos);
	// When a doc postion is overlapped, it would be in middle of one chunk and on edge of another
	// chunk, so (MiddleOfChunk && overlapped) is also included.
	while (containment == TTmChunkInterpreter::ENotInChunk
		|| containment == TTmChunkInterpreter::ERightEdgeOfChunk
		||(containment == TTmChunkInterpreter::EMiddleOfChunk && overlapped))
		{
		aLeft.iStart = ci.ChunkStart();
		aLeft.iEnd = ci.ChunkEnd();
		aLeft.iRightToLeft = ci.ChunkRightToLeft();
		if (!ci.ChunkNext())
			{
			aRight.iStart = -1;
			return ETrue;
			}

		if ( containment == TTmChunkInterpreter::ENotInChunk )
			{
			containment = ci.ChunkContainsPos(aPos);
			}
		else if (containment == TTmChunkInterpreter::EMiddleOfChunk && overlapped)
			{
			containment = ci.ChunkContainsPos(aPos);
			// just in case the doc position is in middle of both of the two overlapped chunks,
			// however this should not happen.
			if (containment == TTmChunkInterpreter::EMiddleOfChunk)
				break;
			}
		else if (containment == TTmChunkInterpreter::ERightEdgeOfChunk)
			{
			containment = TTmChunkInterpreter::ELeftEdgeOfChunk;
			}
		}
	if (containment == TTmChunkInterpreter::ELeftEdgeOfChunk)
		{
		aRight.iStart = ci.ChunkStart();
		aRight.iEnd = ci.ChunkEnd();
		aRight.iRightToLeft = ci.ChunkRightToLeft();
		return ETrue;
		}
	// position is in the middle of a chunk, so we will split it.
	if (ci.ChunkRightToLeft())
		{
		aLeft.iRightToLeft = ETrue;
		aRight.iRightToLeft = ETrue;
		aRight.iStart = ci.ChunkStart();
		aRight.iEnd = aPos.iPos;
		aLeft.iStart = aPos.iPos;
		aLeft.iEnd = ci.ChunkEnd();
		}
	else
		{
		aLeft.iRightToLeft = EFalse;
		aRight.iRightToLeft = EFalse;
		aLeft.iStart = ci.ChunkStart();
		aLeft.iEnd = aPos.iPos;
		aRight.iStart = aPos.iPos;
		aRight.iEnd = ci.ChunkEnd();
		}
	return ETrue;
	}

/**
Returns metrics for the cursor suitable for the document position and placement
specified, as well as information about the line containing the position
specified.
@param aDocPos The document position to return information about.
@param aPlacement The shape of the cursor.
@param aLineInfo
	Returns information about the line containing aDocPos if this position is
	within the formatted text, or an undefined value otherwise.
@param aOrigin
	Returns a position to which aWidth, aAscent and aDescent is relative if the
	position is within formatted text. Returns an undefined value otherwise.
	This position will be on the baseline of the character found.
@param aWidth
	When aPlacement is ECursorVertical aWidth represents which side of the
	origin to paint the cursor, -1 paint left, +1 paint right. When aPlacement
	is ECursorUnderlineXXXX aWidth represents the X offset from the origin X to
	draw the cursor between, -ve for previous & +ve for next when text L2R
	(e.g. Latin), +ve for previous & -ve for next when text is R2L (e.g. Arabic)
@param aAscent
	Returns how far above aOrigin the cursor extends if aDocPos is within
	formatted text, or an undefined value otherwise.
@param aDescent
	Returns how far below aOrigin the cursor extends if aDocPos is within
	formatted text, or an undefined value otherwise.
*/
EXPORT_C TBool CTmTextLayout::GetCursor(const TTmDocPosSpec& aDocPos,TTmCursorPlacement aPlacement,
		TTmLineInfo& aLineInfo,TPoint& aOrigin,TInt& aWidth,TInt& aAscent,TInt& aDescent) const
	{
	aOrigin.SetXY(0,0);
	aWidth = aAscent = aDescent = 0;
	if (!iSource)
		return EFalse;

	RTmGeneralInterpreter interpreter(*iSource, TTmInterpreterParam(*this));
	if (!interpreter.DocPosToLine(aDocPos))
		return EFalse;
	RTmGraphemeEdgeIterator edgeIterator;
	edgeIterator.Begin(interpreter);

	TTmPosInfo2 this_info;
	TTmVisualDocPos next;
	TBool go_left = (aPlacement == ECursorUnderlinePrev);
	RTmGraphemeEdgeIterator::TEdgesFound found = go_left?
		edgeIterator.FindEdgeLeftwards(aDocPos, this_info, next)
		: edgeIterator.FindEdgeRightwards(aDocPos, this_info, next);
	if (found == RTmGraphemeEdgeIterator::ENone)
		{
		edgeIterator.Close();
		interpreter.Close();
		return EFalse;
		}
	TPoint nextPoint = this_info.iEdge;
	if (found == RTmGraphemeEdgeIterator::ENearestAndNext)
		nextPoint = next.NearestToX(nextPoint.iX)->iEdge;

	aOrigin = go_left ? nextPoint : this_info.iEdge;
	aLineInfo = interpreter.LineInfo();

	int pos = this_info.iDocPos.iPos;
	if (pos > interpreter.LineInfo().iStart || !interpreter.ParStart())
		pos--;
	TPtrC text;
	CTmTextFontCache* font = NULL;
	interpreter.GetText(pos,KMaxTInt,text,NULL,&font);

	if (font)
		{
		if (aPlacement == ECursorVertical)
			{
			aAscent = font->Font().AscentInPixels();
			aDescent = font->Font().DescentInPixels();
			// In case the cursor is positioned on some super/sub or normal 
			// font we need to adjust the Y position of the cursor with the 
			// font's baseline offset so the cursor sits on the same horizon 
			// as the text.
			aOrigin.iY += font->Font().BaselineOffsetInPixels();
			// -1 indicates that the cursor should hang to the left: this is
			// for right-to-left paragraphs.
			aWidth = aLineInfo.iFlags & TTmLineInfo::EParRightToLeft? -1 : 1;
			}
		else
			{
			aAscent = 0;
			aDescent = 1;
			aWidth = nextPoint.iX - this_info.iEdge.iX;
			// The next if doesn't work to well with ECursorUnderlinePrev 
			// placement as it is an approximation.
			if (aWidth == 0) // Triggered when at start or end of text line
				aWidth = font->Font().HeightInPixels() / 2;
			if (aWidth < 0)
				aWidth = -aWidth;
			}
		font->Close();
		}
	else
		found = RTmGraphemeEdgeIterator::ENone;
	edgeIterator.Close();
	interpreter.Close();
	return found != RTmGraphemeEdgeIterator::ENone;
	}

/**
Return cursor metrics suitable for the supplied document position and placement.
The cursor is defined like a character, with an origin, width, height and depth.
This allows the caller to preserve the origin while overriding the other metrics.
@deprecated 7.0s
*/
EXPORT_C TBool CTmTextLayout::GetCursor(const TTmDocPos& aDocPos,TTmCursorPlacement aPlacement,
		TTmLineInfo& aLineInfo,TPoint& aOrigin,TInt& aWidth,TInt& aAscent,TInt& aDescent) const
	{
	TTmDocPosSpec dp = aDocPos;
	return GetCursor(dp, aPlacement, aLineInfo, aOrigin, aWidth, aAscent, aDescent);
	}

/**
Return the amount of memory used by a CTmTextLayout object,
The amount returned is in bytes.
The amount returned does not include memory used by non-owned objects.
*/
EXPORT_C TInt CTmTextLayout::MemoryUsed() const
	{
	return sizeof(*this) + iCode.MemoryUsed() - sizeof(iCode);
	}

/** 
Stops or allows text to be drawn.  Included to allow users to control visibility
if text is part of an invisible control.
*/
EXPORT_C void CTmTextLayout::MakeVisible(TBool aVisible)
	{
	if(aVisible)
		{
		iDrawingInterpFlags &= ~RTmDrawingInterpreter::EInvisible;
		}
	else
		{
		iDrawingInterpFlags |= RTmDrawingInterpreter::EInvisible;
		}
	}

TInt CTmTextLayout::GetDrawingInterpFlags() const
	{
	return iDrawingInterpFlags;
	}

#ifdef _DEBUG
void CTmTextLayout::Invariant() const
	{
	int lines = Lines();
	if (lines > 0)
		{
		TTmLineInfo info;
		if (!LineNumberToLine(lines - 1,info))
			TmPanic(EInvariant);
		int end = info.iEnd;
		if (end != iEndChar)
			TmPanic(EInvariant);
		}
	}
#endif

/*
Adjust the width of the text, taking overwidth paragraphs like non-wrapped ones into account.
aParam is the formatting parameters.
aWidthOfNewText is the width of the widest line of any new text that has been inserted and has caused the adjustment.
*/
void CTmTextLayout::AdjustWidth(const TTmFormatParamBase& aParam,TInt aWidthOfNewText)
	{
	if (aParam.IsWrapping())
		{
		int wrap_width = aParam.iWrapWidth;
		int x_scale = 1000;
		int y_scale = 1000;
		CalculateScale(*iSource,x_scale,y_scale);
		if (x_scale != 1000)
			wrap_width = wrap_width * x_scale / 1000;
		if (iWidth > wrap_width || aWidthOfNewText > wrap_width)
			{
			iWidth = WidthOfWidestLine();
			if (iWidth < wrap_width)
				iWidth = wrap_width;
			}
		else
			iWidth = wrap_width;
		}
	else
		iWidth = WidthOfWidestLine();
	}

/**
The equality operator.
@return True if both sides are identical.
*/
EXPORT_C TBool TTmDocPos::operator==(const TTmDocPos& aPos) const
	{
	return iPos == aPos.iPos && !iLeadingEdge == !aPos.iLeadingEdge;
	}

/**
Greater than operator.
@return
	True if the left side of the operator is further on in the document than
	the right hand side.
*/
EXPORT_C TBool TTmDocPos::operator>(const TTmDocPos& aPos) const
	{
	return iPos > aPos.iPos || (iPos == aPos.iPos && iLeadingEdge && !aPos.iLeadingEdge);
	}

/**
Greater than or equal to operator.
@return
	True if the left side of the operator is further on in the document than
	the right hand side or if both sides are identical.
*/
EXPORT_C TBool TTmDocPos::operator>=(const TTmDocPos& aPos) const
	{
	return iPos > aPos.iPos || (iPos == aPos.iPos && (iLeadingEdge || !aPos.iLeadingEdge));
	}
