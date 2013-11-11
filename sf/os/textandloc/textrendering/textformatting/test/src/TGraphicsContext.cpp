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
*
*/


#include "TGraphicsContext.h"
#include <e32std.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <graphics/gdi/gdiconsts.h>
#include <graphics/gdi/gdistructs.h>
#endif

_LIT(KTestFontName, "Non Functional Test Font");


// Utility functions to show contents of test data using test.Printf

extern void PrintTestData (const TDesC& aTitle , const TDesC16& aDataBuffer);

extern void PrintTestData(const TDesC& aTitle, const TText16* aDataBuffer, const TInt aSize);

//
//
// CLineArray
//
//
CLineArray::CLineArray() : iArrayIsEnabled(ETrue), iArray(0)
	{
	iPrev = iNext = this;
	}

void CLineArray::Null()
	{
	if (iNext == this && iArray)
		{
		iArray->Close();
		delete iArray;
		}
	iArray = 0;
	iPrev->iNext = iNext;
	iNext->iPrev = iPrev;
	iNext = iPrev = this;
	}

CLineArray::~CLineArray()
	{
	Null();
	}

void CLineArray::ConstructL(TInt aGranularity)
	{
	CLineArray::Null();
	iArray = new (ELeave) RArray<TTestGCDisplayLine>(aGranularity);
	}

void CLineArray::Copy(const CLineArray& aOther)
	{
	CLineArray::Null();
	iNext = &aOther;
	iPrev = aOther.iPrev;
	iPrev->iNext = this;
	aOther.iPrev = this;
	iArray = aOther.iArray;
	iArrayIsEnabled = aOther.iArrayIsEnabled;
	}

void CLineArray::ResetLineArray()
	{
	iArray->Reset();
	}

const TTestGCDisplayLine& CLineArray::Line(TInt aIndex)
	{
	return (*iArray)[aIndex];
	}

const TTestGCDisplayLine* CLineArray::Find(const TDesC& aText)
	{
	TInt count = LinesPresent();
	for (TInt i = 0; i != count; ++i)
		{		
		const TTestGCDisplayLine& line = Line(i);
		if (line.iLineData.Find(aText) != KErrNotFound)
			return &line;
		}
	return 0;
	}

void CLineArray::AddLineL(TTestGCDisplayLine& aLine)
	{
	if (iArrayIsEnabled)
		User::LeaveIfError(iArray->Append(aLine));
	}

TInt CLineArray::LinesPresent()
	{
	return iArray->Count();
	}

//
//
// CTestGraphicsDevice
//
//
CTestGraphicsDevice* CTestGraphicsDevice::NewL(TSize aSizeInPixels, RWsSession* aWsSession)
	{
	CTestGraphicsDevice* r = aWsSession?
		new (ELeave) CTestGraphicsDevice(aSizeInPixels, aWsSession)
		: new (ELeave) CTestGraphicsDevice(aSizeInPixels);
	r->iLineArray.ConstructL(4);
	if (aWsSession && KErrNone != r->Construct())
		{
		delete r;
		return 0;
		}
	return r;
	}

CTestGraphicsDevice::CTestGraphicsDevice(TSize aSizeInPixels, RWsSession* aWsSession)
	: CWsScreenDevice(*aWsSession)
	{
	Set(aSizeInPixels);
	}

CTestGraphicsDevice::CTestGraphicsDevice(TSize aSizeInPixels)
	{
	Set(aSizeInPixels);
	}

void CTestGraphicsDevice::Set(TSize aSizeInPixels)
	{
	iSize = aSizeInPixels;
	iHorizontalTwipsToPixels = 40;
	iVerticalTwipsToPixels = 40;
	iPalette.SetEntry(0, KRgbBlack);
	iPalette.SetEntry(1, KRgbWhite);
	iPalette.SetEntry(2, KRgbMagenta);
	iPalette.SetEntry(3, KRgbCyan);
	}

void CTestGraphicsDevice::SetHorizontalTwipsToPixels(TInt aTwipsToPixels)
	{
	iHorizontalTwipsToPixels = aTwipsToPixels;
	}

void CTestGraphicsDevice::SetVerticalTwipsToPixels(TInt aTwipsToPixels)
	{
	iVerticalTwipsToPixels = aTwipsToPixels;
	}

TDisplayMode CTestGraphicsDevice::DisplayMode() const
	{
	return EColor16M;
	}

TSize CTestGraphicsDevice::SizeInPixels() const
	{
	return iSize;
	}

TSize CTestGraphicsDevice::SizeInTwips() const
	{
	return TSize(iSize.iWidth * iHorizontalTwipsToPixels,
		iSize.iHeight * iVerticalTwipsToPixels);
	}

TInt CTestGraphicsDevice::CreateContext(CWindowGc*& aGC)
	{
	CTestGraphicsContext* r = new CTestGraphicsContext(this);
	if (!r)
		return KErrNoMemory;
	// only contruct if it is a fully-fledged Window Server thing
	if (iBuffer)
		r->Construct();
	aGC = r;
	return KErrNone;
	}

TInt CTestGraphicsDevice::CreateContext(CGraphicsContext*& aGC)
	{
	CWindowGc* p;
	TInt r = CreateContext(p);
	aGC = p;
	return r;
	}

TInt CTestGraphicsDevice::NumTypefaces() const
	{
	return 1;
	}

void CTestGraphicsDevice::TypefaceSupport(TTypefaceSupport& aTypefaceSupport, TInt aTypefaceIndex) const
	{
	// The only font we have at the moment is 10 pixels * 12 pixels for every character
	__ASSERT_ALWAYS(aTypefaceIndex == 0,
		CTestGraphicsContext::Panic(CTestGraphicsContext::ETypefaceIndexOutOfRange));
	aTypefaceSupport.iIsScalable = EFalse;
	aTypefaceSupport.iMaxHeightInTwips = iVerticalTwipsToPixels * 12;
	aTypefaceSupport.iMinHeightInTwips = iVerticalTwipsToPixels * 10;
	aTypefaceSupport.iNumHeights = 1;
	aTypefaceSupport.iTypeface.iName = KTestFontName;
	aTypefaceSupport.iTypeface.SetIsProportional(ETrue); // a bit of a lie
	aTypefaceSupport.iTypeface.SetIsSerif(EFalse);
	aTypefaceSupport.iTypeface.SetIsSymbol(EFalse);
	}

TInt CTestGraphicsDevice::FontHeightInTwips(TInt aTypefaceIndex, TInt aHeightIndex) const
	{
	// The only font we have at the moment is 10 pixels * 12 pixels for every character
	__ASSERT_ALWAYS(aTypefaceIndex == 0,
		CTestGraphicsContext::Panic(CTestGraphicsContext::ETypefaceIndexOutOfRange));
	return iVerticalTwipsToPixels * FontHeightInPixels(aTypefaceIndex, aHeightIndex);
	}

void CTestGraphicsDevice::PaletteAttributes(TBool& aModifiable, TInt& aNumEntries) const
	{
	aModifiable = ETrue;
	aNumEntries = 4;
	}

void CTestGraphicsDevice::SetPalette(CPalette* aPalette)
	{
	for (TInt i = 0; i != CTestPalette::KNumEntries; ++i)
		{
		TRgb col = aPalette->GetEntry(i);
		iPalette.SetEntry(i, col);
		}
	}

TInt CTestGraphicsDevice::GetPalette(CPalette*& aPalette) const
	{
	aPalette = const_cast<CTestPalette*>(&iPalette);
	return KErrNone;
	}

void CTestGraphicsDevice::GetPixel(TRgb& aColor, const TPoint&) const
	{
	aColor = KRgbWhite;
	}

void CTestGraphicsDevice::GetScanLine(TDes8&, const TPoint&, TInt, TDisplayMode) const
	{
	__ASSERT_DEBUG(0, CTestGraphicsContext::Panic(CTestGraphicsContext::EUnimplemented));
	}

TInt CTestGraphicsDevice::AddFile(const TDesC&, TInt&)
	{
	return KErrNotSupported;
	}

void CTestGraphicsDevice::RemoveFile(TInt)
	{
	}

TInt CTestGraphicsDevice::GetNearestFontInPixels(CFont*& aFont, const TFontSpec&)
	{
	CTestFont* font = new CTestFont();
	if (!font)
		return KErrNoMemory;
	aFont = font;
	return KErrNone;
	}

TInt CTestGraphicsDevice::FontHeightInPixels(TInt, TInt) const
	{
	return 12;
	}

TInt CTestGraphicsDevice::HorizontalTwipsToPixels(TInt aTwips) const
	{
	return aTwips / iHorizontalTwipsToPixels;
	}

TInt CTestGraphicsDevice::VerticalTwipsToPixels(TInt aTwips) const
	{
	return aTwips / iVerticalTwipsToPixels;
	}

TInt CTestGraphicsDevice::HorizontalPixelsToTwips(TInt aPixels) const
	{
	return aPixels * iHorizontalTwipsToPixels;
	}

TInt CTestGraphicsDevice::VerticalPixelsToTwips(TInt aPixels) const
	{
	return aPixels * iVerticalTwipsToPixels;
	}

TInt CTestGraphicsDevice::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	TFontSpec fontSpec = aFontSpec;
	fontSpec.iHeight = VerticalTwipsToPixels(fontSpec.iHeight);
	return GetNearestFontInPixels(aFont, fontSpec);
	}

void CTestGraphicsDevice::ReleaseFont(CFont* aFont)
	{
	__ASSERT_ALWAYS(aFont->TypeUid() == TUid::Uid(12345),
		CTestGraphicsContext::Panic(CTestGraphicsContext::EUnknownFont));
	delete static_cast<CTestFont*>(aFont);
	}

void CTestGraphicsDevice::AddRectToDrawnArea(const TRect& aRect, TBool aCondition)
	{
	// check if the drawing is on testing area
	if (!(iTestingArea.IsEmpty()) &&  	// if testing area has been set
		!iHasDrawnOnTestingArea)		// and nothing hasn't been drawn on it so far
		iHasDrawnOnTestingArea = iTestingArea.Intersects(aRect);
	
	if (iDrawnArea.IsEmpty())
		iDrawnArea = aRect;
	else
		iDrawnArea.BoundingRect(aRect);
	// only one condition at the moment
	if (aCondition)
		{
		if (iAreaDrawnWithCondition.IsEmpty())
			iAreaDrawnWithCondition = aRect;
		else
			iAreaDrawnWithCondition.BoundingRect(aRect);
		}
	}

void CTestGraphicsDevice::SetTestingArea(TRect& aTestingArea)
	{
	iTestingArea = aTestingArea;
	}

void CTestGraphicsDevice::AddTestingArea(TRect& moreTestingArea)
	{
	if (iTestingArea.IsEmpty())
		iTestingArea = moreTestingArea;
	else
		iTestingArea.BoundingRect(moreTestingArea);
	}

//
//
// CTestGraphicsContext
//
//
void CTestGraphicsContext::Panic(TInt aReason)
	{
	User::Panic(_L("CTestGC"), aReason);
	}

CTestGraphicsContext::CTestGraphicsContext(CTestGraphicsDevice* aGd)
	: CWindowGc(aGd), iGd(aGd), iDrawMode(EDrawModePEN), iPenSize(1,1)
	{
	iLineArray.Copy(aGd->LineArray());
	}

TInt CTestGraphicsContext::Construct()
	{
	return CWindowGc::Construct();
	}

void CTestGraphicsContext::AddRectToDrawnArea(const TRect& aRect)
	{
	TRect drawnRect = aRect;
	drawnRect.Grow(iPenSize);
	iGd->AddRectToDrawnArea(drawnRect,
		iDrawMode == EDrawModeXOR || iDrawMode == EDrawModeNOTSCREEN);
	}

void CTestGraphicsContext::AddPointToDrawnArea(const TPoint& aPoint)
	{
	AddRectToDrawnArea(TRect(aPoint, iPenSize));
	}

CGraphicsDevice* CTestGraphicsContext::Device() const
	{
	return iGd;
	}

void CTestGraphicsContext::SetOrigin(const TPoint& aPos)
	{
	iOrigin = aPos;
	}

void CTestGraphicsContext::SetDrawMode(TDrawMode aDrawingMode)
	{
	iDrawMode = aDrawingMode;
	}

void CTestGraphicsContext::SetClippingRect(const TRect& /*aRect*/)
	{
	}

void CTestGraphicsContext::CancelClippingRect()
	{
	}

TInt CTestGraphicsContext::SetClippingRegion(const TRegion& /*aRegion*/)
	{
	return KErrNone;
	}

void CTestGraphicsContext::CancelClippingRegion()
	{
	}

void CTestGraphicsContext::Reset()
	{
	iDrawMode = EDrawModePEN;
	iFont = 0;
	iPenSize.iWidth = 1;
	iPenSize.iHeight = 1;
	}

void CTestGraphicsContext::UseFont(const CFont* aFont)
	{
	iFont = aFont;
	}

void CTestGraphicsContext::DiscardFont()
	{
	iFont = 0;
	}

void CTestGraphicsContext::SetUnderlineStyle(TFontUnderline /*UnderlineStyle*/)
	{
	}

void CTestGraphicsContext::SetStrikethroughStyle(TFontStrikethrough /*aStrikethroughStyle*/)
	{
	}

void CTestGraphicsContext::SetWordJustification(TInt /*aExcessWidth*/,TInt /*aNumGaps*/)
	{
	}

void CTestGraphicsContext::SetCharJustification(TInt /*aExcessWidth*/,TInt /*aNumChars*/)
	{
	}

void CTestGraphicsContext::SetPenColor(const TRgb& aColor)
	{
	CPalette* palette;
	iGd->GetPalette(palette);
	iPenColorIndex = palette->NearestIndex(aColor);
	}

void CTestGraphicsContext::SetPenStyle(TPenStyle /*aPenStyle*/)
	{
	}

void CTestGraphicsContext::SetPenSize(const TSize& aSize)
	{
	iPenSize = aSize;
	}

void CTestGraphicsContext::SetBrushColor(const TRgb& /*aColor*/)
	{
	}

void CTestGraphicsContext::SetBrushStyle(TBrushStyle /*aBrushStyle*/)
	{
	}

void CTestGraphicsContext::SetBrushOrigin(const TPoint& /*aOrigin*/)
	{
	}

void CTestGraphicsContext::UseBrushPattern(const CFbsBitmap* /*aBitmap*/)
	{
	}

void CTestGraphicsContext::DiscardBrushPattern()
	{
	}

void CTestGraphicsContext::MoveTo(const TPoint& aPoint)
	{
	iCurrentPos = iOrigin + aPoint;
	}

void CTestGraphicsContext::MoveBy(const TPoint& aVector)
	{
	iCurrentPos += aVector;
	}

void CTestGraphicsContext::Plot(const TPoint& aPoint)
	{
	iCurrentPos = iOrigin + aPoint;
	AddPointToDrawnArea(iCurrentPos);
	}

void CTestGraphicsContext::DrawArc(const TRect& aRect,const TPoint& /*aStart*/,const TPoint& aEnd)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	AddRectToDrawnArea(r);
	iCurrentPos = iOrigin + aEnd;
	}

void CTestGraphicsContext::DrawLine(const TPoint& aPoint1,const TPoint& aPoint2)
	{
	AddPointToDrawnArea(iOrigin + aPoint1);
	iCurrentPos = iOrigin + aPoint2;
	AddPointToDrawnArea(iCurrentPos);
	}

void CTestGraphicsContext::DrawLineTo(const TPoint& aPoint)
	{
	AddPointToDrawnArea(iCurrentPos);
	iCurrentPos = iOrigin + aPoint;
	AddPointToDrawnArea(iCurrentPos);
	}

void CTestGraphicsContext::DrawLineBy(const TPoint& aVector)
	{
	AddPointToDrawnArea(iCurrentPos);
	iCurrentPos += aVector;
	AddPointToDrawnArea(iCurrentPos);
	}

void CTestGraphicsContext::DrawPolyLine(const CArrayFix<TPoint>* aPointList)
	{
	TInt num = aPointList->Count();
	while (num--)
		{
		iCurrentPos = iOrigin + (*aPointList)[num - 1];
		AddPointToDrawnArea(iCurrentPos);
		}
	}

void CTestGraphicsContext::DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)
	{
	while (aNumPoints--)
		{
		iCurrentPos = iOrigin + aPointList[aNumPoints - 1];
		AddPointToDrawnArea(iCurrentPos);
		}
	}

void CTestGraphicsContext::DrawPie(const TRect& aRect,
	const TPoint& /*aStart*/, const TPoint& aEnd)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	AddRectToDrawnArea(r);
	iCurrentPos = iOrigin + aEnd;
	}

void CTestGraphicsContext::DrawEllipse(const TRect& aRect)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	AddRectToDrawnArea(r);
	}

void CTestGraphicsContext::DrawRect(const TRect& aRect)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	AddRectToDrawnArea(r);
	}

void CTestGraphicsContext::DrawRoundRect(const TRect& aRect,const TSize& aCornerSize)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	r.Grow(aCornerSize);
	AddRectToDrawnArea(r);
	}

TInt CTestGraphicsContext::DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule /*aFillRule*/)
	{
	TInt num = aPointList->Count();
	while (num--)
		{
		iCurrentPos = iOrigin + (*aPointList)[num - 1];
		AddPointToDrawnArea(iCurrentPos);
		}
	return KErrNone;
	}

TInt CTestGraphicsContext::DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule /*aFillRule*/)
	{
	while (aNumPoints--)
		{
		iCurrentPos = iOrigin + aPointList[aNumPoints - 1];
		AddPointToDrawnArea(iCurrentPos);
		}
	return KErrNone;
	}

void CTestGraphicsContext::DrawBitmap(const TPoint& /*aTopLeft*/,const CFbsBitmap* /*aSource*/)
	{
	}

void CTestGraphicsContext::DrawBitmap(const TRect& /*aDestRect*/,const CFbsBitmap* /*aSource*/)
	{
	}

void CTestGraphicsContext::DrawBitmap(const TRect& /*aDestRect*/,const CFbsBitmap* /*aSource*/,const TRect& /*aSourceRect*/)
	{
	}

void CTestGraphicsContext::DrawText(const TDesC& aText, const TPoint& aPosition)
	{
#ifdef PRINT_DRAWTEXT_LINES

	_LIT(KDrawTextTitle, "Text being drawn");
		PrintTestData(KDrawTextTitle, aText);

#endif /* PRINT_DRAWTEXT_LINES */
	
	TTestGCDisplayLine thisLine;
	thisLine.Set(aPosition, aText);
	iLineArray.AddLineL(thisLine);
	}

void CTestGraphicsContext::DrawText(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,
	TTextAlign /*aAlignment*/, TInt aLeftMargin)
	{
	TPoint pos(aBox.iBr.iX + aLeftMargin, aBox.iTl.iY + aBaselineOffset);
	pos += iOrigin;
	DrawText(aText, pos);
	}
	
void CTestGraphicsContext::DrawText(const TDesC& aText,TTextParameters* aParam, const TPoint& aPosition)
	{
#ifdef PRINT_DRAWTEXT_LINES

	_LIT(KDrawTextTitle, "Text being drawn");
		PrintTestData(KDrawTextTitle, aText);

#endif /* PRINT_DRAWTEXT_LINES */

	//Avoid crash by ASSERT in BitGdi DrawText function
	__ASSERT_ALWAYS(aParam->iStart < aParam->iEnd,
		CTestGraphicsContext::Panic(CTestGraphicsContext::EErrorParameter));
	
	TTestGCDisplayLine thisLine;
	TPtrC actualText = aText.Mid(aParam->iStart,aParam->iEnd - aParam->iStart + 1);
	thisLine.Set(aPosition, actualText);
	iLineArray.AddLineL(thisLine);
	}

void CTestGraphicsContext::DrawText(const TDesC& aText,TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,
	TTextAlign /*aAlignment*/, TInt aLeftMargin)
	{
	TPoint pos(aBox.iBr.iX + aLeftMargin, aBox.iTl.iY + aBaselineOffset);
	pos += iOrigin;
	DrawText(aText,aParam, pos);
	}

TInt CTestGraphicsContext::APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput)
	{
	if (aUid == KDrawTextInContextUid)
		{
		TDrawTextInContextInternal* contextParam = (TDrawTextInContextInternal*)aInput;
		DrawText(contextParam->iText, &contextParam->iParam, contextParam->iPosition);
		return KErrNone;
		}
	else if (aUid == KDrawBoxTextInContextUid)
		{
		TDrawTextInContextInternal* contextParam = (TDrawTextInContextInternal*)aInput;
		DrawText(contextParam->iText,&contextParam->iParam,contextParam->iBox,contextParam->iBaselineOffset,contextParam->iAlign,contextParam->iMargin);
		return KErrNone;
		}
	// Future cases may be placed here later
	else
		return CWindowGc::APIExtension(aUid, aOutput, aInput);
	}


void CTestGraphicsContext::Clear()
	{
	}

void CTestGraphicsContext::Clear(const TRect& /*aRect*/)
	{
	}

void CTestGraphicsContext::CopyRect(const TPoint& /*aOffset*/, const TRect& /*aRect*/)
	{
	}

void CTestGraphicsContext::BitBlt(const TPoint& /*aPoint*/, const CFbsBitmap* /*aBitmap*/)
	{
	}

void CTestGraphicsContext::BitBlt(const TPoint& /*aPoint*/, const CFbsBitmap* /*aBitmap*/,
	const TRect& /*aRect*/)
	{
	}

void CTestGraphicsContext::BitBltMasked(const TPoint& /*aPoint*/, const CFbsBitmap* /*aBitmap*/,
	const TRect& /*aSourceRect*/, const CFbsBitmap* /*aMaskBitmap*/, TBool /*aInvertMask*/)
	{
	}

void CTestGraphicsContext::SetFaded(TBool)
	{
	}

void CTestGraphicsContext::SetFadingParameters(TUint8,TUint8)
	{
	}

//
//
// CTestFont
//
//
inline TBool IsSurrogate(TText a) { return 0xD800 == (a & 0xF800); }
inline TBool IsHighSurrogate(TText a) { return 0xD800 == (a & 0xFC00); }
inline TBool IsLowSurrogate(TText a) { return 0xDC00 == (a & 0xFC00); }
inline TChar PairSurrogates(TText aHigh, TText aLow)
	{
	return ((aHigh - 0xd7f7) << 10) + aLow;
	}

TUid CTestFont::DoTypeUid() const
	{
	return TUid::Uid(12345);
	}

TInt CTestFont::DoHeightInPixels() const
	{
	return 12;
	}

TInt CTestFont::DoAscentInPixels() const
	{
	return 10;
	}

struct TSpecialSizes
	{
	TInt iCharacter;
	TInt iWidth;
	};

static const TSpecialSizes KSpecialSizes[] =
	{
	{ CTestFont::KThreePerEmSpace, 3 },
	{ CTestFont::KThinSpace, 2 },
	{ CTestFont::KHairSpace, 1 },
	{ CTestFont::KZeroWidthSpace, 0 },
	{ CTestFont::KZeroWidthNoBreakSpace, 0 },
	};

TInt CTestFont::DoCharWidthInPixels(TChar aChar) const
	{
	TInt cn = aChar;
	// non-characters 0x??FFFE and 0x??FFFF
	if ((cn & 0xFFFE) == 0xFFFE)
		return 0;

	// Find character in the special sizes table
	TInt first = 0;
	TInt last = sizeof(KSpecialSizes)/sizeof(KSpecialSizes[0]);
	while (first != last)
		{
		TInt c = static_cast<TInt>(aChar);
		TInt mid = (first + last) >> 1;
		if (c < KSpecialSizes[mid].iCharacter)
			last = mid;
		else if (KSpecialSizes[mid].iCharacter < c)
			first = mid + 1;
		else
			return KSpecialSizes[mid].iWidth;
		}

	return 10;
	}

TInt CTestFont::DoTextCount(const TDesC& aText, TInt aWidthInPixels,
	TInt& aExcessWidthInPixels) const
	{
	const TText* p = &aText[0];
	const TText* pEnd = p + aText.Length();
	TInt total = 0;
	TInt prevSurrogate = 0;
	TInt charactersThatFit = 0;
	while (p != pEnd)
		{
		TChar c = *p;
		if (IsSurrogate(*p))
			{
			c = 0xFFFF;
			if (IsHighSurrogate(*p))
				prevSurrogate = *p;
			else if (prevSurrogate != 0)
				{
				c = PairSurrogates(static_cast<TText>(prevSurrogate), *p);
				prevSurrogate = 0;
				}
			}
		else
			prevSurrogate = 0;
		total += CharWidthInPixels(c);
		if (total <= aWidthInPixels)
			++charactersThatFit;
		++p;
		}
	aExcessWidthInPixels = total - aWidthInPixels;
	return charactersThatFit;
	}

TInt CTestFont::DoTextWidthInPixels(const TDesC& aText) const
	{
	TInt excess;
	TextCount(aText, 0, excess);
	return excess;
	}

TInt CTestFont::DoBaselineOffsetInPixels() const
	{
	return 10;
	}

TInt CTestFont::DoTextCount(const TDesC& aText,TInt aWidthInPixels) const
	{
	TInt excess;
	return TextCount(aText, aWidthInPixels, excess);
	}

TInt CTestFont::DoMaxCharWidthInPixels() const
	{
	return 10;
	}

TInt CTestFont::DoMaxNormalCharWidthInPixels() const
	{
	return 10;
	}

TFontSpec CTestFont::DoFontSpecInTwips() const
	{
	return TFontSpec(KTestFontName, 12);
	}
 
CFont::TCharacterDataAvailability 
CTestFont::DoGetCharacterData(TUint aCode, TOpenFontCharMetrics& aMetrics,
	const TUint8*& aBitmap, TSize& aBitmapSize) const
	{
	TInt width;
	switch (aCode)
		{
	case 0x001B:
		// ESC character should cause this fault; no character data available.
		return CFont::ENoCharacterData;
	case 'W':
		// We want 'W' to have side-bearings
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		width = aMetrics.Width();
		aMetrics.SetHorizBearingX(-1);
		aMetrics.SetWidth(width + 2);
		return CFont::ECharacterWidthOnly ;
	case '/':
		// We want / to have a left side-bearing
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		width = aMetrics.Width();
		aMetrics.SetHorizBearingX(-1);
		aMetrics.SetWidth(width + 1);
		return CFont::ECharacterWidthOnly ;
	case 'D':
		// We want 'D' to have a left side-bearing only
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		aMetrics.SetHorizBearingX(-1);
		return CFont::ECharacterWidthOnly ;
	case KTav:
		// We want Hebrew Tav to have a +ve left side-bearing
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		aMetrics.SetHorizBearingX(1);
		return CFont::ECharacterWidthOnly ;
	case KFullWidthSolidus:
		// We want fw/ to have a right side-bearing
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		width = aMetrics.Width();
		aMetrics.SetWidth(width + 1);
		return CFont::ECharacterWidthOnly ;
	case KArabicWaw:
		// Arabic Waw-- has massive left side-bearing
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		width = aMetrics.Width();
		aMetrics.SetHorizBearingX(-5);
		aMetrics.SetWidth(width + 5);
		return CFont::ECharacterWidthOnly ;
	default:
		return CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		}
	}
	
