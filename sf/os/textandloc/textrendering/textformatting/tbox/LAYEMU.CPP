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
* CTextLayout implementation using the TAGMA text formatting system.
*
*/


#include <txtlaydc.h>
#include "FRMTLAY.H"
#include "FRMCONST.H"
#include "FORMUTIL.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "FRMCONST_INTERNAL.H"
#include "FRMCONST_PARTNER.H"
#include "TAGMA_INTERNAL.H"
#include "FRMTLAY_INTERNAL.H"
#endif

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "LAYEMUTraces.h"
#endif

const TInt KMaxExtraLines = 10; // maximum number of lines to format after the current and following lines
								// before using background formatting
// during page down/up, the number of lines that should remain visible after
// the scroll, counting from the first line that is not completely visible.
// i.e. setting this to 0 means always scroll KMaxProportionOfScreenToScroll.
// setting it to 1 means make sure that no lines are ever both partially off the
// top/bottom before the scroll and partially visible off the bottom/top after
// the scroll.
// setting it to any number higher means keep that number -1 lines completely
// visible before and after the scroll.
const TInt KNumberOfLinesToKeepVisibleDuringScroll = 2;
/** maximum scroll proportions in thousandths. This overrides
 the KNumberOfLinesToKeepVisibleDuringScroll constant.*/
const TInt KMaxProportionOfScreenToScroll = 1000;
/** minimum scroll proportions in thousandths. This overrides
 the KNumberOfLinesToKeepVisibleDuringScroll constant.*/
const TInt KMinProportionOfScreenToScroll = 600;


/**
Constructs an iterator over the text referenced by aSource.
@param aSource The source of the text to be iterated over.
*/
CTextLayout::TUtf32SourceCache::TUtf32SourceCache(const MTmSource& aSource)
	: iSource(&aSource), iCurrentViewIndex(-1)
	{
	}

/**
Constructs an iterator over the text formatted by aLayout.
@param aLayout The formatter of the text to be iterated over.
*/
CTextLayout::TUtf32SourceCache::TUtf32SourceCache(const CTextLayout& aLayout)
	: iSource(aLayout.iSource), iCurrentViewIndex(-1)
	{
	}

/**
Gets the specified character without uniting surrogate pairs.
@param aIndex The document position to retrieve.
@pre 0 <= aIndex && aIndex < {document length}
*/
TText CTextLayout::TUtf32SourceCache::GetUtf16(TInt aIndex)
	{
	TTmCharFormat dummy;
	if (aIndex < iCurrentViewIndex)
		{
		iCurrentViewIndex = 0;
		iSource->GetText(iCurrentViewIndex, iCurrentView, dummy);
		}
	TInt currentViewEnd = iCurrentViewIndex + iCurrentView.Length();
	while (currentViewEnd <= aIndex)
		{
		TInt difference = aIndex - iCurrentViewIndex;
		TInt newIndex = aIndex - (difference >> 1);
		iCurrentViewIndex = newIndex < currentViewEnd?
			currentViewEnd : newIndex;
		iSource->GetText(iCurrentViewIndex, iCurrentView, dummy);
		currentViewEnd = iCurrentViewIndex + iCurrentView.Length();
		}
	return iCurrentView[aIndex - iCurrentViewIndex];
	}

/**
Gets the specified character, uniting surrogate pairs if appropriate.
@param aIndex
	The document position to retrive.
@return
	The character at position aIndex. If aIndex is at the first code of a
	surrogate pair, the full character is returned. If it is the second of a
	valid surrogate pair or an unpaired surrogate, the surrogate is returned.
*/
TChar CTextLayout::TUtf32SourceCache::GetUtf32(TInt aIndex)
	{
	TText code = GetUtf16(aIndex);
	if (TChar::IsHighSurrogate(code) && iSource->DocumentLength() < aIndex + 1)
		{
		TText code2 = GetUtf16(aIndex + 1);
		if (TChar::IsLowSurrogate(code2))
			return TChar::JoinSurrogate(code, code2);
		}
	return code;
	}

/** Allocates and constructs a CTextLayout object. By default, the formatting
is set to the entire document (EFFormatAllText).

The text needs to be reformatted after a call to this function.

@param aDoc Pointer to the MLayDoc implementation that is the source of the
text and formatting information. Must not be NULL or a panic occurs.
@param aWrapWidth The wrapping width in pixels.
@return Pointer to the new CTextLayout object. */
EXPORT_C CTextLayout *CTextLayout::NewL(MLayDoc *aLayDoc,TInt aWrapWidth)
	{
	CTextLayout* t = new(ELeave) CTextLayout;
	CleanupStack::PushL(t);
	t->ConstructL(aLayDoc,aWrapWidth);
	CleanupStack::Pop();
	return t;
	}

EXPORT_C CTextLayout::CTextLayout():
	iText(NULL),
	iExcessHeightRequired(0),
	iWnd(NULL),
	iBeginRedrawCount(0),
	iRedrawRect(),
	iTextViewCursorPos(NULL),
	iIsWndInExternalRedraw(EFalse),
	iUnformattedStart(KMaxTInt),
	iHighlightExtensions(NULL),
	iSource(NULL)
	{
	}

/** Constructs an object with a text source of aLayDoc and a wrap width of
aWrapWidth, with wrapping turned on.
*/
EXPORT_C void CTextLayout::ConstructL(MLayDoc *aLayDoc,TInt aWrapWidth)
	{
	iHighlightExtensions = new(ELeave) TTmHighlightExtensions;
	CleanupStack::PushL(iHighlightExtensions);
	iHighlightExtensions->SetAll(0);
	
	iText = new(ELeave) CTmTextLayout;
	CleanupStack::PushL(iHighlightExtensions);
	
	iSource = new(ELeave) TLayDocTextSource;
	iSource->iLayDoc = aLayDoc;
	SetWrapWidth(aWrapWidth);
	SetAmountToFormat(EFFormatAllText);
	
	CleanupStack::Pop(2);
	}

EXPORT_C CTextLayout::~CTextLayout()
	{
	delete iHighlightExtensions;
	iHighlightExtensions = NULL;
	delete iText;
	iText = NULL;
	delete iSource;
	iSource = NULL;
	}

void CTextLayout::SetWindow(RWindow* aWnd)
	{
	iWnd = aWnd;
	}

void CTextLayout::SetReadyToRedraw()
	{
	iReadyToRedraw = ETrue;
	}

void CTextLayout::BeginRedraw(const TRect& aRect)
	{
	if(!iReadyToRedraw)
		return;
	
	if (0 == iBeginRedrawCount++)
		{
		iRedrawRect = aRect;
		if (NULL != iWnd)
			{
			if (iWnd->GetDrawRect() == TRect::EUninitialized)
				{
				iWnd->Invalidate(aRect);
				iWnd->BeginRedraw(aRect);
				}
				else
					iIsWndInExternalRedraw = ETrue;
			}
		}
	}

void CTextLayout::EndRedraw()
	{
	if(!iReadyToRedraw)
		return;
	
	if (iBeginRedrawCount <= 0)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTLAYOUT_ENDREDRAW, "CTextLayout::EndRedraw" );
	    }
	__ASSERT_ALWAYS(iBeginRedrawCount > 0, Panic(EInvalidRedraw));

	if (0 == --iBeginRedrawCount)
		{
		if (NULL != iWnd)
			{
			if (!iIsWndInExternalRedraw)
				{
				iWnd->EndRedraw();
				iRedrawRect = TRect();
				}
				else
					iIsWndInExternalRedraw = EFalse;
			}
		}
	}

void CTextLayout::SetExternalDraw(const TRect& aRect)
	{
	if (0 != iBeginRedrawCount)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CTEXTLAYOUT_SETEXTERNALDRAW, "EInvalidRedraw" );
	    }
	__ASSERT_ALWAYS(0 == iBeginRedrawCount, Panic(EInvalidRedraw));
	iBeginRedrawCount++;
	iRedrawRect = aRect;
	}

void CTextLayout::ResetExternalDraw()
	{
	if (1 != iBeginRedrawCount)
	    {
	    OstTrace0( TRACE_FATAL, CTEXTLAYOUT_RESETEXTERNALDRAW, "EInvalidRedraw" );
	    }
	__ASSERT_ALWAYS(1 == iBeginRedrawCount, Panic(EInvalidRedraw));

	iBeginRedrawCount--;
	iRedrawRect = TRect();
	}

TBool CTextLayout::BeginRedrawCalled() const
	{
	return iBeginRedrawCount > 0;
	}


/** Discards all formatting information. This function is used by the CTextView
and the printing classes, but should be called by higher-level classes that
need to clean up after any CTextLayout function has caused an out-of-memory
exception. */
EXPORT_C void CTextLayout::DiscardFormat()
	{
	iText->Clear();
	iBandTop = 0;
	}

TInt CTextLayout::SetBandTop()
	{
	TInt originalBandTop = iBandTop;
	if (iScrollFlags & EFScrollOnlyToTopsOfLines)
		{
		TTmLineInfo line;
		if (iText->YPosToLine(iBandTop, line))
			iBandTop = line.iOuterRect.iTl.iY;
		}
	return originalBandTop - iBandTop;
	}

/** Sets the layout object's source text to aDoc.

The text needs to be reformatted after a call to this function.

@param aDoc Pointer to the MLayDoc implementation that is the source of the
text and formatting information. Must not be NULL or a panic occurs. */
EXPORT_C void CTextLayout::SetLayDoc(MLayDoc *aLayDoc)
	{
	iSource->iLayDoc = aLayDoc;
	}

/** Sets the wrap width. If the current format mode is screen mode
(CLayoutData::EFScreenMode) aWrapWidth is in pixels, otherwise it is in twips.

The text needs to be reformatted after a call to this function.

Note:

A valid wrap width (>0) must be supplied or the expected amount of formatting will 
not take place. This could lead to panics when trying to retrieve formatting 
information that does not exist.

@param aWrapWidth The wrap width in pixels or twips. */
EXPORT_C void CTextLayout::SetWrapWidth(TInt aWrapWidth)
	{
	if (iSource->iFormatMode == CLayoutData::EFScreenMode)
		iSource->iWidth = aWrapWidth;
	else
		iSource->iWidth = iSource->iFormatDevice->HorizontalTwipsToPixels(aWrapWidth);
	}

/** Sets the height of the band in pixels or twips. This is the height of the
visible text, or the view window, and it is also the page height for the
purposes of scrolling up and down by page. If the current mode is screen mode 
(CLayoutData::EFScreenMode) or what-you-see-is-what-you-get mode 
(CLayoutData::EFWysiwygMode), aHeight is in pixels, otherwise it is in twips.

The text needs to be reformatted after a call to this function. 

Note:

A valid band height (>0) must be supplied or the expected amount of formatting will 
not take place. This could lead to panics when trying to retrieve formatting 
information that does not exist.

@param aHeight The band height in pixels or twips. */
EXPORT_C void CTextLayout::SetBandHeight(TInt aHeight)
	{
	iVisibleHeight = aHeight;
	if (iBandHeight != CLayoutData::EFHeightForFormattingAllText || aHeight < 1)
		iBandHeight = aHeight;
	}

/** Gets the height of the band in pixels or twips.
@return The height of the band in pixels or twips. */
EXPORT_C TInt CTextLayout::BandHeight() const
	{
	return VisibleHeightInPixels();
	}

/** Sets the device map used for drawing and formatting. This device map is
also used for formatting and drawing paragraph labels unless a separate label
device map has been set (see SetLabelsDeviceMap()).

The text needs to be reformatted after a call to this function.

Note:

Although the name of the function suggests that only the image device is set,
the formatting device is also set.

@param aGd The device map used for drawing and formatting. */
EXPORT_C void CTextLayout::SetImageDeviceMap(MGraphicsDeviceMap *aDeviceMap)
	{
	iSource->iImageDevice = aDeviceMap;
	iSource->iFormatDevice = aDeviceMap;
	}

/** Sets the device map used for formatting and drawing paragraph labels. If
not set, the device map used for labels will be the same as that used for the
text.

The text needs to be reformatted after a call to this function.

@param aDeviceMap The device map used for formatting and drawing paragraph
labels. */
EXPORT_C void CTextLayout::SetLabelsDeviceMap(MGraphicsDeviceMap *aDeviceMap)
	{
	iSource->iLabelsDevice = aDeviceMap;
	}

/** Sets whether to format all the text (if aAmountOfFormat is
EFFormatAllText), or just the visible band (if aAmountOfFormat is
EFFormatBand). If band formatting is selected, enough text is formatted to fill
the visible height.

The text needs to be reformatted after a call to this function.

@param aAmountOfFormat CTextLayout::EFFormatBand (the default) to format the
visible text only. CTextLayout::EFFormatAllText to format all the text in the
document. */
EXPORT_C void CTextLayout::SetAmountToFormat(TAmountFormatted aAmountOfFormat)
	{
	if (aAmountOfFormat == EFFormatBand)
		iBandHeight = iVisibleHeight;
	else
		iBandHeight = CLayoutData::EFHeightForFormattingAllText;
	}

/** Tests whether band formatting is on, as set by
CTextLayout::SetAmountToFormat().
@return ETrue if band formatting is on, EFalse if not. */
EXPORT_C TBool CTextLayout::IsFormattingBand() const
	{
	return iBandHeight != CLayoutData::EFHeightForFormattingAllText;
	}

/** Sets the format mode and wrap width and (for certain format modes only)
sets the formatting device.

The text needs to be reformatted after a call to this function.

Notes:

If aFormatMode is CLayoutData::EFWysiwygMode or
CLayoutData::EFPrintPreviewMode, the format device is set to aFormatDevice,
which must not be NULL.

If aFormatMode is CLayoutData::EFScreenMode or CLayoutData::EFPrintMode,
aFormatDevice is ignored and should be NULL; the format device is set to the
image device.

The wrap width is set in either twips or pixels using the same rule as for
SetWrapWidth().

@param aFormatMode The format mode.
@param aWrapWidth The wrap width in pixels or twips.
@param aFormatDevice The formatting device or NULL, depending on the format
mode. */
EXPORT_C void CTextLayout::SetFormatMode(CLayoutData::TFormatMode aFormatMode,TInt aWrapWidth,
										 MGraphicsDeviceMap* aFormatDevice)
	{
	if (aFormatMode == CLayoutData::EFWysiwygMode || aFormatMode == CLayoutData::EFPrintPreviewMode)
		{
		if (aFormatDevice == NULL)
		    {
		    OstTrace0( TRACE_FATAL, CTEXTLAYOUT_SETFORMATMODE, "EFormatDeviceNotSet" );
		    }
		__ASSERT_ALWAYS(aFormatDevice != NULL,Panic(EFormatDeviceNotSet));
		iSource->iFormatDevice = aFormatDevice;
		}
	else
		iSource->iFormatDevice = iSource->iImageDevice;
	iSource->iFormatMode = aFormatMode;
	SetWrapWidth(aWrapWidth);
	}

/** Turns wrapping on (if aNoWrapping is EFParagraphsWrappedByDefault) or off
(if aNoWrapping is EFAllParagraphsNotWrapped). Overrides the paragraph format
when wrapping is turned off -paragraphs are not broken into lines even if the
iWrap member of CParaFormat is ETrue. If wrapping is turned on,
CParaFormat::iWrap is honoured.

The text needs to be reformatted after a call to this function.

@param aNoWrapping EFAllParagraphsNotWrapped (the default) to turn wrapping
off, EFParagraphsWrappedByDefault to turn wrapping on. */
EXPORT_C void CTextLayout::ForceNoWrapping(TBool aNoWrapping)
	{
	if (aNoWrapping)
		iSource->iFlags &= ~TLayDocTextSource::EWrap;
	else
		iSource->iFlags |= TLayDocTextSource::EWrap;
	}

/** Tests whether wrapping is on or off.
@return ETrue if wrapping is on, EFalse if off. */
EXPORT_C TBool CTextLayout::IsWrapping() const
	{
	return (iSource->iFlags & TLayDocTextSource::EWrap) != 0;
	}

/** Sets the truncation mode. If truncation is on, lines that exceed the wrap
width, either because they have no legal line break, or because wrapping is
off, are truncated, and an ellipsis is inserted.

@param aOn If ETrue, lines which extend beyond the wrap width are truncated
with an ellipsis character. If EFalse, no ellipsis is used. */
EXPORT_C void CTextLayout::SetTruncating(TBool aOn)
	{
	if (aOn)
		iSource->iFlags |= TLayDocTextSource::ETruncateWithEllipsis;
	else
		iSource->iFlags &= ~TLayDocTextSource::ETruncateWithEllipsis;
	}

/** Tests whether truncation is on (as set by SetTruncating()).
@return ETrue if truncation is on, EFalse if not. */
EXPORT_C TBool CTextLayout::Truncating() const
	{
	return (iSource->iFlags & TLayDocTextSource::ETruncateWithEllipsis) != 0;
	}

/** Sets the ellipsis character to be used if truncation is on. Specify the
value 0xFFFF (the illegal Unicode character) if no ellipsis character should
be used. By default, the ellipsis character is 0x2026, the ordinary horizontal
ellipsis.

@param aEllipsis The Unicode value of the truncating ellipsis character. */
EXPORT_C void CTextLayout::SetTruncatingEllipsis(TChar aEllipsis)
	{
	iSource->iEllipsis = aEllipsis;
	}

/** Returns the ellipsis character used when truncation is on. The value 0xFFFF
(the illegal Unicode character) means that no ellipsis character is appended to
truncated text.

@return The Unicode value of the truncating ellipsis character. */
EXPORT_C TChar CTextLayout::TruncatingEllipsis() const
	{
	return iSource->iEllipsis;
	}

/** Sets the width in pixels of the margin in which labels are drawn.

The text needs to be reformatted after a call to this function.

@param aWidth The width in pixels of the labels margin. */
EXPORT_C void CTextLayout::SetLabelsMarginWidth(TInt aWidth)
	{
	iSource->iLabelsWidth = aWidth;
	}

/** Specifies which non-printing characters (e.g. space, paragraph break, etc.)
are to be drawn using symbols.

The text needs to be reformatted after a call to this function.(because
non-printing characters may differ in width from their visible
representations).

@param aVisibility Indicates which non-printing characters are drawn using
symbols. */
EXPORT_C void CTextLayout::SetNonPrintingCharsVisibility(TNonPrintingCharVisibility aVisibility)
	{
	iSource->iNonPrintingCharVisibility = aVisibility;
	}

/** Returns which non-printing characters are drawn using symbols.
@return Indicates which non-printing characters are drawn using symbols. */
EXPORT_C TNonPrintingCharVisibility CTextLayout::NonPrintingCharsVisibility() const
	{
	return iSource->iNonPrintingCharVisibility;
	}

/** Tests whether background formatting is currently taking place. Background
formatting is managed by CTextView, using an active object, when the
CTextLayout object is owned by a CTextView object.

Not generally useful.

@return ETrue if background formatting is currently taking place. EFalse if not.
*/
EXPORT_C TBool CTextLayout::IsBackgroundFormatting() const
	{
	return iUnformattedStart < KMaxTInt;
	}

/** CTextView calls this function when background formatting has ended. It
allows the CTextLayout object to discard information used only during
background formatting.

Not generally useful. */
EXPORT_C void CTextLayout::NotifyTerminateBackgroundFormatting()
	{
	iUnformattedStart = KMaxTInt;
	}


/** Specifies whether partially displayed lines (at the top and bottom of
the view) are to be prevented from being drawn, and whether the top of
the display is to be aligned to the nearest line.

This function takes effect only when the text is next formatted or
scrolled.Note:This function was designed for non-editable text in the
Agenda application, and there is an important restriction:
CTextView functions that reformat the text after editing must not be used
while partial lines are excluded; these functions are 
CTextView::HandleCharEditL(),
CTextView::HandleInsertDeleteL() and
CTextView::HandleRangeFormatChangeL().

@param aExcludePartialLines ETrue (the default) to exclude partially
displayed lines from the view. EFalse to include them.
@deprecated 7.0 */
//It did not work, and it was not what the customer wanted. A more comprehensive
//solution is being considered.
EXPORT_C void CTextLayout::SetExcludePartialLines(TBool) {}

/** Tests whether partial lines at the top and bottom of the view are
currently excluded.

@return ETrue if partial lines are excluded, EFalse if they are displayed.
@deprecated 7.0 */
EXPORT_C TBool CTextLayout::ExcludingPartialLines() const
	{
	return EFalse;
	}
 
/** Sets the percentage by which font heights are increased in order to provide
automatic extra spacing (leading) between lines. This amount is set to
CLayoutData::EFFontHeightIncreaseFactor, which is 7, when a CTextLayout object
is created.

The text needs to be reformatted after a call to this function.

@param aPercentage Factor by which to increase font heights. */
EXPORT_C void CTextLayout::SetFontHeightIncreaseFactor(TInt aPercentage)
	{
	iSource->iFontHeightIncreaseFactor = aPercentage;
	}

/** Returns the font height increase factor as a percentage (i.e. a return
value of 7 means that font heights are increased by 7% to provide automatic
extra spacing between lines).

@return Factor by which font heights are increased. */
EXPORT_C TInt CTextLayout::FontHeightIncreaseFactor() const
	{
	return iSource->iFontHeightIncreaseFactor;
	}

/** Sets the minimum line descent in pixels. This amount is set to
CLayoutData::EFMinimumLineDescent, which is 3, when a CTextLayout object is
created.

The text needs to be reformatted after a call to this function.

@param aPixels The minimum line descent in pixels. */
EXPORT_C void CTextLayout::SetMinimumLineDescent(TInt aPixels)
	{
	iSource->iMinimumLineDescent = aPixels;
	}

/** Returns the minimum line descent in pixels.
@return The minimum line descent in pixels. */
EXPORT_C TInt CTextLayout::MinimumLineDescent() const
	{
	return iSource->iMinimumLineDescent;
	}

/** Returns the document length in characters, including all the text, not just
the formatted portion, but not including the final paragraph delimiter (the
"end-of-text character") if any. Thus the length of an empty document is zero.

@return The number of characters in the document */
EXPORT_C TInt CTextLayout::DocumentLength() const
	{
	return iSource->DocumentLength();
	}

/** Sets aDocPos to the paragraph start and returns the amount by which aDocPos
has changed, as a non-negative number.

@param aDocPos A document position. On return, contains the document position
of the first character in the paragraph.
@return The number of characters skipped in moving to the new document
position. */
EXPORT_C TInt CTextLayout::ToParagraphStart(TInt& aDocPos) const
	{
	TInt old_pos = aDocPos;
	aDocPos = iSource->ParagraphStart(aDocPos);
	return old_pos - aDocPos;
	}

/** Returns the height in pixels of any formatted text above the visible
region.
@return The height in pixels of any formatted text above the visible region. */
EXPORT_C TInt CTextLayout::PixelsAboveBand() const
	{
	return iBandTop;
	}

/** Returns the y coordinate of the bottom of the last formatted line, relative
to the top of the visible region.

@return The y coordinate of the bottom of the last formatted line. */
EXPORT_C TInt CTextLayout::YBottomLastFormattedLine() const
	{
    return Max(0, iText->LayoutHeight() - iBandTop );
	}

/** Returns the height in pixels of the formatted text.

@return The height in pixels of all the formatted text. */
EXPORT_C TInt CTextLayout::FormattedHeightInPixels() const
	{
	return iText->LayoutHeight();
	}

/** Returns the number of fully or partially visible characters in the visible
band.

@param aDocPos On return, contains the document position of the first fully or
partially visible character in the band.
@return The total number of characters in the band. */
EXPORT_C TInt CTextLayout::PosRangeInBand(TInt& aDocPos) const
	{
	TTmLineInfo info;
	if (!iText->YPosToLine(iBandTop,info))
		{
		aDocPos = 0;
		return 0;
		}
	aDocPos = info.iStart;
	int end = iSource->DocumentLength();
	//INC085809 - less 1 pixel. Rect edge is actually outside bounds
	if (iText->YPosToLine(iBandTop + VisibleHeightInPixels() - 1,info))
		end = info.iEnd;
	return end - aDocPos;
	}

/** Tests whether the document position aDocPos is fully or partially visible.
If it is, puts the y coordinate of the left-hand end of the baseline of the
line containing aDocPos into aXyPos.

@param aDocPos The document position of interest.
@param aXyPos On return, contains the y coordinate of the left-hand end of the
baseline of the line containing aDocPos.
@return ETrue if the position is visible. EFalse if the position is not visible.
*/
EXPORT_C TBool CTextLayout::PosInBand(TInt aDocPos,TPoint& aXyPos) const
	{
	TTmLineInfo info;
	TTmDocPos pos(aDocPos, ETrue);
	TBool result = PosInBand(pos,&info);
	aXyPos.iX = 0;
	aXyPos.iY = info.iBaseline;
	return result;
	}

/** Tests whether the document position aDocPos is fully or partially visible.
If it is, puts the y coordinate of the left-hand end of the baseline of the
line containing aDocPos into aXyPos.

@param aDocPos The document position of interest.
@param aXyPos On return, contains the y coordinate of the left-hand end of the
baseline of the line containing aDocPos.
@return ETrue if the position is visible. EFalse if the position is not visible.
*/
EXPORT_C TBool CTextLayout::PosInBand(TTmDocPos aDocPos,TPoint& aXyPos) const
	{
	TTmLineInfo info;
	TBool result = PosInBand(aDocPos,&info);
	aXyPos.iX = 0;
	aXyPos.iY = info.iBaseline;
	return result;
	}
	
/** Tests whether the document position aDocPos is fully or partially visible.
If it is, puts the baseline of the line containing aDocPos into aLineInfo.

@param aDocPos The document position of interest.
@param aLineInfo On return, contains the baseline of the line containing
aDocPos.

@return ETrue if the document position aDocPos is fully or partially visible. If
so, and if aLineInfo is non-NULL, puts information about the line in aLineInfo.
Otherwise, EFalse */
EXPORT_C TBool CTextLayout::PosInBand(const TTmDocPos& aDocPos,TTmLineInfo* aLineInfo) const
	{
	TTmLineInfo dummy;
	TTmLineInfo* info_ptr = aLineInfo ? aLineInfo : &dummy;
	TTagmaForwarder forwarder(*this);
	if (!forwarder.DocPosToLine(aDocPos,*info_ptr))
		return FALSE;
	return info_ptr->iOuterRect.iBr.iY > 0 && info_ptr->iOuterRect.iTl.iY < VisibleHeightInPixels();
	}

/** Tests whether the character aDocPos is formatted.

Note:

If a section of text contains characters p to q, it contains document positions
p to q + 1; but this function returns ETrue for positions p to q only, so it
refers to characters, not positions. However, it will return ETrue for q if q is
the end of the document.

@param aDocPos The document position of interest.
@return ETrue if the character at the document position specified is formatted.
EFalse if not. */
EXPORT_C TBool CTextLayout::PosIsFormatted(TInt aDocPos) const
	{
	if (aDocPos < iText->StartChar())
		return FALSE;
	if (aDocPos >= iText->StartChar() + FormattedLength())
		return FALSE;
	return TRUE;
	}

/** Gets the document position of the first character in the specified line,
counting the first line as line one (not zero) in the band. If the line is
after the band, returns the last character position of the band. If there is no
formatted text, returns CTextLayout::EFNoCurrentFormat.

@param aLineNo Line number in formatted text, counting the first line as line
one.
@return The document position of the first character on the line. */
EXPORT_C TInt CTextLayout::FirstCharOnLine(TInt aLineNo) const
	{
	if (aLineNo <= 0)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTLAYOUT_FIRSTCHARONLINE, "EInvalidLineNumber" );
	    }
	__ASSERT_DEBUG(aLineNo > 0,Panic(EInvalidLineNumber));
	if (iText->StartChar() == iText->EndChar())
		return EFNoCurrentFormat;
	aLineNo--;
	TTmLineInfo info;
	if (!iText->LineNumberToLine(aLineNo,info))
		return Min(iText->EndChar(),iSource->DocumentLength());
	return info.iStart;
	}

/** Returns the number of formatted characters. This will be one more than
expected if the formatted text runs to the end of the document, because it will
include the end-of-text character.

@return The number of formatted characters in the document. */
EXPORT_C TInt CTextLayout::FormattedLength() const
	{
	return iText->EndChar() - iText->StartChar();
	}

/** Returns the document position of the first formatted character.

@return The document position of the first formatted character. */
EXPORT_C TInt CTextLayout::FirstFormattedPos() const
	{
	return iText->StartChar();
	}

/** Gets the number of formatted lines.

@return The number of formatted lines in the document. */
EXPORT_C TInt CTextLayout::NumFormattedLines() const
	{
	return iText->Lines();
	}

/** Returns the line number, counting from 0, of the first fully visible line.

@return The line number of the first fully visible line. */
EXPORT_C TInt CTextLayout::FirstLineInBand() const
	{
	TTmLineInfo info;
	if (!iText->YPosToLine(iBandTop,info))
		return 0;
	int line = info.iLineNumber;
	if (info.iOuterRect.iTl.iY < iBandTop)
		line++;
	return line;
	}

/** Gets the rectangle enclosing the formatted line that contains or is closest
to y coordinate aYPos. If aYPos is above the first formatted line, the
rectangle returned is that of the first formatted line. If aYPos is below the
last formatted line the rectangle returned is that of the last formatted line.
If there is no formatted text, returns CTextLayout::EFNoCurrentFormat.

@param aYPos The y coordinate of the line of interest.
@param aLine On return, contains the rectangle which encloses the line at
aYPos.

@return The line width in pixels. */
EXPORT_C TInt CTextLayout::GetLineRect(TInt aYPos,TRect& aRect) const
	{
	int y = iBandTop + aYPos;

	// Snap to formatted area.
	if (y >= iText->LayoutHeight())
		y = iText->LayoutHeight() - 1;
	if (y < 0)
		y = 0;

	TTmLineInfo info;
	TBool found = iText->YPosToLine(y,info);
	if (!found)
		{
		aRect.SetSize(TSize(0,0));
		return EFNoCurrentFormat;
		}

	aRect = info.iOuterRect;
	aRect.iTl.iX = info.iInnerRect.iTl.iX;
	aRect.iBr.iX = info.iInnerRect.iBr.iX;
	aRect.Move(0,-iBandTop);
	return aRect.Width();
	}

/** Returns the height of the paragraph containing aDocPos. If the paragraph is
not formatted, returns zero. If the paragraph is partially formatted, returns
the height of the formatted part.

@param aDocPos A document position within the paragraph of interest.
@return The height in pixels of the paragraph. Zero if the paragraph is not
formatted. */
EXPORT_C TInt CTextLayout::ParagraphHeight(TInt aDocPos) const
	{
	TRect r;
	GetParagraphRect(TTmDocPos(aDocPos, ETrue), r);
	return r.Height();
	}

/** Returns the rectangle enclosing the paragraph containing aDocPos. If the
paragraph is not formatted, returns an empty rectangle. If the paragraph is
partially formatted, returns the rectangle enclosing the formatted part.

@param aDocPos A document position within the paragraph.
@return The rectangle which encloses the paragraph containing aDocPos. */
EXPORT_C TRect CTextLayout::ParagraphRectL(TInt aDocPos) const
	{	
	TRect r;
	GetParagraphRect(TTmDocPos(aDocPos, ETrue), r);
	return r;
	}

/** Returns the left and right extremes, in layout coordinates, of the
formatted text.

@param aLeftX On return, contains the x coordinate of the leftmost point of the
formatted text.
@param aRightX On return, contains the x coordinate of the rightmost point of
the formatted text.
@param aOnlyVisibleLines If ETrue, only scans partially or fully visible lines.
If EFalse, scans all the formatted text.
@param aIgnoreWrapCharacters If ETrue, does not include wrap characters in the
measurement (paragraph delimiters, forced line breaks, etc.). If EFalse,
includes them.
@return EFalse if there is no formatted text, otherwise ETrue. */
EXPORT_C TBool CTextLayout::CalculateHorizontalExtremesL(TInt& aLeftX,TInt& aRightX,
														 TBool aOnlyVisibleLines,
														 TBool /*aIgnoreWrapCharacters*/) const
	{
	return CalculateHorizontalExtremes(aLeftX,aRightX,aOnlyVisibleLines);
	//+ implement aIgnoreWrapCharacters? not until clearly specified
	}

TBool CTextLayout::CalculateHorizontalExtremes(TInt& aLeftX,TInt& aRightX,
											   TBool aOnlyVisibleLines) const
	{
	int top = 0;
	int bottom = KMaxTInt;
	if (aOnlyVisibleLines)
		{
		top = iBandTop;
		bottom = iBandTop + VisibleHeightInPixels();
		}
	iText->HorizontalExtremes(aLeftX, aRightX, top, bottom);
	if (0 < aLeftX)
		aLeftX = 0;
	if (aRightX < iSource->iWidth)
		aRightX = iSource->iWidth;
	return iText->StartChar() < iText->EndChar();
	}

TInt CTextLayout::WrapWidth() const
	{
	return iSource->iWidth;
	}

TBool CTextLayout::GetCursor(const TTmDocPos& aDocPos,TTmCursorPlacement aPlacement,
							 TRect& aLineRect,TPoint& aOrigin,TInt& aWidth,TInt& aAscent,TInt& aDescent) const
	{
	TTmLineInfo info;
	TBool result = iText->GetCursor(aDocPos,aPlacement,info,aOrigin,aWidth,aAscent,aDescent);
	if (result)
		{
		aOrigin.iY -= iBandTop;
		aLineRect = info.iOuterRect;
		aLineRect.iTl.iX = info.iInnerRect.iTl.iX;
		aLineRect.iBr.iX = info.iInnerRect.iBr.iX;
		aLineRect.Move(0,-iBandTop);
		}
	return result;
	}

/** Gets the height (ascent + descent) and ascent of the font of the character
at aDocPos, as created using the graphics device map used for drawing (the
"image device") and returns them in aHeight and aAscent, after increasing
aHeight by the font height increase factor (see SetFontHeightIncreaseFactor()).

@param aDocPos A document position.
@param aHeight On return contains the height in pixels of the character at
aDocPos.
@param aAscent On return, contains the ascent in pixels of the character at
aDocPos. */
EXPORT_C void CTextLayout::GetCharacterHeightAndAscentL(TInt aDocPos,TInt& aHeight,TInt& aAscent) const
	{
	TPtrC text;
	TTmCharFormat format;
	iSource->GetText(aDocPos,text,format);
	TFontSpec fs;
	format.iFontSpec.GetTFontSpec(fs);
	GetFontHeightAndAscentL(fs,aHeight,aAscent);
	}

/** Gets the height (ascent + descent) and ascent of the font specified by
aFontSpec, as created using the graphics device map used for drawing (the
"image device") and puts them into aHeight and aAscent, after increasing
aHeight by the font height increase factor (see SetFontHeightIncreaseFactor()).

@param aFontSpec Font specification.
@param aHeight On return, contains the height in pixels of the font.
@param aAscent On return, contains the ascent in pixels of the font. */
EXPORT_C void CTextLayout::GetFontHeightAndAscentL(const TFontSpec& aFontSpec,TInt& aHeight,TInt& aAscent) const
	{
	CFont* font;
	MGraphicsDeviceMap& device = iSource->InterpretDevice();
	User::LeaveIfError(device.GetNearestFontInTwips(font,aFontSpec));
	aHeight = font->HeightInPixels();
	int increase = (iSource->iFontHeightIncreaseFactor * aHeight) / 100;
	aHeight += increase;
	aAscent = font->AscentInPixels() + increase;
	device.ReleaseFont(font);
	}

/** Returns the index of the nearest character in the document to the window 
coordinates specified. Sets aPos to the actual position of the intersection 
of the line's baseline with the character's edge. If aPos is before the start
of the formatted area, returns the first formatted character; if it is after 
the end of the formatted area, returns the position after the last formatted 
character, or the end of the document, whichever is less.

This function is deprecated in v7.0s. Use the more powerful FindXYPos() instead.

@param aPos Contains coordinates to convert to a document position. On return,
contains the exact coordinates of the intersection of the line's baseline with
the character edge at the document position.
@param aFlags Three possible values: 0 is the default, and performs the task at
full accuracy (the function returns the document position of the character edge
nearest to the coordinates). CLayoutData::EFWholeLinesOnly examines lines only
and returns the position at the right end of the line if aPos.iX > 0, otherwise
the position at the left end.
@return The document position of the nearest character to the coordinates, or
of the start or end of the line, depending on the value of aFlags. */
EXPORT_C TInt CTextLayout::XyPosToDocPosL(TPoint &aXyPos,TUint) const
	{
	if (aXyPos.iY < -iBandTop)
		return iText->StartChar();
	else if (aXyPos.iY >= iText->LayoutHeight() - iBandTop)
		return Min(iText->EndChar(),iSource->DocumentLength());
	TTmPosInfo2 pos_info;
	TTmLineInfo lineInfo;
	FindXyPos(aXyPos,pos_info, &lineInfo);
	aXyPos = pos_info.iEdge;
	TInt r = pos_info.iDocPos.iPos;
	if (!pos_info.iDocPos.iLeadingEdge && lineInfo.iStart != r)
		{
		r -= 1;

		//	surrogate support
		if ( r > 0 )
			{
			TPtrC text;
			TTmCharFormat format;

			iSource->GetText( r - 1, text, format );
			if ( text.Length() > 1 )
				{
				TUint highSurrogate = text[0];
				TUint lowSurrogate = text[1];
				if ( TChar::IsHighSurrogate( highSurrogate ) &&
				        TChar::IsLowSurrogate( lowSurrogate ) )
					--r;
				}
			}
		}

	return r;
	}

/** Finds the document position nearest to aXyPos. If aXyPos is in the
formatted text returns ETrue, otherwise returns EFalse. If ETrue is returned,
places information about the document position in aPosInfo and information
about the line containing the document position in aLineInfo if it is non-NULL.

@param aXyPos Contains coordinates to convert to a document position.
@param aPosInfo Buffer to store information about the document position if the
specified coordinates are located in the formatted text.
@param aLineInfo Buffer to store information about the line if the specified
coordinates are located in the formatted text.
@return ETrue if aXyPos is a formatted position, otherwise EFalse. */
EXPORT_C TBool CTextLayout::FindXyPos(const TPoint& aXyPos,TTmPosInfo2& aPosInfo,TTmLineInfo* aLineInfo) const
	{
	TTagmaForwarder forwarder(*this);
	TTmLineInfo line_info;
	TBool result = forwarder.FindXyPos(aXyPos,aPosInfo,line_info);
	if (aLineInfo && result)
		*aLineInfo = line_info;
	return result;
	}

/** Returns the x-y coordinates of the document position aDocPos in aPos. The
return value is ETrue if the position is formatted, or EFalse if it is not, in
which case aPos is undefined.

Deprecated - use the more powerful FindDocPos() instead

@param aDocPos The document position.
@param aPos On return, contains the x-y coordinates of aDocPos.
@param aFlags Two possible values: 0 is the default, and performs the task at
full accuracy, and CLayoutData::EFWholeLinesOnly, which examines lines only and
sets aXyPos.iY only, and cannot leave.
@return ETrue if the document position is formatted, EFalse if not. */
EXPORT_C TBool CTextLayout::DocPosToXyPosL(TInt aDocPos,TPoint& aXyPos,TUint /*aFlags*/) const
	{
	TTmDocPos doc_pos(aDocPos, ETrue);
	TTmPosInfo2 pos_info;
	TBool result = FindDocPos(doc_pos,pos_info);
	aXyPos = pos_info.iEdge;
	return result;
	}

/** Finds the x-y position of the document position aDocPos.

If ETrue is returned, places information about the document position in aPosInfo
and information about the line containing the document position in aLineInfo if
it is non-NULL.

@param aDocPos Contains the document position to check.
@param aPosInfo On return, stores information about the document position if
the position is formatted.
@param aLineInfo Buffer to store the line information if the document position
is formatted.
@return ETrue if aDocPos is in the formatted text, otherwise EFalse. */
EXPORT_C TBool CTextLayout::FindDocPos(const TTmDocPosSpec& aDocPos,TTmPosInfo2& aPosInfo,TTmLineInfo* aLineInfo) const
	{
	TTagmaForwarder forwarder(*this);
	TTmLineInfo line_info;
	TBool result = forwarder.FindDocPos(aDocPos,aPosInfo,line_info);
	if (aLineInfo && result)
		*aLineInfo = line_info;
	return result;
	}

/** Finds the next cursor position to aDocPos in the visually ordered line.

@param aDocPos Contains the document position to check.
@param aPosInfo On return, stores information about the document position of
the next cursor position, if ETrue is returned.
@param aToLeft ETrue if the position to the left is to be found, or EFalse if
the position to the right is to be found.
@return EFalse if there is no formatting, or the position is at the end of the
line already. ETrue otherwise. */
EXPORT_C TBool CTextLayout::GetNextVisualCursorPos(
	const TTmDocPosSpec& aDocPos, TTmPosInfo2& aPosInfo, TBool aToLeft) const
	{
	TTagmaForwarder forwarder(*this);
	return forwarder.GetNextVisualCursorPos(aDocPos, aPosInfo, aToLeft);
	}

/** Gets a rectangle enclosing two formatted document positions on the same
line. If the second position is less than the first, or on a different line, it
is taken to indicate the end of the line. This function panics if either
position is unformatted.

Note:

CTextLayout must have been set with a valid wrap width and band height before
calling this function, otherwise no formatting will take place and the function 
will panic.  Wrap width and band height values must be > 0 to be valid.

@param aDocPos1 The first document position on the line.
@param aDocPos2 The second document position on the line.
@return The minimal rectangle, which bounds both positions.
@see SetBandHeight
@see SetWrapWidth */
EXPORT_C TRect CTextLayout::GetLineRectL(TInt aDocPos1,TInt aDocPos2) const
	{
	TRect rect;
	TPoint point;
	TInt xCoords[4];
	
	if (iText->LayoutHeight() == 0)
		return TRect(0,0,0,0);

	if (!PosIsFormatted(aDocPos1))
	    {
	    OstTrace0( TRACE_FATAL, CTEXTLAYOUT_GETLINERECTL, "ECharacterNotFormatted" );
	    }
	__ASSERT_ALWAYS(PosIsFormatted(aDocPos1),Panic(ECharacterNotFormatted));
	if (!PosIsFormatted(aDocPos2))
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTLAYOUT_GETLINERECTL, "ECharacterNotFormatted" );
	    }
	__ASSERT_DEBUG(PosIsFormatted(aDocPos2),Panic(ECharacterNotFormatted));

	TTmDocPosSpec docSpec(aDocPos1, TTmDocPosSpec::ELeading);
	TTmPosInfo2 pos_info;
	
	// Finding the leading edge of aDocPos1
	FindDocPos(docSpec,pos_info);
	point = pos_info.iEdge;
	xCoords[0] = point.iX;

	// Getthe Line rectangle
	GetLineRect(point.iY,rect);
	
	if (rect.iTl.iY > point.iY || rect.iBr.iY < point.iY)
	    {
	    OstTrace0( TRACE_DUMP, DUP2_CTEXTLAYOUT_GETLINERECTL, "EPixelNotInFormattedLine" );
	    }
	__ASSERT_DEBUG(rect.iTl.iY <= point.iY && rect.iBr.iY >= point.iY,Panic(EPixelNotInFormattedLine));
	
	//	Finding the leading edge of aDocPos2
	docSpec.iPos = aDocPos2;
	TBool isformatted = FindDocPos(docSpec, pos_info);
	point = pos_info.iEdge;
	
	// Checks if the aDocPos2 is contained in the same line of the aDocPos1
	// Can't use TRect::Contains() here, because TRect::Contains() considers a point located on the right  
	// hand side or bottom as outside the rectangle, which will has problem when work with RTL text that 
	// is at the right hand end of a line.
	TBool isContained = (point.iX>=rect.iTl.iX && point.iX<=rect.iBr.iX 
				&& point.iY>=rect.iTl.iY && point.iY<=rect.iBr.iY);

	RTmParFormat parFormat;
	iSource->GetParagraphFormatL(aDocPos1, parFormat);
	
	//	The special cases (as indicated in the description):
	//	When the aDocPos2 is less than the aDocPos1, or not formatted, or on a different line from which the  
	//	aDocPos1 is in, then the function will return the rectangle starting from docPos1's leading edge and
	//	ending at the end of the line (which is determined by paragraph directionality).
	if (aDocPos2 < aDocPos1 || !isformatted || !isContained)
		{
		if (parFormat.RightToLeft())
			rect.iBr.iX = xCoords[0];
		else
			rect.iTl.iX = xCoords[0];
		
		parFormat.Close();
		return rect;
		}

	xCoords[1] = point.iX;
	
	//	Finding the Trailing edge of (aDocPos1 + 1)
	docSpec.iType = TTmDocPosSpec::ETrailing;
	docSpec.iPos = aDocPos1 + 1;
	FindDocPos(docSpec, pos_info);
	xCoords[2] = pos_info.iEdge.iX;
	
	//	Finding the Trailing edge of (aDocPos2 + 1)
	docSpec.iPos = aDocPos2 + 1;
	FindDocPos(docSpec, pos_info);
	xCoords[3] = pos_info.iEdge.iX;
	
	rect.iBr.iX = xCoords[0];
	rect.iTl.iX = xCoords[0];
	
	// The returned rectangle is generated by (when is not a special case)
	// 1) Find the Line rectangle
	// 2) Find the:
	//		- leading edge of docPos1 
    //		- leading edge of docPos2 
    //		- trailing edge of docPos1+1 
    //		- trailing edge of docPos2+1
	// 3) Cut the line rectangle at the smallest edge and the largest edge among 
	//	  the four edges found in step 2.
	for (TInt i = 1; i<4; i++ )
		{
		if (rect.iBr.iX < xCoords[i])
			rect.iBr.iX = xCoords[i];
		if (rect.iTl.iX > xCoords[i])
			rect.iTl.iX = xCoords[i];
		}
	
	parFormat.Close();
	return rect;
	}

/** Gets the bounding rectangle of the picture, if any, located at the document
position or coordinates specified, and returns it in aPictureRect.

If aCanScaleOrCrop is non-null, sets aCanScaleOrCrop to indicate 
whether the picture can be scaled or cropped. 
Returns ETrue if the operation was successful. Returns EFalse otherwise;
that is, if there is no picture at the position, or if the position is unformatted.

@param aDocPos The document position of interest.
@param aXyPos The layout coordinates of interest.
@param aPictureRect On return, contains the rectangle which encloses the
picture located at the position specified.
@param aCanScaleOrCrop If non-NULL and the function returns ETrue, on return,
indicates whether the picture can be scaled or cropped. By default, NULL.
@return ETrue if the operation was successful, (i.e. there is a
picture character at the position, it has been loaded into
memory, and the position is formatted). EFalse if any of these
conditions are not met. */
EXPORT_C TBool CTextLayout::PictureRectangleL(TInt aDocPos,TRect& aPictureRect,TBool* aCanScaleOrCrop) const
	{
	if (aDocPos < iText->StartChar() || aDocPos >= iText->EndChar() - 1)
		return FALSE;

	aPictureRect.SetRect(0,0,0,0);
	TSize size;
	if (iSource->GetPictureSizeInTwipsL(aDocPos,size) != KErrNone)
		return FALSE;

	TRect rect;
	TTmDocPos pos(aDocPos, ETrue);
	TTmPosInfo2 pos_info;
	TTmLineInfo info;
	TInt subscript;
	if (!iText->FindDocPos(pos,pos_info,info,subscript))
		return FALSE;

	MGraphicsDeviceMap& device = iSource->InterpretDevice();
	size.iWidth = device.HorizontalTwipsToPixels(size.iWidth);
	size.iHeight = device.VerticalTwipsToPixels(size.iHeight);
	rect.iBr.iY = pos_info.iEdge.iY - iBandTop + subscript;
	rect.iTl.iY = rect.iBr.iY - size.iHeight;
	if (pos_info.iRightToLeft)
		{
		rect.iBr.iX = pos_info.iEdge.iX;
		rect.iTl.iX = rect.iBr.iX - size.iWidth;
		}
	else
		{
		rect.iTl.iX = pos_info.iEdge.iX;
		rect.iBr.iX = rect.iTl.iX + size.iWidth;
		}
/*	rect.iTl = pos_info.iEdge;
	rect.Move(0,-iBandTop);
	rect.iTl.iY -= size.iHeight;
	rect.iBr = rect.iTl + size;*/
	CPicture* picture = iSource->PictureL(aDocPos);
	if (!picture)
		return FALSE;

	if (aCanScaleOrCrop)
		*aCanScaleOrCrop = picture->Capability().iScalingType != TPictureCapability::ENotScaleable ||
						   picture->Capability().iIsCroppable;
	aPictureRect = rect;
	return TRUE;
	}

/** Finds if there is a picture at the position under the point aXyPos.
If there is, returns the document position of it, sets the rectangle
occupied in aPictureRect, and whether the picture allows scaling
in aCanScaleOrCrop, if non-null. Note that aXyPos may be outside
the picture found.
@return  The document position of the picture found, or KErrNotFound if there 
is none. */
TInt CTextLayout::PictureRectangleAndPosL(const TPoint& aXyPos, TRect& aPictureRect,
	TBool* aCanScaleOrCrop) const
	{
	TTmPosInfo2 posInfo;
	if (!FindXyPos(aXyPos, posInfo))
		return KErrNotFound;
	TInt doc_pos = posInfo.iDocPos.iPos - (posInfo.iDocPos.iLeadingEdge? 0 : 1);
	if (PictureRectangleL(doc_pos, aPictureRect, aCanScaleOrCrop))
		return doc_pos;
	return KErrNotFound;
	}

/** Gets the bounding rectangle of the picture (if any) at aXyPos and puts it
in aPictureRect. If aCanScaleOrCrop is non-null sets *aCanScaleOrCrop to
indicate whether the picture can be scaled or cropped. Note that aXyPos
may be outside aPictureRect on a successful return, if the picture does
not occupy the whole of the section of the line it is in.
@return ETrue if the position is formatted and there is a picture there. */	
EXPORT_C TBool CTextLayout::PictureRectangleL(const TPoint& aXyPos,
	TRect& aPictureRect, TBool* aCanScaleOrCrop) const
	{
	return 0 <= PictureRectangleAndPosL(aXyPos, aPictureRect, aCanScaleOrCrop)?
		ETrue : EFalse;
	}

/** Gets the first document position in a line that starts at or below the top
of the visible area. If there is no such line, returns the position after the
last formatted character.

@return The document position of the first character in a line within the
visible area. */
EXPORT_C TInt CTextLayout::FirstDocPosFullyInBand() const
	{
	TTmLineInfo info;
	if (!iText->YPosToLine(iBandTop,info))
		return iText->EndChar();
	if (info.iOuterRect.iTl.iY < iBandTop)
		return info.iEnd;
	else
		return info.iStart;
	}


 
 
/** This interface is deprecated, and is made available in version 7.0s solely
to provide binary compatibility with Symbian OS v6.1. Developers are strongly
advised not to make use of this API in new applications. In particular, use the
other overload of this function if you need to distinguish between leading and
trailing edge positions.

Do not use if a CTextView object owns this CTextLayout object.

@param aDocPos A valid document position.
@param aYPos The y coordinate at which to display the character at aDocPos. On
return, contains the actual vertical position of the specified part of the
line.
@param aYPosQualifier Controls which part of the line is set to appear at
aYPos.
@param aDiscardFormat If ETrue (EFViewDiscardAllFormat), the text is reformatted
to include aDocPos, otherwise text is formatted only as necessary when bringing
new lines into the visible area.
@return The number of pixels the text was scrolled, may be positive or
negative. A value of CTextLayout::EFScrollRedrawWholeScreen indicates that the
entire visible area, at least, was scrolled, and so there is no point in
blitting text; a full redraw is needed. */
EXPORT_C TInt CTextLayout::SetViewL(TInt aDocPos,TInt& aYPos,TViewYPosQualifier aYPosQualifier,TDiscard aDiscardFormat)
	{
	TTmDocPos pos(aDocPos, ETrue);
	return SetViewL(pos,aYPos,aYPosQualifier,aDiscardFormat);
	}

/** Changes the top of the visible area so that the line containing aDocPos is
vertically positioned at aYPos. Which part of the line is set to appear at
aYPos (top, baseline, or bottom) is controlled by the TViewYPosQualifier
argument, which also specifies whether the visible area is to be filled and
whether the line should be made fully visible if possible.

Do not use if a CTextView object owns this CTextLayout object.

@param aDocPos A valid document position.
@param aYPos The y coordinate at which to display the character at aDocPos. On
return, contains the actual vertical position of the specified part of the
line.
@param aYPosQualifier Controls which part of the line is set to appear at
aYPos.
@param aDiscardFormat If ETrue (EFViewDiscardAllFormat), the text is reformatted
to include aDocPos, otherwise text is formatted only as necessary when bringing
new lines into the visible area.
@return The number of pixels the text was scrolled, may be positive or
negative. A value of CTextLayout::EFScrollRedrawWholeScreen indicates that the
entire visible area, at least, was scrolled, and so there is no point in
blitting text; a full redraw is needed. */
EXPORT_C TInt CTextLayout::SetViewL(const TTmDocPos& aDocPos, TInt& aYPos,
	TViewYPosQualifier aYPosQualifier, TDiscard aDiscardFormat)
	{
	if (aDocPos.iPos < 0 || aDocPos.iPos > iSource->DocumentLength())
		Panic(EInvalidDocPos);

	/*
	If the format is to be discarded, or no text has yet been formatted, or if the document position
	to be viewed is not formatted, format the band.
	*/
	TTmLineInfo info;
	TBool all_formatted = FALSE;
	TBool pos_is_formatted = iText->DocPosToLine(aDocPos,info);
	if (!pos_is_formatted ||
		aDiscardFormat == EFViewDiscardAllFormat ||
		(iText->StartChar() == iText->EndChar() && iSource->DocumentLength() > 0))
		{
		FormatBandL(aDocPos.iPos,aDocPos.iPos);
		all_formatted = TRUE;
		pos_is_formatted = iText->DocPosToLine(aDocPos,info);
		}

	// Find out where the top of the line is.
	if (!pos_is_formatted)
		return 0;
	int line_top_y = info.iOuterRect.iTl.iY - iBandTop;

	// Determine the desired position of the top of the line.
	int offset = 0;
	if (aYPosQualifier.iHotSpot == TViewYPosQualifier::EFViewBaseLine)
		offset = info.iBaseline - info.iOuterRect.iTl.iY;
	else if (aYPosQualifier.iHotSpot == TViewYPosQualifier::EFViewBottomOfLine)
		offset = info.iOuterRect.iBr.iY - info.iOuterRect.iTl.iY;
	int desired_line_top_y = aYPos - offset;

	// Adjust aYPos so that the line is fully visible if desired.
	if (aYPosQualifier.iFullyVisible == TViewYPosQualifier::EFViewForceLineFullyVisible)
		{
		TInt screenHeight = VisibleHeightInPixels();
		TInt lineHeight = info.iOuterRect.Height();
		TInt lineAscent = info.iBaseline - info.iOuterRect.iTl.iY;
		// If the top of the line is off the top of the screen, and the
		// baseline and the top can both fit, make the top of the line flush
		// with the top of the screen.
		if (lineAscent <= screenHeight
			&& desired_line_top_y < 0)
			desired_line_top_y = 0;
		// If the whole line can fit and the bottom if off the bottom of the
		// screen, make the bottom flush with the bottom of the screen.
		if (lineHeight <= screenHeight
			&& screenHeight < desired_line_top_y + lineHeight)
			desired_line_top_y = screenHeight - lineHeight;
		// If the ascent will not fit, or the baseline is off the bottom of
		// the screen, move the baseline flush with the bottom of the screen
		if (screenHeight < lineAscent
			|| screenHeight < desired_line_top_y + lineAscent)
			desired_line_top_y = screenHeight - lineAscent;
		}

	// Scroll the document position to the desired vertical coordinate and update aYPos.
	TInt dy = desired_line_top_y - line_top_y;
	if (dy)
		{
		dy = ScrollL(dy,aYPosQualifier.iFillScreen ? EFDisallowScrollingBlankSpace : EFAllowScrollingBlankSpace);
		line_top_y += dy;
		aYPos = line_top_y + offset;

		// Ensure that aYPos is in the line.
		if (aYPosQualifier.iHotSpot == TViewYPosQualifier::EFViewBottomOfLine)
			aYPos--;
		}
	return all_formatted ? EFScrollRedrawWholeScreen : dy;
	}

/** Formats enough text to fill the visible band.

Note: Do not use if a CTextView object owns this CTextLayout object. */
EXPORT_C void CTextLayout::FormatBandL()
	{
	FormatBandL(0,0);
	}

/**
Format enough text to fill the visible band and include both aStartDocPos and
aEndDocPos. Start at the start of the document if formatting everything, or at
the start of the paragraph containing aStartDocPos if formatting the visible
band only.
*/
void CTextLayout::FormatBandL(TInt aStartDocPos,TInt aEndDocPos)
	{
	TTmFormatParam param;
	InitFormatParam(param);
	if (iBandHeight != CLayoutData::EFHeightForFormattingAllText)
		param.iStartChar = iSource->ParagraphStart(aStartDocPos);
	if (param.iWrapWidth < 1 || param.iMaxHeight < 1)
		{
		// Do just a little formatting if the values are illegal.
		// This is to prevent the AddParL running with no
		// height limit, taking up huge amounts of time, and
		// having to return with 0 lines formatted, which
		// confuses CEikEdwin.
		param.iMaxHeight = 1;
		param.iWrapWidth = 1;
		iText->SetTextL(*iSource,param);
		return;
		}
	iText->SetTextL(*iSource,param);
	param.iMaxHeight = KMaxTInt;
	
	if(IsFormattingBand() && (iText->EndChar() <= aEndDocPos && iText->EndChar() < iSource->DocumentLength()))
		{
		param.iEndChar = Min(aEndDocPos+1,iSource->DocumentLength());
		iText->ExtendFormattingDownwardsL(param);
		}
	else
		{
		while (iText->EndChar() <= aEndDocPos && iText->EndChar() < iSource->DocumentLength())
			{
			int h,p;
			AddFormattingAtEndL(param, h, p);
			}
		}
	

	if (iBandHeight != CLayoutData::EFHeightForFormattingAllText)
		{
		int visible_height = VisibleHeightInPixels();
		while (iText->LayoutHeight() < visible_height)
			{
			int h,p;
			if (!iText->AddParL(param,TRUE,h,p))
				break;
			}
		}
	iBandTop = 0;
	}

/** Makes sure the line that aYPos is in (if it exists) is covered by the
formatting.
@param aYPos Y pixel position in window-relative co-ordinates.
@internalComponent
*/
EXPORT_C void CTextLayout::ExtendFormattingToCoverYL(TInt aYPos)
	{
	TTmFormatParam param;
	InitFormatParam(param);
	param.iStartChar = iText->StartChar() - 1;
	param.iEndChar = iText->EndChar();
	param.iMaxHeight = KMaxTInt;
	TInt heightIncrease;
	TInt p;
	while (iBandTop < -aYPos && iText->AddParL(param, ETrue, heightIncrease, p))
		{
		iBandTop += heightIncrease;
		param.iStartChar = iText->StartChar() - 1;
		}
	TInt heightRemaining = iBandTop + aYPos - iText->LayoutHeight() + 1;
	if(heightRemaining > 0)
		{
		param.iStartChar = iText->EndChar();
		param.iEndChar = KMaxTInt;
		param.iMaxHeight = heightRemaining;
		iText->ExtendFormattingDownwardsL(param);
		}
	}

/** Allows you to increase the formatted text range by specifying a point in the text you
want to increase the range to. Makes sure the line that aDocPos is in (if it exists) is covered by the
formatting.
@param aDocPos Position in the text you wish to extend the formatting to. e.g. passing in 0 will
increase the formatting range to the very first character entered in the document.
@pre aDocPos is in the range 0...DocumentLength
@post aDocPos has been formatted
*/
EXPORT_C void CTextLayout::ExtendFormattingToCoverPosL(TInt aDocPos)
	{
	if (0 > aDocPos || aDocPos > DocumentLength())
	    {
	    OstTrace0( TRACE_DUMP, CTEXTLAYOUT_EXTENDFORMATTINGTOCOVERPOSL, "EInvalidDocPos" );
	    }
	__ASSERT_DEBUG(0 <= aDocPos && aDocPos <= DocumentLength(),
			Panic(EInvalidDocPos));
	TTmFormatParam param;
	InitFormatParam(param);
	param.iStartChar = iText->StartChar();
	param.iEndChar = iText->EndChar();
	param.iMaxHeight = KMaxTInt;
	TInt heightIncrease;
	TInt p;
	TInt pos = aDocPos;
	if(pos > 0)     // Avoid going into infinite loop.
		{
		pos -= 1;	
		}						
	while ((pos < param.iStartChar) && (iText->AddParL(param, ETrue, heightIncrease, p)))
		{
		iBandTop += heightIncrease;
		param.iStartChar = iText->StartChar();
		}	
	param.iEndChar = aDocPos + 1;
	if(iText->EndChar() < param.iEndChar)
		{
		iText->ExtendFormattingDownwardsL(param);
		}
	if ((aDocPos < iText->StartChar()) || (aDocPos > iText->EndChar()))
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTLAYOUT_EXTENDFORMATTINGTOCOVERPOSL, "ECharacterNotFormatted" );
	    }
	__ASSERT_DEBUG((aDocPos >= iText->StartChar()) && (aDocPos <= iText->EndChar()),
			Panic(ECharacterNotFormatted));
	}


/** Sets the formatted text to begin at the start of the paragraph including
aStartPos and end at aEndPos. Moves the line containing aStartDocPos to the top
of the visible area.

Notes:

This function is not generally useful; it exists for the convenience of the
printing system.

Do not use if a CTextView object owns this CTextLayout object.

@param aStartDocPos A document position within the paragraph from which to
begin formatting.
@param aEndDocPos Document position at which to end formatting. */
EXPORT_C void CTextLayout::FormatCharRangeL(TInt aStartDocPos,TInt aEndDocPos)
	{
	FormatCharRangeL(aStartDocPos,aEndDocPos,0);
	}

void CTextLayout::FormatCharRangeL(TInt aStartDocPos,TInt aEndDocPos,TInt aPixelOffset)
	{
	if (aStartDocPos < 0 && aStartDocPos > DocumentLength())
	    {
	    OstTrace0( TRACE_DUMP, CTEXTLAYOUT_FORMATCHARRANGEL, "EInvalidDocPos" );
	    }
	__ASSERT_DEBUG(aStartDocPos >= 0 && aStartDocPos <= DocumentLength(),Panic(EInvalidDocPos));
	if (aEndDocPos < 0 || aEndDocPos > DocumentLength())
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTLAYOUT_FORMATCHARRANGEL, "EInvalidDocPos" );
	    }
	__ASSERT_DEBUG(aEndDocPos >= 0 && aEndDocPos <= DocumentLength(),Panic(EInvalidDocPos));
	if (aStartDocPos > aEndDocPos)
	    {
	    OstTrace0( TRACE_DUMP, DUP2_CTEXTLAYOUT_FORMATCHARRANGEL, "ENoCharRangeToFormat" );
	    }
	__ASSERT_DEBUG(aStartDocPos <= aEndDocPos,Panic(ENoCharRangeToFormat));

	TTmFormatParam param;
	InitFormatParam(param);
	param.iStartChar = iSource->ParagraphStart(aStartDocPos);
	param.iEndChar = aEndDocPos;
	param.iMaxHeight = KMaxTInt;
	iText->SetTextL(*iSource,param);
	TTmLineInfo info;
	TTmDocPos pos(aStartDocPos, ETrue);
	if (iText->DocPosToLine(pos,info))
		iBandTop = info.iOuterRect.iTl.iY - aPixelOffset;
	else
		iBandTop = 0; //+ raise an exception?
	}

/** A special function to support background formatting by the higher level
CTextView class. It formats the next pending line. The return value is ETrue if
there is more formatting to do. On entry, aBotPixel contains the y coordinate
of the bottom of the formatted text; this is updated by the function.

Notes:

Not generally useful.

Do not use if a CTextView object owns this CTextLayout object.

@param aBotPixel On entry, contains the y coordinate of the bottom of the
formatted text; this is updated by the function.
@return ETrue if there is more formatting to do. EFalse if not. */
EXPORT_C TBool CTextLayout::FormatNextLineL(TInt& aBottomPixel)
	{
	if (iUnformattedStart < KMaxTInt)
		{
		TTmFormatParamBase param;
		InitFormatParam(param);
		param.iMaxHeight = KMaxTInt;
		TTmReformatParam reformat_param;
		reformat_param.iStartChar = iUnformattedStart;
		reformat_param.iMaxExtraLines = KMaxExtraLines;
		reformat_param.iParInvalid = iParInvalid;
		TTmReformatResult result;
		iText->FormatL(param,reformat_param,result);

		// If there is no formatting to do, indicate that formatting is complete to the end of the paragraph
		if (result.iUnformattedStart == KMaxTInt)
			{
			TTmLineInfo info;
			TTmDocPos pos(iUnformattedStart, ETrue);
			TBool isFormatted = iText->DocPosToLine(pos,info);
			if (!isFormatted)
			    {
			    OstTrace0( TRACE_DUMP, CTEXTLAYOUT_FORMATNEXTLINEL, "EPosNotFormatted" );
			    }
			__ASSERT_DEBUG(isFormatted, Panic(EPosNotFormatted));
			isFormatted = iText->ParNumberToLine(info.iParNumber,KMaxTInt,info);
			if (!isFormatted)
			    {
			    OstTrace0( TRACE_DUMP, DUP1_CTEXTLAYOUT_FORMATNEXTLINEL, "EPosNotFormatted" );
			    }
			__ASSERT_DEBUG(isFormatted, Panic(EPosNotFormatted));
			aBottomPixel = info.iOuterRect.iBr.iY - iBandTop;
			}

		// Indicate that formatting is complete up to the lower edge of the current line.
		else
			aBottomPixel = result.iRedrawRect.iBr.iY - iBandTop;
		iUnformattedStart = result.iUnformattedStart;
		}
	return iUnformattedStart < KMaxTInt;
	}

/** Controls the height of a single line, for use by the pagination system
only. Using the format supplied in aParaFormat, determines the height of the
line containing aDocPos and returns it in aHeight. Changes aDocPos to the end
of the line and returns ETrue if that position is not the end of the paragraph.

Notes:

Not generally useful; it exists for use by the pagination system only.

Do not use if a CTextView object owns this CTextLayout object.

@param aParaFormat Contains paragraph formatting.
@param aDocPos A document position. On return, contains the document position
of the end of the line.
@param aHeight On return, contains the height of the formatted line containing
aDocPos.
@param aPageBreak On return, ETrue if the last character on the line is a page
break. EFalse if not.
@return ETrue if the line is not the last line in the paragraph. EFalse if it is
the last line. */
EXPORT_C TBool CTextLayout::FormatLineL(CParaFormat* /*aParaFormat*/,TInt& aDocPos,
										TInt& aHeight,TBool& aPageBreak)
	{
	// If the line is not formatted, replace the formatted text with the single paragraph containing the line.
	if (aDocPos < iText->StartChar() || aDocPos >= iText->EndChar())
		{
		TTmFormatParam param;
		InitFormatParam(param);
		param.iStartChar = iSource->ParagraphStart(aDocPos);
		param.iEndChar = iSource->ParagraphEnd(aDocPos);
		param.iMaxHeight = KMaxTInt;
		iText->SetTextL(*iSource,param);
		}
	TTmLineInfo info;
	TTmDocPos pos(aDocPos, ETrue);
	if (!iText->DocPosToLine(pos,info))
		User::Leave(KErrGeneral);
	aHeight = info.iOuterRect.Height();
	aDocPos = info.iEnd;
	TPtrC text;
	TTmCharFormat format;
	iSource->GetText(info.iEnd - 1,text,format);
	aPageBreak = text[0] == CEditableText::EPageBreak;

	return !(info.iFlags & TTmLineInfo::EParEnd);
	}

/** Scrolls the text up or down by aNumParas paragraphs, disallowing blank
space at the bottom of the visible area if aScrollBlankSpace is
CTextLayout::EFDisallowScrollingBlankSpace.

Do not use if a CTextView object owns this CTextLayout object.

@param aNumParas The number of paragraphs to scroll; may be a positive or
negative value. On return, contains the number of paragraphs not scrolled; that
is the difference between the requested number and the number of paragraphs
actually scrolled.
@param aScrollBlankSpace Only relevant when scrolling downwards.
CTextLayout::EFAllowScrollingBlankSpace allows blank space to scroll into the
visible area. CTextLayout::EFDisallowScrollingBlankSpace prevents blank space
from scrolling into the visible area.
@pre aPars must not scroll the display beyond the formatted range. If aPars
scrolls beyond the formatted range, this method will leave with the error code
CTextLayout::EPosNotFormatted
@return The number of pixels actually scrolled. */
EXPORT_C TInt CTextLayout::ScrollParagraphsL(TInt& aPars,TAllowDisallow aScrollBlankSpace)
	{
	TTmFormatParam param;
	InitFormatParam(param);
	param.iMaxHeight = KMaxTInt;
	TTmLineInfo info;
	int height_increase = 0;
	int paragraphs_increase = 0;
	int cur_par = 0;
	int desired_par = 0;
	int pixels_to_scroll = 0;

	if (aPars > 0)
		{
		if (iText->YPosToLine(iBandTop,info))
			cur_par = info.iParNumber;
		else
			User::Leave(EPosNotFormatted);
		if (info.iParTop < iBandTop)
			aPars--;
		desired_par = cur_par - aPars;
		while (desired_par < 0 && iText->AddParL(param,TRUE,height_increase,paragraphs_increase))
			{
			iBandTop += height_increase;
			desired_par += paragraphs_increase;
			}
		aPars = -desired_par;
		if (desired_par < 0)
			desired_par = 0;
		if (!iText->ParNumberToLine(desired_par,0,info))
			User::Leave(EPosNotFormatted);
		pixels_to_scroll = iBandTop - info.iOuterRect.iTl.iY;
		}
	else if (aPars < 0)
		{
		int band_bottom = iBandTop + VisibleHeightInPixels();

		// Extend formatting until the visible height is filled.
		param.iEndChar = KMaxTInt;
		param.iMaxHeight = band_bottom - iText->LayoutHeight();
		if(param.iMaxHeight > 0)
			{
			iText->ExtendFormattingDownwardsL(param);
			}

		if (iText->YPosToLine(band_bottom - 1,info))
			cur_par = info.iParNumber;
		else
			User::Leave(EPosNotFormatted);
		if (!((info.iFlags & TTmLineInfo::EParEnd) && info.iOuterRect.iBr.iY == band_bottom))
			aPars++;
		desired_par = cur_par - aPars;
		int last_par = iText->Paragraphs() - 1;
		while (desired_par > last_par && iText->AddParL(param, EFalse, height_increase,paragraphs_increase))
			last_par += paragraphs_increase;
		aPars = last_par - desired_par;
		if (desired_par > last_par)
			desired_par = last_par;
		if (!iText->ParNumberToLine(desired_par,KMaxTInt,info))
			iText->AddParL(param, EFalse, height_increase, paragraphs_increase);
		if (!iText->ParNumberToLine(desired_par,KMaxTInt,info))
			User::Leave(EPosNotFormatted);
		pixels_to_scroll = band_bottom - info.iOuterRect.iBr.iY;
		}

	return ScrollL(pixels_to_scroll,aScrollBlankSpace);
	}

/** Scrolls the text up or down by aNumLines lines, disallowing blank space at
the bottom of the visible area if aScrollBlankSpace is
CTextLayout::EFDisallowScrollingBlankSpace.

Do not use if a CTextView object owns this CTextLayout object.

@param aNumLines The number of lines to scroll; may be a positive or negative
value. On return, contains the number of lines not scrolled; that is, the
requested number, minus the number actually scrolled.
@param aScrollBlankSpace Only relevant when scrolling downwards.
CTextLayout::EFAllowScrollingBlankSpace allows blank space to scroll into the
visible area. CTextLayout::EFDisallowScrollingBlankSpace prevents blank space
from scrolling into the visible area.
@pre aLines must not scroll the display beyond the formatted range. If aLines
scrolls beyond the formatted range, this method will leave with the error code
CTextLayout::EPosNotFormatted
@return The number of pixels actually scrolled. */
EXPORT_C TInt CTextLayout::ScrollLinesL(TInt& aLines,TAllowDisallow aScrollBlankSpace)
	{
	TTmFormatParam param;
	InitFormatParam(param);
	param.iMaxHeight = KMaxTInt;
	TTmLineInfo info;
	int height_increase = 0;
	int paragraphs_increase = 0;
	int lines_scrolled = aLines;
	int visible_height = VisibleHeightInPixels();
	if (aLines > 0)			// bring aLines lines into view at the top of the display
		{
		int desired_top_line_number = 0;
		for (;;)
			{
			// Find the line number of the first visible line.
			TBool partial_line = FALSE;
			int top_line_number = 0;
			if (iBandTop >= iText->LayoutHeight())
				top_line_number = iText->Lines();
			else
				{
				if (iText->YPosToLine(iBandTop,info))
					top_line_number = info.iLineNumber;
				else
				    {
					OstTrace0( TRACE_DUMP, DUP1_CTEXTLAYOUT_SCROLLLINESL, "EPosNotFormatted" );
					User::Leave(EPosNotFormatted);
					}
				partial_line = iBandTop > info.iOuterRect.iTl.iY;
				}

			// Find the line number of the desired first visible line.
			// Defect fix for INC015850. Changed IF so that if the currently
			// visible top line is as tall or taller (due to a large font or picture)
			// than the visible height of the view our desired top line remains
			// the next one above so that a scroll takes place.
			desired_top_line_number = top_line_number - aLines;
			if (partial_line && (info.iOuterRect.iBr.iY-info.iOuterRect.iTl.iY < visible_height))
				desired_top_line_number++;

			// If the desired first visible line number is negative, more lines need to be formatted.
			if (desired_top_line_number >= 0)
				break;
			if (!iText->AddParL(param,TRUE,height_increase,paragraphs_increase))
				break;
			iBandTop += height_increase;
			}

		if (desired_top_line_number < 0)
			{
			lines_scrolled += desired_top_line_number;
			desired_top_line_number = 0;
			}
		aLines -= lines_scrolled;
		if (lines_scrolled)
			{
			if (!iText->LineNumberToLine(desired_top_line_number,info))
			    {
			    OstTrace0( TRACE_DUMP, CTEXTLAYOUT_SCROLLLINESL, "EPosNotFormatted" );
			    User::Leave(EPosNotFormatted);
			    }
			// if the line to be scrolled to is taller than the screen, we want
			// to make sure that the baseline is not scrolled off the screen.
			if (visible_height < info.iBaseline - info.iOuterRect.iTl.iY)
				return ScrollL(iBandTop + visible_height - info.iBaseline, aScrollBlankSpace);
			return ScrollL(iBandTop - info.iOuterRect.iTl.iY,aScrollBlankSpace);
			}
		else
			return 0;
		}
	else if (aLines < 0)	// bring aLines lines into view at the bottom of the display
		{
		if (iScrollFlags & EFScrollOnlyToTopsOfLines)
			{
			// If we are restricting scroll to the tops of lines, then lines at
			// bottom are irrelevant, so all we do is lose the top line.
			if (!iText->YPosToLine(iBandTop, info))
			    {
			    OstTrace0( TRACE_DUMP, DUP2_CTEXTLAYOUT_SCROLLLINESL, "EPosNotFormatted" );
			    User::Leave(EPosNotFormatted);
			    }
			return ScrollL(-info.iOuterRect.Height(), aScrollBlankSpace);
			}

		int desired_bottom_line_number = 0;
		int band_bottom = iBandTop + visible_height;
		int last_formatted_line = iText->Lines() - 1;

		// Extend formatting until the visible height is filled.
		param.iEndChar = KMaxTInt;
		param.iMaxHeight = band_bottom - iText->LayoutHeight();
		if(param.iMaxHeight > 0)
			{
			iText->ExtendFormattingDownwardsL(param);
			}	
		for (;;)
			{
			// Find the line number of the last visible line.
			TBool partial_line = FALSE;
			int bottom_line_number = 0;
			if (iText->YPosToLine(band_bottom - 1,info))
				bottom_line_number = info.iLineNumber;
			else
			    {
			    OstTrace0( TRACE_DUMP, DUP3_CTEXTLAYOUT_SCROLLLINESL, "EPosNotFormatted" );
			    User::Leave(EPosNotFormatted);
			    }
			partial_line = band_bottom < info.iOuterRect.iBr.iY;

			// Find the line number of the desired last visible line.
			desired_bottom_line_number = bottom_line_number - aLines;
			if (partial_line)
				desired_bottom_line_number--;

			// If the desired last visible line number is greater than the last line, more lines need to be formatted.
			if (desired_bottom_line_number <= last_formatted_line)
				break;
			if (!AddFormattingAtEndL(param, height_increase,paragraphs_increase))
				break;
			last_formatted_line = iText->Lines() - 1;
			}

		int shortfall = desired_bottom_line_number - last_formatted_line;
		if (shortfall > 0)
			{
			lines_scrolled += shortfall;
			desired_bottom_line_number = last_formatted_line;
			}
		aLines -= lines_scrolled;
		if (lines_scrolled)
			{
			if (!iText->LineNumberToLine(desired_bottom_line_number,info))
			    {
			    OstTrace0( TRACE_DUMP, DUP4_CTEXTLAYOUT_SCROLLLINESL, "EPosNotFormattedL" );
			    User::Leave(EPosNotFormatted);
			    }
			return ScrollL(band_bottom - info.iOuterRect.iBr.iY,aScrollBlankSpace);
			}
		else
			return 0;
		}
	else
		return 0;
	}

/** Scrolls the text up or down by aPixels pixels, disallowing blank space at
the bottom of the visible area if aScrollBlankSpace is
CTextLayout::EFDisallowScrollingBlankSpace.

The return value (not aPixels, as you would expect from ScrollParagraphsL() and
ScrollLinesL()) contains the number of pixels not successfully scrolled, that
is, the original value of aPixels, minus the number of pixels actually
scrolled. On return, aPixels is set to the number of pixels actually scrolled.

Do not use if a CTextView object owns this CTextLayout object.

@param aPixels The number of pixels to scroll; may be a positive or negative
value. On return, contains the number of pixels actually scrolled.
@param aScrollBlankSpace Only relevant when scrolling downwards.
CTextLayout::EFAllowScrollingBlankSpace allows blank space to scroll into the
visible area. CTextLayout::EFDisallowScrollingBlankSpace prevents blank space
from scrolling into the visible area.
@return The difference between the requested number of pixels to scroll and the
number of pixels actually scrolled. */
EXPORT_C TInt CTextLayout::ChangeBandTopL(TInt& aPixels,TAllowDisallow aScrollBlankSpace)
	{
	int desired_pixels = aPixels;
	aPixels = ScrollL(aPixels,aScrollBlankSpace);
	return desired_pixels - aPixels;
	}



/** Scrolls the text up or down by aPixels pixels, allowing blank space at
top and bottom of the visible area, which means the scrolling can go beyond the
top or bottom border.

Do not use if a CTextView object owns this CTextLayout object.

@param aPixels The number of pixels to scroll; may be a positive or negative
value. The actual scrolled pixel number is always identical to aPixels*/
EXPORT_C void CTextLayout::ChangeBandTopNoLimitBorderL(TInt aPixels)
    {
    ScrollL(aPixels,EFAllowScrollingBlankSpace,ETrue,ETrue);
    }


/**
 * Finds a position (in pixels, from the top of the window) where the cursor
 * can be such that its line will be fully visible. If this is not possible,
 * then return one with its baseline visible. If this is not possible, we don't
 * care what the answer is.
 */
TInt CTextLayout::SuggestCursorPos(TInt aCurrentCursorPos) const
	{
	int visible_height = VisibleHeightInPixels();
	TTmLineInfo info;
	if (iText->YPosToLine(iBandTop + aCurrentCursorPos, info))
		{
		TBool currentLineHasBaselineVisible =
			(TBool)(iBandTop <= info.iBaseline
			&& info.iBaseline < iBandTop + visible_height);
		TInt tryThisLine = -1;
		// find a good line to put the cursor on.
		if (info.iOuterRect.iTl.iY < iBandTop)
			{
			// try next line
			tryThisLine = info.iLineNumber + 1;
			}
		else if (iBandTop + visible_height < info.iOuterRect.iBr.iY)
			{
			tryThisLine = info.iLineNumber - 1;
			}
		if (0 <= tryThisLine && iText->LineNumberToLine(tryThisLine, info))
			{
			if (iBandTop <= info.iOuterRect.iTl.iY
				&& info.iOuterRect.iBr.iY < iBandTop + visible_height)
				// this line fully visible
				aCurrentCursorPos = info.iBaseline - iBandTop;
			else if (!currentLineHasBaselineVisible
				&& iBandTop <= info.iBaseline
				&& info.iBaseline < iBandTop + visible_height)
				// not fully visible, but its baseline is, and the original
				// line's baseline was not
				aCurrentCursorPos = info.iBaseline - iBandTop;
			}
		}
	return aCurrentCursorPos;
	}

/** Scrolls up by a page (that is the band height as set by SetBandHeight(), or
half that amount if scrolling over lines taller than this), moving the text
downwards. The current desired vertical cursor position is passed in
aYCursorPos and updated to a new suggested position as near as possible to it,
but within the visible text and on a baseline.

Do not use if a CTextView object owns this CTextLayout object.

@param aYCursorPos The current desired vertical cursor position. On return,
updated to a new suggested position as near as possible to it.
@param aPixelsScrolled On return, contains the number of pixels scrolled. */
EXPORT_C void CTextLayout::PageUpL(TInt& aYCursorPos,TInt& aPixelsScrolled)
	{
	// Move the cursor into the visible area.
	int visible_height = VisibleHeightInPixels();
	if (aYCursorPos < 0)
		aYCursorPos = 0;
	else if (aYCursorPos > visible_height)
		aYCursorPos = visible_height - 1;

	TTmLineInfo info;
	// position the top of the screen must be at least as low as
	TInt longestScrollTo = iBandTop + visible_height
		- visible_height * KMaxProportionOfScreenToScroll/1000;
	// position the top of the screen must be at least as high as
	TInt shortestScrollTo = iBandTop + visible_height
		- visible_height * KMinProportionOfScreenToScroll/1000;
	TInt desiredScrollTo = shortestScrollTo;
	// find the line at the top of the screen
	// (we cannot find the one that includes the first pixel off the screen
	// because it might not have been formatted yet)
	if  (iText->YPosToLine(iBandTop, info))
		{
		// subtract one from the line number if this was on the screen
		TInt line = info.iLineNumber
			- (info.iOuterRect.iTl.iY < iBandTop? 0 : 1)
			+ KNumberOfLinesToKeepVisibleDuringScroll;
		if (iText->LineNumberToLine(Max(0, line), info))
			desiredScrollTo = info.iOuterRect.iTl.iY;
		}
	if (shortestScrollTo < desiredScrollTo)
		desiredScrollTo = shortestScrollTo;
	else if (desiredScrollTo < longestScrollTo)
		desiredScrollTo = longestScrollTo;
	aPixelsScrolled = ScrollL(iBandTop + visible_height - desiredScrollTo,
		EFDisallowScrollingBlankSpace);
	aYCursorPos = aPixelsScrolled == 0?
		0
		: SuggestCursorPos(aYCursorPos);
	}

/** Scrolls down by a page (that is the band height as set by SetBandHeight(),
or half that amount if scrolling over lines taller than this), moving the text
upwards. The current desired vertical cursor position is passed in aYCursorPos
and updated to a new suggested position as near as possible to it, but within
the visible text and on a baseline.

Do not use if a CTextView object owns this CTextLayout object.

@param aYCursorPos The current desired vertical cursor position. On return,
updated to a new suggested position as near as possible to it.
@param aPixelsScrolled On return, contains the number of pixels scrolled - a
negative value. */
EXPORT_C void CTextLayout::PageDownL(TInt& aYCursorPos,TInt& aPixelsScrolled)
	{
	// Move the cursor into the visible area.
	int visible_height = VisibleHeightInPixels();
	if (aYCursorPos < 0)
		aYCursorPos = 0;
	else if (aYCursorPos > visible_height)
		aYCursorPos = visible_height - 1;

	TTmLineInfo info;
	// position the bottom of the screen must be at least as high as
	TInt longestScrollTo = iBandTop +
		visible_height * KMaxProportionOfScreenToScroll/1000;
	// position the bottom of the screen must be at least as low as
	TInt shortestScrollTo = iBandTop + visible_height * KMinProportionOfScreenToScroll/1000;
	TInt desiredScrollTo = shortestScrollTo;
	// find the line at the bottom of the screen
	// (we cannot find the one that includes the first pixel off the screen
	// because it might not have been formatted yet)
	if  (iText->YPosToLine(iBandTop + visible_height - 1, info))
		{
		// add one to the line number if this was on the screen
		TInt line = info.iLineNumber
			+ (iBandTop + visible_height < info.iBaseline? 0 : 1)
			- KNumberOfLinesToKeepVisibleDuringScroll;
		if (iText->LineNumberToLine(Max(0, line), info))
			desiredScrollTo = info.iOuterRect.iBr.iY;
		}
	if (desiredScrollTo < shortestScrollTo)
		desiredScrollTo = shortestScrollTo;
	else if (longestScrollTo < desiredScrollTo)
		desiredScrollTo = longestScrollTo;
	aPixelsScrolled = ScrollL(iBandTop - desiredScrollTo,
		EFDisallowScrollingBlankSpace);
	aYCursorPos = aPixelsScrolled == 0?
		visible_height - 1
		: SuggestCursorPos(aYCursorPos);

	if (-visible_height > aPixelsScrolled)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTLAYOUT_PAGEDOWNL, "EPageScrollError" );
	    }
	__ASSERT_DEBUG(-visible_height <= aPixelsScrolled,
		Panic(EPageScrollError));
	if (0 > aYCursorPos || aYCursorPos > visible_height)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTLAYOUT_PAGEDOWNL, "EPageScrollError" );
	    }
	__ASSERT_DEBUG(0 <= aYCursorPos && aYCursorPos <= visible_height,
		Panic(EPageScrollError));
	}

/** Reformats to reflect a single character edit.

Do not use if a CTextView object owns this CTextLayout object.

@param aType Indicates the type of edit which has taken place.
CTextLayout::EFCharacterInsert (the default) for a character insertion,
CTextLayout::EFParagraphDelimiter for a paragraph delimiter insertion,
CTextLayout::EFLeftDelete or CTextLayout::EFRightDelete for a character or
paragraph delimiter deletion to the left or right of the document position.
@param aCursorPos The document position at which the edit took place, before
the edit. If this position is not formatted, a panic occurs; it is modified in
accordance with the edit.
@param aGood On return, the y coordinate of the top of the paragraph following
the paragraph which has been edited, before the edit.
@param aFormattedUpTo On return, the y coordinate of the bottom of the
reformatted line or lines, after the edit.
@param aFormattedFrom On return, the vertical layout coordinate of the top of
the reformatted line or lines, after the edit.
@param aScroll The number of pixels by which the text had to be scrolled
(positive means text moved down).
@param aFormatChanged ETrue if text is to be reformatted from the start of the
paragraph the cursor was on before the edit, EFalse if from the start of the
line the cursor was on before the edit.
@return EFalse if no more lines need to be reformatted. ETrue if some more lines
need to be reformatted. */
EXPORT_C TBool CTextLayout::HandleCharEditL(TUint aType,TInt& aCursorPos,TInt& aGood,TInt& aFormatBottom,
											TInt& aFormatTop,TInt& aScroll,TBool aFormatFromStartOfPar)
	{
	if (iSource->iFormatMode == CLayoutData::EFPrintPreviewMode)
	    {
	    OstTrace0( TRACE_FATAL, DUP2_CTEXTLAYOUT_HANDLECHAREDITL, "EPrintPreviewModeError" );
	    }
	__ASSERT_ALWAYS(iSource->iFormatMode != CLayoutData::EFPrintPreviewMode,Panic(EPrintPreviewModeError));
	if (aType > EFRightDelete)
	    {
	    OstTrace0( TRACE_FATAL, CTEXTLAYOUT_HANDLECHAREDITL, "EBadCharacterEditType" );
	    }
	__ASSERT_ALWAYS(aType <= EFRightDelete,Panic(EBadCharacterEditType));
	if (!(!aFormatFromStartOfPar || aType == EFRightDelete || aType == EFLeftDelete))
	    {
	    OstTrace0( TRACE_FATAL, DUP1_CTEXTLAYOUT_HANDLECHAREDITL, "EBadCharacterEditType" );
	    }
	__ASSERT_ALWAYS(!aFormatFromStartOfPar || aType == EFRightDelete || aType == EFLeftDelete,Panic(EBadCharacterEditType));
	if (aCursorPos < iText->StartChar() || aCursorPos >= iText->EndChar())
	    {
	    OstTrace0( TRACE_FATAL, DUP3_CTEXTLAYOUT_HANDLECHAREDITL, "ECharacterNotFormatted" );
	    }
	__ASSERT_ALWAYS(aCursorPos >= iText->StartChar() && aCursorPos < iText->EndChar(),Panic(ECharacterNotFormatted));

	// Mark the entire paragraph invalid if background formatting is taking place.
	iParInvalid = iUnformattedStart != KMaxTInt;

	// Cancel any pending background formatting, which has now been invalidated by the change.
	iUnformattedStart = KMaxTInt;

	// Adjust the cursor position and determine the range of characters to reformat.
	TTmReformatParam reformat_param;
	reformat_param.iParInvalid = iParInvalid;
	switch (aType)
		{
		case EFCharacterInsert:
		case EFParagraphDelimiter:
			{
			reformat_param.iStartChar = aCursorPos++;
			reformat_param.iNewLength = 1;

			// surrogate support
			TPtrC textForSurrogate;
			TTmCharFormat formatForSurrogate;
		
			iSource->GetText(aCursorPos-1, textForSurrogate, formatForSurrogate);
			if ( textForSurrogate.Length() > 1 )
				{
				TUint highSurrogate = textForSurrogate[0];
				TUint lowSurrogate = textForSurrogate[1];
				if ( TChar::IsHighSurrogate( highSurrogate ) && TChar::IsLowSurrogate( lowSurrogate ) )
					{
					// if we are inserting a surrogate pair, do not break the pair
					aCursorPos++;
					reformat_param.iNewLength++;
					}
				}
			break;
			}
		case EFLeftDelete:
			{
			reformat_param.iStartChar = --aCursorPos;
			reformat_param.iOldLength = 1;

			// surrogate support
			TPtrC textForSurrogate;
			TTmCharFormat formatForSurrogate;
		
			if (aCursorPos >= 1)
				{
				iSource->GetText(aCursorPos-1, textForSurrogate, formatForSurrogate);
				if ( textForSurrogate.Length() > 1 )
					{
					TUint highSurrogate = textForSurrogate[0];
					TUint lowSurrogate = textForSurrogate[1];
					if ( TChar::IsHighSurrogate( highSurrogate ) && TChar::IsLowSurrogate( lowSurrogate ) )
						{
						// if we are deleting a surrogate pair, do not break the pair
						reformat_param.iStartChar = --aCursorPos;
						reformat_param.iOldLength++;
						}
					}
				}
			break;
			}
		case EFRightDelete:
			{
			reformat_param.iStartChar = aCursorPos;
			reformat_param.iOldLength = 1;

			// surrogate support
			TPtrC textForSurrogate;
			TTmCharFormat formatForSurrogate;
		
			iSource->GetText(aCursorPos, textForSurrogate, formatForSurrogate);
			if ( textForSurrogate.Length() > 1 )
				{
				TUint highSurrogate = textForSurrogate[0];
				TUint lowSurrogate = textForSurrogate[1];
				if ( TChar::IsHighSurrogate( highSurrogate ) && TChar::IsLowSurrogate( lowSurrogate ) )
					{
					reformat_param.iOldLength++;
					}
				}
			break;
			}
		default: break;
		}

	// Set up the formatting parameters.
	TTmFormatParam param;
	InitFormatParam(param);
	param.iMaxHeight = KMaxTInt;

	// Format the whole band if necessary.
	TTmDocPos cursorPos(aCursorPos, EFalse);
	if (reformat_param.iStartChar < iText->StartChar() || reformat_param.iStartChar + reformat_param.iOldLength >= iText->EndChar())
		{
		FormatBandL(reformat_param.iStartChar,reformat_param.iStartChar + reformat_param.iOldLength);
		aFormatTop = 0;
		aFormatBottom = iText->LayoutHeight() - iBandTop;
		aGood = aFormatBottom;
		aScroll = 0;
		ScrollDocPosIntoViewL(cursorPos);
		return FALSE;
		}

	// Reformat the chosen range.
	reformat_param.iMaxExtraLines = KMaxExtraLines;
	TTmReformatResult result;
	iText->FormatL(param,reformat_param,result);
	result.iRedrawRect.Move(0,-iBandTop);
	iUnformattedStart = result.iUnformattedStart;

	// Scroll if necessary and extend the redraw area to include material scrolled into view.
	aScroll = ScrollDocPosIntoViewL(cursorPos);
	if (aScroll > 0)
		result.iRedrawRect.iBr.iY += aScroll;
	else
		result.iRedrawRect.iTl.iY += aScroll;

	// Return coordinates.
	aFormatTop = result.iRedrawRect.iTl.iY;
	aFormatBottom = result.iRedrawRect.iBr.iY;
	aGood = result.iUnchangedTop - iBandTop;

	// Add new paragraphs if necessary.
	int visible_height = VisibleHeightInPixels();
	
	param.iEndChar = KMaxTInt;
	param.iMaxHeight = (iBandTop + visible_height) - iText->LayoutHeight();
	if(param.iMaxHeight > 0)
		{
		iText->ExtendFormattingDownwardsL(param);
		}
	
	//remove formatting from the end if necessary
	PruneFormatL(EFalse);
	// Return TRUE if more text needs to be formatted.
	return iUnformattedStart < KMaxTInt;
	}

/** Reformats to reflect changes to a block of text.

Do not use if a CTextView object owns this CTextLayout object.

@param aSelection The start and new length of the changed block. When
inserting, specify the insertion position. When deleting, specify the position
of the start of the deletion. When reformatting, specify the start and length
of the reformatted block.
@param aOldCharsChanged The old length of the changed block. When inserting,
specify zero. When deleting, specify the number of deleted characters. When
reformatting, specify the number of reformatted characters.
@param aViewChanges On return, contains the top of the reformatted text
(iFormattedFrom), the bottom of the reformatted text (iFormattedTo), the amount
by which the text above the reformatted text has scrolled (iScrollAtTop) and
the amount by which the text below the reformatted text has scrolled
(iScrollAtBottom) (positive values mean the text moves down).
@param aFormatChanged Indicates whether the paragraph format for the first or
last affected paragraph has changed, meaning that the text to be reformatted
must extend out to paragraph boundaries and cannot be restricted to only some
lines. */
EXPORT_C void CTextLayout::HandleBlockChangeL(TCursorSelection aSelection,TInt aOldLength,
											  TViewRectChanges& aChanges,TBool aFormatFromStartOfPar)
	{
	if (iSource->iFormatMode == CLayoutData::EFPrintPreviewMode)
	    {
	    OstTrace0( TRACE_FATAL, CTEXTLAYOUT_HANDLEBLOCKCHANGEL, "EPrintPreviewModeError" );
	    }
	__ASSERT_ALWAYS(iSource->iFormatMode != CLayoutData::EFPrintPreviewMode,Panic(EPrintPreviewModeError));

	// Do nothing if the selection is outside the formatted range.
	if (aSelection.LowerPos() > iText->EndChar() || aSelection.HigherPos() < iText->StartChar())
		{
		aChanges.iFormattedFrom = 0;
		aChanges.iFormattedTo = 0;
		aChanges.iScrollAtTop = 0;
		aChanges.iScrollAtBottom = 0;
		return;
		}

	// Format the whole band if necessary.
	TTmDocPos cursorPos(aSelection.iCursorPos,
		aSelection.iCursorPos < aSelection.iAnchorPos? ETrue : EFalse);
	if (aSelection.LowerPos() < iText->StartChar() || aSelection.LowerPos() + aOldLength >= iText->EndChar())
		{
		FormatBandL(aSelection.iCursorPos,aSelection.iCursorPos);
		aChanges.iFormattedFrom = 0;
		aChanges.iFormattedTo = VisibleHeightInPixels();
		aChanges.iScrollAtTop = 0;
		aChanges.iScrollAtBottom = 0;
		ScrollDocPosIntoViewL(cursorPos);
		return;
		}

	// Reformat the chosen range.
	TTmFormatParam param;
	InitFormatParam(param);
	param.iMaxHeight = KMaxTInt;
	TTmReformatParam reformat_param;
	reformat_param.iStartChar = aSelection.LowerPos();
	reformat_param.iOldLength = aOldLength;
	reformat_param.iNewLength = aSelection.Length();
	reformat_param.iParFormatChanged = aFormatFromStartOfPar;
	TTmReformatResult result;
	iText->FormatL(param,reformat_param,result);
	result.iRedrawRect.Move(0,-iBandTop);

	// Scroll if necessary.
	int dy;
	if(iTextViewCursorPos)
		{
		dy = ScrollDocPosIntoViewL(iTextViewCursorPos->TmDocPos());
		iTextViewCursorPos = NULL;
		}
	else
		{
		dy = ScrollDocPosIntoViewL(cursorPos);
		}
	result.iRedrawRect.Move(0,dy);

	aChanges.iFormattedFrom = result.iRedrawRect.iTl.iY;
	aChanges.iFormattedTo = result.iRedrawRect.iBr.iY;
	aChanges.iScrollAtTop = dy;
	aChanges.iScrollAtBottom = dy + result.iHeightChange;

	// Extend formatting to fill the band if necessary.
	int visible_height = VisibleHeightInPixels();

	param.iEndChar = KMaxTInt;
	param.iMaxHeight = (iBandTop + visible_height) - iText->LayoutHeight();
	if(param.iMaxHeight > 0)
		{
		iText->ExtendFormattingDownwardsL(param);
		}

	//remove formatting from the end if necessary
	PruneFormatL(EFalse);
	}

/** Reformats to reflect the addition of one or more complete paragraphs at the
end of the text.

Do not use if a CTextView object owns this CTextLayout object.

@param aFirstPixel On return, the top y coordinate of the added material.
@param aLastPixel On return, the bottom y coordinate of the added material.
*/
EXPORT_C void CTextLayout::HandleAdditionalCharactersAtEndL(TInt& aNewTextTop,TInt& aNewTextBottom)
	{
	aNewTextTop = aNewTextBottom = iText->LayoutHeight() - iBandTop;
	int format_required = BandHeightInPixels() - aNewTextBottom;
	if (format_required > 0)
		{
		TTmFormatParam param;
		InitFormatParam(param);
		param.iMaxHeight = format_required;
		TInt oldHeight = iText->LayoutHeight();
		iText->ExtendFormattingDownwardsL(param);
		aNewTextBottom += (iText->LayoutHeight() - oldHeight);
		}
	}

/** Reformats to reflect changes to the space above and below paragraphs
(CParaFormat::iSpaceBeforeInTwips and iSpaceAfterInTwips).

Do not use if a CTextView object owns this CTextLayout object.
@deprecated 6.1 Use FormatBandL()
*/
EXPORT_C void CTextLayout::ReformatVerticalSpaceL()
	{
	// Reformat the whole band; only space above and below paragraphs has changed, but who cares?
	FormatBandL();
	}

/** Temporarily changes the vertical alignment of the text with respect to the
visible height.

Notes:

Not generally useful.

Do not use if a CTextView object owns this CTextLayout object.

@param aVerticalAlignment Specifies whether the formatted text should be placed
at the top (CParaFormat::ETopAlign), vertical centre (CParaFormat::ECenterAlign
or CParaFormat::EJustifiedAlign) or bottom (CParaFormat::EBottomAlign) of the
band. CParaFormat::EUnspecifiedAlign or CParaFormat::ECustomAlign may also be
specified. These values cause the baseline of the first formatted line to be
positioned 82% of the way down the band (provided for the Agenda's
application's year view). */
EXPORT_C void CTextLayout::AdjustVerticalAlignment(CParaFormat::TAlignment aVerticalAlignment)
	{
	int excess = BandHeight() - FormattedHeightInPixels();
	int space_before = 0;
	
	if (IsFormattingBand())
	    {
	    OstTrace0( TRACE_FATAL, CTEXTLAYOUT_ADJUSTVERTICALALIGNMENT, "EMustFormatAllText" );
	    }
	__ASSERT_ALWAYS(!IsFormattingBand(),Panic(EMustFormatAllText));
	TTmLineInfo info;

	switch (aVerticalAlignment)
		{
		case CParaFormat::EAbsoluteLeftAlign:
		case CParaFormat::ETopAlign:
			break;
		case CParaFormat::ECenterAlign:
		case CParaFormat::EJustifiedAlign:
			space_before = excess / 2;
			break;
		case CParaFormat::EAbsoluteRightAlign:
		case CParaFormat::EBottomAlign:
			space_before = excess;
			break;
		case CParaFormat::EUnspecifiedAlign:
		case CParaFormat::ECustomAlign:
			if (iText->LineNumberToLine(0,info))
				{
				space_before = CLayoutData::EFBaseLinePosition * BandHeight() / 100 - info.iBaseline;
				}
			else
				{
				space_before = CLayoutData::EFBaseLinePosition * BandHeight() / 100;
				}
			break;
		}

	iBandTop = -space_before;
	}

static TInt SingleBorderWidthInPixels(const MGraphicsDeviceMap* aGd,const TParaBorder& aBorder,TBool aHoriz)
	{
	TInt width = aBorder.iThickness;
	if (width > 0)
		{
		if (aHoriz)
			width = aGd->VerticalTwipsToPixels(width);
		else
			width = aGd->HorizontalTwipsToPixels(width);
		}
	else
		width = 1;
	return width;
	}

/** Draws paragraph borders, optionally with a background colour for the border
and a clip region. Provided for applications that display a menu of border
styles, like a wordprocessor.

@param aGd Provides twip-to-pixel conversion.
@param aGc Graphics context to which to draw the border. Its pen settings are
overridden by the values specified by aBorder and its draw mode is set to
CGraphicsContext::EDrawModePEN.
@param aRect The outer bounds of the border.
@param aBorder Specifies the four sides of the border.
@param aBackground If not null, the background colour, (used between double
border lines, or between dots or dashes).
@param aClipRegion If non-null, specifies a clip region.
@param aDrawRect If non-null, and if aClipRegion is non-null, specifies a
rectangle to be subtracted from the clip region. */
EXPORT_C void CTextLayout::DrawBorders(const MGraphicsDeviceMap* aGd,CGraphicsContext& aGc,const TRect& aBoundingRect,
									   const TParaBorderArray& aBorder,const TRgb* aBackground,TRegion *aClipRegion,
									   const TRect* aDrawRect)
	{
	TPoint pointTl,pointBr;
	TRect rect;
	TInt * ptrStartLength=NULL;			//To stop a warning
	TInt * ptrEndLength=NULL;			//To stop a warning
	TInt * ptrStartWidth=NULL;			//To stop a warning
	TInt * ptrEndWidth=NULL;			//To stop a warning
	TInt directionOut=0;			//To stop a warning
	TInt indexJoint1=0,indexJoint2=0;			//To stop a warning
	TBool drawAsLine=EFalse;
	TRect clipRect;
	CGraphicsContext::TPenStyle	penStyle[4];
	TInt widthInPixels[4];
	TBool horiz;
	const MFormParam* form_param = MFormParam::Get();
		
	{for (TInt border=0; border<=3; border++)
		{
		TParaBorder::TLineStyle lineStyle=aBorder.iBorder[border].iLineStyle;
		if (lineStyle == TParaBorder::ENullLineStyle)
			{
			penStyle[border]=CGraphicsContext::ENullPen;
			widthInPixels[border]=0;
			continue;
			}
		else
			{
			horiz=(border==CParaFormat::EParaBorderTop || border==CParaFormat::EParaBorderBottom);

			widthInPixels[border]=SingleBorderWidthInPixels(aGd,aBorder.iBorder[border],horiz);
			}

		if (lineStyle==TParaBorder::ESolid || lineStyle==TParaBorder::EDouble)
			{
			penStyle[border]=CGraphicsContext::ESolidPen;
			continue;
			}

		if (lineStyle == TParaBorder::EDashed)
			penStyle[border]=CGraphicsContext::EDashedPen;
		else if (lineStyle == TParaBorder::EDotted)
			penStyle[border]=CGraphicsContext::EDottedPen;
		else if (lineStyle == TParaBorder::EDotDash)
			penStyle[border]=CGraphicsContext::EDotDashPen;
		else if (lineStyle == TParaBorder::EDotDotDash)
			penStyle[border]=CGraphicsContext::EDotDotDashPen;

		}}

	{for (TInt border=0; border<=3; border++)
		{
// Go to next border, if have NULL linestyle.
		if (widthInPixels[border]==0)
			continue;

// Reset clipping region
		clipRect.SetSize(TSize(0,0));

// Draw as line if not solid lines.

		if (penStyle[border]!=CGraphicsContext::ESolidPen)
			drawAsLine=ETrue;
			
		pointTl=aBoundingRect.iTl;
		pointBr=aBoundingRect.iBr;

		if (border==CParaFormat::EParaBorderLeft)
			{
			pointBr.iX=pointTl.iX;
			ptrStartLength=&pointTl.iY;
			ptrEndLength=&pointBr.iY;
			ptrStartWidth=&pointTl.iX;
			ptrEndWidth=&pointBr.iX;
			directionOut=-1;
			indexJoint1=CParaFormat::EParaBorderTop;
			indexJoint2=CParaFormat::EParaBorderBottom;
			}
		if (border == CParaFormat::EParaBorderRight)
			{
			pointTl.iX=pointBr.iX;
			ptrStartLength=&pointTl.iY;
			ptrEndLength=&pointBr.iY;
			ptrStartWidth=&pointTl.iX;
			ptrEndWidth=&pointBr.iX;
			directionOut=1;
			indexJoint1=CParaFormat::EParaBorderTop;
			indexJoint2=CParaFormat::EParaBorderBottom;
			}
		if (border == CParaFormat::EParaBorderTop)
			{
			pointBr.iY=pointTl.iY;
			ptrStartLength=&pointTl.iX;
			ptrEndLength=&pointBr.iX;
			ptrStartWidth=&pointTl.iY;
			ptrEndWidth=&pointBr.iY;
			directionOut=-1;
			indexJoint1=CParaFormat::EParaBorderLeft;
			indexJoint2=CParaFormat::EParaBorderRight;
			}
		if (border == CParaFormat::EParaBorderBottom)
			{
			pointTl.iY=pointBr.iY;
			ptrStartLength=&pointTl.iX;
			ptrEndLength=&pointBr.iX;
			ptrStartWidth=&pointTl.iY;
			ptrEndWidth=&pointBr.iY;
			directionOut=1;
			indexJoint1=CParaFormat::EParaBorderLeft;
			indexJoint2=CParaFormat::EParaBorderRight;
			}

		if (!ptrStartWidth || !ptrEndWidth)
			{
			continue;
			}

		if (drawAsLine)
			{
			if (directionOut<0)
				{
				(*ptrStartWidth)+=(widthInPixels[border]-1)/2;
				(*ptrEndWidth)+=(widthInPixels[border]-1)/2;
				}
			else
				{
				(*ptrStartWidth)-=(widthInPixels[border]+2)/2;
				(*ptrEndWidth)-=(widthInPixels[border]+2)/2;
				}
			(*ptrStartLength)+=(widthInPixels[border]-1)/2;
			(*ptrEndLength)-=(widthInPixels[border])/2;
			}
		else
			{
			if (directionOut<0)
				(*ptrEndWidth)+=widthInPixels[border];
			else
				(*ptrStartWidth)-=widthInPixels[border];
			}

// Colour of pen as is - NO logical combination with background etc.
		aGc.SetDrawMode(CGraphicsContext::EDrawModePEN);

		if (drawAsLine)
			{
// Must draw lines in background colour first, as have dotted/dashed lines.
			aGc.SetPenSize(TSize(widthInPixels[border],widthInPixels[border]));
			if (aBackground)
				{
				FormUtil::SetPenColor(form_param,&aGc,*aBackground);
				aGc.SetPenStyle(CGraphicsContext::ESolidPen);
				aGc.DrawLine(pointTl,pointBr);
				}
			FormUtil::SetPenColor(form_param,&aGc,aBorder.iBorder[border].iColor);
			aGc.SetPenStyle(penStyle[border]);
			aGc.DrawLine(pointTl,pointBr);
			(*ptrStartWidth)-=(widthInPixels[border]-1)/2;
			(*ptrEndWidth)+=(widthInPixels[border]+2)/2;
			(*ptrStartLength)-=(widthInPixels[border]-1)/2;
			(*ptrEndLength)+=(widthInPixels[border])/2;
			clipRect.SetRect(pointTl,pointBr);
			}
		else
			{
// Brush
			FormUtil::SetBrushColor(form_param,&aGc,aBorder.iBorder[border].iColor);
			aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
			aGc.SetPenStyle(CGraphicsContext::ENullPen);
			rect.SetRect(pointTl,pointBr);
			aGc.DrawRect(rect);
			clipRect=rect;
			};

// Repeat draw, for double border.
		if (aBorder.iBorder[border].iLineStyle==TParaBorder::EDouble)	 // Now have only got solid border, drawn as rect.
			{
			if (drawAsLine)
			    {
			    OstTrace0( TRACE_DUMP, CTEXTLAYOUT_DRAWBORDERS, "EDrawingBorderError" );
			    }
			__ASSERT_DEBUG(!drawAsLine,Panic(EDrawingBorderError));
			(*ptrStartWidth)-=directionOut*widthInPixels[border];
			(*ptrEndWidth)-=directionOut*widthInPixels[border];
			(*ptrStartLength)+=widthInPixels[indexJoint1];
			(*ptrEndLength)-=widthInPixels[indexJoint2];

			if (aBackground)
				{
				rect.SetRect(pointTl,pointBr);
				FormUtil::SetBrushColor(form_param,&aGc,*aBackground);
				aGc.DrawRect(rect);
				}

			(*ptrStartWidth)-=directionOut*widthInPixels[border];
			(*ptrEndWidth)-=directionOut*widthInPixels[border];

			if (aBorder.iBorder[indexJoint1].iLineStyle==TParaBorder::EDouble)
				(*ptrStartLength)+=widthInPixels[indexJoint1];
			if (aBorder.iBorder[indexJoint2].iLineStyle==TParaBorder::EDouble)
				(*ptrEndLength)-=widthInPixels[indexJoint2];

			rect.SetRect(pointTl,pointBr);
			FormUtil::SetBrushColor(form_param,&aGc,aBorder.iBorder[border].iColor);
			aGc.DrawRect(rect);
			clipRect.BoundingRect(rect);
			};


// Restore defaults
		aGc.SetPenStyle(CGraphicsContext::ESolidPen);
		aGc.SetPenSize(TSize(1,1));
// Should change to BACKGROUND colour.
		if (aBackground)
			FormUtil::SetBrushColor(form_param,&aGc,*aBackground);
// If have to clip region, then remove rectangles corresponding to the border region.

		if (aClipRegion)
			{
			if (aDrawRect)
				clipRect.Intersection(*aDrawRect);
			aClipRegion->SubRect(clipRect);
			}

	}};

	if (aClipRegion)
		{
		aClipRegion->Tidy();
		((CWindowGc *) &aGc)->SetClippingRegion(*aClipRegion);
		}
	
	}

/** Draws the text. Draws any lines that intersect aDrawRect, which is
specified in window coordinates. The drawing parameters, including the graphics
context, are given in aDrawTextLayoutContext. If aHighlight is non-null,
highlights (by exclusive-ORing) the specified range of text.

@param aDrawRect The function draw the lines within the visible area, which
intersect this rectangle (which is specified in window coordinates).
@param aDrawTextLayoutContext Provides a graphics context and other parameters
for the function.
@param aHighlight If not NULL, this range of text is drawn highlighted. */
EXPORT_C void CTextLayout::DrawL(const TRect& aDrawRect,const TDrawTextLayoutContext* aDrawTextLayoutContext,
								 const TCursorSelection* aHighlight)
	{
	// Set the drawing parameters in the MTmSource interface.
	iSource->iFormParam = MFormParam::Get();
	iSource->iLabelsGutter = aDrawTextLayoutContext->iGutterMarginWidth;

	// Calculate the top left corner of the text.
	TPoint top_left = aDrawTextLayoutContext->TopLeftText();
	top_left.iY -= iBandTop;

	// Get the graphics context.
	CGraphicsContext* gc = aDrawTextLayoutContext->PrimaryGc();

	/*
	Clip the draw rectangle to the view rectangle, and clip it further to exclude
	a final partial line if necessary.
	*/
	TRect clip_rect(aDrawRect);
	clip_rect.Intersection(aDrawTextLayoutContext->iViewRect);


	if (aDrawTextLayoutContext->UseClippingRect())
		{
		gc->SetClippingRect(clip_rect);
		}

	// Draw the text and background.

	TBool isDrawingOnAWindowDC = aDrawTextLayoutContext->UseWindowGc();

	if (isDrawingOnAWindowDC)
		{
		BeginRedraw(clip_rect);
		}

	iText->DrawLayout(*gc,top_left,clip_rect,&aDrawTextLayoutContext->iBackgroundColor,TRUE, aHighlight, iHighlightExtensions);

	if (isDrawingOnAWindowDC)
		{
		EndRedraw();
		}

	iSource->iFormParam = NULL;
	}
	
/** Default constructor. 
	
The start and end positions are set to zero - Set() should be called to 
initialise the object. */	
EXPORT_C CTextLayout::TRangeChange::TRangeChange()
	: iA(0), iB(0) {}

/** Sets the start and end positions and whether the highlighting should be
set or cleared.

Called by the non-default constructor. The start and end positions can be
specified in any order.

@param aStart The start of the range.
@param aEnd The end of the range.
@param aChange Whether the highlighting should be set or cleared. */
EXPORT_C void CTextLayout::TRangeChange::Set(TInt aStart, TInt aEnd,
	CTextLayout::TRangeChange::TChangeType aChange)
	{
	if ((aChange == ESet && aStart < aEnd)
		|| (!(aChange == ESet) && !(aStart < aEnd)))
		{
		iA = aStart;
		iB = aEnd;
		}
	else
		{
		iA = aEnd;
		iB = aStart;
		}
	}

/** Constructor with a start and end position and whether the highlighting
in the range should be set or cleared.

The start and end positions can be specified in any order.

@param aStart The start position.
@param aEnd The end position.
@param aChange Specifies whether the highlighting should be set or cleared.
*/
EXPORT_C CTextLayout::TRangeChange::TRangeChange(TInt aStart, TInt aEnd,
	CTextLayout::TRangeChange::TChangeType aChange)
	{
	Set(aStart, aEnd, aChange);
	}

/** Gets the start and end of the range and whether the highlighting should be
set or cleared.

@param aStart On return, the start of the range. This is always the lesser of
the two positions.
@param aEnd On return, the end of the range. This is always the greater of the
two positions.
@return Specifies whether the highlighting should be set or cleared. */
EXPORT_C CTextLayout::TRangeChange::TChangeType
	CTextLayout::TRangeChange::Get(TInt& aStart, TInt& aEnd) const
	{
	if (iA < iB)
		{
		aStart = iA;
		aEnd = iB;
		return ESet;
		}
	aStart = iB;
	aEnd = iA;
	return EClear;
	}

/** Tests whether the range is not null.

@return ETrue if the range is not of zero length, EFalse if the range is of zero
length. */
EXPORT_C TBool CTextLayout::TRangeChange::NonNull() const
	{
	return iA - iB;
	}

/** Clips the range so that its start and end positions are within the minimum
and maximum positions specified.

@param aMin The minimum value for the start and end of the range.
@param aMax The maximum value for the start and end of the range.
@return ETrue if the resulting range is not of zero length, EFalse if it is of
zero length. */
EXPORT_C TBool CTextLayout::TRangeChange::Clip(TInt aMin, TInt aMax)
	{
	if (iA < aMin)
		iA = aMin;
	if (iB < aMin)
		iB = aMin;
	if (aMax < iA)
		iA = aMax;
	if (aMax < iB)
		iB = aMax;
	return iA - iB;
	}

/** Try to cancel out sections of the ranges that overlap Merges two ranges of
 characters.

Two successive calls to CTextLayout::Highlight() could cause unecessary flicker
or redrawing if the arguments to each call overlap. For example, if extending a
highlight involved removing the old highlight and then drawing the new one,
this would cause visible flicker. This can be eliminated by calling this
function to remove any overlap between the two ranges. If there is overlap,
this range is set to the result of the merge, and the other range (aBuddy) is
set to zero.

When calling this function, it does not matter whether or not the two ranges
overlap. Also it does not matter which range is the parameter and which is the
calling object. After calling OptimizeWith(), it is guaranteed that the
resulting ranges will not overlap, and they will represent the same change to
the highlight as the original two ranges.

See the code fragment in the class description for TRangeChange for an example
of how this function is used.

@param aBuddy The range of characters to combine with this range. */
EXPORT_C void CTextLayout::TRangeChange::OptimizeWith(TRangeChange& aBuddy)
	{
	// we make this have the minimum iA and iB, and aBuddy have the maximum
	if (aBuddy.iA < iA)
		{
		TInt temp = aBuddy.iA;
		aBuddy.iA = iA;
		iA = temp;
		}
	if (aBuddy.iB < iB)
		{
		TInt temp = aBuddy.iB;
		aBuddy.iB = iB;
		iB = temp;
		}
	// if they now overlap, we combine them into one and zero the other
	if (aBuddy.iB <= iA)
		iA = aBuddy.iB = aBuddy.iA;
	else if (aBuddy.iA <= iB)
		iB = aBuddy.iA = aBuddy.iB;
	}


TBool CTextLayout::TRangeChange::IsJoinedTo(const TRangeChange aRange)
	{
	TInt a1 = (iA < iB) ? iA : iB;
	TInt b1 = (iA < iB) ? iB : iA;
	TInt a2 = (aRange.iA < aRange.iB) ? aRange.iA : aRange.iB;
	TInt b2 = (aRange.iA < aRange.iB) ? aRange.iB : aRange.iA;

	return a2 <= b1 && a1 <= b2;
	}

void CTextLayout::TRangeChange::Join(const TRangeChange aRange)
	{
	TInt a1 = (iA < iB) ? iA : iB;
	TInt b1 = (iA < iB) ? iB : iA;
	TInt a2 = (aRange.iA < aRange.iB) ? aRange.iA : aRange.iB;
	TInt b2 = (aRange.iA < aRange.iB) ? aRange.iB : aRange.iA;

	// type set to ESet
	iA = (a1 < a2) ? a1 : a2;
	iB = (b1 > b2) ? b1 : b2;
	}


/** Sets or clears a highlight.

If the range of characters to highlight is of zero length, the function has no
effect.

The function affects only those lines that intersect aDrawRect, which is
specified in window coordinates. The drawing parameters, including the graphics
context, are given in aDrawTextLayoutContext.

From v7.0, this function replaces InvertRangeL().

This function is not intended to be used to set any part of a highlight already
set, nor to clear any piece of text not highlighted. It is intended to do
either or both of: clear an existing selection, set a new selection. See the
class description for TRangeChange for a code fragment showing how this
function should be used.

@param aHighlight Specifies the range of characters to highlight or from which
to remove the highlight.
@param aDrawRect Only lines which intersect this rectangle are affected
(specified in window coordinates).
@param aDrawTextLayoutContext Provides a graphics context and other drawing
parameters, e.g. the text and background colours for the highlighted region. */

EXPORT_C void CTextLayout::Highlight(const CTextLayout::TRangeChange& aChangeHighlight,
	const TRect& aDrawRect,	const TDrawTextLayoutContext* aDrawTextLayoutContext)
	{
	if (!aChangeHighlight.NonNull())
		return;

	TRect clip_rect(aDrawRect);

	CGraphicsContext* gc = aDrawTextLayoutContext->PrimaryGc();
	if (aDrawTextLayoutContext->UseClippingRect())
		gc->SetClippingRect(clip_rect);

	TInt visible_start;
	TInt visible_length = PosRangeInBand(visible_start);

	TInt start_pos;
	TInt end_pos;

	CTextLayout::TRangeChange::TChangeType type = aChangeHighlight.Get(start_pos, end_pos);
	start_pos = start_pos < visible_start? visible_start : start_pos;
	end_pos = end_pos < visible_start + visible_length? end_pos : start_pos + visible_length;
	TCursorSelection selection(start_pos, end_pos);

	TPoint top_left = aDrawTextLayoutContext->TopLeftText();
	top_left.iY -= iBandTop;

	TBool isDrawingOnAWindowDC = aDrawTextLayoutContext->UseWindowGc();

	TRect boundingRect;
	iText->GetUpdateBoundingRect(start_pos, end_pos, top_left, boundingRect);
	iHighlightExtensions->AbsExtendRect(boundingRect);
	boundingRect.Intersection(clip_rect);
	if (! boundingRect.IsEmpty())
		{
		if (isDrawingOnAWindowDC)
			{
			BeginRedraw(boundingRect);
			}
		iText->DrawLayout(*gc, top_left, boundingRect, &aDrawTextLayoutContext->iBackgroundColor, TRUE, &selection, NULL);
		if (isDrawingOnAWindowDC)
			{
			EndRedraw();
			}
		}
	}

void CTextLayout::HighlightUsingExtensions(const CTextLayout::TRangeChange& aChangeHighlight,const TRangeChange& aFullHighlight,
	const TRect& aDrawRect,	const TDrawTextLayoutContext* aDrawTextLayoutContext)
	{
	if (!aChangeHighlight.NonNull())
		return;

	TRect clip_rect(aDrawRect);
	clip_rect.Intersection(aDrawTextLayoutContext->TextArea());

	CGraphicsContext* gc = aDrawTextLayoutContext->PrimaryGc();
	if (aDrawTextLayoutContext->UseClippingRect())
		{
		gc->SetClippingRect(clip_rect);
		}

	TInt visible_start;
	TInt visible_length = PosRangeInBand(visible_start);
	TInt start_pos;
	TInt end_pos;
	CTextLayout::TRangeChange::TChangeType type = aChangeHighlight.Get(start_pos, end_pos);
	start_pos = start_pos < visible_start? visible_start : start_pos;
	end_pos = end_pos < visible_start + visible_length? end_pos : start_pos + visible_length;

	TInt full_start_pos;
	TInt full_end_pos;
	(void)aFullHighlight.Get(full_start_pos, full_end_pos);
	full_start_pos = full_start_pos < visible_start? visible_start : full_start_pos;
	full_end_pos = full_end_pos < visible_start + visible_length? full_end_pos : full_start_pos + visible_length;
	TCursorSelection selection(full_start_pos, full_end_pos);

	TPoint top_left = aDrawTextLayoutContext->TopLeftText();
	top_left.iY -= iBandTop;

	TBool isDrawingOnAWindowDC = aDrawTextLayoutContext->UseWindowGc();

	TRect boundingRect;
	iText->GetUpdateBoundingRect(start_pos, end_pos, top_left, boundingRect);
	iHighlightExtensions->AbsExtendRect(boundingRect);
	boundingRect.Intersection(clip_rect);
	if (! boundingRect.IsEmpty())
		{
		if (isDrawingOnAWindowDC)
			{
			BeginRedraw(boundingRect);
			}
		iText->DrawLayout(*gc, top_left, boundingRect, &aDrawTextLayoutContext->iBackgroundColor, TRUE, &selection, iHighlightExtensions);
		if (isDrawingOnAWindowDC)
			{
			EndRedraw();
			}
		}

	if (aDrawTextLayoutContext->UseClippingRect())
		{
		gc->CancelClippingRect();
		}
	}

/** Toggles the selection highlight for the range of text in aHighlight.

Highlights only those lines that intersect aDrawRect, which is specified in
window coordinates. The drawing parameters, including the graphics context, are
given in aDrawTextLayoutContext.

In v7.0 and onwards, this function is deprecated -Highlight() should be used
instead.
@deprecated
@param aHighlight The range of characters for which to invert the highlighting.
@param aDrawRect Only lines which intersect this rectangle are affected;
specified in window coordinates.
@param aDrawTextLayoutContext Provides a graphics context and other drawing
parameters. */
EXPORT_C void CTextLayout::InvertRangeL(const TCursorSelection& aHighlight,const TRect& aDrawRect,
	const TDrawTextLayoutContext* aDrawTextLayoutContext)
	{
	TRangeChange range(aHighlight.iAnchorPos, aHighlight.iCursorPos, TRangeChange::ESet);
	HighlightUsingExtensions(range,range,aDrawRect,aDrawTextLayoutContext);
	}

/**
Return the FORM major version number. This function is not generally useful. It
is used in test code and was used while making a transition between this and
the former version of FORM. The return value is always 2.
*/
EXPORT_C TInt CTextLayout::MajorVersion() const
	{
	return 2;
	}

/**
This method allows Form clients to register an object able to
create or return references to customisation objects used within Form for
various tasks e.g. inline text.
@param aProvider
	Reference to interface provider object to register with Formm.
*/
EXPORT_C void CTextLayout::SetInterfaceProvider( MFormCustomInterfaceProvider* aProvider ) 
    {
    iSource->iInterfaceProvider = aProvider;
    }

/** Gets the width and height of the bounding box of the text, including
indents and margins, when formatted to the specified wrap width.

This is useful for applications like a web browser that need to determine the
minimum width for a piece of text: if you specify zero as the wrap width, the
returned aSize.iWidth contains the minimum width that could be used for the
text without illegal line breaks, and if you specify KMaxTInt for aWrapWidth,
the returned aSize.iHeight contains the minimum height: the height when each
paragraph is a single line of unlimited length.
 @since 6.0
@param aWrapWidth The wrap width for the bounding box.
@param aSize On return, contains the width and height of the bounding box. */
EXPORT_C void CTextLayout::GetMinimumSizeL(TInt aWrapWidth,TSize& aSize)
	{
	//
	//Get the size of the minimal bounding box of the text when formatted to the specified wrap width.
	//The width may be greater than aWrapWidth. To find the minimum width for the text,
	//pass 0 for aWrapWidth. To find the minimum height, pass KMaxTInt for aWrapWidth.
	//
	iText->GetMinimumLayoutSizeL(aWrapWidth,aSize);
	}

/** Gets the width and height of the bounding box of the text, including
indents and margins, when formatted to the specified wrap width.

This is useful for applications like a web browser that need to determine the
minimum width for a piece of text: if you specify zero as the wrap width, the
returned aSize.iWidth contains the minimum width that could be used for the
text, and if you specify KMaxTInt for aWrapWidth, the returned aSize.iHeight
contains the minimum height: the height when each paragraph is a single
line of unlimited length. Use aAllowLegalLineBreaksOnly to set whether or
not illegal line breaks should be considered when determining aSize.
 @since 6.0
@param aWrapWidth The wrap width for the bounding box.
@param aAllowLegalLineBreaksOnly ETrue to only allow legal line breaks, or EFalse to also allow illegal line breaks.
@param aSize On return, contains the width and height of the bounding box. */
EXPORT_C void CTextLayout::GetMinimumSizeL(TInt aWrapWidth,TBool aAllowLegalLineBreaksOnly,TSize& aSize)
	{
	//
	//Get the size of the minimal bounding box of the text when formatted to the specified wrap width.
	//The width may be greater than aWrapWidth. To find the minimum width for the text,
	//pass 0 for aWrapWidth. To find the minimum height, pass KMaxTInt for aWrapWidth.
	//
	iText->GetMinimumLayoutSizeL(aWrapWidth,aAllowLegalLineBreaksOnly,aSize);
	}

/** Sets the custom drawing object, for customising the way text and its
background are drawn.
@since 6.0
@param aCustomDraw Pointer to a custom drawing object. */
EXPORT_C void CTextLayout::SetCustomDraw(const MFormCustomDraw* aCustomDraw)
	{
	iSource->iCustomDraw = aCustomDraw;
	}

/** Returns a pointer to the current custom drawing implementation. Returns
NULL if custom drawing is not in force.
@since 6.0
@return Pointer to the custom drawing object. */
EXPORT_C const MFormCustomDraw* CTextLayout::CustomDraw() const
	{
	return iSource->iCustomDraw;
	}

/** Sets custom line breaking.

If this function is not called, default line breaking behaviour is used.

Ownership of the custom line breaking object is not transferred to this object.

@param aCustomWrap A pointer to an object that implements the custom line
breaking interface. Specify NULL to disable custom line breaking. */
EXPORT_C void CTextLayout::SetCustomWrap(const MFormCustomWrap* aCustomWrap)
	{
	iSource->iCustomWrap = aCustomWrap;
	}

/** Gets the custom line breaking object, as set using SetCustomWrap().

@return A pointer to the custom line breaking object, or NULL if custom line
breaking is not in effect. */
EXPORT_C const MFormCustomWrap* CTextLayout::CustomWrap() const
	{
	return iSource->iCustomWrap;
	}

/**
@internalAll
@released
*/
EXPORT_C void MFormCustomDraw::MFormCustomDraw_Reserved_2()
	{
	}

/** This function is called whenever part of the background of a CTextLayout or
CTextView object needs to be drawn. The default implementation fills
aParam.iDrawRect with the colour specified in aBackground.

The default background colour is contained in aBackground. This is the
background colour of the paragraph if drawing text, or the background colour
specified in the TDrawTextLayoutContext object passed to CTextLayout::DrawL()
if drawing outside the text area.

The rectangle which is drawn by this function, (this may not be the whole of
aParam.iDrawRect) must be returned in aDrawn; areas not drawn by you are
automatically filled using the colour aBackground.

@param aParam Contains the drawing parameters: aParam.iGc is the graphics
context to use. aParam.iMap is the graphics device map, which allows you to
convert between pixels and twips and create fonts. aParam.iTextLayoutTopLeft is
the origin of the text; bitmaps and other graphics must be drawn relative to
this position. aParam.iDrawRect is the area to be drawn; do not draw outside
this rectangle.
@param aBackground The default background colour. This is the background colour
of the paragraph if drawing text, or the background colour specified in the
TDrawTextLayoutContext object passed to CTextLayout::DrawL() if drawing outside
the text area.
@param aDrawn Must return the rectangle you actually draw. This may not be the
whole of aParam.iDrawRect (for instance, if you are drawing a non-tiled bitmap
that occupies only part of aRect). */
EXPORT_C void MFormCustomDraw::DrawBackground(const TParam& aParam,const TRgb& aBackground,TRect& aDrawn) const
	{
	MTmCustom c;
	c.DrawBackground(aParam.iGc,aParam.iTextLayoutTopLeft,aParam.iDrawRect,aBackground,aDrawn);
	}

/** This function is called after the background has been drawn by
DrawBackground(), and before drawing the text. This function might be used to
draw a ruled line under each line of text.

The default implementation of this function does nothing.

@param aParam Contains the drawing parameters. You should only draw to
aParam.iDrawRect. There is no need to fill aParam.iDrawRect or to indicate the
rectangle drawn.
@param aLineInfo Contains the line metrics: aLineInfo.iOuterLineRect specifies
the bounding rectangle of the line, including margins, indents and automatic
space above and below paragraphs, aLineInfo.iInnerLineRect specifies the bounds
of the text only, aLineInfo.iBaseline specifies the baseline of the text. */
EXPORT_C void MFormCustomDraw::DrawLineGraphics(const TParam& /*aParam*/,const TLineInfo& /*aLineInfo*/) const
	{
	// do nothing
	}

/** This function is called to draw the text and its highlighted background, if
any, after bidirectional reordering and other character mappings have taken
place.

The default implementation of this function draws the text with no special
effects and supports standard, round-cornered and shadowed highlighting only.
The text is drawn with the left end of its baseline located at aTextOrigin
after drawing the background, if any, in aParam.iDrawRect.

The main reason to override this function is to apply custom text highlighting,
(for this, aFormat.iFontPresentation.iHighlightStyle should be in the range
EFontHighlightFirstCustomStyle to EFontHighlightLastCustomStyle).

The horizontal spacing between the characters in the text string is increased
by the number of pixels specified in aExtraPixels. The standard way to do this
is by calling CGraphicsContext::SetCharJustification().

The font and other graphics parameters (e.g. pen colour, font style), are
specified in aParam.iGc but a character format container (aFormat) is supplied
so that a different font can be used. Note that any graphics drawn cannot
exceed the bounds of aParam.iDrawRect, so changes are usually restricted to
drawing shadows, outlines, etc. if custom highlighting is in use.

@param aParam Contains the drawing parameters. Drawing can only occur within
aParam.iDrawRect.
@param aLineInfo Contains the line metrics.
@param aFormat Specifies the character formatting to apply to the text,
including the type of text highlighting.
@param aText The text string to be drawn.
@param aTextOrigin The point at which the left end of the baseline of the text
should be drawn.
@param aExtraPixels The number of additional pixels to insert between the
characters in the text string, in order to increase its length. */
EXPORT_C void MFormCustomDraw::DrawText(const TParam& aParam,const TLineInfo& aLineInfo,const TCharFormat& aFormat,
										const TDesC& aText,const TPoint& aTextOrigin,TInt aExtraPixels) const
	{
	TTmLineInfo info;
	info.iOuterRect = aLineInfo.iOuterRect;
	info.iInnerRect = aLineInfo.iInnerRect;
	info.iBaseline = aLineInfo.iBaseline;

	MTmCustom c;
	c.DrawText(aParam.iGc,aParam.iTextLayoutTopLeft,aParam.iDrawRect,info,aFormat,
			   aText,aTextOrigin,aExtraPixels);
	}

EXPORT_C void MFormCustomDraw::DrawText(const TParam& aParam,const TLineInfo& aLineInfo,const TCharFormat& aFormat,
										const TDesC& aText,const TInt aStart, const TInt aEnd, const TPoint& aTextOrigin,TInt aExtraPixels) const
	{
	TTmLineInfo info;
	info.iOuterRect = aLineInfo.iOuterRect;
	info.iInnerRect = aLineInfo.iInnerRect;
	info.iBaseline = aLineInfo.iBaseline;

	MTmCustomExtension c;
	c.DrawText(aParam.iGc,aParam.iTextLayoutTopLeft,aParam.iDrawRect,info,aFormat,
			   aText,aStart,aEnd,aTextOrigin,aExtraPixels);
	}

/** This function translates logical colours specified in FORM objects into
real colours. The default implementation just returns the default colour that
is passed in and ignores aColorIndex.

Overriding implementations may use aColorIndex in any desired way, either to
replace or modify aDefaultColor. The values used in aColorIndex are taken from
the top byte of a TLogicalRgb object and are thus in the range 0...255. The
TLogicalRgb class defines two reserved values:
TLogicalRgb::ESystemForegroundIndex = 254 and
TLogicalRgb::ESystemBackgroundIndex = 255.
*/
EXPORT_C TRgb MFormCustomDraw::SystemColor(TUint /*aColorIndex*/,TRgb aDefaultColor) const
	{
	return aDefaultColor;
	}

void CTextLayout::DrawBackground(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
								 const TLogicalRgb& aBackground) const
	{
	TPoint top_left(aTopLeft);
	top_left.iY -= iBandTop;
	iText->DrawBackground(aGc,top_left,aClipRect,aBackground);
	}

/** Returns the line break class for a Unicode character.

For convenience, it also gets the range of consecutive characters (if any)
according to the Unicode standard, including aCode, that share the same line
break class.

The Unicode line break classes are enumerated in class MTmCustom.

Each character's line break class is obtained using LineBreakClass(). To find
out whether a line break is allowed between two adjacent characters, call
LineBreakPossible() with the line break classes of the two characters as
arguments. For example, the line break class EClLineBreakClass (closing
punctuation) applies to characters such as ")", "]" and ";". Line breaks are
typically allowed after these characters, but not before, so a call to
LineBreakPossible() with the arguments (EAlLineBreakClass, EClLineBreakClass)
would return EFalse in the default implementation, but a call with
(EClLineBreakClass, EAlLineBreakClass) would return ETrue.

@param aCode The Unicode character code of interest.
@param aRangeStart On return, contains the Unicode character code at the start
of the range including aCode that shares the same line break class as aCode.
@param aRangeEnd On return, contains the Unicode character code at the end of
the range including aCode that shares the same line break class as aCode.
@return The line break class assigned to the character. Line break classes are
enumerated in class MTmCustom. */
 EXPORT_C TUint MFormCustomWrap::LineBreakClass(TUint aCode,TUint& aRangeStart,TUint& aRangeEnd) const
	{
	// Create an instance of MTmCustom and then call the MTmCustom's functions for getting a Line Break class 
	MTmCustom c;
	TUint temp;
	temp=c.LineBreakClass(aCode,aRangeStart,aRangeEnd);
	return temp;
	}

/** Tests whether a line break is possible between two characters.

If aHaveSpaces is true, the characters are not adjacent one or more space
characters (with a line break class of ESpLineBreakClass) occur between them.

The aPrevClass and aNextClass arguments never have the value ESpLineBreakClass.
Instead, this function is called with the classes of the characters on either
side of the space or run of spaces, (and if so, aHaveSpaces is true). This is
so that line breaks can be prohibited between certain characters with
intervening spaces, for instance an alphabetic character (EAlLineBreakClass,
such as 'a') and a closing bracket, with a space inbetween.

Additionally, the arguments to this function never have a value of
ESaLineBreakClass. For such characters, GetLineBreakInContext() is called
instead.

@param aPrevClass The line break class of the previous non-space character.
@param aNextClass The line break class of the next non-space character.
@param aHaveSpaces ETrue if there are one or more space characters (with a line
break class of ESpLineBreakClass) between aPrevClass and aNextClass. EFalse if
not.
@return ETrue if a line break is possible between characters with the two line
break classes, EFalse if not. */
EXPORT_C TBool MFormCustomWrap::LineBreakPossible(TUint aPrevClass,TUint aNextClass,TBool aHaveSpaces) const
	{
	MTmCustom c;
	TBool temp;
	temp=c.LineBreakPossible(aPrevClass,aNextClass,aHaveSpaces);
	return temp;
	}

/** Gets the position of the first or last possible line break position in a
text string.

This function is called instead of LineBreakPossible() for runs of characters
of class ESaLineBreakClass. It is used for South Asian languages like Thai, Lao
and Khmer that have no spaces between words, so that line breaks must be
calculated using dictionary lookup or a linguistic algorithm.

The default implementation of this function just returns false.

@param aText A string containing characters of class ESaLineBreakClass.
@param aMinBreakPos A position within aText at which to begin searching for a
possible line break position.
@param aMaxBreakPos A position within aText at which to stop searching for a
possible line break position.
@param aForwards If ETrue, the function gets the first possible line break
position (searches forwards from aMinBreakPos); if EFalse, gets the last one
(searches backwards from aMaxBreakPos).
@param aBreakPos On return, the position of the first or last possible line
break within aText. This must be greater than zero and less than aText.Length()
- 1, and must also be in the range aMinBreakPos to aMaxBreakPos.
@return ETrue if a possible line break position is found, EFalse if not. */
EXPORT_C TBool MFormCustomWrap::GetLineBreakInContext(const TDesC& aText,TInt aMinBreakPos,TInt aMaxBreakPos,
													  TBool aForwards,TInt& aBreakPos) const
	{
	MTmCustom c;
	TBool temp;
	temp=c.GetLineBreakInContext(aText,aMinBreakPos,aMaxBreakPos,aForwards,aBreakPos);
	return temp;
	}

/** Tests whether a character can overhang the right margin.

This function can be overridden to customise the line breaking behaviour for
closing punctuation in Japanese. Any characters for which this function returns
ETrue are allowed to overhang the right margin. The rest will be moved to the
next line.

The default implementation of this function just returns false.

@param aChar The Unicode character code of interest.
@return ETrue if the character specified can overhang the right margin, EFalse if
not. */
EXPORT_C TBool MFormCustomWrap::IsHangingCharacter(TUint aChar) const
	{
	MTmCustom c;
	TBool temp;
	temp=c.IsHangingCharacter(aChar);
	return temp;
	}

// Reserved functions are private until they are used
/**
 @internalAll
 @released
 */ 
EXPORT_C void MFormCustomWrap::MFormCustomWrap_Reserved_1()
	{
	// reserved functions
	}
// Reserved functions are private until they are used. 
/**
 @internalAll
 @released
 */

EXPORT_C void MFormCustomWrap::MFormCustomWrap_Reserved_2()
	{
	// reserved functions 
	}

#ifdef _DEBUG
TBool CTextLayout::__DbgIsFormattingUpToDate() const
	{
	return iUnformattedStart == KMaxTInt;
	}
#endif

// TLayDocTextSource stuff begins here.
TLayDocTextSource::TLayDocTextSource():
	iLayDoc(NULL),
	iFlags(EWrap),
	iWidth(KMaxTInt),
	iEllipsis(0x2026),
	iLabelsWidth(0),
	iLabelsGutter(0),
	iFormatMode(CLayoutData::EFScreenMode),
	iImageDevice(NULL),
	iLabelsDevice(NULL),
	iFormatDevice(NULL),
	iFontHeightIncreaseFactor(EDefaultFontHeightIncreaseFactor),
	iMinimumLineDescent(EDefaultMinimumLineDescent),
	iNonPrintingCharVisibility(),
	iFormParam(NULL),
	iCustomDraw(NULL),
	iCustomWrap(NULL),
	iInterfaceProvider(NULL),
	iDrawOpaque(EFalse),
	iExcessHeightRequired(0),
	iInvisibleCharacterRemapper(NULL)
	{
	}

MGraphicsDeviceMap& TLayDocTextSource::FormatDevice() const
	{
	if (!iFormatDevice)
		CTextLayout::Panic(CTextLayout::EFormatDeviceNotSet);
	if (iLabelsDevice && (iFlags & EUseLabelsDevice))
		return *iLabelsDevice;
	else
		return *iFormatDevice;
	}

MGraphicsDeviceMap& TLayDocTextSource::InterpretDevice() const
	{
	if (!iImageDevice)
		CTextLayout::Panic(CTextLayout::EImageDeviceNotSet);
	if (iLabelsDevice && (iFlags & EUseLabelsDevice))
		return *iLabelsDevice;
	else
		return *iImageDevice;
	}

TInt TLayDocTextSource::DocumentLength() const
	{
	return iLayDoc->LdDocumentLength();
	}

void TLayDocTextSource::GetText(TInt aPos,TPtrC& aText,TTmCharFormat& aFormat) const
	{
	TCharFormat f;
	iLayDoc->GetChars(aText,f,aPos);
	aFormat = f;
	}

void TLayDocTextSource::GetParagraphFormatL(TInt aPos,RTmParFormat& aFormat) const
	{
	CParaFormat f;
	iLayDoc->GetParagraphFormatL(&f,aPos);

	// Labels should not have a forced line height or borders so remove these (this is necessary for Agenda).
	if (iFlags & EUseLabelsDevice)
		{
		f.iLineSpacingInTwips = 0;
		f.iLineSpacingControl = CParaFormat::ELineSpacingAtLeastInTwips;
		f.RemoveAllBorders();
		}

	aFormat.CopyL(f);
	f.Reset();
	}

TRgb TLayDocTextSource::SystemColor(TUint aColorIndex,TRgb aDefaultColor) const
	{
	if (iCustomDraw)
		return iCustomDraw->SystemColor(aColorIndex,aDefaultColor);
	else if (iFormParam)
		return iFormParam->SystemColor(aColorIndex,aDefaultColor);
	else
		return aDefaultColor;
	}

CPicture* TLayDocTextSource::PictureL(TInt aPos) const
	{
	return iLayDoc->PictureHandleL(aPos,MLayDoc::EForceLoadTrue);
	}

TInt TLayDocTextSource::GetPictureSizeInTwipsL(TInt aPos,TSize& aSize) const
	{
	return iLayDoc->GetPictureSizeInTwips(aSize,aPos);
	}

TInt TLayDocTextSource::ParagraphStart(TInt aPos) const
	{
	iLayDoc->LdToParagraphStart(aPos);
	return aPos;
	}

TBool TLayDocTextSource::LabelModeSelect(TLabelType aType, TInt aPos)
	{
	if (!(iFlags & EUseLabelsDevice) && aType == EParLabel)
		{
		// Labels are not allowed on zero-length documents;
		// this is required for Agenda (see ER5U defect EDNGASR-482LSF).
		if (iLayDoc->LdDocumentLength() == 0)
			return FALSE;

		if (iLayDoc->SelectParagraphLabel(aPos))
			{
			iFlags |= EUseLabelsDevice;
			return TRUE;
			}
		}
	
	return FALSE;
	}

void TLayDocTextSource::LabelMetrics(TLabelType aType, TSize& aLabelSize, TInt& aMarginSize) const
	{
	if (aType == EParLabel)
		{
		aLabelSize.iWidth = iLabelsWidth;
		aLabelSize.iHeight = KMaxTInt;
		aMarginSize = iLabelsWidth + iLabelsGutter;
		}
	else
		{
		aLabelSize.iWidth = 0;
		aLabelSize.iHeight = 0;
		aMarginSize = 0;
		}
	}

void TLayDocTextSource::LabelModeCancel()
	{
	iLayDoc->CancelSelectLabel();
	iFlags &= ~EUseLabelsDevice;
	}

/**
This method is responsible for discovering interface extension objects
requried by Form for the specified interface Uid.
@param aInterfaceId
	Identifier for the optional interface requried.
@return
	Pointer to object supporting the requested interface, 
	or 0 if not supported.	
*/
TAny* TLayDocTextSource::GetExtendedInterface(const TUid& aInterfaceId)
	{
	TAny* interfacePtr = 0;

	// First check to see if their is an external interface provider
	// registered and ask it for the interface.
	if (iInterfaceProvider)
		{
		interfacePtr = iInterfaceProvider->GetExtendedInterface( aInterfaceId );
		}

	// If interface still not supplied check self and parent to provide it
	if (!interfacePtr)
		{
		if (aInterfaceId == KFormLabelApiExtensionUid)
			{
			return static_cast<MFormLabelApi*>(this);
			}
		else if(aInterfaceId == KTmTextDrawExtId)
			{
			return static_cast <MTmTextDrawExt*> (this);
			}
		else if(aInterfaceId == KTmCustomExtensionUid)
			{
			return static_cast <MTmCustomExtension*> (this);
			}
		else
			{
			// In this instance, calling the parent class will always return NULL
			// but the pattern should be followed by all implementors for safety
			return MTmSource::GetExtendedInterface(aInterfaceId);
			}
		}

	// Return the interface object or 0 if not supported.
	return interfacePtr;
	}

void TLayDocTextSource::SetLineHeight(const TLineHeightParam& aParam,TInt& aAscent,TInt& aDescent) const
	{
	// Increase the ascent by the font height increase percentage.
	TLineHeightParam p = aParam;
	p.iFontMaxAscent += ((p.iFontMaxAscent + p.iFontMaxDescent) * iFontHeightIncreaseFactor) / 100;

	// Call the standard SetLineHeight.
	MTmSource::SetLineHeight(p,aAscent,aDescent);

	// iExcessHeightRequired is used when height of the highest glyph can be
	// greater than CFont::AscentInPixels() and in iExactLineSpacing mode
	// This value is set via CTextView::SetExcessHeightRequired()
	if ( aParam.iExactLineHeight )
		{
		aAscent += iExcessHeightRequired;
		aDescent -= iExcessHeightRequired;
	}
	
	// Enforce the minimum descent; the following logic is borrowed from CLineLayout::WrapLineL in old FORM.
	if (aAscent + aDescent <= iMinimumLineDescent && aDescent == 0)
		{
		aDescent = 1;
		aAscent--;
		}
	else if (aDescent < iMinimumLineDescent)
		aDescent = iMinimumLineDescent;

	/*
	Ensure the line is at least 1 pixel high and ascent and descent are non-negative so that assertion
	TCursorPosition::GetLineRectL that cursor pos is contained in line rect doesn't fire.
	*/
	if (aAscent < 0)
		aAscent = 0;
	if (aDescent < 0)
		aDescent = 0;
	if (aAscent + aDescent == 0)
		aDescent = 1;
	}

TBool TLayDocTextSource::CanMap() const
	{
	return iFormatMode == CLayoutData::EFScreenMode || iFormatMode == CLayoutData::EFWysiwygMode;
	}

TBool TLayDocTextSource::PageBreakInRange(TInt aStartPos,TInt aEndPos) const
	{
	if (CanMap() && iNonPrintingCharVisibility.PageBreaksVisible())
		return iLayDoc->EnquirePageBreak(aStartPos,aEndPos - aStartPos);
	else
		return FALSE;
	}

void TLayDocTextSource::DrawBackground(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aRect,
									   const TLogicalRgb& aBackground,TRect& aDrawn) const
	{
	ResetOpaque(aGc);
	if (iCustomDraw)
		{
		MFormCustomDraw::TParam param(aGc,InterpretDevice(),aTopLeft,aRect);
		TLogicalRgb background = aBackground;
		FormUtil::LogicalToActualColor(iFormParam,background);
		iCustomDraw->DrawBackground(param,background,aDrawn);
		}
	else
		MTmSource::DrawBackground(aGc,aTopLeft,aRect,aBackground,aDrawn);
	}

void TLayDocTextSource::DrawLineGraphics(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aRect,
										 const TTmLineInfo& aLineInfo) const
	{
	SetOpaque(aGc);
	if (iCustomDraw)
		{
		MFormCustomDraw::TParam param(aGc,InterpretDevice(),aTopLeft,aRect);
		MFormCustomDraw::TLineInfo lineinfo(aLineInfo.iOuterRect,aLineInfo.iInnerRect,aLineInfo.iBaseline);
		iCustomDraw->DrawLineGraphics(param,lineinfo);
		}
	else
		MTmSource::DrawLineGraphics(aGc,aTopLeft,aRect,aLineInfo);
	ResetOpaque(aGc);
	}

void TLayDocTextSource::DrawText(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aRect,
								 const TTmLineInfo& aLineInfo,const TTmCharFormat& aFormat,
								 const TDesC& aText,const TPoint& aTextOrigin,TInt aExtraPixels) const
	{
	SetOpaque(aGc);
	if (iCustomDraw)
		{
		MFormCustomDraw::TParam param(aGc,InterpretDevice(),aTopLeft,aRect);
		MFormCustomDraw::TLineInfo lineinfo(aLineInfo.iOuterRect,aLineInfo.iInnerRect,aLineInfo.iBaseline);
		TCharFormat f;
		aFormat.GetTCharFormat(f);
		iCustomDraw->DrawText(param,lineinfo,f,aText,aTextOrigin,aExtraPixels);
		}
	else
		MTmSource::DrawText(aGc,aTopLeft,aRect,aLineInfo,aFormat,aText,aTextOrigin,aExtraPixels);
	ResetOpaque(aGc);
	}

void TLayDocTextSource::DrawText(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aRect,
								 const TTmLineInfo& aLineInfo,const TTmCharFormat& aFormat,
								 const TDesC& aText,const TInt aStart, const TInt aEnd, const TPoint& aTextOrigin,TInt aExtraPixels) const
	{
	SetOpaque(aGc);
	if (iCustomDraw)
		{
		MFormCustomDraw::TParam param(aGc,InterpretDevice(),aTopLeft,aRect);
		MFormCustomDraw::TLineInfo lineinfo(aLineInfo.iOuterRect,aLineInfo.iInnerRect,aLineInfo.iBaseline);
		TCharFormat f;
		aFormat.GetTCharFormat(f);
		iCustomDraw->DrawText(param,lineinfo,f,aText,aStart,aEnd,aTextOrigin,aExtraPixels);
		}
	else
		MTmCustomExtension::DrawText(aGc,aTopLeft,aRect,aLineInfo,aFormat,aText,aStart,aEnd,aTextOrigin,aExtraPixels);
	ResetOpaque(aGc);
	}

TBool TLayDocTextSource::LineBreakPossible(TUint aPrevClass,TUint aNextClass,TBool aHaveSpaces) const
	
	{
	TBool temp;
	if (iCustomWrap)
		temp=iCustomWrap->LineBreakPossible(aPrevClass,aNextClass,aHaveSpaces);
	else
		temp=MTmSource::LineBreakPossible(aPrevClass,aNextClass,aHaveSpaces);
	return temp;
	}

TUint TLayDocTextSource::LineBreakClass(TUint aCode,TUint& aRangeStart,TUint& aRangeEnd) const
	{
	TUint temp;
	if (iCustomWrap)
		temp=iCustomWrap->LineBreakClass(aCode,aRangeStart,aRangeEnd);
	else
		temp=MTmSource::LineBreakClass(aCode,aRangeStart,aRangeEnd);
	return temp;
	}


TBool TLayDocTextSource::GetLineBreakInContext(const TDesC& aText,TInt aMinBreakPos,TInt aMaxBreakPos,
												 TBool aForwards,TInt& aBreakPos) const
	{
	TBool temp;
	if (iCustomWrap)
		temp=iCustomWrap->GetLineBreakInContext(aText,aMinBreakPos,aMaxBreakPos,aForwards,aBreakPos);
	else
		temp=MTmSource::GetLineBreakInContext(aText,aMinBreakPos,aMaxBreakPos,aForwards,aBreakPos);
	return temp;
	}

TBool TLayDocTextSource::IsHangingCharacter(TUint aChar) const
	{
	TBool temp;
	if (iCustomWrap)
		temp=iCustomWrap->IsHangingCharacter(aChar);
	else
		temp=MTmSource::IsHangingCharacter(aChar);
	return temp;
	}


TUint TLayDocTextSource::Map(TUint aChar) const
	{
	// Check if custom formatting has been installed
	if (iInvisibleCharacterRemapper)
		{ // Then use the supplied custom invisible character remapping
		return iInvisibleCharacterRemapper->Remap(aChar, iNonPrintingCharVisibility, *this);
		}
	else // Use the default
		return MFormCustomInvisibleCharacterRemapper::DefaultMapping(aChar, iNonPrintingCharVisibility, *this);
	}

void TLayDocTextSource::DrawPicture(CGraphicsContext& aGc,
									const TPoint& aTextLayoutTopLeft, const TRect& aRect,
									MGraphicsDeviceMap& aDevice, const CPicture& aPicture) const
	{
	SetOpaque(aGc);
	MTmSource::DrawPicture(aGc, aTextLayoutTopLeft, aRect, aDevice, aPicture);
	ResetOpaque(aGc);
	}

//MTmTextDrawExt implementations 

/**
Draws a line. Implements MTmTextDrawExt::DrawLine().
If the opaque drawing mode is active, then the line color will stay unchanged (it will not be
alpha-blended in a case of a transparent window).
@param aGc A reference to a graphics context. If the drawing mode is opaque, then this is a 
           CWindowGc reference.
@param aPt1 Line start point
@param aPt2 Line end point
*/
void TLayDocTextSource::DrawLine(CGraphicsContext& aGc, const TPoint& aPt1, const TPoint& aPt2) const
	{
	SetOpaque(aGc);
	aGc.DrawLine(aPt1, aPt2);
	ResetOpaque(aGc);
	}

/**
Draws a text. Implements MTmTextDrawExt::DrawText().
If the opaque drawing mode is active, then the text color will stay unchanged (it will not be
alpha-blended in a case of a transparent window).
@param aGc A reference to a graphics context. If the drawing mode is opaqe, then this is a 
           CWindowGc reference.
@param aPt1 Text start point
*/
void TLayDocTextSource::DrawText(CGraphicsContext& aGc, const TDesC& aText, const TPoint& aPt) const
	{
	SetOpaque(aGc);
	aGc.DrawText(aText, aPt);
	ResetOpaque(aGc);
	}

/**
Draws a rectangle. Implements MTmTextDrawExt::DrawRect().
If the opaque drawing mode is active, then the rectabgle color will stay unchanged (it will not be
alpha-blended in a case of a transparent window).
@param aGc A reference to a graphics context. If the drawing mode is opaqe, then this is a 
           CWindowGc reference.
@param aRc Rectangle coordinates
*/
void TLayDocTextSource::DrawRect(CGraphicsContext& aGc, const TRect& aRc) const
	{
	SetOpaque(aGc);
	aGc.DrawRect(aRc);
	ResetOpaque(aGc);
	}

/**
Sets opaque drawing mode.
@param aGc A reference to a graphics context. If the drawing mode is opaqe, then this is a 
           CWindowGc reference.
*/
void TLayDocTextSource::SetOpaque(CGraphicsContext& aGc) const
	{
	if(iDrawOpaque)
		{
		static_cast <CWindowGc&> (aGc).SetOpaque(ETrue);
		}
	}

/**
Resets opaque drawing mode.
@param aGc A reference to a graphics context. If the drawing mode is opaqe, then this is a 
           CWindowGc reference.
*/
void TLayDocTextSource::ResetOpaque(CGraphicsContext& aGc) const
	{
	if(iDrawOpaque)
		{
		static_cast <CWindowGc&> (aGc).SetOpaque(EFalse);
		}
	}

// Private CTextLayout functions start here.

void CTextLayout::Panic(TPanicNumber aNumber)
	{
	_LIT(KPanicNumber,"CTextLayout using TAGMA");
	User::Panic(KPanicNumber,aNumber);
	}

void CTextLayout::GetParagraphRect(const TTmDocPos& aDocPos,TRect& aRect) const
	{
	aRect.SetRect(0,0,0,0);
	TTmLineInfo info, info2;
	if (!iText->DocPosToLine(aDocPos,info))
		return;
	aRect = info.iOuterRect;
	if (!(info.iFlags & TTmLineInfo::EParStart))
		{
		iText->ParNumberToLine(info.iParNumber,0,info2);
		aRect.iTl = info2.iOuterRect.iTl;
		}
	if (!(info.iFlags & TTmLineInfo::EParEnd))
		{
		iText->ParNumberToLine(info.iParNumber,KMaxTInt,info2);
		aRect.iBr = info2.iOuterRect.iBr;
		}
	aRect.Move(0,-iBandTop);
	}

/**
Scroll aDy pixels (positive = text moves down); return number of pixels
actually scrolled. Create a new formatted band if necessary.
*/
TInt CTextLayout::ScrollL(TInt aDy,TAllowDisallow aScrollBlankSpace,
        TBool aTopNoLimitBorder/*=EFalse*/,
        TBool aBottomNoLimitBorder/*=EFalse*/)
	{
	if ( EFDisallowScrollingBlankSpace == aScrollBlankSpace )
	    {
	    aTopNoLimitBorder = EFalse;
	    aBottomNoLimitBorder = EFalse;
	    }
	int old_bandtop = iBandTop;
	int desired_bandtop = iBandTop - aDy;
	int pixels_scrolled = 0;
	int height_increase = 0;
	int paragraphs_increase = 0;
	TTmFormatParamBase param;
	InitFormatParam(param);
	// Refuse to scroll if the width is illegal. This prevents
	// time being taken in certain situations where it does
	// not matter. Once the formatting is capable of
	// adding smaller chunks of text at a time, this early-out
	// should become unnecessary.
	if ((param.iWrapWidth < 1 || param.iMaxHeight < 1)
		&& param.iFlags & TTmFormatParamBase::EWrap)
		return 0;
	param.iMaxHeight = KMaxTInt;
	int visible_height = VisibleHeightInPixels();
	if (aDy > 0)		// text moves down; iBandTop decreases
		{
		if (aTopNoLimitBorder)
	        iBandTop = desired_bandtop;
		else
		    iBandTop = Max(0,desired_bandtop);//Disallow text scrolled beyond top border
		
		pixels_scrolled = old_bandtop - iBandTop;
		while (pixels_scrolled < aDy)
			{
			if (!iText->AddParL(param,TRUE,height_increase,paragraphs_increase))
				break;
			pixels_scrolled += height_increase;
			}
		if (pixels_scrolled > aDy)
			{
			iBandTop = pixels_scrolled - aDy;
			pixels_scrolled = aDy;
			}
		}
	else if (aDy < 0)	// text moves up; iBandTop increases
		{
        if (aBottomNoLimitBorder)
            iBandTop = desired_bandtop;
        else
            iBandTop = Min(iText->LayoutHeight(),desired_bandtop);//Disallow text scrolled beyond bottom border

		pixels_scrolled = old_bandtop - iBandTop;
		while (pixels_scrolled > aDy)
			{
			if (!AddFormattingAtEndL(param, height_increase,paragraphs_increase))
				break;
			pixels_scrolled -= height_increase;
			if (pixels_scrolled < aDy)
				{
				height_increase -= aDy - pixels_scrolled;
				pixels_scrolled = aDy;
				}
			iBandTop += height_increase;
			}
		
		// Fill in missing part of visible height.
		while (iText->LayoutHeight() - iBandTop < visible_height)
			{
			if (!AddFormattingAtEndL(param, height_increase,paragraphs_increase))
				break;
			}
		}

	// Scroll blank space off the display if desired.
	if (aScrollBlankSpace == EFDisallowScrollingBlankSpace && iText->LayoutHeight() - iBandTop < visible_height)
		{
		int new_bandtop = iText->LayoutHeight() - visible_height;
		if (new_bandtop < 0)
			new_bandtop = 0;
		pixels_scrolled += iBandTop - new_bandtop;
		iBandTop = new_bandtop;
		}

	PruneFormatL(aDy < 0);
	return pixels_scrolled + SetBandTop();
	}

TInt CTextLayout::ScrollDocPosIntoViewL(const TTmDocPos& aDocPos)
	{
	if (aDocPos.iPos > iText->Source()->DocumentLength())
	    {
	    OstTrace0( TRACE_DUMP, CTEXTLAYOUT_SCROLLDOCPOSINTOVIEWL, "EInvalidDocPos" );
	    }
	__ASSERT_DEBUG(aDocPos.iPos <= iText->Source()->DocumentLength(),
		Panic(EInvalidDocPos));
	TTmLineInfo info;
	ExtendFormattingToCoverPosL(aDocPos.iPos);
	if (!iText->DocPosToLine(aDocPos,info))
		{
		if (iText->Source()->DocumentLength() != 0)
		    {
		    OstTrace0( TRACE_DUMP, DUP1_CTEXTLAYOUT_SCROLLDOCPOSINTOVIEWL, "ECharacterNotFormatted" );
		    }
		__ASSERT_DEBUG(iText->Source()->DocumentLength() == 0,
			Panic(ECharacterNotFormatted));
		return ScrollL(iBandTop, EFDisallowScrollingBlankSpace);
		}
	TRect line_rect = info.iOuterRect;
	line_rect.Move(0,-iBandTop);
	int visible_height = VisibleHeightInPixels();
	// if the line is taller than the screen, we must not scroll the
	// baseline off the screen.
	if (visible_height < info.iBaseline - info.iOuterRect.iTl.iY)
		return ScrollL(iBandTop + visible_height - info.iBaseline,
			EFDisallowScrollingBlankSpace);
	if (line_rect.iTl.iY < 0)
		return ScrollL(-line_rect.iTl.iY, EFDisallowScrollingBlankSpace);
	if (line_rect.iBr.iY > visible_height)
		{
		int available = line_rect.iTl.iY;
		int desired = line_rect.iBr.iY - visible_height;
		if (available > 0)
			return ScrollL(-Min(desired,available), EFDisallowScrollingBlankSpace);
		}
	return 0;
	}

EXPORT_C TInt CTextLayout::GetLineNumber(TInt aDocPos)
	{
	TTmLineInfo info;
	TTmDocPos pos(aDocPos,TRUE);
	if (iText->DocPosToLine(pos,info))	
		return info.iLineNumber;
	return 0;
	};


/*
Prune the formatted band, if not formatting all the text, to the required size. If aFromStart is ETrue
prune from the start, otherwise prune from the end.
*/
void CTextLayout::PruneFormatL(TBool aFromStart)
	{
	if (IsFormattingBand())
		{
		int pixels_to_prune = 0;
		if (aFromStart)
			pixels_to_prune = iBandTop;
		else
			pixels_to_prune = iText->LayoutHeight() - iBandTop - BandHeightInPixels();
		if (pixels_to_prune <= 0)
			return;
		TTmFormatParamBase param;
		InitFormatParam(param);
		int height_decrease = 0;
		if(aFromStart)
			{
			while (pixels_to_prune > 0 && iText->DeletePar(param,aFromStart,pixels_to_prune,height_decrease))
				{
				pixels_to_prune -= height_decrease;
				if (aFromStart)
					iBandTop -= height_decrease;
				}
			}
		else
			{
			if (pixels_to_prune > (iBandHeight/2))
				{
				iText->DeleteFormattingFromEndL(param,pixels_to_prune,height_decrease);
				}
			}
		}
	}

/** Sets the hotspot.

@param aHotSpot Which part of the line (top, baseline or bottom) should appear
at a vertical pixel position. */
EXPORT_C void TViewYPosQualifier::SetHotSpot(TPartOfLine aHotSpot)
	{
	iHotSpot = aHotSpot;
	}

/** Sets whether blank space should be allowed at the bottom of the view. This
applies if the document is more than one page long and the last line is
visible.

@param aFillScreen ETrue (the default) tries to fill the screen, by ensuring
that there is as little blank space as possible at the bottom of the view.
EFalse allows blank space at the bottom. */
EXPORT_C void TViewYPosQualifier::SetFillScreen(TBool aFillScreen)
	{
	iFillScreen = aFillScreen;
	}

/** Forces the top line in the view to become fully visible if it is partially
above the top of the view rectangle.

@param aMakeLineFullyVisible EFViewForceLineFullyVisible (the default) forces
the top line to be fully visible EFViewDontForceLineFullyVisible does not. */
EXPORT_C void TViewYPosQualifier::SetMakeLineFullyVisible(TFullyVisible aMakeLineFullyVisible)
	{
	iFullyVisible = aMakeLineFullyVisible;
	}

void CTextLayout::InitFormatParam(TTmFormatParamBase& aParam)
	{
	aParam.iMaxHeight = BandHeightInPixels();
	iSource->iExcessHeightRequired = iExcessHeightRequired;

	aParam.iFlags = TTmFormatParamBase::EAtLeastMaxHeight;
	aParam.iWrapWidth = iSource->iWidth;
	if (iSource->iFlags & TLayDocTextSource::EWrap)
		aParam.iFlags |= TTmFormatParamBase::EWrap;
	if (iSource->iFlags & TLayDocTextSource::ETruncateWithEllipsis)
		{
		aParam.iFlags |= TTmFormatParamBase::ETruncateWithEllipsis;
		aParam.iEllipsis = iSource->iEllipsis;
		}
	}

TInt CTextLayout::VisibleHeightInPixels() const
	{
	if (iSource->iFormatMode == CLayoutData::EFScreenMode || iSource->iFormatMode == CLayoutData::EFWysiwygMode)
		return iVisibleHeight;
	else
		return iSource->InterpretDevice().VerticalTwipsToPixels(iVisibleHeight);
	}

TInt CTextLayout::BandHeightInPixels() const
	{
	if (iBandHeight == CLayoutData::EFHeightForFormattingAllText)
		return CLayoutData::EFHeightForFormattingAllText;
	else
		return VisibleHeightInPixels();
	}

/**
Sets offsets for the edges of the selection highlight.
@param aLeftExtension
	Number of pixels to move the left edge of the highlight to the left.
@param aRightExtension
	Number of pixels to move the right edge of the highlight to the right.
@param aTopExtension
	Number of pixels to move the top edge of the highlight to up.
@param aBottomExtension
	Number of pixels to move the bottom edge of the highlight down.
*/
EXPORT_C void CTextLayout::SetHighlightExtensions(TInt aLeftExtension,
	TInt aRightExtension, TInt aTopExtension, TInt aBottomExtension)
	{
	iHighlightExtensions->SetLeftExtension(aLeftExtension);
	iHighlightExtensions->SetRightExtension(aRightExtension);
	iHighlightExtensions->SetTopExtension(aTopExtension);
	iHighlightExtensions->SetBottomExtension(aBottomExtension);
	}

/**
Set the delta required to position the baseline so there is enough
space for the highset glyph in pixels.  This is the height of the highest glyph - 
CFont::AscentInPixels().  Only used when using TLineSpacingControl::EAttLineSpacingControl.
By default zero.
@param aExcessHeightRequired
	Extra height above CFont::AscentInPixels() required for the highest glyph in pixels.
*/
void CTextLayout::SetExcessHeightRequired(TInt aExcessHeightRequired) 
	{
	iExcessHeightRequired = aExcessHeightRequired;
	}

/**
For any rectangle, aRect, which may be extended, calculate
the "remainder" rectanges when the view rectangle is intersected with the
extended rect and then subtracted.
The remainder rectangles are returned via aRemainderRects, which is required
to be a pointer to an array of 4 TRect's.
aRemainderRects[0] is top remainder, ...[1] is bottom, ...[2] is left, ...[3] is right
*/
void CTextLayout::GetHighlightRemnants(const TRect& aRect, const TDrawTextLayoutContext& aDrawTextLayoutContext,
									   TRect* aRemainderRects) const
	{
	ASSERT(aRemainderRects);
	/*
	overlap is the portion of the view rect that aRect intersects with
	subtract this from the aRect, giving 4 non-overlapping remainder rectangles,
	aRemainderRects, any or all of which may be empty. Copied from SubtractRect in TAGMA
	*/
	TRect overlap(aDrawTextLayoutContext.iViewRect);
	overlap.Intersection(aRect);
	if (overlap.IsEmpty())
		overlap.SetRect(aRect.iTl,aRect.iTl);
	aRemainderRects[0] = aRect;
	aRemainderRects[0].iBr.iY = overlap.iTl.iY;
	aRemainderRects[1] = aRect;
	aRemainderRects[1].iTl.iY = overlap.iBr.iY;
	aRemainderRects[2] = overlap;
	aRemainderRects[2].iTl.iX = aRect.iTl.iX;
	aRemainderRects[2].iBr.iX = overlap.iTl.iX;
	aRemainderRects[3] = overlap;
	aRemainderRects[3].iTl.iX = overlap.iBr.iX;
	aRemainderRects[3].iBr.iX = aRect.iBr.iX;
	}

/**
Cleanup method for the opaque flag.
*/
void CTextLayout::ResetOpaque(void* aThis)
	{
	ASSERT(aThis != NULL);
	CTextLayout* p = reinterpret_cast <CTextLayout*> (aThis);
	p->iSource->iDrawOpaque = EFalse;
	}

/**
Sets opaque drawing flag for CTextLayout object. It will used later when the 
content/background has to be drawn.
Until the flag is not reseted, the opaque drawing will be used 
for all the content except the background - the flag has an useful meaning only for transparent
editors.
A TCleanupItem object will be pushed into the Cleanup Stack, which will reset the opaque
flag durring its destruction.
*/
void CTextLayout::SetOpaqueLC()
	{
	iSource->iDrawOpaque = ETrue;
	CleanupStack::PushL(TCleanupItem(&CTextLayout::ResetOpaque, this));
	}

void FormPanic(TFormPanic aPanic)
	{
	_LIT(KFormPanic,"Form");
	User::Panic(KFormPanic,aPanic);
	}

/**
Default implementation of mapping invisible character to its specified alternate.

Called by TLayDocTextSource::Map() unless overidden by custom mapping class.
May be called by custom mapping class
@param aChar
	Invisible character to be remapped
@param aNonPrintingCharVisibility
	Current state of flags showing visibility of invisible characters
@param aLayDoc
	Const ref to the calling CLayDocTextSource
@return
	The replacement character if remapping has taken place, else return original character
*/
EXPORT_C TUint MFormCustomInvisibleCharacterRemapper::DefaultMapping( TUint aChar, const TNonPrintingCharVisibility aNonPrintingCharVisibility, const TLayDocTextSource& aLayDoc )
	{
	// If mapping special characters is possible, use the specified flags.
	if (!aNonPrintingCharVisibility.NoneVisible() && aLayDoc.CanMap())
		{
		switch (aChar)
			{
			case CEditableText::EParagraphDelimiter:
				if (aNonPrintingCharVisibility.ParagraphDelimitersVisible())
					return KVisibleParagraphBreak;
				break;

			case CEditableText::ELineBreak:
				if (aNonPrintingCharVisibility.LineBreaksVisible())
					return KVisibleLineBreak;
				break;

			case CEditableText::ENonBreakingSpace:
				if (aNonPrintingCharVisibility.NonBreakingSpacesVisible())
					return KVisibleNonBreakSpace;
				break;

			case CEditableText::EPotentialHyphen:
				if (aNonPrintingCharVisibility.PotentialHyphensVisible())
					return KVisiblePotentialHyphen;
				break;

			case CEditableText::ENonBreakingHyphen:
				if (aNonPrintingCharVisibility.NonBreakingHyphensVisible())
					return KVisibleNonBreakHyphen;
				break;

			case CEditableText::ETabCharacter:
				if (aNonPrintingCharVisibility.TabsVisible())
					return KVisibleTab;
				break;

			case CEditableText::EPictureCharacter:
				return KVisiblePicture;

            case 0x200B:    // Zero Width Space: the same behaviour as 0x20, while different category from 0x20.
                if (aNonPrintingCharVisibility.SpacesVisible())
                   return KVisibleSpace;
                break;
                   
			/*
			For the moment, treat bidirectional controls as if they were non-break spaces, as far as visibility is
			concerned, and map as follows: LRE=<, RLE=>, PDF=currency symbol, LRO=left guillemet, RLO=right guillemet.
			*/
			case 0x202A: // LRE
				if (aNonPrintingCharVisibility.NonBreakingSpacesVisible())
					return 0x003C;
				break;
			case 0x202B: // RLE
				if (aNonPrintingCharVisibility.NonBreakingSpacesVisible())
					return 0x003E;
				break;
			case 0x202C: // PDF
				if (aNonPrintingCharVisibility.NonBreakingSpacesVisible())
					return 0x00A4;
				break;
			case 0x202D: // LRO
				if (aNonPrintingCharVisibility.NonBreakingSpacesVisible())
					return 0x00AB;
				break;
			case 0x202E: // RLO
				if (aNonPrintingCharVisibility.NonBreakingSpacesVisible())
					return 0x00BB;
				break;

			default:
				if (aNonPrintingCharVisibility.SpacesVisible() && TChar(aChar).GetCategory() == TChar::EZsCategory)
					return KVisibleSpace;
				break;
			}
		}

	// If not mapping special characters, or not mapping this particular character, use the default mapping.
	return aLayDoc.MTmSource::Map(aChar);
	}


/**
Allows Form clients to register an invisible character remapper object to 
customize the visible display of invisible characters such as paragraph marks.
@param aInvisibleCharacterRemapper
	Pointer to custom invisible character remapper to use
*/
EXPORT_C void CTextLayout::SetCustomInvisibleCharacterRemapper( MFormCustomInvisibleCharacterRemapper* aInvisibleCharacterRemapper ) 
	{
	iSource->iInvisibleCharacterRemapper = aInvisibleCharacterRemapper;
	}

/**
Allows Form clients to see which character remapper object is currently 
registered.
*/
EXPORT_C MFormCustomInvisibleCharacterRemapper* CTextLayout::GetCustomInvisibleCharacterRemapper()
	{
	return iSource->iInvisibleCharacterRemapper;
	}

/** INC092568: CTextView::SetPendingSelection is not honoured
Sets the cursor position member added for this fix that allows the text layout object
to access any pending selection made by the owning text view object
@param aPos:- pointer to the owning textview's iCursorPos
*/
void CTextLayout::SetTextViewCursorPos(TCursorPosition* aPos)
	{
	iTextViewCursorPos = aPos;
	}

/**
This is the function used by CTextLayout when it wants to extend the formatted range downwards. 
It just calls CTmTextLayout::AddParL, but restricts the maximum height of the new formatting to the 
band height/2. If the next paragraph is bigger than this, AddParL will only format part of that 
paragraph, and the formatting will end in the middle of the paragraph.
*/
TBool CTextLayout::AddFormattingAtEndL(TTmFormatParamBase& aFormatParam, TInt& aHeightIncrease, TInt& aParagraphsIncrease)
	{
	aFormatParam.iMaxHeight = iBandHeight/2;
	return iText->AddParL(aFormatParam, EFalse, aHeightIncrease, aParagraphsIncrease);
	}
	
/** 
Stops or allows text to be drawn.  Included to allow users to control visibility
if text is part of an invisible control.

@param aVisible		ETrue to make the text visible, EFalse to make it invisible.
@see CCoeControl::MakeVisible()
*/
EXPORT_C void CTextLayout::MakeVisible(TBool aVisible)
	{
	iText->MakeVisible(aVisible);
	}

