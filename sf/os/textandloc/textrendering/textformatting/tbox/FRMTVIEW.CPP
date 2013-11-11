/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "FRMTLAY.H"
#include "FRMTVIEW.H"
#include "FRMCONST.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FRMTVIEWTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "FRMCONST_INTERNAL.H"
#include "FRMCONST_PARTNER.H"
#include "TAGMA_INTERNAL.H"
#endif

/** Reports whether a character is a virama (indic vowel-killer,
used for joining syllables together) or not.
@param aCharacter The character.
@return ETrue if aCharacter is a virama.
@internalComponent */
TBool IsVirama(TChar aCharacter)
	{
	// Unicode combining class 9 indicates a virama
	return aCharacter.GetCombiningClass() == 9;
	}

/**
Reports whether a character is a graphical spacing character or not.
@param aCharacter Unicode UTF32 character to test.
@return ETrue if it is both graphical and not a combining mark.
@internalComponent
*/
TBool IsGraphicalSpacing(TChar aCharacter)
	{
	TChar::TCategory cat = aCharacter.GetCategory();
	switch (cat & 0xF0)
		{
	case TChar::EAlphaGroup:
	case TChar::ELetterOtherGroup:
	case TChar::ELetterModifierGroup:
	case TChar::ENumberGroup:
	case TChar::EPunctuationGroup:
	case TChar::ESymbolGroup:
		return ETrue;
	default:
		return EFalse;
		}
	}

TCursor::TCursor(TCursorPosition& aCursorPos,RScreenDisplay& aDisplay):
	iDisplay(aDisplay),
	iCursorPos(aCursorPos),
	iVisible(FALSE),
	iFlash(TRUE),
	iLineCursor(EFCursorInvisible),
	iTextCursor(EFCursorInvisible),
	iLineCursorBitmap(NULL),
	iAscent(-1),
	iDescent(-1),
	iWeight(ETextCursorWeight),
	iType(TTextCursor::ETypeRectangle),
	iXorColor(ETextCursorInvertColor),
	iPlacement(ECursorVertical)
	{
	}

void TCursor::SetLineCursorBitmap(const CFbsBitmap* aLineCursorBitmap)
	{
	iLineCursorBitmap=aLineCursorBitmap;
	}

/**
Sets the selection highlight to have its edges moved by the specified number of pixels.
Problems with highlights appearing in the wrong place may be due to incorrect 
font metrics, and would not be fixed in general with a call to this function.
@param aLeftExtension
	Number of pixels to the left to move the left of the highlight.
@param aRightExtension
	Number of pixels to the right to move the right of the highlight.
@param aTopExtension
	Number of pixels to the up to move the top of the highlight.
@param aBottomExtension
	Number of pixels to the down to move the bottom of the highlight.
*/
EXPORT_C void CTextView::SetHighlightExtensions(TInt aLeftExtension,
	TInt aRightExtension, TInt aTopExtension, TInt aBottomExtension)
	{
	iLayout->SetHighlightExtensions(aLeftExtension, aRightExtension,
		aTopExtension, aBottomExtension);
	}

/**
Set the delta required to position the baseline so there is enough
space for the highset glyph in pixels.  This is the height of the highest glyph - 
CFont::AscentInPixels().  Only used when using TLineSpacingControl::EAttLineSpacingControl.
By default zero.
@param aExcessHeightRequired
	Extra height above CFont::AscentInPixels() required for the highest glyph in pixels.
*/
EXPORT_C void CTextView::SetExcessHeightRequired(TInt aExcessHeightRequired)
	{
	iLayout->SetExcessHeightRequired( aExcessHeightRequired );
	}

void TCursor::SetVisibility(TVisibility aLineCursor,TVisibility aTextCursor)
	{
	TUint cursorsToDraw=EFNeitherCursor;

	if (aLineCursor==EFCursorFlashing)
		aLineCursor=EFCursorVisible;
	if (iLineCursor!=aLineCursor)
		{
		iLineCursor=aLineCursor;
		cursorsToDraw|=EFLineCursor;
		}
	if (iTextCursor!=aTextCursor)
		{
		iTextCursor=aTextCursor;
		cursorsToDraw|=EFTextCursor;
		}
	Draw(cursorsToDraw);
	}

// Change the cursor type and redraw it.
void TCursor::SetType(TTextCursor::EType aType)
	{
	if (aType != TTextCursor::ETypeNone)
		{
		iType = aType;
		Draw(EFTextCursor);
		}
	}

// Change the cursor placement and redraw it.
void TCursor::SetPlacement(TTmCursorPlacement aPlacement)
	{
	iPlacement = aPlacement;
	Draw(EFTextCursor);
	}
   
/** Sets the ascent and descent of the text cursor in pixels and redraw it. To
use the ascent or descent of the adjacent character pass a value of -1. */
void TCursor::SetAscentAndDescent(TInt aAscent,TInt aDescent)
	{
	iAscent = aAscent;
	iDescent = aDescent;
	Draw(EFTextCursor);
	}

/** Changes the cursor weight and redraws it.*/
void TCursor::SetWeight(TInt aWeight)
	{
	if (aWeight > 0)
		{
		iWeight = aWeight;
		Draw(EFTextCursor);
		}
	}

/** Sets the flashing state of the cursor and redraws it. */
void TCursor::SetFlash(TBool aEnabled)
	{
	iFlash = aEnabled;
	Draw(EFTextCursor);
	}

/** Sets the XOR colour of the cursor and redraws it. */
void TCursor::SetXorColor(TRgb aColor)
	{
	iXorColor = aColor;
	Draw(EFTextCursor);
	}

/** Sets the cursor so that when it is next drawn its metrics will match those
of adjacent text: cancels any overriding metrics. Does not redraw. */
void TCursor::MatchCursorHeightToAdjacentChar()
	{
	iAscent = iDescent = -1;
	}

/** Draws the specified cursors. */
void TCursor::Draw(TUint aCursors)
	{
	TPoint caret_origin;
	TInt caret_width, caret_ascent, caret_descent;
	TBool cursorsVisible = iCursorPos.GetCursor((TTmCursorPlacement)iPlacement,
											    caret_origin,caret_width,caret_ascent,caret_descent);
	caret_origin += iDisplay.TopLeftTextArea();

	if (aCursors & EFTextCursor)
		{
		if (cursorsVisible && (iTextCursor != EFCursorInvisible) && !iCursorPos.IsNewPictureFrame())
			{
			if (iTextCursor == EFCursorVisible)
				RemoveTextCursor();
			else
				{
				int ascent = 0;
				int descent = 0;
				int width = 0;

				if (iPlacement == ECursorVertical)
					{
					if (iAscent >= 0)
						ascent = iAscent;
					else
						ascent = caret_ascent;
					ascent += iFirstExtension;
					if (iDescent >= 0)
						descent = iDescent;
					else
						descent = caret_descent;
					descent += iSecondExtension;
					width = iWeight;
					// Adjust which side the cursor hangs around the origin
					// based on the sign of the 'width' from tagma.
					if (caret_width<0)
						caret_origin.iX -= iWeight;

					}
				else
					{
					ascent = 0;
					descent = iWeight;
					// Adjust which side the cursor hangs around the origin
					// based on the sign of the 'width' from tagma and set the
					// drawn width to the abs(width) value tagma has given us.
					if (caret_width < 0)
						{
						width = -caret_width;
						caret_origin.iX -= width; 
						}
					else
						width = caret_width;
					caret_origin.iX -= iFirstExtension;
					width += (iFirstExtension + iSecondExtension);
					}
				DrawTextCursor(caret_origin,width,ascent,descent);
				}
			}
		else
			RemoveTextCursor();
		}
	if (iDisplay.IsLineCursor() && (aCursors&EFLineCursor))
		{
		if (cursorsVisible && (iLineCursor != EFCursorInvisible))
			DrawLineCursor(caret_origin.iY);
		else
			RemoveLineCursor();
		}
	}

void TCursor::DrawLineCursor(TInt aHeight)
	{
	if (!iLineCursorBitmap)
	    {
	    OstTrace0( TRACE_FATAL, DUP1_TCURSOR_DRAWLINECURSOR, "EFLineCursorBitmapNotSet" );
	    }
	__ASSERT_ALWAYS(iLineCursorBitmap,FormPanic(EFLineCursorBitmapNotSet));
	TRect cursorMargin=iDisplay.LineCursorMargin();
	TRect bitmapRect;
	
	bitmapRect.iTl.iX=cursorMargin.iTl.iX+ELineCursorToLabelGap;
	bitmapRect.iBr.iX=bitmapRect.iTl.iX+iLineCursorBitmap->SizeInPixels().iWidth;
	bitmapRect.iBr.iY=aHeight;
	bitmapRect.iTl.iY=aHeight-iLineCursorBitmap->SizeInPixels().iHeight;

	iDisplay.SetRects(RScreenDisplay::EFClipLineCursor);
	iDisplay.ActivateContext();
	iDisplay.ResetClippingRect();

	if (iDisplay.UseWindowGc() && NULL != iDisplay.Layout())
		{
		iDisplay.Layout()->BeginRedraw(cursorMargin);
		}

	TRegionFix<4> clearRegion(cursorMargin);
	clearRegion.SubRect(bitmapRect);

	for (TUint ii=0; ii<(TUint)clearRegion.Count(); ii++)
		{
		iDisplay.ClearRect(clearRegion.RectangleList()[ii]);
		}

	iDisplay.BlastBitmap(bitmapRect.iTl,iLineCursorBitmap,TRect(TPoint(0,0),bitmapRect.Size()));
	if (iDisplay.UseWindowGc() && NULL != iDisplay.Layout())
		{
		iDisplay.Layout()->EndRedraw();
		}
	
	iDisplay.DeactivateContext();
	}

void TCursor::RemoveLineCursor()
	{
	TRect cursorMargin=iDisplay.LineCursorMargin();
	iDisplay.SetRects(RScreenDisplay::EFClipLineCursor);
	iDisplay.ActivateContext();
	iDisplay.ResetClippingRect();

	if (iDisplay.UseWindowGc() && NULL != iDisplay.Layout())
		{
		iDisplay.Layout()->BeginRedraw(cursorMargin);
		}
	iDisplay.ClearRect(cursorMargin);
	if (iDisplay.UseWindowGc() && NULL != iDisplay.Layout())
		{
		iDisplay.Layout()->EndRedraw();
		}

	iDisplay.DeactivateContext();
	}

/** Draws the text cursor. */
void TCursor::DrawTextCursor(TPoint aOrigin,TInt aWidth,TInt aAscent,TInt aDescent)
	{
	TTextCursor cursor;
 	cursor.iType = iType;
	cursor.iHeight = aAscent + aDescent;
	cursor.iAscent = aAscent;
	cursor.iWidth = aWidth;
	cursor.iFlags = iFlash ? 0 : TTextCursor::EFlagNoFlash;
	if (iPlacement == ECursorVertical)
		cursor.iFlags |= TTextCursor::EFlagClipVertical;
	if (iPlacement == ECursorUnderlineNext || iPlacement == ECursorUnderlinePrev)
		cursor.iFlags |= TTextCursor::EFlagClipHorizontal;
	cursor.iColor = iXorColor;
	iDisplay.SetTextCursor(aOrigin,cursor);
	iVisible = TRUE;
	}

/** Removes the text cursor only if it is currently visible in this window. */
void TCursor::RemoveTextCursor()
	{
	if (iVisible)
		{
		iDisplay.RemoveTextCursor();
		iVisible = FALSE;
		}
	}

void TCursor::SetExtensions(TInt aFirstExtension, TInt aSecondExtension)
	{
	TBool redraw=EFalse;
	if (aFirstExtension!=iFirstExtension)
		{
		iFirstExtension=aFirstExtension;
		redraw=ETrue;
		}
	if (aSecondExtension!=iSecondExtension)
		{
		iSecondExtension=aSecondExtension;
		redraw=ETrue;
		}
	if( redraw)
		Draw(EFTextCursor);
	}

/** Allocates and constructs a CTextView object.

@param aLayout Pointer to the layout object referenced by the text view. Must 
not be NULL or a panic occurs. 
@param aDisplay The rectangle in which text is displayed (the "view rectangle"). 

@param aGd Bitmapped graphics device to draw to, e.g. the screen. 
@param aDeviceMap The device map used for drawing and formatting. 
@param aWin The window to draw to. Should not be NULL. 
@param aGroupWin Window group. Must be provided if a text cursor is to be displayed. 
It can be NULL if you don't need a text cursor. 
@param aSession Pointer to the window server session. 
@return Pointer to the new CTextView object. */
EXPORT_C CTextView *CTextView::NewL(CTextLayout *aLayout,const TRect &aDisplay,CBitmapDevice *aGd,
									MGraphicsDeviceMap *aDeviceMap,RWindow *aWin,RWindowGroup *aGroupWin,
									RWsSession *aSession)
	{
	CTextView* self = new(ELeave) CTextView();
  	CleanupStack::PushL(self);
	self->ConstructL(aLayout,aDisplay,aGd,aDeviceMap,aWin,aGroupWin,aSession);
	CleanupStack::Pop();
	return self;
	}

/** Called when system is idle. Use to do background formatting. Enters OOM
state before leaving */
EXPORT_C TInt CTextView::IdleL(TAny *aSelf)
	{

	if (((CTextView *) aSelf)->NoMemoryCheckL())
		return EFalse;
	return ((CTextView *) aSelf)->NextLineL();
	}

EXPORT_C CTextView::CTextView():
	iDisplay(&iDrawTextLayoutContext),
	iCursor(iCursorPos,iDisplay),
	iCursorPos(),
	iFlags(EFEverythingVisible | EFFlickerFreeRedraw | EFTextVisible),
	iHorizontalScroll(EFNoPreviousHorizontalScroll),
	iContextIsNavigation(ETrue)
	{
	if (EFMemoryOK == EFRecovering)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_CTEXTVIEW, "EFSystemConstantsChanged" );
	    }
	__ASSERT_DEBUG(EFMemoryOK != EFRecovering,FormPanic(EFSystemConstantsChanged));
	}

EXPORT_C void CTextView::ConstructL(CTextLayout *aLayout,const TRect &aDisplay,CBitmapDevice *aGd,
									MGraphicsDeviceMap *aDeviceMap,RWindow *aWin,RWindowGroup *aGroupWin,
									RWsSession *aSession)
	{
	if (!aLayout)
	    {
	    OstTrace0( TRACE_FATAL, CTEXTVIEW_CONSTRUCTL, "EFInvalidLayout" );
	    }
	__ASSERT_ALWAYS(aLayout,FormPanic(EFInvalidLayout));
	iHorizontalScrollJump=EFDefaultHorizontalScrollJump;
	iWrap = CIdle::NewL(EFBackgroundFormattingPriority);
	SetLayout(aLayout);
	SetViewRect(aDisplay);
	SetDisplayContextL(aGd,aWin,aGroupWin,aSession);
	// Tell the Layout classes about the Graphics Device
	aLayout->SetImageDeviceMap(aDeviceMap);
	iDrawTextLayoutContext.SetDrawToEveryPixel(ETrue);
	}

EXPORT_C CTextView::~CTextView()
	{
	delete iPictureFrame;
	delete iWrap;
	DestroyWindowServerObjects();
	}

/** Changes the text layout object used by the text view.

@param aLayout Pointer to the text layout object used by the text view. Must 
not be NULL or a panic occurs. */
EXPORT_C void CTextView::SetLayout(CTextLayout *aLayout)
	{
	if (!aLayout)
	    {
	    OstTrace0( TRACE_FATAL, CTEXTVIEW_SETLAYOUT, "EFInvalidLayout" );
	    }
	__ASSERT_ALWAYS(aLayout,FormPanic(EFInvalidLayout));
	iLayout = aLayout;
	iCursorPos.SetLayout(aLayout);
	iDisplay.SetLayout(aLayout);
	}

/** Changes the window server handles which the display uses. Any of the parameters 
can be NULL in which case they are not changed.

@param aGd Pointer to the bitmapped graphics device to draw to. 
@param aWin Pointer to the view window. 
@param aGroupWin Pointer to the window group. 
@param aSession Pointer to the window server session. */
EXPORT_C void CTextView::SetDisplayContextL(CBitmapDevice * aGd,RWindow *aWin,RWindowGroup *aGroupWin,RWsSession *aSession)
	{
	if (aSession)
		iDisplay.SetWindowsServer(aSession);
	if (aGroupWin)
		iDisplay.SetWindowGroup(aGroupWin);
	if (aWin)
		{
		iDisplay.SetWindow(aWin);
		iLayout->SetWindow(aWin);
		}
	if (aGd)
		iDisplay.SetGraphicsDeviceL(aGd);
	}

/** Sets the rectangle in which the text is displayed.


Note:

A valid band height (>0) must be supplied(i.e. aViewRect.Tl.iY coordinate must be lower than the aViewRect.Br.iY coordinate) or the expected amount of formatting will not take place. This could lead to panics when trying to retrieve formatting information that does not exist.

@param aDisplay The view rectangle. */
EXPORT_C void CTextView::SetViewRect(const TRect &aViewRect)
	{
	iDrawTextLayoutContext.iViewRect = aViewRect;
	iLayout->SetBandHeight(aViewRect.Height());

	TRect oldViewRect = iDrawTextLayoutContext.iViewRect;

	iDrawTextLayoutContext.iViewRect = aViewRect;
	iLayout->SetBandHeight(aViewRect.Height());

	if (NULL != iDisplay.Window() && ! oldViewRect.IsEmpty())
		{
		iDisplay.Window()->Invalidate(oldViewRect);
		}
	}


// deprecated, planning to remove
EXPORT_C void CTextView::AlterViewRect(const TRect &aViewRect)
	{
	iReducedDrawingAreaRect = aViewRect;
	}

/** Sets the label and line cursor margin widths. 

Does not redraw.

@param aLabels The width in pixels of the label margin. 
@param aLineCursor The width in pixels of the line cursor margin. */
EXPORT_C void CTextView::SetMarginWidths(TInt aLabels,TInt aLineCursor)
	{
	iDrawTextLayoutContext.iLabelMarginWidth = aLabels;
	iDrawTextLayoutContext.iGutterMarginWidth = aLineCursor;
	iLayout->SetLabelsMarginWidth(aLabels);
	}

/** Gets the label and line cursor margin widths in pixels.

@param aLabels On return contains the width in pixels of the label margin. 
@param aLineCursor On return contains the width in pixels of the line cursor 
margin. */
EXPORT_C void CTextView::MarginWidths(TInt& aLabels,TInt& aLineCursor) const
	{
	aLineCursor = iDrawTextLayoutContext.iGutterMarginWidth;
	aLabels = iDrawTextLayoutContext.iLabelMarginWidth;
	}

/** Removes a selection, and redraws the affected part of the screen.

Any background formatting is forced to complete first. */
EXPORT_C void CTextView::CancelSelectionL()
	{
	TCursorSelection selection;

	NoMemoryCheckL();
	iCursorPos.GetSelection(selection);
	TBool isPictureFrame=iCursorPos.IsPictureFrame();
	if (selection.Length()>0)
		{
		iCursorPos.CancelHighlight();
		if (!SelectionVisible())
			return;
		iDisplay.SetRects(RScreenDisplay::EFClipExtendedTextArea);		
		iDisplay.ActivateContext();
		iDisplay.ResetClippingRect();
		if (isPictureFrame)
			{
			RedrawPictureFrameRectL(selection.LowerPos()); // iCursorPos.CancelHighlight() was called, so the picture will be redrawn without frame
			DrawCursor(TCursor::EFTextCursor);
			}
		else
			{
			CTextLayout::TRangeChange clear_range(selection.iAnchorPos, selection.iCursorPos,CTextLayout::TRangeChange::EClear);
	       	CTextLayout::TRangeChange original(selection.iCursorPos,selection.iCursorPos,CTextLayout::TRangeChange::ESet);
		   	HighlightUsingExtensions(clear_range, original);
			}
		}
		iDisplay.DeactivateContext();
	}

/** This function should not be used. This function is used in TTextView to
test a fix for a defect.
@internalTechnology */
EXPORT_C CBitmapContext* CTextView::BitmapContext()
	{
	return iDisplay.BitmapContext();
	}

/** Calculates which of two document positions would be considered more left or
more right, depending on the value of aDirection.

@param aStart First document position to compare.
@param aEnd Second document position to compare.
@param aDirection If EVisualLeft, the function returns the left-most of aStart
and aEnd. If EVisualRight, the function returns the right-most.
@return The left-most of aStart and aEnd if aDirection was EVisualLeft;
otherwise, the right-most. */
EXPORT_C const TTmDocPos& CTextView::VisualEndOfRunL(
	const TTmDocPos& aStart, const TTmDocPos& aEnd,
	TCursorPosition::TVisualEnd aDirection)
	{
	return iCursorPos.VisualEndOfRunL(aStart, aEnd, aDirection);
	}

/** Gets the cursor position as a TTmDocPos.

@param aPos On return, the cursor position. */
EXPORT_C void CTextView::GetCursorPos(TTmDocPos& aPos) const
	{
	aPos = iCursorPos.TmDocPos();
	}

/** Un-draws a selection, redraws the affected part of the screen. The cursor
selection remains as before. Analogous to CancelSelectionL(), but leaves the
cursor selection untouched. This function should be called before calling
CTextView::SetPendingSelection() _and_ SetHighlightExtensions has been used to
extend the size of the highlight. Any background formatting is forced to
complete first. */
EXPORT_C void CTextView::ClearSelectionL()
	{
	TCursorSelection selection;

	NoMemoryCheckL();
	iCursorPos.GetSelection(selection);
	TBool isPictureFrame=iCursorPos.IsPictureFrame();
	if (selection.Length()>0)
		{
		iCursorPos.CancelHighlight();
		DoClearSelectionL(selection,isPictureFrame);
		iDisplay.DeactivateContext();
		}
	}

void CTextView::DoClearSelectionL(const TCursorSelection& aSelection, TBool aIsPictureFrame)
	{
	if (!SelectionVisible())
		return;
	iDisplay.SetRects(RScreenDisplay::EFClipExtendedTextArea);
	iDisplay.ActivateContext();
	iDisplay.ResetClippingRect();
	if (aIsPictureFrame)
		{
		RedrawPictureFrameRectL(aSelection.LowerPos()); // iCursorPos.CancelHighlight() was called, so the picture will be redrawn without frame
		DrawCursor(TCursor::EFTextCursor);
		}
	else
		{
		iDisplay.SetRects(RScreenDisplay::EFClipExtendedTextArea);
		iDisplay.ResetClippingRect();
		CTextLayout::TRangeChange clear_range(aSelection.iAnchorPos, aSelection.iCursorPos,CTextLayout::TRangeChange::EClear);
		CTextLayout::TRangeChange new_range(aSelection.iCursorPos,aSelection.iCursorPos,CTextLayout::TRangeChange::ESet);
		HighlightUsingExtensions(clear_range,new_range);
		}
	}

/** Used to correct the display after a cursor move type call Enters OOM before
leaving */
TInt CTextView::DrawAfterCursorMoveL(TInt aVerticalScrollBy)
	{
	TInt horizontalScrollBy = CheckHorizontalScroll(iCursorPos.TmDocPos());
	
	DrawWithPreviousHighlight();

	if (0 != aVerticalScrollBy || 0 != horizontalScrollBy)
		{
		ScrollDisplayL();
		}
	else
		{
		DrawCursor();
		}

	DrawWithCurrentHighlight();

	return horizontalScrollBy;
	}

/** Sets a selection. The selection is highlighted unless selection visibility
has been unset. Any existing selection is cancelled. If necessary, the view is
scrolled and redrawn.

Any background formatting is forced to complete first.

@param aSelection Specifies the start and end positions of the selection.
@return The number of pixels scrolled in the x (horizontal) and y (vertical)
directions. */
EXPORT_C TPoint CTextView::SetSelectionL(const TCursorSelection& aSelection)
	{
	iContextIsNavigation = ETrue;
	TCursorSelection oldSelection;
	TBool isPictureFrame=iCursorPos.IsPictureFrame();
	TInt scrollBy=0;		//To avoid warning

	NoMemoryCheckL();
	FinishBackgroundFormattingL();
	iCursorPos.GetSelection(oldSelection);
	iCursor.MatchCursorHeightToAdjacentChar();
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
	TRAPD(err,scrollBy=iCursorPos.SetSelectionL(aSelection));
	if (err)
		NoMemoryL(err);
	TInt horizScrollBy=DrawAfterCursorMoveL(scrollBy);
	iDisplay.SetRects(RScreenDisplay::EFClipTextArea);
	iDisplay.ActivateContext();
	iDisplay.ResetClippingRect();
	if (SelectionVisible())
		{
		CTextLayout::TRangeChange old;
		CTextLayout::TRangeChange select;
		if (isPictureFrame)
			{
			iCursorPos.DontDrawOldPictureFrame();
			RedrawPictureFrameRectL(oldSelection.LowerPos()); // this will erase the old picture frame
			DrawCursor(TCursor::EFTextCursor);
			}
		else
			{
			old.Set(oldSelection.iAnchorPos, oldSelection.iCursorPos,
				CTextLayout::TRangeChange::EClear);
			}

		if (iCursorPos.IsNewPictureFrame())
			{
			RedrawPictureFrameRectL(aSelection.LowerPos());	// this will draw the new picture frame
			}
		else
			{
			select.Set(aSelection.iAnchorPos, aSelection.iCursorPos,
				CTextLayout::TRangeChange::ESet);
			}

		CTextLayout::TRangeChange original;
		original = select;
		select.OptimizeWith(old);
		iDisplay.SetRects(RScreenDisplay::EFClipExtendedTextArea);
		iDisplay.ResetClippingRect();

		if (select.IsJoinedTo(old))
			{
			select.Join(old);
			HighlightUsingExtensions(select,original);
			}
		else
			{
			HighlightUsingExtensions(old,original);
			HighlightUsingExtensions(select, original);
			}
		}
	iDisplay.DeactivateContext();
	return TPoint(horizScrollBy,scrollBy);
	}

/** Sets the bitmap to be used as a line cursor. This function must be called
before attempting to draw the line cursor. Use SetCursorVisibilityL() to make
the line cursor visible.
@param aLineCursorBitmap The bitmap which represents the line cursor. */
EXPORT_C void CTextView::SetLineCursorBitmap(const CFbsBitmap* aLineCursorBitmap)
	{
	iCursor.SetLineCursorBitmap(aLineCursorBitmap);
	}

/** Sets the height and ascent of the text cursor to the values contained in
aFontSpec and redraws it.

Note:

The values set by this function are temporary. If the cursor is subsequently
moved, it reverts to being based on the preceding character (unless at the
start of a paragraph, in which case, it is based on the following character).

@param aFontSpec Specifies a height and ascent to which to set the text cursor.
*/
EXPORT_C void CTextView::MatchCursorHeightL(const TFontSpec& aFontSpec)
	{
	
	TInt fontHeight = 0;
	TInt fontAscent = 0;
	TRAPD(err,iLayout->GetFontHeightAndAscentL(aFontSpec,fontHeight,fontAscent));
	if (!err)
		iCursor.SetAscentAndDescent(fontAscent,fontHeight - fontAscent);
	if (err)
	    {
        OstTrace1( TRACE_FATAL, CTEXTVIEW_MATCHCURSORHEIGHTL, "Leave code=%d", err );
        User::Leave(err);
	    }
	}

/** Gets the current selection.
@return The current selection. */
EXPORT_C TCursorSelection CTextView::Selection() const
	{
	TCursorSelection selection;
	
	iCursorPos.GetSelection(selection);
	return selection;
	}

/** Tests whether there is a picture with a picture frame at the current cursor
position. If there is, the aPictureFrameRect and aDocPos arguments are set to
contain the picture frame rectangle and the document position of the picture,
respectively.

@param aPictureFrameRect If the function returns true, on return contains the
picture frame rectangle.
@param aDocPos If the function returns true, on return contains the document
position of the picture.
@return ETrue if there is a picture with a picture frame at the current cursor
position. EFalse if not. */
EXPORT_C TBool CTextView::IsPictureFrameSelected(TRect& aPictureFrameRect,TInt& aDocPos) const
	{
	if (iCursorPos.IsPictureFrame())
		{
		if (!iPictureFrame)
		    {
		    OstTrace0( TRACE_DUMP, CTEXTVIEW_ISPICTUREFRAMESELECTED, "EFNoPictureFrame" );
		    }
		__ASSERT_DEBUG(iPictureFrame,FormPanic(EFNoPictureFrame));
		aPictureFrameRect=iPictureFrame->Rect();
		aDocPos=Selection().LowerPos();
		return ETrue;
		}
	return EFalse;
	}

/** Gets the bounding rectangle of the picture, if any, located at the document
position or x,y coordinates specified, and returns it in aPictureRect.

If aCanScaleOrCrop is non-null, sets aCanScaleOrCrop to indicate 
whether the picture can be scaled or cropped. Returns true if the
operation was successful. Returns false otherwise; that is, if there is no
picture at the position, or if the position is unformatted.

@param aDocPos The document position of interest.
@param aXyPos The window coordinates of interest.
@param aPictureRect	On return, contains the rectangle which encloses the picture 
located at the position specified.
@param aCanScaleOrCrop If non null and the function returns true, indicates 
whether the picture can be scaled or cropped. By default, null.
@return ETrue if the operation was successful, (i.e. there is a picture 
character at the position, it has been loaded into memory, and the position is
formatted). EFalse if any of these conditions are not met. */
EXPORT_C TBool CTextView::GetPictureRectangleL(TInt aDocPos,TRect& aPictureRect,TBool* aCanScaleOrCrop/*=NULL*/) const
	{
	TBool isPicture=iLayout->PictureRectangleL(aDocPos,aPictureRect,aCanScaleOrCrop);
	
	iDrawTextLayoutContext.TextToWindow(aPictureRect);
	return isPicture;
	}

/** Returns EFalse if the specified position is not contained within a
formatted picture.
*/
EXPORT_C TBool CTextView::GetPictureRectangleL(TPoint aXyPos,TRect& aPictureRect,TBool* aCanScaleOrCrop/*=NULL*/)
	{
	iDrawTextLayoutContext.WindowToText(aXyPos);
	TBool isPicture=EFalse;
	TRAPD(err,
		isPicture = iLayout->PictureRectangleL(aXyPos, aPictureRect, aCanScaleOrCrop));
	if (err)
		NoMemoryL(err);
	iDrawTextLayoutContext.TextToWindow(aPictureRect);
	return isPicture;
	}

/** Gets the index of the nearest character in the document to the window coordinates
specified. The function first identifies the line which contains, or is nearest 
to, the vertical coordinate. Then it identifies the character in that line 
closest to the horizontal coordinate.

@param aPoint Window coordinates. On return, contains the window coordinates 
of the nearest character to the point. 
@return Index of the nearest character in the document to specified window coordinates. */
EXPORT_C TInt CTextView::XyPosToDocPosL(TPoint& aPoint)
	{
	NoMemoryCheckL();
	FinishBackgroundFormattingL();
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));

	iDrawTextLayoutContext.WindowToText(aPoint);
	TInt pos = iLayout->XyPosToDocPosL(aPoint);
	iDrawTextLayoutContext.TextToWindow(aPoint);
	return pos;
	}

/** Gets the window coordinates of the character located at the specified document 
position. If the document position specified has not been formatted, the function 
returns a value of false.

@param aDocPos The document position. 
@param aPoint On return, contains the window coordinates of aDocPos. This value 
is undefined if the position has not been formatted. 
@return True if the document position is formatted, false if not. */
EXPORT_C TBool CTextView::DocPosToXyPosL(TInt aDocPos,TPoint& aPoint)
	{
	NoMemoryCheckL();
	FinishBackgroundFormattingL();
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
	iLayout->ExtendFormattingToCoverPosL(aDocPos);
	TTmPosInfo2 pos_info;
	TTmDocPosSpec pos(aDocPos,
		aDocPos == iLayout->DocumentLength()?
		TTmDocPosSpec::ETrailing : TTmDocPosSpec::ELeading);
	TBool in_format = iLayout->FindDocPos(pos, pos_info);
	aPoint = pos_info.iEdge;
	iDrawTextLayoutContext.TextToWindow(aPoint);
	return in_format;
	}

/** Gets the document position of of the nearest character edge to the window coordinates 
specified.

@param aXyPos The window coordinates to investigate. 
@param aPosInfo If non-null, on return contains information about the position 
located in the text. 
@param aLineInfo If non-null, on return contains information on the line that 
the position is in. 
@return ETrue if the position was in the text and aPosInfo and aLineInfo have 
been set. */
EXPORT_C TBool CTextView::FindXyPosL(const TPoint& aXyPos, TTmPosInfo2& aPosInfo, TTmLineInfo* aLineInfo)
	{
	TPoint pos = aXyPos;
	iDrawTextLayoutContext.WindowToText(pos);
	iLayout->ExtendFormattingToCoverYL(pos.iY);
	return iLayout->FindXyPos(pos, aPosInfo, aLineInfo);
	}

/** Finds the x-y position of the document position aDocPos. 

If aDocPos is formatted, TRUE is returned and aPosInfo returns information 
about the document position and aLineInfo returns information about the line 
containing the document position, if it is non-null. 

@param aDocPos The document position to investigate. 
@param aPosInfo If non-null, on return, contains information about the document 
position.
@param aLineInfo If non-null, on return, contains information on the line that 
the position is in.
@return ETrue if the position was in the text and aPosInfo and aLineInfo have 
been set; EFalse otherwise. */
EXPORT_C TBool CTextView::FindDocPosL(const TTmDocPosSpec& aDocPos,TTmPosInfo2& aPosInfo,TTmLineInfo* aLineInfo)
	{
	TTagmaForwarder forwarder;
	forwarder.InitL(this);
	iLayout->ExtendFormattingToCoverPosL(aDocPos.iPos);
	TTmLineInfo line_info;
	TBool result = forwarder.FindDocPos(aDocPos,aPosInfo,line_info);
	if (aLineInfo)
		*aLineInfo = line_info;
	return result;
	}

/**
Returns a selection for which characters should be deleted in a forwards delete.
@return
	A selection for which characters should be deleted in a forwards delete.
*/	
EXPORT_C TCursorSelection CTextView::GetForwardDeletePositionL()
	{
	TCursorSelection selection = Selection();
	if (selection.Length() != 0)
		return selection;
	iLayout->ExtendFormattingToCoverPosL(Min(selection.iCursorPos + 1, iLayout->DocumentLength()));
	TInt anchor = iLayout->TagmaTextLayout().FindNextPos(selection.iCursorPos);
	if (anchor < 0)
		{
		selection.iAnchorPos = iLayout->DocumentLength();
		return selection;
		}

	if (selection.iCursorPos >= anchor)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_GETFORWARDDELETEPOSITIONL, "selection.iCursorPos >= anchor" );
	    }
	__ASSERT_DEBUG(selection.iCursorPos < anchor, User::Invariant());
	if (selection.Length())
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTVIEW_GETFORWARDDELETEPOSITIONL, "selection.Length()!=0" );
	    }
	__ASSERT_DEBUG(selection.Length() == 0, User::Invariant());

	// Now we have found the next formatted position. However, this
	// may have skipped a ligature. We should only delete the first character
	// in any ligature.
	// However, an indic syllable (glued together with viramas) should be
	// deleted all in one go.
	CTextLayout::TUtf32SourceCache sourceCache(*iLayout);
	TInt graphicalSpacingCharactersFound = 0;
	TBool ignoreNextSpacingCharacter = EFalse;
	for (TInt i = selection.iCursorPos; i != anchor; ++i)
		{
		if (i >= anchor)
		    {
		    OstTrace0( TRACE_DUMP, DUP2_CTEXTVIEW_GETFORWARDDELETEPOSITIONL, "i >= anchor" );
		    }
		__ASSERT_DEBUG(i < anchor, User::Invariant());
		TChar c = sourceCache.GetUtf32(i);
		if ( IsGraphicalSpacing(c) )
			{
			if (!ignoreNextSpacingCharacter)
				{
				if (graphicalSpacingCharactersFound != 0)
					{
					// Found two graphical spacing characters: don't
					// include the second
					selection.iAnchorPos = i;
					return selection;
					}
				++graphicalSpacingCharactersFound;
				}
			else
				ignoreNextSpacingCharacter = EFalse;
			}
		else if (IsVirama(c))
			{
			// virama joins a graphical spacing character to the
			// previous character, so the second one doesn't count.
			ignoreNextSpacingCharacter = ETrue;
			}
		}
	selection.iAnchorPos = anchor;
	return selection;
	}

/** Returns a selection for which characters should be deleted in a backwards
delete.
@return
	A selection for which characters should be deleted in a backwards delete.
*/

EXPORT_C TCursorSelection CTextView::GetBackwardDeletePositionL()
	{
	TCursorSelection selection = Selection();
	if (selection.Length() != 0)
		return selection;

	TInt anchor = KErrNotFound;
	TTmDocPos docPos;
	GetCursorPos(docPos);
	// loop through each previous line in case there are only zero width
	// chars before the cursor on this line. In practice, the following loop
	// should only ever iterate twice at most, because you can never have
	// more than one continuous line of zero-width characters (think about it)
	while (anchor==KErrNotFound && docPos.iPos > 0)
		{ 
		anchor = iLayout->TagmaTextLayout().FindPreviousPos(docPos.iPos);
		if (anchor==KErrNotFound) // no non-zero-width character before
			{                     // this position on this line
			// get first doc position on line then extend formatting to the one before
			TTmPosInfo2 dummy;
		   	TTmLineInfo currentLineInfo;
		   	iLayout->FindDocPos(docPos, dummy, &currentLineInfo);
		   	docPos.iPos = currentLineInfo.iStart;
			iLayout->ExtendFormattingToCoverPosL(Max(docPos.iPos - 1, 0));
			}
		}
	if (anchor==KErrNotFound)
		{
		selection.iAnchorPos = 0;
		return selection;
		}
	if (anchor >= selection.iCursorPos)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_GETBACKWARDDELETEPOSITIONL, "anchor >= selection.iCursorPos" );
	    }
	__ASSERT_DEBUG(anchor < selection.iCursorPos, User::Invariant());
	if (selection.Length() != 0)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTVIEW_GETBACKWARDDELETEPOSITIONL, "selection.Length() != 0" );
	    }
	__ASSERT_DEBUG(selection.Length() == 0, User::Invariant());

	// If this cluster is a ligature, it needs to be split.
	// Note that an indic syllable (where consonants are glued
	// together with viramas) does not count as a ligature.
	CTextLayout::TUtf32SourceCache sourceCache(*iLayout);
	for (TInt i = selection.iCursorPos - 1; anchor < i; --i)
		{
		if ( IsGraphicalSpacing(sourceCache.GetUtf32(i)) )
			{
			if ( !IsVirama(sourceCache.GetUtf32(i-1)) )
				{
				// Found two possible start points: cut to the last.
				selection.iAnchorPos = i;
				return selection;
				}
			}
		}
	// Not a ligature
	selection.iAnchorPos = anchor;
	return selection;
	}

/** Returns the rectangle enclosing the paragraph containing aDocPos. If the
paragraph is not formatted, returns an empty rectangle. If the paragraph is
partially formatted, returns the rectangle enclosing the formatted part.

@param aDocPos A document position within the paragraph.
@return The rectangle which encloses the paragraph containing aDocPos. */
EXPORT_C TRect CTextView::ParagraphRectL(TInt aDocPos) const
	{
	TRect r;
	TTmDocPos pos(aDocPos,
		aDocPos == iLayout->DocumentLength()? EFalse : ETrue);
	iLayout->GetParagraphRect(pos,r);
	iDrawTextLayoutContext.TextToWindow(r);
	return r;
	}

/** Formats the next line for background formatting, returns True if more to do
Enters OOM state before leaving */
TBool CTextView::NextLineL()
	{
	TBool moreToDo=EFalse;

	if (IsFormatting())
		{
		TInt formattedFrom=iFormattedUpTo;

		TRAPD(err,moreToDo=iLayout->FormatNextLineL(iFormattedUpTo));
		if (err)
			NoMemoryL(err);
		NotifyReformatL();
		TInt displayTo=iDrawTextLayoutContext.DisplayHeight();
		TBool drawAllLines=(iHorizontalScroll==EFPreviousHorizontalScroll && !moreToDo);
		if (iFormattedUpTo<displayTo && !drawAllLines)
			displayTo=iFormattedUpTo;
		DisplayNewLinesL(formattedFrom,displayTo);
		if (!moreToDo)
			{
			if (!drawAllLines)
				CheckScrollUpL();
			iHorizontalScroll=EFNoPreviousHorizontalScroll;
			}
		}
	return moreToDo;
 	}

/** Draws good parts of the screen that have recently been formatted by
background formatting. Enters OOM before leaving. */
void CTextView::DisplayNewLinesL(TInt aFrom,TInt aTo)
	{
	if (iNoMemory)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_DISPLAYNEWLINESL, "EFNoMemory" );
	    }
	__ASSERT_DEBUG(!iNoMemory,FormPanic(EFNoMemory));
	if (aTo>iGood)
		{
		ScrollTextL(aTo - iGood,iGood,0,FALSE);
		iGood=aTo;
		}
	iDisplay.SetRects(RScreenDisplay::EFClipViewRect);
	DisplayLineRangeL(aFrom,aTo);
	DrawCursor();
	}

void CTextView::DrawCursor(TUint aCursors)
	{
	iCursor.Draw(aCursors);
	}

/** Scrolls the screen up if necessary following background formatting. Enters
OOM state before leaving */
void CTextView::CheckScrollUpL()
	{
	if (iFormattedUpTo<iGood)
		{
		TInt height=iDrawTextLayoutContext.DisplayHeight();
		
		if (height>iGood)
			{
			TInt scroll=iFormattedUpTo-iGood;
			ScrollTextL(scroll,iGood,0,FALSE);
			//invalidate the area from the bottom of the last formatted line 
			//to the bottom of view rect
  			iDisplay.SetInvalidRect(iFormattedUpTo-height);						
			iFormattedUpTo+=height-iGood;
			iDisplay.SetRects(RScreenDisplay::EFClipViewRect|RScreenDisplay::EFClipInvalid);
			DrawTextL(iFormattedUpTo);
			}
		else if (height>iFormattedUpTo)
			{
			iDisplay.SetRects(RScreenDisplay::EFClipViewRect);
			DrawTextL(iFormattedUpTo);
			}
		}
	}

/**
CheckHorizontalScroll's job is to take the cursor position in aDocPos and
ensure it is visible in the view. It does this by calculating the horizontal
scroll neccessary and adjusts the iTextStartX (left text margin) attribute of
the view member iDrawTextLayoutContext. This change (if any) would be reflected
in the next re-draw.

This functionality is required when the layout is in non-screen/window wrapped
mode (ie. wysiwyg mode) and the cursor is out of view to the left or the right
(say if the user has horizontally scrolled) and either a cursor movement
happens or a char is inserted or deleted by the user. The end result is the
view jumps back to the cursor so the user can see when change/movement was
made.

When the layout format is set to screen mode this functionality may still be
required as paragraphs with wrapping turned off can break the margins. Even
with wrapping turned on, spaces and hanging characters can break margins;
however it is usually considered ugly to allow the screen to scroll to follow
the cursor in these situations.

@param aDocPos Cusror position to base calculations on
@post Drawing layout context updated with new horizontal scroll position
@return Amount left margin (horizontal scroll) adjusted by or 0 if none
*/
TInt CTextView::CheckHorizontalScroll (const TTmDocPos& aDocPos)
	{
	/* Here are the requirements for this function:

	(1) Areas beyond the text +10% of the window width +iHorizontalScrollJump
	must not be shown if the text edge is beyond its normal bounds, 
	except that 
		a distance of (window width - text width) beyond the texts
		bounds may be shown if this is positive.  
	or
		If this is not positive then a small amount of padding 
		(windowWidth - iLayout->WrapWidth) is allowed.

	(2) The cursor must be 10% of the window's width away from the window's
	edge if the text is wider than the window.

	(3) In all cases where (1) and (2) do not force otherwise, the scroll
	amount must be a multiple of iHorizontalScrollJump.

	(4) The screen must scroll the minimum amount allowed by requirements (1),
	(2) and (3).
	*/
	TInt windowWidth = iDrawTextLayoutContext.TextArea().Width();
	// Scroll position is the position on the screen at which the text
	// has its X coordinate
	TInt currentScrollPosition = iDrawTextLayoutContext.iTextStartX;
	TInt rightTextEdge;
	TInt leftTextEdge;
	iLayout->CalculateHorizontalExtremes(leftTextEdge, rightTextEdge, ETrue);

	TInt maximumScrollPosition = -leftTextEdge;
	TInt minimumScrollPosition = windowWidth - rightTextEdge;
	TInt padding = maximumScrollPosition < minimumScrollPosition?
		minimumScrollPosition - maximumScrollPosition : 
			((windowWidth - iLayout->WrapWidth() > 0) ? windowWidth - iLayout->WrapWidth() : 0);

	TInt buffer = windowWidth / 10;
	TInt textWrapWidth = iLayout->WrapWidth();
	// loosen the bounds by 10% of the window width + iHorizontalScrollJump
	// if the text is beyond them
	TInt leftAllowance = leftTextEdge < 0?
		buffer + iHorizontalScrollJump : 0;
	TInt rightAllowance = textWrapWidth < rightTextEdge?
		buffer + iHorizontalScrollJump : 0;
	maximumScrollPosition += Max(leftAllowance, padding);
	minimumScrollPosition -= Max(rightAllowance, padding);

	// requirement (2) sets the preferred min and max
	TInt preferredMinimumScrollPosition = minimumScrollPosition;
	TInt preferredMaximumScrollPosition = maximumScrollPosition;
	if (windowWidth < rightTextEdge - leftTextEdge)
		{
		TTmPosInfo2 posInfo;
		iLayout->FindDocPos(aDocPos, posInfo);
		TInt cursorWithinTextX = posInfo.iEdge.iX;
		
		preferredMinimumScrollPosition = buffer - cursorWithinTextX;
		preferredMaximumScrollPosition = windowWidth - buffer - cursorWithinTextX;

		// keep the preferred max and min within range
		preferredMinimumScrollPosition = Max(preferredMinimumScrollPosition, minimumScrollPosition);
		preferredMinimumScrollPosition = Min(preferredMinimumScrollPosition, maximumScrollPosition);
		
		preferredMaximumScrollPosition = Max(preferredMaximumScrollPosition, minimumScrollPosition);
		preferredMaximumScrollPosition = Min(preferredMaximumScrollPosition, maximumScrollPosition);
		}

	// Easy out if no scrolling will do
	if (preferredMinimumScrollPosition <= currentScrollPosition
		&& currentScrollPosition <= preferredMaximumScrollPosition)
		return 0;

	TInt suggestedPosition = currentScrollPosition;
	// Otherwise scrolling is either to just greater than the minimum or
	// just greater than the maximum.
	if (currentScrollPosition < preferredMinimumScrollPosition)
		{
		TInt jump = preferredMinimumScrollPosition - currentScrollPosition
			 + iHorizontalScrollJump - 1;
		jump = (jump / iHorizontalScrollJump) * iHorizontalScrollJump;
		suggestedPosition = Min(currentScrollPosition + jump,
			preferredMaximumScrollPosition);
		}
	else
		{
		TInt jump = currentScrollPosition - preferredMaximumScrollPosition
			 + iHorizontalScrollJump - 1;
		jump = (jump / iHorizontalScrollJump) * iHorizontalScrollJump;
		suggestedPosition = Max(currentScrollPosition - jump,
			preferredMinimumScrollPosition);
		}

	TInt delta = suggestedPosition - iDrawTextLayoutContext.iTextStartX;
	iDrawTextLayoutContext.iTextStartX = suggestedPosition;

	return delta;
	}

/** Reformats the whole document, or just the visible text, if formatting is
set to the band only (see CTextLayout::SetAmountToFormat()).

Notes:

Reformatting the entire document may be time consuming, so only use this
function if necessary.

This function does not redraw the reformatted text. */
EXPORT_C void CTextView::FormatTextL()
	{

	iCursor.MatchCursorHeightToAdjacentChar();
	iLayout->DiscardFormat();
	if (iNoMemory)
		{
		iNoMemory=EFRecovering;
		RecreateWindowServerObjectsL();
		}
	TRAPD(err,iLayout->FormatBandL());
	if (err)
		NoMemoryL(err);
	NotifyReformatL();
	iNoMemory=EFMemoryOK;
	}

void CTextView::ResetOffScreenBitmapContext(TAny* aTextView)
	{
	CTextView* that = reinterpret_cast<CTextView*>(aTextView);
	that->iOffScreenContext = 0;
	}

void CTextView::ResetExternalDraw(TAny* aTextView)
	{
	CTextView* that = reinterpret_cast<CTextView*>(aTextView);
	that->iLayout->ResetExternalDraw();
	}

/** Draws the text to a rectangle.

Draw to a specific GC rather than the stored one. This is essential when
drawing CTextView objects that are embedded inside others, as happens for
editable text controls in WEB (the outermost WEB object is a CTextView, which
has embedded controls that own CEikEdwins, and these own CTextView objects).
@param aRect
	Rectangle to which the text is drawn, normally the view rectangle.
@param aGc
	Graphics context to be drawn to. Flicker-free redrawing will not be applied,
	so this parameter should not be a window gc if flickering is to be avoided.
	Notice that the fonts drawn to this context will be taken from the graphics
	device map given to the constructor of this object or in subsequent calls to
	CTextLayout::SetImageDeviceMap. If such fonts cannot be used with this bitmap
	context, the behaviour is undefined.
*/
EXPORT_C void CTextView::DrawL(TRect aRect, CBitmapContext& aGc)
	{
	NoMemoryCheckL();
	iOffScreenContext = &aGc;
	CleanupStack::PushL(TCleanupItem(ResetOffScreenBitmapContext, this));
	DrawL(aRect);
	if (!iDisplay.IsLineCursor())
		DrawCursor();
	CleanupStack::PopAndDestroy();
	}

/** Draws the text to a rectangle. Enters OOM state before leaving.
RWindow::BeginRedraw() should always be called before calling this function,
for the window that the textview draws to.

@param aRect Rectangle to which the text is drawn, normally the view rectangle.
*/
EXPORT_C void CTextView::DrawL(TRect aRect)
	{		//make sure exactly the correct bits are drawn ### eg line cursor etc.
    TRect rect(iDrawTextLayoutContext.iViewRect);
    TInt start=iFormattedUpTo;

	if (NoMemoryCheckL())
		{
        iDisplay.Invalidate(rect);
		return;
		}
 
	if (!(iFlags & EFTextVisible))
		return;
	
	TRect drawRect(aRect);
	iDrawTextLayoutContext.WindowToText(drawRect);
	iLayout->SetReadyToRedraw();
	iLayout->SetExternalDraw(drawRect); // view coordinates

	FinishBackgroundFormattingL();
    if (start<iFormattedUpTo)
		{
        rect.iTl.iY=start;
        rect.iBr.iY=iFormattedUpTo;
        iDisplay.Invalidate(rect);
	    }
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
	iDisplay.SetInvalidRect(aRect);
	iDrawTextLayoutContext.WindowToText(aRect);
	iDisplay.SetRects(RScreenDisplay::EFClipViewRect|RScreenDisplay::EFClipInvalid);

	CleanupStack::PushL(TCleanupItem(ResetExternalDraw, this));

	DrawTextL(aRect.iTl.iY,aRect.iBr.iY);

	CleanupStack::PopAndDestroy();
	}

/** Clears the specified number of pixels from the bottom of the screen. Enters
OOM state before leaving. */
void CTextView::ClearRectAtBottom(TInt aHeight)
	{
	iDisplay.AddRects(RScreenDisplay::EFClipViewRect);
	iDisplay.ActivateContext();
	iDisplay.ResetClippingRect();
	TRect rect(ViewRect());
	rect.iTl.iY += aHeight;

	iLayout->BeginRedraw(rect);
	iDisplay.ClearRect(rect);
	iLayout->EndRedraw();

	iDisplay.DeactivateContext();
	}

void CTextView::DrawTextL(TInt aFromHeight,TInt aToHeight)
	{
	TInt layHeight=iLayout->YBottomLastFormattedLine();
	TInt winHeight=iDrawTextLayoutContext.DisplayHeight();

	if (layHeight>winHeight)
		layHeight=winHeight;
	if (layHeight<aToHeight)
		aToHeight=layHeight;
	DisplayLineRangeL(aFromHeight,aToHeight);
	if (layHeight<winHeight && layHeight==aToHeight)		//Only last line has been drawn
		ClearRectAtBottom(layHeight);		//Must be done last as it changes the flags in RScreenDisplay
	DrawCursor();
	}

 

/** Reformats and redraws the view in response to a single key press. The
precise amount of text to be reformatted is controlled by the second parameter.

@param aType Indicates the type of edit which has taken place.
CTextLayout::EFCharacterInsert (the default) for a character insertion,
CTextLayout::EFParagraphDelimiter for a paragraph delimiter insertion,
CTextLayout::EFLeftDelete or CTextLayout::EFRightDelete for a character or
paragraph delimiter deletion to the left or right of the cursor position.
@param aFormatChanged ETrue if text is to be reformatted from the start of the
paragraph the cursor was on before the edit, EFalse if from the start of the
line the cursor was on before the edit.
@return The number of pixels by which the text had to be scrolled vertically
(positive means text moved down). */
EXPORT_C TInt CTextView::HandleCharEditL(TUint aType/*=EFCharacterInsert*/
	,TBool aFormatChanged/*=EFalse*/)
	{
	iContextIsNavigation = EFalse;
	TInt good = 0;
	TInt formattedFrom = 0;   
	TInt from = 0;
 	TBool formatting = IsFormatting();
	TBool moreToDo = ETrue;

	if (iCursorPos.IsSelection())
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_HANDLECHAREDITL, "EFSelectionCannotCharEdit" );
	    }
	__ASSERT_DEBUG(!iCursorPos.IsSelection(),FormPanic(EFSelectionCannotCharEdit));
	if (aType>CTextLayout::EFRightDelete)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTVIEW_HANDLECHAREDITL, "EFBadCharacterEditType" );
	    }
	__ASSERT_DEBUG(aType<=CTextLayout::EFRightDelete,FormPanic(EFBadCharacterEditType));
	if ( !(!aFormatChanged || aType==CTextLayout::EFRightDelete 
                            || aType==CTextLayout::EFLeftDelete) )
	    {
	    OstTrace0( TRACE_DUMP, DUP2_CTEXTVIEW_HANDLECHAREDITL, "EFBadCharacterEditType" );
	    }
	__ASSERT_DEBUG(!aFormatChanged || aType==CTextLayout::EFRightDelete 
							|| aType==CTextLayout::EFLeftDelete,FormPanic(EFBadCharacterEditType));
	TBool recovered=NoMemoryCheckL();
	iCursor.MatchCursorHeightToAdjacentChar();
	TPoint dummyPoint;
	if ( !(iLayout->PosInBand(iCursorPos.TmDocPos(),dummyPoint)
	        || !formatting) )
	    {
	    OstTrace0( TRACE_DUMP, DUP3_CTEXTVIEW_HANDLECHAREDITL, "EFBackgroundFormatting" );
	    }
	__ASSERT_DEBUG(iLayout->PosInBand(iCursorPos.TmDocPos(),dummyPoint)
		|| !formatting, FormPanic(EFBackgroundFormatting));
	TTmDocPos doc_pos = iCursorPos.TmDocPos();
	if ((!formatting && !iLayout->PosInBand(doc_pos,dummyPoint)) || recovered)
		{
		TInt y=0;
		switch (aType)
			{
		case CTextLayout::EFCharacterInsert:
			doc_pos.iLeadingEdge = EFalse;
			++doc_pos.iPos;
			break;
		case CTextLayout::EFParagraphDelimiter:
			++doc_pos.iPos;
			doc_pos.iLeadingEdge = ETrue;
			break;
		case CTextLayout::EFLeftDelete:
			--doc_pos.iPos;
			doc_pos.iLeadingEdge = EFalse;
			break;
		default:
			doc_pos.iLeadingEdge = EFalse;
			break;
			}
		iCursorPos.SetDocPos(doc_pos);

		if (recovered)
			return 0;
		else
			return ViewTopOfLineL(iCursorPos.TmDocPos(),y,CTextView::EFViewDiscardAllFormat).iY;
		}
	TInt scroll = 0;
	TRAPD(err,moreToDo = iLayout->HandleCharEditL(aType,doc_pos.iPos,good,iFormattedUpTo,formattedFrom,
												  scroll,aFormatChanged));

	doc_pos.iLeadingEdge =
		aType == CTextLayout::EFParagraphDelimiter?
		ETrue : EFalse;
	if (doc_pos.iPos == 0)
		doc_pos.iLeadingEdge = EFalse;

	// If this document position will not yield a cursor, try the leading edge
	if (!doc_pos.iLeadingEdge)
		{
		TRect line;
		TPoint origin;
		TInt width, ascent, descent;
		if (!iLayout->GetCursor(doc_pos, ECursorVertical,
			line, origin, width, ascent, descent))
			doc_pos.iLeadingEdge = ETrue;
		}
	iCursorPos.SetDocPos(doc_pos);
	if (err)
		NoMemoryL(err);
	NotifyReformatL();
	if (formatting==EFalse)
		{
		iGood=good;
		if (moreToDo)
			StartIdleObject();
		}
	formattedFrom=Max(formattedFrom,0);
	TInt to=Min(iFormattedUpTo,iDrawTextLayoutContext.DisplayHeight());
#if defined(_DEBUG)
	TInt oldFormattedFrom = formattedFrom;
#endif
	if (scroll<0 && formattedFrom>0)
		ScrollRect(scroll,from,formattedFrom);
	__ASSERT_DEBUG(from==0 && formattedFrom==oldFormattedFrom,FormPanic(EFScrollCurtailed));
	if (to>=formattedFrom)
		DisplayNewLinesL(formattedFrom,to);
	if (!moreToDo)
		CheckScrollUpL();
	iCursorPos.UpdateLatentPosition();
	TInt horizScrollBy=CheckHorizontalScroll(iCursorPos.TmDocPos());	// Have had to put at end, because sometimes format all above
	if (horizScrollBy!=0)
		{
		ScrollDisplayL();
		if (moreToDo)
			iHorizontalScroll=EFPreviousHorizontalScroll;
		}
	return scroll;
	}

/** Reformats after a change to part of the text contents. Enters OOM state
before leaving. */
TPoint CTextView::HandleBlockChangeL(TCursorSelection aSelection,TInt aOldCharsChanged
	,TBool aFormatChanged)
	{
	TViewRectChanges viewChanges;
	TCursorSelection selection;

	iCursorPos.GetSelection(selection);
	TRAPD(err,iLayout->HandleBlockChangeL(aSelection,aOldCharsChanged,viewChanges,aFormatChanged));
	if (err)
		NoMemoryL(err);
	NotifyReformatL();
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));

	TInt from=0;	
	TInt to=iDrawTextLayoutContext.DisplayHeight();
	TInt horizScrollBy=CheckHorizontalScroll(iCursorPos.TmDocPos());
	if (horizScrollBy!=0)
		{
		TInt yBot=iLayout->YBottomLastFormattedLine();
		viewChanges.iFormattedFrom=0;
		if (yBot<to)
			to=yBot;
		viewChanges.iFormattedTo=to;
		}
	else
		{
		iDisplay.SetRects(RScreenDisplay::EFClipViewRect);
		iDisplay.ActivateContext();
		iDisplay.ResetClippingRect();
		if (viewChanges.iScrollAtBottom>0 && viewChanges.iFormattedTo<to)
 			ScrollRect(viewChanges.iScrollAtBottom,viewChanges.iFormattedTo,to);
		if (viewChanges.iScrollAtTop!=0 && viewChanges.iFormattedFrom>0)
			ScrollRect(viewChanges.iScrollAtTop,from,viewChanges.iFormattedFrom);
		if (viewChanges.iScrollAtBottom<0 && viewChanges.iFormattedTo<to)
			ScrollRect(viewChanges.iScrollAtBottom,viewChanges.iFormattedTo,to);
		iDisplay.DeactivateContext();
		}
	iDisplay.SetRects(RScreenDisplay::EFClipViewRect);
	if (from>viewChanges.iFormattedFrom && from>0)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTVIEW_HANDLEBLOCKCHANGEL, "EFScrollError" );
	    }
	__ASSERT_DEBUG(from<=viewChanges.iFormattedFrom || from<=0,FormPanic(EFScrollError));
	if (to<viewChanges.iFormattedTo && to<iDrawTextLayoutContext.DisplayHeight())
	    {
	    OstTrace0( TRACE_DUMP, DUP2_CTEXTVIEW_HANDLEBLOCKCHANGEL, "EFScrollError" );
	    }
	__ASSERT_DEBUG(to>=viewChanges.iFormattedTo || to>=iDrawTextLayoutContext.DisplayHeight(),FormPanic(EFScrollError));
	if (from>0)
		DisplayLineRangeL(0,from);
	DisplayLineRangeL(viewChanges.iFormattedFrom,viewChanges.iFormattedTo);
	if (to<iDrawTextLayoutContext.DisplayHeight())
		DrawTextL(to);
	else
		DrawCursor();
	iCursorPos.UpdateLatentPosition();
	TInt vertScroll=viewChanges.iScrollAtTop;
	if (selection.LowerPos()!=selection.iCursorPos)
		vertScroll=viewChanges.iScrollAtBottom;
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
	return TPoint(horizScrollBy,vertScroll);
	}

/** Makes a selection, but without updating the view.

This function would only be used before calling CTextView::HandleRangeFormatChangeL() 
or HandleInsertDeleteL() to make a selection which will become visible (if 
selection visibility is on) after the change handling function has returned.

@param aSelection The range to be selected after calling CTextView::HandleRangeFormatChangeL() 
or HandleInsertDeleteL(). */
EXPORT_C void CTextView::SetPendingSelection(const TCursorSelection& aSelection)
	{
	iCursorPos.SetPendingSelection(aSelection);
	}

/** Reformats and redraws the view after changes to the character or paragraph
formatting of a block of text. Reformatting can take place either from the
start of the line or the start of the paragraph containing the cursor position,
depending on aFormatChanged.

Can be used in combination with SetPendingSelection() to set a selection which
should remain after the function has returned.

Note:

A panic occurs if partial lines are excluded from the view (see
CTextLayout::ExcludingPartialLines()).

Any background formatting is forced to complete first.

@param aSelection Indicates the text range which has been reformatted.
@param aFormatChanged ETrue if text is to be reformatted from the start of the
paragraph the cursor was on before the edit, EFalse if from the start of the
line the cursor was on before the edit.
@return The number of pixels scrolled horizontally and vertically. */
EXPORT_C TPoint CTextView::HandleRangeFormatChangeL(TCursorSelection aSelection
	,TBool aFormatChanged/*=EFalse*/)
	{
	if (NoMemoryCheckL() || (!aFormatChanged && aSelection.Length()==0))
		return TPoint(0,0);
	FinishBackgroundFormattingL();
	iCursor.MatchCursorHeightToAdjacentChar();
	if (!iLayout->PosIsFormatted(iCursorPos.DocPos()))
		{
		TInt y=0;
		return ViewTopOfLineL(iCursorPos.TmDocPos(),y,CTextView::EFViewDiscardAllFormat);
		}
	return HandleBlockChangeL(aSelection,aSelection.Length(),aFormatChanged);
	}

/** Reformats and redraws the view after inserting, deleting or replacing a
block of text.

Can be used in combination with SetPendingSelection() to set a selection which
should remain after the function has returned.

Notes:

If inserting or deleting a single character, use CTextView::HandleCharEditL()
instead.

A panic occurs if partial lines are excluded from the view (see
CTextLayout::ExcludingPartialLines()).

@param aSelection The start and new length of the changed block. If this
function is being used to handle deletion only, this argument should be of
length zero and its start position should be the beginning of the deletion.
@param aDeletedChars The number of deleted characters. Specify zero if this
function is being used to handle insertion only.
@param aFormatChanged ETrue if text is to be reformatted from the start of the
paragraph the cursor was on before the edit, EFalse if from the start of the
line the cursor was on before the edit.
@return The number of pixels scrolled horizontally and vertically. */
EXPORT_C TPoint CTextView::HandleInsertDeleteL(TCursorSelection aSelection,TInt aDeletedChars
	,TBool aFormatChanged/*=EFalse*/)
	{
	iContextIsNavigation = EFalse;
	TInt docPos=aSelection.LowerPos();
	if (NoMemoryCheckL())
		return TPoint(0,CTextLayout::EFScrollRedrawWholeScreen);
	if (IsFormatting())
		{
		TViewYPosQualifier yPosQualifier;
		TPoint scroll;
		iCursorPos.SetPendingSelection(aSelection);
		scroll=DoHandleGlobalChangeL(yPosQualifier,EFViewDiscardAllFormat);
		return scroll;
		}
	iCursor.MatchCursorHeightToAdjacentChar();
	if (aSelection.iCursorPos!=aSelection.LowerPos())
		docPos+=aDeletedChars;
	if (!iLayout->PosIsFormatted(docPos))
		{
		TInt y=0;
		TTmDocPos pos(aSelection.iCursorPos, EFalse);
		if (pos.iPos == 0)
			pos.iLeadingEdge = ETrue;
		return ViewTopOfLineL(pos,y,CTextView::EFViewDiscardAllFormat);
		}
	return HandleBlockChangeL(aSelection,aDeletedChars,aFormatChanged);
	}

/** Moves the text cursor to a new document position. The view is scrolled so
that the line containing the new cursor position is at the top, or the bottom,
if the scroll direction was downwards.

Any background formatting is forced to complete first.

@param aDocPos
	The new document position of the text cursor.
@param aDragSelectOn
	ETrue if the region between the old and new cursor positions should be
	selected, EFalse if not. If ETrue, any existing selection remains selected,
	so that this function can be used to extend or shorten a selected region.
	EFalse cancels any existing selection.
@return
	The number of pixels scrolled horizontally and vertically. */
EXPORT_C TPoint CTextView::SetDocPosL(const TTmDocPos& aDocPos,TBool aDragSelectOn)
	{
	iContextIsNavigation = ETrue;
	TInt vertical_scroll = 0;
	NoMemoryCheckL();
	FinishBackgroundFormattingL();
	iCursor.MatchCursorHeightToAdjacentChar();
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
	TTmDocPos pos(aDocPos);
	if (pos.iPos < 0)
		{
		pos.iPos = 0;
		pos.iLeadingEdge = EFalse;
		}
	else
		{
		TInt doc_length = iLayout->DocumentLength();
		if (pos.iPos > doc_length)
			{
			pos.iPos = doc_length;
			pos.iLeadingEdge = ETrue;
			}
		}
	TRAPD(error,vertical_scroll = iCursorPos.SetDocPosL(aDragSelectOn,pos));
	if (error)
		NoMemoryL(error);
	TInt horizontal_scroll = DrawAfterCursorMoveL(vertical_scroll);
	if (iCursorPos.IsSelectionToDraw())
		UpdateHighlightL();
	return TPoint(horizontal_scroll,vertical_scroll);
	}

 
/** Moves the text cursor to a new document position. The view is scrolled so
that the line containing the new cursor position is at the top, or the bottom,
if the scroll direction was downwards.

Any background formatting is forced to complete first.

@param aDocPos
	The new document position of the text cursor.
@param aDragSelectOn
	ETrue if the region between the old and new cursor positions should be
	selected, EFalse if not. If ETrue, any existing selection remains selected,
	so that this function can be used to extend or shorten a selected region.
	EFalse cancels any existing selection.
@return
	The number of pixels scrolled horizontally and vertically. */
EXPORT_C TPoint CTextView::SetDocPosL(TInt aDocPos,TBool aDragSelectOn)
	{
	TTmDocPos pos(aDocPos,
		aDocPos == iLayout->DocumentLength()? EFalse : ETrue);
	return SetDocPosL(pos,aDragSelectOn);
	}

/** Moves the text cursor to a new document position. The view is scrolled so
that the line containing the new cursor position is at the top, or the bottom,
if the scroll direction was downwards.

Any background formatting is forced to complete first.

@param aDocPos
	The new document position of the text cursor.
@param aDragSelectOn
	ETrue if the region between the old and new cursor positions should be
	selected, EFalse if not. If ETrue, any existing selection remains selected,
	so that this function can be used to extend or shorten a selected region.
	EFalse cancels any existing selection.
@return
	The number of pixels scrolled horizontally and vertically. */
EXPORT_C TPoint CTextView::SetDocPosL(const TTmDocPosSpec& aDocPos,TBool aDragSelectOn)
	{
	TTmDocPos pos(aDocPos.iPos, aDocPos.iType == TTmDocPosSpec::ELeading);
	return SetDocPosL(pos, aDragSelectOn);
	}

/** Moves the text cursor to the nearest point on the screen to the x,y coordinates 
specified.

Any background formatting is forced to complete first.

@param aPos The window coordinates to which to move the text cursor. 
@param aDragSelectOn ETrue if the region between the old and new cursor positions 
should be selected, EFalse if not. If ETrue, any existing selection remains 
selected, so that this function can be used to extend or shorten a selected 
region.EFalse cancels any existing selection. 
@param aPictureRect On return, a pointer to the rectangle enclosing the picture 
at position aPos. If there is no picture at this position, returns NULL.
@param aPictureFrameEdges On return, indicates whether aPos is located within 
the active region around a picture edge. The active region is the region inside 
which the cursor can be used to resize the picture. A value of zero indicates 
that aPos is not within this region. Otherwise, this value indicates which 
active region aPos is located in. For more information, see TFrameOverlay::TEdges. 

@return The number of pixels scrolled horizontally and vertically. */
EXPORT_C TPoint CTextView::SetXyPosL(TPoint aPos,TBool aDragSelectOn,TRect*& aPictureRect,TInt& aPictureFrameEdges)
	{
	iContextIsNavigation = ETrue;
	NoMemoryCheckL();
	FinishBackgroundFormattingL();
	iCursor.MatchCursorHeightToAdjacentChar();
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
	aPictureFrameEdges=TFrameOverlay::ENoEdges;
	if (iPictureFrame && iCursorPos.IsPictureFrame() && SelectionVisible())
		{
		aPictureFrameEdges=iPictureFrame->XyPosToEdges(aPos);
		if (aPictureFrameEdges!=TFrameOverlay::ENoEdges && iDrawTextLayoutContext.TextArea().Contains(aPos))
			{
			aPos=iPictureFrame->Rect().Center();
			aPictureRect=CONST_CAST(TRect*,&iPictureFrame->RefRect());
			return TPoint(0,0);
			}
		}
	iDrawTextLayoutContext.WindowToText(aPos);
	TInt scrollBy=0;		//To stop warning
	TRAPD(err,scrollBy=iCursorPos.SetXyPosL(aDragSelectOn,aPos,(iFlags & EFPictureFrameEnabled)));
	// Have we just moved over a picture?
	if(iCursorPos.IsPictureFrame()|| iCursorPos.IsNewPictureFrame())
		UpdatePictureFrameL();

	if (err)
		NoMemoryL(err);
	TInt horizScrollBy=DrawAfterCursorMoveL(scrollBy);
	if (iCursorPos.IsSelectionToDraw())
		UpdateHighlightL();
	if (iCursorPos.IsPictureFrame() && SelectionVisible())		//Should the rectangle be returned when there is color dimming #JD# The way it is written here it will not be
		aPictureRect=CONST_CAST(TRect*,&iPictureFrame->RefRect());
	else
		aPictureRect=NULL;
	return TPoint(horizScrollBy,scrollBy);
	}

/** Scrolls the view either horizontally or vertically and does a redraw.

Any background formatting is forced to complete first.

@param aMovement Controls the direction and the amount of scroll. 
@param aScrollBlankSpace CTextLayout::EFAllowScrollingBlankSpace allows blank 
space to scroll into the visible area (applies when scrolling horizontally 
as well as vertically), CTextLayout::EFDisallowScrollingBlankSpace prevents 
blank space from scrolling into the visible area. 
@pre aMovement must not scroll the display beyond the formatted range. If aMovement
scrolls beyond the formatted range, this method will leave with the error code
CTextLayout::EPosNotFormatted
@return The number of pixels scrolled */
EXPORT_C TInt CTextView::ScrollDisplayL(TCursorPosition::TMovementType aMovement
					,CTextLayout::TAllowDisallow aScrollBlankSpace/*=EFDisallowScrollingBlankSpace*/)
	{
	TInt pixels=0;		//To stop warning

	NoMemoryCheckL();
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
	FinishBackgroundFormattingL();
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
	TRAPD(err,pixels=DoScrollDisplayL(aMovement,aScrollBlankSpace));
	if (err)
		{
		if (err == CTextLayout::EPosNotFormatted)
			User::Leave(err);
		else
			NoMemoryL(err);
		}
	return pixels;
	}

TInt CTextView::DoHorizontalScrollDisplayL(TCursorPosition::TMovementType aMovement
	,CTextLayout::TAllowDisallow aScrollBlankSpace)
	{
	TInt pixels=0;
	TInt leftX,rightX;

	CalculateHorizontalExtremes(leftX,rightX,ETrue);
	TInt rightEdgeScroll=iDrawTextLayoutContext.TextArea().Width()-rightX;
	switch (aMovement)
		{
	case TCursorPosition::EFLeft:
		pixels=iHorizontalScrollJump;
		if (aScrollBlankSpace==CTextLayout::EFDisallowScrollingBlankSpace)
			pixels=Min(pixels,Max(-leftX,0));
		break;
	case TCursorPosition::EFRight:
		pixels=-iHorizontalScrollJump;
		if (aScrollBlankSpace==CTextLayout::EFDisallowScrollingBlankSpace)
			pixels=Max(pixels,Min(rightEdgeScroll,0));
		break;
	case TCursorPosition::EFLineBeg:
		pixels=-leftX;
		break;
	case TCursorPosition::EFLineEnd:
		pixels=rightEdgeScroll;
		break;
	default:
		#if defined(_DEBUG)
			FormPanic(EFInvalidScrollingType)
		#endif
		;
		}
	SetLeftTextMargin(LeftTextMargin()-pixels);

	if (0 != pixels)
		{
		ScrollDisplayL();
		}
	else
		{
		DrawCursor();
		}

	return pixels;
	}

TInt CTextView::DoScrollDisplayL(TCursorPosition::TMovementType aMovement,
								 CTextLayout::TAllowDisallow aScrollBlankSpace)
	{
	TInt pixels = 0;
	TInt lines = 0;
	TInt cursor_pos = 0;

	switch (aMovement)
		{
		case TCursorPosition::EFLeft:
		case TCursorPosition::EFRight:
		case TCursorPosition::EFLineBeg:
		case TCursorPosition::EFLineEnd:
			return DoHorizontalScrollDisplayL(aMovement,aScrollBlankSpace);

		case TCursorPosition::EFLineUp:
			lines = 1;
			break;

		case TCursorPosition::EFLineDown:
			lines = -1;
			break;

		case TCursorPosition::EFPageUp:
			iLayout->PageUpL(cursor_pos,pixels);
			break;

		case TCursorPosition::EFPageDown:
			cursor_pos = iDrawTextLayoutContext.DisplayHeight() - 1;
			iLayout->PageDownL(cursor_pos,pixels);
			break;

		default:
			break;
		}

	if (lines)
		pixels = iLayout->ScrollLinesL(lines,aScrollBlankSpace);

	if (pixels)
		{
		iCursorPos.TextMoveVertically();
		ScrollDisplayL();
		}

	return pixels;
	}

/** Moves the text cursor in the direction and manner specified. If necessary,
the view is scrolled so that the line containing the new cursor position is
visible.

Any background formatting is forced to complete first.

@param aMovement The direction and manner in which to move the cursor. On
return set to the actual cursor movement. The actual cursor movement may be
different from the one requested if for example the desired cursor movement is
upwards and the cursor is already on the top line. In this case, this argument
will return TCursorPosition::EFLineBeg. A return value of
TCursorPosition::EFNoMovement indicates that no cursor movement took place.
@param aDragSelectOn ETrue if the region between the old and new cursor
positions should be selected, EFalse if not. If ETrue, any existing selection
remains selected, so that this function can be used to extend or shorten a
selected region. EFalse cancels any existing selection.
@return The number of pixels scrolled horizontally and vertically. */
EXPORT_C TPoint CTextView::MoveCursorL(TCursorPosition::TMovementType& aMovement,TBool aDragSelectOn)
	{
	iContextIsNavigation = ETrue;
	NoMemoryCheckL();
	FinishBackgroundFormattingL();
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
	iCursor.MatchCursorHeightToAdjacentChar();
	return DoMoveCursorL(aDragSelectOn, aMovement,
		iFlags & EFPictureFrameEnabled);
	}

TPoint CTextView::DoMoveCursorL(TBool aDragSelectOn,TCursorPosition::TMovementType& aMovement
	,TBool aAllowPictureFrame)
	{
	TInt scrollBy=0;		//To avoid complier warning

	TRAPD(err,scrollBy=iCursorPos.MoveL(aDragSelectOn,aMovement,aAllowPictureFrame));
	// Have we just moved over a picture?
	if(iCursorPos.IsPictureFrame()|| iCursorPos.IsNewPictureFrame())
		UpdatePictureFrameL();

	if (err)
		NoMemoryL(err);
	TInt horizScrollBy=DrawAfterCursorMoveL(scrollBy);
	if (iCursorPos.IsSelectionToDraw())
		UpdateHighlightL();
	return TPoint(horizScrollBy,scrollBy);
	}

/** Scrolls the view vertically by a number of pixels, disallowing blank space
from scrolling into the bottom of the visible area. Redraws the newly visible
lines.

Any background formatting is forced to complete first.

@param aDeltaY
	The number of pixels to scroll; may be a positive or negative value. On
	return, contains the number of pixels actually scrolled. Positive values
	move the text down, negative move it up. */
EXPORT_C void CTextView::ScrollDisplayPixelsL(TInt& aDeltaY)
	{

	NoMemoryCheckL();
	FinishBackgroundFormattingL();
	TRAPD(err,iLayout->ChangeBandTopL(aDeltaY));
	if (err)
		NoMemoryL(err);

	if (0 != aDeltaY)
		{
		ScrollDisplayL();
		}
	else
		{
		DrawCursor();
		}
	} 

/** Scrolls the view vertically by a number of pixels, allowing blank space
from scrolling into both top and bottom of the visible area, which means the 
scrolling can go beyond the top or bottom border. Redraws the newly visible
lines.

Any background formatting is forced to complete first.

@param aDeltaY
    The number of pixels to scroll; may be a positive or negative value. On
    return, contains the number of pixels actually scrolled. Positive values
    move the text down, negative move it up. */
EXPORT_C void CTextView::ScrollDisplayPixelsNoLimitBorderL(TInt aDeltaY)
    {
    NoMemoryCheckL();
    FinishBackgroundFormattingL();
    TRAPD(err,iLayout->ChangeBandTopNoLimitBorderL(aDeltaY));
    if (err)
        NoMemoryL(err);

    if (0 != aDeltaY)
        {
        ScrollDisplayL();
        }
    else
        {
        DrawCursor();
        }
    } 

/** Scrolls the view vertically by a number of wholly or partially visible
lines, disallowing blank space at the bottom of the visible area if
aScrollBlankSpace is CTextLayout::EFDisallowScrollingBlankSpace. Redraws the
newly visible lines.

Any background formatting is forced to complete first.

@param aDeltaLines
	The number of lines to scroll; may be a positive or negative value.
	Positive values move the text down, negative move it up. On return,
	contains the number of lines not scrolled; that is, the requested number,
	minus the number actually scrolled.
@param aScrollBlankSpace
	Only relevant when scrolling downwards.
	CTextLayout::EFAllowScrollingBlankSpace allows blank space to scroll into
	the visible area. CTextLayout::EFDisallowScrollingBlankSpace prevents blank
	space from scrolling into the visible area.
@pre aDeltaLines must not scroll the display beyond the formatted range. If aDeltaLines
	scrolls beyond the formatted range, this method will leave with the error code
	CTextLayout::EPosNotFormatted
@return The number of pixels scrolled.
*/
EXPORT_C TInt CTextView::ScrollDisplayLinesL(TInt& aDeltaLines
					,CTextLayout::TAllowDisallow aScrollBlankSpace/*=EFDisallowScrollingBlankSpace*/)
	{
	TInt pixels=0;

	NoMemoryCheckL();
	FinishBackgroundFormattingL();
	TRAPD(err,pixels=iLayout->ScrollLinesL(aDeltaLines,aScrollBlankSpace));
	if (err)
 		{
 		if (err == CTextLayout::EPosNotFormatted)
 			User::Leave(err);
 		else
 			NoMemoryL(err);
 		}

	if (0 != pixels)
		{
		ScrollDisplayL();
		}
	else
		{
		DrawCursor();
		}

	return pixels;
	} 

/** Scrolls the view by a number of wholly or partially visible paragraphs
disallowing blank space at the bottom of the visible area if aScrollBlankSpace
is CTextLayout::EFDisallowScrollingBlankSpace. Redraws the newly visible
paragraphs.

Any background formatting is forced to complete first.

@param aDeltaParas
	The number of paragraphs to scroll; may be a positive or negative value;
	positive values move the text down, negative move it up. On return,
	contains the number of paragraphs not scrolled; that is the difference
	between the requested number and the number of paragraphs actually
	scrolled.
@param aScrollBlankSpace
	Only relevant when scrolling downwards.
	CTextLayout::EFAllowScrollingBlankSpace allows blank space to scroll into
	the visible area. CTextLayout::EFDisallowScrollingBlankSpace prevents blank
	space from scrolling into the visible area.
@pre aDeltaParas must not scroll the display beyond the formatted range. If aDeltaParas
	scrolls beyond the formatted range, this method will leave with the error code
	CTextLayout::EPosNotFormatted
@return The number of pixels scrolled. */
EXPORT_C TInt CTextView::ScrollDisplayParagraphsL(TInt& aDeltaParas
					,CTextLayout::TAllowDisallow aScrollBlankSpace/*=EFDisallowScrollingBlankSpace*/)
	{
	TInt pixels=0;

	NoMemoryCheckL();
	FinishBackgroundFormattingL();
	TRAPD(err,pixels=iLayout->ScrollParagraphsL(aDeltaParas,aScrollBlankSpace));
	if (err)
 		{
 		if (err == CTextLayout::EPosNotFormatted)
 			User::Leave(err);
 		else
 			NoMemoryL(err);
 		}

	if (0 != pixels)
		{
		ScrollDisplayL();
		}
	else
		{
		DrawCursor();
		}

	return pixels;
	}

/** Returns the left and right extremes, in window coordinates, of the formatted 
text.

@param aLeftX On return, contains the x coordinate of the leftmost point of 
the formatted text. 
@param aRightX On return, contains the x coordinate of the rightmost point 
of the formatted text. 
@param aOnlyVisibleLines If ETrue, only scans partially or fully visible lines. 
If EFalse, scans all the formatted text. */
EXPORT_C void CTextView::CalculateHorizontalExtremesL(TInt& aLeftX,TInt& aRightX,TBool aOnlyVisibleLines)
	{

	CalculateHorizontalExtremes(aLeftX,aRightX,aOnlyVisibleLines);
	}

void CTextView::CalculateHorizontalExtremes(TInt& aLeftX,TInt& aRightX,TBool aOnlyVisibleLines)
	{
	iLayout->CalculateHorizontalExtremes(aLeftX,aRightX,aOnlyVisibleLines);
	aLeftX += iDrawTextLayoutContext.iTextStartX;
	aRightX += iDrawTextLayoutContext.iTextStartX;
	}

/** Scroll the display below point aFrom by aScrollX.

aScrollY>0 ==> text moves down.

aScrollX>0 ==> text moves right.

aFrom is relative to the text area of the window.

aScrollBackground determines whether the background can scroll along with the text.

Enters OOM before leaving */
void CTextView::ScrollTextL(TInt aScrollY,TInt aFrom,TInt aScrollX,TBool aScrollBackground)
	{
	TRect rect;
	TInt height=iDrawTextLayoutContext.DisplayHeight();

	if (aScrollX==0 && aScrollY==0)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_SCROLLTEXTL, "EFScrollByZero2" );
	    }
	__ASSERT_DEBUG(aScrollX!=0 || aScrollY!=0,FormPanic(EFScrollByZero2));
	if (aFrom!=0 && aScrollX!=0)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTVIEW_SCROLLTEXTL, "EFScrollError" );
	    }
	__ASSERT_DEBUG(aFrom==0 || aScrollX==0,FormPanic(EFScrollError));       //Can only scroll the whole of the ViewRect Horizontally
	if (aScrollX==0)
		rect=iDrawTextLayoutContext.iViewRect;
	else
		rect=iDrawTextLayoutContext.TextArea();

	if (iReducedDrawingAreaRect.Height())
		{
		rect.Intersection(iReducedDrawingAreaRect);
		}

	if (iReducedDrawingAreaRect.Height())
		rect.Intersection(iReducedDrawingAreaRect);

	if (ExtendedHighlightExists())
		{
		AdjustRectForScrolling(rect,aScrollY,aScrollX);
		}
	
	ScrollRect(rect,aScrollY,aFrom,aScrollX,aScrollBackground);
	if (aScrollX!=0 && aScrollY!=0)
		{
		rect=iDrawTextLayoutContext.TotalMargin();
		ScrollRect(rect,aScrollY,0,0,aScrollBackground);
		}

	if (iHeightNotDrawn>0 && aScrollY<0 && height+aScrollY>=aFrom)
		{
		iDisplay.SetRects(RScreenDisplay::EFClipViewRect);
		DisplayLineRangeL(aScrollY+height-iHeightNotDrawn,aScrollY+height);
		}
	}


/** Scroll part of the view rect.
@param aScrollY
	Number of pixels downwards to scroll
@param aFrom
	First line visible after the scroll.
@param aTo
	Last line visible after the scroll.
*/
void CTextView::ScrollRect(TInt aScrollY,TInt& aFrom,TInt& aTo)
	{
	TRect rect=ViewRect();

	if (ExtendedHighlightExists())
		{
		AdjustRectForScrolling(rect,aScrollY,0);
		}

	TInt top=rect.iTl.iY;

	if (aTo<0 || aTo<=aFrom)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_SCROLLRECT, "EFScrollError" );
	    }
	__ASSERT_DEBUG(aTo>=0 && aTo>aFrom,FormPanic(EFScrollError));
	aFrom-=aScrollY;
	if (aFrom>0)
		{
		rect.iTl.iY+=aFrom;
		if (aFrom>=iDrawTextLayoutContext.DisplayHeight())
			{
			aFrom+=aScrollY;
			aTo=aFrom;
			return;
			}
		}
	aTo+=top-aScrollY;
	if (aTo<rect.iBr.iY)
		{
		rect.iBr.iY=aTo;
		if (aTo<=top)
			{
			aTo-=top-aScrollY;
			aFrom=aTo;
			return;
			}
		}
	iDisplay.Scroll(rect,TPoint(0,aScrollY),FALSE);
	aFrom=rect.iTl.iY+aScrollY-top;
	aTo=rect.iBr.iY+aScrollY-top;
	}

/**  Scroll part of the view rect */
void CTextView::ScrollRect(TRect& aRect,TInt aScrollY,TInt aFrom,TInt aScrollX,TBool aScrollBackground)
	{

	aFrom=aFrom<0?0:aFrom;
	if (aScrollY+aFrom<0)
		aRect.iTl.iY-=aScrollY;
	else
		{
		aRect.iTl.iY+=aFrom;
		if (aScrollY>0)
			aRect.iBr.iY-=aScrollY;
		}
	if (aScrollX<0)
		aRect.iTl.iX-=aScrollX;
	else
		aRect.iBr.iX-=aScrollX;
	iDisplay.SetRects(RScreenDisplay::EFClipViewRect);
	iDisplay.ActivateContext();
	iDisplay.ResetClippingRect();
	iDisplay.Scroll(aRect,TPoint(aScrollX,aScrollY),aScrollBackground);
	iDisplay.DeactivateContext();
	}

/** Scroll the display and redraw the missing lines.*/
void CTextView::ScrollDisplayL()
	{
	iDisplay.SetRects(RScreenDisplay::EFClipViewRect);
	DrawTextL();
	}

/** Sets the visibility of the line and text cursors.

Notes:

A group window must have been provided before a text cursor can be drawn.

Before making the line cursor visible, a line cursor bitmap must have been 
provided (see SetLineCursorBitmap()).

@param aLineCursor Sets whether the line cursor should be displayed. For possible 
values, see the TCursor::TVisibility enumeration. 
@param aTextCursor Sets whether the text cursor should be displayed and if 
so, whether it should flash. For possible values, see the TCursor::TVisibility 
enumeration. */
EXPORT_C void CTextView::SetCursorVisibilityL(TUint aLineCursor,TUint aTextCursor)
	{
	__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate() ||
				   (aLineCursor==TCursor::EFCursorInvisible && aTextCursor==TCursor::EFCursorInvisible),
				   FormPanic(EFFormatOutOfDate));
	NoMemoryCheckL();
	iCursor.SetVisibility((TCursor::TVisibility)aLineCursor,(TCursor::TVisibility)aTextCursor);
	}

/** Sets whether the text and pictures within a selection should be highlighted. 
If necessary, the view is redrawn.

An example of when this function may need to be used is when a control loses 
or gains focus.

@param aSelectionVisible ETrue for highlighted selection. EFalse for unhighlighted 
selection. */
EXPORT_C void CTextView::SetSelectionVisibilityL(TBool aVisible)
	{

	NoMemoryCheckL();
	TBool needInverting=((!aVisible)!=(!(iFlags & EFSelectionVisible))); //Extra '!' needed to cope with aVisible>1
	if (needInverting && !iDrawTextLayoutContext.TextOverrideColor() && (iFlags & EFTextVisible))
		{
	 	TCursorSelection selection;
		__ASSERT_DEBUG(iLayout->__DbgIsFormattingUpToDate(),FormPanic(EFFormatOutOfDate));
		iFlags |= EFSelectionVisible;
		iCursorPos.GetSelection(selection);
		TBool isPictureFrame=iCursorPos.IsPictureFrame();
		iDisplay.SetRects(RScreenDisplay::EFClipTextArea);
		iDisplay.ActivateContext();
		iDisplay.ResetClippingRect();
		if (selection.Length()>0)
			{
			if (isPictureFrame)
				{
				iCursorPos.DontDrawOldPictureFrame();
				RedrawPictureFrameRectL(selection.LowerPos());
				DrawCursor(TCursor::EFTextCursor);
				}
			else
				{
				CTextLayout::TRangeChange select(selection.iAnchorPos, selection.iCursorPos,
					aVisible? CTextLayout::TRangeChange::ESet : CTextLayout::TRangeChange::EClear);
				iDisplay.SetRects(RScreenDisplay::EFClipExtendedTextArea);
				iDisplay.ResetClippingRect();
				HighlightUsingExtensions(select,select);
				}
			}
		iDisplay.DeactivateContext();
		}
	if (aVisible)
		iFlags |= EFSelectionVisible;
	else
		iFlags &= ~EFSelectionVisible;
	}

/** Sets whether a picture frame should be displayed when the text cursor is located 
at a picture character.

Note:

If a picture frame is currently displayed, and aEnabled is EFalse, a redraw 
takes place, so that the frame is removed.

@param aEnabled True to enable picture frames, false to disable them. */
EXPORT_C void CTextView::EnablePictureFrameL(TBool aEnabled)
	{

	NoMemoryCheckL();
	if (aEnabled)
		iFlags |= EFPictureFrameEnabled;
	else
		iFlags &= ~EFPictureFrameEnabled;
	if (iCursorPos.IsPictureFrame() && !aEnabled)
		CancelSelectionL();
	}

/** Returns if the selection is currently visible.

In particular, the selection is invisible if the text color has been
overridden. */
EXPORT_C TBool CTextView::SelectionVisible() const
	{
	return ((!iDrawTextLayoutContext.TextOverrideColor())
 		&& (iFlags & EFSelectionVisible) && (iFlags & EFTextVisible));
	}

TPoint CTextView::ViewTopOfLineL(const TTmDocPos& aDocPos,TInt& aYPos,CTextView::TDiscard aDiscardFormat,
								 TDoHorizontalScroll aHorizontalScroll)
	{
	TViewYPosQualifier yPosQualifier;
	yPosQualifier.SetHotSpot(TViewYPosQualifier::EFViewTopOfLine);
	return ViewL(aDocPos,aYPos,yPosQualifier,aDiscardFormat,aHorizontalScroll);
	}

TPoint CTextView::ViewL(const TTmDocPos& aDocPos,TInt& aYPos,TViewYPosQualifier aYPosQualifier,
						CTextView::TDiscard aDiscardFormat,TDoHorizontalScroll aHorizontalScroll)
	{
	CTextLayout::TDiscard discardFormat=CTextLayout::EFViewDontDiscardFormat;
	TInt scrollBy = 0;
	
	if (aDiscardFormat==EFViewDiscardAllFormat || aDiscardFormat==EFViewDiscardAllNoRedraw)
		discardFormat=CTextLayout::EFViewDiscardAllFormat;
	if (IsFormatting())
		{
		if (discardFormat==CTextLayout::EFViewDontDiscardFormat)
			FinishBackgroundFormattingL();
		else
			{
			iWrap->Cancel();
			iLayout->NotifyTerminateBackgroundFormatting();
			}
		}
	TRAPD(err,scrollBy=iLayout->SetViewL(aDocPos,aYPos,aYPosQualifier,discardFormat));
	if (err)
		NoMemoryL(err);
	NotifyReformatL();
	if (aDiscardFormat==EFViewDontDiscardFullRedraw)
		scrollBy=CTextLayout::EFScrollRedrawWholeScreen;
	TInt horizScrollBy=0;
	if (aHorizontalScroll)
		horizScrollBy=CheckHorizontalScroll(aDocPos);
	if (aDiscardFormat!=EFViewDiscardAllNoRedraw)
		{
		if (0 != scrollBy || 0 != horizScrollBy)
			{
			ScrollDisplayL();
			}
		else
			{
			DrawCursor();
			}
		}
	iCursorPos.UpdateLatentPosition();
	return TPoint(horizScrollBy,scrollBy);
	}

/** Moves the view vertically and redraws, so that the top of the specified line 
is located flush with the top of the view rectangle.

Note:

Line numbering is relative to formatted text only and the first formatted 
line is numbered one.

@param aLineNo The number of the line to be displayed at the top of the view 
rectangle.
@return The number of pixels the text was scrolled, may be positive or negative. 
A value of CTextLayout::EFScrollRedrawWholeScreen indicates that the entire 
visible area, at least, was scrolled. */
EXPORT_C TPoint CTextView::SetViewLineAtTopL(TInt aLineNo)
	{
	NoMemoryCheckL();
	TTmDocPos pos(iLayout->FirstCharOnLine(aLineNo), ETrue);
	TInt pixel = 0;
	return ViewTopOfLineL(pos,pixel,CTextView::EFViewDontDiscardFormat,EFNoHorizontalScroll);
	}

/**  Sets the vertical position of the view, so that the line containing
aDocPos is located as close as possible to the vertical window coordinate 
aYPos.

Which part of the line is set to appear at aYPos (top, baseline, or bottom) 
is controlled by aYPosQualifier, which also specifies whether the visible 
area is to be filled and whether the line should be made fully visible if 
possible.

@param aDocPos The document position. Must be valid or a panic occurs.
@param aYPos The y coordinate at which to position the character at aDocPos. 
On return, contains the actual vertical window coordinate at which the document
position has been set.
@param aYPosQualifier Controls which part of the line should appear at aYPos.
@param aDiscardFormat If EFViewDiscardAllFormat or EFViewDiscardAllNoRedraw 
the text is reformatted to include aDocPos, otherwise text is formatted 
only as necessary when bringing new lines into the visible area. 
If EFViewDiscardAllNoRedraw, no redraw takes place.
@return The number of pixels scrolled horizontally and vertically. The 
vertical coordinate may have a value of CTextLayout::EFScrollRedrawWholeScreen.
This indicates that the entire visible area, at least, was scrolled, and 
so a full redraw was needed. */
EXPORT_C TPoint CTextView::SetViewL(TInt aDocPos,TInt& aYPos,TViewYPosQualifier aYPosQualifier,
									TDiscard aDiscardFormat,TDoHorizontalScroll aDoHorizontalScroll)
	{
	if (NoMemoryCheckL())
		aDiscardFormat = CTextView::EFViewDontDiscardFullRedraw;
	if (aDocPos < 0 || aDocPos > iLayout->DocumentLength())
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_SETVIEWL, "EFInvalidDocPos" );
	    }
	__ASSERT_ALWAYS(aDocPos >= 0 && aDocPos <= iLayout->DocumentLength(),FormPanic(EFInvalidDocPos));
	aYPos -= TopViewRect();
	TTmDocPos pos(aDocPos,
		aDocPos == iLayout->DocumentLength()? EFalse : ETrue);
	TPoint scroll = ViewL(pos,aYPos,aYPosQualifier,aDiscardFormat,aDoHorizontalScroll);
	aYPos += TopViewRect();
	return scroll;
	}

/** Finds the base line height of the cursor pos */
TInt CTextView::CalculateBaseLinePos(TTmDocPos& aDocPos)
	{
 	TInt y=iGood;			//The height to put the cursor at to recovery from OOM

	aDocPos = iCursorPos.TmDocPos();
	if (!iNoMemory)
		{
		TPoint xy(0,0);
		if (!iLayout->PosInBand(aDocPos,xy))
			{
			xy.SetXY(0,0);
			TTmPosInfo2 pos_info;
			iLayout->FindXyPos(xy,pos_info);
			aDocPos = pos_info.iDocPos;
			xy = pos_info.iEdge;
			}
		y=xy.iY;
		}
	return y;
	}

/** Discards and reformats keeping the cursor at the same height on the screen
if possible. Enters OOM state before leaving */
TPoint CTextView::DoHandleGlobalChangeL(TViewYPosQualifier aYPosQualifier,CTextView::TDiscard aDiscard)
	{
	TTmDocPos docPos;
	TInt y = CalculateBaseLinePos(docPos);
	TCursorSelection selection;
	TTmDocPos docLen(iLayout->DocumentLength(),FALSE);
	const TInt visHeight=iDrawTextLayoutContext.iViewRect.Height();
	TRect cursorLine;

	iLayout->GetLineRect(y,cursorLine);
	switch (aYPosQualifier.iHotSpot)
		{
	case TViewYPosQualifier::EFViewTopOfLine:
		y=cursorLine.iTl.iY;
		break;
	case TViewYPosQualifier::EFViewBottomOfLine:
		y=cursorLine.iBr.iY-1;
	default:;
		}
	if (aYPosQualifier.iFullyVisible==TViewYPosQualifier::EFViewDontForceLineFullyVisible)
		{
		if (y<0)
			{
UseBottom:
			y=(cursorLine.iBr.iY>visHeight ? visHeight:cursorLine.iBr.iY)-1;
			aYPosQualifier.iHotSpot=TViewYPosQualifier::EFViewBottomOfLine;
			}
		else if (y>=visHeight)
			{
			if (cursorLine.iTl.iY<0)
				goto UseBottom;		//else UseTop
 			y=cursorLine.iTl.iY;
			aYPosQualifier.iHotSpot=TViewYPosQualifier::EFViewTopOfLine;
			}
		}
	if (docPos > docLen)
		docPos = docLen;
	iCursorPos.GetSelection(selection);	
	if (selection.LowerPos() > docLen.iPos)
		{
		iCursorPos.CancelHighlight();
		iCursorPos.SetDocPos(docLen);
		}
	else if (selection.HigherPos() > docLen.iPos)
		{
		iCursorPos.CancelHighlight();
		iCursorPos.SetDocPos(docLen);
		}
	return ViewL(docPos,y,aYPosQualifier,aDiscard);
	}

/** Reformats and redraws the view after a global change has been made to the
layout. Examples of global layout changes include changing the format mode, the
wrap width, or the visibility of nonprinting characters.

Forces any background formatting to complete.

@param aYPosQualifier Specifies whether the visible area is to be filled and
whether the top line should be made fully visible if possible. */
EXPORT_C void CTextView::HandleGlobalChangeL(TViewYPosQualifier aYPosQualifier)
	{
	CTextView::TDiscard discard=EFViewDiscardAllFormat;

	if (NoMemoryCheckL())
		discard=EFViewDontDiscardFullRedraw;
	DoHandleGlobalChangeL(aYPosQualifier,discard);
	}

/** Reformats the view after a global change has been made to the layout, but
without causing a redraw. Examples of global layout changes include changing
the format mode, the wrap width, or the visibility of nonprinting characters.

Forces any background formatting to complete.

@param aYPosQualifier Specifies whether the visible area is to be filled and
whether the top line should be made fully visible if possible. */
EXPORT_C void CTextView::HandleGlobalChangeNoRedrawL(TViewYPosQualifier aYPosQualifier)
	{
	CTextView::TDiscard discard=EFViewDiscardAllNoRedraw;

	if (NoMemoryCheckL())
		return;
	DoHandleGlobalChangeL(aYPosQualifier,discard);
	}

/** Reformats and redraws to reflect the addition of one or more complete
paragraphs at the end of the text. For instance, it may be used where
information is being found by an active object and appended to a document.

Note:

This function should not be used to handle the situation where text is added to
the final paragraph. In this case, use HandleInsertDeleteL() or
HandleCharEditL() instead. */
EXPORT_C void CTextView::HandleAdditionalCharactersAtEndL()
	{
	if (IsFormatting())
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_HANDLEADDITIONALCHARACTERSATENDL, "EFBackgroundFormatting" );
	    }
	__ASSERT_DEBUG(!IsFormatting(),FormPanic(EFBackgroundFormatting));
	if (!NoMemoryCheckL())
		{
		TInt from = 0;
		TInt to = 0;
		TRAPD(err,iLayout->HandleAdditionalCharactersAtEndL(from,to));
		if (err)
			NoMemoryL(err);
		NotifyReformatL();
		iDisplay.SetRects(RScreenDisplay::EFClipViewRect);
		DisplayLineRangeL(from,to);
		}
	}

/** Forces completion of background formatting.

This is called automatically by all CTextView functions which depend on the
formatting being up to date. */
EXPORT_C void CTextView::FinishBackgroundFormattingL()
	{
	while (NextLineL()) {}
	}

void CTextView::UpdateHighlightL()
	//
	//Correct the highlight after a cursor movement
	//Enters OOM before leaving
	//
	{
	if (!SelectionVisible())
		return;

	TCursorSelection oldSelection;
	TCursorSelection newSelection;
  	DrawWithPreviousHighlight();
	iCursorPos.GetSelection(oldSelection);
	TBool oldPictureFrame = iCursorPos.IsPictureFrame();
	DrawWithCurrentHighlight();
 	iCursorPos.GetSelection(newSelection);

	if (newSelection.LowerPos()==oldSelection.LowerPos()
		&& iCursorPos.DrawOldPictureFrame() && iCursorPos.DrawNewPictureFrame())
		{
 		iCursorPos.DontDrawOldPictureFrame();
		return;
		}

	iDisplay.SetRects(RScreenDisplay::EFClipTextArea);
	iDisplay.ActivateContext();
	iDisplay.ResetClippingRect();

	if (iCursorPos.DrawOldPictureFrame())
		{
		RedrawPictureFrameRectL(oldSelection.LowerPos()); // DrawWithCurrentHighlight() was called, so it will hide the old picture frame
 		iCursorPos.DontDrawOldPictureFrame();
		}
	if (iCursorPos.DrawHighlight())
		{
		TCursorSelection select;
		iCursorPos.GetOldSelection(select);
		CTextLayout::TRangeChange old;
		if (!oldPictureFrame)
			old.Set(select.iAnchorPos, select.iCursorPos,
				CTextLayout::TRangeChange::EClear);
		iCursorPos.GetSelection(select);
		CTextLayout::TRangeChange now;
		if (!iCursorPos.IsNewPictureFrame())
			now.Set(select.iAnchorPos, select.iCursorPos,
				CTextLayout::TRangeChange::ESet);
		CTextLayout::TRangeChange original;
		original=now;
		now.OptimizeWith(old);
		iDisplay.SetRects(RScreenDisplay::EFClipExtendedTextArea);
		iDisplay.ResetClippingRect();

		if (now.IsJoinedTo(old))
			{
			now.Join(old);
			HighlightUsingExtensions(now,original);
			}
		else
			{
			HighlightUsingExtensions(old,original);
			HighlightUsingExtensions(now,original);
			}
		}
	if (iCursorPos.DrawNewPictureFrame())
		{
 		RedrawPictureFrameRectL(newSelection.LowerPos()); // DrawWithCurrentHighlight() was called, so it will draw the new picture frame
		}
	iDisplay.DeactivateContext();
	}

void CTextView::HighlightUsingExtensions(CTextLayout::TRangeChange aOptimizedRange, CTextLayout::TRangeChange aOriginalRange)
	{
	TInt visPos;
	TInt visLen=iLayout->PosRangeInBand(visPos);
	
	if (visLen<0)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_HIGHLIGHTUSINGEXTENSIONS, "EFNoMemory" );
	    }
	__ASSERT_DEBUG(visLen>=0,FormPanic(EFNoMemory));  //  Shouldn't be in OOM here.
	if (!SelectionVisible())
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTVIEW_HIGHLIGHTUSINGEXTENSIONS, "EFSelectionNotVisible" );
	    }
	__ASSERT_DEBUG(SelectionVisible(),FormPanic(EFSelectionNotVisible));

	if (!aOptimizedRange.Clip(visPos, visPos + visLen))
		return;
	(void)aOriginalRange.Clip(visPos, visPos + visLen);

	// Added to reduce the area drawn to allow layout to work consistently
	TRect drawArea = iDisplay.ClippingRect();
	if (iReducedDrawingAreaRect.Height())
		drawArea.Intersection(iReducedDrawingAreaRect);
	iLayout->HighlightUsingExtensions(aOptimizedRange,aOriginalRange,drawArea, &iDrawTextLayoutContext);
	}


/** Gets called after the cursor moved to a picture, creates the picture frame if it has not been created
	and updates its state.
*/
void CTextView::UpdatePictureFrameL()
	{
	TCursorSelection selection;
	iCursorPos.GetSelection(selection);

	TInt pos = selection.LowerPos();
	TRect rect;
	
	TInt err = FALSE;
	TBool canScaleOrCrop = FALSE;

	if (!iPictureFrame)
		{
		TRAP(err,iPictureFrame=new(ELeave) TFrameOverlay());
		}

	TBool isPicture=EFalse;
	if (!err)
		{
		TRAP(err,isPicture=iLayout->PictureRectangleL(pos,rect,&canScaleOrCrop))
		if(!isPicture)
			__ASSERT_DEBUG(isPicture,FormPanic(EFNoPictureFrame));
		}
	if (err)
		NoMemoryL(err);
	
	iPictureFrame->SetFlags(TFrameOverlay::EFrameOverlayFlagBlobsInternal);
	if (canScaleOrCrop)
		{
		iPictureFrame->SetVisibleBlobWidthInPixels(EFFrameVisibleBlobWidth);
		iPictureFrame->SetActiveBlobWidthInPixels(EFFrameActiveBlobWidth);
		iPictureFrame->ClearFlags(TFrameOverlay::EFrameOverlayFlagShowBorder);
		}
	else
		{
		iPictureFrame->SetVisibleBlobWidthInPixels(0);
		iPictureFrame->SetActiveBlobWidthInPixels(0);
		iPictureFrame->SetFlags(TFrameOverlay::EFrameOverlayFlagShowBorder);
		}
	iDrawTextLayoutContext.TextToWindow(rect);
	iPictureFrame->SetRect(rect);

	}



/** Shows or hides the picture frame at specified location, redrawing the picture frame rect.
It assumes that: 1)iCursorPos.iFlags has been correctly updated to specify the visibility of
				   the picture frame.
				 2)the picture frame object has been created and initialised.
				 3)Gc has already been activated.
Enters OOM before leaving.
*/
void CTextView::RedrawPictureFrameRectL(TInt aPos)
	{
	TRect drawRect;
	if(iLayout->PictureRectangleL(aPos,drawRect))
		{
		iDrawTextLayoutContext.TextToWindow(drawRect);
		drawRect.Intersection(iDrawTextLayoutContext.iViewRect);
		TBool callBeginEndRedraw = iDrawTextLayoutContext.UseWindowGc() && ! iLayout->BeginRedrawCalled();
		if (callBeginEndRedraw)
			{
			iLayout->BeginRedraw(drawRect);
			}
		// since drawRect contains the picture, it calls DrawPictureFrameL()
		TRAPD(err,DrawTextSupportL(drawRect,NULL));
		if (callBeginEndRedraw)
			{
			iLayout->EndRedraw();
			}

		if (err)
			{
			NoMemoryL(err);		//Gc's are deleted
			}
		}
	}
	

 
/*
Draws a focus frame around a picture. 
It assumes that:  1) cursor is sitting on a picture frame (iCursorPos.IsPictureFrame==ETrue)	
				  2) the picture frame object has been created and initialised
 				  3) BeginRedraw() has already been called
					 and the background already has been drawn. So we can use the EDrawModePEN 
					 without problems. 
*/
void CTextView::DrawPictureFrameL(TRect& aClipRect)
	{
	if (!iCursorPos.IsPictureFrame())
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_DRAWPICTUREFRAMEL, "EFNoPictureFrame" );
	    }
	__ASSERT_DEBUG(iCursorPos.IsPictureFrame(),FormPanic(EFNoPictureFrame));
	TCursorSelection selection;
	iCursorPos.GetSelection(selection);
	if(iLayout->PosIsFormatted(selection.LowerPos()))
		{
		aClipRect.Intersection(iDrawTextLayoutContext.TextArea());
		iDrawTextLayoutContext.PrimaryGc()->SetClippingRect(aClipRect);
		TRect rect;
		TInt pos = selection.LowerPos();
		rect = iPictureFrame->RefRect();
		TRect lineRect=iLayout->GetLineRectL(pos, pos);
		iDrawTextLayoutContext.TextToWindow(lineRect);
		lineRect.iTl.iX=iDrawTextLayoutContext.TextArea().iTl.iX;
		lineRect.iBr.iX=iDrawTextLayoutContext.TextArea().iBr.iX;
		lineRect.Intersection(aClipRect);
		if (rect.iTl.iY<iDrawTextLayoutContext.iViewRect.iBr.iY-iHeightNotDrawn)
			{
			iDisplay.DrawPictureFrame(iPictureFrame, lineRect);
			}
		}
	}

/** Enters the OOM state. */
void CTextView::NoMemoryL(TInt aError)
	{
	if (iNoMemory==EFOutOfMemory)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_NOMEMORYL, "EFNoMemory" );
	    }
	__ASSERT_DEBUG(iNoMemory!=EFOutOfMemory,FormPanic(EFNoMemory));
	iLayout->DiscardFormat();
	if (iNoMemory==EFMemoryOK)
		{
		TTmPosInfo2 pos_info;
		if (iLayout->FindDocPos(iCursorPos.TmDocPos(),pos_info))
			iGood = pos_info.iEdge.iY;
		if (iGood < 0 || iGood >= ViewRect().Height())
			iGood = 0;
		}
	iNoMemory=EFOutOfMemory;
	DrawWithCurrentHighlight();
	DestroyWindowServerObjects();
	User::Leave(aError);
	}

void CTextView::DestroyWindowServerObjects()
	{
	iDisplay.Close();
	}

/** Return True if successfully recovered from being out of memory Enters OOM
before leaving */
TBool CTextView::NoMemoryCheckL()
	{
	TBool recovered=iNoMemory;

	if (iNoMemory==EFRecovering)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_NOMEMORYCHECKL, "EFRecoverNoMemory" );
	    }
	__ASSERT_DEBUG(iNoMemory!=EFRecovering,FormPanic(EFRecoverNoMemory));
	if (iNoMemory)
		RecoverNoMemoryL();
	return recovered;
	}

/** Make an attempt to recover from no system memory
Enters OOM before leaving */
void CTextView::RecoverNoMemoryL()
	{
	if (iNoMemory!=EFOutOfMemory)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_RECOVERNOMEMORYL, "EFRecoverNoMemory" );
	    }
	__ASSERT_DEBUG(iNoMemory==EFOutOfMemory,FormPanic(EFRecoverNoMemory));
	iNoMemory=EFRecovering;
	RecreateWindowServerObjectsL();
	TTmDocPos end(iLayout->DocumentLength(), EFalse);
	TTmDocPos pos = iCursorPos.TmDocPos();
	if (pos.iPos > end.iPos)
		iCursorPos.SetDocPos(end);
	ViewL(iCursorPos.TmDocPos(),iGood);
	if (iNoMemory!=EFRecovering)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_CTEXTVIEW_RECOVERNOMEMORYL, "EFNoMemory" );
	    }
	__ASSERT_DEBUG(iNoMemory==EFRecovering,FormPanic(EFNoMemory));
	iNoMemory=EFMemoryOK;
	iGood=0;
	if (iLayout->FormattedHeightInPixels() < iLayout->BandHeight() && iLayout->FirstDocPosFullyInBand()>0)
		{
		TInt scroll=iLayout->BandHeight()-iLayout->YBottomLastFormattedLine();
		ScrollDisplayPixelsL(scroll);
		}
	}

void CTextView::RecreateWindowServerObjectsL()
	{
	TRAPD(err,iDisplay.CreateContextL());
	if (err)
		NoMemoryL(err);
	}

/** Sets the number of pixels by which a horizontal scroll jump will cause the
view to scroll. To carry out a horizontal scroll, use ScrollDisplayL().

@param aScrollJump The number of pixels by which a horizontal scroll jump will
cause the view to scroll. Must be a positive value or a panic occurs. */
EXPORT_C void CTextView::SetHorizontalScrollJump(TInt aScrollJump)
	{
	if (aScrollJump<0)
	    {
	    OstTrace0( TRACE_DUMP, CTEXTVIEW_SETHORIZONTALSCROLLJUMP, "EFInvalidJumpValue" );
	    }
	__ASSERT_ALWAYS(aScrollJump>=0,FormPanic(EFInvalidJumpValue));
	__ASSERT_DEBUG(aScrollJump<EFUnreasonablyLargeHorizontalScrollJump
																	,FormPanic(EFInvalidJumpValue));
	iHorizontalScrollJump=aScrollJump;
	}

/** Sets the left text margin width in pixels.

The left text margin width is the distance between the text and the left edge
of the page. Increasing the left text margin width causes text to move
leftwards and decreasing the left text margin width causes text to move
rightwards.

Does not redraw.

@param aLeftMargin The left text margin width in pixels. */
EXPORT_C void CTextView::SetLeftTextMargin(TInt aLeftMargin)
	{
	iDrawTextLayoutContext.iTextStartX=-aLeftMargin;
	}

/** Gets the left text margin width in pixels.

@return The left text margin width in pixels. */
EXPORT_C TInt CTextView::LeftTextMargin() const
	{
	return -iDrawTextLayoutContext.iTextStartX;
	}

/** Sets the pending horizontal text cursor position. This is the horizontal
coordinate to which the text cursor is moved after a scroll up or down by a
line or a page.

@param aLatentX The horizontal coordinate to which the text cursor should be
moved after a line or page scroll. */
EXPORT_C void CTextView::SetLatentXPosition(TInt aLatentX)
	{
	aLatentX-=iDrawTextLayoutContext.TopLeftText().iX;
	iCursorPos.UpdateLatentX(aLatentX);
	}

/** Sets the horizontal extent of the view rectangle to fill with paragraph
fill colour (CParaFormat::iFillColor).

Does not redraw.

@param aFillTextOnly If true, the region filled with paragraph fill colour is
the area within the paragraph's bounding rectangle only (see ParagraphRectL()).
If false, the filled region includes the left text margin, if one has been set.
By default, false. */
EXPORT_C void CTextView::SetParagraphFillTextOnly(TBool aFillTextOnly)
	{
	iDrawTextLayoutContext.SetParagraphFillTextOnly(aFillTextOnly);
	}

/** Sets the text cursor's type and weight and redraws it. If the cursor's
placement is vertical (ECursorVertical), the weight is the width of the cursor.
If the placement is horizontal, the weight is the height of the cursor.

@param aType The text cursor type.
@param aWidth The weight in pixels of the text cursor. Specify zero (the
default) to leave the current weight unchanged. */
EXPORT_C void CTextView::SetCursorWidthTypeL(TTextCursor::EType aType,TInt aWidth/*=0*/)
	{
	iCursor.SetType(aType);
	iCursor.SetWeight(aWidth);
	}

/** Sets the text cursor's placement (its shape and position relative to the
insertion position) and redraws it.

@param aPlacement The text cursor's placement */
EXPORT_C void CTextView::SetCursorPlacement(TTmCursorPlacement aPlacement)
	{
	iCursor.SetPlacement(aPlacement);
	}

/** Sets the weight of the text cursor and redraws it.

If the cursor's placement is vertical (ECursorVertical), the weight is the
width of the cursor. If the placement is horizontal, the weight is the height
of the cursor. Has no effect if the weight value specified is zero or less.

@param aWeight The weight in pixels of the text cursor. */
EXPORT_C void CTextView::SetCursorWeight(TInt aWeight)
	{
	iCursor.SetWeight(aWeight);
	}

 
EXPORT_C void CTextView::SetCursorFlash(TBool aEnabled)
/** Sets the flashing state of the text cursor and redraws it.

@param aEnabled ETrue for a flashing cursor, EFalse for a non-flashing cursor.
*/
	{
	iCursor.SetFlash(aEnabled);
	}

/** Sets the text cursor's colour and redraws it. The cursor is drawn using an
Exclusive OR draw mode which ensures that the cursor's colour is different from
the background colour.

@param aColor The text cursor's colour. */
EXPORT_C void CTextView::SetCursorXorColor(TRgb aColor)
	{
	iCursor.SetXorColor(aColor);
	}

/** Sets the height and ascent of the text cursor to be the same as an adjacent
character and redraws it. By default, the values are set to be based on the
preceding character.

@param aBasedOn EFCharacterBefore to base the height and ascent of the text
cursor on the preceding character. EFCharacterAfter to base them on the
following character. */
EXPORT_C void CTextView::MatchCursorHeightToAdjacentChar(TBeforeAfter /*aBasedOn*/)
	{
	iCursor.MatchCursorHeightToAdjacentChar();
	}

/** Overrides the text colour, so that when redrawn, all text has the colour
specified, rather than the colour which is set in the text object. When text is
in this overridden state, no highlighting is visible. To return the text colour
to its original state, call this function again with an argument of NULL. This
also has the effect that if selection visibility is set, any highlighting will
be made visible again.

Does not do a redraw.

@param aOverrideColor If not NULL, overrides the text colour. */
EXPORT_C void CTextView::SetTextColorOverride(const TRgb* aOverrideColor)
	{
	iDrawTextLayoutContext.SetTextColorOverride(aOverrideColor);
	}

/** Sets the background colour for the view rectangle.

Does not redraw.

@param aColor The background colour. */
EXPORT_C void CTextView::SetBackgroundColor(TRgb aColor)
	{
	iDrawTextLayoutContext.iBackgroundColor=aColor;
	}

/** Gets the rectangle in which the text is displayed.

@return The view rectangle. */
EXPORT_C const TRect& CTextView::ViewRect() const
	{

	return iDrawTextLayoutContext.iViewRect;
	}

/** Gets the number of pixels by which a horizontal scroll jump will cause the 
view to scroll. On construction, the default horizontal scroll jump value 
is set to 20 pixels.

@return The number of pixels by which a horizontal scroll jump will cause 
the view to scroll. */
EXPORT_C TInt CTextView::HorizontalScrollJump() const
	{

	return iHorizontalScrollJump;
	}

/** Starts the Idle Object if not in the middle of drawing para borders. */
void CTextView::StartIdleObject()
	{

	if (!iWrap->IsActive())
		iWrap->Start(TCallBack(CTextView::IdleL,this));
	}

class RDrawTextSupport
	{
public:
	/** Constructor. Ownership of aBitmap is passed.
	*/
	RDrawTextSupport(RScreenDisplay* aScreenDisplay,
		TDrawTextLayoutContext* aLayoutContext,
		CFbsBitmap* aBitmap)
		: iScreenGc(0), iScreenDisplay(aScreenDisplay),
		iLayoutContext(aLayoutContext),
		iDevice(0), iGc(0), iBitmap(aBitmap)
		{
		iLayoutContextDrawMode = aLayoutContext->DrawMode();
		iScreenGc = iScreenDisplay->BitmapContext();
		}
	void ConstructL(CBitmapContext* aOffScreenContext)
		{
		if (aOffScreenContext)
			{
			iScreenDisplay->SetBitmapContext(aOffScreenContext);
			}
		else
			{
			// Create the device and graphics context.
			iDevice = CFbsBitmapDevice::NewL(iBitmap);
			CFbsBitGc* bitGc;
			User::LeaveIfError(iDevice->CreateContext(bitGc));
			iGc = bitGc;
			iScreenDisplay->SetBitmapContext(iGc);

			// Set the twips size of the bitmap to match the screen so that
			// background bitmaps are blitted at the right size.
			iBitmap->SetSizeInTwips(iScreenGc->Device());
			}
		iLayoutContext->SetBitmapGc(iScreenDisplay->BitmapContext());
		}
	void Close()
		{
		iScreenDisplay->SetBitmapContext(iScreenGc);
		iLayoutContext->SetGc(iScreenGc);
		iLayoutContext->SetDrawMode(iLayoutContextDrawMode);
		delete iGc; iGc = NULL;
		delete iDevice; iDevice = NULL;
		delete iBitmap; iBitmap = NULL;
		}
private:
	CBitmapContext* iScreenGc;
	RScreenDisplay* iScreenDisplay;
	TDrawTextLayoutContext* iLayoutContext;
	CFbsBitmapDevice* iDevice;
	CBitmapContext* iGc;
	CFbsBitmap* iBitmap;
	TUint iLayoutContextDrawMode;
	};

/**
Draws the text. Depending on the opaque mode ("iDrawOpaque" value) and graphics context
type, the method calls either DoDrawTextSupportL() or DoDrawTextSupportOpaqueL().
The additional graphics context type check has to be done, because currently only 
CWindowGc graphics context implementation supports opaque drawing.

Does not enter OOM state.
*/
void CTextView::DrawTextSupportL(const TRect& aScreenDrawRect,
								 const TCursorSelection* aHighlight)
    {
	// Anything to do?
	if( aScreenDrawRect.IsEmpty()||!(iFlags & EFTextVisible) )
		return;
	    
	if(iDisplay.Window() && iDrawOpaque
		&& !iOffScreenContext)
		{
		// It is a CWindowGc and opaque drawing mode
		iLayout->SetOpaqueLC();
		DoDrawTextSupportOpaqueL(aScreenDrawRect, aHighlight);
		CleanupStack::PopAndDestroy();//iLayout->SetOpaqueLC() operation
		}
	else
		{
		DoDrawTextSupportL(aScreenDrawRect, aHighlight);
		}
	}


/**
Draws the text, using an offscreen bitmap to avoid flicker if possible.
All text drawing is done by this function except in printing.

Does not enter OOM state.
*/

void CTextView::DoDrawTextSupportL(const TRect& aScreenDrawRect,
								   const TCursorSelection* aHighlight)
	{
	// extend the screen draw rect for any extended highlights
	TRect screenDrawRect(aScreenDrawRect);
	iLayout->HighlightExtensions().ExtendRect(screenDrawRect);
	screenDrawRect.Intersection(iDisplay.ClippingRect());

	
	//	Create an offscreen bitmap. Although Window server bitmaps are quicker to bilt,
	//	but using it need introduce static_cast, which may make code fragile. So we just 
	//	use CFbsBitmaps.
	
	CFbsBitmap* bitmap = NULL;
	int error = 0;
	if ((iFlags & EFFlickerFreeRedraw) && !iOffScreenContext)
		{
		TDisplayMode displayMode = iDisplay.Window()?
			iDisplay.Window()->DisplayMode()
			: iDisplay.BitmapDevice()->DisplayMode();

		// We are going to make the bitmap size the size from (0,0) on
		// the screen to the bottom right of the drawing area.
		// This is because if we only make it exactly the size we need,
		// the DrawBackground code does not know how to align the bitmap
		// in some cases:
		// 1) where the bitmap doesn't scroll with the text
		// 2) where the bitmap is to be stretched in a way that depends
		// on the co-ordinates it is being stretched to.
		TSize bitmapSize(screenDrawRect.iBr.iX, screenDrawRect.iBr.iY);
		bitmap = new CFbsBitmap;
		if (bitmap)
			{
			error = bitmap->Create(bitmapSize, displayMode);
			}
		}

	TBool isDrawingOnABitmap = NULL != bitmap || NULL != iOffScreenContext;

	CBitmapContext* screen_gc = iDisplay.BitmapContext();
	RDrawTextSupport support(&iDisplay, &iDrawTextLayoutContext, bitmap);
	CleanupClosePushL(support);
	if (error)
	    {
	    OstTrace1( TRACE_DUMP, CTEXTVIEW_DODRAWTEXTSUPPORTL, "Leave code=%d", error );
	    User::Leave(error);
	    }

	// If we are flicker-free drawing (and there was enough memory for it) or
	// drawing to an off screen bitmap for some other reason, set up
	// iDrawTextLayoutContext to temporarily point to the appropriate place
	if (isDrawingOnABitmap)
		{
		support.ConstructL(iOffScreenContext);
		}

	// Draw the text.
	iLayout->DrawL(screenDrawRect, &iDrawTextLayoutContext, aHighlight);

	// Draw picture frame if applicable
	TBool pictureFrame = SelectionVisible() && iCursorPos.IsPictureFrame();
	if (pictureFrame)
		{
		DrawPictureFrameL(screenDrawRect);
		}

	if (NULL != bitmap)
		{
		if (NULL == iOffScreenContext)
			{
			iLayout->BeginRedraw(screenDrawRect);
			}	

		// Copy the drawn rectangle from the offscreen bitmap (if present)
		// to the screen.
		screen_gc->BitBlt(screenDrawRect.iTl, bitmap, screenDrawRect);

		if (NULL == iOffScreenContext)
			{
			iLayout->EndRedraw();
			}
		}

	if (pictureFrame)
		{
		DrawCursor(TCursor::EFTextCursor);
		}

	// Puts iDrawTextLayoutContext back to point to the real screen
	CleanupStack::PopAndDestroy(&support);
	}

/*
Draws the text, when opaque drawing mode is "ON" and the used graphics context type is
CWindowGc.
*/
void CTextView::DoDrawTextSupportOpaqueL(const TRect& aScreenDrawRect,
										 const TCursorSelection* aHighlight)
	{
	// extend the screen draw rect for any extended highlights
	TRect screenDrawRect(aScreenDrawRect);
	iLayout->HighlightExtensions().ExtendRect(screenDrawRect);

	iLayout->BeginRedraw(screenDrawRect);
	iLayout->DrawL(screenDrawRect, &iDrawTextLayoutContext, aHighlight);

	// Draw picture frame if applicable
	TBool pictureFrame = SelectionVisible() && iCursorPos.IsPictureFrame();
	if (pictureFrame)
		{
		DrawPictureFrameL(screenDrawRect);
		DrawCursor(TCursor::EFTextCursor);
		}

	iLayout->EndRedraw();
	}

/** Draws text from aFrom to aTo.

Enters OOM state before leaving.
@param aFrom Top screen Y co-ordinate to draw (inclusive of aFrom)
@param aTo Bottom screen Y co-ordinate to draw (exclusive of aTo)
*/
void CTextView::DisplayLineRangeL(TInt aFrom,TInt aTo)
	{
	TCursorSelection* highlight=NULL;
	TCursorSelection selection;

	iCursorPos.GetSelection(selection);
	TBool pictureFrame=iCursorPos.IsPictureFrame();

	TRect drawRect=iDisplay.ClippingRect();

	drawRect.iTl.iY=Max(aFrom+ViewRect().iTl.iY,drawRect.iTl.iY);
	drawRect.iBr.iY=Min(aTo+ViewRect().iTl.iY,drawRect.iBr.iY);

	if (SelectionVisible() && selection.Length()>0 && !pictureFrame)
		highlight=&selection;

	iDisplay.ActivateContext(iDrawTextLayoutContext.PrimaryGc());
	TRAPD(err,DrawTextSupportL(drawRect,highlight));
	if (err)
		NoMemoryL(err);		//Gc's are deleted
	
	iDisplay.DeactivateContext(iDrawTextLayoutContext.PrimaryGc());
	iHeightNotDrawn=0;
	}

EXPORT_C const TRect& CTextView::AlteredViewRect() const
	{
	return iReducedDrawingAreaRect;
	}


EXPORT_C void CTextView::SetCursorExtensions(TInt aFirstExtension, TInt aSecondExtension)
	{
	iCursor.SetExtensions(aFirstExtension,aSecondExtension);
	}

TBool CTextView::ExtendedHighlightExists() const
	{
	ASSERT(iLayout);
	if (SelectionVisible() && iLayout->HighlightExtensions().Extends())
		{
		TCursorSelection selection;
		iCursorPos.GetSelection(selection);
		if (selection.Length()>0)
			return ETrue;
		}
	return EFalse;
	}


// When scrolling, we also want to include any highlight extensions within the scroll area.
void CTextView::AdjustRectForScrolling(TRect &aRect, TInt aScrollY, TInt aScrollX) const
	{
	ASSERT(iLayout);
	const TTmHighlightExtensions& extensions=iLayout->HighlightExtensions();
	ASSERT(extensions.Extends() && SelectionVisible());

	if (aScrollY==0)
		{
		// scrolling horizontally, so we include top and bottom extensions
		if (extensions.TopExtension()>0)
			aRect.iTl.iY-=extensions.TopExtension();
		if (extensions.BottomExtension()>0)
			aRect.iBr.iY+=extensions.BottomExtension();
		}
	if (aScrollX==0)
		{
		// scrolling vertically, so we include left and right extensions
		if (extensions.LeftExtension()>0)
			aRect.iTl.iX-=extensions.LeftExtension();
		if (extensions.RightExtension()>0)
			aRect.iBr.iX+=extensions.RightExtension();
		}
	}



/**
Returns whether the two hints passed in are the opposites of each other.
@param aHint1 First hint.
@param aHint2 Second hint.
@return ETrue if the two hints are the reversals of each other.
@internalComponent
*/
TBool PositioningHintsReversed(TCursorPosition::TPosHint aHint1,
	TCursorPosition::TPosHint aHint2)
	{
	if (aHint1 == TCursorPosition::EInsertStrongL2R
		&& aHint2 == TCursorPosition::EInsertStrongR2L)
		return ETrue;
	if (aHint1 == TCursorPosition::EInsertStrongR2L
		&& aHint2 == TCursorPosition::EInsertStrongL2R)
		return ETrue;
	return EFalse;
	}

/**
This method allows clients of CTextView to provided a cursor positioning hint
(as defined in TCursorPosition::TPosHint) to the Cursor Navigation
Policy object currently in use in this view.

This information can be used by the CNP object to resolve problems such as
visual cursor positioning ambiguity in bidirectional text.
@param aHint
	Cursor position hint provided by client of CTextView.
@post
	The hint is stored for use in subsequent cursor movement.
*/
EXPORT_C void CTextView::SetCursorPositioningHintL(
	TCursorPosition::TPosHint aHint)
	{
	if (!PositioningHintsReversed(aHint, iCursorPos.PositioningHint()))
		{
		iCursorPos.SetPositioningHint(aHint);
		return;
		}
	iCursorPos.SetPositioningHint(aHint);

	const CTmTextLayout& layout = iLayout->TagmaTextLayout();
	TTmPosInfo2 posInfo;
	TTmLineInfo lineInfo;
	if (!layout.FindDocPos(iCursorPos.TmDocPos(), posInfo, lineInfo))
		return;
	TBool preferRightToLeft =
		aHint == TCursorPosition::EInsertStrongR2L?
		ETrue : EFalse;
	if (iContextIsNavigation)
		{
		// Navigation: keep the visual position the same but maybe change the
		// logical position
		TTmPosInfo2 posInfoLeft;
		TTmPosInfo2 posInfoRight;
		if (!layout.FindXyPosWithDisambiguation(posInfo.iEdge,
			posInfoLeft, posInfoRight, lineInfo))
			return;
		TBool posInfoRightGood = preferRightToLeft?
			posInfoRight.iRightToLeft : !posInfoRight.iRightToLeft;
		if (posInfoRightGood)
			posInfoLeft = posInfoRight;
		SetDocPosL(posInfoLeft.iDocPos, EFalse);
		}
	else
		{
		// Editing: keep the logical position the same but maybe change the
		// visual position
		TBool posInfoGood = preferRightToLeft?
			posInfo.iRightToLeft :  !posInfo.iRightToLeft;
		if (posInfoGood)
			return;

		// Cursor is not currently attatched to text of the correct
		// directionality. We shall try attatching to the one in the other
		// direction.
		TTmDocPosSpec spec;
		spec.iPos = posInfo.iDocPos.iPos;
		spec.iType = posInfo.iDocPos.iLeadingEdge?
			TTmDocPosSpec::ETrailing : TTmDocPosSpec::ELeading;
		if (!layout.FindDocPos(spec, posInfo, lineInfo))
			return;
		posInfoGood = preferRightToLeft?
			posInfo.iRightToLeft :  !posInfo.iRightToLeft;
		if (posInfoGood)
			SetDocPosL(posInfo.iDocPos, EFalse);
		iContextIsNavigation = EFalse;
		}
	}

/**
Sets and unsets an opaque flag on the text view's window. 
@param aDrawOpaque If true, opaque drawing mode is switched on. 
If false, normal drawing will be used.
*/
EXPORT_C void CTextView::SetOpaque(TBool aDrawOpaque)
	{
	iDrawOpaque = aDrawOpaque;
   	}

/** 
Stops or allows text to be drawn.  Included to allow users to control visibility
if text is part of an invisible control.

@param aVisible		ETrue to make the text visible, EFalse to make it invisible.
@see CCoeControl::MakeVisible()
*/
EXPORT_C void CTextView::MakeVisible(TBool aVisible)
	{
	if(aVisible)
		{
		iFlags |= EFTextVisible;
		}
	else
		{
		iFlags &= ~EFTextVisible;
		}
	iLayout->MakeVisible(aVisible);
	}
