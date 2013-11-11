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
* MTmTextLayoutForwarder functions.
*
*/


#include "TAGMA.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

/**
Return the number of lines in the text.
*/
EXPORT_C TInt MTmTextLayoutForwarder::Lines() const
	{
	return TextLayout().Lines();
	}

/**
Return the number of paragraphs in the text.
*/
EXPORT_C TInt MTmTextLayoutForwarder::Paragraphs() const
	{
	return TextLayout().Paragraphs();
	}

/**
Return the displayed form of the text for a particular line. See CTmLayout::GetDisplayedText for details.
*/
EXPORT_C TBool MTmTextLayoutForwarder::GetDisplayedText(TInt aLineNumber,TDes& aText,TInt& aNeeded) const
	{
	return TextLayout().GetDisplayedText(aLineNumber,aText,aNeeded);
	}

/**
Get the minimum size of the formatted text. See CTmTextLayout::GetMinimumLayoutSizeL for details.
*/
EXPORT_C void MTmTextLayoutForwarder::GetMinimumLayoutSizeL(TInt aWrapWidth,TSize& aSize) const
	{
	TextLayout().GetMinimumLayoutSizeL(aWrapWidth,aSize);
	}

/**
Get the minimum size of the formatted text. See CTmTextLayout::GetMinimumLayoutSizeL for details.
*/
EXPORT_C void MTmTextLayoutForwarder::GetMinimumLayoutSizeL(TInt aWrapWidth,TBool aAllowLegalLineBreaksOnly,TSize& aSize) const
	{
	TextLayout().GetMinimumLayoutSizeL(aWrapWidth,aAllowLegalLineBreaksOnly,aSize);
	}

/**
Return the first formatted character.
*/
EXPORT_C TInt MTmTextLayoutForwarder::StartChar() const
	{
	return TextLayout().StartChar();
	}

/**
Return the character after the last formatted character.
*/
EXPORT_C TInt MTmTextLayoutForwarder::EndChar() const
	{
	return TextLayout().EndChar();
	}

/**
Return the width in pixels of the formatted text, not including paragraph labels if present.
*/
EXPORT_C TInt MTmTextLayoutForwarder::LayoutWidth() const
	{
	return TextLayout().LayoutWidth();
	}

/**
Return the height in pixels of the formatted text.
*/
EXPORT_C TInt MTmTextLayoutForwarder::LayoutHeight() const
	{
	return TextLayout().LayoutHeight();
	}

EXPORT_C void MTmTextLayoutForwarder::DrawLayout(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
												 const TLogicalRgb* aDocBackground,TBool aDrawParBackground) const
	{
	TPoint origin;
	GetOrigin(origin);
	TextLayout().DrawLayout(aGc,aTopLeft + origin,aClipRect,aDocBackground,aDrawParBackground);
	}

EXPORT_C void MTmTextLayoutForwarder::DrawBackground(CGraphicsContext& aGc,const TPoint& aTopLeft,
													 const TRect& aClipRect,const TLogicalRgb& aBackground) const
	{
	TPoint origin;
	GetOrigin(origin);
	TextLayout().DrawBackground(aGc,aTopLeft + origin,aClipRect,aBackground);
	}

EXPORT_C void MTmTextLayoutForwarder::InvertLayout(CGraphicsContext& aGc,const TPoint& aTopLeft,
												   TInt aStartDocPos,TInt aEndDocPos)
	{
	TPoint origin;
	GetOrigin(origin);
	TextLayout().InvertLayout(aGc,aTopLeft + origin,aStartDocPos,aEndDocPos);
	}

EXPORT_C TBool MTmTextLayoutForwarder::FindDocPos(const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aPosInfo, TTmLineInfo& aLineInfo) const
	{
	TPoint origin;
	GetOrigin(origin);
	TBool result = TextLayout().FindDocPos(aDocPos,aPosInfo,aLineInfo);
	if (origin.iX || origin.iY)
		{
		FixUpLineInfo(aLineInfo,&origin);
		aPosInfo.iEdge += origin;
		}
	return result;
	}

/** @deprecated 7.0s */
EXPORT_C TBool MTmTextLayoutForwarder::FindDocPos(const TTmDocPos& aDocPos,TTmPosInfo& aPosInfo,
												  TTmLineInfo& aLineInfo) const
	{
	TTmDocPosSpec dp = aDocPos;
	TTmPosInfo2 pi;
	TBool r = FindDocPos(dp, pi, aLineInfo);
	aPosInfo = pi;
	return r;
	}

EXPORT_C TBool MTmTextLayoutForwarder::FindXyPos(const TPoint& aXyPos,
	TTmPosInfo2& aPosInfo,TTmLineInfo& aLineInfo) const
	{
	TPoint origin;
	GetOrigin(origin);
	TBool result = TextLayout().FindXyPos(aXyPos - origin,aPosInfo,aLineInfo);
	if (origin.iX || origin.iY)
		{
		FixUpLineInfo(aLineInfo,&origin);
		aPosInfo.iEdge += origin;
		}
	return result;
	}

EXPORT_C TBool MTmTextLayoutForwarder::FindXyPosWithDisambiguation(
	const TPoint& aXyPos, TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight,
	TTmLineInfo& aLineInfo) const
	{
	TPoint origin;
	GetOrigin(origin);
	TBool result = TextLayout().FindXyPosWithDisambiguation(aXyPos - origin,
		aPosLeft, aPosRight, aLineInfo);
	if (origin.iX || origin.iY)
		{
		FixUpLineInfo(aLineInfo, &origin);
		aPosLeft.iEdge += origin;
		aPosRight.iEdge += origin;
		}
	return result;
	}

EXPORT_C TBool MTmTextLayoutForwarder::GetNextPosLeftWithDisambiguation(
	const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight) const
	{
	TPoint origin;
	GetOrigin(origin);
	TBool result = TextLayout().GetNextPosLeftWithDisambiguation(aDocPos,
		aPosLeft, aPosRight);
	if (origin.iX || origin.iY)
		{
		aPosLeft.iEdge += origin;
		aPosRight.iEdge += origin;
		}
	return result;
	}

EXPORT_C TBool MTmTextLayoutForwarder::GetNextPosRightWithDisambiguation(
	const TTmDocPosSpec& aDocPos,
	TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight) const
	{
	TPoint origin;
	GetOrigin(origin);
	TBool result = TextLayout().GetNextPosRightWithDisambiguation(aDocPos,
		aPosLeft, aPosRight);
	if (origin.iX || origin.iY)
		{
		aPosLeft.iEdge += origin;
		aPosRight.iEdge += origin;
		}
	return result;
	}

/** @deprecated 7.0s */
EXPORT_C TBool MTmTextLayoutForwarder::FindXyPos(const TPoint& aXyPos,TTmPosInfo& aPosInfo,TTmLineInfo& aLineInfo) const
	{
	TTmPosInfo2 pi;
	TBool r = FindXyPos(aXyPos, pi, aLineInfo);
	aPosInfo = pi;
	return r;
	}

EXPORT_C TBool MTmTextLayoutForwarder::DocPosToLine(
	const TTmDocPosSpec& aDocPos, TTmLineInfo& aLineInfo) const
	{
	TBool result = TextLayout().DocPosToLine(aDocPos,aLineInfo);
	FixUpLineInfo(aLineInfo);
	return result;
	}

/** @deprecated 7.0s */
EXPORT_C TBool MTmTextLayoutForwarder::DocPosToLine(const TTmDocPos& aDocPos,TTmLineInfo& aLineInfo) const
	{
	TTmDocPosSpec dp = aDocPos;
	return DocPosToLine(dp, aLineInfo);
	}

EXPORT_C TBool MTmTextLayoutForwarder::LineNumberToLine(TInt aLineNumber,TTmLineInfo& aLineInfo) const
	{
	TBool result = TextLayout().LineNumberToLine(aLineNumber,aLineInfo);
	FixUpLineInfo(aLineInfo);
	return result;
	}

EXPORT_C TBool MTmTextLayoutForwarder::ParNumberToLine(TInt aParNumber,TInt aLineInPar,TTmLineInfo& aLineInfo) const
	{
	TBool result = TextLayout().ParNumberToLine(aParNumber,aLineInPar,aLineInfo);
	FixUpLineInfo(aLineInfo);
	return result;
	}

EXPORT_C TBool MTmTextLayoutForwarder::YPosToLine(TInt aYPos,TTmLineInfo& aLineInfo) const
	{
	TPoint origin;
	GetOrigin(origin);
	TBool result = TextLayout().YPosToLine(aYPos - origin.iY,aLineInfo);
	if (origin.iX || origin.iY)
		FixUpLineInfo(aLineInfo,&origin);
	return result;
	}

EXPORT_C TInt MTmTextLayoutForwarder::WidthOfWidestLine(TInt aTop,TInt aBottom) const
	{
	TPoint origin;
	GetOrigin(origin);
	aTop -= origin.iY;
	aBottom -= origin.iY;
	return TextLayout().WidthOfWidestLine(aTop,aBottom);
	}

EXPORT_C void MTmTextLayoutForwarder::HorizontalExtremes(
	TInt &aLeft, TInt &aRight, TInt aTopY, TInt aBottomY) const
	{
	TPoint origin;
	GetOrigin(origin);
	aTopY -= origin.iY;
	aBottomY -= origin.iY;
	TextLayout().HorizontalExtremes(aLeft, aRight, aTopY, aBottomY);
	if (aLeft != KMaxTInt)
		aLeft += origin.iX;
	if (aRight != KMinTInt)
		aRight += origin.iX;
	}

EXPORT_C TBool MTmTextLayoutForwarder::GetNextVisualCursorPos(
	const TTmDocPosSpec& aDocPos, TTmPosInfo2& aInfo, TBool aToLeft) const
	{
	TBool result = TextLayout().GetNextVisualCursorPos(aDocPos,aInfo,aToLeft);
	TPoint origin;
	GetOrigin(origin);
	aInfo.iEdge += origin;
	return result;
	}

/** deprecated 7.0s */
EXPORT_C TBool MTmTextLayoutForwarder::GetNextVisualCursorPos(const TTmDocPos& aDocPos,TTmPosInfo& aInfo,
															  TBool aToLeft) const
	{
	TBool result = TextLayout().GetNextVisualCursorPos(aDocPos,aInfo,aToLeft);
	TPoint origin;
	GetOrigin(origin);
	aInfo.iEdge += origin;
	return result;
	}

EXPORT_C TBool MTmTextLayoutForwarder::GetCursor(const TTmDocPosSpec& aDocPos,
	TTmCursorPlacement aPlacement, TTmLineInfo& aLineInfo, TPoint& aOrigin,
	TInt& aWidth, TInt& aAscent, TInt& aDescent) const
	{
	TBool result = TextLayout().GetCursor(aDocPos, aPlacement, aLineInfo,
		aOrigin, aWidth, aAscent, aDescent);
	if (result)
		{
		TPoint origin;
		GetOrigin(origin);
		if (origin.iX || origin.iY)
			{
			aOrigin += origin;
			FixUpLineInfo(aLineInfo,&origin);
			}
		}
	return result;
	}

/** @deprecated 7.0s */
EXPORT_C TBool MTmTextLayoutForwarder::GetCursor(const TTmDocPos& aDocPos,TTmCursorPlacement aPlacement,
		TTmLineInfo& aLineInfo,TPoint& aOrigin,TInt& aWidth,TInt& aAscent,TInt& aDescent) const
	{
	TBool result = TextLayout().GetCursor(aDocPos,aPlacement,aLineInfo,aOrigin,aWidth,aAscent,aDescent);
	if (result)
		{
		TPoint origin;
		GetOrigin(origin);
		if (origin.iX || origin.iY)
			{
			aOrigin += origin;
			FixUpLineInfo(aLineInfo,&origin);
			}
		}
	return result;
	}

void MTmTextLayoutForwarder::FixUpLineInfo(TTmLineInfo& aInfo,const TPoint* aOrigin) const
	{
	TPoint origin;
	if (aOrigin)
		origin = *aOrigin;
	else
		GetOrigin(origin);
	if (origin.iX || origin.iY)
		{
		aInfo.iInnerRect.Move(origin);
		aInfo.iOuterRect.Move(origin);
		aInfo.iBaseline += origin.iY;
		aInfo.iParTop += origin.iY;
		}
	}

EXPORT_C void MTmTextLayoutForwarder::GetOrigin(TPoint& aPoint) const
	{
	aPoint.iX = aPoint.iY = 0;
	}
