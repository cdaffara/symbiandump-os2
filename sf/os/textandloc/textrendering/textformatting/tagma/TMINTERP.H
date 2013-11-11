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
* Header for TAGMA's bytecode interpreter.
*
*/





#ifndef __INTERP_H__
#define __INTERP_H__ 1

#include <gdi.h>
#include <bidi.h>
#include "TAGMA.H"
#include "TMSTD.H"
#include "TmTextDrawExt.h"
#include <shapeinfo.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

// forward declarations:
class RTmBoundingRectInterpreter;

/**
Parameters for the interpreter classes.
@internalComponent
*/
class TTmInterpreterParam
	{
	public:
	TTmInterpreterParam(const CTmTextLayout& aTextLayout);
	explicit TTmInterpreterParam(const CTmCode& aByteCode);

	const CTmCode* iByteCode;	// bytecode to be read
	TInt iCodeStart;			// start position in bytecode
	TInt iCodeEnd;				// end position in bytecode
	TInt iTextStart;			// starting document position
	TInt iWidth;				// width - used for calculating line bounds
	TInt iDrawingInterpFlags;	// flags destined for the RTmDrawingInterpreter object
	};

/**
The bytecode interpreter base class; this has no text source or font supplier
so can only do things that don't require them, like finding a line containing a
certain y coordinate.
@internalComponent
*/
class TTmInterpreter
	{
public:

	enum
		{
		// Major operator codes: high three bits
		EOpLine = 0x00,	// <N> { <F> } <B> <H> <I> <A>
						//				start of line; set the current position to the left end of the baseline
						//				<N> is the number of characters in the line; mandatory.
						//				<F> is flags given in TLineFlags. 0 is default.
						//				<B> is the size in bytes of the line's bytecode (excluding the ELine op and its args)
						//				<H> is the overall height of the line
						//				<I> is the inner bounds of the line, relative to the line's top-left corner
						//				<A>	is the ascent: distance from the top of the line to the baseline
		EOpText = 0x20, // { <P> } { <N> } { <Y> } <X>
						//				draw text and advance <X> pixels; the other arguments are optional
						//				and depend on modifier flags defined below:
						//				If <X> is negative, it means draw the text to the left of the current position.
						//				<P> is the start position relative to the end of the previous text; if <P> is
						//				absent, 0 is assumed
						//				<N> is the number of characters; if <N> is absent, 1 is assumed
						//				<Y> is the offset from the baseline to the baseline of the text; 0 if not present
		EOpInlineText = 0x40, // { <N> } { <Y> } <X> <F> <T1> ... <Tn>
						//				draw text specified as a series of values inline in the bytecode;
						//				as for EOpText, except that <P> is never necessary, and there are extra arguments:
						//				<F> is the character format index
						//				<T1> ... <Tn> are the character codes of the inline text
		EOpSpecialChar = 0x60, // { <P> } { <Y> } <X>  as EOpText except that the text is
						//				a single character that needs special treatment: e.g., a a picture
		EOpRule = 0x80,	// <R> <K>		<R> is the bounding rectangle of the rule relative to the current position
						//				<K> is the colour, as a number that is converted to a TRgb value (e.g., 0=black)
		EOpDoc = 0xA0,	// <N> <B> <R>	embedded document; text formatted by a custom formatter
						//				<N> is the number of characters in the document
						//				<B> is the size in bytes of the document's bytecode
						//				<R> is the rectangle occupied by the band, relative to the current point
						//+				a document is made up of a number of bands - these must be specified
		EOpLabel = 0xC0,// <N> <B> <R> <L>	a paragraph label or other type of label
						//				<N> is the number of characters in the label
						//				<B> is the size in bytes of the label's bytecode
						//				<R> is the rectangle occupied by the label, relative to the document
						//				<L> is the label type as defined by MTmSource::TLabelType
		EOpParam = 0xE0,// <X> <B>	optional parameter following an operator
						//				<X> is the parameter identifier
						//				<B> is the size of the bytecode, not including EOpParam and its arguments


		// Modifiers: low five bits
		EModPos = 1,			// has <P> argument; combines with EOpText and EOpSpecialChar
		EModLineFlags = 1,		// has <F> argument; combines with EOpLine
		EModCount = 2,			// has <N> argument; combines with all except EOpRule, EOpParam and EOpLine
		EModParStart = 2,		// paragraph start; combines with EOpLine
		EModSubscript = 4,		// has <Y> argument; combines with EOpText, EOpInlineText and EOpSpecialChar
		EModBordered = 4,		// has border rules; combines with EOpLine
		EModRightToLeft = 8,	// right to left; combines with EOpText, EOpInlineText and EOpLine
		EModDontMeasure = 16,	// don't measure - used for line termination characters - combines with EOpText
		EModParEnd = 16,		// paragraph end; combines with EOpLine

		// Parameter identifiers
		EParamBdState = 0,		// the bidirectional state at the start of the current line

		// Bit masks
		KMaskOpcode = 0xE0,
		KMaskModifiers = 0x1F
		};
	enum TLineFlags
		{
		ELineFlagLineEndsInForcedLineBreak = 1,
		ELineFlagVisualEndOfLineIsAmbiguous = 2
		};

	TTmInterpreter(const TTmInterpreterParam& aParam,const TPoint* aTopLeft = NULL);
	const TTmLineInfo& LineInfo() const { return iLineInfo; }

	enum
		{
		EMaxInlineChars = 8		// maximum number of characters specified by EOpInlineText
		};

	TInt Lines();
	TInt Paragraphs();
	TBool DocPosToLine(const TTmDocPosSpec& aDocPos);
	TBool LineNumberToLine(TInt aLineNumber);
	TBool ParNumberToLine(TInt aParNumber,TInt aLineInPar);
	TBool YPosToLine(TInt aYPos);
	TInt WidthOfWidestLine(TInt aTop,TInt aBottom);
	void HorizontalExtremes(TInt &aLeft, TInt &aRight,
		TInt aTopY, TInt aBottomY);
	inline TBool PosIsInLine(const TTmDocPos& aDocPos) const;
	void CalculateRedrawRect(TTmInterpreter& aNewCode,TInt aStartChar,TInt aOldLength,TInt aNewLength,TRect& aRedrawRect);
	TBool ParStart() const { return iLineInfo.iFlags & TTmLineInfo::EParStart; }
	TBool ParEnd() const { return iLineInfo.iFlags & TTmLineInfo::EParEnd; }

public:
	TBool Next();
	void Skip(TInt aCodePos,TInt aTextPos);
	void Skip() { Skip(iOpEndCode,iOpEndChar); }
	TUint8 Op() const { return iOp; }
	TUint8 ParamId() const { return iParamId; }
	TBool RightToLeft() const { return (iOp & (EOpText | EOpInlineText)) && (iOpMod & EModRightToLeft); }
	TBool Subscript() const { return (iOp & (EOpText | EOpInlineText)) && (iOpMod & EModSubscript); }
	TBool Bordered() const { return iOp == EOpLine && (iOpMod & EModBordered); }
	TBool DontMeasure() const { return iOp == EOpText && (iOpMod & EModDontMeasure); }
	TInt StartCodePos() const { return iOpStartCode; }
	TInt EndCodePos() const { return iOpEndCode; }
	TInt StartChar() const { return iOpStartChar; }
	TTmDocPos StartDocPos() const { return TTmDocPos(iOpStartChar,iOpStartChar > 0); }
	TInt EndChar() const { return iOpEndChar; }
	TTmDocPos EndDocPos() const { return TTmDocPos(iOpEndChar,iOpEndChar > 0); }
	const TPoint& StartPen() const { return iOpStartPen; }
	const TPoint& EndPen() const { return iOpEndPen; }
	const TRect& Bounds() const { return iBounds; }
	TRgb RuleColour() const { return iRuleColour; }
	MTmSource::TLabelType LabelType() const { return iLabelType; }
	const TDesC& InlineText() const { return iInlineText; }
	TInt InlineTextFormat() const { return iInlineTextFormat; }
	TBool AtEndOfTextLayout() const { return iOpEndCode == iReader.CodeEndPos(); }
	const CTmCode& ByteCode() const { return iReader.Code(); }
	TInt CodePos() const { return iReader.CodePos(); }
	void SetBdStatePtr(TBidirectionalContext* aBdState) { iBdState = aBdState; }
	TPoint TextLayoutTopLeft() const { return iTextLayoutTopLeft; }
	TBool VisualEndOfLineIsAmbiguous() const { return iVisualEndOfLineIsAmbiguous; }
	TBool VisualStartOfLineIsAmbiguous() const { return iVisualStartOfLineIsAmbiguous; }
	TInt Subscript();
	TUint ContextCharChar() const { return iContextChar; }
	TUint LineContextCharChar() const { return iLineContextChar; }

	enum TPanic
		{
		ECorrupt,
		EZeroLengthText,
		ENotImplemented				//MTmTextDrawExt interface has no implementation
		};
	static void Panic(TPanic aPanic) 
		{ 
		_LIT(KPanic,"TTmInterpreter");
		User::Panic(KPanic,aPanic); 
		}

private:
	TInt ReadOpLine(TInt aChars);

private:
	TPoint iTextLayoutTopLeft;		// top-left corner of the text - used to derive absolute line bounds and pen position
	TInt iTextLayoutWidth;			// width of the text - used when setting line rectangles
	TTmCodeReader iReader;			// an iterator to read the bytecode
	TInt iTextPos;					// current text position
	TPoint iPen;					// current pen

	// Information obtained from operators
	TUint8 iOp;						// the major operator just read
	TUint8 iOpMod;					// the modifier flags of the operator just read
	TUint8 iParamId;				// the parameter identifier, if the operator was EOpParam
	TInt iOpStartCode;				// code position before the operator
	TInt iOpEndCode;				// code position after the operator
	TPoint iOpStartPen;				// pen position before the operator
	TPoint iOpEndPen;				// pen position after the operator
	TInt iOpStartChar;				// start of text consumed by the operator
	TInt iOpEndChar;				// end of text consumed by the operator
	TInt iOpSubscript;				// height of subscript value currently only used for pictures

	TTmLineInfo iLineInfo;			// information about the current line
	// ETrue if either of the characters either side of the end of the line are in
	// an embedded run of text. If EFlase, this means that the cursor can omit
	// sitting on the visual end of the line without missing any ambiguous edges.
	TBool iVisualEndOfLineIsAmbiguous;
	// This line is after an ambiguous end of line 
	TBool iVisualStartOfLineIsAmbiguous;
	TInt iLineStartCode;
	TInt iNextLineTop;				// absolute y coordinate of the top of the next line
	TInt iNextLineStartChar;		// start character of the next line
	TInt iInlineTextFormat;			// document position for format for inline text
	TBuf<EMaxInlineChars+2> iInlineText;	// inline text
	TRect iBounds;					// rule, document, or band bounds
	TRgb iRuleColour;				// rule colour
	MTmSource::TLabelType iLabelType;	// label type if any
	TBidirectionalContext* iBdState;	// if non-null the current bidirectional state is placed here
	//	Context  characters: These one character context data members are used by the rendering code in GDI
	//	to render punctuation marks based on the context. The context here represents the script in which to render the punctuation.
	TUint iContextChar;			// One character context that was stored in the byte code during formatting.
										// Represents context for the line, or context for each chunk in the line.
										// The first chunk in the line gets the context from the previous line. Thereafter,
										// each chunk gets its context from the previous chunk.
	TUint iLineContextChar;
	};

/**
An interpreter class that can act as the base for any sort of interpreter; it
has a text source and font supplier.
@internalComponent
*/
class RTmGeneralInterpreter: public TTmInterpreter
	{
public:
	RTmGeneralInterpreter(MTmSource& aSource,const TTmInterpreterParam& aParam,const TPoint* aTopLeft = NULL);
	TBool GetDisplayedTextL(TInt aLineNumber,TDes& aText,TUint aContextChar,TInt& aNeeded);
	TBool FindXyPos(const TPoint& aXyPos,TTmPosInfo2& aInfo);
	TBool FindDocPos(const TTmDocPosSpec& aXyPos,TTmPosInfo2& aInfo);
	void GetText(TInt aPos,TInt aEndChar,TPtrC& aText,TTmCharFormat* aFormat = NULL,CTmTextFontCache** aFont = NULL);
	void Close() { iTextCache.Close(); }

public:
	void GetInlineText(TPtrC& aText,TTmCharFormat* aFormat = NULL,CTmTextFontCache** aFont = NULL);
	RTmTextCache& TextCache() { return iTextCache; }
	MTmSource& Source() { return iTextCache.Source(); }
	MTmTextDrawExt& TmTextDrawExt() {return *iTmTextDrawExt;}
	MGraphicsDeviceMap& Device() { return iTextCache.Device(); }
	TBool CopyFit() const { return iCopyFit; }
	void ReleaseFont() { iTextCache.ReleaseFont(); }

	TBool TrailingEdgeOfUnambiguousLineEnd(
		TTmPosInfo2& aPosOut, TBool aOnRight) const;

private:
	RTmTextCache iTextCache;		// caches text and formats obtained from the text source
	TBool iCopyFit;					// true if text must be stretched or shrunk to fit its formatted size
	MTmTextDrawExt* iTmTextDrawExt; // MTmTextDrawExt interface will be used to replace all
	                                // CGraphicsContext::Draw<Item> calls.
	TTmTextDrawExt	iTmTextDrawExtDefault;//The default implementation of MTmTextDrawExt
	};

/**
Draws formatted text.
@internalComponent
*/
class RTmDrawingInterpreter: public RTmGeneralInterpreter
	{
	public:
	RTmDrawingInterpreter(MTmSource& aSource,CGraphicsContext& aGc,
						  const TTmInterpreterParam& aParam,
						  const TPoint& aTopLeft,
						  const TRect& aClipRect,
						  const TLogicalRgb* aDocBackground,
						  TUint aFlags,
						  const TCursorSelection* aHighlight,
						  const TTmHighlightExtensions* aHighlightExtensions,
						  TBool aDrawInvertedHighlight = EFalse);
	~RTmDrawingInterpreter();
	void Draw();
	void DrawBackground(const TRect& aRect,const TLogicalRgb& aBackground);

	// bit values for iFlags
	enum
		{
		EIsLabel = 1,
		EDrawParBackground = 2,
		EInvisible = 4
		};

	private:
	void InitGraphicsContext();
	void Line(TRect& aDrawn);
	void Text();
	void InlineText();
	void DrawText(const TDesC& aText,const TInt aStart, const TInt aEnd, const TTmCharFormat& aFormat,CFont* aFont);
	void SpecialCharL();
	void Rule();
	void Label();
	void DrawLabelBackground();
	void DrawPageBreak();
	void DrawBullet();
	void DrawLineGraphics(const TRect& aRect);

	void GetBorderInfo(TRect& aInnerBounds,TRect& aOuterBounds,
					   TInt& aLeftWidth,TInt& aRightWidth,TInt& aTopWidth,TInt& aBottomWidth);
	void DrawBorders(const TRect& aInnerBounds,const TRect& aOuterBounds,
					 TInt aLeftWidth,TInt aRightWidth,TInt aTopWidth,TInt aBottomWidth);
	void DrawBorderRect(const TTmParBorder& aBorder,const TRect& aRect);
	void DrawPicture(CPicture* aPicture);
	void SetPenColor(TLogicalRgb aColor) { Source().SetPenColor(iGc,aColor); }
	void SetBrushColor(TLogicalRgb aColor) { Source().SetBrushColor(iGc,aColor); }

	TBool GetHighlightPos(TInt& aHighlightStartPos, TInt& aHighlightEndPos) const;
	TBool GetHighlightClipRegion(const TRect& aClipRect, RRegion& aHighlightRegion) const;
	TBool GetAdjustedHighlightClipRegion(const TRect& aClipRect, RRegion& aHighlightRegion) const;
	TBool IsCurrentOpCompletelyHighlighted() const;
	TBool UsesExtendedHighlight() const;
	TBool UsesAdjustedHighlight() const;
	void InvertHighlightedLineRgn();

	TBool GetFirstHighlightClipRect(RTmBoundingRectInterpreter& aInterpreter,
		const TRect& aClipRect, TRect& aRect) const;
	TBool GetNextHighlightClipRect(RTmBoundingRectInterpreter& aInterpreter,
		const TRect& aClipRect, TRect& aRect) const;
	TBool GetFirstAdjustedHighlightClipRect(RTmBoundingRectInterpreter& aInterpreter,
		const TRect& aClipRect, TRect& aRect) const;
	TBool GetNextAdjustedHighlightClipRect(RTmBoundingRectInterpreter& aInterpreter,
		const TRect& aClipRect, TRect& aRect) const;

	CGraphicsContext& iGc;				// graphics context on which to draw
	TRect iClipRect;					// clip rectangle in drawing coordinates
	TBool iFirstLine;					// TRUE if this is the first line to be drawn
	TBool iLabelled;					// TRUE if the current paragraph has a label
	TInt iLabelLeft;					// left edge of the label if any
	TInt iLabelRight;					// right edge of the label if any
	TLogicalRgb iLabelBackground;		// background colour for the label if any
	RTmParFormat iParFormat;			// current paragraph format
	const TLogicalRgb* iDocBackground;	// document background colour; if null, don't draw document background
	TUint iFlags;						// is this a label? draw paragraph backgrounds?

	TInt iHighlightStartPos;			// start position of the current selection
	TInt iHighlightEndPos;				// end position of the current selection
	const TTmHighlightExtensions* iHighlightExtensions;	// highlight extension to be used
	TRect iHighlightedLineRect;			// bounding rect of the highlighted region in the current line; it is only used when the highlight is xor-ed (iDrawInvertedHighlight == TRUE)
	RRegion iHighlightedLineRgn;		// highlighted region in the current line; it is only used when the highlight is xor-ed (iDrawInvertedHighlight == TRUE) 
	MTmSource& iSource;
	const TTmInterpreterParam& iParam;
	TBool iDrawInvertedHighlight;		// if true, the highlighted area is drawn xor-ing the pixels with iHighlightXorColor.
	TRgb iHighlightXorColor;			// color to be used to xor-draw the highlights.
	};

/**
Returns the bounding rectangles of a range of formatted text.
@internalComponent
*/
class RTmBoundingRectInterpreter: public RTmGeneralInterpreter
	{
	public:
	RTmBoundingRectInterpreter(MTmSource& aSource,const TTmInterpreterParam& aParam);
	TBool FirstRect(TInt aStartDocPos,TInt aEndDocPos,TRect& aRect);
	TBool NextRect(TRect& aRect);

	private:
	void Line();
	void Text();
	void SpecialCharacter();

	TTmDocPos iStartDocPos;	// start of range
	TTmDocPos iEndDocPos;	// end of range
	TRect iRect;				// current bounding rectangle
	enum TRangeState
		{
		EBeforeAllLines,
		EInFirstLine,
		EInMedialLine,
		EInLastLine,
		EAfterAllLines
		};
	TRangeState iRangeState;
	};

/**
Finds the bytecode containing a range of document positions
@internalComponent
*/
class TTmByteCodeFinder: public TTmInterpreter
	{
	public:
	TTmByteCodeFinder(const TTmInterpreterParam& aParam,TInt aStartDocPos,TInt aEndDocPos);

	/**
	Information returned by FindByteCode.
	@internalComponent
	*/
	class TInfo
		{
		public:
		TRect iBounds;				// rectangle enclosing all lines of the bytecode
		TInt iStartCodePos;			// start of the bytecode
		TInt iEndCodePos;			// end of the bytecode
		TTmLineInfo iStartInfo;		// line information for the first line
		TTmLineInfo iEndInfo;		// line information for the last line
		
		//	Context  characters: These one character context data members are used by the rendering code in GDI
		//	to render punctuation marks based on the context. The context here represents the script in which to render the punctuation.
		TUint iContextCharPerLine;	//	Temporary storage of one context character 
												//	that represents the context for the last line.
												//	Found in the byte code and used when formatting the
												//	current line.
		};

	TBool FindByteCode(TBool aToParStart, TBool aToParEnd, TInt aMaxExtraChars,
		TInfo& aInfo, TBidirectionalContext* aStartBdState = 0,
		TBidirectionalContext* aEndBdState = 0);
	TBool FindByteCodeAtEnd(TInfo& aInfo, TInt aMaxHeight, TInt aTotalHeight, 
		TBidirectionalContext* aStartBdState,TBidirectionalContext* aEndBdState);

	private:
	TInt iStartDocPos;			// start of range
	TInt iEndDocPos;			// end of range
	};

/**
Holds information for the position at the edge of a grapheme and the number
of code-points within.
@internalComponent
*/
class TTmGraphemeEdgeInfo
	{
public:
	/** leading or trailing edge position of the grapheme. */
	TTmPosInfo2 iPos;
	/** Number of code-points in the grapheme described. */
	TInt iCodePoints;
	};

/**
Iterates through the edge positions within a text chunk of formatted text.
@internalComponent
*/
class RTmGraphemeInTextChunkIterator
	{
public:
	void Close();
	void Reset();
	RTmGraphemeInTextChunkIterator();
	void Begin(RTmGeneralInterpreter& aByteCodeIterator,
		TTmGraphemeEdgeInfo (*aCache)[2]);
	TInt Next();
	TBool AtEnd() const;

private:
	CFont::TPositionParam iPosition;
	TText iBuffer[KMaxTextChunkSize + 2];
	TInt iLength;
	TInt iStartChar;
	TInt iEndChar;
	CTmTextFontCache* iFont;
	TTmGraphemeEdgeInfo (*iCache)[2];
	RTmTextCache::TDisplayedTextDirectionality iReverse;
	TInt iIgnorePosition;
	TInt iStartPenX;
	TInt iEndPenX;
	RShapeInfo iShapeInfo;	//	Shaping header to keep shaping information alive across chunk iterations
							//	Used only for complex script rendering such as Devanagari
	};

/**
Iterates through the edge positions within a text chunk of formatted text,
with a nice one-edge-at-a-time interface.
@internalComponent
*/
class RTmGraphemeInTextChunkIteratorNice
	{
public:
	RTmGraphemeInTextChunkIteratorNice(
		RTmGeneralInterpreter& aByteCodeIterator);
	void Close();
	void Next();
	const TTmGraphemeEdgeInfo* Get() const;
	TBool AtEnd() const;

	void FindEdge(const TTmDocPos& aDocPos);

private:
	RTmGraphemeInTextChunkIterator iBase;
	TTmGraphemeEdgeInfo iCache[2];
	const TTmGraphemeEdgeInfo* iCurrent;
	const TTmGraphemeEdgeInfo* iEnd;
	};

/**
Represents a document position that may have an ambiguous logical position.

TTmVisualDocPos is used wherever a function needs to return a position which
the caller may want to "disambiguate". This is typically when finding a cursor
position given some criteria for its visual location; depending on the current
input language, the caller may prefer to attach the cursor to a character
within a right-to-left block, or a character within a left-to-right block. This
can be done by examining each edge of the TTmVisualDocPos to see if either fits
the requirements.

In documentation here, the idea that a "visual position" can be "logically
ambiguous" means that positions from different runs of text coincide at the
same visual position.

The idea that a "logical position" can be "visually ambiguous" means that the
pair of positions (n, leading) and (n, trailing) for some document position n
have different visual positions. In normal monodirectional text on one line
these positions are visually coincident.

Note that all mandatory line break characters (such as paragraph delimiters)
have their leading and trailing edges on different lines.

@internalComponent
@see TTmAmbiguityChecker
*/
class TTmVisualDocPos
	{
public:
	/** Which sort of ambiguity we have. */
	enum TAmbiguity
		{
		/** No positions set. */
		ENotFound = -1,
		/** No ambiguity. */
		EUnambiguous = 0,
		/** Text goes right from here but not left. This often happens at the
		left hand edge of a line. The edge that belongs with it logically is on
		another line, so positions like this are visually ambiguous. */
		ERightOnly = 1,
		/** Text goes left from here but not right. This often happens at the
		right hand edge of a line. The edge that belongs with it logically is
		on another line, so positions like this are visually ambiguous. */
		ELeftOnly = 2,
		/** Text in either direction meets at an ambiguity. Each edge has a
		logical partner that is in a different position, and both positions are
		visually coincident even though they are not logical partners. Hence
		each logical position is visually ambiguous and the visual position
		they share is logically ambiguous. */
		EAmbiguous = 3
		};

	void Reset();
	TTmVisualDocPos();
	void SetAsUnambiguous(const TTmPosInfo2& aPos);
	void SetIfAmbiguous(const TTmPosInfo2& aLeft, const TTmPosInfo2& aRight);

	/**
	What sort of document position we have: whether it is unambiguous,
	left-sided, right-sided or ambiguous.
	*/
	TAmbiguity Ambiguity() const { return iAmbiguity; }
	/**
	Left side of the ambiguity, undefined if Ambiguity returns ENotFound.
	*/
	const TTmPosInfo2& LeftEdge() const { return iLeft; }
	/**
	Right side of the ambiguity, undefined if Ambiguity returns ENotFound.
	*/
	const TTmPosInfo2& RightEdge() const { return iRight; }
	const TTmPosInfo2* NearestToX(TInt aX) const;

private:
	TTmPosInfo2 iLeft;
	TTmPosInfo2 iRight;
	TAmbiguity iAmbiguity;
	};

/**
When given the edges in a line one by one, TTmAmbiguityChecker can report the
latest logically ambiguous position before the last edge entered. Once EndLine
has been called after all the edges in a line have been entered, whether the
end of the line is ambiguous can be determined.

A logically ambiguous position is a position where two chunks with different
directionalities meet. Many edges can coincide in their X-coordinate if there
are characters of zero width there. A logically ambiguous position with
multiple coincident edges is therefore likely wherever a left-to-right marker
or right-to-left marker is used.

If we have a choice of which of several coincident edges to return (as with
FindXPos or FindEdgeRightwards, for example) it is useful to return the
logically ambiguous position if there is one. Returning the logically ambiguous
position rather than any other position at the same X-coordinate helps the
caller if they want to "disambiguate": to choose a position that attaches to a
character of the favoured directionality, for example.

The position is represented by a TTmVisualDocPos, which can be unset, set but
unambiguous, or ambiguous in one of three ways. The TTmAmbiguityChecker will
never return an unambiguous position, it is up to the caller to determine which
position is favoured if no ambiguity exists with the appropriate
characteristics.

@see TTmVisualDocPos
@internalComponent
*/
class TTmAmbiguityChecker
	{
public:
	void Reset();
	TTmAmbiguityChecker();
	void AddEdge(const TTmPosInfo2& aEdge);
	void EndLine();
	/** Gets the last ambiguous position found. */
	const TTmVisualDocPos& LastAmbiguity() const
		{ return iLastAmbiguousPos; }

private:
	TTmPosInfo2 iPrev;
	TTmVisualDocPos iLastAmbiguousPos;
	};

/**
Iterates through the edges of the characters in a line.
@internalComponent
 */
class RTmGraphemeEdgeIterator
	{
public:
	void Close();
	void Begin(RTmGeneralInterpreter& aByteCodeIterator);
	void Next();
	const TTmGraphemeEdgeInfo& Get() const;
	const TTmPosInfo2& GetInfo() const;
	TBool AtEnd() const;

	TBool FindEdgeByX(TInt aX, TTmPosInfo2& aNearest,
		TTmVisualDocPos& aAmbiguity);
	void FindXPos(TInt aX, TTmVisualDocPos& aNearest);
	TBool FindEdge(const TTmDocPosSpec& aDocPos, TTmPosInfo2& aInfo);

	/**
	Which positions were found.
	@see FindEdgeLeftwards
	@see FindEdgeRightwards
	*/
	enum TEdgesFound
		{
		ENone = 0,
		ENearestOnly = 1,
		ENearestAndNext = 2
		};

	TEdgesFound FindEdgeLeftwards(const TTmDocPosSpec& aDocPos,
		TTmPosInfo2& aNearest, TTmVisualDocPos& aNext);
	TEdgesFound FindEdgeRightwards(const TTmDocPosSpec& aDocPos,
		TTmPosInfo2& aNearest, TTmVisualDocPos& aNext);
	TInt NextPosition(TInt aDocPos);
	TInt PreviousPosition(TInt aDocPos);

	/**
	@see DocPosMatches
	Note that two "position only" matches add up to a "total" match.
	This is intentional, and can simplify some algorithms.
	*/
	enum TGraphemeMatch
		{
		ENoMatch = 0,
		EPositionOnly = 1,
		ETotalMatch = 2
		};

	static TGraphemeMatch DocPosMatches(const TTmDocPosSpec& aDocPos,
		const TTmGraphemeEdgeInfo& aEdgeInfo);

private:
	RTmGeneralInterpreter* iByteCodeIterator;
	TInt iEndLineCodePos;
	RTmGraphemeInTextChunkIterator iTextChunkIterator;
	TTmGraphemeEdgeInfo iPosInfoCache[2];
	TInt iCacheSize;
	TInt iCachePos;
	TBool iTrailingEdgeOfUnambiguousLineEndAdded;
	/**	Edge which is used by Next() to determine where to stop advancing the interation
		when it iterates through EOpSpecialChar in the bytecode. Currently only set in Begin() 
		to the end of the line position LineInfo().iEnd */
	TInt iIgnorePosition;
	};

#endif // __INTERP_H__
