/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TmHighlightSource.h"
#include <txtfrmat.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TmHighlightSourceTraces.h"
#endif


// MTmSource derivatives that can draw text in the appropriate highlight colours

TTmHighlightSource::TTmHighlightSource(MTmSource& aDelegate)
	: iDelegate(aDelegate)
	{
	// Explicit construction of the 2nd parameter for SystemColor, TRgb, ensuring opaque alpha blending.
	// FF is opaque alpha channel value .
    TRgb defaultColorOfForeground(static_cast<TUint32>(TLogicalRgb::ESystemSelectionForegroundColor), 0xFF); 
   	iForeground = aDelegate.SystemColor(TLogicalRgb::ESystemSelectionForegroundIndex,defaultColorOfForeground);
   
   	// Explicit construction of the 2nd parameter for SystemColor, TRgb, ensuring opaque alpha blending.
   	// FF is opaque alpha channel value .
   	TRgb defaultColorOfBackground(static_cast<TUint32>(TLogicalRgb::ESystemSelectionBackgroundColor), 0xFF); 
    iBackground = aDelegate.SystemColor(TLogicalRgb::ESystemSelectionBackgroundIndex,defaultColorOfBackground);
    
    // Explicit construction of the 2nd parameter for SystemColor, TRgb, ensuring opaque alpha blending.
   	// FF is opaque alpha channel value .
    TRgb defaultColorOfXorPictures(static_cast<TUint32>(TLogicalRgb::ESystemBackgroundColor), 0xFF); 
    iXorPictures = aDelegate.SystemColor(TLogicalRgb::ESystemBackgroundIndex,defaultColorOfXorPictures);
    
	iXorPictures ^= iBackground;

	iTmTextDrawExt = static_cast <MTmTextDrawExt*> (iDelegate.GetExtendedInterface(KTmTextDrawExtId));
	if(!iTmTextDrawExt)
		{
		iTmTextDrawExt = &iTmTextDrawExtDefault;
		OstTrace0( TRACE_FATAL, TTMHIGHLIGHTSOURCE_TTMHIGHLIGHTSOURCE, "Invariant" );
		}
	__ASSERT_ALWAYS(iTmTextDrawExt, User::Invariant());

	}

void TTmHighlightSource::DrawBackground(CGraphicsContext& aGc,
	const TPoint&, const TRect& aRect,
	const TLogicalRgb&, TRect& aRectDrawn) const
	{
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(iBackground);
	iTmTextDrawExt->DrawRect(aGc, aRect);
	aRectDrawn = aRect;
	}

void TTmHighlightSource::DrawLineGraphics(CGraphicsContext&,
	const TPoint&, const TRect&, const TTmLineInfo&) const
	{
	// suppress line graphics for highlight
	}

void TTmHighlightSource::DrawText(CGraphicsContext& aGc,
	const TPoint& aTextLayoutTopLeft, const TRect& aRect,
	const TTmLineInfo& aLineInfo, const TTmCharFormat& aFormat,
	const TDesC& aText, const TInt aStart, const TInt aEnd, const TPoint& aTextOrigin, TInt aExtraPixels) const
	{
	SetPenColor(aGc, iForeground);
	// If a custom interface to draw the text in context exists, then use it to call the appropriate DrawText method.
	MTmCustomExtension* extensionInterface = reinterpret_cast <MTmCustomExtension*> (iDelegate.GetExtendedInterface(KTmCustomExtensionUid));
	if (extensionInterface)
		{
		extensionInterface->DrawText(aGc, aTextLayoutTopLeft, aRect, aLineInfo,
				aFormat, aText,aStart,aEnd, aTextOrigin, aExtraPixels);
		}
		else
			{
			iDelegate.DrawText(aGc, aTextLayoutTopLeft, aRect, aLineInfo,
				aFormat, aText.Mid(aStart,aEnd - aStart + 1), aTextOrigin, aExtraPixels);
			}
	
	}

void TTmHighlightSource::DrawPicture(CGraphicsContext& aGc,
	const TPoint& aTopLeft, const TRect& aRect,
	MGraphicsDeviceMap& aDevice, const CPicture& aPicture) const
	{
	// draw the picture
	aGc.SetClippingRect(aRect);
	iDelegate.DrawPicture(aGc, aTopLeft, aRect, aDevice, aPicture);
	// invert it
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	SetBrushColor(aGc, iXorPictures);
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.SetDrawMode(CGraphicsContext::EDrawModeXOR);
	iTmTextDrawExt->DrawRect(aGc, aRect);
	}

MGraphicsDeviceMap& TTmHighlightSource::FormatDevice() const
	{
	return iDelegate.FormatDevice();
	}

MGraphicsDeviceMap& TTmHighlightSource::InterpretDevice() const
	{
	return iDelegate.InterpretDevice();
	}

TInt TTmHighlightSource::DocumentLength() const
	{
	return iDelegate.DocumentLength();
	}

void TTmHighlightSource::GetText(TInt aPos, TPtrC& aText,
	TTmCharFormat& aFormat) const
	{
	iDelegate.GetText(aPos, aText, aFormat);
	}

void TTmHighlightSource::GetParagraphFormatL(TInt aPos, RTmParFormat& aFormat) const
	{
	iDelegate.GetParagraphFormatL(aPos, aFormat);
	}

TInt TTmHighlightSource::ParagraphStart(TInt aPos) const
	{
	return iDelegate.ParagraphStart(aPos);
	}

CPicture* TTmHighlightSource::PictureL(TInt aPos) const
	{
	return iDelegate.PictureL(aPos);
	}

TInt TTmHighlightSource::GetPictureSizeInTwipsL(TInt aPos, TSize& aSize) const
	{
	return iDelegate.GetPictureSizeInTwipsL(aPos, aSize);
	}

TBool TTmHighlightSource::PageBreakInRange(TInt aStartPos, TInt aEndPos) const
	{
	return iDelegate.PageBreakInRange(aStartPos, aEndPos);
	}

TAny* TTmHighlightSource::GetExtendedInterface(const TUid& aInterfaceId)
	{
	return iDelegate.GetExtendedInterface(aInterfaceId);
	}

