/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Simple stubs for testing purposes
*
*/


#ifndef __TGRAPHICSCONTEXT_H__
#define __TGRAPHICSCONTEXT_H__

#include <w32std.h>

/** Representing a line of displayed data.
@internalComponent */
class TTestGCDisplayLine 
	{
public:
	TPoint iLinePos;
	TBuf16<80> iLineData;

	TTestGCDisplayLine() : iLinePos(0,0) {}
	TTestGCDisplayLine(const TInt aX, const TInt aY, const TDesC16& aLineData) : iLinePos(aX, aY) 
		{
		iLineData = aLineData;
		}
	TTestGCDisplayLine(const TPoint aLinePos, const TDesC16& aLineData) : iLinePos(aLinePos) 
		{
		iLineData = aLineData;
		}

	void Set(const TPoint& aLinePos, const TDesC16& aLineData)
		{
		iLineData = aLineData;
		iLinePos = aLinePos;
		}

	TPtrC16 LineData() const
		{
		return TPtrC16(iLineData.Ptr(), iLineData.Length());
		}

	const TPoint& Position() const
		{
		return iLinePos;
		}
	};

/** Holds a reference-counted array of TTestGCDisplayLines. Copy makes another
reference to the same array, allowing arrays to be shared between graphics
contexts and devices.
@internalComponent
*/
class CLineArray : public CBase
	{
public:
	CLineArray();
	~CLineArray();
	void ConstructL(TInt aGranularity);
	/** Make this refer to the same array as aOther currently does. */
	void Copy(const CLineArray& aOther);
	/** Removes this reference to the array. Deletes the array if all
	references have been removed. */
	void Null();
	void ResetLineArray();
	const TTestGCDisplayLine& Line(TInt aIndex);
	void AddLineL(TTestGCDisplayLine& aLine);
	TInt LinesPresent();
	/** Finds a TTestGCDisplayLine with aText contained somewhere in its data.
	Returns 0 if there is no such line. */
	const TTestGCDisplayLine* Find(const TDesC& aText);
	/** Disables any more adding to the line array, and disables it
	in any CLineArrays copied from this one. */
	void Disable() { iArrayIsEnabled = EFalse; }
	/** Re-enables adding to the line array, and in any CLineArrays copied from
	this one. */
	void Enable() { iArrayIsEnabled = ETrue; }
private:
	// CLineArrays copied from one another form a circular doubly-linked
	// list. This is roughly equivalent to reference-counting; when iPrev and
	// iNext point to 'this', this is the same as the reference count being 1.
	mutable const CLineArray* iPrev;
	mutable const CLineArray* iNext;
	TBool iArrayIsEnabled;
	RArray<TTestGCDisplayLine>* iArray;
	};

class CTestPalette : public CPalette
	{
public:
	enum { KNumEntries = 4 };
	CTestPalette()
		{
		iArray = iRgbArray;
		iNumEntries = KNumEntries;
		}
	~CTestPalette()
		{
		// Stop iRgbArray from being deleted by ~CPalette.
		// Ideally, iRgbArray should be removed in favour of
		// allocating one on the heap.
		iArray = 0;
		}
private:
	TRgb iRgbArray[KNumEntries];
	};

class CTestGraphicsDevice : public CWsScreenDevice
	{
public:
	static CTestGraphicsDevice* NewL(TSize aSizeInPixels, RWsSession* aWsSession);
	CTestGraphicsDevice(TSize aSizeInPixels, RWsSession* aWsSession);
	CTestGraphicsDevice(TSize aSizeInPixels);
	void SetHorizontalTwipsToPixels(TInt aTwipsToPixels);
	void SetVerticalTwipsToPixels(TInt aTwipsToPixels);
	TDisplayMode DisplayMode() const;
	TSize SizeInPixels() const;
	TSize SizeInTwips() const;
	TInt CreateContext(CGraphicsContext*& aGC);
	TInt CreateContext(CWindowGc*& aGC);
	TInt NumTypefaces() const;
	void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const;
	TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const;
	void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const;
	void SetPalette(CPalette* aPalette);
	TInt GetPalette(CPalette*& aPalette) const;

	// from CBitmapDevice
	void GetPixel(TRgb& aColor,const TPoint& aPixel) const;
	void GetScanLine(TDes8& aBuf,const TPoint& aStartPixel,TInt aLength,TDisplayMode aDispMode) const;
	TInt AddFile(const TDesC& aName,TInt& aId);
	void RemoveFile(TInt aId);
	TInt GetNearestFontInPixels(CFont*& aFont,const TFontSpec& aFontSpec);
	TInt FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const;

	// from MGraphicsDeviceMap from CBitmapDevice
	TInt HorizontalTwipsToPixels(TInt aTwips) const;
	TInt VerticalTwipsToPixels(TInt aTwips) const;
	TInt HorizontalPixelsToTwips(TInt aPixels) const;
	TInt VerticalPixelsToTwips(TInt aPixels) const;
	TInt GetNearestFontInTwips(CFont*& aFont,const TFontSpec& aFontSpec);
	void ReleaseFont(CFont* aFont);

	// new functions
	void ClearDrawnArea()
		{
		iDrawnArea.iTl.iX = iDrawnArea.iBr.iX = iDrawnArea.iTl.iY = iDrawnArea.iBr.iY = 0;
		}
	TRect DrawnArea() const
		{
		return iDrawnArea;
		}
	void ClearAreaDrawnWithCondition()
		{
		iAreaDrawnWithCondition.iTl.iX = iAreaDrawnWithCondition.iBr.iX
			= iAreaDrawnWithCondition.iTl.iY = iAreaDrawnWithCondition.iBr.iY = 0;
		}
	const TRect& AreaDrawnWithCondition() const
		{
		return iAreaDrawnWithCondition;
		}
	void AddRectToDrawnArea(const TRect& aRect, TBool aCondition);
	CLineArray& LineArray() { return iLineArray; }
	void SetLineArray(const CLineArray& aLineArray)
		{
		iLineArray.Copy(aLineArray);
		}
	
	// new functions for testing area
	void SetTestingArea(TRect& aTestingArea);
	void AddTestingArea(TRect& moreTestingArea);
	void ClearTestingArea()
		{
		iTestingArea.iTl.iX = iTestingArea.iBr.iX
			= iTestingArea.iTl.iY = iTestingArea.iBr.iY = 0;
		}
	// check if anything has been drawn on testing area
	inline TBool HasDrawnOnTestingArea()
		{
		return iHasDrawnOnTestingArea;
		}
	
private:
	void Set(TSize aSizeInPixels);

	// a rect of testing area
	TRect iTestingArea;
	TBool iHasDrawnOnTestingArea;
	
	TRect iDrawnArea;
	TRect iAreaDrawnWithCondition;
	TSize iSize;
	TInt iHorizontalTwipsToPixels;
	TInt iVerticalTwipsToPixels;
	CTestPalette iPalette;
	CLineArray iLineArray;
	};

class CTestGraphicsContext : public CWindowGc
	{
public:
	enum TPanic
		{
		EUnimplemented,
		ETypefaceIndexOutOfRange,
		EUnknownFont,
		EErrorParameter
		};
	static void Panic(TInt aReason);
	CTestGraphicsContext(CTestGraphicsDevice* aGd);
	TInt Construct();

	// from CBitmapContext
	CGraphicsDevice* Device() const;
	void SetOrigin(const TPoint& aPos);
	void SetDrawMode(TDrawMode aDrawingMode);
	void SetClippingRect(const TRect& aRect);
	void CancelClippingRect();
	void Reset();
	void UseFont(const CFont* aFont);
	void DiscardFont();
	void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
	void SetWordJustification(TInt aExcessWidth,TInt aNumGaps);
	void SetCharJustification(TInt aExcessWidth,TInt aNumChars);
	void SetPenColor(const TRgb& aColor);
	void SetPenStyle(TPenStyle aPenStyle);
	void SetPenSize(const TSize& aSize);
	void SetBrushColor(const TRgb& aColor);
	void SetBrushStyle(TBrushStyle aBrushStyle);
	void SetBrushOrigin(const TPoint& aOrigin);
	void UseBrushPattern(const CFbsBitmap* aBitmap);
	void DiscardBrushPattern();
	void MoveTo(const TPoint& aPoint);
	void MoveBy(const TPoint& aVector);
	void Plot(const TPoint& aPoint);
	void DrawArc(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
	void DrawLine(const TPoint& aPoint1,const TPoint& aPoint2);
	void DrawLineTo(const TPoint& aPoint);
	void DrawLineBy(const TPoint& aVector);
	void DrawPolyLine(const CArrayFix<TPoint>* aPointList);
	void DrawPolyLine(const TPoint* aPointList,TInt aNumPoints);
	void DrawPie(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
	void DrawEllipse(const TRect& aRect);
	void DrawRect(const TRect& aRect);
	void DrawRoundRect(const TRect& aRect,const TSize& aCornerSize);
	TInt DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule);
	TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule);
	void DrawBitmap(const TPoint& aTopLeft,const CFbsBitmap* aSource);
	void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource);
	void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource,const TRect& aSourceRect);
	void DrawText(const TDesC& aText,const TPoint& aPosition);
	void DrawText(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TTextAlign aAlignment,
		TInt aLeftMargin);
	TInt SetClippingRegion(const TRegion &aRegion);
	void CancelClippingRegion();

	// from CBitmapContext
	void Clear();
	void Clear(const TRect& aRect);
	void CopyRect(const TPoint& aOffset,const TRect& aRect);
	void BitBlt(const TPoint& aPoint,const CFbsBitmap* aBitmap);
	void BitBlt(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aRect);
	void BitBltMasked(const TPoint& aPoint, const CFbsBitmap* aBitmap,
		const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask);
	void SetFaded(TBool aFaded);
	void SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap);

	// new functions
	void AddRectToDrawnArea(const TRect& aRect);
	void AddPointToDrawnArea(const TPoint& aPoint);
	
	void DrawText(const TDesC& aText,TTextParameters* aParam,const TPoint& aPosition);
	void DrawText(const TDesC& aText,TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TTextAlign aAlignment,
		TInt aLeftMargin);
protected:	
	TInt APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput);

private:
	CTestGraphicsDevice* iGd;
	TPoint iOrigin;
	TDrawMode iDrawMode;
	const CFont* iFont;
	TSize iPenSize;
	TPoint iCurrentPos;
	TInt iPenColorIndex;
	CLineArray iLineArray;
	};

class CTestFont : public CFont
	{
public:
	/**
	Sample Unicode characters.
	@internalComponent
	*/
	enum
		{
 		/** ESC has no data at all */
 		KEsc = 0x001B,
 		/** Hebrew letter Tav, has right side-bearing */
 		KTav = 0x05EA,
 		/** Arabic Waw has a massive left side-bearing */
 		KArabicWaw = 0x0648,
   		/** ZWNBS has zero width. */
   		KZeroWidthNoBreakSpace = 0xFEFF,
   		/** ZWS has zero width. */
   		KZeroWidthSpace = 0x200B,
   		/** Hair space has one pixel width. */
   		KHairSpace = 0x200A,
   		/** Thin space has two pixel width. */
   		KThinSpace = 0x2009,
   		/** 3-per-em space has three pixel width. */
   		KThreePerEmSpace = 0x2004,
   		/** Full-width solidus has a right side-bearing */
   		KFullWidthSolidus = 0xFF0F
   		};
   	TUid DoTypeUid() const;
   	TInt DoHeightInPixels() const;
   	TInt DoAscentInPixels() const;
   	TInt DoCharWidthInPixels(TChar aChar) const;
   	TInt DoTextWidthInPixels(const TDesC& aText) const;
   	TInt DoBaselineOffsetInPixels() const;
   	TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels) const;
   	TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels,TInt& aExcessWidthInPixels) const;
   	TInt DoMaxCharWidthInPixels() const;
   	TInt DoMaxNormalCharWidthInPixels() const;
   	TFontSpec DoFontSpecInTwips() const;
 	TCharacterDataAvailability DoGetCharacterData(TUint aCode,
 		TOpenFontCharMetrics& aMetrics,
 		const TUint8*& aBitmap, TSize& aBitmapSize) const;
	};

#endif
