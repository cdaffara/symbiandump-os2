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
* classes used in the implementation of CTmTextLayout
*
*/


#ifndef TMLAYOUTIMP_H_
#define TMLAYOUTIMP_H_

#include "TMSTD.H"
#include <bidi.h>

class CTmFormatContext;

/**
A chunk of text is a reference to a run of characters that have constant
character format, directionality, stretchiness and that are to be drawn on
the same line. It includes information about the dimensions of the chunk
when rendered.

@internalComponent
*/
class TTmChunk
	{
public:
	TTmChunk();
	TTmChunk& operator=(const TTmChunk& aChunk) { Mem::Copy(this,&aChunk,sizeof(TTmChunk)); return *this; } 

	/**
	Information returned by SetL
	@internalComponent
	*/
	class TInfo

		{
		public:
		TInfo(): iAtLineEnd(EFalse), iAtParEnd(EFalse), iTruncated(EFalse), iBdCat(TChar::EOtherNeutral), iEllipsisWidth(0) { }

		TBool iAtLineEnd;			// true if the new chunk is at the end of a line
		TBool iAtParEnd;			// true if the new chunk is at the end of a paragraph
		TBool iTruncated;			// true if the chunk was cut short to fit the measure
		TChar::TBdCategory iBdCat;	// the bidirectional category of the chunk
		TInt iEllipsisWidth;		// the width of any ellipsis allowed for, if truncating with ellipsis
		};

	void SetL(CTmFormatContext& aContext, TInt aStartChar, TInt aStartXPos, TInt aMaxChar, TInt aMaxXPos,
			  TBool aAllowSpaceForFinalInlineText, TInfo& aInfo);
private:
	void TruncateIfNeeded(CTmFormatContext& aContext, TPtrC& aText, TInfo& aInfo);

public:
	TInt iDocPos;				// position of the start of the text in the document
	TInt iTextLength;			// length of the text
	TTmCharFormat iFormat;		// the text's format
	TInt iWidth;				// width of the text in pixels
	TInt iStandardWidth;		// width before modification
								// centre and right tabs may be reduced in size, soft hyphens are zeroed if
								// non-final, and text may be stretched for full justification
	TInt iAscent;				// font ascent
	TInt iDescent;				// font descent
	TInt iMaxCharHeight;		// greatest character height
	TInt iMaxCharDepth;			// greatest character depth
	TInt iFontMaxHeight;		// maximum height of any character in the font
	TInt iFontMaxDepth;			// maximum depth of any character in the font
	TInt iBaselineOffset;		// -ve(above)/0/+ve baseline offset from CFont in pixels

	TInt iStretch;				// stretchability level (non-zero means space can be appended when fully justifying)
	enum TType
		{
		ETabFlag = 0x80,

		ETextChunk = 0,					// ordinary text
		EStretchedChunk = 1,			// text plus extra space for justification
		EPictureChunk = 2,				// a picture
		ESoftHyphenChunk = 3,			// a soft (potential) hyphen
		EStandardTabChunk = ETabFlag | 0,	// a left-aligning tab
		ECenterTabChunk = ETabFlag | 1,	// a centring tab
		EReverseTabChunk = ETabFlag | 2	// a right-aligning tab
		};
	TType iType;				// the type of this chunk
	// Indicates that the chunk must not be amalgamated with its successor
	TBool iForcedChunkEnd;
	// Indicates that the chunk must not be amalgamated with its predecessor
	TBool iForcedChunkStart;
	TInt iInitialInlineWidth;	// the width occupied by any inline text attached to the start of this chunk
	TInt iFinalInlineWidth;		// the width occupied by any inline text attached to the end of this chunk

	TInt iOverlappingChars;		// the number of characters, at the end of this chunk, that overlap and 
								// will also be inserted at the beginning of the following chunk.
								
	//	Context  characters: These one character context data members are used by the rendering code in GDI
	//	to render punctuation marks based on the context. The context here represents the script in which to render the punctuation.
	TUint iContextChar;			//	One character of context stored per chunk. This context is either taken from the
											//	previous chunk (if it has context), or from the previous line (if its the first chunk
											//	in the current line). This value will hence move through the chunks ONLY IF IT IS NOT NULL.
											//	If it is NULL, then the context from the last non-NULL context chunk is taken. 
											//	See RTmTextCache::AdvanceWidthL() and CTmChunk::SetL()
	TUint iContextCharInByteCode;	//	One character of context stored per chunk to be stored in the byte code once the
											//	chunk has been formatted. This is different from iContextChar as this can be NULL,
											//	in which case it is NOT stored in the byte code. See RTmTextCache::AdvanceWidthL() and CTmline::WriteCodeL()
	};

/**
A line of text, including the chunks that make it up, in which direction
they flow and where it appears.

@internalComponent
*/
class CTmLine
	{
public:
	class TEllipsisInfo
		{
	public:
		TEllipsisInfo(): iParEndPos(KMaxTInt), iEllipsisFormat(0), iEllipsisWidth(0),
						 iAtLineEnd(EFalse), iAtParEnd(EFalse) { }
		TInt iParEndPos;			// end of paragraph
		TInt iEllipsisFormat;		// index of character position that provides ellipsis format
		TInt iEllipsisWidth;		// width of ellipsis if any
		TBool iAtLineEnd;			// true if the new chunk is at the end of a line
		TBool iAtParEnd;			// true if the new chunk is at the end of a paragraph
		};

	CTmLine();
	void FormatL(CTmFormatContext& aContext);
	void WriteCodeL(CTmFormatContext& aContext);
	TInt NextLineStartChar() const { return iNextLineStartChar; }
	TInt AtParEnd() const { return iAtParEnd; }
	TInt Height() const { return iHeight; }
	const TRect& InnerBounds() const { return iInnerBounds; }
	TUint GetContextChar() const { return iContextChar; }
	void SetContextChar(TUint aContext, TBool aFromChunk);
	

private:
	void BreakL(CTmFormatContext& aContext,TInt aStartXCoord,TInt aWrapXCoord);
	void WriteLabelCodeL(CTmFormatContext& aContext,MTmSource::TLabelType aLabelType);
	void WriteBdStateL(CTmFormatContext& aContext);
	void AppendChunksL(CTmFormatContext& aContext,TInt& aStartChar,TInt& aStartXPos,TInt aMaxChar,TInt aMaxXPos,
					   TBool aAllowSpaceForFinalInlineText,CTmLine::TEllipsisInfo& aEllipsisInfo);
	TBool AppendChunkL(CTmFormatContext& aContext,TInt& aStartChar,TInt& aStartXPos,TInt aMaxChar,TInt aMaxXPos,
					   TBool aAllowSpaceForFinalInlineText,CTmLine::TEllipsisInfo& aEllipsisInfo);
	void CalculateMetrics(CTmFormatContext& aContext,TInt& aWidth,TInt& aAscent,TInt& aDescent) const;
	void Justify(TInt aExtraSpace);
	void WriteInlineTextL(CTmFormatContext& aContext, CTmCode& aCode, TInt aPos, TInt aInlineWidth, TBool aLeadingEdge, TInt aInlineFormat);
	
	CTmBuffer<TText> iText;				// the text of the line
	CTmBuffer<TText> iTextWithoutChunkOverlaps;	// the text of the line without any duplicate overlapping characters, used for determining line-breaks
	CTmBuffer<TTmChunk> iChunk;			// the chunks of text
	CTmBuffer<TBidirectionalState::TRunInfo> iRunInfo;	// information about the bidirectionally reordered text

	TInt iCountedEndChar;				// line end, including only characters that fit the measure
	TInt iHangingEndChar;				// line end, including visible characters outside the measure if any
										// (hanging characters): >= iCountedEndChar
	TInt iEndChar;						// line end, including everything except text after a truncation with ellipsis:
										// >= iHangingEndChar
	TInt iNextLineStartChar;			// start of the next line; after undrawn characters if any; >= iEndChar
	TBool iAtParEnd;					// true if the line is at the end of a paragraph
	TBool iBordered;					// true if the line has border rules
	TInt iHeight;						// height of the line's outer bounds; the horizontal slice of the band
										// occupied by this line
	TRect iInnerBounds;					// bounds not including indents or space above and below the line or paragraph,
										// relative to 0,0 as the origin of the outer bounds
	TInt iAscent;						// distance between baseline and top of inner bounds
	TChar iEllipsis;					// if not 0xFFFF, append this ellipsis character
	TInt iEllipsisFormat;				// index of character position that provides ellipsis format
	TInt iEllipsisWidth;				// width of ellipsis if any
	
	//	Context  characters: These one character context data members are used by the rendering code in GDI
	//	to render punctuation marks based on the context. The context here represents the script in which to render the punctuation.
	TUint iContextChar;			//	One character of context stored per line to be used by the first chunk of the next line
											// 	as context. At the end of the formatting of the current line, this context is stored in CTmTextLayout
											//	so that the next line to be formatted can acquire this context. See CTmFormatContext::FormatL()
	};

/**
Context used during formatting.

@internalComponent
*/
NONSHARABLE_CLASS(CTmFormatContext) : public CBase
	{
public:
	/**
	Information returned by FormatL.
	@internalComponent
	*/
	class TInfo
	
		{
		public:
		TInt iWidth;				// actual width of formatted lines			
		TInt iHeight;				// actual height of formatted lines
		TInt iFirstLineEndChar;
		TInt iLastLineStartChar;
		TInt iEndChar;
		//	Context  characters: These one character context data members are used by the rendering code in GDI
		//	to render punctuation marks based on the context. The context here represents the script in which to render the punctuation.
		TUint iContextCharPerLine;	//	Temporary storage of one context character that represents the context for the last 
												//	line. Set from TTmByteCodeFinder::TInfo when found in the byte code and used when 
												//	formatting the current line. See CTmReformat::GenerateNewCodeL() and CTmFormatContext::FormatL()
		};
	static void FormatL(MTmSource& aSource,const TTmFormatParam& aFormatParam,CTmCode& aCode,TInfo& aInfo,
						CTmTextLayout* aTextLayout, const TBidirectionalContext* aStartBdState = 0,
						TBidirectionalContext* aEndBdState = 0,
						TBidirectionalEndOfLineContext* aStartBdContext = 0);
	MGraphicsDeviceMap& Device() { return iTextCache.Device(); }

	/**
	It is not generally useful to construct this: Use the static FormatL()
	instead.
	
	@internalComponent
	*/
	CTmFormatContext(MTmSource& aSource,const TTmFormatParam& aFormatParam,CTmCode& aCode,CTmTextLayout* aTextLayout,
					 const TBidirectionalContext* aBdContext = NULL);
	~CTmFormatContext();
private:
	void FormatL();
	void SetParagraphMetricsL();
	void ResolveParagraphDirectionality();

public:
	MTmSource& iSource;					// source of text and text attributes
										
										// Scaling parameters are used when formatting for one device and displaying
										// on another to produce a what-you-see-is-what-you-get ('wysiwyg') effect:
	TInt iXScale;						// amount by which to scale horizontal pixels in 1000ths
	TInt iYScale;						// amount by which to scale vertical pixels in 1000ths

	const TTmFormatParam& iParam;		// formatting parameters
	RTmTextCache iTextCache;			// gets the text and its format
	TBidirectionalState iBdState;		// the current bidirectional reordering state
	TBidirectionalEndOfLineContext iBdEndOfLine;
	TInt iDocumentLength;				// the document length
	TInt iActualEndChar;				// the minimum of the document length plus one
										// (to include the final paragraph delimiter) and iParam.iEndChar
	CTmCode& iCode;						// destination for the bytecode
	CTmLine iLine;						// the current line
	TInt iLineInPar;					// line number in the paragraph
	TInfo iInfo;						// information returned by the static FormatL function
	// Returned by CTmLine::BreakL
	TBool iLineEndsInForcedLineBreak;

	// the current paragraph format and pixel versions of dimensions from it.
	RTmParFormat iParFormat;
	TInt iLeftMargin;
	TInt iRightMargin;
	TInt iFirstLineLeftMargin;
	TInt iFirstLineRightMargin;
	TInt iLineSpacing;
	TBool iExactLineSpacing;
	TInt iSpaceBefore;
	TInt iSpaceAfter;
	TInt iHBorderMargin;
	TInt iVBorderMargin;
	TInt iLeftBorder;
	TInt iRightBorder;
	TInt iTopBorder;
	TInt iBottomBorder;
	TInt iBulletAscent;
	TInt iBulletDescent;
	CTmTextLayout* iTextLayout;
	};

#endif
