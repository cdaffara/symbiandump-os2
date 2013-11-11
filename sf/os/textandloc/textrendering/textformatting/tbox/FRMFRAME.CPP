/*
* Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "FRMFRAME.H"

const TInt KBorderThicknessInPixels=2;

/** This constructs a TFrameOverlay, clearing all flags and initializing both 
blob widths to zero. */
EXPORT_C TFrameOverlay::TFrameOverlay()
	:iFlags(0),iVisibleBlobWidth(0),iActiveBlobWidth(0)
	{
    }

/** Sets the visible and active blob widths to the same value in pixels.
@param aWidth The visible and active blob width in pixels. */
EXPORT_C void TFrameOverlay::SetBlobWidthInPixels(const TInt aWidth)
    {
	SetVisibleBlobWidthInPixels(aWidth);
	SetActiveBlobWidthInPixels(aWidth);
    }

/** Sets the visible blob width in pixels.
@param aWidth The visible blob width in pixels. */
EXPORT_C void TFrameOverlay::SetVisibleBlobWidthInPixels(const TInt aWidth)
    {
	iVisibleBlobWidth=aWidth;
    }

/** Sets the active blob width in pixels. The active blob width should normally
be at least as large as the visible blob width.
@param aWidth The active blob width in pixels. */
EXPORT_C void TFrameOverlay::SetActiveBlobWidthInPixels(const TInt aWidth)
	{
	iActiveBlobWidth=aWidth;
	}

/** Sets the flags which control how the frame is drawn. Adds the flags
specified to the existing flag settings, which are preserved. For a description
of the available flags, see the TFrameOverlayFlags enum.
@param aFlag Flags to add to the existing settings. */
EXPORT_C void TFrameOverlay::SetFlags(TInt aFlag)
    {
	iFlags|=aFlag;
    }

/** Clears the flags specified from the frame's flag settings. For a description 
of the available flags, see the TFrameOverlayFlags enum.
@param aFlag Flags to clear from the existing flag settings. */
EXPORT_C void TFrameOverlay::ClearFlags(TInt aFlag)
    {
	iFlags&=(~aFlag);
    }

 

EXPORT_C void TFrameOverlay::SetRect(const TRect& aRect)
/** Sets the picture frame rectangle.

@param aRect The picture frame rectangle. */
    {
	iRect=aRect;
    }

/** Draws the picture frame and blobs to a graphics context using the frame's
flag settings. If drawn, the frame is represented by a dotted line. The
operation uses a draw mode of CGraphicsContext::EDrawModeNOTSCREEN so that the
colour of each pixel which is drawn over is inverted. The frame's coordinates
are set using SetRect().
@param aGc The graphics context to draw to. */
EXPORT_C void TFrameOverlay::XorDraw(CGraphicsContext& aGc) const
    {
	TRect rect=iRect;
	TInt blobWidth=VisibleBlobWidth();
	if (iFlags&EFrameOverlayFlagBlobsInternal)
		rect.Shrink(blobWidth,blobWidth);
	TPoint tl=rect.iTl;
	TPoint br=rect.iBr;
	aGc.SetDrawMode(CGraphicsContext::EDrawModeNOTSCREEN);
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

	if (iFlags&EFrameOverlayFlagShowBorder)
		{
		rect.Grow(blobWidth,blobWidth);
		DrawDottedRectangle(aGc, rect);
		}

	if (blobWidth > 0)
		{
		TInt width=br.iX-tl.iX;
		TInt height=br.iY-tl.iY;

		aGc.SetPenStyle(CGraphicsContext::ESolidPen);
		if (!(iFlags&EFrameOverlayFlagTopBlobsDimmed) && !(iFlags&EFrameOverlayFlagLeftBlobsDimmed))
			aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc.DrawRect(TRect(tl-TPoint(blobWidth,blobWidth),tl));
		aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

		if (DrawTopAndBottom())
			{
			if (!(iFlags&EFrameOverlayFlagTopBlobsDimmed))
				aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    		aGc.DrawRect(TRect(tl+TPoint((width-blobWidth)/2,-blobWidth),tl+TPoint((width+blobWidth)/2,0)));
			aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
			}

		if (!(iFlags&EFrameOverlayFlagTopBlobsDimmed) && !(iFlags&EFrameOverlayFlagRightBlobsDimmed))
			aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);

		aGc.DrawRect(TRect(TPoint(br.iX,tl.iY-blobWidth),TPoint(br.iX+blobWidth,tl.iY)));
		aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

		if (DrawLeftAndRight())
			{
 			if (!(iFlags&EFrameOverlayFlagRightBlobsDimmed))
				aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
   			aGc.DrawRect(TRect(br-TPoint(0,(height+blobWidth)/2),br+TPoint(blobWidth,-(height-blobWidth)/2)));
			aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
			}

		if (!(iFlags&EFrameOverlayFlagRightBlobsDimmed) && !(iFlags&EFrameOverlayFlagBottomBlobsDimmed))
			aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc.DrawRect(TRect(br,br+TPoint(blobWidth,blobWidth)));
		aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

		if (DrawTopAndBottom())
			{
 			if (!(iFlags&EFrameOverlayFlagBottomBlobsDimmed))
				aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    		aGc.DrawRect(TRect(br-TPoint((width+blobWidth)/2,0),br-TPoint((width-blobWidth)/2,-blobWidth)));
			aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
			}

		if (!(iFlags&EFrameOverlayFlagBottomBlobsDimmed) && !(iFlags&EFrameOverlayFlagLeftBlobsDimmed))
			aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc.DrawRect(TRect(TPoint(tl.iX-blobWidth,br.iY),TPoint(tl.iX,br.iY+blobWidth)));
		aGc.SetBrushStyle(CGraphicsContext::ENullBrush);

		if (DrawLeftAndRight())
			{
  			if (!(iFlags&EFrameOverlayFlagLeftBlobsDimmed))
				aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
   			aGc.DrawRect(TRect(tl+TPoint(-blobWidth,(height-blobWidth)/2),tl+TPoint(0,(height+blobWidth)/2)));
			aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
			}
		}

	aGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
    }

/** Gets the active region in which a pixel position is located.

Note: Adjacent active regions overlap at the corners of the picture frame, so
that a pixel position may be located within more than one active region.

@param aPos A pixel position.
@return The active area(s) within which the position specified is located. Zero
if not located within any active area. For details, see the TEdges enumeration,
described below. */
EXPORT_C TInt TFrameOverlay::XyPosToEdges(const TPoint& aPos) const
    {
	TInt visibleBlobWidth=VisibleBlobWidth();
	TInt marginWidth=ActiveMarginWidth();
	TInt edgesTouched=ENoEdges;
	TRect frameOuterRect=iRect;
	TRect frameInnerRect=iRect;
	frameOuterRect.Grow(marginWidth,marginWidth);
	frameInnerRect.Shrink(marginWidth,marginWidth);
	if (iFlags&EFrameOverlayFlagBlobsInternal)
		frameInnerRect.Shrink(visibleBlobWidth,visibleBlobWidth);
	else
		frameOuterRect.Grow(visibleBlobWidth,visibleBlobWidth);
	TRect rightRect(TPoint(frameInnerRect.iBr.iX,frameOuterRect.iTl.iY),frameOuterRect.iBr);
	TRect leftRect(frameOuterRect.iTl,TPoint(frameInnerRect.iTl.iX,frameOuterRect.iBr.iY));
	if (rightRect.Contains(aPos))
		edgesTouched|=EEdgeRight;
	else if (leftRect.Contains(aPos))
		edgesTouched|=EEdgeLeft;
	TRect topRect(frameOuterRect.iTl,TPoint(frameOuterRect.iBr.iX,frameInnerRect.iTl.iY));
	TRect bottomRect(TPoint(frameOuterRect.iTl.iX,frameInnerRect.iBr.iY),frameOuterRect.iBr);
	if (topRect.Contains(aPos))
		edgesTouched|=EEdgeTop;
	else if (bottomRect.Contains(aPos))
		edgesTouched|=EEdgeBottom;
	return(edgesTouched);
    }


TInt TFrameOverlay::VisibleBlobWidth()	const
	{
	if (iFlags&EFrameOverlayFlagBlobsInternal)
		return Min(Min(iRect.Width(),iRect.Height())/3,(TInt) iVisibleBlobWidth);
	else
		return iVisibleBlobWidth;
	}

TInt TFrameOverlay::ActiveMarginWidth() const
	{
	TInt margin=(iActiveBlobWidth-VisibleBlobWidth())/2;
	if (iFlags&EFrameOverlayFlagBlobsInternal)
		return Min(Min(iRect.Width(),iRect.Height())/3-VisibleBlobWidth(),margin);
	else
		return Min(Min(iRect.Width(),iRect.Height())/3,margin);

	}

TBool TFrameOverlay::DrawLeftAndRight() const
	{
	if (iFlags&EFrameOverlayFlagBlobsInternal)
		return (iRect.Height()>3*iVisibleBlobWidth);
	else
		return (iRect.Height()>iVisibleBlobWidth);
	}

TBool TFrameOverlay::DrawTopAndBottom() const
	{
	if (iFlags&EFrameOverlayFlagBlobsInternal)
		return (iRect.Width()>3*iVisibleBlobWidth);
	else
		return (iRect.Width()>iVisibleBlobWidth);
	}

void TFrameOverlay::DrawDottedRectangle(CGraphicsContext& aGc,const TRect& aRect) const
	{
	TInt dashWidth=KBorderThicknessInPixels; 
	TInt dashLength=KBorderThicknessInPixels;
	TInt horizontalDashes=(aRect.Width()+dashLength-dashWidth)/(2*dashLength);
	TInt verticalDashes=(aRect.Height()+dashLength-dashWidth)/(2*dashLength);
	aGc.SetPenStyle(CGraphicsContext::ENullPen);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	TPoint move(2*dashLength,0);
	TRect drawRect(aRect.iTl,TSize(dashLength,dashWidth));

	{for (TInt ii=0;ii<horizontalDashes;ii++)
		{
 		aGc.DrawRect(drawRect);
 		drawRect.Move(move);
		}
	}

	move.SetXY(0,2*dashLength);
	drawRect.SetRect(TPoint(aRect.iBr.iX-dashWidth,aRect.iTl.iY),TSize(dashWidth,dashLength));

	{for (TInt ii=0;ii<verticalDashes;ii++)
		{
 		aGc.DrawRect(drawRect);
 		drawRect.Move(move);
		}
	}

	move.SetXY(-2*dashLength,0);
	drawRect.SetRect(TPoint(aRect.iBr.iX-dashLength,aRect.iBr.iY-dashWidth),TSize(dashLength,dashWidth));

	{for (TInt ii=0;ii<horizontalDashes;ii++)
		{
 		aGc.DrawRect(drawRect);
 		drawRect.Move(move);
		}
	}

	move.SetXY(0,-2*dashLength);
	drawRect.SetRect(TPoint(aRect.iTl.iX,aRect.iBr.iY-dashLength),TSize(dashWidth,dashLength));

	{for (TInt ii=0;ii<verticalDashes;ii++)
		{
 		aGc.DrawRect(drawRect);
 		drawRect.Move(move);
		}
	}

	}



