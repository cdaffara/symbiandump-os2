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
*
*/


#include "TAGMA.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

/**
@internalComponent
*/
template<class T> class RRefCountedArray

	{
public:
	void Close() { iArray.Close(); }
	TInt Insert(const T& aEntry,TInt& aIndex);
	void Remove(TInt aIndex);
	TInt Count() const { return iArray.Count(); }
	T& operator[](TInt aIndex) { return iArray[aIndex].iEntry; }
	const T& operator[](TInt aIndex) const { return iArray[aIndex].iEntry; }
	void Reset() { iArray.Reset(); }
	TInt MemoryUsed() const;

private:
	class TCountedEntry
		{
	public:
		TInt iRefCount;
		T iEntry;
		};

	RArray<TCountedEntry> iArray;
	};

/**
 * Implementation for the CTmText class.
 * @internalComponent
 */
NONSHARABLE_CLASS(CTmTextImp) : public CBase, public MTmSource
	{
public:
	CTmTextImp(MGraphicsDeviceMap& aDevice,const TTmFormatParamBase& aFormatParam):
		iDevice(aDevice),
		iFormatParam(aFormatParam)
		{
		}
	~CTmTextImp();
	void InsertL(TInt aPos, const TDesC& aText,
		const TTmCharFormat* aCharFormat, const RTmParFormat* aParFormat,
		TRect* aRedrawRect, TInt* aScroll);
	void GetFormat(TTmFormatParamBase& aFormatParam) const { aFormatParam = iFormatParam; }
	void ChangeFormatL(const TTmFormatParamBase& aFormatParam);
	void SetWrapWidthL(TInt aWrapWidth) { iFormatParam.iWrapWidth = aWrapWidth; ReformatL(); }
	void Clear();
	void CustomizeL(const MTmCustom* aCustom);
	const CTmTextLayout& TextLayout() const { return iTextLayout; }
	TInt MemoryUsed() const;

	void ReformatL();

	// implementations of MTmSource virtual functions
	MGraphicsDeviceMap& FormatDevice() const;
	MGraphicsDeviceMap& InterpretDevice() const;
	TInt DocumentLength() const;
	void GetText(TInt aPos,TPtrC& aText,TTmCharFormat& aFormat) const;
	void GetParagraphFormatL(TInt aPos,RTmParFormat& aFormat) const;
	TInt ParagraphStart(TInt aPos) const;
	TRgb SystemColor(TUint aColorIndex,TRgb aDefaultColor) const;
	TInt Stretch(TUint aChar) const;
	TUint Map(TUint aChar) const;
	void SetLineHeight(const TLineHeightParam& aParam,TInt& aAscent,TInt& aDescent) const;
	void DrawBackground(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
						const TLogicalRgb& aBackground,TRect& aRectDrawn) const;
	void DrawLineGraphics(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
						  const TTmLineInfo& aLineInfo) const;
	void DrawText(CGraphicsContext& aGc,const TPoint& aTextLayoutTopLeft,const TRect& aRect,
				  const TTmLineInfo& aLineInfo,const TTmCharFormat& aFormat,
				  const TDesC& aText,const TPoint& aTextOrigin,TInt aExtraPixels) const;

	/**
	A run of text with the same attribute.
	@internalComponent
	*/
	class TRun
		{
	public:
		TInt iIndex;		// index to the attribute
		TInt iLength;		// length of the text
		};

	/**
	An array of runs.
	@internalComponent
	*/
	class RRunArray
		{
	public:
		TInt Insert(TInt aPos,TInt aLength,TInt aIndex);
		TInt Set(TInt aPos,TInt aLength,TInt aIndex);
		void Delete(TInt aPos,TInt aLength);
		TInt Index(TInt aPos) const;
		TInt RunAndStartPos(TInt aPos, TRun& aRunOut) const;
		void Close() { iRun.Close(); }
		void Reset() { iRun.Reset(); }
		TInt MemoryUsed() const;

		TInt SplitRun(TInt aRunIndex,TInt aOffset);
		void FindRun(TInt aPos,TInt& aRunIndex,TInt& aOffset) const;
		TInt FindRunNonstrict(TInt aPos,TInt& aRunIndex,TInt& aOffset) const;

	private:
		RArray<TRun> iRun;
		};

	/**
	A buffer to store the text.
	@internalComponent
	*/
	class CTextBuffer: private CTmBufSeg
		{
	public:
		CTextBuffer(): CTmBufSeg(128 * sizeof(TText)) { }
		void InsertL(TInt aPos,const TDesC& aText)
			{ CBufSeg::InsertL(aPos * sizeof(TText),aText.Ptr(),aText.Length() * sizeof(TText)); }
		/*
		Don't rename the following function Delete because there is a virtual Delete with different semantics
		that is called by CBufSeg::Reset.
		*/
		void DeleteText(TInt aPos,TInt aLength)
			{ CBufSeg::Delete(aPos * sizeof(TText),aLength * sizeof(TText)); }
		TInt Length() const { return CBufSeg::Size() / sizeof(TText); }
		TPtrC PtrC(TInt aPos) const
			{ TPtrC8 p = CONST_CAST(CTextBuffer*,this)->CBufSeg::Ptr(aPos * sizeof(TText));
			  return TPtrC((TText*)p.Ptr(),p.Length() / sizeof(TText)); }
		TPtrC BackPtrC(TInt aPos) const
			{ TPtrC8 p = CONST_CAST(CTextBuffer*,this)->CBufSeg::BackPtr(aPos * sizeof(TText));
			  return TPtrC((TText*)p.Ptr(),p.Length() / sizeof(TText)); }
		void Reset() { CBufSeg::Reset(); }
		TInt MemoryUsed() const { return CTmBufSeg::MemoryUsed(); }
		};

	/**
	A TCharFormat usable in RRefCountedArray
	@internalComponent
	*/
	class TMyCharFormat: public TTmCharFormat
		{
	public:
		TInt Copy(const TMyCharFormat& aCharFormat) { *this = aCharFormat; return KErrNone; }
		};

	/**
	A CParaFormat usable in RRefCountedArray
	@internalComponent
	*/
	class RMyParFormat: public RTmParFormat
		{
	public:
		TInt Copy(const RMyParFormat& aParFormat)
			{
			TRAPD(error,CopyL(aParFormat));
			if (error)
				Close();
			return error;
			}
		};

private:
	MGraphicsDeviceMap& iDevice;					// the device used for formatting and drawing
	TTmFormatParamBase iFormatParam;				// formatting parameters: wrap width, etc.
	CTextBuffer iText;								// the text
	RRunArray iCharFormatRun;						// character format runs
	RRunArray iParFormatRun;						// paragraph format runs
	RRefCountedArray<TMyCharFormat> iCharFormat;	// the character formats
	RRefCountedArray<RMyParFormat> iParFormat;		// the paragraph formats
	CTmTextLayout iTextLayout;						// the text layout
	const MTmCustom* iCustom;						// if non-null, layout and drawing customisation
	};
