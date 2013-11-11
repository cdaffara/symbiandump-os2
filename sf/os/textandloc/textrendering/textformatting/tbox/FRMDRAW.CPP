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


#include "FRMTLAY.H"
#include "FRMCONST.H"

/** Default C++ constructor.

This constructs a fully initialized TDrawTextLayoutContext object. Its view
rectangle, iLabelMarginWidth and iGutterMarginWidth and iTextStartX are all
initialized to zero, the background colour is initialized to the default system
background colour, text and graphics are set to be drawn and clipping is set.
The graphics context is initialised to null. */
EXPORT_C TDrawTextLayoutContext::TDrawTextLayoutContext():
	iLabelMarginWidth(0),
	iGutterMarginWidth(0),
	iTextStartX(0),
	iBackgroundColor(TLogicalRgb::ESystemBackgroundColor),
	iGc(NULL),
	iPictureGc(NULL),
	iOverrideTextColor(TLogicalRgb::ESystemForegroundColor),
	iDrawMode(EFDrawText | EFDrawGraphics | EFUseClippingRect)
	{
	}

/** Sets the graphics context for drawing text and pictures.
@param aGc Pointer to the graphics context for drawing text and pictures.
@param aPictureGc This argument is not used - all drawing is done using aGc. */
EXPORT_C void TDrawTextLayoutContext::SetGc(CGraphicsContext* aGc,CGraphicsContext* aPictureGc/*=NULL*/)
	{
	iGc=aGc;
	iPictureGc=aPictureGc;
	iDrawMode&=EFAllFlags-EFUseGcClear;
	iDrawMode&=EFAllFlags-EFUseWindowGc;
	};

EXPORT_C void TDrawTextLayoutContext::SetBitmapGc(CBitmapContext* aGc,CBitmapContext* aPictureGc/*=NULL*/)
	{
	iGc=aGc;
	iPictureGc=aPictureGc;
	iDrawMode|=EFUseGcClear;
	iDrawMode&=EFAllFlags-EFUseWindowGc;
	};

EXPORT_C void TDrawTextLayoutContext::SetWindowGc(CWindowGc* aGc,CWindowGc* aPictureGc/*=NULL*/)
	{
	iGc=aGc;
	iPictureGc=aPictureGc;
	iDrawMode|=EFUseGcClear;
	iDrawMode|=EFUseWindowGc;
	iDrawMode|=EFUseClippingRect;
	};

/** Sets whether the background colour as specified by iBackgroundColor should
be drawn.
@param aDrawToEveryPixel Specify ETrue to draw the background colour, EFalse to
draw no background colour. */
EXPORT_C void TDrawTextLayoutContext::SetDrawToEveryPixel(TBool aDrawToEveryPixel)
	{
	if (aDrawToEveryPixel)
		iDrawMode|=EFUseBackgroundColor;
	else
		iDrawMode&=EFAllFlags-EFUseBackgroundColor;
	}

/** Overrides the existing text colour, so that when redrawn, all text has the
colour specified, rather than the colour which is set in the text object. To
return the text to its original colour, call this function again with an
argument of null.
@param aOverrideColor If not NULL, overrides the text colour. */
EXPORT_C void TDrawTextLayoutContext::SetTextColorOverride(const TRgb *aOverrideColor)
	{
	if (aOverrideColor)
		{
		iDrawMode|=EFUseOverrideTextColor;
		iOverrideTextColor=*aOverrideColor;
		}
	else
		iDrawMode&=EFAllFlags-EFUseOverrideTextColor;
	}

/** This function is obsolete. When a text object is drawn, all parts of it are
drawn. */
EXPORT_C void TDrawTextLayoutContext::SetDrawTextOnly()
	{
	iDrawMode|=EFDrawText;
	iDrawMode&=EFAllFlags-EFDrawGraphics;
	}

/** This function is obsolete. When a text object is drawn, all parts of it are
drawn. */
EXPORT_C void TDrawTextLayoutContext::SetDrawGraphicsOnly()
	{
	iDrawMode|=EFDrawGraphics;
	iDrawMode&=EFAllFlags-EFDrawText;
	}

/** This function is obsolete. When a text object is drawn, all parts of it are
drawn. */
EXPORT_C void TDrawTextLayoutContext::SetDrawTextAndGraphics()
	{
	iDrawMode|=EFDrawGraphics;
	iDrawMode|=EFDrawText;
	}

/** Sets whether drawing should be clipped to the intersection of the draw
rectangle (the aDrawRect parameter passed to CTextLayout::DrawL() or
InvertRangeL()) and the text area.
@param aClipping True to clip drawing to the clip rectangle. False to ignore
the clip rectangle. */
EXPORT_C void TDrawTextLayoutContext::SetClipping(TBool aClipping)
	{
	if (aClipping)
		iDrawMode|=EFUseClippingRect;
	else
		{
		iDrawMode&=EFAllFlags-EFUseClippingRect;
		}
	}
	
/** Sets the horizontal extent of a paragraph to be filled with paragraph fill
colour.
@deprecated 7.0
@param aFillTextOnly If true, the region filled with paragraph fill colour is
the area within the paragraph only. If false, the left text margin, if present,
is also filled. */
EXPORT_C void TDrawTextLayoutContext::SetParagraphFillTextOnly(TBool aFillTextOnly)
	{
	if (aFillTextOnly)
		iDrawMode|=EFParagraphFillTextOnly;
	else
 		iDrawMode&=EFAllFlags-EFParagraphFillTextOnly;
	}

/** If the text colour is overridden, this function gets the text override
colour, as set by SetTextColorOverride().
@return Text override colour. NULL if no text override colour is in use. */
EXPORT_C const TRgb* TDrawTextLayoutContext::TextOverrideColor() const 
	{
	return (iDrawMode&EFUseOverrideTextColor) ? &iOverrideTextColor : NULL;
	}

/** Gets the graphics context for drawing the text and pictures.
@return The graphics context. */
EXPORT_C CGraphicsContext* TDrawTextLayoutContext::PrimaryGc() const
	{
	return iGc;
	}

EXPORT_C CGraphicsContext* TDrawTextLayoutContext::PictureGc() const 
	{
	return iPictureGc;
	}

/** Tests whether clipping is set to the intersection of the draw rectangle,
(the aDrawRect parameter passed to CTextLayout::DrawL() or InvertRangeL()) and
the text area.
@return True if clipping set to clip rectangle, false if not. */
EXPORT_C TBool TDrawTextLayoutContext::UseClippingRect() const 
	{
	return iDrawMode&EFUseClippingRect;
	}

/** Tests if the graphics context for drawing is either CWindowGc
or CBitmapContext.

@return True if the graphics context is either CWindowGc or CBitmapContext.
        False otherwise. */
EXPORT_C TBool TDrawTextLayoutContext::UseGcClear() const
	{
	return iDrawMode&EFUseGcClear;
	}

/** Obsolete function. When a text object is drawn, all parts of it are drawn. */
EXPORT_C TBool TDrawTextLayoutContext::DrawText() const
	{
	return iDrawMode&EFDrawText;
	}

/** Obsolete function. When a text object is drawn, all parts of it are drawn.
*/
EXPORT_C TBool TDrawTextLayoutContext::DrawGraphics() const
	{
	return iDrawMode&EFDrawGraphics;
	}

/** Tests whether the background colour is used, as set by
SetDrawToEveryPixel().
@return True if the background colour is used. False if not. */
EXPORT_C TBool TDrawTextLayoutContext::UseBackgroundColor() const
	{
	return iDrawMode&EFUseBackgroundColor;
	}

/** Tests whether the region filled with paragraph fill colour is the area
within the paragraph only, or also the area within the left text margin.
@deprecated 7.0
@return If true, the region filled with paragraph fill colour is the area
within the paragraph only. If false, the left text margin, if present, is also
filled. */
EXPORT_C TBool TDrawTextLayoutContext::ParagraphFillTextOnly() const
	{
	return iDrawMode&EFParagraphFillTextOnly;
	}

/** Gets the text area. The text area is the view rectangle minus the label and
gutter margins.
@return The text area. */
EXPORT_C TRect TDrawTextLayoutContext::TextArea() const
	{
	TRect area(iViewRect);
	area.iTl.iX+=iLabelMarginWidth+iGutterMarginWidth;
	return area;
	}

/** Gets the display height in pixels. The display height is the same as the
view rectangle height.
@return The display height. */
EXPORT_C TInt TDrawTextLayoutContext::DisplayHeight() const
	{
	return iViewRect.Height();
	}

/** Gets the (window) coordinates of the top left hand corner of the text area.
@return The top left hand corner of the text area. */
EXPORT_C TPoint TDrawTextLayoutContext::TopLeftTextArea() const
	{
	return TextArea().iTl;
	}

/** Gets the area occupied by the label and gutter margins.
@return Rectangle representing the label and gutter margins. */
EXPORT_C TRect TDrawTextLayoutContext::TotalMargin() const
	{
	TRect area(iViewRect);
	area.iBr.iX=area.iTl.iX+iLabelMarginWidth+iGutterMarginWidth;
	return area;
	}

/** Gets the area occupied by the label margin.
@return Rectangle representing the label margin. */
EXPORT_C TRect TDrawTextLayoutContext::LabelMargin() const
	{
	TRect margin(iViewRect);
	margin.iBr.iX=margin.iTl.iX+iLabelMarginWidth;
	return margin;
	}

/** Tests whether a label margin has been set.
@return True if a label margin has been set. False if not. */
EXPORT_C TBool TDrawTextLayoutContext::IsLabelMargin() const
	{
	return iGutterMarginWidth > 0;
	}

/** Gets the area occupied by the gutter margin.
@return Rectangle representing the gutter margin. */
EXPORT_C TRect TDrawTextLayoutContext::GutterMargin() const
	{
	TRect margin(iViewRect);
	margin.iTl.iX+=iLabelMarginWidth;
	margin.iBr.iX=margin.iTl.iX+iGutterMarginWidth;
	return margin;
	}

/** This function is identical to IsLabelMargin().
@return True if a label margin has been set, false if not. */
EXPORT_C TBool TDrawTextLayoutContext::IsGutterMargin() const
	{
	return iGutterMarginWidth > 0; 
	}

/** Gets the window coordinates of the top left hand corner at which text may
be drawn. This point has the same vertical coordinate as the top of the text
area, but its horizontal coordinate is increased by the left text margin width
(iTextStartX). This point is the origin for layout coordinates.

@return The coordinates of the top left hand point at which text can be drawn.
*/
EXPORT_C TPoint TDrawTextLayoutContext::TopLeftText() const
	{
	return TopLeftTextArea()+TPoint(iTextStartX,0);
	}

/** Converts a point in window coordinates to layout coordinates. Window
coordinates have their origin at the top left corner of the view window. Layout
coordinates have their origin at the top left corner of the area within the
view rectangle in which text can appear, (this value is returned by
TopLeftText()).
@param aWinPos Point in window coordinates. On return, set to layout
coordinates. */
EXPORT_C void TDrawTextLayoutContext::WindowToText(TPoint& aWinPos) const
	{
	aWinPos-=TopLeftText();
	}

/** Converts a rectangle in window coordinates to layout coordinates. Window
coordinates have their origin at the top left corner of the view window. Layout
coordinates have their origin at the top left corner of the area within the
view rectangle in which text can appear, (this value is returned by
TopLeftText()).
@param aRect Rectangle in window coordinates. On return, set to layout
coordinates. */
EXPORT_C void TDrawTextLayoutContext::WindowToText(TRect& aRect) const
	{
	aRect.Move(TPoint(0,0)-TopLeftText());
	}

/** Converts a point in layout coordinates to window coordinates.
@param aTextAreaPos A point in layout coordinates. On return, set to window
coordinates. */
EXPORT_C void TDrawTextLayoutContext::TextToWindow(TPoint& aTextAreaPos) const
	{
	aTextAreaPos+=TopLeftText();
	}

/** Converts a rectangle in layout coordinates to window coordinates.
@param aRect Rectangle in layout coordinates. On return, set to window
coordinates. */
EXPORT_C void TDrawTextLayoutContext::TextToWindow(TRect& aRect) const
	{
	aRect.Move(TopLeftText());
	}

/** Tests if a CWindowGc is being used. */
TBool TDrawTextLayoutContext::UseWindowGc() const
	{
	return iDrawMode & EFUseWindowGc;
	}

/** Returns the current drawing mode flags. */
void TDrawTextLayoutContext::SetDrawMode(TUint aDrawMode)
	{
	iDrawMode = aDrawMode;
	}

/** Returns the flags that identify the current drawing mode. */
TUint TDrawTextLayoutContext::DrawMode() const
	{
	return iDrawMode;
	}
