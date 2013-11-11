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
* The main header file for TAGMA, the low-level text formatting engine for EPOC.
* 'Tm' stands for 'TAGMA' and is the standard prefix for TAGMA classes, after
* the C, T, or whatever.
*
*/


#ifndef __TAGMA_H__
#define __TAGMA_H__

#include <e32base.h>
#include <openfont.h>
#include <txtfrmat.h>
#include <txtstyle.h>
#include <linebreak.h>

// forward declarations
class TTmLineInfo;
class CTmTextImp;
class RWindow;
class RWsSession;
class RParagraphStyleInfo;
class CStyleList;
class TTmDocPosSpec;
class TTmDocPos;
class TCursorSelection;
class TTmHighlightExtensions;
class TBidirectionalContext;
class TTmLineInfo;
class CTmTextLayout;

/** 
A character format; all dimensions are in twips. A character format object
stores all text style attributes that can differ from any one character to the
next. Attributes that affect an entire paragraph only are kept in RTmParFormat
objects. 
The class should only be used internally by FORM component.
@internalComponent
*/
class TTmCharFormat
	{
public:
	/** Bit assignments for effects */
	enum
		{
		EBackground = 1,	///< Text background is drawn in the background colour
		ERounded = 2,		///< Text background (if any) has rounded corners
		EUnderline = 4,	///< Text is underlined
		EStrikethrough = 8,	///< Text is struck through with a horizontal line.
		EShadow = 16,	///< Text has a drop shadow.
		/** Bits 24-31 are reserved for styles defined by the custom drawer. */
		EUserDefinedMask = 0xFF000000,
		/** Shift TCharFormat highlight styles left by this to get the
		TTmCharFormat effects portion. */
		EUserDefinedShift = 24,
		/** For custom drawers: draw whatever is wanted for "no matches" in the FEP. */
		ENoMatchesIndicator = 0x4000000,
		/** Reset if the effect in bits 24-30 is defined defined by Symbian,
		set if it is defined externally. */
		ECustomDrawEffectIsExternallyDefined = 0x80000000
		};

	/** Picture alignment */
	enum TPictureAlignment
		{
		EPictureAlignBaseline,
		EPictureAlignTop,
		EPictureAlignBottom,
		EPictureAlignCenter
		};

	IMPORT_C TTmCharFormat();
	IMPORT_C TTmCharFormat(const TDesC& aFontName,TInt aFontHeight);
	IMPORT_C void operator=(const TCharFormat& aFormat);
	inline TTmCharFormat(const TCharFormat& aFormat);
	IMPORT_C void GetTCharFormat(TCharFormat& aFormat) const;
	IMPORT_C TBool operator==(const TTmCharFormat& aFormat) const;
	inline TBool operator!=(const TTmCharFormat& aFormat) const;

	TOpenFontSpec iFontSpec;
	TLogicalRgb iTextColor;
	TLogicalRgb iBackgroundColor;		// used only when EBackground flag is set in iEffects
	TUint iEffects;						// bit flags as defined above;
										// note that effects NEVER change character metrics and so can
										// be ignored when measuring text
	TUint iLanguage;					// language used for proofing
	TPictureAlignment iPictureAlignment;// vertical alignment of embedded objects
	TUint iTag;							// tag for use by URL parsers, etc.; a way of marking a range of
										// text without using any typographic attributes
	};

/** 
The text customization interface. You can customize the colors, word
spacing, line breaking, line height calculation, background drawing, and text
appearance, of a text object by supplying an implementation of the MTmCustom
class, either directly (as in CTmText::CustomizeL, which takes a pointer to
MTmCustom) or indirectly, making use of the fact that MTmSource is derived from
MTmCustom (as in CTmTextLayout::SetTextL, which takes a reference to
MTmSource).
@publishedAll
@released
*/
class MTmCustom 
				: public MLineBreaker
	{
public:
	/**
    TLineHeightParam structure is used in MTmCustom::SetLineHeight() method to set 
    text line height related parameters such as max character height, max ascent and descent,
    height and depth of the tallest pictures (top-aligned, bottom-aligned or centered).
    @see MTmCustom::SetLineHeight()
	@publishedAll
	@released
	*/
	class TLineHeightParam
		{
	public:
		inline TLineHeightParam();
        /** Height of the highest character in the line. */
		TInt iMaxCharHeight;
        /** Depth of the deepest character in the line. */
		TInt iMaxCharDepth;
        /** Height plus depth of the tallest top-aligned picture. */
		TInt iMaxTopPictureHeight;
        /** Height plus depth of the tallest bottom-aligned picture. */
		TInt iMaxBottomPictureHeight;
        /** Height plus depth of the tallest centred picture. */
		TInt iMaxCenterPictureHeight;
        /** Height of the tallest character of any in the fonts in the line. */
		TInt iFontMaxCharHeight;
        /** Depth of the deepest character of any in the fonts in the line. */
		TInt iFontMaxCharDepth;
        /** Maximum ascent of the fonts in the line. */
		TInt iFontMaxAscent;
        /** Maximum descent of the fonts in the line. */
		TInt iFontMaxDescent;
        /** Desired precise or minimum line height. */
		TInt iDesiredLineHeight;
        /** True if the line height must be precise. */
		TBool iExactLineHeight;
		};

	/**	The Unicode line breaking classes; see Unicode Technical Report 14.
	Not a named enumerated type, so that overriding applications can add new
	line breaking classes freely.
	The description of each constant gives the name of the line-breaking
	class, an example and a brief, imprecise description of the default
	behaviour of characters of that class.
	*/
	enum
		{
		/** Opening Punctuation (e.g. '['). Breaking after prohibited. */
		EOpLineBreakClass,
		/** Closing Punctuation (e.g. ']'). Breaking before prohibited. */
		EClLineBreakClass,
		/** Ambiguous Quotes (e.g. '"'). Breaking before and after prohibited. */
		EQuLineBreakClass,
		/** Glue (e.g. Non-breaking space). Breaking before and after prohibited
		unless spaces are present. */
		EGlLineBreakClass,
		/** Non-Starter (e.g. small Japanese kana). Breaking before prohibited
		if no spaces present. */
		ENsLineBreakClass,
		/** Exclamation or Interrogation (e.g. '?'). Like closing punctuation
		except before Postfix or Non-starter. */
		EExLineBreakClass,
		/** Symbol (e.g. '/'. Like Alphabetic, but allows breaking before
		Alphabetic. */
		ESyLineBreakClass,
		/** Numeric Infix Separator (e.g. ','). Forbids breaking after any and before
		Numeric. */
		EIsLineBreakClass,
		/** Numeric Prefix (e.g. '$'). Forbids breaking before Numeric. */
		EPrLineBreakClass,
		/** Numeric Postfix (e.g. '%'). Forbids breaking after Numeric. */
		EPoLineBreakClass,
		/** Numeric (e.g. '1'). */
		ENuLineBreakClass,
		/** Alphabetic (e.g. 'a'). */
		EAlLineBreakClass,
		/** Ideographic (e.g. Japanese Kanji). Generally break before or after */
		EIdLineBreakClass,
		/** Inseparable (e.g. ellipsis). Forbid breaks between Inseparables. */
		EInLineBreakClass,
		/** Hyphen (e.g. '-'). Allows a break after except before Numeric. */
		EHyLineBreakClass,
		/** Break After. Generally allow a break after. Breaking between Break
		Afters not separated by spaces is prohibited. */
		EBaLineBreakClass,
		/** Break Before. Generally allow a break before. Breaking between Break
		Befores not separated by spaces is prohibited. */
		EBbLineBreakClass,
		/** Break Before and After. Generally allow a break before or after.
		Breaking between Break Before and Afters is prohibited, even if spaces
		are present. */
		EB2LineBreakClass,
		/** Zero-Width Space. Allow a break. */
		EZwLineBreakClass,
		/** Combining Mark. Takes on the class of its base class. */
		ECmLineBreakClass,
		/** Mandatory Break. */
		EBkLineBreakClass,
		/** Carriage Return. Break after unless part of a CRLF pair. */
		ECrLineBreakClass,
		/** Line Feed. Break after. */
		ELfLineBreakClass,
		/** Surrogate. Half of a surrogate pair. */
		ESgLineBreakClass,
		/** Contingent Break (e.g. embedded pictures). Uses external
		information */
		ECbLineBreakClass,

		/** Space. Intervening characters of class Space are indicated by
		aHaveSpaces in LineBreakPossible. */
		ESpLineBreakClass, 

		/** Complex Context (e.g. Thai). Runs of Complex Context are passed to
		GetLineBreakInContext. */
		ESaLineBreakClass,

		/** Ambiguous. Characters of ambiguous East Asian width are treated
		as Alphabetic, unless they are resolved as being "Wide", in which case
		they are treated as Ideographic. */
		EAiLineBreakClass,

		/** The Xx class is used when the class is unknown; e.g.; outside the provided context. */
		EXxLineBreakClass,

		/** The number of Unicode line break classes. */
		ELineBreakClasses
		};

	IMPORT_C virtual TRgb SystemColor(TUint aColorIndex,TRgb aDefaultColor) const;
	IMPORT_C virtual TInt Stretch(TUint aChar) const;
	IMPORT_C virtual TUint Map(TUint aChar) const;
	IMPORT_C virtual void SetLineHeight(const TLineHeightParam& aParam,TInt& aAscent,TInt& aDescent) const;
	IMPORT_C virtual void DrawBackground(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
										 const TLogicalRgb& aBackground,TRect& aRectDrawn) const;
	IMPORT_C virtual void DrawLineGraphics(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
										   const TTmLineInfo& aLineInfo) const;
	IMPORT_C virtual void DrawText(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aRect,
								   const TTmLineInfo& aLineInfo,const TTmCharFormat& aFormat,
								   const TDesC& aText,const TPoint& aTextOrigin,TInt aExtraPixels) const;
	IMPORT_C virtual void DrawPicture(CGraphicsContext& aGc,
		const TPoint& aTextLayoutTopLeft, const TRect& aRect,
		MGraphicsDeviceMap& aDevice, const CPicture& aPicture) const;
	IMPORT_C virtual TUint LineBreakClass(TUint aCode,TUint& aRangeStart,TUint& aRangeEnd) const;
	IMPORT_C virtual TBool LineBreakPossible(TUint aPrevClass,TUint aNextClass,TBool aHaveSpaces) const;
	IMPORT_C virtual TBool GetLineBreakInContext(const TDesC& aText,TInt aMinBreakPos,TInt aMaxBreakPos,
												 TBool aForwards,TInt& aBreakPos) const;
	IMPORT_C virtual TBool IsHangingCharacter(TUint aChar) const;

	// non-virtuals
	IMPORT_C void SetPenColor(CGraphicsContext& aGc,TLogicalRgb aColor) const;
	IMPORT_C void SetBrushColor(CGraphicsContext& aGc,TLogicalRgb aColor) const;
	IMPORT_C TRgb SystemColor(TLogicalRgb aColor) const;
	};

/* MTmSource extension interface****************************************************/


const TUid KTmCustomExtensionUid = {0x10285BB5};

/** MTmSource Extension Interface abstract base class to enable further customisation of the source.

MTmSource derivers should also derive from this interface to enable drawing text in context

This interface should be returned by the overridden
MTmSource::GetExtendedInterface function when KUidMTmSourceExtension is
supplied as the UID.

@see MTmSource
@see KUidMTmSourceExtension 
@publishedAll
@released
*/
class MTmCustomExtension
	{
public:
	IMPORT_C virtual TRgb SystemColor(TUint aColorIndex,TRgb aDefaultColor) const;
	IMPORT_C virtual void DrawText(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aRect,
								   const TTmLineInfo& aLineInfo,const TTmCharFormat& aFormat,
								   const TDesC& aText, const TInt aStart, const TInt aEnd,const TPoint& aTextOrigin,TInt aExtraPixels) const;
		// non-virtuals
	IMPORT_C void SetPenColor(CGraphicsContext& aGc,TLogicalRgb aColor) const;
	IMPORT_C void SetBrushColor(CGraphicsContext& aGc,TLogicalRgb aColor) const;
	IMPORT_C TRgb SystemColor(TLogicalRgb aColor) const;
	};

/** 
A structure for returning information about a line. One of these is
returned by all hit-detection functions. Typically, after finding the position
in the document that corresponds to given x-y coordinates, you will want some
more information like the line's bounding rectangle. When you call, for
example, CTmTextLayout::FindXyPos you will receive a TTmLineInfo object that
will tell you this, and much more. 
@publishedAll
@released
*/
class TTmLineInfo

	{
public:
	// bit values for iFlags
	enum
		{
		EParStart = 1,
		EParEnd = 2,
		EParRightToLeft = 4,
 		ELineEndsInForcedLineBreak = 8,
 		EPictureButtsLowerEdge = 16,	/**< @deprecated - no effect, present for compatibility only */
 		EPictureButtsUpperEdge = 32	/**< @deprecated - no effect, present for compatibility only */
		};

	inline TTmLineInfo();
	
	/** Outer enclosing rectangle including margins */
	TRect iOuterRect;	
	
	/** Inner enclosing rectangle: the text only */
	TRect iInnerRect;
	
	/** y coordinate of the baseline */
	TInt iBaseline;	
	
	/** Start document position */
	TInt iStart;
	
	/** End document position */
	TInt iEnd;			
	
	/** Line number */
	TInt iLineNumber;
	
	/** Paragraph number */
	TInt iParNumber;	
	
	/** Line number in the paragraph */
	TInt iLineInPar;
	
	/** y coordinate of the top of the paragraph */
	TInt iParTop;

	/** Start of paragraph, end of paragraph, etc. */
	TUint iFlags;		
	};

/**
A structure to hold a logical document position that can be converted to a raw
document position or an x-y position.

A document position can specify a leading or trailing edge or a text
directionality so that bidirectional hit testing can use both these
distinctions.

The leading edge at position N is the position before character N in logical
order, and the trailing edge is the position after character in logical order.

Specification by directionality works differently. Character N in left-to-right
text is preceded by position N (left-to-right) and followed by position N+1
(left-to-right). Character N in right-to-left text is preceded (in display
order) by position N+1 (right-to-left) and followed by position N
(right-to-left).
@publishedAll
@released
*/
class TTmDocPosSpec

	{
public:
	/**
	The cursor type.
	*/
	enum TType
		{
		/** trailing edge */
		ETrailing,

		/** leading edge */
		ELeading,

		/** left-to-right */
		ELeftToRight,

		/** right-to-left */
		ERightToLeft
		};

	inline TTmDocPosSpec();
	inline TTmDocPosSpec(TInt aPos,TType aType);
	inline TTmDocPosSpec(const TTmDocPos& aRawDocPos);

	/** the edge position in the document; 0 ... document length */
	TInt iPos;
	/** the type as specified above */
	TType iType;	
	};

/** 
A structure for holding a raw document position that can be converted to or
from an x-y position and compared ordinally, which cannot be done with the more
abstract TTmDocPosSpec class. Leading edges are distinguished from trailing
edges so that bidirectional hit testing and cursor positioning can distinguish
between 'after character N', and 'before character N + 1', which may be some
distance apart if N and N + 1 are in runs of opposite directionality. 
@publishedAll
@released
*/
class TTmDocPos

	{
public:
	inline TTmDocPos();
	inline TTmDocPos(TInt aPos,TBool aLeadingEdge);
	IMPORT_C TBool operator==(const TTmDocPos& aPos) const;
	inline TBool operator!=(const TTmDocPos& aPos) const;
	IMPORT_C TBool operator>(const TTmDocPos& aPos) const;
	IMPORT_C TBool operator>=(const TTmDocPos& aPos) const;
	inline TBool operator<(const TTmDocPos& aPos) const;
	inline TBool operator<=(const TTmDocPos& aPos) const;

	/** the edge position in the document; 0 ... document length */
	TInt iPos;
	/** true if the position is a leading edge */
	TBool iLeadingEdge;	
	};

/** 
Holds information about a position in a document. 
@publishedAll
@released
*/	
class TTmPosInfo2

	{
public:
	/** Document position. */
	TTmDocPos iDocPos;
	/** True if the position is attatched to text flowing right-to-left. */
	TBool iRightToLeft;
	/** Intersection of the character edge with the baseline. */
	TPoint iEdge;
	};

/** 
A structure for returning information about a position in a line.
The class should only be used internally by FORM component.
@deprecated 7.0s 
@internalComponent
*/
class TTmPosInfo

	{
public:
	TTmPosInfo() {}
	TTmPosInfo(const TTmPosInfo2& a) : iDocPos(a.iDocPos), iEdge(a.iEdge) {}
	TTmDocPos iDocPos;	// the document position
	TPoint iEdge;			// intersection of the character edge at iDocPos with the baseline
	};


/** 
Cursor placement. Used as an argument to CTextView::SetCursorPlacement().
@see CTextView::SetCursorPlacement()
@publishedAll
@released
*/
enum TTmCursorPlacement
	{
	 /** The text cursor is a vertical line at the insertion position, with its height 
	and depth based on the metrics of the previous character in the paragraph 
	or if none, the next character. */
	ECursorVertical,		
	/** The text cursor is an underline below the character logically after the insertion 
	position. */
	ECursorUnderlineNext,
	/** The text cursor is an underline below the character logically before the insertion 
	position. */
	ECursorUnderlinePrev
	};



/**
A mixin class to make it easy for higher-level classes that own a CTmTextLayout
object to have enquiry functions without it being necessary to implement them
all as forwarding functions. The owner class just implements TextLayout and
overrides GetOrigin if necessary.

Non-const CTmTextLayout functions like Clear are not included because allowing
them to be called on owner classes would probably put the owner class into an
inconsistent state.

The word Layout is prefixed, suffixed or infixed to functions with names that
would usually conflict with owner class names. For example, we have DrawLayout,
not Draw, and GetMinimumLayoutSizeL, not GetMinimumSizeL.

The class should only be used internally by FORM component.
@internalComponent
*/
class MTmTextLayoutForwarder

	{
public:
	IMPORT_C void DrawLayout(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
							 const TLogicalRgb* aDocBackground,TBool aDrawParBackground) const;
	IMPORT_C void DrawBackground(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
								 const TLogicalRgb& aBackground) const;
	IMPORT_C void InvertLayout(CGraphicsContext& aGc,const TPoint& aTopLeft,TInt aStartDocPos,TInt aEndDocPos);
	IMPORT_C TBool FindDocPos(const TTmDocPosSpec& aDocPos,TTmPosInfo2& aPosInfo,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool FindXyPos(const TPoint& aXyPos,TTmPosInfo2& aPosInfo,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool FindXyPosWithDisambiguation(const TPoint& aXyPos,
		TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight,
		TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool DocPosToLine(const TTmDocPosSpec& aDocPos,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool LineNumberToLine(TInt aLineNumber,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool ParNumberToLine(TInt aParNumber,TInt aLineInPar,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool YPosToLine(TInt aYPos,TTmLineInfo& aLineInfo) const;
	IMPORT_C TInt WidthOfWidestLine(TInt aTop = 0,TInt aBottom = KMaxTInt) const;
	IMPORT_C void HorizontalExtremes(TInt &aLeft, TInt &aRight,
		TInt aTopY = 0, TInt aBottomY = KMaxTInt) const;
	IMPORT_C TBool GetNextVisualCursorPos(const TTmDocPosSpec& aDocPos,TTmPosInfo2& aInfo,TBool aToLeft) const;
	IMPORT_C TBool GetNextPosLeftWithDisambiguation(const TTmDocPosSpec& aDocPos,
		TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight) const;
	IMPORT_C TBool GetNextPosRightWithDisambiguation(const TTmDocPosSpec& aDocPos,
		TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight) const;
	IMPORT_C TBool GetCursor(const TTmDocPosSpec& aDocPos,TTmCursorPlacement aPlacement,
							 TTmLineInfo& aLineInfo,TPoint& aOrigin,TInt& aWidth,TInt& aAscent,TInt& aDescent) const;

	IMPORT_C TInt Lines() const;
	IMPORT_C TInt Paragraphs() const;
	IMPORT_C TBool GetDisplayedText(TInt aLineNumber,TDes& aText,TInt& aNeeded) const;
	IMPORT_C void GetMinimumLayoutSizeL(TInt aWrapWidth,TSize& aSize) const;
	IMPORT_C void GetMinimumLayoutSizeL(TInt aWrapWidth,TBool aAllowLegalLineBreaksOnly,TSize& aSize) const;
	IMPORT_C TInt StartChar() const;
	IMPORT_C TInt EndChar() const;
	IMPORT_C TInt LayoutWidth() const;
	IMPORT_C TInt LayoutHeight() const;

	// deprecated functions
	// deprecated 7.0s
	IMPORT_C TBool FindDocPos(const TTmDocPos& aDocPos,TTmPosInfo& aPosInfo,TTmLineInfo& aLineInfo) const;
	// deprecated 7.0s
	IMPORT_C TBool FindXyPos(const TPoint& aXyPos,TTmPosInfo& aPosInfo,TTmLineInfo& aLineInfo) const;
	// deprecated 7.0s
	IMPORT_C TBool DocPosToLine(const TTmDocPos& aDocPos,TTmLineInfo& aLineInfo) const;
	// deprecated 7.0s
	IMPORT_C TBool GetNextVisualCursorPos(const TTmDocPos& aDocPos,TTmPosInfo& aInfo,TBool aToLeft) const;
	// deprecated 7.0s
	IMPORT_C TBool GetCursor(const TTmDocPos& aDocPos,
		TTmCursorPlacement aPlacement, TTmLineInfo& aLineInfo,
		TPoint& aOrigin, TInt& aWidth, TInt& aAscent, TInt& aDescent) const;
private:
	/** Returns a reference to the CTmTextLayoutObject this
	MTmTextLayoutForwarder forwards inquiries to. */
	virtual const CTmTextLayout& TextLayout() const = 0;
	
	/** The origin is subtracted from coordinates passed in and added to those
	passed out. */
	IMPORT_C virtual void GetOrigin(TPoint& aPoint) const;

	void FixUpLineInfo(TTmLineInfo& aInfo,const TPoint* aOrigin = NULL) const;
	};

/** Constructs a TTmLineInfo object, setting all data members to 0. */
TTmLineInfo::TTmLineInfo()
	{
	Mem::FillZ(this,sizeof(*this));
	}

/** Constructs a TTmDocPos object, setting iPos to 0 and iLeadingEdge to false.
This is the lowest legal value for a TTmDocPos object. A trailing edge comes
before a leading edge because it is the trailing edge of the character before
the position, while the leading edge is that of the character after the
position. ('Leading' means the first to be encountered when traversing the
document in logical order). */
TTmDocPos::TTmDocPos():
	iPos(0),
	iLeadingEdge(FALSE)
	{
	}

/** Constructs a TTmDocPos object, setting iPos to aPos and iLeadingEdge to
aLeadingEdge.
*/
TTmDocPos::TTmDocPos(TInt aPos,TBool aLeadingEdge):
	iPos(aPos),
	iLeadingEdge(aLeadingEdge)
	{
	}

/** Constructs a TTmDocPosSpec, setting the position to 0 and the type to
trailing. */
TTmDocPosSpec::TTmDocPosSpec():
	iPos(0),
	iType(ETrailing)
	{
	}
	

/**
The not equal operator.
@return True if both sides have different values.
*/
TBool TTmDocPos::operator!=(const TTmDocPos& aPos) const { return !((*this)==aPos); }

/**
Smaller than operator.
@return
	True if the right side of the operator is further on in the document than
	the left hand side.
*/
TBool TTmDocPos::operator<(const TTmDocPos& aPos) const { return !((*this)>=aPos); }

/**
Smaller than or equal to operator.
@return
	True if the right side of the operator is further on in the document than
	the left hand side or if both sides are identical.
*/
TBool TTmDocPos::operator<=(const TTmDocPos& aPos) const { return !((*this)>aPos); }

/** Constructs a TTmDocPosSpec, setting the position to aPos and the type to
aType.
*/
TTmDocPosSpec::TTmDocPosSpec(TInt aPos,TType aType):
	iPos(aPos),
	iType(aType)
	{
	}

/** Constructs a TTmDocPosSpec from a TTmDocPos. */
TTmDocPosSpec::TTmDocPosSpec(const TTmDocPos& aDocPos):
	iPos(aDocPos.iPos),
	iType(aDocPos.iLeadingEdge ? ELeading : ETrailing)
	{
	}

/** Constructs a TTmCharFormat by converting the information in a TCharFormat. */
TTmCharFormat::TTmCharFormat(const TCharFormat& aFormat)
	{
	*this = aFormat;
	}

/** The inequality operator. Return TRUE if this object and aFormat differ in any way. */
TBool TTmCharFormat::operator!=(const TTmCharFormat& aFormat) const
	{
	return !operator==(aFormat);
	}

/** Constructs a line height parameter structure, setting all the data members
to zero. */
MTmCustom::TLineHeightParam::TLineHeightParam()
	{
	Mem::FillZ(this,sizeof(*this));
	}

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <tagma_internal.h>
#include <tagmalayoutandsource.h>
#endif

#endif // __TAGMA_H__
