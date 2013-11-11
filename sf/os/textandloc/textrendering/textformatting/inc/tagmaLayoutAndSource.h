/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TAGMA_LAYOUT_AND_SOURCE_H
#define TAGMA_LAYOUT_AND_SOURCE_H

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <tagma.h>
#endif

class CTmBufSeg;
class CTmCode;
class TTmTab;
class TTmBullet;
class TTmParBorder;
class TTmReformatParam;
class TTmReformatResult;

/** 
WARNING: this Class is for internal use ONLY.  Compatibility is not guaranteed in future releases.
Paragraph format; all dimensions are in twips unless otherwise specified. 
@internalComponent
*/
class RTmParFormat

	{
public:
	/** Index used to select one of the four borders of a paragraph */
	enum TBorderIndex
		{
		ETopBorder = 0,
		EBottomBorder = 1,
		ELeadingBorder = 2,
		ETrailingBorder = 3,
		};

	/** Bit assignments for RTmParFormat::iFlags. */
	enum
		{
		/** Base direction of paragraph is right-to-left (as for Arabic) */
		ERightToLeft = 1,
		/** Do not allow paragraph to straddle page boundaries. */
		EKeepTogether = 2,
		/** Put this paragraph on the same page as the next. */
		EKeepWithNext = 4,
		/** Put this paragraph at the start of a new page. */
		EStartNewPage = 8,
		/** Suppress widows and orphans. */
		EWidowOrphan = 16,
		/** Suppresses line breaking. */
		ENoWrap = 32,
		/** Force line spacing distance to be respected even on lines that are
		 * taller than the specified height. */
		EExactLineSpacing = 64,
		/** Line spaceing is in pixels, not twips. */
		EPixelLineSpacing = 128,
		/** Work out paragraph directionality from the text. */
		EDirectionalityFromText = 256
		};

	/** Paragraph alignment */
	enum TAlignment
		{
		EAlignNormalBidirectional,
		EAlignNormal = EAlignNormalBidirectional,		/** @deprecated in 7.0s */
		EAlignCenter,
		EAlignReverseBidirectional,
		EAlignReverse = EAlignReverseBidirectional,		/** @deprecated in 7.0s */
		EAlignJustify,
		EAlignAbsoluteLeft,
		EAlignAbsoluteRight
		};

	IMPORT_C RTmParFormat();
	IMPORT_C void Close();
#ifdef _DEBUG
	inline ~RTmParFormat();
#endif
	IMPORT_C void CopyL(const RTmParFormat& aFormat);
	IMPORT_C void CopyL(const CParaFormat& aFormat);
	IMPORT_C void GetCParaFormatL(CParaFormat& aFormat) const;
	IMPORT_C TBool operator==(const RTmParFormat& aFormat) const;
	inline TBool operator!=(const RTmParFormat& aFormat) const;
	IMPORT_C TInt Tabs() const;
	inline const TTmTab& Tab(TInt aIndex) const;
	inline const TTmBullet* Bullet() const;
	inline const TTmParBorder* Border(TBorderIndex aIndex) const;
	IMPORT_C TBool HaveBorders() const;
	inline TBool RightToLeft() const;

	TAlignment iAlignment;
	TUint iFlags;						// bit flags as defined above
	TInt iLeadingMargin;				// left margin, or right if right-to-left
	TInt iTrailingMargin;				// right margin, or left if right-to-left
	TInt iFirstLineIndent;				// added to first line leading margin
	TInt iLineSpacing;					// distance between baselines
	TInt iSpaceAbove;					// space above the paragraph
	TInt iSpaceBelow;					// space below the paragraph
	TInt iTabSize;						// default size of tabs
	TInt iBorderMargin;					// distance between the text and the border if any
	TLogicalRgb iBackgroundColor;		// paragraph background colour

private:
	RTmParFormat(const RTmParFormat& aFormat);	// deliberately unimplemented
	void operator=(const RTmParFormat& aFormat);// deliberately unimplemented

	RArray<TTmTab>* iTabList;			// custom tabs if any
	TTmBullet* iBullet;					// bullet if any
	TTmParBorder* iBorder[4];			// borders if any
	};

/** 
WARNING: this Class is for internal use ONLY.  Compatibility is not guaranteed in future releases.
MTmSource is an interface class that must be implemented by users of
CTmTextLayout to provide text content and attributes. MTmSource is derived from
MTmCustom, which contains the functions to customise the layout and display, so
that this can be done in CTmText without affecting the content.
@internalComponent
*/
class MTmSource: public MTmCustom

	{
public:
	/** Label type used in LabelModeSelect */
	enum TLabelType
		{
		/** Not a label; used for TLabelTypes not yet assigned. */
		ENoLabel,

		/** Paragraph label of the type supported by FORM. */
		EParLabel
		};

	enum
		{
		/**
		The maximum number of characters supplied to GetLineBreak, after aMaxBreakPos,
		and after trailing whitespace, to provide context for line breaking decisions.
		*/
		ELineBreakContext = 32
		};

	// pure virtuals
	
	/** Return the device used to format the text. */
	virtual MGraphicsDeviceMap& FormatDevice() const = 0;

	/**
	Return the device used to draw the text and when converting between x-y coordinates and document positions.
	If the formatting and interpreting devices are different, text is scaled appropriately when it is drawn;
	this allows text to be formatted for printer fonts and displayed on the screen, giving a wysiwyg print preview.
	*/
	virtual MGraphicsDeviceMap& InterpretDevice() const = 0;

	/** Return the length of the document in characters. */
	virtual TInt DocumentLength() const = 0;

	/**
	Return at least one character of text, but preferably as many as possible, starting at aPos. Put its character
	format in aFormat. The text must be a run of characters sharing the same character format.
	*/
	virtual void GetText(TInt aPos,TPtrC& aText,TTmCharFormat& aFormat) const = 0;

	/**
	Return the paragraph format of the paragraph containing aPos. End-of-paragraph characters belong to the preceding
	paragraph.
	*/
	virtual void GetParagraphFormatL(TInt aPos,RTmParFormat& aFormat) const = 0;

	/**
	Return the document position of the start of the paragraph containing aPos. End-of-paragraph characters belong to
	the preceding paragraph.
	*/
	virtual TInt ParagraphStart(TInt aPos) const = 0;

	// ordinary virtuals
	IMPORT_C virtual CPicture* PictureL(TInt aPos) const;
	IMPORT_C virtual TInt GetPictureSizeInTwipsL(TInt aPos,TSize& aSize) const;
	IMPORT_C virtual TAny* GetExtendedInterface(const TUid& aInterfaceId);
	IMPORT_C virtual void MTmSource_Reserved_1();
	IMPORT_C virtual void MTmSource_Reserved_2();
	IMPORT_C virtual TBool PageBreakInRange(TInt aStartPos,TInt aEndPos) const;

	// non-virtuals
	IMPORT_C TInt ParagraphEnd(TInt aPos) const;
	IMPORT_C TBool GetLineBreakL(const TDesC& aText,TInt aDocPos,TInt aMinBreakPos,TInt aMaxBreakPos,TBool aForwards,
								 TInt& aBreakPos,TInt& aHangingChars,TInt& aBreakPosAfterSpaces) const;
	IMPORT_C static void GetStandardLineBreak(const TDesC& aText,TInt aMinBreakPos,TInt aMaxBreakPos,
											  TInt& aBreakPos,TInt& aNextLineStart);
	};

/**
WARNING: this Class is for internal use ONLY.  Compatibility is not guaranteed in future releases.
@internalTechnology
*/
class CTmCode: public CBase
	{
public:
	inline ~CTmCode();
	void InsertByteL(TUint8 aByte,TInt aPos);
	TInt InsertNumberL(TInt aNumber,TInt aPos);
	TInt InsertRectL(const TRect& aRect,TInt aPos);
	void AppendByteL(TUint8 aByte);
	TInt AppendNumberL(TInt aNumber);
	TInt AppendRectL(const TRect& aRect);
	void ChangeL(TInt aStart,TInt aEnd,CTmCode& aNewCode);
	TInt MemoryUsed() const;
	TInt Size() const;
	void Delete(TInt aPos,TInt aLength);
	void Reset();
	inline TPtr8 Ptr(TInt aPos);
	void CreateBufferL();
	inline CBufBase* Buffer();
	inline const CBufBase* Buffer() const;

private:
	enum
		{
		EExpandSize = 512
		};
	TInt WriteNumber(TInt aNumber,TUint8* aBuffer);

	CTmBufSeg* iBuffer;
	};

/**
WARNING: this Class is for internal use ONLY.  Compatibility is not guaranteed in future releases.
Formatting parameters used when formatting part of a layout object.
@internalComponent
*/
class TTmFormatParamBase

	{
public:
	enum
		{
		EWrap = 1,							// wrap the text at iWrapWidth
		EAtLeastMaxHeight = 2,				// stop adding text only when iMaxHeight has been reached or exceeded
		ETruncateWithEllipsis = 4,			// truncate any text that sticks out; see iEllipsis below
		ELegalLineBreaksOnly = 8			// break only at allowed line breaks, even if that
											// results in the text not fitting the measure
		};

	IMPORT_C TTmFormatParamBase();
	inline TBool IsWrapping() const;
	inline TBool IsTruncatingWithEllipsis() const;
	inline TBool LegalLineBreaksOnly() const;

	TInt iWrapWidth;					// wrapping width of the text including margins and indents, if wrapping is on
	TInt iMaxHeight;					// maximum height of the text
	TInt iMaxLines;						// maximum number of lines to be formatted
	TInt iFlags;						// flags defined above
	TChar iEllipsis;					// if truncating and not 0xFFFF, insert this ellipsis char (defaults to '...').
				
	};

/** 
WARNING: this Class is for internal use ONLY.  Compatibility is not guaranteed in future releases.
Formatting parameters used when formatting an entire layout object or
creating a section of format code.
@internalComponent
*/
class TTmFormatParam: public TTmFormatParamBase

	{
public:
	IMPORT_C TTmFormatParam();
	IMPORT_C TTmFormatParam(const TTmFormatParamBase& aBase);

	TInt iStartChar;					// first character position to be formatted
	TInt iEndChar;						// last character position to be formatted
	TInt iLineInPar;					// starting line number in the paragraph
	};

/**
WARNING: this Class is for internal use ONLY.  Compatibility is not guaranteed in future releases.
The text layout for a single rectangular piece of text. The object does not own its text or format attributes but
gets them from an interface class called MTmSource.

There are functions for setting the text, drawing it, and converting between x-y coordinates and document positions.

Ranges

All ranges of character positions and pixels include the start but not the end. This means that if a line is described
by a TTmLineInfo object as starting at document position 345 and ending at 389, character positions 345 to 388 are in
the line; 389 is in the next line if any.

Coordinates and units

All coordinates used in TAGMA classes and function arguments are in pixels and are relative to the origin of the object,
which is the top left corner of the ordinary text, not including paragraph labels. Vertical coordinates increase from
top to bottom.

Document format and positions

A TAGMA document, as supplied by the MTmSource interface, has a length as returned by MTmSource::DocumentLength of
zero or more characters. It consists of Unicode text split into paragraphs by standard Unicode paragraph delimiters
(character value 0x2029), so the number of paragraphs in the entire document is one more than the number of
paragraph delimiters. A document position is a value in the range 0 to one more than the document length.
Lines contain all document positions from their start to one less than their end position. These conventions allow
all lines to end in a delimiter; the last line contains an imaginary paragraph end, which need not be supplied by
the MTmSource interface, at a position equal to the document length. A CTmTextLayout object formatted for an
empty document will thus have a start position (returned by StartChar) of 0 and an end position
(returned by EndChar) of 1.
@internalComponent
WARNING: this Class is for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
class CTmTextLayout: public CBase

	{
public:
	
	class TTmChunkDescription
	/** 
	Output from FindAdjacentChunks. Describes a chunk of text that is of a
	constant format and directionality. 
	@internalComponent
	*/
		{
	public:
		/** Position of the first character in the chunk. A negative number
		indicates "no such chunk". */
		TInt iStart;
		/** One past the position of the last character in the chunk. */
		TInt iEnd;
		/** ETrue if the chunk is displayed right-to-left. */
		TBool iRightToLeft;
		};

	IMPORT_C CTmTextLayout();
	IMPORT_C ~CTmTextLayout();
	IMPORT_C void SetTextL(MTmSource& aSource,const TTmFormatParam& aParam);
	IMPORT_C void FormatL(const TTmFormatParamBase& aParam,const TTmReformatParam& aReformatParam,
						  TTmReformatResult& TTmReformatResult);
	IMPORT_C TBool AddParL(const TTmFormatParamBase& aParam,TBool aAtStart,TInt& aHeightIncrease,TInt& aParagraphsIncrease);
	IMPORT_C TBool DeletePar(const TTmFormatParamBase& aParam,TBool aAtStart,TInt aMaxDeletedHeight,TInt& aHeightDecrease);
	IMPORT_C void Clear();
	IMPORT_C void DrawLayout(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
							 const TLogicalRgb* aDocBackground,TBool aDrawParBackground) const;
	IMPORT_C void DrawBackground(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
								 const TLogicalRgb& aBackground) const;
	IMPORT_C void InvertLayout(CGraphicsContext& aGc, const TPoint& aTopLeft,
		TInt aStartDocPos, TInt aEndDocPos) const;
	IMPORT_C void HighlightSection(CGraphicsContext& aGc, const TPoint& aTopLeft,
		TInt aStartDocPos, TInt aEndDocPos, const TRect& aClipRect) const;
	IMPORT_C void DrawSection(CGraphicsContext& aGc, const TPoint& aTopLeft,
		TInt aStartDocPos, TInt aEndDocPos, const TRect& aClipRect) const;
	/* this function is some what used by partner already */
	IMPORT_C void HighlightSection(CGraphicsContext& aGc, const TPoint& aTopLeft,
		TInt aStartDocPos, TInt aEndDocPos, const TRect& aClipRect, const TTmHighlightExtensions& aHighlightExtensions,
		TInt aHighlightStartDocPos, TInt aHighlightEndDocPos) const;
	/* this function is some what used by partner already */
	IMPORT_C void InvertLayout(CGraphicsContext& aGc,
		const TPoint& aTopLeft, TInt aStartDocPos,TInt aEndDocPos, const TTmHighlightExtensions& aHighlightExtensions,
		TInt aHighlightStartDocPos, TInt aHighlightEndDocPos) const;
	/* this function is some what used by partner already */
	IMPORT_C void DrawSection(CGraphicsContext& aGc, const TPoint& aTopLeft,
		TInt aStartDocPos, TInt aEndDocPos, const TRect& aClipRect, const TTmHighlightExtensions& aHighlightExtensions,
		TInt aHighlightStartDocPos, TInt aHighlightEndDocPos) const;
	IMPORT_C TBool FindDocPos(const TTmDocPosSpec& aDocPos,TTmPosInfo2& aPosInfo,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool FindXyPos(const TPoint& aXyPos,TTmPosInfo2& aPosInfo,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool FindXyPosWithDisambiguation(const TPoint& aXyPos,
		TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight,
		TTmLineInfo& aLineInfo) const;
	IMPORT_C TInt FindNextPos(TInt aStart) const;
	IMPORT_C TInt FindPreviousPos(TInt aStart) const;
	IMPORT_C TInt Lines() const;
	IMPORT_C TInt Paragraphs() const;
	IMPORT_C TBool DocPosToLine(const TTmDocPosSpec& aDocPos,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool LineNumberToLine(TInt aLineNumber,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool ParNumberToLine(TInt aParNumber,TInt aLineInPar,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool YPosToLine(TInt aYPos,TTmLineInfo& aLineInfo) const;
	IMPORT_C TBool GetDisplayedTextL(TInt aLineNumber,TDes& aText,TInt& aNeeded) const;
	IMPORT_C void GetMinimumLayoutSizeL(TInt aWrapWidth,TSize& aSize) const;
	IMPORT_C void GetMinimumLayoutSizeL(TInt aWrapWidth,TBool aAllowLegalLineBreaksOnly,TSize& aSize) const;
	IMPORT_C TInt WidthOfWidestLine(TInt aTop = 0,TInt aBottom = KMaxTInt) const;
	IMPORT_C void HorizontalExtremes(TInt &aLeft, TInt &aRight,
		TInt aTopY = 0, TInt aBottomY = KMaxTInt) const;
	IMPORT_C TBool GetNextVisualCursorPos(const TTmDocPosSpec& aDocPos,TTmPosInfo2& aInfo,TBool aToLeft) const;
	IMPORT_C TBool GetNextPosLeftWithDisambiguation(const TTmDocPosSpec& aDocPos,
		TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight) const;
	IMPORT_C TBool GetNextPosRightWithDisambiguation(const TTmDocPosSpec& aDocPos,
		TTmPosInfo2& aPosLeft, TTmPosInfo2& aPosRight) const;
	IMPORT_C TBool LineExtreme(const TTmDocPosSpec& aPos, TBool aToRight,
		TTmDocPos& aExtreme) const;
	IMPORT_C TBool FindAdjacentChunks(const TTmDocPosSpec& aPos,
		TTmChunkDescription& aLeft, TTmChunkDescription& aRight) const;
	IMPORT_C TBool GetCursor(const TTmDocPosSpec& aDocPos,TTmCursorPlacement aPlacement,
							 TTmLineInfo& aLineInfo,TPoint& aOrigin,TInt& aWidth,TInt& aAscent,TInt& aDescent) const;
	IMPORT_C TInt MemoryUsed() const;
	inline TInt StartChar() const;
	inline TInt EndChar() const;
	inline TInt LayoutWidth() const;
	inline TInt LayoutHeight() const;
	inline MTmSource* Source();
	inline const MTmSource* Source() const;
	inline const CTmCode& Code() const;
 	IMPORT_C void MakeVisible(TBool aVisible);
 	TInt GetDrawingInterpFlags() const;
	IMPORT_C void DeleteFormattingFromEndL(
		const TTmFormatParamBase& aParam, TInt aMaxDeletedHeight, TInt& aHeightDecrease);
	IMPORT_C void ExtendFormattingDownwardsL(TTmFormatParam& aParam);

	// deprecated functions
	// deprecated 7.0
	IMPORT_C TBool GetDisplayedText(TInt aLineNumber,TDes& aText,TInt& aNeeded) const;
	// deprecated 7.0s
	IMPORT_C TBool FindDocPos(const TTmDocPos& aDocPos,TTmPosInfo& aPosInfo,TTmLineInfo& aLineInfo) const;
	// deprecated 7.0s
	IMPORT_C TBool FindXyPos(const TPoint& aXyPos,TTmPosInfo& aPosInfo,TTmLineInfo& aLineInfo) const;
	// deprecated 7.0s
	IMPORT_C TBool GetNextVisualCursorPos(const TTmDocPos& aDocPos,TTmPosInfo& aInfo,TBool aToLeft) const;
	// deprecated 7.0s
	IMPORT_C TBool DocPosToLine(const TTmDocPos& aDocPos,TTmLineInfo& aLineInfo) const;
	// deprecated 7.0s
	IMPORT_C TBool GetCursor(const TTmDocPos& aDocPos,
		TTmCursorPlacement aPlacement, TTmLineInfo& aLineInfo,
		TPoint& aOrigin, TInt& aWidth, TInt& aAscent, TInt& aDescent) const;

	IMPORT_C void DrawLayout(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,
							 const TLogicalRgb* aDocBackground,TBool aDrawParBackground,
							 const TCursorSelection* aHighlight,
							 const TTmHighlightExtensions* aHighlightExtensions) const;
	IMPORT_C void GetUpdateBoundingRect(TInt aStartDocPos, TInt aEndDocPos, const TPoint& aTopLeft,
		TRect& aBoundingRect) const;

	IMPORT_C TBool FindDocPos(const TTmDocPosSpec& aDocPos,TTmPosInfo2& aPosInfo,TTmLineInfo& aLineInfo, TInt& aSubscript) const;
	void SetCurrentContextChar(TUint aContextChar);
	TUint GetCurrentContextChar() const;
private:
	TBool LastLine(TTmLineInfo& aLine);
#ifdef _DEBUG
	void Invariant() const;
#else
	void Invariant() const { }
#endif
	void AdjustWidth(const TTmFormatParamBase& aParam,TInt aWidthOfNewText);

	MTmSource* iSource;				// source of text and text attributes; not owned
	CTmCode iCode;					// the layout bytecode
	TInt iWidth;					// width in pixels
	TInt iHeight;					// height in pixels
	TInt iStartChar;				// start character position in the document
	TInt iEndChar;					// end character position in the document
	TBidirectionalContext* iBdStateAtEnd; //bidirectional state at end of formatted range
	TInt iDrawingInterpFlags;		// flags destined for the RTmDrawingInterpreter object
	// please try to avoid removing or adding any new members to this class. although it
	// is internal, it is included by value in CTextLayout which is published.  therefore
	// a size change in this class would mean a size change in CTextLayout and a BC break.
	// if adding or removing members is unavoidable, please remember to update the dummy
	// member in CTextLayout accordingly.
	TUint iCurrentContextChar;		// One character of context that is stored here between formatting on lines.
						// At the end of the formatting of a line, the context for that line is stored
						// here and taken by the next line to be formatted. The first chunk of the new line
						// takes this context from the line, and then all subsequent chunks take their context
						// from the previous chunk.
	};


//inline functions

#ifdef _DEBUG
RTmParFormat::~RTmParFormat()
	{
	__ASSERT_DEBUG(!iTabList && !iBullet && !HaveBorders(), User::Invariant());
	}
#endif

/** The inequality operator. Returns true if this object and aFormat differ in
any way.
*/
TBool RTmParFormat::operator!=(const RTmParFormat& aFormat) const
	{
	return !operator==(aFormat);
	}

/** Returns a reference to the tab selected by aIndex. */
const TTmTab& RTmParFormat::Tab(TInt aIndex) const
	{
	return (*iTabList)[aIndex];
	}

/** Returns a pointer to the TTmBullet object if any. Returns null if the
RTmParFormat does not contain a TTmBullet. */
const TTmBullet* RTmParFormat::Bullet() const
	{
	return iBullet;
	}

/** Returns a pointer to the border selected by aIndex. Returns null if the
selected border is not present. */
const TTmParBorder* RTmParFormat::Border(TBorderIndex aIndex) const
	{
	return iBorder[aIndex];
	}

/** Returns true if the paragraph's base direction is right-to-left. */
TBool RTmParFormat::RightToLeft() const
	{
	return iFlags & ERightToLeft;
	}

/** Returns true if wrapping is turned on. */
TBool TTmFormatParamBase::IsWrapping() const
	{
	return iFlags & EWrap;
	}

/** Returns true if truncation with ellipsis is turned on. */
TBool TTmFormatParamBase::IsTruncatingWithEllipsis() const
	{
	return iFlags & ETruncateWithEllipsis;
	}

/** Returns true if line breaking must only occur at legal line breaks, even if
the line cannot legally be broken at the desired wrap width. */
TBool TTmFormatParamBase::LegalLineBreaksOnly() const
	{
	return iFlags & ELegalLineBreaksOnly;
	}


/** Returns the first formatted character. */
TInt CTmTextLayout::StartChar() const
	{
	return iStartChar;
	}

/** Returns the character after the last formatted character. */
TInt CTmTextLayout::EndChar() const
	{
	return iEndChar;
	}

/** Return the width in pixels of the formatted text, not including paragraph
labels if present. */
TInt CTmTextLayout::LayoutWidth() const
	{
	return iWidth;
	}

/** Returns the height in pixels of the formatted text. */
TInt CTmTextLayout::LayoutHeight() const
	{
	return iHeight;
	}

/** Returns a pointer to to the MTmSource object, if any, that provides text
and formatting to this object. Returns null if no source object has been set. */
MTmSource* CTmTextLayout::Source()
	{
	return iSource;
	}

const MTmSource* CTmTextLayout::Source() const
	{
	return iSource;
	}

/**
Returns a reference to the CTmCode object containing the bytecode
representing the text layout.
@internalComponent
*/
const CTmCode& CTmTextLayout::Code() const
	{
	return iCode;
	}


#endif
