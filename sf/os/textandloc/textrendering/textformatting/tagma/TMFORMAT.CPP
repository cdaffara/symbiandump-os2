/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Character format, paragraph format and style classes. Many member functions convert to and from the legacy format
* classes exported by ETEXT.
*
*/


#include "TAGMA.H"
#include <txtstyle.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

EXPORT_C TTmCharFormatMask::TTmCharFormatMask(const TCharFormatMask& aMask):
	iFlags(0)
	{				   
	if (aMask.AttribIsSet(EAttFontTypeface))
		iFlags |= (EFontName | EFontCoverage | ESerif | EMonoWidth);
	if (aMask.AttribIsSet(EAttFontStrokeWeight))
		iFlags |= EBold;
	if (aMask.AttribIsSet(EAttFontPosture))
		iFlags |= EItalic;
	if (aMask.AttribIsSet(EAttFontHeight))
		iFlags |= EHeight;
	if (aMask.AttribIsSet(EAttFontPrintPos))
		iFlags |= EPrintPosition;
	if (aMask.AttribIsSet(EAttColor))
		iFlags |= ETextColor;
	if (aMask.AttribIsSet(EAttFontHighlightColor))
		iFlags |= EBackgroundColor;
	if (aMask.AttribIsSet(EAttFontHighlightStyle))
		iFlags |= (EBackground | EShadow | EUserDefinedEffects);
	if (aMask.AttribIsSet(EAttFontUnderline))
		iFlags |= EUnderline;
	if (aMask.AttribIsSet(EAttFontStrikethrough))
		iFlags |= EStrikethrough;
	if (aMask.AttribIsSet(EAttCharLanguage))
		iFlags |= ELanguage;
	if (aMask.AttribIsSet(EAttFontPictureAlignment))
		iFlags |= EPictureAlignment;
	}

EXPORT_C void TTmCharFormatMask::GetTCharFormatMask(TCharFormatMask& aMask) const
	{
	aMask.ClearAll();
	if (iFlags & (EFontName | EFontCoverage | ESerif | EMonoWidth))
		aMask.SetAttrib(EAttFontTypeface);
	if (iFlags & EBold)
		aMask.SetAttrib(EAttFontStrokeWeight);
	if (iFlags & EItalic)
		aMask.SetAttrib(EAttFontPosture);
	if (iFlags & EHeight)
		aMask.SetAttrib(EAttFontHeight);
	if (iFlags & EPrintPosition)
		aMask.SetAttrib(EAttFontPrintPos);
	if (iFlags & ETextColor)
		aMask.SetAttrib(EAttColor);
	if (iFlags & EBackgroundColor)
		aMask.SetAttrib(EAttFontHighlightColor);
	if (iFlags & (EBackground | EShadow | EUserDefinedEffects))
		aMask.SetAttrib(EAttFontHighlightStyle);
	if (iFlags & EUnderline)
		aMask.SetAttrib(EAttFontUnderline);
	if (iFlags & EStrikethrough)
		aMask.SetAttrib(EAttFontStrikethrough);
	if (iFlags & ELanguage)
		aMask.SetAttrib(EAttCharLanguage);
	if (iFlags & EPictureAlignment)
		aMask.SetAttrib(EAttFontPictureAlignment);
	}

EXPORT_C TTmParFormatMask::TTmParFormatMask(const TParaFormatMask& aMask):
	iFlags(0)
	{
	if (aMask.AttribIsSet(EAttAlignment))
		iFlags |= EAlignment;
	if (aMask.AttribIsSet(EAttKeepTogether))
		iFlags |= EKeepTogether;
	if (aMask.AttribIsSet(EAttKeepWithNext))
		iFlags |= EKeepWithNext;
	if (aMask.AttribIsSet(EAttStartNewPage))
		iFlags |= EStartNewPage;
	if (aMask.AttribIsSet(EAttWidowOrphan))
		iFlags |= EWidowOrphan;
	if (aMask.AttribIsSet(EAttWrap))
		iFlags |= ENoWrap;
	if (aMask.AttribIsSet(EAttLineSpacingControl))
		iFlags |= (EExactLineSpacing | EPixelLineSpacing);
	if (aMask.AttribIsSet(EAttLeftMargin))
		iFlags |= ELeadingMargin;
	if (aMask.AttribIsSet(EAttRightMargin))
		iFlags |= ETrailingMargin;
	if (aMask.AttribIsSet(EAttIndent))
		iFlags |= EFirstLineIndent;
	if (aMask.AttribIsSet(EAttLineSpacing))
		iFlags |= ELineSpacing;
	if (aMask.AttribIsSet(EAttSpaceBefore))
		iFlags |= ESpaceAbove;
	if (aMask.AttribIsSet(EAttSpaceAfter))
		iFlags |= ESpaceBelow;
	if (aMask.AttribIsSet(EAttDefaultTabWidth))
		iFlags |= ETabSize;
	if (aMask.AttribIsSet(EAttBorderMargin))
		iFlags |= EBorderMargin;
	if (aMask.AttribIsSet(EAttTabStop))
		iFlags |= ETabList;
	if (aMask.AttribIsSet(EAttBullet))
		iFlags |= EBullet;
	if (aMask.AttribIsSet(EAttTopBorder) ||
		aMask.AttribIsSet(EAttBottomBorder) ||
		aMask.AttribIsSet(EAttLeftBorder) ||
		aMask.AttribIsSet(EAttRightBorder))
		iFlags |= EBorder;
	if (aMask.AttribIsSet(EAttParaLanguage))
		iFlags |= EDirection;
	}

EXPORT_C void TTmParFormatMask::GetTParaFormatMask(TParaFormatMask& aMask) const
	{
	aMask.ClearAll();
	if (iFlags & EAlignment)
		aMask.SetAttrib(EAttAlignment);
	if (iFlags & EKeepTogether)
		aMask.SetAttrib(EAttKeepTogether);
	if (iFlags & EKeepWithNext)
		aMask.SetAttrib(EAttKeepWithNext);
	if (iFlags & EStartNewPage)
		aMask.SetAttrib(EAttStartNewPage);
	if (iFlags & EWidowOrphan)
		aMask.SetAttrib(EAttWidowOrphan);
	if (iFlags & ENoWrap)
		aMask.SetAttrib(EAttWrap);
	if (iFlags & (EExactLineSpacing | EPixelLineSpacing))
		aMask.SetAttrib(EAttLineSpacingControl);
	if (iFlags & ELeadingMargin)
		aMask.SetAttrib(EAttLeftMargin);
	if (iFlags & ETrailingMargin)
		aMask.SetAttrib(EAttRightMargin);
	if (iFlags & EFirstLineIndent)
		aMask.SetAttrib(EAttIndent);
	if (iFlags & ELineSpacing)
		aMask.SetAttrib(EAttLineSpacing);
	if (iFlags & ESpaceAbove)
		aMask.SetAttrib(EAttSpaceBefore);
	if (iFlags & ESpaceBelow)
		aMask.SetAttrib(EAttSpaceAfter);
	if (iFlags & ETabSize)
		aMask.SetAttrib(EAttDefaultTabWidth);
	if (iFlags & EBorderMargin)
		aMask.SetAttrib(EAttBorderMargin);
	if (iFlags & ETabList)
		aMask.SetAttrib(EAttTabStop);
	if (iFlags & EBullet)
		aMask.SetAttrib(EAttBullet);
	if (iFlags & EBorder)
		{
		aMask.SetAttrib(EAttTopBorder);
		aMask.SetAttrib(EAttBottomBorder);
		aMask.SetAttrib(EAttLeftBorder);
		aMask.SetAttrib(EAttRightBorder);
		}
	if (iFlags & EDirection)
		aMask.SetAttrib(EAttParaLanguage);
	}

EXPORT_C TTmCharFormat::TTmCharFormat():
	iTextColor(TLogicalRgb::ESystemForegroundColor),
	iBackgroundColor(TLogicalRgb::ESystemBackgroundColor),
	iEffects(0),
	iLanguage(0),
	iPictureAlignment(EPictureAlignBaseline),
	iTag(0)
	{
	}

EXPORT_C TTmCharFormat::TTmCharFormat(const TDesC& aFontName,TInt aFontHeight):
	iTextColor(TLogicalRgb::ESystemForegroundColor),
	iBackgroundColor(TLogicalRgb::ESystemBackgroundColor),
	iEffects(0),
	iLanguage(0),
	iPictureAlignment(EPictureAlignBaseline),
	iTag(0)
	{
	iFontSpec.SetName(aFontName);
	iFontSpec.SetHeight(aFontHeight);
	}
	
EXPORT_C void TTmCharFormat::operator=(const TCharFormat& aFormat)
	{
	iFontSpec = aFormat.iFontSpec;
	iTextColor = aFormat.iFontPresentation.iTextColor;
	iBackgroundColor = aFormat.iFontPresentation.iHighlightColor;
	iEffects = 0;
	iLanguage = aFormat.iLanguage;
	iTag = 0;
	TUint style = aFormat.iFontPresentation.iHighlightStyle;
	switch (style)
		{
		case TFontPresentation::EFontHighlightNormal: iEffects = EBackground; break;
		case TFontPresentation::EFontHighlightRounded: iEffects = EBackground | ERounded; break;
		case TFontPresentation::EFontHighlightShadow: iEffects = EShadow; break;
		default:
			if (style >= 128
				|| style == TFontPresentation::EFontHighlightNoMatchesIndicator)
				iEffects = style << 24;
			break;
		}
	if (aFormat.iFontPresentation.iUnderline == EUnderlineOn)
		iEffects |= EUnderline;
	if (aFormat.iFontPresentation.iStrikethrough == EStrikethroughOn)
		iEffects |= EStrikethrough;
	switch (aFormat.iFontPresentation.iPictureAlignment)
		{
		case TFontPresentation::EAlignTop: iPictureAlignment = EPictureAlignTop; break;
		case TFontPresentation::EAlignBottom: iPictureAlignment = EPictureAlignBottom; break;
		case TFontPresentation::EAlignCentered: iPictureAlignment = EPictureAlignCenter; break;
		case TFontPresentation::EAlignBaseLine: iPictureAlignment = EPictureAlignBaseline; break;
		default: /*+ panic? */ break;
		}
	}

EXPORT_C void TTmCharFormat::GetTCharFormat( TCharFormat& aFormat ) const
	{
	iFontSpec.GetTFontSpec( aFormat.iFontSpec );
	aFormat.iFontPresentation.iTextColor = iTextColor;
	aFormat.iFontPresentation.iHighlightColor = iBackgroundColor;
	aFormat.iLanguage = iLanguage;
	if ( iEffects & EBackground )
		{
		aFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightNormal;
		if ( iEffects & ERounded )
			aFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightRounded;
		}
	else if ( iEffects & EShadow )
		aFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightShadow;
	else if ( iEffects & static_cast<TUint>( EUserDefinedMask ) )
		aFormat.iFontPresentation.iHighlightStyle = static_cast<TFontPresentation::TFontHighlightStyle>( iEffects >> EUserDefinedShift );
	aFormat.iFontPresentation.iUnderline = ( iEffects & EUnderline ) ? EUnderlineOn : EUnderlineOff;
	aFormat.iFontPresentation.iStrikethrough = ( iEffects & EStrikethrough ) ? EStrikethroughOn : EStrikethroughOff;
	switch ( iPictureAlignment )
		{
		case EPictureAlignTop: aFormat.iFontPresentation.iPictureAlignment = TFontPresentation::EAlignTop; break;
		case EPictureAlignBottom: aFormat.iFontPresentation.iPictureAlignment = TFontPresentation::EAlignBottom; break;
		case EPictureAlignCenter: aFormat.iFontPresentation.iPictureAlignment = TFontPresentation::EAlignCentered; break;
		case EPictureAlignBaseline: aFormat.iFontPresentation.iPictureAlignment = TFontPresentation::EAlignBaseLine; break;
		default: /*+ panic? */ break;
		}
	}

EXPORT_C TBool TTmCharFormat::operator==(const TTmCharFormat& aFormat) const
	{
	return iTextColor == aFormat.iTextColor &&
		   iBackgroundColor == aFormat.iBackgroundColor &&
		   iEffects == aFormat.iEffects &&
		   iLanguage == aFormat.iLanguage &&
		   iPictureAlignment == aFormat.iPictureAlignment &&
		   iTag == aFormat.iTag &&
		   iFontSpec == aFormat.iFontSpec;
	}

/**
The equality operator. Return TRUE if this object and aTab are identical.
*/
EXPORT_C TBool TTmTab::operator==(const TTmTab& aTab) const
	{
	return iPosition == aTab.iPosition && iType == aTab.iType;
	}

EXPORT_C void TTmTab::operator=(const TTabStop& aTab)
	{
	iPosition = aTab.iTwipsPosition;
	switch (aTab.iType)
		{
		case TTabStop::ELeftTab: iType = EStandardTab; break;
		case TTabStop::ECenteredTab: iType = ECenterTab; break;
		case TTabStop::ERightTab: iType = EReverseTab; break;
		default: iType = EStandardTab; break;
		}
	}

EXPORT_C void TTmTab::GetTTabStop(TTabStop& aTab) const
	{
	aTab.iTwipsPosition = iPosition;
	switch (iType)
		{
		case EStandardTab: aTab.iType = TTabStop::ELeftTab; break;
		case ECenterTab: aTab.iType = TTabStop::ECenteredTab; break;
		case EReverseTab: aTab.iType = TTabStop::ERightTab; break;
		default: /* panic? */; break;
		}
	}

EXPORT_C TTmBullet::TTmBullet():
	iCharacterCode(0x2022),
	iHangingIndent(TRUE),
	iColor(TLogicalRgb::ESystemForegroundColor),
	iStyle(EBulletStyle),
	iStartNumber(1),
	iAlignment(ELeftAlign)
	{
	}

EXPORT_C void TTmBullet::operator=(const TBullet& aBullet)
	{
	iCharacterCode = aBullet.iCharacterCode;
	TFontSpec fs;
	fs.iTypeface = aBullet.iTypeface;
	fs.iHeight = aBullet.iHeightInTwips;
	iFontSpec = fs;
	iHangingIndent = aBullet.iHangingIndent;
	iColor = aBullet.iColor;
	if (aBullet.iStyle == TBullet::ENullStyle)
		iStyle = EBulletStyle;
	else
		iStyle = (TStyle)(aBullet.iStyle - 1);			// depends on values in TBullet::TStyle
	iStartNumber = aBullet.iStartNumber;
	iAlignment = (TAlignment)aBullet.iAlignment;		// depends on values in TBullet::TAlignment
	}

EXPORT_C void TTmBullet::GetTBullet(TBullet& aBullet) const
	{
	aBullet.iCharacterCode = iCharacterCode;
	TFontSpec fs;
	iFontSpec.GetTFontSpec(fs);
	aBullet.iTypeface = fs.iTypeface;
	aBullet.iHeightInTwips = fs.iHeight;
	aBullet.iHangingIndent = iHangingIndent;
	aBullet.iColor = iColor;
	aBullet.iStyle = (TBullet::TStyle)(iStyle + 1);			// depends on values in TBullet::TStyle
	aBullet.iStartNumber = iStartNumber;
	aBullet.iAlignment = (TBullet::TAlignment)iAlignment;	// depends on values in TBullet::TAlignment
	}

EXPORT_C TBool TTmBullet::operator==(const TTmBullet& aBullet) const
	{
	return iCharacterCode == aBullet.iCharacterCode &&
		   iHangingIndent == aBullet.iHangingIndent &&
		   iColor == aBullet.iColor &&
		   iStyle == aBullet.iStyle &&
		   iStartNumber == aBullet.iStartNumber &&
		   iAlignment == aBullet.iAlignment &&
		   iFontSpec == aBullet.iFontSpec;
	}

EXPORT_C TTmParBorder::TTmParBorder():
	iStyle(ESolidStyle),
	iWeight(10), // 0.5pt
	iColor(TLogicalRgb::ESystemForegroundColor),
	iAutoColor(TRUE)
	{
	}

EXPORT_C void TTmParBorder::operator=(const TParaBorder& aBorder)
	{
	if (aBorder.iLineStyle == TParaBorder::ENullLineStyle)
		iStyle = ESolidStyle;
	else
		iStyle = (TStyle)(aBorder.iLineStyle - 1);	// depends on values in TParaBorder::TLineStyle
	iWeight = aBorder.iThickness;
	iColor = aBorder.iColor;
	iAutoColor = aBorder.iAutoColor;
	}

EXPORT_C void TTmParBorder::GetTParaBorder(TParaBorder& aBorder) const
	{
	aBorder.iLineStyle = (TParaBorder::TLineStyle)(iStyle + 1);	// depends on values in TParaBorder::TLineStyle
	aBorder.iThickness = iWeight;
	aBorder.iColor = iColor;
	aBorder.iAutoColor = iAutoColor;
	}

EXPORT_C TBool TTmParBorder::operator==(const TTmParBorder& aBorder) const
	{
	return iStyle == aBorder.iStyle &&
		   iWeight == aBorder.iWeight &&
		   iColor == aBorder.iColor &&
		   iAutoColor == aBorder.iAutoColor;
	}

EXPORT_C RTmParFormat::RTmParFormat():
	iAlignment(EAlignNormalBidirectional),
	iFlags(0),
	iLeadingMargin(0),
	iTrailingMargin(0),
	iFirstLineIndent(0),
	iLineSpacing(0),
	iSpaceAbove(0),
	iSpaceBelow(0),
	iTabSize(360),
	iBorderMargin(0),
	iBackgroundColor(TLogicalRgb::ESystemBackgroundColor),
	iTabList(NULL),
	iBullet(NULL)
	{
	for (int i = 0; i < 4; i++)
		iBorder[i] = NULL;
	}

EXPORT_C void RTmParFormat::Close()
	{
	if (iTabList)
		{
		iTabList->Close();
		delete iTabList;
		iTabList = NULL;
		}
	delete iBullet;
	iBullet = NULL;
	for (int i = 0; i < 4; i++)
		{
		delete iBorder[i];
		iBorder[i] = NULL;
		}
	}

EXPORT_C void RTmParFormat::CopyL(const RTmParFormat& aFormat)
	{
	iAlignment = aFormat.iAlignment;
	iFlags = aFormat.iFlags;
	iLeadingMargin = aFormat.iLeadingMargin;
	iTrailingMargin = aFormat.iTrailingMargin;
	iFirstLineIndent = aFormat.iFirstLineIndent;
	iLineSpacing = aFormat.iLineSpacing;
	iSpaceAbove = aFormat.iSpaceAbove;
	iSpaceBelow = aFormat.iSpaceBelow;
	iTabSize = aFormat.iTabSize;
	iBorderMargin = aFormat.iBorderMargin;
	iBackgroundColor = aFormat.iBackgroundColor;
	int i;
	if (aFormat.iTabList)
		{
		if (!iTabList)
			iTabList = new(ELeave) RArray<TTmTab>;
		else
			iTabList->Reset();
		for (i = 0; i < aFormat.iTabList->Count(); i++)
			User::LeaveIfError(iTabList->Append((*aFormat.iTabList)[i]));
		}
	else if (iTabList)
		{
		iTabList->Close();
		delete iTabList;
		iTabList = NULL;
		}
	if (aFormat.iBullet)
		{
		if (!iBullet)
			iBullet = new(ELeave) TTmBullet;
		*iBullet = *aFormat.iBullet;
		}
	else if (iBullet)
		{
		delete iBullet;
		iBullet = NULL;
		}
	for (i = 0; i < 4; i++)
		{
		if (aFormat.iBorder[i])
			{
			if (!iBorder[i])
				iBorder[i] = new(ELeave) TTmParBorder;
			*iBorder[i] = *aFormat.iBorder[i];
			}
		else if (iBorder[i])
			{
			delete iBorder[i];
			iBorder[i] = NULL;
			}
		}
	}

EXPORT_C void RTmParFormat::CopyL(const CParaFormat& aFormat)
	{
	switch (aFormat.iHorizontalAlignment)
		{
		case CParaFormat::ELeftAlign: iAlignment = EAlignNormal; break;
		case CParaFormat::ECenterAlign: iAlignment = EAlignCenter; break;
		case CParaFormat::ERightAlign: iAlignment = EAlignReverse; break;
		case CParaFormat::EJustifiedAlign: iAlignment = EAlignJustify; break;
		case CParaFormat::EAbsoluteLeftAlign: iAlignment = EAlignAbsoluteLeft; break;
		case CParaFormat::EAbsoluteRightAlign: iAlignment = EAlignAbsoluteRight; break;
		default: iAlignment = EAlignNormalBidirectional; break;
		}

	iFlags = EDirectionalityFromText;
	if (aFormat.iKeepTogether)
		iFlags |= EKeepTogether;
	if (aFormat.iKeepWithNext)
		iFlags |= EKeepWithNext;
	if (aFormat.iStartNewPage)
		iFlags |= EStartNewPage;
	if (aFormat.iWidowOrphan)
		iFlags |= EWidowOrphan;
	if (!aFormat.iWrap)
		iFlags |= ENoWrap;
	if (aFormat.iLanguage == ELangArabic || aFormat.iLanguage == ELangHebrew
		|| aFormat.iLanguage == ELangFarsi || aFormat.iLanguage == ELangUrdu)
		iFlags |= ERightToLeft;

	switch (aFormat.iLineSpacingControl)
		{
		case CParaFormat::ELineSpacingExactlyInTwips: iFlags |= EExactLineSpacing; break;
		case CParaFormat::ELineSpacingAtLeastInPixels: iFlags |= EPixelLineSpacing; break;
		case CParaFormat::ELineSpacingExactlyInPixels: iFlags |= (EExactLineSpacing | EPixelLineSpacing); break;
		default: break;
		}

	iLeadingMargin = aFormat.iLeftMarginInTwips;
	iTrailingMargin = aFormat.iRightMarginInTwips;
	iFirstLineIndent = aFormat.iIndentInTwips;
	iLineSpacing = aFormat.iLineSpacingInTwips;
	iSpaceAbove = aFormat.iSpaceBeforeInTwips;
	iSpaceBelow = aFormat.iSpaceAfterInTwips;
	iTabSize = aFormat.iDefaultTabWidthInTwips;
	iBorderMargin = aFormat.iBorderMarginInTwips;
	iBackgroundColor = aFormat.iFillColor;

	int i = 0;
	int tabs = aFormat.TabCount();
	if (aFormat.TabCount())
		{
		if (!iTabList)
			iTabList = new(ELeave) RArray<TTmTab>;
		else
			iTabList->Reset();
		for (i = 0; i < tabs; i++)
			{
			TTmTab tab(aFormat.TabStop(i));
			User::LeaveIfError(iTabList->Append(tab));
			}
		}
	else if (iTabList)
		{
		iTabList->Close();
		delete iTabList;
		iTabList = NULL;
		}
	if (aFormat.iBullet && aFormat.iBullet->iStyle != TBullet::ENullStyle)
		{
		if (!iBullet)
			iBullet = new(ELeave) TTmBullet;
		*iBullet = *aFormat.iBullet;
		}
	else if (iBullet)
		{
		delete iBullet;
		iBullet = NULL;
		}
	for (i = 0; i < 4; i++) // relies on border indices being the same in RTmParFormat and CParaFormat
		{
		const TParaBorder* b = CONST_CAST(CParaFormat&,aFormat).ParaBorderPtr((CParaFormat::TParaBorderSide)i);
		if (b && b->iLineStyle != TParaBorder::ENullLineStyle)
			{
			if (!iBorder[i])
				iBorder[i] = new(ELeave) TTmParBorder;
			*iBorder[i] = *b;
			}
		else if (iBorder[i])
			{
			delete iBorder[i];
			iBorder[i] = NULL;
			}
		}
	}

EXPORT_C void RTmParFormat::GetCParaFormatL(CParaFormat& aFormat) const
	{
	aFormat.iVerticalAlignment = CParaFormat::ETopAlign;
	switch (iAlignment)
		{
		case EAlignNormal: aFormat.iHorizontalAlignment = CParaFormat::ELeftAlign; break;		// to maintain sc
		case EAlignCenter: aFormat.iHorizontalAlignment = CParaFormat::ECenterAlign; break;
		case EAlignReverse: aFormat.iHorizontalAlignment = CParaFormat::ERightAlign; break;		// to maintain sc
		case EAlignJustify: aFormat.iHorizontalAlignment = CParaFormat::EJustifiedAlign; break;
		case EAlignAbsoluteLeft: aFormat.iHorizontalAlignment = CParaFormat::EAbsoluteLeftAlign; break;
		case EAlignAbsoluteRight: aFormat.iHorizontalAlignment = CParaFormat::EAbsoluteRightAlign; break;
		default: /* panic? */ break;
		}

	aFormat.iLanguage = iFlags & ERightToLeft? ELangArabic : 0;
	aFormat.iKeepTogether = (iFlags & EKeepTogether) != 0;
	aFormat.iKeepWithNext = (iFlags & EKeepWithNext) != 0;
	aFormat.iStartNewPage = (iFlags & EStartNewPage) != 0;
	aFormat.iWidowOrphan = (iFlags & EWidowOrphan) != 0;
	aFormat.iWrap = !(iFlags & ENoWrap);

	switch (iFlags & (EExactLineSpacing | EPixelLineSpacing))
		{
		case 0: aFormat.iLineSpacingControl = CParaFormat::ELineSpacingAtLeastInTwips; break;
		case EExactLineSpacing: aFormat.iLineSpacingControl = CParaFormat::ELineSpacingExactlyInTwips; break;
		case EPixelLineSpacing: aFormat.iLineSpacingControl = CParaFormat::ELineSpacingAtLeastInPixels; break;
		case EExactLineSpacing | EPixelLineSpacing: aFormat.iLineSpacingControl = CParaFormat::ELineSpacingExactlyInPixels; break;
		};

	aFormat.iLeftMarginInTwips = iLeadingMargin;
	aFormat.iRightMarginInTwips = iTrailingMargin;
	aFormat.iIndentInTwips = iFirstLineIndent;
	aFormat.iLineSpacingInTwips = iLineSpacing;
	aFormat.iSpaceBeforeInTwips = iSpaceAbove;
	aFormat.iSpaceAfterInTwips = iSpaceBelow;
	aFormat.iDefaultTabWidthInTwips = iTabSize;
	aFormat.iBorderMarginInTwips = iBorderMargin;
	aFormat.iFillColor = iBackgroundColor;

	int i = 0;
	aFormat.Strip();
	if (iTabList)
		{
		int tabs = iTabList->Count();
		for (i = 0; i < tabs; i++)
			{
			TTabStop tab;
			(*iTabList)[i].GetTTabStop(tab);
			aFormat.StoreTabL(tab);
			}
		}
	if (iBullet)
		{
		aFormat.iBullet = new(ELeave) TBullet;
		iBullet->GetTBullet(*aFormat.iBullet);
		}
	for (i = 0; i < 4; i++) // relies on border indices being the same in RTmParFormat and CParaFormat
		{
		TParaBorder border;
		if (iBorder[i])
			{
			iBorder[i]->GetTParaBorder(border);
			aFormat.SetParaBorderL((CParaFormat::TParaBorderSide)i,border);
			}
		}
	}

EXPORT_C TBool RTmParFormat::operator==(const RTmParFormat& aFormat) const
	{
	if (iAlignment != aFormat.iAlignment ||
		iFlags != aFormat.iFlags ||
		iLeadingMargin != aFormat.iLeadingMargin ||
		iTrailingMargin != aFormat.iTrailingMargin ||
		iFirstLineIndent != aFormat.iFirstLineIndent ||
		iLineSpacing != aFormat.iLineSpacing ||
		iSpaceAbove != aFormat.iSpaceAbove ||
		iSpaceBelow != aFormat.iSpaceBelow ||
		iTabSize != aFormat.iTabSize ||
		iBorderMargin != aFormat.iBorderMargin ||
		iBackgroundColor != aFormat.iBackgroundColor)
		return FALSE;

	int tabs = Tabs();
	if (tabs != aFormat.Tabs())
		return FALSE;
	int i;
	for (i = 0; i < tabs; i++)
		if ((*iTabList)[i] != (*aFormat.iTabList)[i])
			return FALSE;

	TBool have_bullet = iBullet != NULL;
	if (have_bullet != (aFormat.iBullet != NULL))
		return FALSE;
	if (have_bullet && *iBullet != *aFormat.iBullet)
		return FALSE;

	for (i = 0; i < 4; i++)
		{
		TBool have_border = iBorder[i] != NULL;
		if (have_border != (aFormat.iBorder[i] != NULL))
			return FALSE;
		if (have_border && *iBorder[i] != *aFormat.iBorder[i])
			return FALSE;
		}

	return TRUE;
	}

/**
Return the number of tab stops stored in the object.
*/
EXPORT_C TInt RTmParFormat::Tabs() const
	{
	return iTabList ? iTabList->Count() : 0;
	}

/**
Return TRUE if the paragraph has any borders.
*/
EXPORT_C TBool RTmParFormat::HaveBorders() const
	{
	return iBorder[0] || iBorder[1] || iBorder[2] || iBorder[3];
	}

EXPORT_C void RTmStyle::CopyL(const RParagraphStyleInfo& aStyle)
	{
	iName = aStyle.iStyle->iName;
	iNextStyleName = aStyle.iStyleForNextPara->iName;
	iCharFormat = *aStyle.iStyle->CharFormatLayer();
	iParFormat.CopyL(*aStyle.iStyle);
	iOutlineLevel = aStyle.iStyle->OutlineLevel();
	}

EXPORT_C void RTmStyle::GetRParagraphStyleInfoL(RParagraphStyleInfo& aStyle,
		const CParaFormatLayer& aGlobalParaFormatLayer,const CCharFormatLayer& aGlobalCharFormatLayer,
		const CStyleList& aList) const
	{
	delete aStyle.iStyle;
	aStyle.iStyle = NULL;
	aStyle.iStyle = CParagraphStyle::NewL(aGlobalParaFormatLayer,aGlobalCharFormatLayer);
	aStyle.iStyle->iName = iName;
	aStyle.iStyleForNextPara = aList.PtrByName(iNextStyleName)->iStyle;
	
	}

EXPORT_C TTmCharFormatLayer::TTmCharFormatLayer(const CCharFormatLayer& aFormat)
	{
	TCharFormat format;
	TCharFormatMask mask; 
	aFormat.Sense(format,mask);
	iFormat = format;
	iMask = mask;
	}

EXPORT_C void RTmParFormatLayer::CopyL(const CParaFormatLayer& aFormat)
	{
	CParaFormat* format = new(ELeave) CParaFormat;
	CleanupStack::PushL(format);
	TParaFormatMask mask;
	aFormat.SenseL(format,mask);
	iFormat.CopyL(*format);
	iMask = mask;
	CleanupStack::PopAndDestroy(format);
	}
