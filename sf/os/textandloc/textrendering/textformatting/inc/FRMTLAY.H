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


#ifndef __FRMTLAY_H__
#define __FRMTLAY_H__

#include <e32std.h>
#include <e32base.h>
#include <gdi.h>
#include <txtfrmat.h>
#include <txtetext.h>
#include <frmlaydt.h>
#include <frmparam.h>
#include <frmvis.h>
#include <tagma.h>

class CWindowGc;
class MLayDoc;
class TLayDocTextSource;
class TCursorPosition;

class TLayDocTextSource;
class CTmTextLayout;
class TTmHighlightExtensions;
class MTmSource;
class TTmFormatParamBase;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <frmtlay_internal.h>
#endif

/**
This interface class defines an interface for clients of Form to provided 
implementations of customisation interfaces relating to the source text
document as requried by the internals of Form and Tagma. Its use by
Form clients is optional.
Derived objects of this interface are registered with the CTextLayout 
API and are called from the TLayDocTextSource class at present.
@publishedAll
@released
@see MTmInlineTextSource
*/
class MFormCustomInterfaceProvider
    {
    public:
	/**
	Form uses this method to request an interface implmentation for the
	specified interface Uid. See class description for interfaces covered
	by this API.
	@param aInterfaceId
		The unique identifier for the interface wanted by Form.
	@return 
		Ptr to interface instance or 0 if not supported by client
	*/
	virtual TAny* GetExtendedInterface(const TUid& aInterfaceId) = 0;
    };


/** Parameter used to control which part of a line (top, baseline or bottom) 
should be scrolled to a certain vertical position in a view rectangle. Used 
as a parameter in functions like CTextView::HandleGlobalChangeL() and 
CTextLayout::SetViewL(). The selected part of the line is called the hotspot.
@publishedAll
@released
*/
class TViewYPosQualifier
	{
friend class CTextLayout;
friend class CTextView;
public:
	/** Which part of a line (top, baseline or bottom) should appear at a
	vertical pixel position. */
	enum TPartOfLine
		{
		/** The top pixel is placed at the specified vertical point. */
		EFViewTopOfLine = 1,
		/** The baseline is placed at the specified vertical point. */
		EFViewBaseLine = 0,
		/** The bottom pixel is placed at the specified vertical point. */
		EFViewBottomOfLine = 2	
		};

	/** Whether the top line in the view should be fully visible. */
	enum TFullyVisible
		{
		/** Force a partially visible top line to be fully visible. */
		EFViewForceLineFullyVisible = ETrue,
		/** Do not force a partially visible top line to be fully visible. */
		EFViewDontForceLineFullyVisible = EFalse
		};
public:
	inline TViewYPosQualifier();
	IMPORT_C void SetHotSpot(TPartOfLine aHotSpot);
	IMPORT_C void SetFillScreen(TBool aFillScreen = ETrue);
	IMPORT_C void SetMakeLineFullyVisible(TFullyVisible aMakeLineFullyVisible = EFViewForceLineFullyVisible);
private:
	TPartOfLine iHotSpot;
	TBool iFillScreen;
	TFullyVisible iFullyVisible;
	};

/** 
A structure used to return the results of a reformatting operation.
@see CTextLayout::HandleBlockChangeL()
@publishedAll
@released
*/
class TViewRectChanges

	{
public:
	inline TViewRectChanges();

public:
	/** The vertical coordinate of the top of the first line of reformatted 
	text. */
	TInt iFormattedFrom;
	/** The vertical coordinate of the bottom of the last line of reformatted 
	text. */
	TInt iFormattedTo;
	/** The number of pixels by which text above the reformatted or edited 
	block has scrolled (positive values mean the text moved down). */
	TInt iScrollAtTop;
	/** The number of pixels by which text below the reformatted or edited 
	block has scrolled (positive values mean the text moved down). */
	TInt iScrollAtBottom;
	};

/** 
The cursor or cursor selection within a document.

If the cursor and anchor position differ, the selection covers the text from
the lower to the higher position, not including the character after the higher
position. If the selection is changed (by shift plus arrow keys in many UIs)
the cursor position changes and the anchor remains the same. 
@publishedAll
@released
*/
class TCursorSelection

	{
public:
	inline TCursorSelection();
	inline TCursorSelection(TInt aCursorPos,TInt aAnchorPos);
	inline void SetSelection(TInt aCursorPos,TInt aAnchorPos);
	inline TInt LowerPos() const;
	inline TInt HigherPos() const;
	inline TInt Length() const;
public:
	/** The cursor position. */
	TInt iCursorPos;
	/** The anchor position. */
	TInt iAnchorPos;
	};

/** 
Parameters used by functions that draw text.

An object of this class is passed to CTextLayout::DrawL() and to
InvertRangeL(). The draw context includes the view rectangle, the graphics
context, the background colour and the margin widths.

You only need to use this class directly when you are using a CTextLayout
object which is not owned by a CTextView object.
@publishedAll
@released
*/
class TDrawTextLayoutContext

	{
private:
	enum TDrawMode
		{
		EFDrawText=0x001,
		EFDrawGraphics=0x002,
		EFUseClippingRect=0x004,
		EFUseWindowGc=0x008,
		EFUseGcClear=0x020,
		EFUseBackgroundColor=0x040,
		EFUseOverrideTextColor=0x080,
		EFParagraphFillTextOnly=0x100,
		EFAllFlags=0xfff
		};
public:
	IMPORT_C TDrawTextLayoutContext();

	IMPORT_C void SetGc(CGraphicsContext* aGc,CGraphicsContext* aPictureGc=NULL);
	IMPORT_C void SetBitmapGc(CBitmapContext* aGc,CBitmapContext* aPictureGc=NULL);
	IMPORT_C void SetWindowGc(CWindowGc* aGc,CWindowGc* aPictureGc=NULL);
	IMPORT_C void SetDrawToEveryPixel(TBool aDrawToEveryPixel);
	IMPORT_C void SetTextColorOverride(const TRgb *aOverrideColor);
	IMPORT_C void SetDrawTextOnly();
	IMPORT_C void SetDrawGraphicsOnly();
	IMPORT_C void SetDrawTextAndGraphics();
	IMPORT_C void SetClipping(TBool aClipping);
	// deprecated 7.0 
	IMPORT_C void SetParagraphFillTextOnly(TBool aFillTextOnly);

//Enquiry functions
	IMPORT_C const TRgb* TextOverrideColor() const;
	IMPORT_C CGraphicsContext* PrimaryGc() const;
	IMPORT_C CGraphicsContext* PictureGc() const;
	IMPORT_C TBool UseClippingRect() const;
	IMPORT_C TBool UseGcClear() const;
	IMPORT_C TBool DrawText() const;
	IMPORT_C TBool DrawGraphics() const;
	IMPORT_C TBool UseBackgroundColor() const;
	// deprecated 7.0
	IMPORT_C TBool ParagraphFillTextOnly() const;

//Physical dimensions
	IMPORT_C TRect TextArea() const;
	IMPORT_C TInt DisplayHeight() const;
	IMPORT_C TPoint TopLeftTextArea() const;
	IMPORT_C TRect TotalMargin() const;
	IMPORT_C TRect LabelMargin() const;
	IMPORT_C TBool IsLabelMargin() const;
	IMPORT_C TRect GutterMargin() const;
	IMPORT_C TBool IsGutterMargin() const;
	IMPORT_C TPoint TopLeftText() const;
	IMPORT_C void WindowToText(TPoint& aWinPos) const;
	IMPORT_C void WindowToText(TRect& aRect) const;
	IMPORT_C void TextToWindow(TPoint& aTextAreaPos) const;
	IMPORT_C void TextToWindow(TRect& aRect) const;

	TBool UseWindowGc() const;
	void SetDrawMode(TUint aDrawMode);
	TUint DrawMode() const;

public:
	/** The view rectangle (specified in window coordinates). This is used to 
	set the	area in which text can be drawn. Text can only be drawn within the 
	intersection between the text area and the aDrawRect parameter passed to 
	CTextLayout::DrawL() or InvertRangeL(). */
	TRect iViewRect;
	/** The label margin width. By default zero. Must have the same value as 
	the label margin width as set in the text layout object. */
	TInt iLabelMarginWidth;
	/** The gutter margin width (also known as the line cursor margin width). 
	By default	zero. */
	TInt iGutterMarginWidth;
	/** The horizontal offset between window coordinates and text layout 
	coordinates. */
	TInt iTextStartX;
	/** The background colour for the view rectangle. The background colour is 
	used to fill the parts of the view rectangle in which text cannot appear, 
	for example, below the last line of the document and in the label, line 
	cursor and left	text margins. */
	TLogicalRgb iBackgroundColor;
private:
	CGraphicsContext* iGc;
	CGraphicsContext* iPictureGc;
	TLogicalRgb iOverrideTextColor;
	TUint iDrawMode;
	};

/** 
An abstract class which specifies the protocol for customising the way text
and its background are drawn.

Common uses for this are to implement custom highlighting or to draw a 
background bitmap. You must create an object of a class derived from this class 
and call CTextLayout::SetCustomDraw(), passing a pointer to the object. All of 
these functions have default implementations. Your class can override any of the
virtual functions listed below.
@publishedAll
@released
*/
class MFormCustomDraw

	{
	public:
	
	class TParam
	/**
	Parameters used by several custom draw functions
	@publishedAll
	@released
	*/
			{
		public:
		TParam(CGraphicsContext& aGc,MGraphicsDeviceMap& aMap,const TPoint& aTextLayoutTopLeft,const TRect& aDrawRect):
			iGc(aGc), iMap(aMap), iTextLayoutTopLeft(aTextLayoutTopLeft), iDrawRect(aDrawRect) { }

		CGraphicsContext& iGc;
		MGraphicsDeviceMap& iMap;
		const TPoint& iTextLayoutTopLeft;
		const TRect& iDrawRect;
		};

	class TLineInfo
	/** 
	Contains the line metrics.
	@publishedAll
	@released
	*/
			{
		public:
		TLineInfo(const TRect& aOuterRect,const TRect& aInnerRect,TInt aBaseline):
			iOuterRect(aOuterRect), iInnerRect(aInnerRect), iBaseline(aBaseline) { }

		const TRect& iOuterRect;
		const TRect& iInnerRect;
		TInt iBaseline;
		};

	IMPORT_C virtual void DrawBackground(const TParam& aParam,const TRgb& aBackground,TRect& aDrawn) const;
	IMPORT_C virtual void DrawLineGraphics(const TParam& aParam,const TLineInfo& aLineInfo) const;
	IMPORT_C virtual void DrawText(const TParam& aParam,const TLineInfo& aLineInfo,const TCharFormat& aFormat,
								   const TDesC& aText,const TPoint& aTextOrigin,TInt aExtraPixels) const;
	IMPORT_C virtual TRgb SystemColor(TUint aColorIndex,TRgb aDefaultColor) const;

	IMPORT_C virtual void DrawText(const TParam& aParam,const TLineInfo& aLineInfo,const TCharFormat& aFormat,
								   const TDesC& aText,const TInt aStart, const TInt aEnd, const TPoint& aTextOrigin,TInt aExtraPixels) const;
	IMPORT_C virtual void MFormCustomDraw_Reserved_2();
	};


/** 
An interface class that can be derived from to implement custom line breaking.

Custom line breaking allows the Text Views line wrapping algorithm to break
lines in any way. For instance, it may be used to avoid the jagged white space
at the right edge of text windows caused by normal line wrapping on a small
screen.

All of these functions have a default implementation. To change the default
behaviour, a pointer to an object that overrides any or all of the functions
in the interface needs to be passed to the function CTextLayout::SetCustomWrap().
@since 6.2

@see CTextLayout::SetCustomWrap()
@publishedAll
@released
*/
class MFormCustomWrap

	{
public:
	IMPORT_C virtual TUint LineBreakClass(TUint aCode,TUint& aRangeStart,TUint& aRangeEnd) const;
	IMPORT_C virtual TBool LineBreakPossible(TUint aPrevClass,TUint aNextClass,TBool aHaveSpaces) const;
	IMPORT_C virtual TBool GetLineBreakInContext(const TDesC& aText,TInt aMinBreakPos,TInt aMaxBreakPos,
												 TBool aForwards,TInt& aBreakPos) const;
	IMPORT_C virtual TBool IsHangingCharacter(TUint aChar) const;
private:
	IMPORT_C virtual void MFormCustomWrap_Reserved_1();
	IMPORT_C virtual void MFormCustomWrap_Reserved_2();	
	};


/** 
Mixin class used to customize visible appearance of invisible characters 
such as a paragraph mark or a tab.

@see CTextView::SetCustomInvisibleCharacterRemapper()
@publishedAll
@released
*/
class MFormCustomInvisibleCharacterRemapper
	{
public:
	IMPORT_C static TUint DefaultMapping(TUint aChar, const TNonPrintingCharVisibility aNonPrintingCharVisibility, const TLayDocTextSource& aLayDoc);

	/** 
	Allows custom remapping of invisible characters.

	Called by TLayDocTextSource::Map() if it has been created and registered 
	with TLayDocTextSource using CTextLayout::SetInvisibleCharacterRemapper().

	Unless there is a specific reason for doing otherwise it is recommended 
	that where this function has not remapped a given character it should 
	pass it to DefaultMapping() to let it try.

	@param aChar
		Invisible character to be remapped
	@param aDesiredVisibilities
		Current state of flags showing visibility of invisible characters
	@param aLayDoc
		Const ref to the calling CLayDocTextSource
	@return
		The replacement character if remapping has taken place, else return original character
	*/
	virtual TUint Remap(TUint aChar, const TNonPrintingCharVisibility aNonPrintingCharVisibility, const TLayDocTextSource& aLayDoc) = 0;
	};

/** 
Text layout.

CTextLayout is the lowest-level text formatting class in the Text Views API. It
obtains text and formatting attributes via the MLayDoc interface class and
formats the text to a certain width.

It has functions for drawing the text and performing hit-detection that is,
converting x-y coordinates to document positions, and vice versa. It defines
many public functions that are not generally useful, but are called by the
higher-level CTextView class, or exist only for the convenience of closely
associated classes like the printing and pagination systems. These are
identified in the documentation by the text "not generally useful".

When using the CTextLayout class, you must be aware of what functions have
previously been called. For example:

1) Formatting and scrolling functions must not be called if a CTextView object
owns the CTextLayout object, because the CTextView object may be reformatting
the CTextLayout object asynchronously by means of an active object, or may hold
some state information about the CTextLayout object that would be invalidated
by reformatting. These functions are identified in the documentation by the
text "Do not use if a CTextView object owns this CTextLayout object.".

2) Functions that raise out of memory exceptions can leave the object in an
inconsistent state; these functions can be identified as usual by their
trailing L. When this occurs, it is necessary to discard the formatting
information by calling DiscardFormat().

3) Some functions change formatting parameters like the wrap width or band
height, but do not reformat to reflect the change. These functions are
identified in the documentation by the text "Reformat needed". 
@publishedAll
@released
*/
class CTextLayout: public CBase

	{
friend class CTestTextLayout;
public:

	/**Flags used by CTextLayout::SetViewL(). Whether to reformat and redraw.*/
	enum TDiscard
		{
		/** Discard all formatting; redraw. */
		EFViewDiscardAllFormat = TRUE,
		/** Do not discard all formatting; redraw. */
		EFViewDontDiscardFormat = FALSE
		};

	/** Indicates whether blank space should scroll.
	Used by several CTextView and CTextLayout scrolling functions. */
	enum TAllowDisallow
		{
		/** Allow blank space to scroll. */
		EFAllowScrollingBlankSpace = TRUE,
		/** Disallow blank space from scrolling. */
		EFDisallowScrollingBlankSpace = FALSE
		};

	enum									// flags for HandleCharEditL
		{
		/** Insert a character, (not a paragraph delimiter). */
		EFCharacterInsert,
		/** Insert a paragraph delimiter. */
		EFParagraphDelimiter,
		/** Delete single character to the left. */
		EFLeftDelete,
		/** Delete single character to the right. */
		EFRightDelete
		};

	enum
		{
		/** A value greater than any possible display height indicates that the
		entire visible area, at least, was scrolled, and so there is no point
		in blitting text; a full redraw is needed.
		 */
		EFScrollRedrawWholeScreen = CLayoutData::EFLargeNumber,
		/** The maximum line width when wrapping is unset. */
		EFMaximumLineWidth = CLayoutData::EFBodyWidthForNoWrapping,
		};

	enum
		{
		/** Wrapping off; overrides the paragraph format. */
		EFAllParagraphsNotWrapped = TRUE,
		/** Wrapping on, unless CParaFormat::iWrap is false. */
		EFParagraphsWrappedByDefault = FALSE
		};

	/** Amount to format. Used by CTextLayout::SetAmountToFormat(). */
	enum TAmountFormatted
		{
		/** Format the whole document. */
		EFFormatAllText = FALSE,
		/** Format the visible band only. */
		EFFormatBand = TRUE,
		};

	enum TScrollFlags
		{
		EFScrollOnlyToTopsOfLines = 1
		};

	/** Formatting information. */
	enum TCurrentFormat
		{
		 /** Returned by some CTextLayout enquiry functions to indicate that no
		formatting has taken place so that the requested value cannot be
		calculated. */
		EFNoCurrentFormat = -1,
		/** Returned by CTextLayout::ParagraphHeight() when the paragraph is not formatted. */
		EFNotInCurrentFormat = 0
		};
public:
	class TRangeChange
	/** 
	Specifies the range of characters involved when setting or clearing a
	text selection.

	This class is used in the CTextLayout::Highlight() function. The following
	code demonstrates how it should be used to clear an existing highlight and
	set a new one:

	@code 
	CTextLayout::TRangeChange oldHighlight(anchorPos, old_cursorPos,
	CTextLayout::TRangeChange::EClear); // existing highlight
	CTextLayout::TRangeChange newHighlight(anchorPos, new_cursorPos,
	CTextLayout::TRangeChange::ESet); // new one
	newHighlight.OptimizeWith(oldHighlight); // doesn't matter which range is
	the parameter and which is the calling object
	layout.Highlight(oldHighlight,drawRect,context); // doesn't matter in which
	order this and following line occur
	layout.Highlight(newHighlight,drawRect,context);
	@endcode

	@see CTextLayout::Highlight()
	@publishedAll
	@released
	*/
			{
	public:
		/** Enumerates the possible change types. */
		enum TChangeType
			{
			/** The object is being used to set a range. */
			ESet,
			/** The object is being used to clear a range. */
			EClear
			};
		IMPORT_C TRangeChange(TInt aStart, TInt aEnd, TChangeType aChange);
		IMPORT_C TRangeChange();
		IMPORT_C void Set(TInt aStart, TInt aEnd, TChangeType aChange);
		IMPORT_C TChangeType Get(TInt& aStart, TInt& aEnd) const;
		IMPORT_C void OptimizeWith(TRangeChange& aBuddy);
		IMPORT_C TBool NonNull() const;
		IMPORT_C TBool Clip(TInt aMin, TInt aMax);

		TBool IsJoinedTo(const TRangeChange aRange);
		void Join(const TRangeChange aRange);

	private:
		TInt iA;
		TInt iB;
		};

public:

	class TTagmaForwarder: public MTmTextLayoutForwarder
	/**
	A standard inquiry interface for the text formatting engine, built on
	top of a CTextView object.

	To use it, construct a TTagmaForwarder object, then call InitL(), which
	finishes background formatting, then call the MTmTextLayoutForwarder
	functions.
	
	The class should only be used internally by FORM component.
	@internalComponent
	*/
		{
	public:
		inline TTagmaForwarder(const CTextLayout& aLayout);

	private:
		// from MTmTextLayoutForwarder
		inline const CTmTextLayout& TextLayout() const;
		inline void GetOrigin(TPoint& aPoint) const;

		const CTextLayout& iLayout;
		};

	/**
	Accesses text supplied by MTmSource.
	@internalComponent
	*/
	class TUtf32SourceCache
		{
	public:
		TUtf32SourceCache(const MTmSource& aSource);
		TUtf32SourceCache(const CTextLayout& aLayout);
		TText GetUtf16(TInt aIndex);
		TChar GetUtf32(TInt aIndex);
	private:
		const MTmSource* iSource;
		TPtrC16 iCurrentView;
		TInt iCurrentViewIndex;
		};
	friend class TUtf32SourceCache;

	IMPORT_C static CTextLayout *NewL(MLayDoc *aDoc,TInt aWrapWidth);
	IMPORT_C ~CTextLayout();
	IMPORT_C void DiscardFormat();
	IMPORT_C void SetLayDoc(MLayDoc *aDoc);
	IMPORT_C void SetWrapWidth(TInt aWrapWidth);
	IMPORT_C void SetBandHeight(TInt aHeight);
	IMPORT_C TInt BandHeight() const;
	IMPORT_C void SetImageDeviceMap(MGraphicsDeviceMap *aGd);
	IMPORT_C void SetLabelsDeviceMap(MGraphicsDeviceMap *aDeviceMap);
	IMPORT_C void SetAmountToFormat(TAmountFormatted aAmountOfFormat = EFFormatBand);
	IMPORT_C TBool IsFormattingBand() const;
	IMPORT_C void SetFormatMode(CLayoutData::TFormatMode aFormatMode,TInt aWrapWidth,MGraphicsDeviceMap* aFormatDevice);
	IMPORT_C void ForceNoWrapping(TBool aNoWrapping = EFAllParagraphsNotWrapped);
	IMPORT_C TBool IsWrapping() const;
	IMPORT_C void SetTruncating(TBool aOn);
	IMPORT_C TBool Truncating() const;
	IMPORT_C void SetTruncatingEllipsis(TChar aEllipsis);
	IMPORT_C TChar TruncatingEllipsis() const;
	IMPORT_C void SetLabelsMarginWidth(TInt aWidth);
	IMPORT_C void SetNonPrintingCharsVisibility(TNonPrintingCharVisibility aVisibility);
	IMPORT_C TNonPrintingCharVisibility NonPrintingCharsVisibility() const;
	IMPORT_C TBool IsBackgroundFormatting() const;
	IMPORT_C void NotifyTerminateBackgroundFormatting();
	// deprecated 7.0
	IMPORT_C void SetExcludePartialLines(TBool aExcludePartialLines = TRUE);
	// deprecated 7.0
	IMPORT_C TBool ExcludingPartialLines() const;
	IMPORT_C void SetFontHeightIncreaseFactor(TInt aPercentage);
	IMPORT_C TInt FontHeightIncreaseFactor() const;
	IMPORT_C void SetMinimumLineDescent(TInt aPixels);
	IMPORT_C TInt MinimumLineDescent() const;
	IMPORT_C TInt DocumentLength() const;
	IMPORT_C TInt ToParagraphStart(TInt& aDocPos) const;
	IMPORT_C TInt PixelsAboveBand() const;
	IMPORT_C TInt YBottomLastFormattedLine() const;
	IMPORT_C TInt FormattedHeightInPixels() const;
	IMPORT_C TInt PosRangeInBand(TInt& aDocPos) const;
	IMPORT_C TBool PosInBand(const TTmDocPos& aDocPos,TTmLineInfo* aLineInfo = NULL) const;
	IMPORT_C TBool PosInBand(TTmDocPos aDocPos,TPoint& aXyPos) const;
	IMPORT_C TBool PosInBand(TInt aDocPos,TPoint& aXyPos) const;
	IMPORT_C TBool PosIsFormatted(TInt aDocPos) const;
	IMPORT_C TInt FirstCharOnLine(TInt aLineNo) const;
	IMPORT_C TInt FormattedLength() const;
	IMPORT_C TInt FirstFormattedPos() const;
	IMPORT_C TInt NumFormattedLines() const;
	IMPORT_C TInt FirstLineInBand() const;
	IMPORT_C TInt GetLineRect(TInt aYPos,TRect& aLine) const;
	IMPORT_C TInt ParagraphHeight(TInt aDocPos) const;
	IMPORT_C TRect ParagraphRectL(TInt aDocPos) const;
	IMPORT_C TBool CalculateHorizontalExtremesL(TInt& aLeftX,TInt& aRightX,TBool aOnlyVisibleLines,
												TBool aIgnoreWrapCharacters = FALSE) const;
	IMPORT_C void GetCharacterHeightAndAscentL(TInt aDocPos,TInt& aHeight,TInt& aAscent) const;
	IMPORT_C void GetFontHeightAndAscentL(const TFontSpec& aFontSpec,TInt& aHeight,TInt& aAscent) const;
	IMPORT_C TInt XyPosToDocPosL(TPoint &aPos, TUint aFlags = 0) const;
	IMPORT_C TBool DocPosToXyPosL(TInt aDocPos, TPoint& aPos, TUint aFlags = 0) const;
	IMPORT_C TBool FindXyPos(const TPoint& aXyPos,TTmPosInfo2& aPosInfo,TTmLineInfo* aLineInfo = NULL) const;
	IMPORT_C TBool FindDocPos(const TTmDocPosSpec& aDocPos,TTmPosInfo2& aPosInfo,TTmLineInfo* aLineInfo = NULL) const;
	IMPORT_C TRect GetLineRectL(TInt aDocPos1,TInt aDocPos2) const;
	IMPORT_C TBool PictureRectangleL(TInt aDocPos,TRect& aPictureRect,TBool* aCanScaleOrCrop = NULL) const;
	IMPORT_C TBool PictureRectangleL(const TPoint& aXyPos,TRect& aPictureRect,TBool* aCanScaleOrCrop = NULL) const;
	IMPORT_C TInt FirstDocPosFullyInBand() const;
	IMPORT_C void GetMinimumSizeL(TInt aWrapWidth,TSize& aSize);
	IMPORT_C void GetMinimumSizeL(TInt aWrapWidth,TBool aAllowLegalLineBreaksOnly,TSize& aSize);
	IMPORT_C TInt MajorVersion() const;
	IMPORT_C TInt SetViewL(const TTmDocPos& aDocPos,TInt& aYPos,TViewYPosQualifier aYPosQualifier,
						   TDiscard aDiscardFormat = EFViewDontDiscardFormat);
	IMPORT_C TInt SetViewL(TInt aDocPos,TInt& aYPos,TViewYPosQualifier aYPosQualifier,
						   TDiscard aDiscardFormat = EFViewDontDiscardFormat);
	IMPORT_C void FormatBandL();
	IMPORT_C void FormatCharRangeL(TInt aStartDocPos,TInt aEndDocPos);
	void FormatCharRangeL(TInt aStartDocPos,TInt aEndDocPos,TInt aPixelOffset);
	IMPORT_C TBool FormatNextLineL(TInt& aBotPixel);
	IMPORT_C TBool FormatLineL(CParaFormat* aParaFormat,TInt& aDocPos,TInt& aHeight,TBool& aPageBreak);
	IMPORT_C TInt ScrollParagraphsL(TInt& aNumParas,TAllowDisallow aScrollBlankSpace);
	IMPORT_C TInt ScrollLinesL(TInt& aNumLines,TAllowDisallow aScrollBlankSpace = EFDisallowScrollingBlankSpace);
	IMPORT_C TInt ChangeBandTopL(TInt& aPixels,TAllowDisallow aScrollBlankSpace = EFDisallowScrollingBlankSpace);
    IMPORT_C void ChangeBandTopNoLimitBorderL(TInt aPixels);
	IMPORT_C void PageUpL(TInt& aYCursorPos,TInt& aPixelsScrolled);
	IMPORT_C void PageDownL(TInt& aYCursorPos,TInt& aPixelsScrolled);
	IMPORT_C TBool HandleCharEditL(TUint aType,TInt& aCursorPos,TInt& aGood,TInt& aFormattedUpTo,
								   TInt& aFormattedFrom,TInt& aScroll,TBool aFormatChanged);
	IMPORT_C void HandleBlockChangeL(TCursorSelection aSelection,TInt aOldCharsChanged,TViewRectChanges& aViewChanges,
									 TBool aFormatChanged);
	IMPORT_C void HandleAdditionalCharactersAtEndL(TInt& aFirstPixel,TInt& aLastPixel);
	// deprecated 6.1
	IMPORT_C void ReformatVerticalSpaceL();
	IMPORT_C void AdjustVerticalAlignment(CParaFormat::TAlignment aVerticalAlignment);
	IMPORT_C static void DrawBorders(const MGraphicsDeviceMap* aGd,CGraphicsContext& aGc,const TRect& aRect,
									 const TParaBorderArray& aBorder,const TRgb* aBackground = NULL,
									 TRegion* aClipRegion = NULL,const TRect* aDrawRect = NULL);
	IMPORT_C void DrawL(const TRect& aDrawRect,const TDrawTextLayoutContext* aDrawTextLayoutContext,
						const TCursorSelection* aHighlight = NULL);
	IMPORT_C TBool GetNextVisualCursorPos(const TTmDocPosSpec& aDocPos,
		TTmPosInfo2& aPosInfo, TBool aToLeft) const;
	// deprecated 7.0
	IMPORT_C void InvertRangeL(const TCursorSelection& aHighlight,const TRect& aDrawRect,
							   const TDrawTextLayoutContext* aDrawTextLayoutContext);
	IMPORT_C void Highlight(const TRangeChange& aHighlight,const TRect& aDrawRect,
		const TDrawTextLayoutContext* aDrawTextLayoutContext);
	IMPORT_C void SetCustomDraw(const MFormCustomDraw* aCustomDraw);
	IMPORT_C const MFormCustomDraw* CustomDraw() const;
	IMPORT_C void SetCustomWrap(const MFormCustomWrap* aCustomWrap);
	IMPORT_C const MFormCustomWrap* CustomWrap() const;
	/* this function should only used by Symbian internally */
	IMPORT_C void ExtendFormattingToCoverYL(TInt aYPos);
	IMPORT_C void ExtendFormattingToCoverPosL(TInt aDocPos);

	IMPORT_C TInt GetLineNumber(TInt aDocPos);
	IMPORT_C void SetHighlightExtensions(TInt aLeftExtension, TInt aRightExtension, TInt aTopExtension, TInt aBottomExtension);
	void SetExcessHeightRequired(TInt aExcessHeightRequired);
    IMPORT_C void SetInterfaceProvider( MFormCustomInterfaceProvider* aProvider ); 
	inline const CTmTextLayout& TagmaTextLayout() const;
	inline void GetOrigin(TPoint& aPoint) const;


	inline void RestrictScrollToTopsOfLines(TBool aRestrict);


	// Non-exported public functions
	void DrawBackground(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
						const TLogicalRgb& aBackground) const;
	TBool CalculateHorizontalExtremes(TInt& aLeftX,TInt& aRightX,TBool aOnlyVisibleLines) const;
	TBool GetCursor(const TTmDocPos& aDocPos,TTmCursorPlacement aPlacement,
				    TRect& aLineRect,TPoint& aOrigin,TInt& aWidth,TInt& aAscent,TInt& aDescent) const;
	void GetParagraphRect(const TTmDocPos& aDocPos,TRect& aRect) const;
	TInt ScrollDocPosIntoViewL(const TTmDocPos& aDocPos);
	TInt PictureRectangleAndPosL(const TPoint& aXyPos, TRect& aPictureRect,
		TBool* aCanScaleOrCrop = 0) const;
	void HighlightUsingExtensions(const CTextLayout::TRangeChange& aChangeHighlight,const TRangeChange& aFullHighlight,
		const TRect& aDrawRect,	const TDrawTextLayoutContext* aDrawTextLayoutContext);
	inline const TTmHighlightExtensions& HighlightExtensions() const;
	void GetHighlightRemnants(const TRect& aRect, const TDrawTextLayoutContext& aDrawTextLayoutContext, TRect* aRemainderRects) const;
	TInt WrapWidth() const;

	void SetOpaqueLC();
	
	IMPORT_C void SetCustomInvisibleCharacterRemapper(MFormCustomInvisibleCharacterRemapper* aInvisibleCharacterRemapper);
	IMPORT_C MFormCustomInvisibleCharacterRemapper* GetCustomInvisibleCharacterRemapper();
	void SetTextViewCursorPos(TCursorPosition* aPos); // Put in for INC092568

	void SetWindow(RWindow* aWnd);
	void SetReadyToRedraw();
	void BeginRedraw(const TRect& aRect);
	void EndRedraw();
	void SetExternalDraw(const TRect& aRect);
	void ResetExternalDraw();
	TBool BeginRedrawCalled() const;
	IMPORT_C void MakeVisible(TBool aVisible);

#ifdef _DEBUG
	TBool __DbgIsFormattingUpToDate() const;
#endif

	enum TPanicNumber
		{
		EUnimplemented,
		ENoMemory,
		EDrawingBorderError,
		EFormatDeviceNotSet,
		EImageDeviceNotSet,
		EPixelNotInFormattedLine,
		EInvalidDocPos,
		ENoCharRangeToFormat,
		ECharacterNotFormatted,
		EPrintPreviewModeError,
		EBadCharacterEditType,
		EInvalidLineNumber,
		EPosNotFormatted,
		EMustFormatAllText,
		EPageScrollError,
		EInvalidRedraw
		};
	static void Panic(TPanicNumber aNumber);

private:
	IMPORT_C CTextLayout();
 	IMPORT_C void ConstructL(MLayDoc *aDoc,TInt aWrapWidth);
	void InitFormatParam(TTmFormatParamBase& aParam);
	TInt ScrollL(TInt aDy,TAllowDisallow aScrollBlankSpace,TBool aTopNoLimitBorder=EFalse,TBool aBottomNoLimitBorder=EFalse);
	void FormatBandL(TInt aStartDocPos,TInt aEndDocPos);
	void PruneFormatL(TBool aFromStart);
	TInt VisibleHeightInPixels() const;
	TInt BandHeightInPixels() const;
	TInt SuggestCursorPos(TInt aCurrentCursorPos) const;
	TInt SetBandTop();
	TBool AddFormattingAtEndL(TTmFormatParamBase& aFormatParam, TInt& aHeightIncrease, TInt& aParagraphsIncrease);
	
	static void ResetOpaque(void* aThis);


	CTmTextLayout *iText;		// the TAGMA object that contains the text layout
	TInt iDummyForIText[10];	// It is only here to add padding for maintain compatibility 
	TInt iExcessHeightRequired; // The delta required to position the baseline so there is enough 
								// space for the highset glyph in pixels
	RWindow *iWnd;				// the window to be used to draw
	TBool iBeginRedrawCount;
	TRect iRedrawRect;
	TCursorPosition* iTextViewCursorPos; // From and owned by owning CTextView if it exists,
										 // Null otherwise.  Put in for INC092568
	TBool iIsWndInExternalRedraw;		 // If ETrue, then iWnd->EndRedraw() will not be called
										 // from within CTextLayout::EndRedraw() as the window redraw 
										 // has been initiated externally and will be ended externally as well
	TBool iReadyToRedraw;				// If EFalse, disables CTextLayout::BeginRedraw() and 
										// CTextLayout::EndRedraw()
	TInt iDummy[4];		// This dummy variable has been inserted to replace the original size of
						// a variable that had to be moved to avaid a BC break because it had
						// increased in size. Feel free to reuse this space - just reduce the size
						// of this variable by the size of any variable you insert in front of it.
						// Currently it occupies 5 words.
	TInt iBandTop;		// effectively, scroll position: subtract this from CTmTextLayout y position
						// to give CTextLayout y position
	TInt iVisibleHeight;// in pixels if iFormatMode is EFScreenMode or EFWysiwygMode, otherwise twips
	TInt iBandHeight;	// in pixels if iFormatMode is EFScreenMode or EFWysiwygMode, otherwise twips

	TInt iScrollFlags;		// from CTextLayout::TScrollFlags
	TInt iUnformattedStart; // document position of the start of any unformatted text;
							// if < KMaxTInt, background formatting can be used
							// (by calling FormatNextLine) to format the remainder
	TBool iParInvalid;		// if true and background formatting is happening, the remainder of the paragraph
							// containing iUnformattedStart is invalid and must be reformatted;
							// otherwise, formatting stops when line ends match
	TTmHighlightExtensions *iHighlightExtensions;
	TInt iDmmyForIHighlightExtensions[3];	// It is only here to add padding for maintain compatibility 
	TLayDocTextSource* iSource;	// the source of the text
	// before adding any new member variables to the end of this class, please
	// consider whether you can insert them before the iDummy member variable
	// further up, which currently represents wasted space.
	TInt iDmmyForISource[23];	// It is only here to add padding for maintain compatibility 
	};

/**
Cursor position.

The TMovementType enum defined in this class is used to indicate the direction
of a scroll or cursor movement. The remainder of this class does not form part
of the API.
@publishedAll
@released
*/
class TCursorPosition

	{
public:


	/** Direction of cursor movement. */
	enum TMovementType
		{
		/** No cursor movement */
		EFNoMovement,
		/** Single character cursor movement to the left */
		EFLeft,
		/** Single character cursor movement to the right */
		EFRight,
		/** Line up cursor movement */
		EFLineUp,
		/** Line down cursor movement */
		EFLineDown,
		/** Page up cursor movement */
		EFPageUp,
		/** Page down cursor movement */
		EFPageDown,
		/** Cursor movement to line start */
		EFLineBeg,
		/** Cursor movement to line end */
		EFLineEnd
		};

	/** Selection of the left end or right end of a run of text. */
	enum TVisualEnd
		{
		/** Leftmost end. */
		EVisualLeft,
		/** Rightmost end. */
		EVisualRight
		};

	enum TPosHint
		{
		EPosHintUndefined           = 0,
		/** Left to right typing expected. */
		EInsertStrongL2R = 1,
		/** Right to left typing expected. */
		EInsertStrongR2L = 2,

		// Maybe several more.
		//...
		EPosHintLast
		};
		
	inline TCursorPosition();
	inline void SetLayout(CTextLayout *aLayout);
	inline void UpdateLatentX(TInt aX);
	inline void SetToPreviousHighlight();
	inline void SetToCurrentHighlight();
	inline void CancelHighlight();
	inline void SetDocPos(const TTmDocPos& aDocPos);
	void UpdateLatentPosition();
	TInt SetSelectionL(const TCursorSelection& aSelection);
	void SetPendingSelection(const TCursorSelection& aSelection);
	void GetOldSelection(TCursorSelection& aSelection) const;
	void GetSelection(TCursorSelection& aSelection) const;
	TInt SetDocPosL(TBool aDragSelectOn,const TTmDocPos& aDocPos);
	TInt SetXyPosL(TBool aDragSelectOn,TPoint aPos,TBool aAllowPictureFrame);
	TInt MoveL(TBool aDragSelectOn,TMovementType& aMovement,TBool aAllowPictureFrame);
	const TTmDocPos& VisualEndOfRunL(
		const TTmDocPos& aStart, const TTmDocPos& aEnd,
		TVisualEnd aDirection);
	void TextMoveVertically();
	inline void DontDrawOldPictureFrame();

	// inquiry functions
	inline TBool IsSelection() const;
	inline TBool IsSelectionToDraw() const;
	TBool IsPictureFrame() const;
	TBool IsNewPictureFrame() const;
	inline const TTmDocPos& TmDocPos() const;
	inline TInt DocPos() const;
	inline TBool DrawHighlight() const;
	inline TBool DrawOldPictureFrame() const;
	inline TBool DrawNewPictureFrame() const;
	TBool GetCursor(TTmCursorPlacement aPlacement,TPoint& aOrigin,TInt& aWidth,TInt& aAscent,TInt& aDescent) const;
	TPosHint PositioningHint() const { return iPositioningHint; }
	void SetPositioningHint (TPosHint aHint)
		{iPositioningHint = aHint;}

	TTmPosInfo2& ChoosePosition(TTmPosInfo2& aPreferred,
		TTmPosInfo2& aBackup);

private:
	enum
		{
		EFAbove = -1,
		EFInside = 0,
		EFBelow = 1
		};

	// bit values for iFlags
	enum
		{
		EDrawHighlight = 1,
		EDrawOldPictureFrame = 2,
		EDrawNewPictureFrame = 4,
		EReturnPreviousHighlight = 8,
		ESelected = 16
		};

	TInt ViewTopOfLineL(const TTmDocPos& aDocPos,TInt& aYPos);
	void CheckSelection(TBool aSelect);
	void CheckNullSelection();
	inline void UpdateLatentY(TInt aY);
	TInt CheckCursorOnScreenL(TInt& aY);
	TBool LeftRightL(TTmPosInfo2& aPos, TTmLineInfo& aLine, TMovementType& aMove);
	void StartEnd(TTmPosInfo2& aPos, TTmLineInfo& aLine, TMovementType& aMove);
	void UpDownL(TTmPosInfo2& aPos, TTmLineInfo& aLine, TMovementType& aMove);
	TInt PageScrollL(TMovementType& aMove);
	TInt DoSetDocPosL(const TTmDocPos& aDocPos);
	TInt DoSetVisibleDocPosL(const TTmDocPos& aDocPos);
	void DoSetXyPos(TPoint& aXy);
	void SetPictureFrame(const TTmDocPos& aDocPos,TInt aAnchor,const TRect& aPictureRect);
	inline TBool CalculateCursorPos(TPoint& aCurPos);

	TTmDocPos iDocPos;
	TInt iAnchor;
	TInt iOldDocPos;
	TInt iOldAnchor;
	TUint iFlags;
	TInt iLatentX;
	TInt iLatentY;
	CTextLayout* iLayout;
	TPosHint iPositioningHint;
	};

// inline functions
inline const CTmTextLayout& CTextLayout::TagmaTextLayout() const
	{
	return *iText;
	}

inline void CTextLayout::GetOrigin(TPoint& aPoint) const
	{
	aPoint.iX = 0;
	aPoint.iY = -iBandTop;
	}

inline CTextLayout::TTagmaForwarder::TTagmaForwarder(const CTextLayout& aLayout):
	iLayout(aLayout)
	{
	}

inline const CTmTextLayout& CTextLayout::TTagmaForwarder::TextLayout() const
	{
	return iLayout.TagmaTextLayout();
	}

inline void CTextLayout::TTagmaForwarder::GetOrigin(TPoint& aPoint) const
	{
	iLayout.GetOrigin(aPoint);
	}

inline const TTmHighlightExtensions& CTextLayout::HighlightExtensions() const
	{
	return *iHighlightExtensions;
	}

/**
Dangerous function. Makes scroll operations set the top of the screen flush to
the top of a line. In general this might scroll the cursor off the screen.
*/
void CTextLayout::RestrictScrollToTopsOfLines(TBool a)
	{
	if (a)
		iScrollFlags |= EFScrollOnlyToTopsOfLines;
	else
		iScrollFlags &= ~EFScrollOnlyToTopsOfLines;
	}

/**Constructs a fully initialized TViewYPosQualifier object. The hotspot is
initialized to be the baseline of the line, the screen is not filled, and the
top line is not forced to be fully visible. */
inline TViewYPosQualifier::TViewYPosQualifier():
	iHotSpot(EFViewBaseLine),
	iFillScreen(EFalse),
	iFullyVisible(EFViewDontForceLineFullyVisible)
	{
	}


/**Constructs a TViewRectChanges object, initializing its iScrollAtTop and
iScrollAtBottom members to zero. */
inline TViewRectChanges::TViewRectChanges():
	iScrollAtTop(0),
	iScrollAtBottom(0)
	{
	}

/**Constructs the TCursorSelection object initialising the cursor and anchor
positions to zero. */
inline TCursorSelection::TCursorSelection():
	iCursorPos(0),
	iAnchorPos(0)
	{
	}

/** Constructs the TCursorSelection object with a cursor and anchor position.
@param aCursorPos The cursor position.
@param aAnchorPos The anchor position. */
inline TCursorSelection::TCursorSelection(TInt aCursorPos,TInt aAnchorPos):
	iCursorPos(aCursorPos),
	iAnchorPos(aAnchorPos)
	{
	}

/** Sets the cursor and anchor positions for the selection.
@param aCursorPos The new cursor position.
@param aAnchorPos The new anchor position. */
inline void TCursorSelection::SetSelection(TInt aCursorPos,TInt aAnchorPos)
	{
	iCursorPos = aCursorPos;
	iAnchorPos = aAnchorPos;
	}

/** Gets the lesser of the cursor and anchor positions.
@return The lesser of the cursor and anchor positions. */
inline TInt TCursorSelection::LowerPos() const
	{
	return Min(iCursorPos,iAnchorPos);
	}

/** Gets the greater of the cursor and anchor positions.
@return The greater of the cursor and anchor positions. */
inline TInt TCursorSelection::HigherPos() const
	{
	return Max(iCursorPos,iAnchorPos);
	}

/** Returns the number of characters in the selected range.
@return The length of the selection. */
inline TInt TCursorSelection::Length() const
	{
	return Abs(iCursorPos - iAnchorPos);
	}

/** Constructs the TCursorPosition object, initializing its members
	iAnchor, iOldDocPos, iOldAnchor, iFlags, iLatentX, iLatentY to 
	zero, iLayout to NULL, and iPositioningHint to undefined.
*/
inline TCursorPosition::TCursorPosition():
	iAnchor(0),
	iOldDocPos(0),
	iOldAnchor(0),
	iFlags(0),
	iLatentX(0),
	iLatentY(0),
	iLayout(NULL),
	iPositioningHint(EPosHintUndefined)
	{
	}

/** Sets the text layout which TCursorPosition is related.
@param aLayout The text layout.
*/
inline void TCursorPosition::SetLayout(CTextLayout *aLayout)
	{
	iLayout = aLayout;
	}

/** Tests whether there is currently a selected region.
@return True if there is a selected region. False if not. */
inline TBool TCursorPosition::IsSelection() const
	{
	return iFlags & ESelected;
	}

/** Tests whether there is currently a highlighted region being drawn.
@return True if there is a selected region. False if not. */
inline TBool TCursorPosition::IsSelectionToDraw() const
	{
	return iFlags & (EDrawHighlight | EDrawOldPictureFrame | EDrawNewPictureFrame);
	}

/** Sets the flag which directs <code>GetSelection()</code> and 
<code>IsPictureFrame()</code> to operate on the previous highlight
instead of the current highlight.
*/
inline void TCursorPosition::SetToPreviousHighlight()
	{
	iFlags |= EReturnPreviousHighlight;
	}

/** Clears the special flag set in <code>SetToPreviousHighlight()</code>
so that <code>GetSelection()</code> and <code>IsPictureFrame()</code>
will operate on the current highlight.
*/
inline void TCursorPosition::SetToCurrentHighlight()
	{
	iFlags &= ~EReturnPreviousHighlight;
	}

/** Removes the selection, and redraws the affected part of the screen.
*/
inline void TCursorPosition::CancelHighlight()
	{
	iFlags &= ~(ESelected | EDrawHighlight | EDrawOldPictureFrame | EDrawNewPictureFrame);
	}

/** Sets the latent horizontal text cursor position. This is the horizontal
coordinate to which the text cursor will be moved. 
@param aX The horizontal coordinate to which the text cursor should be moved
*/
inline void TCursorPosition::UpdateLatentX(TInt aX)
	{
	iLatentX = aX;
	}

/** Sets the latent vertical text cursor position. This is the vertical 
coordinate to which the text cursor will be moved. 
@param aY The vertical coordinate to which the text cursor should be moved
*/
inline void TCursorPosition::UpdateLatentY(TInt aY)
	{
	iLatentY = aY;
	}

/** Caculate  x-y position of the cursor
If ETrue is returned, places the position of the intersection of 
the character edge with the baseline in aPos
@param aPos On return, stores the position of the intersection of 
the character edge with the baseline
@return ETrue if the document is in the formatted text, otherwise EFalse.
*/
inline TBool TCursorPosition::CalculateCursorPos(TPoint& aPos)
	{
	TTmPosInfo2 pos_info;
	TBool result = iLayout->FindDocPos(iDocPos,pos_info);
	aPos = pos_info.iEdge;
	return result;
	}
	
/** Gets the document position, the structure for holding a raw document 
position that can be converted to or from an x-y position and compared 
ordinally, which cannot be done with the more abstract TTmDocPosSpec class. 
@return the document position
*/
inline const TTmDocPos& TCursorPosition::TmDocPos() const
	{
	return iDocPos;
	}
/** Gets the edge position in the document.
@return the edge position in the document.
*/
inline TInt TCursorPosition::DocPos() const
	{
	return iDocPos.iPos;
	}

/** Sets the document position, the structure for holding a raw document 
position that can be converted to or from an x-y position and compared 
ordinally, which cannot be done with the more abstract TTmDocPosSpec class. 
@param aDocPos the document position
*/
inline void TCursorPosition::SetDocPos(const TTmDocPos& aDocPos)
	{
	iDocPos = aDocPos;
	}

/** Tests whether the highlighted region needs to be drawn.
The function will be called by CTextView::UpdateHighlightL()
to correct the highlight after a cursor movement
@return True if the highlighted region needs to be drawn. False if not. */
inline TBool TCursorPosition::DrawHighlight() const
	{
	return iFlags & EDrawHighlight;
	}

/** Tests whether there is a previous picture frame that needs to be drawn.
The function will be called by CTextView::UpdateHighlightL()
to correct the highlight after a cursor movement
@return True if there is a previous picture frame that needs to be drawn. False if not. */	
inline TBool TCursorPosition::DrawOldPictureFrame() const
	{
	return iFlags & EDrawOldPictureFrame;
	}

/** Tests whether there is a new picture frame that needs to be drawn.
The function will be called by CTextView::UpdateHighlightL()
to correct the highlight after a cursor movement
@return True if there is a new picture frame that needs to be drawn. False if not. */		
inline TBool TCursorPosition::DrawNewPictureFrame() const
	{
	return iFlags & EDrawNewPictureFrame;
	}

/** Sets to not draw the previous picture frame
*/
inline void TCursorPosition::DontDrawOldPictureFrame()
	{
	iFlags &= ~EDrawOldPictureFrame;
	}

#endif
