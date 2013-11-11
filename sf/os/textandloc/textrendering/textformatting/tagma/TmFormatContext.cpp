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


#include "TmLayoutImp.h"
#include "frmUtils.h"

CTmFormatContext::CTmFormatContext(MTmSource& aSource,const TTmFormatParam& aFormatParam,CTmCode& aCode,
								   CTmTextLayout* aTextLayout,const TBidirectionalContext* aBdContext):
	iSource(aSource),
	iParam(aFormatParam),
	iTextCache(aSource,aSource.FormatDevice()),
	iCode(aCode),
	iLineInPar(iParam.iLineInPar),
	iTextLayout(aTextLayout)
	{
	iInfo.iEndChar = iParam.iStartChar;
	iDocumentLength = iSource.DocumentLength();
	iActualEndChar = Min(iParam.iEndChar,iDocumentLength + 1);
	if (aBdContext)
		iBdState = *aBdContext;
	CalculateScale(iSource,iXScale,iYScale);
	}

CTmFormatContext::~CTmFormatContext()
	{
	iTextCache.Close();
	iParFormat.Close();
	}

void CTmFormatContext::ResolveParagraphDirectionality()
	{
	if (!(iParFormat.iFlags & RTmParFormat::EDirectionalityFromText))
		// Paragraph directionality is explicit.
		return;
	// iInfo.iEndChar is actually the start of this line, as the line has not been
	// set up yet!
	TInt positionOfLastStrongCategoryInPara = 0;
	TInt startPosOfThisLine = -1;
	TChar::TBdCategory firstStrongCategory = FirstStrongCategoryInParagraph(
		iSource.ParagraphStart(iInfo.iEndChar), iTextCache, positionOfLastStrongCategoryInPara, startPosOfThisLine);
	// Override the directionality set in ERightToLeft only if the text
	// gives us the information.
	if (firstStrongCategory < TChar::ERightToLeft)
		{
		iParFormat.iFlags &= ~RTmParFormat::ERightToLeft;
		iParFormat.iFlags &= ~RTmParFormat::EDirectionalityFromText;
		}
	else if (firstStrongCategory <= TChar::ERightToLeftOverride)
		{
		iParFormat.iFlags |= RTmParFormat::ERightToLeft;
		iParFormat.iFlags &= ~RTmParFormat::EDirectionalityFromText;
		}
	}

void CTmFormatContext::SetParagraphMetricsL()
	{
	iSource.GetParagraphFormatL(iInfo.iEndChar,iParFormat);
	ResolveParagraphDirectionality();

	int bullet_margin, bullet_width;
	MGraphicsDeviceMap& device = Device();
	TmGetMarginsL(device,iParFormat,iLeftMargin,iRightMargin,iFirstLineLeftMargin,iFirstLineRightMargin,
				  bullet_margin,bullet_width,iBulletAscent,iBulletDescent);

	iLineSpacing = iParFormat.iLineSpacing;
	if (iLineSpacing && (!(iParFormat.iFlags & RTmParFormat::EPixelLineSpacing)))
		iLineSpacing = device.VerticalTwipsToPixels(iLineSpacing);
	iExactLineSpacing = iParFormat.iFlags & RTmParFormat::EExactLineSpacing;
	iSpaceBefore = iParFormat.iSpaceAbove;
	if (iSpaceBefore)
		iSpaceBefore = device.VerticalTwipsToPixels(iSpaceBefore);
	iSpaceAfter = iParFormat.iSpaceBelow;
	if (iSpaceAfter)
		iSpaceAfter = device.VerticalTwipsToPixels(iSpaceAfter);

	if (iParFormat.HaveBorders())
		{
		int dummy;
		iHBorderMargin = device.HorizontalTwipsToPixels(iParFormat.iBorderMargin);
		iVBorderMargin = device.VerticalTwipsToPixels(iParFormat.iBorderMargin);
		TmGetBorderWidth(device,iParFormat,RTmParFormat::ETopBorder,dummy,iTopBorder);
		TmGetBorderWidth(device,iParFormat,RTmParFormat::EBottomBorder,dummy,iBottomBorder);
		TmGetBorderWidth(device,iParFormat,RTmParFormat::ELeadingBorder,dummy,iLeftBorder);
		TmGetBorderWidth(device,iParFormat,RTmParFormat::ETrailingBorder,dummy,iRightBorder);
		}
	else
		iLeftBorder = iRightBorder = iTopBorder = iBottomBorder = iHBorderMargin = iVBorderMargin = 0;
	}

// A static function to format a range of text.
void CTmFormatContext::FormatL(MTmSource& aSource,const TTmFormatParam& aFormatParam,CTmCode& aCode,TInfo& aInfo,
	CTmTextLayout* aTextLayout, const TBidirectionalContext* aStartBdState, TBidirectionalContext* aEndBdState,
	TBidirectionalEndOfLineContext* aStartBdContext)
	{
	CTmFormatContext *context = new(ELeave) CTmFormatContext(aSource,aFormatParam,aCode,aTextLayout,aStartBdState);
	
	//	Set the context obtained from the previous line from the byte code, if it exists. If not, then it will be just NULL
	context->iTextLayout->SetCurrentContextChar(aInfo.iContextCharPerLine);
	
	CleanupStack::PushL(context);
	if (aStartBdContext)
		aStartBdContext->Set(context->iTextCache, aFormatParam.iStartChar);
	context->FormatL();
	aInfo = context->iInfo;
	if (aEndBdState)
		aEndBdState->Set(context->iBdState, context->iBdEndOfLine);
	CleanupStack::PopAndDestroy(context);
	}

void CTmFormatContext::FormatL()
	{
	// Format lines until the text is used up.
	TBool at_least_max_height = iParam.iFlags & TTmFormatParamBase::EAtLeastMaxHeight;
	TBool too_high = FALSE;
	int lines = 0;
	iInfo.iFirstLineEndChar = -1;
	int margin_width = 0;

	while (iInfo.iEndChar < iActualEndChar && lines < iParam.iMaxLines && !too_high)
		{
		if (lines == 0 || iLineInPar == 0)
			{
			SetParagraphMetricsL();
			}
		
		if (iLineInPar == 0)
			{
			iBdState.Reset();
			iBdEndOfLine.Reset();
			}
		margin_width = iLineInPar == 0 ? iFirstLineLeftMargin + iFirstLineRightMargin : iLeftMargin + iRightMargin;
		// Set the context for this line obtained from the previous line before formatting this line		
		iLine.SetContextChar(iTextLayout->GetCurrentContextChar(), EFalse);
		iLine.FormatL(*this);
		// Set the context of this line once it has been formatted. This context is the context of the last chunk of this line,
		// so that the next line can continue in the same context.
		iLine.SetContextChar(0, ETrue);
		// Set the context of this line to CTmTextLayout to keep a record of it.
		iTextLayout->SetCurrentContextChar(iLine.GetContextChar());
		
		iInfo.iLastLineStartChar = iInfo.iEndChar;
		if (iInfo.iFirstLineEndChar == -1)
			iInfo.iFirstLineEndChar = iLine.NextLineStartChar();
		int scaled_next_height = iInfo.iHeight + iLine.Height();
		if (iYScale != 1000)
			scaled_next_height = scaled_next_height * iYScale / 1000;
		too_high = scaled_next_height > iParam.iMaxHeight;
		if (too_high && !at_least_max_height)
			break;
		iLine.WriteCodeL(*this);
		int width = iLine.InnerBounds().Width() + margin_width;
		if (width > iInfo.iWidth)
			iInfo.iWidth = width;
		iInfo.iHeight += iLine.Height();
		if (iLine.AtParEnd())
			iLineInPar = 0;
		else
			iLineInPar++;
		lines++;
		iInfo.iEndChar = iLine.NextLineStartChar();
		}

	// Scale the height and width if necessary.
	if (iYScale != 1000)
		iInfo.iHeight = iInfo.iHeight * iYScale / 1000;
	if (iXScale != 1000)
		iInfo.iWidth = iInfo.iWidth * iXScale / 1000;
	}

/*
Get the margins and bullet metrics. If there is a bullet and aFont is non-null, return a font
that may be used to draw the bullet.
*/
void TmGetMarginsL(MGraphicsDeviceMap& aDevice,const RTmParFormat& aParFormat,
				   TInt& aLeftMargin,TInt& aRightMargin,
				   TInt& aFirstLineLeftMargin,TInt& aFirstLineRightMargin,
				   TInt& aBulletMargin,TInt& aBulletWidth,
				   TInt& aBulletAscent,TInt& aBulletDescent,CFont** aFont)
	{
	int trailing_margin = aParFormat.iTrailingMargin;
	if (trailing_margin)
		trailing_margin = aDevice.HorizontalTwipsToPixels(trailing_margin);

	// Bullets change the leading margin and first-line indent.
	int leading_margin = aParFormat.iLeadingMargin;
	int indent = aParFormat.iFirstLineIndent;
	const TTmBullet* bullet = aParFormat.Bullet();
	if (bullet)
		{
		TFontSpec fontspec;
		bullet->iFontSpec.GetTFontSpec(fontspec);
		CFont* font = NULL;
		TInt error = aDevice.GetNearestFontInTwips(font,fontspec);
		if (!font)
			User::Leave(error);
		aBulletWidth = font->CharWidthInPixels(bullet->iCharacterCode);
		aBulletAscent = font->AscentInPixels();
		aBulletDescent = font->DescentInPixels();
		if (aFont)
			*aFont = font;
		else
			aDevice.ReleaseFont(font);
		int first_line_indent = leading_margin + indent;
		if (bullet->iHangingIndent)
			{
			aBulletMargin = aDevice.HorizontalTwipsToPixels(Min(leading_margin,first_line_indent));
			first_line_indent = leading_margin = Max(leading_margin,first_line_indent);
			}
		else
			aBulletMargin = aDevice.HorizontalTwipsToPixels(leading_margin);
		indent = first_line_indent - leading_margin;
		}
	else
		aBulletMargin = aBulletWidth = aBulletAscent = aBulletDescent = 0;
	
	if (leading_margin)
		leading_margin = aDevice.HorizontalTwipsToPixels(leading_margin);

	/*
	Indents are converted to pixels like this so that if the indent in twips is
	the negative of the leading margin, so as to get a zero indent for the first line, this
	is also true after conversion to pixels. Not doing this causes twip values of 360 and -360
	to become 30 and -29 pixels at standard Series 5 resolution.
	*/
	if (indent >= 0)
		indent = aDevice.HorizontalTwipsToPixels(indent);
	else if (indent < 0)
		indent = -aDevice.HorizontalTwipsToPixels(-indent);
	int bullet_overshoot = aBulletMargin + aBulletWidth - leading_margin - indent;
	if (bullet_overshoot > 0)
		indent += bullet_overshoot;

	if (aParFormat.RightToLeft())
		{
		aLeftMargin = trailing_margin;
		aRightMargin = leading_margin;
		aFirstLineLeftMargin = trailing_margin;
		aFirstLineRightMargin = leading_margin + indent;
		}
	else
		{
		aLeftMargin = leading_margin;
		aRightMargin = trailing_margin;
		aFirstLineLeftMargin = leading_margin + indent;
		aFirstLineRightMargin = trailing_margin;
		}
	}

void TmGetBorderWidth(const MGraphicsDeviceMap& aDevice,const RTmParFormat& aParFormat,RTmParFormat::TBorderIndex aSide,
					  TInt& aSingle,TInt& aFull)
	{
	aSingle = 0;
	const TTmParBorder* b = aParFormat.Border(aSide);
	if (b)
		{
		int width = b->iWeight;
		if (width)
			{
			if (aSide == RTmParFormat::ELeadingBorder || aSide == RTmParFormat::ETrailingBorder)
				width = aDevice.HorizontalTwipsToPixels(width);
			else
				width = aDevice.VerticalTwipsToPixels(width);
			if (width <= 0)
				width = 1;
			aSingle = width;
			if (b->iStyle == TTmParBorder::EDoubleStyle)
				aFull = width * 3;
			else
				aFull = width;
			}
		}
	}

// Subtract aQ from aP, giving four non-overlapping remainder rectangles aR, any or all of which may be empty.
void SubtractRect(const TRect& aP,const TRect& aQ,TRect* aR)
	{
	TRect q = aQ;
	q.Intersection(aP);
	if (q.IsEmpty())
		q.SetRect(aP.iTl,aP.iTl);
	aR[0] = aP;
	aR[0].iBr.iY = q.iTl.iY;
	aR[1] = aP;
	aR[1].iTl.iY = q.iBr.iY;
	aR[2] = q;
	aR[2].iTl.iX = aP.iTl.iX;
	aR[2].iBr.iX = q.iTl.iX;
	aR[3] = q;
	aR[3].iTl.iX = q.iBr.iX;
	aR[3].iBr.iX = aP.iBr.iX;
	}
