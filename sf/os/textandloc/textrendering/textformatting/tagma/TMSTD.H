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
* The standard internal header for TAGMA.
*
*/





#ifndef TMSTD_H__
#define TMSTD_H__

#include "TAGMA.H"
#include <txtfrmat.h>
#include <txtetext.h>
#include <fbs.h>
#include <bidi.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

static const TUint KZeroWidthJoiner = 0x200d;
static const TUint KZeroWidthNonJoiner = 0x200C;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CTmBufferBase) : public CBase

	{
	public:
	~CTmBufferBase();
	void Truncate(TInt aLength);
	void Reset() { iLength = 0; }
	TInt Length() const { return iLength; }

	protected:
	CTmBufferBase(TInt aExpandSize): iExpandSize(aExpandSize) { }
	TAny* Data() const { return iData; }
	TAny* ExtendL(TInt aSizeOfData);
	void DoAppendL(const TAny* aItem,TInt aCount,TInt aSizeOfData);

	private:
	void GrowL(TInt aExtra,TInt aSizeOfData);

	TAny* iData;
	TInt iLength;
	TInt iAllocated;
	TInt iExpandSize;
	};

/**
An expandable buffer.
@internalComponent
*/
template<class T> class CTmBuffer: public CTmBufferBase

	{
	public:
	CTmBuffer(TInt aExpandSize): CTmBufferBase(aExpandSize) { }
	void AppendL(const T& aItem) { *STATIC_CAST(T*,ExtendL(sizeof(T)))=aItem; }
	void AppendL(const T* aItem,TInt aCount) { DoAppendL(aItem,aCount,sizeof(T)); }
	T* Ptr() { return (T*)Data(); }
	const T* Ptr() const { return (const T*)Data(); }
	T& operator[](TInt aIndex) { return Ptr()[aIndex]; }
	};

/**
@internalComponent
*/
enum TTmPanic
	{
	EInvariant,
	ENoSource,
	EBadArg,
	EBadLineBreak,
	EFormatNotFound,
	EBadTruncationLength,
	EBadChunkType,
	EBadCodePos,
	EZeroLengthTextSupplied,
	EBadLineBreakRangeTable,
	ETextWidthBufferOverflow,
	ETextRunNotFound,
	EInvalidTextRunLength,
	EInvalidTextRunIndex,
	EUnimplemented,
	EParagraphFormatRequired,
	EIndexOutOfRange,
	EBadReturnValue,
	EBadPtrLen,
	EBadClusterBufLen,
	ENotImplemented
	};

/**
@internalComponent
*/
static inline void TmPanic(TTmPanic aPanic)
	{
	_LIT(KTmPanic,"Tagma");
	User::Panic(KTmPanic,aPanic);
	}

/**
Maximum size for text chunks, and hence the maximum size needed for buffers when measuring or drawing text.
@internalComponent
*/
const TInt KMaxTextChunkSize = 128;

class RTmTextCache;
class CTmTextFontCache
	{
public:
	static CTmTextFontCache* New(MGraphicsDeviceMap& aDevice, CFont& aFont);
	CFont& Font();
	void Open();
	void Close();
private:
	CTmTextFontCache(MGraphicsDeviceMap& aDevice, CFont& aFont);
	~CTmTextFontCache();
	TInt iRefCount;
	MGraphicsDeviceMap& iDevice;
	CFont& iFont;
	};

/**
A class for caching text and formats extracted from a text source.
That it is an R class shows that it must be closed after use; call Close().
@internalComponent
*/
class RTmTextCache
	{
public:
	enum TDisplayedTextDirectionality
		{
		// For requesting left-to-right text whether logical or visual
		ELeftToRight = 0,
		// For requesting right-to-left text in visual order
		EVisualRightToLeft = 1,
		// For requesting right-to-left text in logical order
		ELogicalRightToLeft = 2
		};
	RTmTextCache(MTmSource& aSource,MGraphicsDeviceMap& aDevice):
		iSource(aSource),
		iDevice(aDevice),
		iDocumentLength(aSource.DocumentLength()),
		iText(NULL),
		iTextStart(-1),
		iTextLength(0),
		iFont(NULL),
		iContextCharPerChunk(NULL),
		iContextCharInByteCode(NULL)
		{
		}
#ifdef _DEBUG
	~RTmTextCache() { if (iFont) TmPanic(EInvariant); }
#endif
 	TInt AdvanceWidthL(TInt aStart, TInt aEnd, TBool aRightToLeft,
 		TInt aMaxWidth = KMaxTInt, CFbsFont::TMeasureTextOutput* aOutput = 0,
 		TInt aExtraChar = 0);
 	TInt TotalWidthL(TInt aStart, TInt aEnd, TBool aRightToLeft);
	MTmSource& Source() { return iSource; }
	TInt GetText(TInt aPos,TInt aMaxEndChar,TPtrC& aText,TTmCharFormat* aFormat = NULL,CTmTextFontCache** aFont = NULL);
	TInt GetTextL(TInt aPos, TInt aMaxEndChar, TPtrC& aText,
		TTmCharFormat* aFormat = 0, CTmTextFontCache** aFont = 0);
	TInt GetDisplayedText(TInt aStart, TInt aEnd,
						  TDisplayedTextDirectionality aDirectionality, TText* aBuffer,TUint aContextChar,
						  TTmCharFormat* aFormat = 0, CTmTextFontCache** aFont = NULL);
	TUint Char(TInt aPos);
	const TTmCharFormat& Format() const { return iFormat; }
	MGraphicsDeviceMap& Device() { return iDevice; }
	void ReleaseFont() { if (iFont) { iFont->Close(); iFont = NULL; } }
	void Close() { ReleaseFont(); iTextBuffer.Close(); }

	TBool static IsArabicPoint(TInt aChar);
	void SetContextChar(TUint aContextChar);
	TUint GetContextChar() const { return iContextCharPerChunk; }
	TUint GetContextForByteCode() const { return iContextCharInByteCode; }
private:
	MTmSource& iSource;				// text source
	MGraphicsDeviceMap& iDevice;	// device used for getting fonts
	TInt iDocumentLength;			// length of the document, not including final paragraph delimiter if any
	const TText* iText;				// current text
	TInt iTextStart;				// start position of current text
	TInt iTextLength;				// length of current text
	TTmCharFormat iFormat;			// current text format
	CTmTextFontCache* iFont;		// if non-null, current font
	RBuf iTextBuffer;			// buffer for GetTextL (if appropriate)
	TInt iTextBufferStart;		// start position of where iTextBuffer is copied from
	TBool iTextBufferEndsInFormatChange;
	//	Context  characters: These one character context data members are used by the rendering code in GDI
	//	to render punctuation marks based on the context. The context here represents the script in which to render the punctuation.
	TUint iContextCharPerChunk;	//	One character of context stored temporarily to pass on to 
											//	the rendering code along with the text, and back to the TmChunk 
											//	object being rendered. See TmChunk::SetL() and RTmTextCache::AdvanceWidthL()
	TUint iContextCharInByteCode;	//	One character of context stored temporarily to pass on to
											//	the TmChunk object after rendering the text with context, to be 
											//	put into the byte code. See TmChunk::SetL() and RTmTextCache::AdvanceWidthL()
	};

/**
@internalComponent
*/
class TTmCodeReader

	{
	public:
	TTmCodeReader(const CTmCode& aCode,TInt aCodeStart,TInt aCodeEnd):
		iCode(aCode), iCodePos(aCodeStart), iCodeEndPos(aCodeEnd),
		iCodeSegPtr(NULL), iCodeSegEndPtr(NULL) { }
	const CTmCode& Code() const { return iCode; }
	TInt CodePos() const { return iCodePos; }
	TInt CodeEndPos() const { return iCodeEndPos; }
	TUint8 ReadByte() { if (iCodeSegPtr == iCodeSegEndPtr) SetCodePtr(); iCodePos++; return *iCodeSegPtr++; }
	TUint8 PeekByte() {	if (iCodeSegPtr == iCodeSegEndPtr) SetCodePtr(); return *iCodeSegPtr; }
	int ReadNumber();
	TRect ReadRect();
	void SetCodePos(TInt aNewCodePos);

	private:
	void SetCodePtr();

	const CTmCode& iCode;			// the bytecode
	TInt iCodePos;					// current index into the bytecode
	TInt iCodeEndPos;				// index to the end of the bytecode being interpreted
	const TUint8* iCodeSegPtr;		// pointer to the current position in the current bytecode segment
	const TUint8* iCodeSegEndPtr;	// end of the current bytecode segment
	};

/**
End-of-line information required for bidirectional reformatting.
@internalComponent
*/
class TBidirectionalEndOfLineContext
	{
public:
	TBidirectionalEndOfLineContext() { Reset(); }
	void Set(RTmTextCache& aText, TInt aStartPos);
	void Reset()
		{
		iFirstCategory = iFirstStrongCategory = TChar::EOtherNeutral;
		}
	void ExternalizeL(RWriteStream& aDest);
	void InternalizeL(RReadStream& aSource);
	TBool operator==(const TBidirectionalEndOfLineContext& aState) const
		{
		return iFirstCategory == aState.iFirstCategory
			&& iFirstStrongCategory == aState.iFirstStrongCategory;
		}
	TChar::TBdCategory FirstCategory() const { return iFirstCategory; }
	TChar::TBdCategory FirstStrongCategory() { return iFirstStrongCategory; }
private:
	TChar::TBdCategory iFirstCategory;
	TChar::TBdCategory iFirstStrongCategory;
	TInt iPositionOfLastStrongCategory;
	TInt iStartPosOfThisLine;
	};

/**
Extended TBidirectionalState for most situations in Tagma.
@internalComponent
*/
class TBidirectionalContext : public TBidirectionalState
	{
public:
	TBidirectionalContext() {}
	void Reset()
		{
		TBidirectionalState::Reset();
		iEndOfLine.Reset();
		}
	void InternalizeL(RReadStream& aSource);
	void Set(const TBidirectionalState& aState,
		const TBidirectionalEndOfLineContext& aEndOfLine)
		{
		*static_cast<TBidirectionalState*>(this) = aState;
		iEndOfLine = aEndOfLine;
		}
	TBool operator==(const TBidirectionalContext& aState) const
		{
		return *static_cast<const TBidirectionalState*>(this) == aState
			&& iEndOfLine == aState.iEndOfLine;
		}
	TBool ContextMatches(const TBidirectionalEndOfLineContext& aTest)
		{
		return aTest == iEndOfLine;
		}

private:
	TBidirectionalEndOfLineContext iEndOfLine;
	};

/**
@internalComponent
*/
void TmGetBorderWidth(const MGraphicsDeviceMap& aDevice,const RTmParFormat& aParFormat,RTmParFormat::TBorderIndex aSide,
					  TInt& aSingle,TInt& aFull);
void TmGetMarginsL(MGraphicsDeviceMap& aDevice,const RTmParFormat& aParFormat,
				   TInt& aLeftMargin,TInt& aRightMargin,
				   TInt& aFirstLineLeftMargin,TInt& aFirstLineRightMargin,
				   TInt& aBulletMargin,TInt& aBulletWidth,
				   TInt& aBulletAscent,TInt& aBulletDescent,CFont** aFont = NULL);
void SubtractRect(const TRect& aP,const TRect& aQ,TRect* aR);

#include "TMSTD.inl"

#endif // __TMSTD_H__
