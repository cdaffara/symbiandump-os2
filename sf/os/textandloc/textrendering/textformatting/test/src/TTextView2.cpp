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
* @file
* @internalComponent 
*
*/


#include "TCustomWrap.h"
#include "TGraphicsContext.h"
#include "TBitmapDoc.h"
#include "TestPicture.h"
#include <e32std.h>
#include <e32test.h>
#include <frmtlay.h>
#include <frmtview.h>
#include <txtlaydc.h>
#include <fbs.h>
#include <w32std.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

namespace LocalToFile {

_LIT(KTTextView2, "TTextView2");
const TInt KDisplayWidth = 100;
const TInt KDisplayHeight = 100;
const TInt KAleph = 0x5D0;
const TInt KPictureCharacter = 0xFFFC;
const TInt KRightToLeftMarker = 0x200F;
const TInt KZeroWidthNonJoiner = 0x200C;
RTest test(KTTextView2);

/** Simple test picture. */
class CPinkSquare : public CPicture
	{
public:
	// Size of square in twips.
	// 600 is 15 pixels using the standard test graphics device at
	// its default resolution.
	enum { KWidth = 600, KHeight = 600 };
	CPinkSquare() {}
	void Draw(CGraphicsContext& aGc, const TPoint& aTopLeft,
		const TRect& aClipRect, MGraphicsDeviceMap* aMap) const
		{
		// This picture is a magenta square
		TPoint size(KWidth, KHeight);
		if (aMap)
			size = aMap->TwipsToPixels(size);
		TRect rect(aTopLeft, aTopLeft + size);
		aGc.SetClippingRect(aClipRect);
		aGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
		aGc.SetPenColor(KRgbMagenta);
		aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc.SetBrushColor(KRgbMagenta);
		aGc.DrawRect(rect);
		}
	void ExternalizeL(RWriteStream&) const {}
	void GetOriginalSizeInTwips(TSize& a) const
		{
		a.iWidth = CPinkSquare::KWidth;
		a.iHeight = CPinkSquare::KHeight;
		}
	};

_LIT(KEnd, "\x2029");
/** Lightweight test document model. */
class TDocModel : public MLayDoc
	{
public:
	TDocModel(const TDesC& aDes)
		: iDes(&aDes), iParagraphFormat(0), iBreakPos(0) {}
	void SetParagraphFormat(CParaFormat* a)
		{
		iParagraphFormat = a;
		}
	/** Sets the segmentation of the buffer to cut at this point.
	aParam a 0 means no break at all. */
	void SetBreakPos(TInt a)
		{
		iBreakPos = a;
		}
	// From MLayDoc
	TInt LdDocumentLength() const { return iDes->Length(); }
	TInt LdToParagraphStart(TInt& a) const
		{
		TInt curr = a;
		if (a < LdDocumentLength())
			{
			a = iDes->Left(a).LocateReverse(0x2029);
			a = a < 0? 0 : a + 1;
			}
		return curr - a;
		}
	void GetParagraphFormatL(CParaFormat* aFormat, TInt) const
		{
		if (iParagraphFormat)
			{
			aFormat->CopyL(*iParagraphFormat);
			return;
			}
		aFormat->Reset();
		TTabStop tabStop;
		tabStop.iTwipsPosition = 1000;
		tabStop.iType = TTabStop::ELeftTab;
		aFormat->StoreTabL(tabStop);
		}
	void GetChars(TPtrC& aView,TCharFormat& aFormat, TInt aStartPos)const
		{
		TCharFormat cf;
		aFormat = cf;
		TInt docLength = LdDocumentLength();
		if (aStartPos < iBreakPos && iBreakPos < docLength)
			aView.Set(iDes->Mid(aStartPos, iBreakPos - aStartPos));
		else if (aStartPos == docLength)
			aView.Set(KEnd);
		else
			aView.Set(iDes->Mid(aStartPos));
		}
	TInt GetPictureSizeInTwips(TSize& aSize, TInt aPos) const
		{
		if ((*iDes)[aPos] != KPictureCharacter)
			return KErrNotFound;
		aSize.iWidth = CPinkSquare::KWidth;
		aSize.iHeight = CPinkSquare::KHeight;
		return KErrNone;
		}
	CPicture* PictureHandleL(TInt aPos, TForcePictureLoad) const
		{
		if ((*iDes)[aPos] != KPictureCharacter)
			return 0;
		return new(ELeave) CPinkSquare;
		}
	TBool EnquirePageBreak(TInt aPos, TInt aLength)const
		{
		return iDes->Mid(aPos, aLength).Locate(0x000C) < 0?
			EFalse : ETrue;
		}
	TBool SelectParagraphLabel(TInt) { return EFalse; }
	void CancelSelectLabel() {}
private:
	const TDesC* iDes;
	CParaFormat* iParagraphFormat;
	TInt iBreakPos;
	};
}

// Remaps whatever character is used in construction to the letter 'E'
class TTestCustomRemap : public MFormCustomInvisibleCharacterRemapper
	{
public:
	TTestCustomRemap(TUint aChar)
		{
		iChar = aChar;
		}
	TUint Remap(TUint aChar, const TNonPrintingCharVisibility aNonPrintingCharVisibility,
		const TLayDocTextSource& aLayDoc)
		{
	    if (aChar == iChar)
	        {
			return 'E';
	        }
	    else
	    	{
		    return DefaultMapping(aChar, aNonPrintingCharVisibility, aLayDoc);
	    	}
		}
private:
	TUint iChar;
	};


// Remaps all break characters to the letter 'E'
class TTestCustomRemapper : public MFormCustomInvisibleCharacterRemapper
	{
public:
	TUint Remap(TUint aChar, const TNonPrintingCharVisibility aNonPrintingCharVisibility,
		const TLayDocTextSource& aLayDoc)
		{
	    switch (aChar)
	        {
	        case CEditableText::ELineBreak:
	        case CEditableText::EParagraphDelimiter:
	        case CEditableText::EPageBreak:
				return 'E';

	        default:
	            break; // do nothing
	        }

	    return DefaultMapping(aChar, aNonPrintingCharVisibility, aLayDoc);
		}
	};

using namespace LocalToFile;

class CTestTextView
	{
public:
	static void SetOffScreenContext(CTextView* aView, CBitmapContext* aContext)
		{
		aView->iOffScreenContext = aContext;
		}
	static TRect GetInvalidRect(CTextView* aView)
		{
		return aView->iDisplay.ClippingRect();
		}
	static int GetFormattedHeight(CTextView* aView)
		{
		return aView->iFormattedUpTo;
		}
	static void TestMemberOffsetsL()
		{
		// The members of these classes must not move due to inline method offsets.
		// Any of the following tests failing will indicate a BC break.
		test(_FOFF(CTextView, iLayout)==64);
		test(_FOFF(CTextView, iCursorPos)==192);
		test(_FOFF(CTextView, iFlags)==240);
		test(_FOFF(CTextView, iObserver)==264);
		}
	};

class CTestTextLayout
	{
public:
	static void TestMemberOffsetsL()
		{
		// The members of these classes must not move due to inline method offsets.
		// Any of the following tests failing will indicate a BC break.
		test(_FOFF(CTextLayout, iText)==4);
		test(_FOFF(CTextLayout, iBandTop)==104);
		test(_FOFF(CTextLayout, iScrollFlags)==116);
		test(_FOFF(CTextLayout, iHighlightExtensions)==128);
		}
	};

// Test for TET-5D7MCV: Sound object focus indicators flickering
_LIT(KPicture, "\xFFFC");
_LIT(KInitialText, "xy");

void SetViewRect1(CTextView *aView)
	{
	TRect view(0, 0, 100, 100);
	aView->SetViewRect(view);
	}

void SetViewRect2(CTextView *aView)
	{
	TRect view(10, 15, 60, 65);
	aView->SetViewRect(view);
	}

void TET_5D7MCV_L(TDes& aText, CTestGraphicsDevice* aDevice, CTextView* aView,
	CTextLayout* aLayout)
	{
	// This is supposed to test flicker-free redraw, but that is very difficult
	// to test properly, so we do what we can:
	// 1) If flicker-free redraw is disabled and there is no off-screen context set,
	//    there should be drawing to the real device
	// 2) If there is an off-screen context set, there should be drawing to the
	//    off-screen context, and not the real device
	// 3) If flicker-free redraw is enabled and there is no off-screen context set,
	//    there should be no drawing to the real device.
	// We are not testing that the correct thing is drawn to the internal off-screen
	// bitmap when flicker-free redraw is enabled. This is bad, but I can't work
	// out a good way round it. As the off-screen context uses much of the same
	// machinery as flicker-free redraw, it tests maybe 80% of it.
	SetViewRect1(aView);
	CTestGraphicsDevice* offScreenDevice = CTestGraphicsDevice::NewL(aDevice->SizeInPixels(), 0);
	CleanupStack::PushL(offScreenDevice);
	CWindowGc* offScreenContext;
	User::LeaveIfError(offScreenDevice->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);

	CTestTextView::SetOffScreenContext(aView, 0);
	aView->DisableFlickerFreeRedraw();

	aText.Append(KInitialText);
	aView->HandleInsertDeleteL(TCursorSelection(0,KInitialText().Length()), 0, EFalse);
	aText.Insert(1, KPicture);
	aView->HandleInsertDeleteL(TCursorSelection(1,2), 0, ETrue);
	aView->SetCursorVisibilityL(TCursor::EFCursorVisible, TCursor::EFCursorVisible);
	aView->SetSelectionVisibilityL(ETrue);
	aView->EnablePictureFrameL(ETrue);
	TRect* pictureRectPtr;
	TInt frameEdges;
	aView->SetXyPosL(TPoint(11, 1), EFalse, pictureRectPtr, frameEdges);
	test(pictureRectPtr != 0);
	aDevice->ClearAreaDrawnWithCondition();
	offScreenDevice->ClearAreaDrawnWithCondition();
	aView->DrawL(TRect(0, 0, CPinkSquare::KWidth, CPinkSquare::KHeight));
	test(!aDevice->AreaDrawnWithCondition().IsEmpty());
	test(offScreenDevice->AreaDrawnWithCondition().IsEmpty());
	aDevice->ClearAreaDrawnWithCondition();
	offScreenDevice->ClearAreaDrawnWithCondition();

	CTestTextView::SetOffScreenContext(aView, offScreenContext);
	aView->EnableFlickerFreeRedraw();

	aView->DrawL(TRect(0, 0, CPinkSquare::KWidth, CPinkSquare::KHeight));
	test(aDevice->AreaDrawnWithCondition().IsEmpty());
	TRect xoredArea = offScreenDevice->AreaDrawnWithCondition();
	test(!xoredArea.IsEmpty());
	offScreenDevice->ClearAreaDrawnWithCondition();
	SetViewRect1(aView);
	const TRect &viewRect = aView->ViewRect();
	aView->DrawL(viewRect);
	test(aDevice->AreaDrawnWithCondition().IsEmpty());
	test(offScreenDevice->AreaDrawnWithCondition() == xoredArea);

	// We need a CFbsDevice -- this will supply fonts that can be drawn to the
	// offscreen bitmap created by the flicker-free redraw code.
	CFbsBitmap* bm = new CFbsBitmap;
	CleanupStack::PushL(bm);
	bm->Create(TSize(1,1), EGray2);
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bm);
	CleanupStack::PushL(bitmapDevice);
	aLayout->SetImageDeviceMap(bitmapDevice);
	CTestTextView::SetOffScreenContext(aView, 0);
	aView->EnableFlickerFreeRedraw();

	aView->DrawL(TRect(0, 0, CPinkSquare::KWidth, CPinkSquare::KHeight));
	test(aDevice->AreaDrawnWithCondition().IsEmpty());
	xoredArea = offScreenDevice->AreaDrawnWithCondition();
	test(!xoredArea.IsEmpty());
	offScreenDevice->ClearAreaDrawnWithCondition();
	SetViewRect1(aView);
	aView->DrawL(viewRect);
	test(aDevice->AreaDrawnWithCondition().IsEmpty());

	aLayout->SetImageDeviceMap(aDevice);
	CleanupStack::PopAndDestroy(bitmapDevice);
	CleanupStack::PopAndDestroy(bm);
	CTestTextView::SetOffScreenContext(aView, 0);
	aView->DisableFlickerFreeRedraw();

	aDevice->ClearAreaDrawnWithCondition();
	offScreenDevice->ClearAreaDrawnWithCondition();
	aView->DrawL(viewRect);
	test(offScreenDevice->AreaDrawnWithCondition().IsEmpty());
	xoredArea.Move(viewRect.iTl);
	test(aDevice->AreaDrawnWithCondition() == xoredArea);

	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(offScreenDevice);
	}
/**
@SYMTestCaseID          SYSLIB-FORM-CT-4014
@SYMTestCaseDesc	    Tests CTextView::IsPictureFrameSelected doesn't panic with EFNoPictureFrame
@SYMTestPriority 	    High
@SYMTestActions
@SYMTestExpectedResults Resulting document position are where expected and frame drawn correctly.
 						In  this case picture frame is not drawn as we set the selection visibility to "off"
@SYMDEF                 PDEF118831
*/
void RunPDEF118831TestL()
	{
	//create editor/bitmap
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC(TRect(0, 0, 140, 185));

	//create the picture to insert (this one is a red box)
	CTestPicture* pic = new(ELeave)CTestPicture();
 	CleanupStack::PushL(pic);
   	pic->SetSizeInTwips(TSize(400,400));

   	//add some text before the pictures
   	bitMap->AppendL(_L("A"));

	//Add 2 pictures
	TInt pos = 0;
	for (pos=0;pos<2;pos++)
		bitMap->AppendL(pic);

	//add some text after the pictures
	bitMap->AppendL(_L("A"));
	bitMap->View()->HandleGlobalChangeL();

	//As a result we have
	//            A+Pict+Pict+A
	//with cursor pos: 01 1 2  2 3  4

	//SetSelectionVisibility to false
	bitMap->View()->SetSelectionVisibilityL(EFalse);

	// Place the cursor at the end of the last picture added and start moving the cursor left.
	TCursorPosition::TMovementType type = TCursorPosition::EFLeft;

	// Docuemnt position is already at the end, but it is Trailing. Make it Leading and set it.
	TTmDocPosSpec::TType docPosType = TTmDocPosSpec::ELeading;

	TTmDocPos thisPos;
	TTmDocPosSpec docPos;
	docPos.iPos = 4;
	docPos.iType = docPosType;
	TRect dummyRect;
	TInt dummyPos;
	TBool selectionIsPictureFrame;

	// Cursor on last letter "A"
	bitMap->View()->SetDocPosL(docPos);
	bitMap->View()->GetCursorPos(thisPos);
	selectionIsPictureFrame
	= bitMap->View()->IsPictureFrameSelected(dummyRect, dummyPos);
	test(thisPos.iPos == 4 && type == TCursorPosition::EFLeft);
	test(selectionIsPictureFrame == 0);

	// Move cursor left
	// Cursor moved to first (from the right) picture but picture frame not selected
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	selectionIsPictureFrame
	= bitMap->View()->IsPictureFrameSelected(dummyRect, dummyPos);
	test(thisPos.iPos == 3 && type == TCursorPosition::EFLeft);
	test(selectionIsPictureFrame == 0);


	// Move cursor left
	// Cursor still on first picture and now picture frame has been selected
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	selectionIsPictureFrame
	= bitMap->View()->IsPictureFrameSelected(dummyRect, dummyPos);
	test(thisPos.iPos == 2 && type == TCursorPosition::EFLeft);
	test(selectionIsPictureFrame == 1);

	// Move cursor left
	// Cursor on second picture and picture frame not selected
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	selectionIsPictureFrame
	= bitMap->View()->IsPictureFrameSelected(dummyRect, dummyPos);
	test(thisPos.iPos == 2 && type == TCursorPosition::EFLeft);
	test(selectionIsPictureFrame == 0);


	// Move cursor left
	// Cusror still on second picture and picture frame has been selected
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	selectionIsPictureFrame
	= bitMap->View()->IsPictureFrameSelected(dummyRect, dummyPos);
	test(thisPos.iPos == 1 && type == TCursorPosition::EFLeft);
	test(selectionIsPictureFrame == 1);

	// Move cursor left
	// Cursor moved to second letter "A"
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	selectionIsPictureFrame
	= bitMap->View()->IsPictureFrameSelected(dummyRect, dummyPos);
	test(thisPos.iPos == 1 && type == TCursorPosition::EFLeft);
	test(selectionIsPictureFrame == 0);


	//Move cursor left
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	selectionIsPictureFrame
	= bitMap->View()->IsPictureFrameSelected(dummyRect, dummyPos);
	test(thisPos.iPos == 0 && type == TCursorPosition::EFLeft);
	test(selectionIsPictureFrame == 0);


	CleanupStack::Pop(pic);
	CleanupStack::PopAndDestroy(1);
	}
/**
Test for defect INC020746: Visual cursor position update incorrect at end of
document
*/
void INC020746_L(TDes& aText, CTextView* aView)
	{
	aText.Zero();
	aText.Append('A');
	aText.Append(0x5D0);	// Aleph
	SetViewRect1(aView);
	aView->DisableFlickerFreeRedraw();
	aView->HandleGlobalChangeL();
	TTmDocPosSpec pos(2, TTmDocPosSpec::ELeading);
	aView->SetDocPosL(pos);
	TTmDocPos posOut;
	aView->GetCursorPos(posOut);
	test(posOut.iPos == 2);
	test(posOut.iLeadingEdge);
	pos.iType = TTmDocPosSpec::ETrailing;
	aView->SetDocPosL(pos);
	aView->GetCursorPos(posOut);
	test(posOut.iPos == 2);
	test(!posOut.iLeadingEdge);
	}

/**
@SYMTestCaseID SYSLIB-FORM-UT-1653
@SYMTestCaseDesc Test to make sure an ordinary hyphen is returned when a potential hyphen is inserted.
@SYMTestPriority Critical.
@SYMTestActions Insets a potential hyphen into some text and then check the output once it has passed
				 	through MTmCustom::Map.
@SYMTestExpectedResults The test must not fail.
@SYMDEF INC080603: Soft Hyphen is not replaced by Ordinary Hyphen in Tagma
*/

// Test code had to be changed because of a problem in BullseyeCoverage, which does not like
// dealing with certain hexadecimal character combinations in string literals.
void INC080603L(TDes& aText, CTestGraphicsDevice* aDevice,CTextLayout* aLayout, CTextView* aView)
	{
	_LIT(KInputText,  "ABCDEFABCDEF");
	const TUint16 Hyphen = 0x00ad;

	aDevice->LineArray().ResetLineArray();
	aDevice->LineArray().Enable();

	aText.Zero();
	aText = KInputText;
	aText.Insert(6, (TPtrC(&Hyphen,1)));

	SetViewRect1(aView);
	aLayout->SetWrapWidth(aView->ViewRect().Width());
	aLayout->ForceNoWrapping(CTextLayout::EFParagraphsWrappedByDefault);

	aView->HandleGlobalChangeL();

	TBuf<3> des2 (aDevice->LineArray().Line(1).LineData()); //Hyphen should be on this line
	TInt pos = des2.Find(_L("-"));
	test (pos != KErrNotFound);

	aDevice->LineArray().Disable();
	}

/**
DEF035472 - Non-printable char detection api does not report correct result
*/
void DEF035472_L(TDes& aText, CTextView* aView)
	{
	const TChar leftToRightMarker(0x200E);
	aText.Zero();
	aText.Append(leftToRightMarker);
	aText.Append(leftToRightMarker);
	aText.Append(leftToRightMarker);
	aText.Append('A');
	aText.Append(leftToRightMarker);
	aText.Append(leftToRightMarker);
	aText.Append(leftToRightMarker);
	aView->HandleGlobalChangeL();
	TTmDocPosSpec position(3, TTmDocPosSpec::ELeading);
	aView->SetDocPosL(position);
	TCursorSelection selection = aView->GetBackwardDeletePositionL();
	test(selection.LowerPos() == 0);
	test(selection.HigherPos() == 3);
	position.iType = TTmDocPosSpec::ETrailing;
	aView->SetDocPosL(position);
	selection = aView->GetBackwardDeletePositionL();
	test(selection.LowerPos() == 0);
	test(selection.HigherPos() == 3);
	position.iPos = 4;
	aView->SetDocPosL(position);
	selection = aView->GetForwardDeletePositionL();
	test(selection.LowerPos() == 4);
	test(selection.HigherPos() == 7);
	position.iType = TTmDocPosSpec::ELeading;
	aView->SetDocPosL(position);
	selection = aView->GetForwardDeletePositionL();
	test(selection.LowerPos() == 4);
	test(selection.HigherPos() == 7);
	}

/**
INC036005 - Word--difficult to place focus on an object in word.
This defect is caused by a defect in Form in which when finding a position
by x-y coordinate, the trailing edge is never found, always the leading edge.
This means that the trailing edge of the picture is incorrectly identified as
the leading edge of the following character and the picture is not highlighted.
This test tests that the trailing edge of characters is found correctly.
*/
void INC036005_L(TDes& aText, CTextView* aView)
	{
	aText.Zero();
	aText.Append('A');
	aText.Append('B');
	aText.Append('C');
	SetViewRect2(aView);
	aView->DisableFlickerFreeRedraw();
	aView->HandleGlobalChangeL();
	aView->SetLeftTextMargin(0);
	aView->SetViewLineAtTopL(1);
	TPoint pt(22, 20);
	test(1 == aView->XyPosToDocPosL(pt));
	pt.SetXY(28, 20);
	test(1 == aView->XyPosToDocPosL(pt));
	aText.Zero();
	aText.Append(0x5D0);
	aText.Append(0x5D1);
	aText.Append(0x5D2);
	aView->HandleGlobalChangeL();
	aView->SetLeftTextMargin(0);
	aView->SetViewLineAtTopL(1);
	pt.SetXY(92, 20);
	test(1 == aView->XyPosToDocPosL(pt));
	pt.SetXY(98, 20);
	test(1 == aView->XyPosToDocPosL(pt));

	// Test with real picture
	aText.Zero();
	aText.Append('A');
	aText.Append(KPictureCharacter);
	aText.Append('C');
	aView->HandleGlobalChangeL();
	aView->SetLeftTextMargin(0);
	aView->SetViewLineAtTopL(1);
	TInt edges;
	TRect* rect;
	pt.SetXY(22, 20);
	aView->SetXyPosL(pt, EFalse, rect, edges);
	TCursorSelection sel = aView->Selection();
	test(sel.LowerPos() == 1 && sel.HigherPos() == 2);
	aView->SetDocPosL(0, EFalse);
	pt.SetXY(32, 20);
	aView->SetXyPosL(pt, EFalse, rect, edges);
	sel = aView->Selection();
	test(sel.LowerPos() == 1 && sel.HigherPos() == 2);

	// Test with real picture and potentially confusing ambiguities
	aText.Zero();
	aText.Append('A');
	aText.Append(KRightToLeftMarker);
	aText.Append(KPictureCharacter);
	aText.Append(KRightToLeftMarker);
	aText.Append('C');
	aView->HandleGlobalChangeL();
	aView->SetLeftTextMargin(0);
	aView->SetViewLineAtTopL(1);
	pt.SetXY(22, 20);
	aView->SetXyPosL(pt, EFalse, rect, edges);
	sel = aView->Selection();
	test(sel.LowerPos() == 2 && sel.HigherPos() == 3);
	aView->SetDocPosL(0, EFalse);
	pt.SetXY(32, 20);
	aView->SetXyPosL(pt, EFalse, rect, edges);
	sel = aView->Selection();
	test(sel.LowerPos() == 2 && sel.HigherPos() == 3);
	}


// INC085809: CSHelp - Scrolling doesn't work properly, length of the bar changes weirdly
// With the fix in PosRangeInBand, the output in this case will be one pixel larger than before
void INC085809_L(CTextLayout* aLayout, CTextView* aView)
	{
	TInt input;
	TRect view(0, 0, 102, 26);
	aView->SetViewRect(view);
	TInt output = aLayout->PosRangeInBand(input);
	test(output==11);
	}

// Utility functions to show contents of test data using test.Printf

_LIT(KAddressMarker, "> ");
_LIT(KSpace, " ");
_LIT(KLength, ", Length of Data = %d 16-bit words\r\n");
_LIT(KSpaces, "                                                                      ");
_LIT(KPeriod, ".");
_LIT(KSingleString, "%s\r\n");


 void PrintTestData (const TDesC& aTitle , const TDesC16& aData)
	{

	TInt i;
	TInt j;
	TInt end;

	TInt length = aData.Length();

	TBuf<80> buffer;

	buffer.Zero();
	buffer.Append(aTitle);
	buffer.Append(KLength);

	test.Printf(buffer, length);

	for (i = 0 ; i < length ; i += 8)
		{
		buffer.Zero();
		buffer.AppendNumFixedWidth(i, EHex, 8);
		buffer += KAddressMarker;

		end = ((length-i) >= 8) ? i+8 : length;

		for (j = i ; j < end ; ++j)
			{
			buffer.AppendNumFixedWidth(aData[j], EHex, 4);
			buffer += KSpace;
			}
		buffer += TPtrC(KSpaces().Ptr(), ((8-(j-i))*5)+4);

		for (j = i ; j < end ; ++j)
			{
			if (aData[j] >= 32)
				{
				buffer.Append(aData[j]);
				}
			else
				{
				buffer += KPeriod;
				}
			}
		buffer.ZeroTerminate();
		test.Printf(KSingleString, buffer.Ptr());
		}

	}

void PrintTestData(const TDesC& aTitle, const TText16* aDataBuffer, const TInt aSize)
	{
	PrintTestData(aTitle, TPtrC16(aDataBuffer, aSize));
	}


  _LIT(KTavSequence, "\x05ea\x05ea\x05ea\x05ea\x05ea\x05ea\x05ea\x05ea\x05ea\x05ea");

  _LIT(KMixedSequence, "\x05d1\x05d2\x05d3\x05d4\x05d5\x05d6\x05d7\x05d8\x05d9 JCDEFGHIJ "
  	 L"\x05da\x05db\x05dc\x05dd\x05de\x05df\x05e0\x05e1\x05e2 KLMNOPQRS ");

static const TInt KChunksPerTest = 9;
static const TInt KDirectionalityTests = 5;

static const TPtrC16 KDirectionalityResults[KDirectionalityTests][KChunksPerTest] =
	{
		{
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x05d9\x05d8\x05d7\x05d6\x05d5\x05d4\x05d3\x05d2\x05d1\x05d0\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004a\x0043\x0044\x0045\x0046\x0047\x0048\x0049\x004a\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x05e2\x05e1\x05e0\x05df\x05de\x05dd\x05dc\x05db\x05da\xffff"),
		_S("\xffff\xffff\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004b\x004c\x004d\x004e\x004f\x0050\x0051\x0052\x0053\xffff")
		},
		{
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x05d9\x05d8\x05d7\x05d6\x05d5\x05d4\x05d3\x05d2\x05d1\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004a\x0043\x0044\x0045\x0046\x0047\x0048\x0049\x004a\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x05e2\x05e1\x05e0\x05df\x05de\x05dd\x05dc\x05db\x05da\xffff"),
		_S("\xffff\xffff\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004b\x004c\x004d\x004e\x004f\x0050\x0051\x0052\x0053\xffff"),
		},
		{
		_S("\xffff\x0049\xffff"),
		_S("\xffff\x05d9\x05d8\x05d7\x05d6\x05d5\x05d4\x05d3\x05d2\x05d1\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004a\x0043\x0044\x0045\x0046\x0047\x0048\x0049\x004a\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x05e2\x05e1\x05e0\x05df\x05de\x05dd\x05dc\x05db\x05da\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004b\x004c\x004d\x004e\x004f\x0050\x0051\x0052\x0053\x0020\xffff\xffff"),
		_S("\xffff\xffff\xffff"),
		},
		{
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x05d9\x05d8\x05d7\x05d6\x05d5\x05d4\x05d3\x05d2\x05d1\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004a\x0043\x0044\x0045\x0046\x0047\x0048\x0049\x004a\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x05e2\x05e1\x05e0\x05df\x05de\x05dd\x05dc\x05db\x05da\xffff"),
		_S("\xffff\xffff\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004b\x004c\x004d\x004e\x004f\x0050\x0051\x0052\x0053\xffff"),
		},
		{
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x05d9\x05d8\x05d7\x05d6\x05d5\x05d4\x05d3\x05d2\x05d1\x05d0\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004a\x0043\x0044\x0045\x0046\x0047\x0048\x0049\x004a\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x05e2\x05e1\x05e0\x05df\x05de\x05dd\x05dc\x05db\x05da\xffff"),
		_S("\xffff\xffff\xffff"),
		_S("\xffff\x0020\xffff"),
		_S("\xffff\x004b\x004c\x004d\x004e\x004f\x0050\x0051\x0052\x0053\xffff"),
		}
	};

_LIT(KTitle1, "Initial format (RtoL)");
_LIT(KTitle2, "Deleting first character , still RtoL");
_LIT(KTitle3, "Inserting a first character, now LtoR");
_LIT(KTitle4, "Deleting first character , becoming RtoL");
_LIT(KTitle5, "Inserting a first character, still RToL");
_LIT(KPassed, "passed\r\n");
_LIT(KFailed, "failed\r\n");
_LIT(KExpected, "Expected");
_LIT(KGot, "Got");
_LIT(KCase, "Test case %d, testing chunk %d\r\n");



const TUint16 KUnicodeLetterA			  =   'I';
const TUint16 KUnicodeLetterHebrewAlef  =   0x05d0;

TPtrC KLUnicodeLetterA(&KUnicodeLetterA,1);
TPtrC KLUnicodeLetterHebrewAlef(&KUnicodeLetterHebrewAlef,1);

void DirectionalityTestL(TDes& aText, CTestGraphicsDevice* aDevice, CTextLayout* aLayout, CTextView* aView)
	{
	test.Next(_L("Testing paragraph directionality of editable text"));
	CTestGraphicsDevice* offScreenDevice = CTestGraphicsDevice::NewL(aDevice->SizeInPixels(), 0);
	CleanupStack::PushL(offScreenDevice);
	offScreenDevice->SetLineArray(aDevice->LineArray());
	CWindowGc* offScreenContext;
	User::LeaveIfError(offScreenDevice->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetOffScreenContext(aView, offScreenContext);
	aView->EnableFlickerFreeRedraw();
	TInt i;
	TPtrC16 title;
	TBool match;
	TBool passed;
	TBool allPassed = ETrue;
	TInt chunksInThisTest;
	for (TInt testCase = 0 ; testCase < KDirectionalityTests ; ++testCase)
		{
		test.Printf(_L("Directionality test case %d\r\n"), testCase+1);
		passed = ETrue;
		aDevice->LineArray().ResetLineArray();
		chunksInThisTest = KChunksPerTest;
		switch(testCase)
			{
			case 0:
				title.Set(KTitle1);
				aLayout->SetAmountToFormat(CTextLayout::EFFormatAllText);
				aText.Zero();
				aDevice->LineArray().ResetLineArray();
				aText.Append(KUnicodeLetterHebrewAlef);  //first strong character is right-to-left
				aText.Append(KMixedSequence);			 //rest of test data is constant
				aView->HandleGlobalChangeL();
				aView->FinishBackgroundFormattingL();
				break;
			case 1:
				title.Set(KTitle2);
				aView->SetDocPosL(0,EFalse);
				aDevice->LineArray().ResetLineArray();
				aText.Delete(0,1);	//delete the first character
				aView->HandleGlobalChangeL();
				break;
			case 2:
				title.Set(KTitle3);
				aView->SetDocPosL(0,EFalse);
				aDevice->LineArray().ResetLineArray();
				aText.Insert(0,KLUnicodeLetterA); //replace first character with a left-to-right one
				aView->HandleGlobalChangeL();
				chunksInThisTest = KChunksPerTest - 1;
				break;
			case 3:
				title.Set(KTitle4);
				aView->SetDocPosL(0,EFalse);
				aDevice->LineArray().ResetLineArray();
				aText.Delete(0,1);	//delete the first character
				aView->HandleGlobalChangeL();
				break;
			case 4:
				title.Set(KTitle5);
				aView->SetDocPosL(0,EFalse);
				aDevice->LineArray().ResetLineArray();
				aText.Insert(0,KLUnicodeLetterHebrewAlef); //replace first character with a right-to-left one again
				aView->HandleGlobalChangeL();
				break;
			default:
				test.Printf(_L("Directionality test %d undefined\r\n"), testCase);
				break;
			}
		test.Printf(title);
		for (i = 0; i < chunksInThisTest ; ++i)
			{
            const TDesC16& des1 = KDirectionalityResults[testCase][i];
            const TDesC16& des2 = aDevice->LineArray().Line(i).LineData();
			match = (des1.Compare(des2) == 0);
			if (!match)
				{
				test.Printf(KCase, testCase, i);
				PrintTestData(KExpected,  KDirectionalityResults[testCase][i]);
				PrintTestData(KGot, aDevice->LineArray().Line(i).LineData());
				}
			passed = passed && match;
			}
		if (passed)
			{
			test.Printf(KPassed);
			}
		else
			{
			allPassed = EFalse;
			test.Printf(KFailed);
			}
		}
	test(allPassed);
	CTestTextView::SetOffScreenContext(aView, 0);
	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(offScreenDevice);
	}

// Test for defect DEF021603: Visual cursor position update incorrect when insert a LTR char after
// a RTL string.
void DEF021603_L(TDes& aText, CTestGraphicsDevice* aDevice, CTextView* aView)
	{
	CTestGraphicsDevice* offScreenDevice = CTestGraphicsDevice::NewL(aDevice->SizeInPixels(), 0);
	CleanupStack::PushL(offScreenDevice);
	CWindowGc* offScreenContext;
	User::LeaveIfError(offScreenDevice->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetOffScreenContext(aView, offScreenContext);

	TTmDocPos posOut;
	aText.Zero();
	aText.Append(KLUnicodeLetterHebrewAlef);
	aText.Append(KLUnicodeLetterHebrewAlef);
	aText.Append(KLUnicodeLetterA);
	aView->HandleCharEditL(CTextLayout::EFCharacterInsert, EFalse);
	aView->GetCursorPos(posOut);
	test(!posOut.iLeadingEdge);		// should be on trailing edge (rhs)

	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(offScreenDevice);
	}


_LIT(KText_022229, "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCABCDEFGHIJABCDEFGHIJABCDEFGHIJ");

/**
Test for defect DEF022229: Form: Cursor sent to 'home' position on Page up/down
movement in Word.
*/
void DEF022229_L(TDes& aText, CTestGraphicsDevice* aDevice, CTextView* aView)
	{
	SetViewRect2(aView);
	CTestGraphicsDevice* offScreenDevice = CTestGraphicsDevice::NewL(aDevice->SizeInPixels(), 0);
	CleanupStack::PushL(offScreenDevice);
	CWindowGc* offScreenContext;
	User::LeaveIfError(offScreenDevice->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetOffScreenContext(aView, offScreenContext);

	TTmDocPos cursorPos;
	aText = KText_022229;
	aView->SetDocPosL(0);
	test(cursorPos.iPos==0);

	//LineDown
	TCursorPosition::TMovementType lineDown = TCursorPosition::EFLineDown;
	aView->MoveCursorL(lineDown, EFalse);
	aView->GetCursorPos(cursorPos);
	TInt compare = cursorPos.iPos;

	//PageDown
	TCursorPosition::TMovementType pageDown = TCursorPosition::EFPageDown;
	aView->MoveCursorL(pageDown, EFalse);
	aView->GetCursorPos(cursorPos);
	test(cursorPos.iPos!=0);

	//PageUp
	TCursorPosition::TMovementType pageUp = TCursorPosition::EFPageUp;
	aView->MoveCursorL(pageUp, EFalse);
	aView->GetCursorPos(cursorPos);
	test(cursorPos.iPos==compare);

	CTestTextView::SetOffScreenContext(aView, 0);
	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(offScreenDevice);
	}

/**
Tests that CTextView::XyPosToDocPosL returns the correct positions in the case
where the point passed in is beyond the horizontal bounds of the line.
*/
void TestCTextView_XyPosToDocPosL_ReturnsCorrectLine(
	TDes& aText, CTextView* aView)
	{
	aText = _L("abcdef ghijkl\x2029zyxwvu.");
	aView->HandleGlobalChangeL();
	TPoint pt(-20, 24);
	TInt pos = aView->XyPosToDocPosL(pt);
	test(pos == 0);
	pt.SetXY(200, 24);
	pos = aView->XyPosToDocPosL(pt);
	test(pos == 6 || pos == 5);
	pt.SetXY(-20, 36);
	pos = aView->XyPosToDocPosL(pt);
	test(pos == 7);
	pt.SetXY(200, 36);
	pos = aView->XyPosToDocPosL(pt);
	test(pos == 13);
	pt.SetXY(-20, 48);
	pos = aView->XyPosToDocPosL(pt);
	test(pos == 14);
	pt.SetXY(200, 48);
	pos = aView->XyPosToDocPosL(pt);
	test(pos == 20 || pos == 21);
	}

void INC036809_L(TDes& aText, CTextView* aView)
	{
	aText = _L("A\tB");
	aView->HandleGlobalChangeL();
	TTmPosInfo2 posInfo;
	TTmDocPosSpec pos(1, TTmDocPosSpec::ELeading);
	TBool result = aView->FindDocPosL(pos, posInfo);
	test(result);
	test(posInfo.iDocPos.iPos == 1);
	test(posInfo.iDocPos.iLeadingEdge);
	test(posInfo.iEdge.iX = 30);
	pos.iPos = 2;
	pos.iType = TTmDocPosSpec::ETrailing;
	result = aView->FindDocPosL(pos, posInfo);
	test(result);
	test(posInfo.iDocPos.iPos == 2);
	test(!posInfo.iDocPos.iLeadingEdge);
	test(posInfo.iEdge.iX = 45);
	pos.iType = TTmDocPosSpec::ELeading;
	result = aView->FindDocPosL(pos, posInfo);
	test(result);
	test(posInfo.iDocPos.iPos == 2);
	test(posInfo.iDocPos.iLeadingEdge);
	test(posInfo.iEdge.iX = 45);
	}

void DEF037255_L(TDes& aText, CTextView* aView)
	{
	// 0x2004 = 3-per-em space: 3 pixels in the test font
	// 0x2029 = Paragraph delimiter
	// 0xFFFC = object replacement character
	aText = _L("\x2004\x2004Y\x2029\xFFFC\x2029\x2004\x2004W");
	aView->EnablePictureFrameL(ETrue);
	aView->HandleGlobalChangeL();
	TTmDocPos pos(1, EFalse);
	aView->SetDocPosL(pos);
	const TCursorPosition::TMovementType down = TCursorPosition::EFLineDown;
	TCursorPosition::TMovementType m = down;
	aView->MoveCursorL(m, EFalse);
	TCursorSelection sel = aView->Selection();
	test(sel.LowerPos() == 4);
	test(sel.HigherPos() == 5);
	m = down;
	aView->MoveCursorL(m, EFalse);
	sel = aView->Selection();
	test(sel.LowerPos() == 7);
	test(sel.HigherPos() == 7);
	}

/** Tests that the horizontal scroll position is within acceptable bounds.
*/
void TestCursorPosition(const CTextView* aView)
	{
	TInt hsj = aView->HorizontalScrollJump();
	TInt width = aView->ViewRect().Width();
	TInt buffer = width / 10;
	TInt left;
	TInt right;
	aView->Layout()->CalculateHorizontalExtremesL(left, right, ETrue, EFalse);
	TInt minCursorX = 0;
	TInt maxCursorX = width;
	if (left < 0)
		{
		left -= hsj + buffer;
		minCursorX = buffer;
		}
	if (width < right)
		{
		right += hsj + buffer;
		maxCursorX = width - buffer;
		}
	TInt ltm = aView->LeftTextMargin();
	TTmDocPos docPos;
	aView->GetCursorPos(docPos);
	TTmPosInfo2 posInfo;
	TTmLineInfo lineInfo;
	TBool docPosFormatted = aView->Layout()
		->FindDocPos(docPos, posInfo, &lineInfo);
	TBool cursorIsOutsideBounds = posInfo.iEdge.iX < lineInfo.iInnerRect.iTl.iX
		|| lineInfo.iInnerRect.iBr.iX <= posInfo.iEdge.iX? ETrue : EFalse;
	TInt cursorX = posInfo.iEdge.iX - ltm;
	test(docPosFormatted);
	test((left <= ltm && ltm + width <= right)
		|| cursorIsOutsideBounds);
	test((minCursorX <= cursorX && cursorX <= maxCursorX)
		|| cursorIsOutsideBounds);
	}

void INC037293_L(TDes& aText, CTextView* aView, CTextLayout* aLayout)
	{
	aView->EnablePictureFrameL(EFalse);
	aLayout->ForceNoWrapping(CTextLayout::EFAllParagraphsNotWrapped);
	aLayout->SetWrapWidth(aView->ViewRect().Width());
	aText.Zero();
	aView->HandleGlobalChangeL();
	TestCursorPosition(aView);
	TCursorPosition::TMovementType move;
	for (TInt i0 = 0; i0 != 20; ++i0)
		{
		aText.Append('a' + i0);
		TCursorSelection sel(i0, i0 + 1);
		aView->HandleInsertDeleteL(sel, 0, EFalse);
		TestCursorPosition(aView);
		}
	move = TCursorPosition::EFLineBeg;
	aView->MoveCursorL(move, EFalse);
	TestCursorPosition(aView);
	move = TCursorPosition::EFLineEnd;
	aView->MoveCursorL(move, EFalse);
	TestCursorPosition(aView);
	for (TInt i1 = 0; i1 != 20; ++i1)
		{
		move = TCursorPosition::EFLeft;
		aView->MoveCursorL(move, EFalse);
		TestCursorPosition(aView);
		}
	for (TInt i2 = 0; i2 != 20; ++i2)
		{
		move = TCursorPosition::EFRight;
		aView->MoveCursorL(move, EFalse);
		TestCursorPosition(aView);
		}
	aText.Zero();
	aView->HandleGlobalChangeL();
	TestCursorPosition(aView);
	for (TInt i3 = 0; i3 != 20; ++i3)
		{
		aText.Append(KAleph);
		TCursorSelection sel(i3, i3 + 1);
		aView->HandleInsertDeleteL(sel, 0, EFalse);
		TestCursorPosition(aView);
		}
	move = TCursorPosition::EFLineBeg;
	aView->MoveCursorL(move, EFalse);
	TestCursorPosition(aView);
	move = TCursorPosition::EFLineEnd;
	aView->MoveCursorL(move, EFalse);
	TestCursorPosition(aView);
	for (TInt i4 = 0; i4 != 20; ++i4)
		{
		move = TCursorPosition::EFRight;
		aView->MoveCursorL(move, EFalse);
		TestCursorPosition(aView);
		}
	for (TInt i5 = 0; i5 != 20; ++i5)
		{
		move = TCursorPosition::EFLeft;
		aView->MoveCursorL(move, EFalse);
		TestCursorPosition(aView);
		}

	aText.Zero();
	aLayout->ForceNoWrapping(CTextLayout::EFParagraphsWrappedByDefault);
	aView->HandleGlobalChangeL();
	TestCursorPosition(aView);
	for (TInt i6 = 0; i6 != 10; ++i6)
		{
		aText.Append('a' + i6);
		aText.Append(' ');
		TCursorSelection sel(i6 * 2, i6 * 2 + 2);
		aView->HandleInsertDeleteL(sel, 0, EFalse);
		TestCursorPosition(aView);
		}
	for (TInt i7 = 0; i7 != 30; ++i7)
		{
		move = TCursorPosition::EFLeft;
		aView->MoveCursorL(move, EFalse);
		TestCursorPosition(aView);
		}
	for (TInt i8 = 0; i8 != 30; ++i8)
		{
		move = TCursorPosition::EFRight;
		aView->MoveCursorL(move, EFalse);
		TestCursorPosition(aView);
		}

	aText.Zero();
	aView->HandleGlobalChangeL();
	TestCursorPosition(aView);
	for (TInt i9 = 0; i9 != 10; ++i9)
		{
		aText.Append(KAleph);
		aText.Append(' ');
		TCursorSelection sel(i9 * 2, i9 * 2 + 2);
		aView->HandleInsertDeleteL(sel, 0, EFalse);
		TestCursorPosition(aView);
		}
	for (TInt ia = 0; ia != 30; ++ia)
		{
		move = TCursorPosition::EFRight;
		aView->MoveCursorL(move, EFalse);
		TestCursorPosition(aView);
		}
	for (TInt ib = 0; ib != 30; ++ib)
		{
		move = TCursorPosition::EFLeft;
		aView->MoveCursorL(move, EFalse);
		TestCursorPosition(aView);
		}
	}

/** Tests that the text specified has been drawn by the device and returns
its x-coordinate. */
TInt XCoordinateOfText(const TDesC& aText, CTestGraphicsDevice* aDevice)
	{
	const TTestGCDisplayLine* line = aDevice->LineArray().Find(aText);
	test(line != 0);
	return line->Position().iX;
	}

/**
INC038282 - Right-to-Left text in CSHLPCMP toolchain + Form is not formatted
correctly

Tests that when the text directionality is resolved implicitly the indents,
margins and bullets are on the correct sides.
@internalComponent
*/
void INC038282_L(TDes& aText, CTestGraphicsDevice* aDevice,
	CTextLayout* aLayout, CTextView* aView,
	CParaFormat* aParagraphFormat)
	{
	SetViewRect2(aView);
	// Gentle first test: set margins, left-to-right text, no bullets, leading alignment.
	aText = _L("F s.");
	aParagraphFormat->iLeftMarginInTwips = 200;	// 5 pixels
	aParagraphFormat->iRightMarginInTwips = 600;	// 15 pixels
	aParagraphFormat->iIndentInTwips = 280;		// 7 pixels
	aParagraphFormat->iHorizontalAlignment = CParaFormat::ELeftAlign;
	aLayout->SetWrapWidth(aView->ViewRect().Width());
	TCursorSelection selection(0, 0);
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	TInt x = XCoordinateOfText(_L("F"), aDevice);
	test(x == 22);
	x = XCoordinateOfText(_L("s."), aDevice);
	test(x == 15);
	// Now right aligned
	aParagraphFormat->iHorizontalAlignment = CParaFormat::ERightAlign;
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("F"), aDevice);
	test(x == 35);
	x = XCoordinateOfText(_L("s."), aDevice);
	test(x == 25);
	// Now Hebrew left aligned (with ERightAlign set, because really
	// that means "trailing")
	aText = _L("\x5D0 \x5D1");
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("\x5D0"), aDevice);
	test(x == 25);
	x = XCoordinateOfText(_L("\x5D1"), aDevice);
	test(x == 25);
	// Now right aligned (== leading == ELeftAlign).
	aParagraphFormat->iHorizontalAlignment = CParaFormat::ELeftAlign;
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("\x5D0"), aDevice);
	test(x == 38);
	x = XCoordinateOfText(_L("\x5D1"), aDevice);
	test(x == 45);
	// Now with a bullet.
	aParagraphFormat->iIndentInTwips = 440;		// 11 pixels
	aParagraphFormat->iBullet = new(ELeave) TBullet;
	aParagraphFormat->iBullet->iHangingIndent = EFalse;
	aParagraphFormat->iBullet->iCharacterCode = 0x2022;
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("\x5D0"), aDevice);
	test(x == 34);
	x = XCoordinateOfText(_L("\x5D1"), aDevice);
	test(x == 45);
	x = XCoordinateOfText(_L("\x2022"), aDevice);
	test(44 <= x);
	// Trailing (left) align
	aParagraphFormat->iHorizontalAlignment = CParaFormat::ERightAlign;
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("\x5D0"), aDevice);
	test(x == 25);
	x = XCoordinateOfText(_L("\x5D1"), aDevice);
	test(x == 25);
	x = XCoordinateOfText(_L("\x2022"), aDevice);
	test(35 <= x);
	// Latin text, right aligned.
	aText = _L("F s.");
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("F"), aDevice);
	test(x == 35);
	x = XCoordinateOfText(_L("s."), aDevice);
	test(x == 25);
	x = XCoordinateOfText(_L("\x2022"), aDevice);
	test(x <= 25);
	// left aligned
	aParagraphFormat->iHorizontalAlignment = CParaFormat::ELeftAlign;
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("F"), aDevice);
	test(x == 26);
	x = XCoordinateOfText(_L("s"), aDevice);
	test(x == 15);
	x = XCoordinateOfText(_L("\x2022"), aDevice);
	test(x <= 15);
	// Now bullet with hanging indent (Latin text, leading alignment)
	aParagraphFormat->iBullet->iHangingIndent = ETrue;
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("F"), aDevice);
	test(x == 26);
	x = XCoordinateOfText(_L("s"), aDevice);
	test(x == 26);
	x = XCoordinateOfText(_L("\x2022"), aDevice);
	test(x <= 15);
	// Trailing alignment
	aParagraphFormat->iHorizontalAlignment = CParaFormat::ERightAlign;
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("F"), aDevice);
	test(x == 35);
	x = XCoordinateOfText(_L("s"), aDevice);
	test(x == 35);
	x = XCoordinateOfText(_L("\x2022"), aDevice);
	test(x <= 25);
	// Hebrew, trailing, hanging indent
	aText = _L("\x5D0 \x5D1");
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("\x5D0"), aDevice);
	test(x == 25);
	x = XCoordinateOfText(_L("\x5D1"), aDevice);
	test(x == 25);
	x = XCoordinateOfText(_L("\x2022"), aDevice);
	test(36 <= x);
	// Leading alignment
	aParagraphFormat->iHorizontalAlignment = CParaFormat::ELeftAlign;
	aView->SetPendingSelection(selection);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	x = XCoordinateOfText(_L("\x5D0"), aDevice);
	test(x == 34);
	x = XCoordinateOfText(_L("\x5D1"), aDevice);
	test(x == 34);
	x = XCoordinateOfText(_L("\x2022"), aDevice);
	test(44 <= x);

	aParagraphFormat->Reset();
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-1663
@SYMTestCaseDesc	    Tests to make sure when non-solid border has been set for the paragraph more than 1 line,
						there's no border been drawn between the lines in the paragraph.
@SYMTestPriority 	    High
@SYMTestActions  	    Insert a paragraph of text, which contains at least 3 lines. Draw non-solid borders
						for the paragraph. Check the area between line 1 and line 2 to see if there's any border
						been drawn incorrectly.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF073913
*/
const TInt KBorderThicknessInTwips=40;

void DEF073913L(TDes& aText, CTestGraphicsDevice* aDevice, CTextView* aView, CParaFormat* aParagraphFormat)
	{
	SetViewRect1(aView);

	aText = _L("abcdefghijklmnopqrstuvwxyz");

	// set borders' attribute
	TParaBorder top;
	TParaBorder bottom;
	TParaBorder left;
	TParaBorder right;
	top.iThickness=KBorderThicknessInTwips;
	bottom.iThickness=KBorderThicknessInTwips;
	left.iThickness=KBorderThicknessInTwips;
	right.iThickness=KBorderThicknessInTwips;

	// make it non-solid for the defect testing
	top.iLineStyle=TParaBorder::EDotted;
	bottom.iLineStyle=TParaBorder::EDotted;
	left.iLineStyle=TParaBorder::EDotted;
	right.iLineStyle=TParaBorder::EDotted;

	TParaFormatMask aParaMask;
	aParaMask.SetAttrib(EAttTopBorder);
	aParaMask.SetAttrib(EAttBottomBorder);
	aParaMask.SetAttrib(EAttLeftBorder);
	aParaMask.SetAttrib(EAttRightBorder);

	aParagraphFormat->SetParaBorderL(CParaFormat::EParaBorderTop,top);
	aParagraphFormat->SetParaBorderL(CParaFormat::EParaBorderBottom,bottom);
	aParagraphFormat->SetParaBorderL(CParaFormat::EParaBorderLeft,left);
	aParagraphFormat->SetParaBorderL(CParaFormat::EParaBorderRight,right);

	TCursorSelection selection(0, 0);
	aView->SetPendingSelection(selection);

	// set testing area: border area between text line 1 and line 2
	TRect forbiddenArea(1, 13, 49, 13);
	aDevice->SetTestingArea(forbiddenArea);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();

	// check there have enough lines in the paragraph
	TInt lines = aDevice->LineArray().LinesPresent();
	test(lines >= 3);

	// check the drawing result,
	// expected: no line has been drawn on the testing area
	test(aDevice->HasDrawnOnTestingArea() == EFalse);

	aParagraphFormat->Reset();
	}

/**
INC039567 - Zero-width Non-joiner character does not work in editors

Tests that Arabic letters separated by ZWNJ (U+200C) are either rendered in
separate calls or remain separated in their strings by ZWNJ.
@internalComponent
*/
void INC039567_L(TDes& aText, CTestGraphicsDevice* aDevice, CTextView* aView)
	{
	aText = _L("\x628\x200C\x62D\x200C\x633");
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	for (TInt i = 0; i < aText.Length(); i += 2)
		{
		const TTestGCDisplayLine *line = aDevice->LineArray().Find(
			aText.Mid(i, 1));
		// Check that the line actually was drawn
		test(0 != line);
		const TDesC& displayed = line->iLineData;
		TInt pos = displayed.Locate(aText[i]);
		// Check that the line contains the text we expect. Failure here indicates
		// a failure of the test code.
		test(0 <= pos);
		// Check that the character is followed by a ZWNJ, if anything at all.
		// 0xFFFF intervening is OK
		for (TInt j = pos + 1;
			j != displayed.Length() && displayed[j] != KZeroWidthNonJoiner;
			++j)
			test(displayed[j] == 0xFFFF);
		// Check that the character is preceded by a ZWNJ, if anything at all.
		// 0xFFFF intervening is OK
		for (TInt k = pos;
			0 != k && displayed[k - 1] != KZeroWidthNonJoiner; --k)
			test(displayed[k - 1] == 0xFFFF);
		}
	}

/** Erroneous Cursor movement on top two lines

Tests that when the cursor is moved up to the top of the document and then up
one more to the start of the document, that the latent X position moves to the
start of the line as well.
@internalComponent
*/
void DEF061143_L(TDes& aText, CTextLayout* aLayout, CTextView* aView)
	{
	SetViewRect1(aView);
	aLayout->ForceNoWrapping(CTextLayout::EFParagraphsWrappedByDefault);
	aLayout->SetWrapWidth(aView->ViewRect().Width());
	aText = _L("First Line Second.");
	aView->HandleGlobalChangeL();
	TTmDocPos pos(15, ETrue);
	aView->SetDocPosL(pos);
	TCursorPosition::TMovementType move = TCursorPosition::EFLineUp;
	aView->MoveCursorL(move, EFalse);
	move = TCursorPosition::EFLineUp;
	aView->MoveCursorL(move, EFalse);
	aView->GetCursorPos(pos);
	test(pos.iPos == 0);
	move = TCursorPosition::EFLineDown;
	aView->MoveCursorL(move, EFalse);
	aView->GetCursorPos(pos);
	test(pos.iPos == 11);
	}

/** Page Down from last line moves the cursor to the start of the line. */
void DEF063340_L(TDes& aText, CTextLayout* aLayout, CTextView* aView)
	{
	SetViewRect1(aView);
	aLayout->ForceNoWrapping(CTextLayout::EFParagraphsWrappedByDefault);
	aLayout->SetWrapWidth(aView->ViewRect().Width());
	aText = _L("j");
	aView->HandleGlobalChangeL();
	TTmDocPos pos(1, EFalse);
	aView->SetDocPosL(pos);
	TCursorPosition::TMovementType move = TCursorPosition::EFPageDown;
	aView->MoveCursorL(move, EFalse);
	aView->GetCursorPos(pos);
	test(pos.iPos == 1);
	test(move == TCursorPosition::EFLineEnd);
	}

TBool LineIsEqual(CLineArray& aLineArray, TInt aLineNumber,
	const TDesC& aCandidate)
	{
	TPtrC line;
	line.Set( aLineArray.Line(aLineNumber).LineData() );
	while (1 < line.Length() && line[0] == 0xFFFF)
		line.Set( line.Mid(1) );
	while (1 < line.Length() && line[line.Length() - 1] == 0xFFFF)
		line.Set( line.Left(line.Length() - 1) );
	return 0 == aCandidate.Compare(line);
	}

_LIT(KClusterBreakTest1, "\xE01\xE33");
_LIT(KClusterBreakTest2, "citta\x300");
_LIT(KClusterBreakTest3, "\x928\x94d\x928");
/** @SYMTestCaseID SYSLIB-FORM-UT-1533
@SYMTestCaseDesc
	Test that clusters are not broken by segmented storage.
@SYMTestPriority High
@SYMTestActions
	Format some Devanagari with a text model that deliberately
	refuses to supply Form with whole syllables. Check that whole
	syllables are rendered.
@SYMTestExpectedResults No panics.
@SYMPREQ PREQ18 */
void TestClusterBreaks(TDes& aText, CTestGraphicsDevice* aDevice,
	CTextLayout* aLayout, CTextView* aView, TDocModel* aDocModel)
	{
	SetViewRect1(aView);
	aLayout->ForceNoWrapping(CTextLayout::EFParagraphsWrappedByDefault);
	aLayout->SetWrapWidth(aView->ViewRect().Width());
	aDevice->LineArray().Enable();
	TCharFormat dummy;
	TPtrC text;
	aText = KClusterBreakTest1();
	aDocModel->SetBreakPos(1);
	// Check TDocModel is actually doing the test correctly
	aDocModel->GetChars(text, dummy, 0);
	test(text.Length() == 1);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	test(LineIsEqual(aDevice->LineArray(), 0, KClusterBreakTest1));

	aText = KClusterBreakTest2();
	aDocModel->SetBreakPos(5);
	aDocModel->GetChars(text, dummy, 0);
	test(text.Length() == 5);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	test(LineIsEqual(aDevice->LineArray(), 0, KClusterBreakTest2));

	aText = KClusterBreakTest3();
	aDocModel->SetBreakPos(2);
	aDocModel->GetChars(text, dummy, 0);
	test(text.Length() == 2);
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	test(LineIsEqual(aDevice->LineArray(), 0, KClusterBreakTest3));

	aDocModel->SetBreakPos(0);
	}

/** DEF065322: Inconsistent parameter values in MFormCustomDraw::DrawBackground() */
const TInt KDEF065322_CharSize = 13; // height of char
const TInt KDEF065322_MaxCount = 3;
const TInt KDEF065322_RectSize = 100; // view is 100 by 100

class DEF065322_CustomDraw : public MFormCustomDraw
	{
public:
	void DrawBackground(const TParam &aParam, const TRgb &aBackground, TRect &aDrawn) const
		{
		TRgb ignore(aBackground);
		aDrawn = aParam.iDrawRect;

		// Test that there are 3 calls to this method and that the expected results are returned.
		test(count < KDEF065322_MaxCount);

		test(aDrawn.iTl.iX == 0);
		test(aDrawn.iTl.iY == count*KDEF065322_CharSize);
		test(aDrawn.iBr.iX == KDEF065322_RectSize);

		if(count < KDEF065322_MaxCount-1)
			{
			test(aDrawn.iBr.iY == (1+count)*KDEF065322_CharSize);
			}
		else
			{
			test(aDrawn.iBr.iY == KDEF065322_RectSize);
			}
		count++;
		}

		static TInt count;
	};

TInt DEF065322_CustomDraw::count = 0;

void DEF065322_L(TDes& aText, CTextLayout* aLayout, CTextView* aView)
	{
	// This text results in 3 calls to the DEF065322_CustomDraw::DrawBackground()
	// 1 call for each line of text (2) and 1 for remainder of the text view.
	aText = _L("12345678901234567890");
	TRect displayRect(0, 0, 100, 100);

	// custom draw
	DEF065322_CustomDraw customDraw;
	aLayout->SetCustomDraw(&customDraw);

	// create screen context
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTestGraphicsDevice* screenDevice = CTestGraphicsDevice::NewL(device->SizeInPixels(), 0);
	CleanupStack::PushL(screenDevice);
	screenDevice->SetLineArray(device->LineArray());
	CWindowGc* screenContext;
	User::LeaveIfError(screenDevice->CreateContext(screenContext));
	CleanupStack::PushL(screenContext);
	CTestTextView::SetOffScreenContext(aView, screenContext);

	// enable flicker free redraw
	aView->EnableFlickerFreeRedraw();

	// Cause DEF065322_CustomDraw::DrawBackground() to be executed 3 times
	aView->HandleGlobalChangeL();

	// reset counter
	DEF065322_CustomDraw::count = 0;

	// disable flicker free redraw
	aView->DisableFlickerFreeRedraw();

	// Cause DEF065322_CustomDraw::DrawBackground() to be executed 3 times
	aView->HandleGlobalChangeL();

	// remove custom draw
	aLayout->SetCustomDraw(NULL);

	// tidy up
	CTestTextView::SetOffScreenContext(aView, 0);
	CleanupStack::PopAndDestroy(screenContext);
	CleanupStack::PopAndDestroy(screenDevice);
	CleanupStack::PopAndDestroy(device);
	}

 // Side-bearings at the ends of chunks are now taken into account.
 void INC078304_L(TDes& aText, CTextView* aView)
 	{
 	// W has side-bearings of 1 pixel extending on each side.
 	// Waw (U+0648) has a left side-bearing of -5
 	// Therefore "WWW" has total width 1+10+10+10+1 == 32
 	// WawWaw has total width 5+10+10 == 25
 	SetViewRect1(aView);
 	aText = _L("WWW\x648\x648WWW\x2029\xff0f\xff0f\xff0f\x2029///");
 	aView->HandleGlobalChangeL();
 	TTmLineInfo line;
 	TTmPosInfo2 posInfo;
 	TTmDocPosSpec posSpec(1, TTmDocPosSpec::ETrailing);
 	aView->FindDocPosL(posSpec, posInfo, &line);
 	test(line.iInnerRect.iTl.iX == 0);
 	test(line.iInnerRect.iBr.iX == 89);
 	test(posInfo.iEdge.iX == 11);
 	posSpec.iPos = 2;
 	aView->FindDocPosL(posSpec, posInfo, 0);
 	test(posInfo.iEdge.iX == 21);
 	posSpec.iPos = 3;
 	aView->FindDocPosL(posSpec, posInfo, 0);
 	test(posInfo.iEdge.iX == 32);
 	posSpec.iType = TTmDocPosSpec::ELeading;
 	aView->FindDocPosL(posSpec, posInfo, 0);
 	test(posInfo.iEdge.iX == 57);
 	posSpec.iPos = 4;
 	posSpec.iType = TTmDocPosSpec::ETrailing;
 	aView->FindDocPosL(posSpec, posInfo, 0);
 	test(posInfo.iEdge.iX == 47);
 	posSpec.iPos = 5;
 	aView->FindDocPosL(posSpec, posInfo, 0);
 	test(posInfo.iEdge.iX == 32);
 	posSpec.iType = TTmDocPosSpec::ELeading;
 	aView->FindDocPosL(posSpec, posInfo, 0);
 	test(posInfo.iEdge.iX == 57);
 	posSpec.iPos = 6;
 	posSpec.iType = TTmDocPosSpec::ETrailing;
 	aView->FindDocPosL(posSpec, posInfo, 0);
 	test(posInfo.iEdge.iX == 68);
 	posSpec.iPos = 7;
 	aView->FindDocPosL(posSpec, posInfo, 0);
 	test(posInfo.iEdge.iX == 78);
 	posSpec.iPos = 8;
 	aView->FindDocPosL(posSpec, posInfo, 0);
 	test(posInfo.iEdge.iX == 89);

	// Test fullwidth solidus -- has a right side-bearing in our font
	// It is best if this goes 10, 20, 31
	// but it is acceptable for it to go 11, 22, 33
	posSpec.iPos = 10;
	aView->FindDocPosL(posSpec, posInfo, 0);
	TInt x1 = posInfo.iEdge.iX;
	test(x1 == 10 || x1 == 11);
	posSpec.iPos = 11;
	aView->FindDocPosL(posSpec, posInfo, 0);
	TInt x2 = posInfo.iEdge.iX;
	test(x2 - x1 == 10 || x2 - x1 == 11);
	posSpec.iPos = 12;
	aView->FindDocPosL(posSpec, posInfo, 0);
	test(posInfo.iEdge.iX - x2 == 11);

	// Test "///" -- / has a left side-bearing in our font
	// It is best if this goes 11, 21, 31
	// but it is acceptable for it to go 11, 22, 33
	posSpec.iPos = 14;
	aView->FindDocPosL(posSpec, posInfo, 0);
	test(posInfo.iEdge.iX == 11);
	posSpec.iPos = 15;
	aView->FindDocPosL(posSpec, posInfo, 0);
	x1 = posInfo.iEdge.iX;
	test(x1 == 21 || x1 == 22);
	posSpec.iPos = 16;
	aView->FindDocPosL(posSpec, posInfo, 0);
	x2 = posInfo.iEdge.iX;
	test(x2 - x1 == 10 || x2 - x1 == 11);
 	}

// Pen position now taken into account in bounds calculation/check.
void INC086257_L(TDes& aText, CTextLayout* aLayout, CTextView* aView)
	// Conditions setup to simulate the unwanted horiz scroll pre-fix
	// Fix will cause line-break instead
 	{
 	// Left-to-right
 	aText = _L("DDDDDDDDDD");
	aView->HandleGlobalChangeL();
	TPoint cursorPoint;
 	aView->DocPosToXyPosL(aLayout->TagmaTextLayout().EndChar()-1,cursorPoint);
	test(cursorPoint.iX==11); //break happened - cursor is one character from left margin
	test(cursorPoint.iY==23); //and on the next line
	// Right-to-left
 	aText = KTavSequence;  // right-to-left characters
	aView->HandleGlobalChangeL();
 	aView->DocPosToXyPosL(aLayout->TagmaTextLayout().EndChar()-1,cursorPoint);
	test(cursorPoint.iX==89); //break happened - cursor is one character from right margin
	test(cursorPoint.iY==23); //and on the next line
	}

// Remapped linebreak characters now conditionally taken into account in bounds calculation/check.
void INC087637_test(TDes& aText, CTextLayout* aLayout, CTextView* aView,
	TPoint& cursorPoint, TBool aLeft2Right, TUint aAppendChar=0)
// Set up conditions placed in subroutine to guarantee consistency
	{
	if(aLeft2Right)
		{
	 	aText = _L("DDDDDDDDDD");
		}
	else
		{
	 	aText = KTavSequence;  // right-to-left characters
		}
	if(aAppendChar)
		{
		aText.Append(aAppendChar);
		aView->HandleGlobalChangeL();
	 	aView->DocPosToXyPosL(aLayout->TagmaTextLayout().EndChar()-2,cursorPoint);
		}
	else
		{
		aView->HandleGlobalChangeL();
	 	aView->DocPosToXyPosL(aLayout->TagmaTextLayout().EndChar()-1,cursorPoint);
		}
	};
void INC087637_L(TDes& aText, CTextLayout* aLayout, CTextView* aView)
	// Conditions setup to simulate the linebreak character overhanging margin
	// Fix will cause wrap conditional upon flag set in layout object
 	{
 	TTestCustomRemap paradelim(CEditableText::EParagraphDelimiter);
 	TTestCustomRemap linebreak(CEditableText::ELineBreak);
 	TTestCustomRemap pagebreak(CEditableText::EPageBreak);
 	TTestCustomRemapper allbreaks;
 	TRect view(0, 0, 200, 200);
	aView->SetViewRect(view);
	aLayout->SetWrapWidth(102); // allow a couple of pixels to allow for linebreak char start
  	TPoint cursorPoint;
  	const TBool Left2Right = ETrue;
  	const TBool Right2Left = EFalse;
  	TTestCustomWrap customWrap;

	// Left-to-right

 	// First test status quo to show difference
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::ELineBreak);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::EPageBreak);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
	// Remap paragraph delimiter - still no difference because no custom wrap set
 	aLayout->SetCustomInvisibleCharacterRemapper(&paradelim);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::ELineBreak);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::EPageBreak);
 	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	// Set custom wrap - paragraph delimiter, and only paragraph delimiter breaks to a new line
 	aLayout->SetCustomWrap(&customWrap);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right);
	test(cursorPoint.iX==0);   //cursor is at left margin
	test(cursorPoint.iY==23);  //and on the second line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::ELineBreak);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::EPageBreak);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	// Remap linebreak character - linebreak character, and only linebreak character breaks to a new line
 	aLayout->SetCustomInvisibleCharacterRemapper(&linebreak);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::ELineBreak);
	test(cursorPoint.iX==0);   //cursor is at left margin
	test(cursorPoint.iY==23);  //and on the second line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::EPageBreak);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	// Remap pagebreak character - pagebreak character, and only pagebreak character breaks to a new line
 	aLayout->SetCustomInvisibleCharacterRemapper(&pagebreak);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::ELineBreak);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::EPageBreak);
	test(cursorPoint.iX==0);   //cursor is at left margin
	test(cursorPoint.iY==23);  //and on the second line
 	// Remap all linebreak characters - all test break to a new line
 	aLayout->SetCustomInvisibleCharacterRemapper(&allbreaks);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right);
	test(cursorPoint.iX==0);   //cursor is at left margin
	test(cursorPoint.iY==23);  //and on the second line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::ELineBreak);
	test(cursorPoint.iX==0);   //cursor is at left margin
	test(cursorPoint.iY==23);  //and on the second line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::EPageBreak);
	test(cursorPoint.iX==0);   //cursor is at left margin
	test(cursorPoint.iY==23);  //and on the second line
 	// With all linebreak characters remapped, reset custom wrap - no breaks occur
 	aLayout->SetCustomWrap(NULL);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::ELineBreak);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Left2Right,CEditableText::EPageBreak);
	test(cursorPoint.iX==101); //cursor is near right margin
	test(cursorPoint.iY==10);  //and on the first line

	// Right-to-Left

 	// First test status quo to show difference
 	aLayout->SetCustomInvisibleCharacterRemapper(NULL);// switch off character remapping
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::ELineBreak);
 	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::EPageBreak);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
	// Remap paragraph delimiter - still no difference because no custom wrap set
 	aLayout->SetCustomInvisibleCharacterRemapper(&paradelim);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::ELineBreak);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::EPageBreak);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	// Set custom wrap - paragraph delimiter, and only paragraph delimiter breaks to a new line
 	aLayout->SetCustomWrap(&customWrap);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left);
	test(cursorPoint.iX==102); //cursor is at right margin
	test(cursorPoint.iY==23);  //and on the second line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::ELineBreak);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::EPageBreak);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	// Remap linebreak character - linebreak character, and only linebreak character breaks to a new line
 	aLayout->SetCustomInvisibleCharacterRemapper(&linebreak);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::ELineBreak);
	test(cursorPoint.iX==102); //cursor is at right margin
	test(cursorPoint.iY==23);  //but on the second line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::EPageBreak);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	// Remap pagebreak character - pagebreak character, and only pagebreak character breaks to a new line
 	aLayout->SetCustomInvisibleCharacterRemapper(&pagebreak);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::ELineBreak);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::EPageBreak);
	test(cursorPoint.iX==102); //cursor is at right margin
	test(cursorPoint.iY==23);  //but on the second line
 	// Remap all linebreak characters - all test break to a new line
 	aLayout->SetCustomInvisibleCharacterRemapper(&allbreaks);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left);
	test(cursorPoint.iX==102); //cursor is at right margin
	test(cursorPoint.iY==23);  //but on the second line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::ELineBreak);
	test(cursorPoint.iX==102); //cursor is at right margin
	test(cursorPoint.iY==23);  //but on the second line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::EPageBreak);
	test(cursorPoint.iX==102); //cursor is at right margin
	test(cursorPoint.iY==23);  //but on the second line
 	// With all linebreak characters remapped, reset custom wrap - no breaks occur
 	aLayout->SetCustomWrap(NULL);
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::ELineBreak);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line
 	INC087637_test(aText,aLayout,aView,cursorPoint,Right2Left,CEditableText::EPageBreak);
	test(cursorPoint.iX==1);   //cursor is near left margin
	test(cursorPoint.iY==10);  //and on the first line

	// reset for next test
	aLayout->SetWrapWidth(100);
 	aLayout->SetCustomInvisibleCharacterRemapper(NULL);
	}

// No Memory mode entered twice on a certain leave unwinding
void DEF078967_L(TDes& aText, CTestGraphicsDevice* aDevice, CTextView* aView)
	{
	SetViewRect1(aView);
	aText = _L("some text is bigger than others");
	aView->HandleGlobalChangeL();
	TSize size(100, 100);
	CTestGraphicsDevice* offScreenDevice = CTestGraphicsDevice::NewL(size, 0);
	CleanupStack::PushL(offScreenDevice);
	CWindowGc* offScreenContext;
	User::LeaveIfError(offScreenDevice->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetOffScreenContext(aView, offScreenContext);
	aView->EnableFlickerFreeRedraw();
	TRect rect(size);
	CBitmapContext* bc;
	User::LeaveIfError(aDevice->CreateBitmapContext(bc));
	CleanupStack::PushL(bc);
	// Form can eat leaves, so we'll want a clear run of
	// (say) three before we'll stop testing.
	TInt ok = 0;
	for (TInt i = 1; ok < 3; ++i)
		{
		__UHEAP_FAILNEXT(i);
		TRAPD(error, aView->DrawL(rect, *bc));
		if (error == KErrNone)
			++ok;
		else
			ok = 0;
		__UHEAP_RESET;
		}
	CleanupStack::PopAndDestroy(bc);
	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(offScreenDevice);
	aView->DisableFlickerFreeRedraw();
	}

void INC092568_L(TDes& aText, CTextLayout* aLayout, CTextView* aView)
	{
	//fill up text buffer/ view
	aText = _L("1234567890123456789012345678901234567890123456789012");
	aText.Append(_L("34567890123456789012345678901234567890123456789"));
	aView->HandleGlobalChangeL();

	//set up test variable
	TInt end = aText.Length();
	TPoint viewableWindowTopLeft(0,0);
	TCursorSelection selectTop(0,0);
	TCursorSelection selectBottom(end,end);
	TTmPosInfo2 posInfo;

	//restrict viewable window so that scrolling is necessary
	aLayout->SetBandHeight(70);

	//test scrolling *without* SetPendingSelection
	aView->SetDocPosL(0); //scroll text down to top of document
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 0);
	aView->HandleInsertDeleteL(selectBottom,0,ETrue);
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 40); //text has scrolled up
	aView->SetDocPosL(0); //scroll text down to top of document
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 0); //text has scrolled down
	aView->HandleRangeFormatChangeL(selectBottom,ETrue);
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 40); //text has scrolled up

	//test *no* scrolling with SetPendingSelection
	//and selection reset by Handle.. methods
	aView->SetDocPosL(0);//scroll text down to top of document
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 0);
	aView->SetPendingSelection(selectTop);
	aView->HandleInsertDeleteL(selectBottom,0,ETrue);
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 0); //text has not scrolled
	aView->HandleRangeFormatChangeL(selectBottom,ETrue);
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 40); //text has scrolled up due to reset
	aView->SetDocPosL(0); //scroll text down to top of document
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 0);
	aView->SetPendingSelection(selectTop);
	aView->HandleRangeFormatChangeL(selectBottom,ETrue);
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 0); //text has not scrolled
	aView->HandleInsertDeleteL(selectBottom,0,ETrue);
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 40); //text has scrolled up due to reset

	//test *scrolling* with SetPendingSelection
	aView->SetDocPosL(0);//scroll text down to top of document
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 0);
	aView->SetPendingSelection(selectBottom);
	aView->HandleInsertDeleteL(selectTop,0,ETrue);
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 40); //text has scrolled up due to selectpending
	aView->SetDocPosL(0); //scroll text down to top of document
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 0);
	aView->SetPendingSelection(selectBottom);
	aView->HandleRangeFormatChangeL(selectTop,ETrue);
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 40); //text has scrolled up due to selectpending

	//reset for next test
	aView->SetDocPosL(0);
	}

void INC092257L(TDes& aText,CTextView* aView)
	{
 	aText.Zero();
 	aView->HandleGlobalChangeL();
 	aView->GetForwardDeletePositionL();
 	aView->GetBackwardDeletePositionL();
 	}

/**
@SYMTestCaseID          SYSLIB-FORM-CT-3422
@SYMTestCaseDesc	    "Scrolling works weirdly when replying to email".  This was a particular
						sequence of API calls under certain conditions that was causing an
						unexpected downward line scroll.
@SYMTestPriority 	    High
@SYMTestActions  	    Recreate the defect's pre-conditions and then call the API sequence.
@SYMTestExpectedResults At the end of the sequence, no downward line scroll should have occurred.
@SYMDEF                 PDEF109450
*/
void PDEF109450_L(TDes& aText, CTextLayout* aLayout, CTextView* aView)
	{
	// Text buffer/ view should be filled up and should include
	// a paragraph break after a few characters
	aText = _L("12345");
	aText.Append(_L("\x2029")); // para delim
	aText.Append(_L("6789012345678901234567890123456789012345678901"));
	aText.Append(_L("2345678901234567890123456789012345678901234567"));
	aView->HandleGlobalChangeL();

	// Set up defect preconditions
	TViewYPosQualifier viewYPosQualifier;
	viewYPosQualifier.SetFillScreen();
	viewYPosQualifier.SetMakeLineFullyVisible();
	aLayout->SetBandHeight(50); //restrict viewable window so that scrolling is necessary
	aLayout->SetAmountToFormat(CTextLayout::EFFormatBand);//set visible height to band height
	aView->SetDocPosL(0);//scroll to top of document
	TCursorPosition::TMovementType cursorMovement = TCursorPosition::EFLineDown;
	aView->MoveCursorL(cursorMovement,0);//move the cursor down one line from the top
	TTmDocPos docPos;
	aView->GetCursorPos(docPos);
	aText.Insert(docPos.iPos, _L("\x2029"));//insert a 2nd para break right after the 1st

	// Call the API sequence
	aView->SetPendingSelection(TCursorSelection(docPos.iPos,docPos.iPos));
	aView->HandleInsertDeleteL(TCursorSelection(docPos.iPos+1,docPos.iPos),1);
	aView->HandleGlobalChangeNoRedrawL(viewYPosQualifier);
	aView->HandleCharEditL(CTextLayout::EFParagraphDelimiter);

	// Test to ensure no scroll has occurred
	TPoint viewableWindowTopLeft(0,0);
	TTmPosInfo2 posInfo;
	aView->FindXyPosL(viewableWindowTopLeft, posInfo, NULL);
	test(posInfo.iDocPos.iPos == 0);//viewableWindowTopLeft should still be at doc start

	//reset for next test
	aView->SetDocPosL(0);
	}

void VerifyLineBreak(CTextView* aView,TInt aLines, TInt *aLineEnds)
	{
	CTextView::TTagmaForwarder forwarder;
  	forwarder.InitL(aView);

  	//force reformatting of text
  	aView->HandleGlobalChangeL();

  	//get the number of lines in the text
	TInt lines = forwarder.Lines();
	test(lines == aLines);

	//verify that the lines are broken in the correct places
	TTmLineInfo info;
	for(TInt index = 0;index < aLines;index++)
		{
		forwarder.LineNumberToLine(index,info);
		test(info.iEnd == aLineEnds[index]);
		}
	}
/**
@SYMTestCaseID          SYSLIB-FORM-CT-3478
@SYMTestCaseDesc	    Tests to ensure the correct line breaking behaviour when wrapping tabs
@SYMTestPriority 	    High
@SYMTestActions  	    Formats a series of strings using a variety of custom wrapping rules and verifies that
						strings with tabs are broken correctly
@SYMTestExpectedResults All strings should be broken as specified by the line breaking rules
@SYMDEF                 PDEF107440
*/
void PDEF107440_L(TDes& aText, CTextLayout* aLayout, CTextView* aView)
	{
	//This array defines the positions in the string where the lines should end
  	TInt lineEnds[2] = {10,17};

  	_LIT(KTestString1,"gggggg ggg\taaaaa");
  	_LIT(KTestString2,"gggggggggg\taaaaa");
  	_LIT(KTestString3,"gggggggggg\t aaaa");
  	_LIT(KTestString4,"gggggggggg \taaaa");

  	//Set the text string - the length of this string (actually the position
  	//of the tab within the string) is set so that the tab character should not
  	//fit properly into a single line
  	aText = KTestString1;

  	//This custom wrapper will allow breaking before the tab character
	TTestCustomWrap2 customWrap2;
  	aLayout->SetCustomWrap(&customWrap2);

  	//Check that the line is broken before the tab
  	VerifyLineBreak(aView, 2, lineEnds);

  	//This custom wrapper will allow the tab to hang
	TTestCustomWrap3 customWrap3;
  	aLayout->SetCustomWrap(&customWrap3);

  	aText = KTestString1;

  	//Check that the line is broken after the tab
  	lineEnds[0] = 11;
  	VerifyLineBreak(aView, 2, lineEnds);

  	//This custom wrapper will treat the tab as a space line breaking class
	TTestCustomWrap4 customWrap4;
  	aLayout->SetCustomWrap(&customWrap4);

  	//Check that the line is broken after the tab
  	lineEnds[0] = 11;
  	VerifyLineBreak(aView, 2, lineEnds);

	//Now change the text so that the tab is followed by a space character
  	aText = KTestString3;

  	//Verify that the line is broken after the space - as the space and tab are allowed to hang
  	lineEnds[0] = 12;
  	VerifyLineBreak(aView, 2, lineEnds);

  	//Now change the text so that the tab follows a space character
  	aText = KTestString4;

  	//Verify that the line is broken after the tab - as the space and tab are allowed to hang
  	VerifyLineBreak(aView, 2, lineEnds);

  	//reset to the default wrapper - this will not allow breaking
  	//before the tab and will not allow the tab to hang
  	aLayout->SetCustomWrap(NULL);

  	//Set back to the original string
  	aText = KTestString1;

  	//Verify that the line is broken after the space as the string cannot
  	// be legally broken before or after the tab
  	lineEnds[0] = 7;
  	VerifyLineBreak(aView, 2, lineEnds);

  	//Now change the text so that the line cannot be broken legally
  	//at the space character
  	aText = KTestString2;

  	//Verify that the line is broken before the tab - this is a forced break
  	lineEnds[0] = 10;
  	VerifyLineBreak(aView, 2, lineEnds);

  	//Now change the text so that the tab is followed by a space character
  	aText = KTestString3;

  	//Verify that the line is broken before the tab as the tab is not allowed to hang
  	//this is a forced break
  	lineEnds[0] = 10;
  	VerifyLineBreak(aView, 2, lineEnds);

  	//Now change the text so that the tab follows a space character
  	aText = KTestString4;

  	//Verify that the line is broken before the tab as space is allowed to hang
  	lineEnds[0] = 11;
  	VerifyLineBreak(aView, 2, lineEnds);
 	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-3521
@SYMTestCaseDesc        Testing the fix for INC108075: Marathi Input : Fallback rendered characters appears suddenly when we are insert
@SYMTestPriority        Medium
@SYMTestActions         Check that cverlapping characters in the text do not affect the line breaking
@SYMTestExpectedResults Lines should be broken at correct positions with ZWJ.
@SYMDEF                 INC108075
*/
void INC108075_L(TDes& aText, CTextLayout* aLayout, CTextView* aView)
	{
	// Ra+Virama(Halant)+ZWJ = Eyelash Ra
  	_LIT(KTestString1,"\x0930\x094D\x200D Hello, 12 abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
  	// Same as above, but replacing a space with Arabic letter Feh, this should cause an overlap of chunks
  	// at the ZWJ
  	_LIT(KTestString2,"\x0930\x094D\x200D\x0641Hello, 12 abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
	aLayout->SetWrapWidth(533);
	aText = KTestString1;
  	//This array defines the positions in the string where the lines should end
  	TInt lineEnds[2] = {14,67};
	//Verify that the line is broken in the correct places
  	VerifyLineBreak(aView, 2, lineEnds);
  	aText = KTestString2;
	//Verify that the line is broken in the correct places
  	VerifyLineBreak(aView, 2, lineEnds);
 	}

void TestTextViewL(TDes& aText, CTestGraphicsDevice* aDevice,
	CTextLayout* aLayout, CTextView* aView, TDocModel* aDocModel)
	{
	test.Start(_L("Test fix for defect TET_5D7MCV"));
	TET_5D7MCV_L(aText, aDevice, aView, aLayout);
	test.Next(_L("Test fix for defect INC020746"));
	INC020746_L(aText, aView);
	test.Next(_L("Test fix for defect DEF021603"));
	DEF021603_L(aText, aDevice, aView);
	test.Next(_L("Test fix for defect DEF022229"));
	DEF022229_L(aText, aDevice, aView);
	test.Next(_L("Test fix for defect DEF035472"));
	DEF035472_L(aText, aView);
	test.Next(_L("Test fix for defect INC036005"));
	INC036005_L(aText, aView);
	test.Next(_L("Test XyPosToDocPosL outside horizontal bounds"));
	TestCTextView_XyPosToDocPosL_ReturnsCorrectLine(aText, aView);
	test.Next(_L("Test fix for defect INC036809"));
	INC036809_L(aText, aView);
	test.Next(_L("Test fix for defect DEF037255"));
	DEF037255_L(aText, aView);
	test.Next(_L("Test fix for defect INC037293"));
	INC037293_L(aText, aView, aLayout);
	test.Next(_L("Test fix for defect INC039567"));
	INC039567_L(aText, aDevice, aView);
	test.Next(_L("Test fix for defect DEF061143"));
	DEF061143_L(aText, aLayout, aView);
	test.Next(_L("Test fix for defect DEF063340"));
	DEF063340_L(aText, aLayout, aView);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-1533 Test clusters are not broken "));
	TestClusterBreaks(aText, aDevice, aLayout, aView, aDocModel);
	test.Next(_L("Test fix for defect DEF065322"));
	DEF065322_L(aText, aLayout, aView);
	test.Next(_L("Test fix for defect DEF078967"));
	DEF078967_L(aText, aDevice, aView);
	test.Next(_L("Test fix for defect INC078304"));
	INC078304_L(aText, aView);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-1653 Test fix for defect INC080603 "));
	INC080603L(aText, aDevice, aLayout, aView);
	test.Next(_L("Test fix for defect INC086257"));
	INC086257_L(aText, aLayout, aView);
	test.Next(_L("Test fix for defect INC085809"));
	INC085809_L(aLayout, aView);
	test.Next(_L("Test fix for defect INC092568"));
	INC092568_L(aText, aLayout, aView);
	test.Next(_L("Test fix for defect INC087637"));
	INC087637_L(aText, aLayout, aView);
 	test.Next(_L("Test fix for defect INC092557/DEF094709"));
 	INC092257L(aText,aView);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-3422 Test fix for defect PDEF109450 "));
	PDEF109450_L(aText, aLayout, aView);
 	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-3478 Test fix for defect PDEF107440 "));
 	PDEF107440_L(aText, aLayout, aView);
 	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-3521 Test fix for defect INC108075 "));
 	INC108075_L(aText, aLayout, aView);
 	test.End();
	}

void DEF047281L(TDes& aText,CTextLayout* aLayout)
	{
	_LIT(KText, "ThisIsTheTestForDEF047281");
	aText.Zero();
	aText = KText;

	TSize aSize;
	TInt KHeight=0xD;
	aLayout->GetMinimumSizeL(200, aSize);
	test(aSize.iHeight == KHeight); // height of 1 line
	aLayout->GetMinimumSizeL(200, ETrue, aSize);
	test(aSize.iHeight == KHeight); // height of 1 line
	aLayout->GetMinimumSizeL(200, EFalse, aSize);
	test(aSize.iHeight == KHeight*2); // height of 2 lines
 	}


void TestTextViewParagraphL(TDes& aText, CTestGraphicsDevice* aDevice,
	CTextLayout* aLayout, CTextView* aView, CParaFormat* aParagraphFormat)
	{
	test.Start(_L("Test fix for defect INC038282"));
	INC038282_L(aText, aDevice, aLayout, aView, aParagraphFormat);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-1663 Test fix for defect DEF073913 "));
	DEF073913L(aText, aDevice, aView, aParagraphFormat);
	test.Next(_L("Test fix for defect DEF047281"));
	DEF047281L(aText, aLayout);

	test.End();
	}

void RunTestTextViewL()
	{
	TBuf<100> text;
	TDocModel docModel(text);
	TRect displayRect(0, 0, KDisplayWidth, KDisplayHeight);
	CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTextView* view = CTextView::NewL(layout, displayRect,
		device, device, 0, 0, 0);
	CleanupStack::PushL(view);
	TestTextViewL(text, device, layout, view, &docModel);
	CParaFormat* paragraph = CParaFormat::NewLC();
	docModel.SetParagraphFormat(paragraph);
	TestTextViewParagraphL(text, device, layout, view, paragraph);
	CleanupStack::PopAndDestroy(paragraph);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(layout);
	}

void RunDirectionalityTestL()
	{
	TBuf<100> text;
	TDocModel docModel(text);
	TRect displayRect(0, 0, KDisplayWidth, KDisplayHeight);
	CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTextView* view = CTextView::NewL(layout, displayRect,
		device, device, 0, 0, 0);
	CleanupStack::PushL(view);
	DirectionalityTestL(text, device, layout, view);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(layout);
	}

_LIT(KText_020329, "ABCDEFGHIJ ABCDEFGHIJ ABCDEFGHIJ ABC\x2029 ABCDEFGHIJ ABCDEFGHIJ ABCDEFGHIJ \x2029 ABCDEFGHIJ ABCDEFGHIJ ABCDEFGHIJ \x2029 ABCDEFGHIJ ABCDEFGHIJ ABCDEFGHIJ \x2029");

void INC020329TestL(TDes& aText, CTestGraphicsDevice* /*aDevice*/, CTextLayout* aLayout, CTextView* aView)
	{
	TSize smallSize(100, 40);
	CTestGraphicsDevice* offScreenDevice = CTestGraphicsDevice::NewL(smallSize, 0);
	CleanupStack::PushL(offScreenDevice);
	CWindowGc* offScreenContext;
	User::LeaveIfError(offScreenDevice->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetOffScreenContext(aView, offScreenContext);
	aView->EnableFlickerFreeRedraw();

	// This test procedure checks for the presence of defect INC020329.
	// To do this we need a small view (100x40) and two paragraphs where
	// the first is 3.5 lines long. The defect results in the cursor
	// being unable to move down a line to the second paragraph from
	// the last half line of paragraph one. The cursor first jumps to end
	// of paragraph (end of line 4) and then no further. This defect occurs
	// when band formatting is configured.

	TTestCustomWrap wrapAnywhere;
	TCursorPosition::TMovementType moveDown = TCursorPosition::EFLineDown;
	TPoint scrollBy;
	TTmDocPos cursorPos;

	// Setup view/layout state appropriate for the 1st part of test.
	// Insert text, setup wrapping on any character
	// Cursor doc pos is 0, formatting is EFFormatAllText not band

	aText = KText_020329;
	aLayout->SetCustomWrap(&wrapAnywhere);
	aLayout->DiscardFormat();
	aView->HandleGlobalChangeL();

	// TEST: Cursor movement with WHOLE text formatting

	scrollBy = aView->SetDocPosL(0); // =(0,10)

	scrollBy = aView->SetDocPosL(22); // =(0,0)

	scrollBy = aView->MoveCursorL(moveDown, EFalse); // =(0,-12), EFLineDown
	test(moveDown==TCursorPosition::EFLineDown);
	aView->GetCursorPos(cursorPos); // (32,1)
	test(cursorPos.iPos==32);

	scrollBy = aView->MoveCursorL(moveDown, EFalse); // =(0,-13), EFLineDown
	test(moveDown==TCursorPosition::EFLineDown);
	aView->GetCursorPos(cursorPos); // (39,1)
	test(cursorPos.iPos==39);

	scrollBy = aView->MoveCursorL(moveDown, EFalse); // =(0,-13), EFLineDown
	test(moveDown==TCursorPosition::EFLineDown);
	aView->GetCursorPos(cursorPos); // (49,1)
	test(cursorPos.iPos==49);

	// Setup view/layout state appropriate for the 2nd part of test.
	// Loose all formatting data from previous test
	// Cursor doc pos is 0, formatting is EFFormatBand only

	aLayout->DiscardFormat();
	aLayout->SetAmountToFormat(CTextLayout::EFFormatBand);
	test(aLayout->IsFormattingBand());
	aView->HandleGlobalChangeL();

	// TEST: Same cursor movement above BUT with Band formatting

	scrollBy = aView->SetDocPosL(0);

	scrollBy = aView->SetDocPosL(22);

	scrollBy = aView->MoveCursorL(moveDown, EFalse);
	test(moveDown==TCursorPosition::EFLineDown);
	aView->GetCursorPos(cursorPos); // (32,1)
	test(cursorPos.iPos==32);

	scrollBy = aView->MoveCursorL(moveDown, EFalse); // Defect: =(0,0), EFLineEnd!
	test(moveDown==TCursorPosition::EFLineDown);
	aView->GetCursorPos(cursorPos); // Defect: (36,1)
	test(cursorPos.iPos==39);

	scrollBy = aView->MoveCursorL(moveDown, EFalse); // Defect: =(0,0), EFNoMovement!
	test(moveDown==TCursorPosition::EFLineDown);
	aView->GetCursorPos(cursorPos); // Defect: (36,1)
	test(cursorPos.iPos==49);

	// Cleanup
	aLayout->SetCustomWrap(0);
	aText.Delete(0, aText.Length());

	CTestTextView::SetOffScreenContext(aView, 0);
	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(offScreenDevice);
	}

void RunTestINC020329L()
	{
	TBuf<200> text;
	TDocModel docModel(text);
	TRect displayRect(0, 0, 100, 40);
	CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTextView* view = CTextView::NewL(layout, displayRect,
		device, device, 0, 0, 0);
	CleanupStack::PushL(view);

	INC020329TestL(text, device, layout, view);

	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(layout);
	}

/**
@SYMTestCaseID          SYSLIB-FORM-CT-1861
@SYMTestCaseDesc	    Tests to ensure the correct operation of highlight extensions.
@SYMTestPriority 	    High
@SYMTestActions  	    Insert a paragraph of text, which contains at least 3 lines. Highlight different
						sections with different highlight extension settings and compare resulting
						screen with pre-defined bitmaps.
@SYMTestExpectedResults Screen and bitmaps should match
@SYMDEF                 PDEF085280
*/
void RunTestPDEF085280L()
	{
	//create editor/bitmap
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC(TRect(0, 0, 360, 94));

	//open the zipfile containing the comparison bitmaps
	CTestBitmapZipFileExtractor* extract = CTestBitmapZipFileExtractor::NewLC(
    		_L("Z:\\test\\app-framework\\form\\input\\bitmaps.zip"));

	//add some text
 	bitMap->AppendL(_L("the quick brown fox jumped over the lazy dog. 1234567890?!*"));
 	bitMap->View()->HandleGlobalChangeL();

	/*********************************************************
	Note:- The commented "SaveFileL" code below was used to originally
	generate the baseline test data.  It has been left in and commented
	so that if the data needs to be changed in the future, it will just
	be  case of temporarily moving the commenter from the "SaveFileL"
	to the "test".
	*********************************************************/

 	//Base-line test - no highlight extensions
 	bitMap->View()->SetDocPosL(27);
 	bitMap->View()->SetDocPosL(31,ETrue);
//	bitMap->SaveFileL(_L("c:\\SetHighlightExtensions(0,0,0,0).bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("SetHighlightExtensions(0,0,0,0).bmp"))));

 	//Shift highlight north-west
 	bitMap->View()->SetDocPosL(27);
	bitMap->View()->SetHighlightExtensions(25,-25,12,-12);
 	bitMap->View()->SetDocPosL(31,ETrue);
//	bitMap->SaveFileL(_L("c:\\SetHighlightExtensions(25,-25,12,-12).bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("SetHighlightExtensions(25,-25,12,-12).bmp"))));

 	//Shift highlight south-east
 	bitMap->View()->SetDocPosL(27);
	bitMap->View()->SetHighlightExtensions(-25,25,-12,12);
 	bitMap->View()->SetDocPosL(31,ETrue);
//	bitMap->SaveFileL(_L("c:\\SetHighlightExtensions(-25,25,-12,12).bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("SetHighlightExtensions(-25,25,-12,12).bmp"))));

 	//Shift highlight north-east
 	bitMap->View()->SetDocPosL(27);
	bitMap->View()->SetHighlightExtensions(-25,25,12,-12);
 	bitMap->View()->SetDocPosL(31,ETrue);
//	bitMap->SaveFileL(_L("c:\\SetHighlightExtensions(-25,25,12,-12).bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("SetHighlightExtensions(-25,25,12,-12).bmp"))));

 	//Shift highlight south-west
 	bitMap->View()->SetDocPosL(27);
	bitMap->View()->SetHighlightExtensions(25,-25,-12,12);
 	bitMap->View()->SetDocPosL(31,ETrue);
//	bitMap->SaveFileL(_L("c:\\SetHighlightExtensions(25,-25,-12,12).bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("SetHighlightExtensions(25,-25,-12,12).bmp"))));

 	//Make extensions huge and highlight one character - highlight blobs over nearly whole screen
 	bitMap->View()->SetDocPosL(29);
	bitMap->View()->SetHighlightExtensions(120,100,15,15);
 	bitMap->View()->SetDocPosL(30,ETrue);
//	bitMap->SaveFileL(_L("c:\\SetHighlightExtensions(120,100,15,15).bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("SetHighlightExtensions(120,100,15,15).bmp"))));

 	//Precursor to highlight overlap - skinny highlight through text
 	bitMap->View()->SetDocPosL(8);
	bitMap->View()->SetHighlightExtensions(0,0,-5,-5);
 	bitMap->View()->SetDocPosL(31,ETrue);
//	bitMap->SaveFileL(_L("c:\\SetHighlightExtensions(0,0,-5,-5).bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("SetHighlightExtensions(0,0,-5,-5).bmp"))));

 	//Highlight overlap - extension top and bottom overlaps between lines
 	bitMap->View()->SetDocPosL(8);
	bitMap->View()->SetHighlightExtensions(0,0,10,10);
 	bitMap->View()->SetDocPosL(31,ETrue);
//	bitMap->SaveFileL(_L("c:\\SetHighlightExtensions(0,0,10,10).bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("SetHighlightExtensions(0,0,10,10).bmp"))));

	CleanupStack::PopAndDestroy(2,bitMap);
	}

void PDEF097387TestL(CRichText* aRichText, CTextView* aTextView)
	{
	_LIT(KTest,"0123456789");
	TPtrC text1(KTest().Ptr(), KTest().Length());
	CTextLayout* layout = const_cast<CTextLayout*>(aTextView->Layout());
	aRichText->Reset();
	aRichText->InsertL(0, text1);

	// set line ascent and descent so baseline will be outside line rect
	aTextView->SetExcessHeightRequired(3);
	layout->SetMinimumLineDescent(0);
	aTextView->FormatTextL();

	// scroll through the document and test that the cursor goes to the right position
	// none of these tests should fail.
	TTmDocPos pos(9, ETrue);
	aTextView->SetDocPosL(pos);
	TCursorPosition::TMovementType move = TCursorPosition::EFLeft;
	aTextView->MoveCursorL(move, EFalse);
	aTextView->GetCursorPos(pos);
	test(pos.iPos == 8);
	aTextView->MoveCursorL(move, EFalse);
	aTextView->GetCursorPos(pos);
	test(pos.iPos == 7);
	aTextView->MoveCursorL(move, EFalse);
	aTextView->GetCursorPos(pos);
	test(pos.iPos == 6);
	move = TCursorPosition::EFLeft;
	aTextView->MoveCursorL(move, EFalse);
	aTextView->GetCursorPos(pos);
	test(pos.iPos == 5);
	move = TCursorPosition::EFLeft;
	aTextView->MoveCursorL(move, EFalse);
	aTextView->GetCursorPos(pos);
	test(pos.iPos == 4);
	move = TCursorPosition::EFLeft;
	aTextView->MoveCursorL(move, EFalse);
	aTextView->GetCursorPos(pos);
	test(pos.iPos == 3);
	move = TCursorPosition::EFLeft;
	aTextView->MoveCursorL(move, EFalse);
	aTextView->GetCursorPos(pos);
	test(pos.iPos == 2);
	move = TCursorPosition::EFLeft;
	aTextView->MoveCursorL(move, EFalse);
	aTextView->GetCursorPos(pos);
	test(pos.iPos == 1);
	move = TCursorPosition::EFLeft;
	aTextView->MoveCursorL(move, EFalse);
	aTextView->GetCursorPos(pos);
	test(pos.iPos == 0);
	}


void RunPDEF097387TestL(CFbsScreenDevice* aDevice)
	{
	CParaFormat* paraFormat = CParaFormat::NewLC();
	TParaFormatMask paraFormatMask;
	// set up paragraph propeties
	paraFormat->iHorizontalAlignment=CParaFormat::ELeftAlign;
	paraFormatMask.SetAttrib(EAttAlignment);
	paraFormat->iLineSpacingInTwips=21;
	paraFormatMask.SetAttrib(EAttLineSpacing);
	paraFormat->iLineSpacingControl=CParaFormat::ELineSpacingExactlyInPixels;
	paraFormatMask.SetAttrib(EAttLineSpacingControl);
	CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL(paraFormat,paraFormatMask);
	CleanupStack::PushL(paraFormatLayer);
	CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
	CleanupStack::PushL(charFormat);
	CRichText* text = CRichText::NewL(paraFormatLayer, charFormat);
	CleanupStack::PushL(text);
	TRect displayRect(0, 0, 70, 42);
	CTextLayout* layout = CTextLayout::NewL(text, displayRect.Width());
	CleanupStack::PushL(layout);
	CTextView* view = CTextView::NewL(layout, displayRect,
		aDevice, aDevice, 0, 0, 0);
	CleanupStack::PushL(view);
	//Run test
	PDEF097387TestL(text, view);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(layout);
	CleanupStack::PopAndDestroy(text);
	CleanupStack::PopAndDestroy(charFormat);
	CleanupStack::PopAndDestroy(paraFormatLayer);
	CleanupStack::PopAndDestroy(paraFormat);
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-3162
@SYMTestCaseDesc	    Tests to ensure the correct cursor position is maintained when scrolling through a document
@SYMTestPriority 	    High
@SYMTestActions  	    Creates a view and layout whose will have baselines outside the rect. of the line.
						It then tests that when scrolling through the document that the correct cursor position is maintained
@SYMTestExpectedResults Cursor position should be maintained correctly when scrolling through document.
@SYMDEF                 PDEF097387
*/
_LIT(KTestFont, "ClearlyU");
void SetupAndRunPDEF097387TestL()
	{
	TInt error;
	CFbsScreenDevice* screenDevice = 0;
	TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EColor16M));
	if (error == KErrNotSupported)
		TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EColor16MA));
	if (error == KErrNotSupported)
		TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EColor16MU));
	if (error == KErrNotSupported)
		TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EColor64K));
	if (error == KErrNotSupported)
		TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EColor4K));
	if (error == KErrNotSupported)
		TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EColor256));
	if (error == KErrNotSupported)
		TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EColor16));
	if (error == KErrNotSupported)
		TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EGray256));
	if (error == KErrNotSupported)
		TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EGray16));
	if (error == KErrNotSupported)
		TRAP(error, screenDevice = CFbsScreenDevice::NewL(0,EGray4));
	if (error == KErrNotSupported)
		screenDevice = CFbsScreenDevice::NewL(0,EGray2);	
	
	CleanupStack::PushL(screenDevice);
	screenDevice->ChangeScreenDevice(0);
	screenDevice->SetAutoUpdate(ETrue);
	CGraphicsContext* gc;
	User::LeaveIfError(screenDevice->CreateContext(gc));
	CleanupStack::PushL(gc);
	TFontSpec fs(KTestFont, 20);
	CFont* font;
	User::LeaveIfError(screenDevice->GetNearestFontInPixels(font, fs));
	TFontSpec fontSpec = font->FontSpecInTwips();
	if(0 != fontSpec.iTypeface.iName.Compare(KTestFont))
		{
		// Test font not found.
		User::Leave(KErrNotFound);
		}
	TRAP(error, RunPDEF097387TestL(screenDevice));
	CleanupStack::PopAndDestroy(gc);
	CleanupStack::PopAndDestroy(screenDevice);
	User::LeaveIfError(error);
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-3241
@SYMTestCaseDesc	    Test to ensure that invalid rectangle of the text view is being set
						correctly after removing of a line of text
@SYMTestPriority 	    High
@SYMTestActions  	    Creates a text document and view and layout objects for it.
						Then appends text to the document one character at a
						time until text starts to wrap to the second line. At this point
						the last character is being deleted, so that the text fits into
						one line once again. Then we get the invalid rectangle of the view
						and check that this rectangle includes the area where the second line
						used to be.
@SYMTestExpectedResults The location of the second line of text should be included into
						invalid area.
@SYMDEF                 PDEF098569
*/
void RunPDEF098569TestL()
	{
	//create editor/bitmap
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC();

	bitMap->AppendL(_L("a")); //insert first character to calculate first line's height
 	bitMap->View()->HandleCharEditL();
 	TInt FirstLineBottom = CTestTextView::GetFormattedHeight(bitMap->View());
 	//perform appending loop
 	do
 		{
 		bitMap->AppendL(_L("a"));
 		bitMap->View()->HandleCharEditL();
 		}
 	while (CTestTextView::GetFormattedHeight(bitMap->View())==FirstLineBottom );
 	TInt SecondLineBottom = CTestTextView::GetFormattedHeight(bitMap->View());

 	//delete last character
 	TInt last = bitMap->Layout()->DocumentLength() - 1;
 	TCursorSelection selection=TCursorSelection(last,last);
	bitMap->DocModel()->DelSetInsertCharFormatL(selection.LowerPos(),1);
	bitMap->View()->HandleCharEditL(CTextLayout::EFLeftDelete,EFalse);

 	//get the invalid area of the view object
 	TRect invalid_rect = CTestTextView::GetInvalidRect(bitMap->View());
 	//check that entire rectangle of the second line lies within the invalid area
 	test(invalid_rect.iTl.iY<=FirstLineBottom && invalid_rect.iBr.iY>=SecondLineBottom);

	CleanupStack::PopAndDestroy(bitMap);
	}

/**
@SYMTestCaseID				SYSLIB-FORM-UT-3345
@SYMTestCaseDesc			Test to ensure that implicit redraw is disabled until an external
							draw is performed.
@SYMTestPriority				High
@SYMTestActions				Create a view and layout object.
							Check BeginDraw() and EndDraw() are disabled.
							Do external draw.
							Check BeginDraw() and EndDraw() are enabled.
@SYMTestExpectedResults		Implicit redraw should be disabled until external draw
							is performed.
@SYMDEF					INC099424
*/
void RunINC099424TestL()
	{
	TBuf<100> text;
	TDocModel docModel(text);
	TRect displayRect(0, 0, KDisplayWidth, KDisplayHeight);
	CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTextView* view = CTextView::NewL(layout, displayRect,
		device, device, 0, 0, 0);
	CleanupStack::PushL(view);

	// This test uses CTextLayout::BeginRedrawCalled() to determine whether or not
	// CTextLayout::BeginRedraw() and CTextLayout::EndRedraw() are enabled.
	// CTextLayout::BeginRedrawCalled() returns ETrue if CTextLayout::iBeginRedrawCount
	// is greater than 0. Otherwise EFalse.
	// CTextLayout::BeginRedraw() and CTextLayout::EndRedraw() increments or
	// decrements iBeginRedrawCount respectively if they are enabled.

	TRect rect(0,0,0,0);

	// Check iBeginRedrawCount is 0 initially.
	// If next line is successful then iBeginRedrawCount must be 0
	// because it is initialised to this on construction.
	test(layout->BeginRedrawCalled() == EFalse);

	// Check begin and end functions are disabled
	layout->BeginRedraw(rect);
	test(layout->BeginRedrawCalled() == EFalse);
	// Doing this many times in case enabled and the CTextLayout::iBeginRedrawCount
	// was already negative.
	for(TInt i = 0; i < 20; i++)
		layout->EndRedraw();
	test(layout->BeginRedrawCalled() == EFalse);

	// Enable begin and end functions with external draw.
	// BeginRedrawCalled returns EFalse because DrawL call both begin and end
	// which increments then decrements iBeginRedrawCount
	view->DrawL(rect);
	test(layout->BeginRedrawCalled() == EFalse);

	// Test begin and end functions are enabled individually
	layout->BeginRedraw(rect);
	test(layout->BeginRedrawCalled() != EFalse);
	layout->EndRedraw();
	test(layout->BeginRedrawCalled() == EFalse);

	// tidy up
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(layout);
 	}

/**
@SYMTestCaseID          SYSLIB-FORM-CT-3754
@SYMTestCaseDesc	    A further test to ensure the correct operation of highlight extensions.
@SYMTestPriority 	    High
@SYMTestActions  	    Fill viewable area with multiple short lines of text. The next line
						to scroll into view should be a longer line of text followed by more
						short lines.  Ensure highlighting and band formatting are on.  Select
						the text from the beginning of the document until after the short line
						directly following the long line.
@SYMTestExpectedResults Screen and bitmaps should match - the whole of the long line should be
						highlighted.
@SYMDEF                 INC109995
*/
void RunINC109995TestL()
	{
	//create editor/bitmap
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC();

	//open the zipfile containing the comparison bitmaps
	CTestBitmapZipFileExtractor* extract = CTestBitmapZipFileExtractor::NewLC(
    		_L("Z:\\test\\app-framework\\form\\input\\bitmaps.zip"));

	//set up test conditions
	bitMap->Layout()->SetAmountToFormat();
	bitMap->Layout()->SetHighlightExtensions(0,0,-1,-1);

	// the offscreen bitmap.  This looks like an unexpected feature in its' own right.  The visual
	// effect, in this case, is that out of the three visible lines, only the
	// second line gets updated when scrolling with highlighting on.
	bitMap->View()->DisableFlickerFreeRedraw();

	//insert some text
	bitMap->AppendL(_L("11111\x2029"));
	bitMap->AppendL(_L("22222\x2029"));
	bitMap->AppendL(_L("33333\x2029"));
	bitMap->AppendL(_L("aaaaaaaaaaaaaaaaa\x2029"));
	bitMap->AppendL(_L("44444\x2029"));
	bitMap->AppendL(_L("55555\x2029"));
 	bitMap->View()->HandleGlobalChangeL();

	//highlight the text
	TCursorPosition::TMovementType lineDown = TCursorPosition::EFLineDown;
	bitMap->View()->SetDocPosL(0);
	bitMap->View()->MoveCursorL(lineDown,ETrue);
	bitMap->View()->MoveCursorL(lineDown,ETrue);
	bitMap->View()->MoveCursorL(lineDown,ETrue);
	bitMap->View()->MoveCursorL(lineDown,ETrue);
	bitMap->View()->MoveCursorL(lineDown,ETrue);

//	bitMap->SaveFileL(_L("c:\\ScrolledHighlight.bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("ScrolledHighlight.bmp"))));

	CleanupStack::PopAndDestroy(2,bitMap);
	}

/**
@SYMTestCaseID          SYSLIB-FORM-CT-4003
@SYMTestCaseDesc	    Test to ensure correct cursor movement between 2 lines containing a pictures.
@SYMTestPriority 	    High
@SYMTestActions  	    Inserts pictures until the line breaks, then moves the cursor leftwards for
						LTR text, and rightwards for RTL text, starting from the end of the 2nd line,
						testing the MovementType that has taken place and the position of the cursor
						after the movement, especially when the cursor moves to the first line.
@SYMTestExpectedResults Resulting document position are where expected.
@SYMDEF                 PDEF112004
*/
void RunPDEF112004TestL()
	{

	//create editor/bitmap
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC(TRect(0, 0, 140, 185));

	//create the picture to insert (this one is a red box)
	CTestPicture* pic = new(ELeave)CTestPicture();
 	CleanupStack::PushL(pic);
   	pic->SetSizeInTwips(TSize(400,400));

	//Add lots of pictures until we get a line break, and then format the view
	// Just to be sure, add one picture after we get a line break.
	TInt numLines = 0;
	TInt pos = 0;
	for (;;)
		{
		if (numLines > 1)
		break;
		bitMap->AppendL(pic);
		numLines = bitMap->View()->Layout()->NumFormattedLines();
		pos++;
		}
	bitMap->View()->HandleGlobalChangeL();

	// Place the cursor at the end of the last picture added and start moving the cursor left.
	TTmDocPos thisPos;
	bitMap->View()->GetCursorPos(thisPos);
	TCursorPosition::TMovementType type = TCursorPosition::EFLeft;

	// Docuemnt position is already at the end, but it is Trailing. Make it Leading and set it.
	TTmDocPosSpec::TType docPosType = TTmDocPosSpec::ELeading;
	TTmDocPosSpec docPos;
	docPos.iPos = pos;//thisPos.iPos;
	docPos.iType = docPosType;

	bitMap->View()->SetDocPosL(docPos);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == 4);
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);

	test(thisPos.iPos == 3 && type == TCursorPosition::EFLeft);
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == 3 && type == TCursorPosition::EFLeft);
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == 2 && type == TCursorPosition::EFLeft);
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == 1 && type == TCursorPosition::EFLeft);
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == 1 && type == TCursorPosition::EFLeft);

	/**	Now test the same for pictures in a RTL paragraph.
		Put some RTL text so that the paragraph changes to a RTL paragraph.
		Then move the cursor right, through the pictures */
	bitMap->AppendL(_L("\x630"));
 	bitMap->View()->HandleGlobalChangeL();

	docPos.iPos = bitMap->View()->Layout()->DocumentLength();
	docPos.iType = TTmDocPosSpec::ELeading;
	bitMap->View()->SetDocPosL(docPos);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == 5);

	type = TCursorPosition::EFRight;
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);

	test(thisPos.iPos == 4 && type == TCursorPosition::EFRight);
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);

	test(thisPos.iPos == 3 && type == TCursorPosition::EFRight);
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);

	test(thisPos.iPos == 3 && type == TCursorPosition::EFRight);
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);

	test(thisPos.iPos == 2 && type == TCursorPosition::EFRight);
	// Move the cursor and test if it has moved
	bitMap->View()->MoveCursorL(type, EFalse);
	bitMap->View()->GetCursorPos(thisPos);

	test(thisPos.iPos == 1 && type == TCursorPosition::EFRight);

	CleanupStack::Pop(pic);
	CleanupStack::PopAndDestroy(1);
	}


/**
@SYMTestCaseID          SYSLIB-FORM-CT-4001
@SYMTestCaseDesc	    Test to ensure correct picture selection and cursor movement in
						line of text containing a picture.
@SYMTestPriority 	    High
@SYMTestActions  	    Writes text wrapping to five lines and inserts pictures in the
						middle three lines and sets picture text alignment other than
						baseline, then tests that various cursor movements result in the
						correct doc positions. Also checks that a frame is drawn around the
						centre picture when expected.
@SYMTestExpectedResults Resulting document position are where expected; frame drawn correctly
@SYMDEF                 INC112423
*/
void RunINC112423TestVariantsL(const TDesC& aComparisonFileName, const TDesC& aText,
	TFontPresentation::TAlignment aAlignment, TCursorPosition::TMovementType aMove)
	{
	//open the zipfile containing the comparison bitmaps
	CTestBitmapZipFileExtractor* extract = CTestBitmapZipFileExtractor::NewLC(
    		_L("Z:\\test\\app-framework\\form\\input\\bitmaps.zip"));

	//create editor/bitmap
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC(TRect(0, 0, 160, 185), TCharFormat(_L("ClearlyU"),1));

	//create the picture to insert (this one is a red box)
	CTestPicture* pic = new(ELeave)CTestPicture();
 	CleanupStack::PushL(pic);
   	pic->SetSizeInTwips(TSize(400,400)); //pic needs to be higher than text height to catch all aligments

	//insert mixture of text and pictures
	bitMap->AppendL(aText);
	bitMap->AppendL(aText);
	bitMap->AppendL(aText);
	TInt firstPicPos = bitMap->DocModel()->DocumentLength(); //get cursor pos of first picture
	bitMap->AppendL(pic);
	bitMap->View()->HandleGlobalChangeL();

	//change picture alignment
	TInt docLength = bitMap->DocModel()->DocumentLength();
	TCursorSelection selection(docLength, docLength-1);
	bitMap->View()->SetSelectionL(selection);
	TCharFormat format;
	format.iFontPresentation.iPictureAlignment = aAlignment;
	TCharFormatMask mask;
	mask.SetAttrib(EAttFontPictureAlignment);
	bitMap->DocModel()->ApplyCharFormatL(format, mask, selection.LowerPos(), 1);
	bitMap->View()->HandleRangeFormatChangeL(selection,EFalse);
	bitMap->View()->CancelSelectionL();

	//insert more text and pictures
	bitMap->AppendL(aText);
	bitMap->AppendL(aText);
	TInt secondPicPos = bitMap->DocModel()->DocumentLength(); //get cursor pos of second picture
	bitMap->AppendL(pic);
	bitMap->AppendL(aText);
	bitMap->AppendL(aText);
	TInt thirdPicPos = bitMap->DocModel()->DocumentLength(); //get cursor pos of third picture
	bitMap->AppendL(pic);
	bitMap->AppendL(aText);
	bitMap->AppendL(aText);
	bitMap->AppendL(aText);
	bitMap->View()->HandleGlobalChangeL();

	//now move cursor over each picture and test for correct selection
	TTmDocPos thisPos;
	bitMap->View()->SetDocPosL(firstPicPos);
	bitMap->View()->MoveCursorL(aMove, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == firstPicPos); //cursor should be over the first picture
	bitMap->View()->SetDocPosL(secondPicPos);
	bitMap->View()->MoveCursorL(aMove, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == secondPicPos); //cursor should be over the second picture
//	bitMap->SaveFileL(aComparisonFileName); //and a frame should be drawn over it
	test(bitMap->CompareL(extract->BitmapFileL(aComparisonFileName)));
	bitMap->View()->SetDocPosL(thirdPicPos);
	bitMap->View()->MoveCursorL(aMove, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == thirdPicPos); //cursor should be over the third picture

	//Test up/down cursor movement
	TTmDocPos lastPos;
	TBool leadingEdge = aMove == TCursorPosition::EFRight;
	TCursorPosition::TMovementType move = TCursorPosition::EFLineDown;
	bitMap->View()->SetDocPosL(0); //go to top of document
	bitMap->View()->GetCursorPos(thisPos);
	lastPos = thisPos;
	TTmDocPos firstLineHome(thisPos.iPos, leadingEdge);
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos > lastPos.iPos); //cursor should have moved down
	TTmDocPos secondLineHome = lastPos = thisPos;
	TTmDocPos firstLineEnd(secondLineHome.iPos - 1, leadingEdge);
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos > lastPos.iPos); //cursor should have moved down again
	TTmDocPos thirdLineHome = lastPos = thisPos;
	TTmDocPos secondLineEnd(thirdLineHome.iPos - 1, leadingEdge);
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos > lastPos.iPos); //cursor should have moved down again
	TTmDocPos fourthLineHome = lastPos = thisPos;
	TTmDocPos thirdLineEnd(fourthLineHome.iPos - 1, leadingEdge);
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos > lastPos.iPos); //cursor should have moved down again
	TTmDocPos fifthLineHome = lastPos = thisPos;
	TTmDocPos fourthLineEnd(fifthLineHome.iPos - 1, leadingEdge);
	bitMap->View()->SetDocPosL(bitMap->DocModel()->DocumentLength()); //go to end of document
	bitMap->View()->GetCursorPos(thisPos);
	lastPos = thisPos;
	TTmDocPos fifthLineEnd(thisPos.iPos, leadingEdge);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineUp, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos < lastPos.iPos); //cursor should have moved up
	lastPos = thisPos;
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos < lastPos.iPos); //cursor should have moved up again
	lastPos = thisPos;
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos < lastPos.iPos); //cursor should have moved up again
	lastPos = thisPos;
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos < lastPos.iPos); //cursor should have moved up again

	//Test home/end cursor movement
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineBeg, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == firstLineHome);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineEnd, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == firstLineEnd);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineDown, EFalse);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineBeg, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == secondLineHome);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineEnd, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == secondLineEnd);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineDown, EFalse);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineBeg, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == thirdLineHome);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineEnd, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == thirdLineEnd);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineDown, EFalse);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineBeg, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == fourthLineHome);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineEnd, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == fourthLineEnd);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineDown, EFalse);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineBeg, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == fifthLineHome);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFLineEnd, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == fifthLineEnd);

	//Test at line extremes cursor movement left/right wraps to next/previous line
	move = aMove == TCursorPosition::EFRight? TCursorPosition::EFLeft : TCursorPosition::EFRight;
	bitMap->View()->SetDocPosL(firstLineEnd);
	bitMap->View()->MoveCursorL(aMove, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == secondLineHome);
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == firstLineEnd);
	bitMap->View()->SetDocPosL(secondLineEnd);
	bitMap->View()->MoveCursorL(aMove, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == thirdLineHome);
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == secondLineEnd);
	bitMap->View()->SetDocPosL(thirdLineEnd);
	bitMap->View()->MoveCursorL(aMove, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == fourthLineHome);
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == thirdLineEnd);
	bitMap->View()->SetDocPosL(fourthLineEnd);
	bitMap->View()->MoveCursorL(aMove, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == fifthLineHome);
	bitMap->View()->MoveCursorL(move, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos == fourthLineEnd);

	//Test pageup/pagedown
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFPageUp, EFalse);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFPageUp, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == firstLineHome.iPos);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFPageDown, EFalse);
	bitMap->View()->MoveCursorL(move = TCursorPosition::EFPageDown, EFalse);
	bitMap->View()->GetCursorPos(thisPos);
	test(thisPos.iPos == fifthLineEnd.iPos);

	CleanupStack::Pop(pic);
	CleanupStack::PopAndDestroy(2, extract);
	}
void RunINC112423TestL()
	{
	RunINC112423TestVariantsL(_L("alignArabicBot.bmp"), _L("\x630\x630\x630\x630\x630\x630\x630\x630 "),
		TFontPresentation::EAlignBottom, TCursorPosition::EFLeft);
	RunINC112423TestVariantsL(_L("alignArabicCen.bmp"), _L("\x630\x630\x630\x630\x630\x630\x630\x630 "),
		TFontPresentation::EAlignCentered, TCursorPosition::EFLeft);
	RunINC112423TestVariantsL(_L("alignArabicTop.bmp"), _L("\x630\x630\x630\x630\x630\x630\x630\x630 "),
		TFontPresentation::EAlignTop, TCursorPosition::EFLeft);
	RunINC112423TestVariantsL(_L("alignLatinBot.bmp"), _L("aaaaaaaa "),
		TFontPresentation::EAlignBottom, TCursorPosition::EFRight);
	RunINC112423TestVariantsL(_L("alignLatinCen.bmp"), _L("aaaaaaaa "),
		TFontPresentation::EAlignCentered, TCursorPosition::EFRight);
	RunINC112423TestVariantsL(_L("alignLatinTop.bmp"), _L("aaaaaaaa "),
		TFontPresentation::EAlignTop, TCursorPosition::EFRight);
	}

/**
@SYMTestCaseID          SYSLIB-FORM-CT-4005
@SYMTestCaseDesc        Given the usecase condition, test to ensure there is no highlighted
                        character following CTextView::CancelSelectionL();
@SYMTestPriority        High
@SYMTestActions         Create an empty document and append one character to it. Select this
                        character in the document and then cancel the selection.
@SYMTestExpectedResults Screen and bitmap should match - the character should not be highlighted.
@SYMDEF                 PDEF114862
*/
void RunPDEF114862TestL()
	{
	//create editor/bitmap
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC();

	//open the zipfile containing the comparison bitmaps
	CTestBitmapZipFileExtractor* extract = CTestBitmapZipFileExtractor::NewLC(
    		_L("Z:\\test\\app-framework\\form\\input\\bitmaps.zip"));

	//set up test conditions
	bitMap->AppendL(_L("@"));
	bitMap->View()->SetSelectionL(TCursorSelection(1,0));
	bitMap->View()->CancelSelectionL();

//	bitMap->SaveFileL(_L("c:\\MailboxWizard.bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("MailboxWizard.bmp"))));

	CleanupStack::PopAndDestroy(2, bitMap);
	}


/**
@SYMTestCaseID          SYSLIB-FORM-CT-4008
@SYMTestCaseDesc	    Make sure any highlighting is properly canceled when moving
                        the cursor without drag.
@SYMTestPriority 	    High
@SYMTestActions         Write some text to an empty document, highlight part of the text
                        and then move the cursor without selecting text.
@SYMTestExpectedResults No text should be highlighted following cursor move.
@SYMDEF                 INC116681
*/
void RunINC116681TestL()
	{
	//open the zipfile containing the comparison bitmaps
	CTestBitmapZipFileExtractor* extract = CTestBitmapZipFileExtractor::NewLC(
    		_L("Z:\\test\\app-framework\\form\\input\\bitmaps.zip"));

	//create editor/bitmap
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC();

 	//setup test conditions
 	bitMap->AppendL(_L("SomeText"));                       // enter some text
 	bitMap->View()->SetSelectionL(TCursorSelection(5,1));  //highlight some of it
	TCursorPosition::TMovementType moveRight = TCursorPosition::EFRight;
 	bitMap->View()->MoveCursorL(moveRight, EFalse);        //move cursor with drag off
//	bitMap->SaveFileL(_L("c:\\INC116681.bmp"));
	test(bitMap->CompareL(extract->BitmapFileL(_L("INC116681.bmp"))));

 	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-4018
@SYMTestCaseDesc	Performance test for formatting document with many pictures and test.
@SYMTestPriority 	Medium
@SYMTestActions         Add a lot pictures separated by commas and time the operation.
@SYMTestExpectedResults The formatting should not take longer than the specified timeout.
@SYMDEF                 PDEF123018
*/
void RunPDEF123018TestL()
	{
	// define a time unit for the performance test timeout
	const TInt KOneSecond = 1000000;
	//create editor/bitmap
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC(TRect(0, 0, 140, 185));
 	
	//create any old picture to insert
	CTestPicture* pic = new(ELeave)CTestPicture();
 	CleanupStack::PushL(pic);
   	pic->SetSizeInTwips(TSize(400,400));
   
   	// Add some text and pictures, and time it.
   	// Appending a picture will invoke the formatting.
   	// Many calls should cause a lot of reformatting which will show the performance.
   	TTime startTime;
   	startTime.HomeTime();
   	for (TInt i = 0; i < 50; i++)
   		{
		bitMap->AppendL(pic);
	   	bitMap->AppendL(_L(","));
   		}
   	TTime endTime;
   	endTime.HomeTime(); 
   	TTimeIntervalMicroSeconds diff = endTime.MicroSecondsFrom(startTime);
   	
   	// Different performance timeouts for emulator / hardware.
	TBuf<100> buf;
	#if defined __WINS__ || defined __WINSCW__
		buf.Format( _L("Testing on emulator: Timeout = 3 seconds; finished in %d microseconds"), diff.Int64() );
		RDebug::Print( buf );
		test(diff < 3*KOneSecond);
	#elif defined __ARMCC__
		// for naviengine SMP, the timeout should be 150% time needed compare with H4, 25m for H4, 38m for naviengine
		buf.Format( _L("Testing on hardware: Timeout = 38 seconds; finished in %d microseconds"), diff.Int64() );
		RDebug::Print( buf );
		test(diff < 38*KOneSecond);
	#endif
   	
   	// Cleanup.
	CleanupStack::Pop(pic);
	CleanupStack::PopAndDestroy(1);
	}

/**
@SYMTestCaseID          SYSLIB-FORM-CT-4019
@SYMTestCaseDesc        When a line in a document is filled with nothing but pictures
                        and the picture alignment is set to anything other than baseline
                        then the cursor should be correctly placed on that line.
@SYMTestPriority        Medium
@SYMTestActions         Fill a line up with pictures so that it wraps, place the cursor
                        at the beginning of that line and measure the line metrics.
@SYMTestExpectedResults The line metrics should be correct in order to place the cursor
                        in the expected position.
@SYMDEF                 DEF122198
*/
void RunDEF122198_1TestL() //picture is taller than font height
	{
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC(TRect(0, 0, 160, 80));

	//create the picture to insert (this one is a red box)
	CTestPicture* pic = new(ELeave)CTestPicture();
 	CleanupStack::PushL(pic);
   	pic->SetSizeInTwips(TSize(400,400));

	//insert line of pictures
	bitMap->AppendL(pic);
	bitMap->AppendL(pic);
	bitMap->AppendL(pic);
	bitMap->AppendL(pic);
	bitMap->AppendL(pic);
	bitMap->View()->HandleGlobalChangeL();
	
	//change picture alignment
	TCursorSelection selection(0, bitMap->DocModel()->DocumentLength());
	bitMap->View()->SetSelectionL(selection);
	TCharFormat format;
	format.iFontPresentation.iPictureAlignment = TFontPresentation::EAlignBottom;
	TCharFormatMask mask;
	mask.SetAttrib(EAttFontPictureAlignment);
	bitMap->DocModel()->ApplyCharFormatL(format, mask, selection.LowerPos(), selection.Length());
	bitMap->View()->HandleRangeFormatChangeL(selection,EFalse);
	bitMap->View()->CancelSelectionL();

	// I could find no way of getting a bitmap screenshot which showed the cursor.
	// All attempts - including rewriting bitmapDoc to use Wserv - failed.
	// Therefore, the best I can do is test the resulting line information against
	// which the position of the cursor is drawn.
	TTmPosInfo2 posInfo;
   	TTmLineInfo currentLineInfo;
	TTmDocPos docPos;
	bitMap->View()->SetDocPosL(0,EFalse);
	bitMap->View()->GetCursorPos(docPos);
   	bitMap->Layout()->FindDocPos(docPos, posInfo, &currentLineInfo);

   	test(posInfo.iEdge.iY==33);
	test(currentLineInfo.iBaseline==33);

   	//change picture alignment and test again
	format.iFontPresentation.iPictureAlignment = TFontPresentation::EAlignCentered;
	bitMap->View()->SetSelectionL(selection);
	bitMap->DocModel()->ApplyCharFormatL(format, mask, selection.LowerPos(), selection.Length());
	bitMap->View()->HandleRangeFormatChangeL(selection,EFalse);
	bitMap->View()->CancelSelectionL();
   	bitMap->Layout()->FindDocPos(docPos, posInfo, &currentLineInfo);
	
   	test(posInfo.iEdge.iY==29);
	test(currentLineInfo.iBaseline==29);

   	//change picture alignment and test again
	format.iFontPresentation.iPictureAlignment = TFontPresentation::EAlignTop;
	bitMap->View()->SetSelectionL(selection);
	bitMap->DocModel()->ApplyCharFormatL(format, mask, selection.LowerPos(), selection.Length());
	bitMap->View()->HandleRangeFormatChangeL(selection,EFalse);
	bitMap->View()->CancelSelectionL();
   	bitMap->Layout()->FindDocPos(docPos, posInfo, &currentLineInfo);

   	test(posInfo.iEdge.iY==25);
	test(currentLineInfo.iBaseline==25);
	
 	CleanupStack::PopAndDestroy(2);
	}

void RunDEF122198_2TestL() //picture is shorter than font height
	{
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC(TRect(0, 0, 160, 80));

	//create the picture to insert (this one is a red box)
	CTestPicture* pic = new(ELeave)CTestPicture();
 	CleanupStack::PushL(pic);
   	pic->SetSizeInTwips(TSize(400,200));

	//insert line of pictures
	bitMap->AppendL(pic);
	bitMap->AppendL(pic);
	bitMap->AppendL(pic);
	bitMap->AppendL(pic);
	bitMap->AppendL(pic);
	bitMap->View()->HandleGlobalChangeL();
	
	//change picture alignment
	TCursorSelection selection(0, bitMap->DocModel()->DocumentLength());
	bitMap->View()->SetSelectionL(selection);
	TCharFormat format;
	format.iFontPresentation.iPictureAlignment = TFontPresentation::EAlignBottom;
	TCharFormatMask mask;
	mask.SetAttrib(EAttFontPictureAlignment);
	bitMap->DocModel()->ApplyCharFormatL(format, mask, selection.LowerPos(), selection.Length());
	bitMap->View()->HandleRangeFormatChangeL(selection,EFalse);
	bitMap->View()->CancelSelectionL();

	// I could find no way of getting a bitmap screenshot which showed the cursor.
	// All attempts - including rewriting bitmapDoc to use Wserv - failed.
	// Therefore, the best I can do is test the resulting line information against
	// which the position of the cursor is drawn.
	TTmPosInfo2 posInfo;
   	TTmLineInfo currentLineInfo;
	TTmDocPos docPos;
	bitMap->View()->SetDocPosL(0,EFalse);
	bitMap->View()->GetCursorPos(docPos);
   	bitMap->Layout()->FindDocPos(docPos, posInfo, &currentLineInfo);
	test(posInfo.iEdge.iY==25);
	test(currentLineInfo.iBaseline==25);
	
 	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID          SYSLIB-FORM-CT-4020
@SYMTestCaseDesc        When a zero-width character precedes the current doc position,
                        pressing backspace should not delete everything back to the
                        beginning of the document.
@SYMTestPriority        Medium
@SYMTestActions         Recreate the reported usecase and test for the correct action,
                        then perform a similar test for every zero-width character.
@SYMTestExpectedResults The selection anchor point should be at the correct position
                        in each case.
@SYMDEF                 INC123427
*/
void RunINC123427TestL()
	{
	// *** First test actual defect usecase ***
 	CTestBitmapFile* bitMap = CTestBitmapFile::NewLC();
 	bitMap->AppendL(_L("a meeting\x2029"));
 	bitMap->AppendL(_L("\x200E\x0038:00 am - 8:00 am 18/06/2008\x2029"));
 	bitMap->AppendL(_L("\x200Ehere\x2029"));
 	bitMap->AppendL(_L("\x2029"));
 	bitMap->AppendL(_L("\x200ERepeats daily:\x2029"));
 	bitMap->AppendL(_L("\x200E\x0046rom 17/06/2008\x2029"));
 	bitMap->View()->HandleGlobalChangeL();
 	
 	bitMap->View()->SetDocPosL(48);
 	TCursorSelection selection = bitMap->View()->GetBackwardDeletePositionL();
 	test(selection.iAnchorPos==46);
 	
 	// *** Now reset the doc and test for all zero-width characters ***
 	bitMap->DocModel()->Reset();
 	bitMap->AppendL(_L("->")); //something valid to start
 	TText charVal;
 	TUint stringIndex=0;
 	TText string[70];

 	// create a continuous line containing every zero width character:
 	// add C0, C1 controls
 	for (charVal=0x1; charVal<0x9; charVal++)
 		{
 		string[stringIndex++] = charVal;
 		}
 	string[stringIndex++] = 0xB;
 	for (charVal=0xD; charVal<0x20; charVal++)
 		{
 		string[stringIndex++] = charVal;
 		}
 	string[stringIndex++] = 0x7F;
 	string[stringIndex++] = 0x81;
	string[stringIndex++] = 0x8F;
 	string[stringIndex++] = 0x90;

 	// add layout controls & invisible operators
 	for (charVal=0x200B; charVal<0x2010; charVal++)
 		{
 		string[stringIndex++] = charVal;
 		}
 	// U+205F is not zero width 
 	for (charVal=0x2060; charVal<0x2065; charVal++)
 		{
 		string[stringIndex++] = charVal;
 		}
 	for (charVal=0x206A; charVal<0x2070; charVal++)
 		{
 		string[stringIndex++] = charVal;
 		}

 	// add specials
 	for (charVal=0xFFF9; charVal<0xFFFD; charVal++)
 		{
 		string[stringIndex++] = charVal;
 		}

 	string[stringIndex] = 0; // zero terminate

 	TPtrC temp;
 	TTmCharFormat temp2;
	bitMap->AppendL(TBuf<70>(string));
	bitMap->AppendL(_L("<"));
 	bitMap->View()->HandleGlobalChangeL();
 	
 	// cycle through the doc at each zerowidth char and test correct backward
 	// delete position.  This test would have passed without the fix
 	TUint startpos=2;
 	for (TUint i=startpos; i<stringIndex+startpos; i++)
 		{
 	 	bitMap->View()->SetDocPosL(i);
 	 	selection = bitMap->View()->GetBackwardDeletePositionL();
 	 	test(selection.iAnchorPos==1);
 		}
 	
 	// same test again but with no valid characters at the start.
 	// This test would have passed without the fix
 	bitMap->DocModel()->Reset();
	bitMap->AppendL(TBuf<70>(string));
 	bitMap->View()->HandleGlobalChangeL();
 	for (TUint i=0; i<stringIndex; i++)
 		{
 	 	bitMap->View()->SetDocPosL(i);
 	 	selection = bitMap->View()->GetBackwardDeletePositionL();
 	 	test(selection.iAnchorPos==0);
 		}
	
 	// same test again but with a complete line of visible characters before the
 	// line of invisible ones. This test would *not* have passed without the fix
 	bitMap->DocModel()->Reset();
	bitMap->AppendL(_L("0123456789\x2029"));
	bitMap->AppendL(_L("0123456789\x2029"));
	bitMap->AppendL(TBuf<70>(string));
 	bitMap->View()->HandleGlobalChangeL();
 	startpos=23;
 	for (TUint i=startpos; i<stringIndex+startpos-1; i++)
 		{
 	 	bitMap->View()->SetDocPosL(i);
 	 	selection = bitMap->View()->GetBackwardDeletePositionL();
 	 	test(selection.iAnchorPos==21);
 		}
 	
 	CleanupStack::PopAndDestroy();
	}

TInt E32Main()
	{
	test.Title();
	static CTrapCleanup* TrapCleanup = CTrapCleanup::New();
	TInt error = RFbsSession::Connect();
	if (error == KErrNotFound)
		{
		FbsStartup();
		error = RFbsSession::Connect();
		}
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	test(error == KErrNone);
	test.Start(_L("CTextView tests"));
	TRAP(error, RunTestTextViewL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-4014 RunPDEF118831TestL "));
	TRAP(error, RunPDEF118831TestL());
	test(error == KErrNone);
	test.Next(_L("Directionality tests"));
	TRAP(error, RunDirectionalityTestL());
	test(error == KErrNone);
	test.Next(_L("Inline offsets test"));
	TRAP(error, CTestTextView::TestMemberOffsetsL());
	test(error == KErrNone);
	TRAP(error, CTestTextLayout::TestMemberOffsetsL());
	test(error == KErrNone);
	test.Next(_L("Test fix for defect INC020329"));
	TRAP(error, RunTestINC020329L());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-1861 Test fix for defect PDEF085280 "));
	TRAP(error, RunTestPDEF085280L());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-3162 Test fix for defect PDEF097387 "));
	TRAP(error, SetupAndRunPDEF097387TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-3241 Test fix for defect PDEF098569 "));
	TRAP(error, RunPDEF098569TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-3345 Test fix for defect INC099424 "));
	TRAP(error, RunINC099424TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-3754 Test fix for defect INC109995 & DEF118277 "));
	TRAP(error, RunINC109995TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-4003 Test fix for defect PDEF112004 "));
	TRAP(error, RunPDEF112004TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-4001 Test fix for defect INC112423 "));
	TRAP(error, RunINC112423TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-4005 Test fix for defect PDEF114862 "));
	TRAP(error, RunPDEF114862TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-4008 Test fix for defect INC116681 "));
	TRAP(error, RunINC116681TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-4019 Test fix for defect DEF122198 "));
	TRAP(error, RunDEF122198_1TestL());
	test(error == KErrNone);
	TRAP(error, RunDEF122198_2TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-4020 Test fix for defect INC123427 "));
	TRAP(error, RunINC123427TestL());
	test(error == KErrNone);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-4018 Test fix for defect PDEF123018"));
	TRAP(error, RunPDEF123018TestL());
	test(error == KErrNone);

	RFbsSession::Disconnect();
	delete scheduler;
	delete TrapCleanup;
	test.End();
	test.Close();
	return error;
	}
