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
* TTextView.cpp test file for UndoSystem classes
*
*/


#include <e32test.h>

#include <coemain.h>
#include <frmtview.h>
#include <txtrich.h>
#include <conpics.h>
#include <e32math.h>

#include "form_and_etext_editor.h"
#include "UndoSystem.h"
#include "EditorUndo.h"
#include "FRMPAGE.H"

_LIT(KHello, "hello world");
const TInt KPaginatePriority = -100;
const TInt KGranularity = 10;

using namespace UndoSystem;

LOCAL_C TInt GetNumber(TInt aMin, TInt aMax)
	{
	__ASSERT_ALWAYS(aMin <= aMax, User::Invariant());

	TInt64 seed = Math::Random();
	TReal randomReal = Math::FRand(seed);

	TReal maxReal = randomReal * ((aMax-aMin)+1);
	TReal rounded = 0;
	User::LeaveIfError(Math::Round(rounded, maxReal, 0));

	TInt result = rounded + aMin;
	
	if(result> aMax)
		{
		return aMax;
		}
	return result;
	}

void ManipulateText(CTextView* aView, CRichText* aText)
	{
	_LIT(KStartText, "The quick brown fox jumped over the lazy dog.");
	aText->InsertL(0, KStartText);
	aView->HandleInsertDeleteL(TCursorSelection(0, KStartText().Length()), 0);
	aText->InsertL(19, TChar(CEditableText::EParagraphDelimiter));
	aView->HandleCharEditL();
	TCharFormat format;
	TCharFormatMask mask;
	format.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	mask.ClearAll();
	mask.SetAttrib(EAttFontStrokeWeight);
	aText->ApplyCharFormatL(format, mask, 41, 3);
	aView->HandleRangeFormatChangeL(TCursorSelection(41, 44));
	aView->SetDocPosL(42);
	TInt length = aText->DocumentLength();
	aText->DeleteL(0, length);
	aView->HandleInsertDeleteL(TCursorSelection(0, 0), length);
	}

void ManipulateText1(CTextView* aView, CRichText* aText)
	{
	TInt testaYPos = 0;
	_LIT(KText, "The weather is quite good today, but it's a bit too cold");
	aText->InsertL(0, KText);
	aView->HandleInsertDeleteL(TCursorSelection(0, KText().Length()), 0);
	aView->SetDocPosL(10); //56 is the position of the last character d
	aView->SetViewLineAtTopL(1);
	aView->SetViewL(50, testaYPos);
	aView->HandleAdditionalCharactersAtEndL();
	}

inline TBool IsSurrogate(TText16 aInt16)
/**
@return True, if aText16 is high surrogate or low surrogate; false, otherwise.
*/
	{
	return (aInt16 & 0xF800) == 0xD800;
	}

class CMyTestPicture : public CPicture
	{
public:
	static CMyTestPicture* NewL(TSize aSize);
	void Draw(CGraphicsContext&,const TPoint&,const TRect&,MGraphicsDeviceMap*) const {}
	void ExternalizeL(RWriteStream& /*aStream*/) const {}
	void GetOriginalSizeInTwips(TSize& aSize) const {aSize=iSizeOfPicture;}
private:
	CMyTestPicture(TSize aSize);
private:
	TSize iSizeOfPicture;
	};

CMyTestPicture* CMyTestPicture::NewL(TSize aSize)
	{
	return new(ELeave) CMyTestPicture(aSize);
	}


CMyTestPicture::CMyTestPicture(TSize aSize):iSizeOfPicture(aSize)
	{}


class CTextViewTest : public CBase
	{
public:
	CTextViewTest(CCoeEnv& aEnv)
		: iEnv(aEnv), 	iWindowRect(10, 10, 110, 110), iWindow(aEnv.WsSession()),
		test(_L("TTextView - Some tests for CTextView")) {}
	void ConstructL();
	void InitializeL();
	void AddTextL(const TDesC&);
	void Destroy();
	~CTextViewTest();
	void TestL();
	void Test1L();
	void TestCancelSelectionL();
	void TestHandleAdditionalCharactersAtEndL();
	void TestFinishBackgroundFormattingL();
	void TestSetCursorVisibilityL();
	void TestSetSelectionVisibilityL();
	void TestEnablePictureFrameL();
	void TestSetCursorWidthTypeL();
	void TestParagraphRectL();
	void TestSetDocPosL();
	void TestSetViewLineAtTopL();
	void TestDrawL();
	void TestFormatTextL();
	void TestHandleCharEditL();
	void TestHandleRangeFormatChangeL();
	void TestHandleInsertDeleteL();
	void TestHandleGlobalChangeL();
	void TestHandleGlobalChangeNoRedrawL();
	void TestScrollDisplayL();
    void TestScrollDisplayPixelsL();
    void TestScrollDisplayPixelsNoLimitBordersL(TInt aOffset);
    void TestForDEF142286BounceScrollingL();
	void TestScrollDisplayLinesL();
	void TestScrollDisplayParagraphsL();
	void TestSetViewL();
	void TestMoveCursorL();
	void TestSetSelectionL();
	void TestMatchCursorHeightL();
	void TestCalculateHorizontalExtremesL();
	void TestXyPosToDocPosL();
//	void TestDrawL1();
	void TestGetPictureRectangleL();
	void TestGetPictureRectangle1L();
	void TestSetDisplayContextL();
	void TestGetPictureRectangleDefectL();
	void TestGetLineRectL(); // Test for defect WEP-567K9C Form panics when picture
							 // inserted in CRichText and alignmnent is set to bottom
	void TestForDEF003426L();
	void TestForDEF038488L();
	void InitializeDiffCharFormatL();
	void TestGetParaFormatL();
	// Function to test the Class TFormAndEtextEditor
	void FormAndEtextTestL();
	void TestForINC092725L();
	void TestForPDEF108443L();
	void TestForPDEF113755L();
	void TestForPDEF115165L();
	void TestForPDEF118443L();
	void TestForPDEF121798L();
	void TestForPDEF120239L();
	void TestForDEF124989L();
	void TestForDEF124975L();
	
	struct STestDataTInt4
		{
		TInt iDoc1;
		TInt iDoc2; 
		TInt iPos1;
		TInt iPos2;		
		};
	
private:
	CCoeEnv& iEnv;
	TRect iWindowRect;
	CParaFormatLayer* iParaLayer;
	CCharFormatLayer* iCharLayer;
	CRichText* iEtext;
	CTextLayout* iLayout;
	RWindow iWindow;
	CTextView* iView;
	RTest test;
	TCursorSelection select;
	TInt testDeltaY;
	TInt testDeltaLines;
	TInt testDeltaParas;
	TInt testaYPos;
	TCursorPosition::TMovementType testaMovement;
	TFontSpec testaFontSpec;
	TInt testaLeft;
	TInt testaRight;
	TPoint testaPoint;
	TBool testaCanScaleOrCrop;
	CBitmapContext* testaGc;
	TPoint testaXyPos;
	CBitmapDevice* testaGd;
	RWindowGroup testaGroupWin;
	RWsSession testaSession;
	};

void CTextViewTest::ConstructL()
	{
	iWindow.Construct(iEnv.RootWin(), 12345);
	iParaLayer = CParaFormatLayer::NewL();
	iCharLayer = CCharFormatLayer::NewL();
	test.Title();
	test.Start(_L("CTextView Tests:"));
	}

void CTextViewTest::InitializeL()
	{
	Destroy();
	iEtext = CRichText::NewL(iParaLayer, iCharLayer);
	iLayout = CTextLayout::NewL(iEtext, iWindowRect.Width());
	iView = CTextView::NewL(iLayout, iWindowRect, iEnv.ScreenDevice(),
		iEnv.SystemGc().Device(), &iWindow, &iEnv.RootWin(), &iEnv.WsSession());
	testaGd=(CBitmapDevice*) iEnv.SystemGc().Device();
	}

void CTextViewTest::AddTextL(const TDesC& aText)
	{
	iEtext->InsertL(0, aText);
	TCursorSelection s(0, aText.Length());
	iView->HandleInsertDeleteL(s, 0);
	}

void CTextViewTest::InitializeDiffCharFormatL()
	{
	Destroy();
	delete iCharLayer;
	iCharLayer=0;
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	charFormat.iFontPresentation.iPictureAlignment=TFontPresentation::EAlignBottom;
	charFormatMask.SetAttrib(EAttFontPictureAlignment);
	iCharLayer = CCharFormatLayer::NewL(charFormat,charFormatMask);
	iEtext = CRichText::NewL(iParaLayer, iCharLayer);
	iLayout = CTextLayout::NewL(iEtext, iWindowRect.Width());
	iView = CTextView::NewL(iLayout, iWindowRect, iEnv.ScreenDevice(),
		iEnv.SystemGc().Device(), &iWindow, &iEnv.RootWin(), &iEnv.WsSession());
	testaGd=(CBitmapDevice*) iEnv.SystemGc().Device();
	}

void CTextViewTest::Destroy()
	{
	delete iView;
	iView = 0;
	delete iLayout;
	iLayout = 0;
	delete iEtext;
	iEtext = 0;
	}


void CTextViewTest::TestL()
	{
	// Test for fix to ASR-4UYHZX: ETEXT panic 12 (ECharPosBeyondDocument) when
	// out of memory
	InitializeL();
	ManipulateText(iView, iEtext);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, ManipulateText(iView, iEtext));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::Test1L()
	{
	// testing functions SetViewL, SetViewLineAtTopL,
	// SetDocPosL & HandleAdditionalCharactersAtEndL
	// - should work but need some kind
	// of pre-settings and initialization before they can all be proved
	// that there is no memory leak
	InitializeL();
	ManipulateText1(iView, iEtext); //where all pre-settings have been done
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, ManipulateText1(iView, iEtext));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestCancelSelectionL()
	{
	InitializeL();
	iView->CancelSelectionL();
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->CancelSelectionL());
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestFinishBackgroundFormattingL()
	{
	InitializeL();
	iView->FinishBackgroundFormattingL();
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->FinishBackgroundFormattingL());
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestFormatTextL()
	{
	InitializeL();
	iView->FormatTextL();
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->FormatTextL());
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestSetCursorVisibilityL()
	{
	InitializeL();
	iView->SetCursorVisibilityL(1,1);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->SetCursorVisibilityL(1,1)); //TInt aLineCursor, TInt aTextCursor
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestSetSelectionVisibilityL()
	{
	InitializeL();
	iView->SetSelectionVisibilityL(1);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->SetSelectionVisibilityL(1)); //TBool aSelectionVisible
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestEnablePictureFrameL()
	{
	InitializeL();
	iView->EnablePictureFrameL(1);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->EnablePictureFrameL(1)); //TBool aEnabled
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestSetCursorWidthTypeL()
	{
	InitializeL();
	iView->SetCursorWidthTypeL(TTextCursor::ETypeRectangle,0);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->SetCursorWidthTypeL(TTextCursor::ETypeRectangle,0)); //TTextCursor::EType aType, TInt aWidth=0
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestParagraphRectL()
	{
	InitializeL();
	iView->ParagraphRectL(1);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->ParagraphRectL(1)); //TInt aDocPos
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestDrawL()
	{
	InitializeL();
	iView->DrawL(iWindowRect);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->DrawL(iWindowRect)); //TRect aRect
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestHandleRangeFormatChangeL()
	{
	InitializeL();
	iView->HandleRangeFormatChangeL(select, EFalse);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->HandleRangeFormatChangeL(select, EFalse));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestHandleInsertDeleteL()
	{
	InitializeL();
	iView->HandleInsertDeleteL(select, 1, EFalse);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->HandleInsertDeleteL(select, 1, EFalse));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestHandleGlobalChangeL()
	{
	InitializeL();
	iView->HandleGlobalChangeL(TViewYPosQualifier());
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->HandleGlobalChangeL(TViewYPosQualifier()));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestHandleGlobalChangeNoRedrawL()
	{
	InitializeL();
	iView->HandleGlobalChangeNoRedrawL(TViewYPosQualifier());
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->HandleGlobalChangeNoRedrawL(TViewYPosQualifier()));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestScrollDisplayL()
	{
	InitializeL();
	iView->ScrollDisplayL(TCursorPosition::EFLeft);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->ScrollDisplayL(TCursorPosition::EFLeft));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestScrollDisplayPixelsL()
	{
	InitializeL();
	iView->ScrollDisplayPixelsL(testDeltaY);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->ScrollDisplayPixelsL(testDeltaY));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestScrollDisplayPixelsNoLimitBordersL(TInt aOffset)
    { 
    /*
     * This test case is for new added function ScrollDisplayPixelsNoLimitBorderL() which support
     * no limit scrolling, using this function text view can be scrolled beyond the top and bottom 
     * border.
     * Text view will be firstly scrolled to border using ScrollDisplayPixelsL() which can't scroll
     * text view beyond the top or bottom border.
     * Then text view will be scrolled using ScrollDisplayPixelsNoLimitBorderL() to same direction.
     * Code will test this step that if text view is really scrolled beyond the border by checking 
     * the iBandTop position before and after the scrolling operation.*/
    
    InitializeL();
    _LIT(KTestParagraph, "This is a piece of text which is used to test the bounce scrolling feature made by s60.");
    for (TInt i=0;i<=20;i++)
        AddTextL(KTestParagraph);
    
    TInt firstBandTop, secondBandTop;
    TInt offset = aOffset;
    while( offset!=0 )
        {
        iView->ScrollDisplayPixelsL(offset);
        }
    offset = aOffset;
    firstBandTop = iLayout->PixelsAboveBand();
    iView->ScrollDisplayPixelsNoLimitBorderL(offset);
    secondBandTop = iLayout->PixelsAboveBand();
    test(firstBandTop - secondBandTop == offset);

    offset = 0 - aOffset;
    while( offset!=0 )
        {
        iView->ScrollDisplayPixelsL(offset);
        }
    offset = 0 - aOffset;
    firstBandTop = iLayout->PixelsAboveBand();
    iView->ScrollDisplayPixelsNoLimitBorderL(offset);
    secondBandTop = iLayout->PixelsAboveBand();
    test(firstBandTop - secondBandTop == offset);
    
    Destroy();
    }


void CTextViewTest::TestScrollDisplayLinesL()
	{
	InitializeL();
	iView->ScrollDisplayLinesL(testDeltaLines);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->ScrollDisplayLinesL(testDeltaLines));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestScrollDisplayParagraphsL()
	{
	InitializeL();
	AddTextL(KHello);
	iView->ScrollDisplayParagraphsL(testDeltaParas);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		AddTextL(KHello);
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->ScrollDisplayParagraphsL(testDeltaParas));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestSetViewL()
	{
	InitializeL();
	iView->SetViewL(1, testaYPos);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->SetViewL(1, testaYPos));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestMoveCursorL()
	{
	InitializeL();
	AddTextL(KHello);
	iView->MoveCursorL(testaMovement, EFalse);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		AddTextL(KHello);
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->MoveCursorL(testaMovement, EFalse));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestSetSelectionL()
	{
	InitializeL();
	iView->SetSelectionL(select);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->SetSelectionL(select));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestMatchCursorHeightL()
	{
	InitializeL();
	iView->MatchCursorHeightL(testaFontSpec);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->MatchCursorHeightL(testaFontSpec));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestCalculateHorizontalExtremesL()
	{
	InitializeL();
	iView->CalculateHorizontalExtremesL(testaLeft, testaRight, EFalse);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->CalculateHorizontalExtremesL(testaLeft, testaRight, EFalse));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestXyPosToDocPosL()
	{
	InitializeL();
	iView->XyPosToDocPosL(testaPoint);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->XyPosToDocPosL(testaPoint));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}

/*
void CTextViewTest::TestDrawL1()
	{
	InitializeL();
	iView->DrawL(iWindowRect, testaGc);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->DrawL(iWindowRect, testaGc));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}
*/


void CTextViewTest::TestGetPictureRectangleL()
	{
	InitializeL();
	iView->GetPictureRectangleL(1, iWindowRect, &testaCanScaleOrCrop);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->GetPictureRectangleL(1, iWindowRect, &testaCanScaleOrCrop));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestGetPictureRectangle1L()
	{
	InitializeL();
	iView->GetPictureRectangleL(testaXyPos, iWindowRect, &testaCanScaleOrCrop);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 100; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		TRAPD(err, iView->GetPictureRectangleL(testaXyPos, iWindowRect, &testaCanScaleOrCrop));
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestSetDisplayContextL()
	{
	CBitmapContext* atestGc;
	CBitmapContext* anotherGc;
	InitializeL();
	iView->SetDisplayContextL(testaGd, &iWindow, &testaGroupWin, &testaSession);
	Destroy();
	TInt consecutiveSuccesses = 0;
	for (TInt i = 1; consecutiveSuccesses != 1; ++i)
		{
		__UHEAP_MARK;
		InitializeL();
		__UHEAP_SETFAIL(RHeap::EDeterministic, i);
		atestGc=iView->BitmapContext();
		TRAPD(err, iView->SetDisplayContextL(testaGd, &iWindow, &testaGroupWin, &testaSession));
		anotherGc=iView->BitmapContext();
		if (err)
			{
			test(atestGc==anotherGc);
			}
		else
			{
			test(atestGc!=anotherGc);
			}
		Destroy();
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);
		if (err == KErrNone)
			++consecutiveSuccesses;
		else
			consecutiveSuccesses = 0;
		}
	}


void CTextViewTest::TestGetPictureRectangleDefectL()
	{
	/*
	_LIT(KParagraphAverage,"Some text to test that the Picture returned is in the correct position. The size of this paragraph should be reasonable, not too long not too short.");
	_LIT(KParagraphBigger,"Moulin Rouge: a film review. I don't know what I was expecting from Oulin Rouge, but what I got came as an even more of a surprise. The movie is a visual feast, a sing along extravaganza. The choices of songs is refreshing, with mix of Nirvan-Smells like teen spirit- to Madonna-Like a virgin-. Who knew that Ewan and Nicole could sing so well together.");
	_LIT(KParagraphSmall,"CBusBase is an abstract base class for dynamic memory buffers. You can read or write bytes.");
	InitializeL();
	TInt length;
	iEtext->InsertL(0,KParagraphAverage);
	iView->HandleInsertDeleteL(TCursorSelection(0, KParagraphAverage().Length()), 0);
	iEtext->InsertL(KParagraphAverage().Length(),TChar(CEditableText::EParagraphDelimiter));
	iView->HandleCharEditL();

	length=iEtext->LdDocumentLength();
	iEtext->InsertL(iEtext->LdDocumentLength(),KParagraphBigger);
	iView->HandleInsertDeleteL(TCursorSelection(length,KParagraphBigger().Length()), 0);

	length=iEtext->LdDocumentLength();
	iEtext->InsertL(iEtext->LdDocumentLength(),KParagraphSmall);
	iView->HandleInsertDeleteL(TCursorSelection(length,KParagraphSmall().Length()), 0);

	TSize size(100,100);
	TPoint xypos;
	CMyTestPicture* testpicture1 = CMyTestPicture::NewL(size);
	CMyTestPicture* testpicture2 = CMyTestPicture::NewL(size);
	CMyTestPicture* testpicture3 = CMyTestPicture::NewL(size);
	size.SetSize(200,200);
	CMyTestPicture* testpicture4 = CMyTestPicture::NewL(size);
	CMyTestPicture* testpicture5 = CMyTestPicture::NewL(size);
	CMyTestPicture* testpicture6 = CMyTestPicture::NewL(size);
	TPictureHeader mypic;
	TPictureHeader mypic2;
	//mypic.iPictureType = KUidXzePictureType;
	mypic.iPicture=testpicture1;
	mypic2.iPicture=testpicture2;
	// Testing the picture

	iEtext->InsertL(10,mypic2);
	mypic2.iPicture=testpicture1;
	iEtext->InsertL(40,mypic2);
	iView->DocPosToXyPosL(200,xypos);
	test.Printf(_L("The xy coords are %d & %d\n"),xypos.iX,xypos.iY);
	xypos.SetXY(78,55);
	TInt docpos;
	docpos=iView->XyPosToDocPosL(xypos);
	test.Printf(_L("The new docpos is %d\n"),docpos);
	TRect rect;
	TBool boo;
	boo=iView->GetPictureRectangleL(40,rect);
	test.Printf(_L("%d & %d \n"),rect.Size().iHeight,rect.Size().iWidth);
	if (boo)
		test.Printf(_L("Yes!"));
	else
		test.Printf(_L("Noo!"));
	*/
	}

void CTextViewTest::TestGetLineRectL()
	{

	_LIT(KSomeText,"Empty. Well this text has to now be something else. Maybe this will increase the height of the CTextLayout and if it does then");
	// calls the initializeDiffCharFormatL to set the new CharFormatLayer
	// which sets the picture alignment to be Bottom.
	InitializeDiffCharFormatL();
	// create test pictures to be inserted into the richtext object
	TSize size(100,100);
	CMyTestPicture* testpicture1 = CMyTestPicture::NewL(size);
	TPictureHeader tPicHeader;
	tPicHeader.iPictureType = KUidXzePictureType;
	tPicHeader.iPicture = testpicture1;
	test.Printf(_L("Created a picture\n"));

	// inserting some text & picture into the richtext object
	iEtext->InsertL(0,KSomeText);
	TInt startOfPicture;
	startOfPicture=iEtext->DocumentLength();
	iEtext->InsertL(startOfPicture,tPicHeader);
	test.Printf(_L("Inserted the picture in CRichText object \n"));

	//Call the guilty function
	TRect resultingRect;
	TInt endOfDocument = iEtext->DocumentLength();
	iView->FormatTextL();
	resultingRect=iLayout->GetLineRectL(startOfPicture,endOfDocument);
	}

void CTextViewTest::TestGetParaFormatL()
	{
	test.Next(_L("Starting GetParaFormatL test"));
	_LIT(KSomeText,"Empty. Well this text has to now be something else. Maybe this will increase the height of the CTextLayout and if it does then");
	InitializeL();
	// create the CParaFormat & TparaFormatMask
	// and set that in the iParaFormatLayer
	CParaFormat* paraFormat = CParaFormat::NewL();
	TParaFormatMask paraFormatMask;
	iParaLayer->SetL(paraFormat,paraFormatMask);
	iEtext->SetGlobalParaFormat(iParaLayer);
	iEtext->InsertL(0,KSomeText);
	iView->FormatTextL();
	// Create another CParaFormat & TParaFormatMask and set
	// some attributes to be different from the default.
	CParaFormat* anotherParaFormat = CParaFormat::NewL();
	TParaFormatMask anotherParaFormatMask;
	anotherParaFormat->iRightMarginInTwips=200;
	anotherParaFormatMask.SetAttrib(EAttRightMargin);
	anotherParaFormat->iLeftMarginInTwips=400;
	anotherParaFormatMask.SetAttrib(EAttLeftMargin);
	anotherParaFormat->iLineSpacingInTwips=300;
	anotherParaFormatMask.SetAttrib(EAttLineSpacing);
	anotherParaFormat->iHorizontalAlignment=CParaFormat::ERightAlign;
	anotherParaFormatMask.SetAttrib(EAttAlignment);

	//Now call CRichText::GetParaFormat using anotherParaFormat * Mask
	// and test that it is the same as paraFormat.
 	iEtext->GetParaFormatL(anotherParaFormat,anotherParaFormatMask,0,10);

	TInt result = anotherParaFormat->iRightMarginInTwips;
	test (result==0);

	result = anotherParaFormat->iLeftMarginInTwips;
	test (result==0);
	result = anotherParaFormat->iLineSpacingInTwips;
	test (result==200); // default value for iLineSpacingInTwips set in paraFormat is 200
	test (anotherParaFormat->iHorizontalAlignment == CParaFormat::ELeftAlign);

	TBool testresult;
	testresult=anotherParaFormat->IsEqual(*paraFormat);
	test(testresult);

	}


void CTextViewTest::TestForDEF003426L()
	{
	// Initialise CTextViewTest object for next test.
	InitializeL();
	test.Next(_L("Verifying CTextView::XyPosToDosPosL() WRT DEF003426"));

	// Insert a one line paragraph into EText object and reformat view.
	_LIT(KTestParagraph, "This is a piece of text to test the character positioning API based in X,Y cocordinates.");
	iEtext->InsertL(0, KTestParagraph);
	TCursorSelection sel(0, KTestParagraph().Length());
	(void) iView->HandleInsertDeleteL(sel, 0, EFalse);

	// Test XyPosToDocPosL() with coordinates beyond top left corner
	TInt docPos = -1;
	docPos = iView->XyPosToDocPosL(testaPoint);
	test(docPos == 0); // Should give char position of 0

	// Test XyPosToDocPosL() with coordinates beyond bottom right corner
	TRect viewRect = iView->ViewRect();
	viewRect.iBr.iX += 300;
	viewRect.iBr.iY += 111;
    docPos = iView->XyPosToDocPosL(viewRect.iBr);
	test(docPos != 0); // Should give char position of 88

	// Clean up test object
	Destroy();
	}

/*** Test code for DEF038858
	 " It isn't poss. to set via CTextView a TTmDocPos of iPos = 0; iLeadingEdge=false"
 */
void CTextViewTest::TestForDEF038488L()
	{
	// Initialise CTextViewTest object for next test.
	InitializeL();
	test.Next(_L("Verifying CTextView::SetDocPosL() DEF038858"));

	_LIT(KText, "This is the test for DEF038488");
	iEtext->InsertL(0, KText);
	iView->HandleInsertDeleteL(TCursorSelection(0, KText().Length()), 0);

	// Test SetDocPosL() with coordinates -1
	iView->SetDocPosL(-1);

	// check the value of iLeadingEdge
	TTmDocPos RawDocPos;
	iView->GetCursorPos(RawDocPos);
	test(RawDocPos.iLeadingEdge == EFalse);

	}

CTextViewTest::~CTextViewTest()
	{
	test.End();
	test.Close();
	delete iView;
	iWindow.Close();
	delete iLayout;
	delete iEtext;
	delete iCharLayer;
	delete iParaLayer;
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-1888
@SYMTestCaseDesc        Testing the Class TFormAndEtextEditor
@SYMTestPriority        Low
@SYMTestActions         Tests the API's of TFormAndEtextEditor by inserting the text, applying specific format and style to the text, getting the text,format and style and also deleting the same(text,format and style)
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void CTextViewTest::FormAndEtextTestL()
	{
	InitializeL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-1888 Testing TFormAndEtextEditor "));
	TCharFormatMask charMask;
    TCharFormat charFormat;
	charFormat.iFontSpec.iTypeface.iName = _S("Arial");
    charFormat.iFontSpec.iHeight = 240;
    charMask.SetAttrib(EAttFontTypeface);
    charMask.SetAttrib(EAttFontHeight);
    CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
    charFormatLayer->SetL(charFormat,charMask);
	TParaFormatMask paraFormatMask;
    CParaFormatLayer* paraFormatLayer=CParaFormatLayer::NewL((CParaFormat*)NULL,paraFormatMask);
    CParagraphStyle* paraStyle = CParagraphStyle::NewL(*paraFormatLayer,*charFormatLayer);
    CStyleList* styleList = CStyleList::NewL();
    RParagraphStyleInfo paraStyleInfo(paraStyle,paraStyle);
    paraStyleInfo.iStyle->iName=_L("Arial");
    paraStyleInfo.iStyleForNextPara->iName=_L("Arial");
    // Appending the new style to stylelist
    styleList->AppendL(&paraStyleInfo);

	iEtext->SetStyleListExternallyOwned(*styleList);
	iEtext->Reset();
	TFormAndEtextEditor newEditor(*iView,*iEtext);

	TTmCharFormatLayer charLayer;
	TTmCharFormatLayer charLayer1;
	TTmCharFormatMask charIMask;
	TTmCharFormat charI;
	RTmParFormatLayer parLayer;
	RTmParFormatLayer parLayer1;
	TTmParFormatMask parNMask;
	charIMask.iFlags = TTmCharFormatMask::EItalic;
	TOpenFontFaceAttribBase attrib;
	attrib.SetBold(EFalse);
	charI.iFontSpec.SetAttrib(attrib);
	parNMask.iFlags = TTmParFormatMask::EKeepWithNext;
	RTmParFormat parN;
	parN.iFlags = RTmParFormat::EKeepWithNext;
	charLayer.iMask = charIMask;
	charLayer.iFormat = charI;
	charLayer1=charLayer;
	parLayer.iMask = parNMask;

	parLayer.iFormat.CopyL(parN);
	TPtrC ptr1(_L("Arial"));
	// Inserting the text and applying the style specified(Arial)
	newEditor.InsertTextL(0, _L("Hello World"),&ptr1);

	// Setting the paragraph and character format explicitly
	newEditor.SetParFormatL(0,11,parLayer);
	newEditor.SetCharFormatL(0,11,charLayer);

	MUnifiedEditor::TFormatLevel level = MUnifiedEditor::EEffective;
	TInt runLen=11;
	// Getting the paragraph and character format
	newEditor.GetParFormatL(0,level,parLayer1,runLen);
	newEditor.GetCharFormat(0,level,charLayer1,runLen);

	// Deleting first 6 characters
	newEditor.DeleteTextL(0,6);
	// Deleting the paragraph and character format for the remaining text
	newEditor.DeleteParFormatL(0,5);
	newEditor.DeleteCharFormatL(0,5);

	TPtrC ptr;
	// Get the text into ptr. A paragraph seperator(\x2029) gets appended at the end of text.
	newEditor.GetText(0,ptr);
	test(ptr==_L("World\x2029"));

	RTmStyle style1;
	RParagraphStyleInfo paraStyleInfo1(paraStyle,paraStyle);
	paraStyleInfo1.iStyle->iName=_L("NewStyle");
	paraStyleInfo1.iStyleForNextPara->iName=_L("NewStyle");
	style1.CopyL(paraStyleInfo1);
	// Creating a new style and changing the current style to the new one.
	newEditor.StyleSupport()->CreateStyleL(style1);
	newEditor.StyleSupport()->ChangeStyleL(style1);

	RTmStyle style2;
	// Get the style by Name
	TInt retVal = newEditor.StyleSupport()->GetStyleByNameL(_L("Arial"),style2);
	retVal = newEditor.StyleSupport()->GetStyleByNameL(_L("NewStyle"),style2);
	// Get the style for a particular length of text
	newEditor.StyleSupport()->GetStyle(0,ptr,runLen);
	// Get the style by index
	retVal = newEditor.StyleSupport()->GetStyleByIndexL(1,style1);
	// Deleting the style
	newEditor.StyleSupport()->DeleteStyleL(_L("NewStyle"));
	retVal = newEditor.StyleCount();
	test(retVal==1);
	style1.Close();
	style2.Close();
	delete charFormatLayer;
    delete paraFormatLayer;
    delete styleList;
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-3347
@SYMTestCaseDesc        Testing the fix for INC092725: RF S60 3.2 Help: Touch and
						scrolling a topic down closing the program
@SYMTestPriority        High
@SYMTestActions         Run affected APIs passing scroll values that would put the display
						outside the formatted range.
@SYMTestExpectedResults First of all, the calls should not panic the process.
						Secondly, that the calls leave with the correct error code.
@SYMDEF                 INC092725
*/
void CTextViewTest::TestForINC092725L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-3347 Testing fix for INC092725 "));
	TInt err = KErrNone;
	InitializeL();
	AddTextL(KHello);

	//Scroll up
	iLayout->AdjustVerticalAlignment(CParaFormat::EAbsoluteRightAlign);
	TRAP(err, iView->ScrollDisplayL(TCursorPosition::EFLineUp, CTextLayout::EFAllowScrollingBlankSpace));
	test(err==CTextLayout::EPosNotFormatted);
	err=KErrNone;

	//Scroll down
	iLayout->AdjustVerticalAlignment(CParaFormat::EAbsoluteLeftAlign);
	TRAP(err, iView->ScrollDisplayL(TCursorPosition::EFLineDown, CTextLayout::EFAllowScrollingBlankSpace));
	test(err==CTextLayout::EPosNotFormatted);
	err=KErrNone;

	//Line scroll up
	TInt i = 105;
	iLayout->AdjustVerticalAlignment(CParaFormat::EAbsoluteRightAlign);
	TRAP(err, iView->ScrollDisplayLinesL(i, CTextLayout::EFAllowScrollingBlankSpace));
	test(err==CTextLayout::EPosNotFormatted);
	err=KErrNone;

	//Line scroll down
	i = -105;
	iLayout->AdjustVerticalAlignment(CParaFormat::EAbsoluteLeftAlign);
	TRAP(err, iView->ScrollDisplayLinesL(i, CTextLayout::EFAllowScrollingBlankSpace));
	test(err==CTextLayout::EPosNotFormatted);
	err=KErrNone;

	//Paragraph scroll up
	i = 105;
	iLayout->AdjustVerticalAlignment(CParaFormat::EAbsoluteRightAlign);
	TRAP(err, iView->ScrollDisplayParagraphsL(i, CTextLayout::EFAllowScrollingBlankSpace));
	test(err==CTextLayout::EPosNotFormatted);
	err=KErrNone;

	//Paragraph scroll down
	i = -105;
	iLayout->AdjustVerticalAlignment(CParaFormat::EAbsoluteLeftAlign);
	TRAP(err, iView->ScrollDisplayParagraphsL(i, CTextLayout::EFAllowScrollingBlankSpace));
	test(err==CTextLayout::EPosNotFormatted);
	Destroy();
	}


class CTestScreenDevice : public CPrinterDevice
	{
public:
    CTestScreenDevice(CWsScreenDevice* aDevice,RDrawableWindow& aWin);
	TDisplayMode DisplayMode() const {return iDevice->DisplayMode();}
	TSize SizeInPixels() const {return iDevice->SizeInPixels();}
	TSize SizeInTwips() const {return iDevice->SizeInTwips();}
    TInt NumTypefaces() const {return iDevice->NumTypefaces();}
    void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const
										{iDevice->TypefaceSupport(aTypefaceSupport,aTypefaceIndex);}
	TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const
									{return iDevice->FontHeightInTwips(aTypefaceIndex,aHeightIndex);}
	void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const
												{iDevice->PaletteAttributes(aModifiable,aNumEntries);}
	void SetPalette(CPalette* aPalette) {iDevice->SetPalette(aPalette);}
	TInt GetPalette(CPalette*& aPalette) const {return iDevice->GetPalette(aPalette);}
 	TInt CreateContext(CGraphicsContext *&aGc);
	TInt HorizontalTwipsToPixels(TInt aTwips) const {return iDevice->HorizontalTwipsToPixels(aTwips);};
	TInt VerticalTwipsToPixels(TInt aTwips) const {return iDevice->VerticalTwipsToPixels(aTwips);};
	TInt HorizontalPixelsToTwips(TInt aPixels) const {return iDevice->HorizontalPixelsToTwips(aPixels);};
	TInt VerticalPixelsToTwips(TInt aPixels) const {return iDevice->VerticalPixelsToTwips(aPixels);};
	TInt GetNearestFontInTwips(CFont*& aFont,const TFontSpec& aFontSpec) {return iDevice->GetNearestFontInTwips(aFont,aFontSpec);};
	void ReleaseFont(CFont* aFont) {iDevice->ReleaseFont(aFont);};
	TPrinterModelName ModelName() const {return _L("");}
	TUid ModelUid() const {TUid dummy; return dummy;}
	void CreateControlL(CPrinterPort* /*aPrinterPort*/) {}
	TPrinterModelEntry Model() const {return iModel;}
	TInt SetModel(const TPrinterModelHeader& /*aModel*/,CStreamStore& /*aStore*/) {return KErrNone;}
	TBool RequiresPrinterPort() {return EFalse;}
private:
	RDrawableWindow& iWin;
	CWsScreenDevice* iDevice;
	TPrinterModelEntry iModel;
	};

CTestScreenDevice::CTestScreenDevice(CWsScreenDevice* aDevice,RDrawableWindow& aWin):
	iWin(aWin)
	{
	iDevice=aDevice;
	iModel.iUid=TUid::Null();
	}

TInt CTestScreenDevice::CreateContext(CGraphicsContext*& aGc)
	{

	TInt ret=iDevice->CreateContext(aGc);
	((CWindowGc *) aGc)->Activate(iWin);
	return ret;
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-3496
@SYMTestCaseDesc        Testing the fix for PDEF108443: Two same content pages are printed when contact fields are exactly one page long
@SYMTestPriority        Medium
@SYMTestActions         Creates text paginators and tests resulting number of pages when there is text to 'full page - 1 line', full page and full page + 1 line of text.
@SYMTestExpectedResults Results should be 1 page, 1 page and 2 pages respectively.
@SYMDEF                 PDEF108443
*/
void CTextViewTest::TestForPDEF108443L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-3496 Testing fix for PDEF108443 "));
	CTestScreenDevice* screenDevice = new(ELeave) CTestScreenDevice(iEnv.ScreenDevice(),iWindow);

	TMargins margins;
	margins.iTop = 1440;
	margins.iBottom = 1440;
	margins.iLeft = 1440;
	margins.iRight = 1440;
	TSize s(11906,16838);
	TPageSpec p(TPageSpec::EPortrait, s);

	CArrayFixFlat<TInt>* charsPerPage1 = new(ELeave) CArrayFixFlat<TInt>(KGranularity);
	CTextPaginator* paginator1 = CTextPaginator::NewL(screenDevice, charsPerPage1, KPaginatePriority);
	paginator1->SetPageMarginsInTwips(margins);
	paginator1->SetPageSpecInTwips(p);

	CArrayFixFlat<TInt>* charsPerPage2 = new(ELeave) CArrayFixFlat<TInt>(KGranularity);
	CTextPaginator* paginator2 = CTextPaginator::NewL(screenDevice, charsPerPage2, KPaginatePriority);
	paginator2->SetPageMarginsInTwips(margins);
	paginator2->SetPageSpecInTwips(p);

	CArrayFixFlat<TInt>* charsPerPage3 = new(ELeave) CArrayFixFlat<TInt>(KGranularity);
	CTextPaginator* paginator3 = CTextPaginator::NewL(screenDevice, charsPerPage3, KPaginatePriority);
	paginator3->SetPageMarginsInTwips(margins);
	paginator3->SetPageSpecInTwips(p);


	// We need to find out the height of lines and print area of the page.
	// From this we determine how many lines *should* appear on the page.
	// This differs between devices(ie. hw and winscw).
	TRect textRect;
	textRect.iTl.iX=margins.iLeft;
	textRect.iTl.iY=margins.iTop;
	textRect.iBr.iX=s.iWidth-margins.iRight;
	textRect.iBr.iY=s.iHeight-margins.iBottom;
	
	textRect = screenDevice->TwipsToPixels(textRect);  //defect 131765, call the same func as paginator
	TInt pageHeight =  textRect.Height();
	_LIT(KDummyString,"AAAAAA");
	InitializeL();
	AddTextL(KDummyString);
	TInt lineHeight = 0;
	CParaFormat* paraFormat = CParaFormat::NewL();
	iEtext->GetParagraphFormatL(paraFormat,0);
	TBool pageBreakChar = EFalse;
	TInt docPos = 0;
	iLayout->FormatLineL(paraFormat,docPos,lineHeight,pageBreakChar);


	TInt numLines = pageHeight/lineHeight; // Number of lines expected on a page with paginator settings defined above and line height = 21
	TChar simpleChar('A');
	TBuf<200> string1;
	for (TInt i = 0; i < numLines-2; i++) // ...numlines - 1
		{
		string1.Append(simpleChar);
		string1.Append(CEditableText::EParagraphDelimiter);
		}
		string1.Append(simpleChar); // final line
	TBuf<200> string2;
	for (TInt i = 0; i < numLines-1; i++) // ...numlines
		{
		string2.Append(simpleChar);
		string2.Append(CEditableText::EParagraphDelimiter);
		}
		string2.Append(simpleChar); // final line
	TBuf<200> string3;
	for (TInt i = 0; i < numLines; i++) // ...numlines + 1
		{
		string3.Append(simpleChar);
		string3.Append(CEditableText::EParagraphDelimiter);
		}
		string3.Append(simpleChar); // final line

	InitializeL();
	AddTextL(string1);
	paginator1->SetDocumentL(iEtext);
	docPos=0;
	paginator1->AppendTextL(docPos);
	TInt numPages=paginator1->PaginationCompletedL();
	test(numPages==1);
	InitializeL();
	AddTextL(string2);
	paginator2->SetDocumentL(iEtext);
	docPos=0;
	paginator2->AppendTextL(docPos);
	numPages=paginator2->PaginationCompletedL();
	test(numPages==1);
	InitializeL();
	AddTextL(string3);
	paginator3->SetDocumentL(iEtext);
	docPos=0;
	paginator3->AppendTextL(docPos);
	numPages=paginator3->PaginationCompletedL();
	test(numPages==2);

	delete charsPerPage1;
	delete charsPerPage2;
	delete charsPerPage3;
	delete screenDevice;
	delete paraFormat;
	Destroy();
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-4002
@SYMTestCaseDesc	    Test to ensure the CTextView::SetSelectionVisibilityL will not panic when EFTextVisible
						is set off.
@SYMTestPriority 	    Normal
@SYMTestActions  	    Create a CTextView instance with EFTextVisible set off. Call SetSelectionVisibilityL(ETrue)
						and SetSelectionVisibilityL(EFalse).
@SYMTestExpectedResults	Given conditions in test actions, calling SetSelectionVisibilityL should not panic.
@SYMDEF                 PDEF113755
*/
void CTextViewTest::TestForPDEF113755L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-4002 Testing fix for PDEF113755 "));
	InitializeL();

	TCursorSelection selection(0,8); // length of selection must be >0
	iView->SetSelectionL(selection);

	iView->MakeVisible(ETrue);	//Test if the EFSelectionVisible flag is set correctly
	iView->SetSelectionVisibilityL(ETrue);
	test(iView->SelectionVisible());
	iView->SetSelectionVisibilityL(EFalse);
	test(!iView->SelectionVisible());

	iView->MakeVisible(EFalse);
	iView->SetSelectionVisibilityL(ETrue); //Should never panic form::1200 here
	iView->SetSelectionVisibilityL(EFalse);

	Destroy();
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-4004
@SYMTestCaseDesc	    Test for INC113143, to ensure CTextLayout::GetLineRectL returns the correct rectangle
						regarding the writting direction of text. Depend on Platform: WINSCW/H4/H6(DEF131765).
@SYMTestPriority 	    Normal
@SYMTestActions  	    Tested 16 scenarios that the CTextLayout::GetLineRectL could be call, also tested for
						edge cases such as 1 char, whole line, DocPos2 < DocPos1, etc..
@SYMTestExpectedResults CTextLayout::GetLineRectL should return expected rectangles.
@SYMDEF                 PDEF115165
*/
void CTextViewTest::TestForPDEF115165L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-4004 Testing fix for PDEF115165 "));
	InitializeL();

	TCharFormat charFormat = TCharFormat(_L("Arial"),100);
	TCharFormatMask charFormatMask;
	charFormatMask.SetAll();
	iCharLayer->SetL(charFormat, charFormatMask);
	iEtext->SetGlobalCharFormat(iCharLayer);
	_LIT(KLtoRChar,"a");
	_LIT(KRtoLChar,"\x6B2");
	_LIT(KLtoRText,"aaa");
	_LIT(KRtoLText,"\x6B2\x6B2\x6B2");
	_LIT(KParaSep, "\x2029");

	TRect rect;

//  Test for 16 scenarios of Bidi texts..
//	Sample text for test
//	Doc_Pos:    | 0| 1| 2| 5| 4| 3| 6| 7| 8|11|10| 9|
//	X-Coords:  |0| 5|10|15|20|25|30|35|40|45|50|55|60|  in case w=5

	iEtext->Reset();
	iEtext->InsertL(0,KLtoRText);
	iEtext->InsertL(iEtext->DocumentLength(),KRtoLText);
	iEtext->InsertL(iEtext->DocumentLength(),KLtoRText);
	iEtext->InsertL(iEtext->DocumentLength(),KRtoLText);
	iEtext->InsertL(iEtext->DocumentLength(),KParaSep);
	iView->FormatTextL();

	TPoint point1,point2;
	iLayout->DocPosToXyPosL(0,point1);
	iLayout->DocPosToXyPosL(1,point2);

	TInt wLTR = point2.iX - point1.iX;		//It depends on platform. WINSCW/H4 w=5; H6 w=4
	
	iLayout->DocPosToXyPosL(5,point1);
	iLayout->DocPosToXyPosL(4,point2);
	TInt wRTL = point2.iX - point1.iX;        //It depends on platform. WINSCW/H4 w=5; H6 w=4
	RDebug::Print(_L("wLTR %d,wRTL %d"), wLTR,wRTL);

	//  DocPos1 is LTR, DocPos1+1 is LTR, DocPos2 is LTR, DocPos2+1 is RTL
    rect = iLayout->GetLineRectL(0,2);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    test(rect.iTl.iX == 0 && rect.iBr.iX == 3*wLTR);

    //  DocPos1 is LTR, DocPos1+1 is LTR, DocPos2 is RTL, DocPos2+1 is RTL
    rect = iLayout->GetLineRectL(0,4);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    test(rect.iTl.iX == 0 && rect.iBr.iX == 3*wLTR + 2*wRTL);

    //  DocPos1 is LTR, DocPos1+1 is LTR, DocPos2 is RTL, DocPos2+1 is LTR
    rect = iLayout->GetLineRectL(0,5);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    test(rect.iTl.iX == 0 && rect.iBr.iX == 3*wLTR + wRTL);
    
    //	DocPos1 is LTR, DocPos1+1 is LTR, DocPos2 is LTR, DocPos2+1 is LTR
	rect = iLayout->GetLineRectL(0,7);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
	test(rect.iTl.iX == 0 && rect.iBr.iX == 3*wLTR + 3*wRTL + 2*wLTR);
	
//	DocPos1 is LTR, DocPos1+1 is RTL, DocPos2 is LTR, DocPos2+1 is LTR
	rect = iLayout->GetLineRectL(2,7);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
	test(rect.iTl.iX == 2*wLTR && rect.iBr.iX == 3*wLTR + 3*wRTL + 2*wLTR);

//	DocPos1 is LTR, DocPos1+1 is RTL, DocPos2 is LTR, DocPos2+1 is RTL
	rect = iLayout->GetLineRectL(2,8);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
	test(rect.iTl.iX == 2*wLTR && rect.iBr.iX == 3*wLTR + 3*wRTL + 3*wLTR);

//	DocPos1 is LTR, DocPos1+1 is RTL, DocPos2 is RTL, DocPos2+1 is LTR
	rect = iLayout->GetLineRectL(2,5);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
	test(rect.iTl.iX == 2*wLTR && rect.iBr.iX == 3*wLTR + wRTL);

//	DocPos1 is LTR, DocPos1+1 is RTL, DocPos2 is RTL, DocPos2+1 is RTL
	rect = iLayout->GetLineRectL(2,4);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
	test(rect.iTl.iX == 2*wLTR && rect.iBr.iX == 3*wLTR + 2*wRTL);

//	Sample text for test
//	Doc_Pos:     | 9|10|11| 8| 7| 6| 3| 4| 5| 2| 1| 0|
//	X-Coords:  |40|45|50|55|60|65|70|75|80|85|90|95|100|     in case w=5

	iEtext->Reset();
	iEtext->InsertL(0,KRtoLText);
	iEtext->InsertL(iEtext->DocumentLength(),KLtoRText);
	iEtext->InsertL(iEtext->DocumentLength(),KRtoLText);
	iEtext->InsertL(iEtext->DocumentLength(),KLtoRText);
	iEtext->InsertL(iEtext->DocumentLength(),KParaSep);
	iView->FormatTextL();
	
//	DocPos1 is RTL, DocPos1+1 is LTR, DocPos2 is LTR, DocPos2+1 is LTR
	rect = iLayout->GetLineRectL(2,4);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);	
	test(rect.iTl.iX == iWindowRect.Width() - 3*wRTL - 2*wLTR && rect.iBr.iX == iWindowRect.Width() - 2*wRTL);
			
//	DocPos1 is RTL, DocPos1+1 is LTR, DocPos2 is LTR, DocPos2+1 is RTL
	rect = iLayout->GetLineRectL(2,5);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);	
	test(rect.iTl.iX == iWindowRect.Width() - 3*wRTL - wLTR && rect.iBr.iX == iWindowRect.Width() - 2*wRTL);

//	DocPos1 is RTL, DocPos1+1 is LTR, DocPos2 is RTL, DocPos2+1 is LTR
	rect = iLayout->GetLineRectL(2,8);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);	
	test(rect.iTl.iX == iWindowRect.Width() - 3*wRTL - 3*wLTR - 3*wRTL && rect.iBr.iX == iWindowRect.Width() - 2*wRTL);

//	DocPos1 is RTL, DocPos1+1 is LTR, DocPos2 is RTL, DocPos2+1 is RTL
	rect = iLayout->GetLineRectL(2,7);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
	test(rect.iTl.iX == iWindowRect.Width() - 3*wRTL - 3*wLTR - 2*wRTL && rect.iBr.iX == iWindowRect.Width() - 2*wRTL);

//	DocPos1 is RTL, DocPos1+1 is RTL, DocPos2 is LTR, DocPos2+1 is LTR
	rect = iLayout->GetLineRectL(0,4);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);	
	test(rect.iTl.iX == iWindowRect.Width() - 3*wRTL - 2*wLTR && rect.iBr.iX == 100);

//	DocPos1 is RTL, DocPos1+1 is RTL, DocPos2 is LTR, DocPos2+1 is RTL
	rect = iLayout->GetLineRectL(0,5);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
	test(rect.iTl.iX == iWindowRect.Width() - 3*wRTL - wLTR && rect.iBr.iX == 100);

//	DocPos1 is RTL, DocPos1+1 is RTL, DocPos2 is RTL, DocPos2+1 is LTR
	rect = iLayout->GetLineRectL(0,8);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);	
	test(rect.iTl.iX == iWindowRect.Width() - 3*wRTL - 3*wLTR - 3*wRTL && rect.iBr.iX == 100);

//	DocPos1 is RTL, DocPos1+1 is RTL, DocPos2 is RTL, DocPos2+1 is RTL
	rect = iLayout->GetLineRectL(0,7);
	RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);	
	test(rect.iTl.iX == iWindowRect.Width() - 3*wRTL - 3*wLTR - 2*wRTL && rect.iBr.iX == 100);
	

	
//	Edge case tests
//	Sample text
//	1st Line:    | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|19|18|17|16|15|14|13|12|
//	X-Coords:  | 0| 5|10|15|20|25|30|35|40|45|50|55|60|65|70|75|80|85|90|95|100|   in case w=5
//	2nd Line:    |23|22|21|20|
	
//	Edge case tests
//	Sample text
//	1st Line:    | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|24|23|22|21|20|19|18|17|16|15|
//	X-Coords:  | 0| 4| 8|12|16|20|24|28|32|36|40|44|48|52|56|60|64|68|72|76|80|84|88|92|96|100|  w=4
//	2nd Line:    |29|28|27|26|25|24|23|22|21|20|
	
	TInt LtoRLength = (iWindowRect.Width() - 5*wRTL)/wLTR;
	TInt RtoLLength = 10;
	TInt RtoLinLine1= (iWindowRect.Width() - LtoRLength * wLTR)/wRTL;
	TInt Line2Start = LtoRLength + RtoLinLine1;
	
	iEtext->Reset();
	for(TInt i=0;i<LtoRLength;i++)
        {
        iEtext->InsertL(iEtext->DocumentLength(),KLtoRChar);
        }
	
	for(TInt i=0;i<RtoLLength;i++)
	    {
	    iEtext->InsertL(iEtext->DocumentLength(),KRtoLChar);
	    }	
	iEtext->InsertL(iEtext->DocumentLength(),KParaSep);
	iView->FormatTextL();

    for(TInt i=0;i<LtoRLength + RtoLLength;i++)
    {
        rect = iLayout->GetLineRectL(i,i+1);
        RDebug::Print(_L("%d: iTl.iX %d,iY %d, iBr.iX %d,iY %d"), i,rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    }          
    
    rect = iLayout->GetLineRectL(0,LtoRLength);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);    
    test(rect.iTl.iX == 0 && rect.iBr.iX == LtoRLength*wLTR + RtoLinLine1*wRTL ); //Line 1
  
    rect = iLayout->GetLineRectL(Line2Start, LtoRLength + RtoLLength);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    test(rect.iTl.iX == 0 && rect.iBr.iX == (RtoLLength - RtoLinLine1)*wRTL); //Line 2

    rect = iLayout->GetLineRectL(0,0);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);    
    test(rect.iTl.iX == 0 && rect.iBr.iX == wLTR); //first char
    
    //firt char of RTL
    rect = iLayout->GetLineRectL(LtoRLength,LtoRLength);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);    
    test(rect.iTl.iX == LtoRLength*wLTR + (RtoLinLine1-1)*wRTL && rect.iBr.iX == LtoRLength*wLTR + RtoLinLine1*wRTL); //end of line 1

    //middle of L to R
    rect = iLayout->GetLineRectL(LtoRLength/2,LtoRLength/2);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);    
    test(rect.iTl.iX == LtoRLength/2 * wLTR && rect.iBr.iX == (LtoRLength/2+1) * wLTR);
      
    //middle of LTR to first of RTL
    rect = iLayout->GetLineRectL(LtoRLength/2,LtoRLength);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);    
    test(rect.iTl.iX == LtoRLength/2 * wLTR && rect.iBr.iX == LtoRLength*wLTR + RtoLinLine1*wRTL);
 
    //second of RTL to start of 2nd line ??
    rect = iLayout->GetLineRectL(LtoRLength+1,Line2Start);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);    
    test(rect.iTl.iX == LtoRLength*wLTR + (RtoLinLine1-1)*wRTL && rect.iBr.iX == LtoRLength*wLTR + RtoLinLine1*wRTL); //end of line 1
  
    //middle of L to R
    rect = iLayout->GetLineRectL(LtoRLength/2,LtoRLength/2-1);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);    
    test(rect.iTl.iX == LtoRLength/2 * wLTR && rect.iBr.iX == LtoRLength*wLTR + RtoLinLine1*wRTL);
    
		
//	Test for edge cases while two lines are in different direction
//	Sample text
//	1st Line:    | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|
//	X-Coords:  | 0| 5|10|15|20|25|30|35|40|45|50|55|60|65|70|75|80|85|90|95|100|  in case w=5
//	2nd Line:    |39|38|37|36|35|34|33|32|31|30|29|28|27|26|25|24|23|22|21|20|
		
//	Test for edge cases while two lines are in different direction
//  Sample text
//	1st Line:    | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|
//	X-Coords:  | 0| 4| 8| 12|16|20|24|28|32|36|40|44|48|52|56|60|64|68|72|76|80|84|88|92|96|100|  w=4
//	2nd Line:    |49|48|47|46|45|44|43|42|41|40|39|38|37|36|35|34|33|32|31|30|29|28|27|26|25|
    
    LtoRLength = iWindowRect.Width()/wLTR;
    RtoLLength = iWindowRect.Width()/wRTL;
        
    iEtext->Reset();
    for(TInt i=0;i<LtoRLength;i++)
        {
        iEtext->InsertL(iEtext->DocumentLength(),KLtoRChar);
        }
    
    for(TInt i=0;i<RtoLLength;i++)
        {
        iEtext->InsertL(iEtext->DocumentLength(),KRtoLChar);
        }       
	iEtext->InsertL(iEtext->DocumentLength(),KParaSep);
	iView->FormatTextL();

    for(TInt i=0;i<LtoRLength + RtoLLength;i++)
    {
        rect = iLayout->GetLineRectL(i,i+1);
        RDebug::Print(_L("%d: iTl.iX %d,iY %d, iBr.iX %d,iY %d"), i,rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    }  
 
    
    //1st line
    rect = iLayout->GetLineRectL(3, LtoRLength-1);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    test(rect.iTl.iX == 3*wLTR && rect.iBr.iX == LtoRLength*wLTR); //Line 2

    //2nd line
    rect = iLayout->GetLineRectL(LtoRLength,LtoRLength + RtoLLength -2);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    test(rect.iTl.iX == wRTL && rect.iBr.iX == RtoLLength*wRTL); //Line 2

    //end of 1st line
    rect = iLayout->GetLineRectL(LtoRLength-1, LtoRLength-1);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    test(rect.iTl.iX == (LtoRLength-1)*wLTR && rect.iBr.iX == LtoRLength*wLTR); //Line 2

    //start of 2nd line
    rect = iLayout->GetLineRectL(LtoRLength, LtoRLength);
    RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
    test(rect.iTl.iX == (RtoLLength-1)*wRTL && rect.iBr.iX == RtoLLength*wRTL); //Line 2

     //1st line to 2nd line
     rect = iLayout->GetLineRectL(LtoRLength-1, LtoRLength);
     RDebug::Print(_L("iTl.iX %d,iY %d, iBr.iX %d,iY %d"), rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY);
     test(rect.iTl.iX == (LtoRLength-1)*wLTR && rect.iBr.iX == LtoRLength*wLTR); //Line 2


	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-4007
@SYMTestCaseDesc	    Test for PDEF118443
@SYMTestPriority 	    Normal
@SYMTestActions			Use test cases in form of "text + ZWJ + non-text characters", format the line,
 						then use CTmTextLayout::FindAdjacentChunks() to find text Chunks surrounding
 						the ZWJ. then verify if the text is broken at the correct place.
@SYMTestExpectedResults CTmTextLayout::FindAdjacentChunks() should return:
						- Left chunk = "text + ZWJ",
						- Right chunk = "non-text characters".
@SYMDEF                 PDEF118443
*/
void CTextViewTest::TestForPDEF118443L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-4007 Testing fix for PDEF118443 "));
	InitializeL();

	TCharFormat charFormat = TCharFormat(_L("ClearlyU"),10);
	TCharFormatMask charFormatMask;
	charFormatMask.SetAll();
	iCharLayer->SetL(charFormat, charFormatMask);
	iEtext->SetGlobalCharFormat(iCharLayer);

	// Note: a 'W' is added at the front to make the text chunk have side-bearings, so that it won't be amalgamated
	// with the following chunk. This is to make the test case be as same as the original use case that caused the defect.
	_LIT(KScriptEndWithZWJ,"W\x0931\x094d\x200d");	// Scripte end with ZWJ (a 'W' in the front)
	_LIT(KTextTestCase0,"\x2029");	// Paragraph seperator (Bidi Category: B)
	_LIT(KTextTestCase1,"0");		// Digit 0 (Bidi Category: EN)
	_LIT(KTextTestCase2,"+");		// Plus sign (Bidi Category: ES)
	_LIT(KTextTestCase3,"\u00A3");		// Pound symbol (Bidi Category: ET)
	_LIT(KTextTestCase4,".");		// Period (Bidi Category: CS)
	_LIT(KTextTestCase5,"\x0009");	// Tab (Bidi Category: S)
	_LIT(KTextTestCase6,"\x0020");	// Space (Bidi Category: WS)
	_LIT(KTextTestCase7,"\x000C");	// Form feed (Bidi Category: WS)
	_LIT(KTextTestCase8,"\x2028");	// Line breaker (Bidi Category: WS)

	TTmDocPosSpec pos(4, TTmDocPosSpec::ETrailing);
	CTmTextLayout::TTmChunkDescription left;
	CTmTextLayout::TTmChunkDescription right;

	// Test case 0: ZWJ + Paragraph seperater
	iEtext->Reset();
	iEtext->InsertL(0,KScriptEndWithZWJ);
	iEtext->InsertL(iEtext->DocumentLength(),KTextTestCase0);
	iView->FormatTextL();
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 4);
	test(left.iRightToLeft == EFalse);
	test(right.iStart == 4);
	test(right.iRightToLeft == EFalse);

	// Test case 1: ZWJ + Digit '0'
	iEtext->Reset();
	iEtext->InsertL(0,KScriptEndWithZWJ);
	iEtext->InsertL(iEtext->DocumentLength(),KTextTestCase1);
	iView->FormatTextL();
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 4);
	test(left.iRightToLeft == EFalse);
	test(right.iStart == 4);
	test(right.iRightToLeft == EFalse);

	// Test case 2: ZWJ + Plus sign '+'
	iEtext->Reset();
	iEtext->InsertL(0,KScriptEndWithZWJ);
	iEtext->InsertL(iEtext->DocumentLength(),KTextTestCase2);
	iView->FormatTextL();
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 4);
	test(left.iRightToLeft == EFalse);
	test(right.iStart == 4);
	test(right.iRightToLeft == EFalse);

	// Test case 3: ZWJ + Pound symbol '??'
	iEtext->Reset();
	iEtext->InsertL(0,KScriptEndWithZWJ);
	iEtext->InsertL(iEtext->DocumentLength(),KTextTestCase3);
	iView->FormatTextL();
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 4);
	test(left.iRightToLeft == EFalse);
	test(right.iStart == 4);
	test(right.iRightToLeft == EFalse);

	// Test case 0: ZWJ + Period '.'
	iEtext->Reset();
	iEtext->InsertL(0,KScriptEndWithZWJ);
	iEtext->InsertL(iEtext->DocumentLength(),KTextTestCase4);
	iView->FormatTextL();
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 4);
	test(left.iRightToLeft == EFalse);
	test(right.iStart == 4);
	test(right.iRightToLeft == EFalse);

	// Test case 0: ZWJ + Tab Character
	iEtext->Reset();
	iEtext->InsertL(0,KScriptEndWithZWJ);
	iEtext->InsertL(iEtext->DocumentLength(),KTextTestCase5);
	iView->FormatTextL();
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 4);
	test(left.iRightToLeft == EFalse);
	test(right.iStart == 4);
	test(right.iRightToLeft == EFalse);

	// Test case 0: ZWJ + Space
	iEtext->Reset();
	iEtext->InsertL(0,KScriptEndWithZWJ);
	iEtext->InsertL(iEtext->DocumentLength(),KTextTestCase6);
	iView->FormatTextL();
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 4);
	test(left.iRightToLeft == EFalse);
	test(right.iStart == 4);
	test(right.iRightToLeft == EFalse);

	// Test case 0: ZWJ + Form feed
	iEtext->Reset();
	iEtext->InsertL(0,KScriptEndWithZWJ);
	iEtext->InsertL(iEtext->DocumentLength(),KTextTestCase7);
	iView->FormatTextL();
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 4);
	test(left.iRightToLeft == EFalse);
	test(right.iStart == 4);
	test(right.iRightToLeft == EFalse);

	// Test case 0: ZWJ + Line breaker
	iEtext->Reset();
	iEtext->InsertL(0,KScriptEndWithZWJ);
	iEtext->InsertL(iEtext->DocumentLength(),KTextTestCase8);
	iView->FormatTextL();
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 4);
	test(left.iRightToLeft == EFalse);
	test(right.iStart == 4);
	test(right.iRightToLeft == EFalse);

	Destroy();
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-4016
@SYMTestCaseDesc        Testing the fix for PDEF121798 Printing: Email is printed only two pages 
@SYMTestPriority        Medium
@SYMTestActions         Paginates random amounts of text and checks whether the correct number of pages are returned from pagination.
@SYMTestExpectedResults The amount of pages produced by the paginator should match the expected number of pages based on lines of text, page size, etc.
@SYMDEF                 PDEF121798
*/
void CTextViewTest::TestForPDEF121798L()
	{
	test.Next(_L("Testing fix for PDEF121798"));
	CTestScreenDevice* screenDevice = new(ELeave) CTestScreenDevice(iEnv.ScreenDevice(),iWindow);
		
	TMargins margins;
	margins.iTop = 1440;
	margins.iBottom = 1440;
	margins.iLeft = 1440;
	margins.iRight = 1440;
	TSize s(11906,16838);
	TPageSpec p(TPageSpec::EPortrait, s);
		
	// We need to find out the height of lines and print area of the page.
	// From this we determine how many lines *should* appear on the page.
	// This differs between devices(ie. hw and winscw).
	TInt pageHeight = screenDevice->VerticalTwipsToPixels(s.iHeight - margins.iTop - margins.iBottom);
	_LIT(KDummyString,"this is used by dummy paginator to find out line height and page size");
	InitializeL();
	AddTextL(KDummyString);
	TInt lineHeight = 0;	
	CParaFormat* paraFormat = CParaFormat::NewL();
	iEtext->GetParagraphFormatL(paraFormat,0);
	TBool pageBreakChar = EFalse;
	TInt docPos = 0;
	iLayout->FormatLineL(paraFormat,docPos,lineHeight,pageBreakChar);
	
	CArrayFixFlat<TInt>* charsPerPage = new(ELeave) CArrayFixFlat<TInt>(KGranularity);
	TInt numLines = pageHeight/lineHeight; // Number of lines expected on a page with paginator settings defined above and line height.
	
	
	
	// Perform 50 random pagination tests.
	for(TInt numTests = 0; numTests < 50; numTests++)
		{
		// Generate the number of lines in this document.
		TBuf<512> testString;
		TInt randomNum = (Math::Random() % 512);
		// Calculate the expected number of pages based on page size and line height
		TInt expectedPages = randomNum/numLines;
		// If it's not an exact fit there will be another page lost in the integer division.
		if ((numLines * expectedPages != randomNum) || randomNum == 0)
			{
			++expectedPages;
			}
		// Append the random number of lines to the test string
		for (TInt i = 0; i < randomNum-1; i++) // randomNum -1 because we add a character after the loop.
			{
			// Empty lines will do.
			testString.Append(CEditableText::EParagraphDelimiter);
			}
		// End the text with a character rather than a paragraph delim.
		testString.Append(_L("A"));
		
		
		InitializeL();
		AddTextL(testString);
		// Set up the paginator.
		CTextPaginator* paginator = CTextPaginator::NewL(screenDevice, charsPerPage, KPaginatePriority);
		paginator->SetPageMarginsInTwips(margins);
		paginator->SetPageSpecInTwips(p);
		paginator->SetDocumentL(iEtext);
		docPos=0;
		paginator->AppendTextL(docPos);
		TInt numPages=paginator->PaginationCompletedL();
		RDebug::Printf("%d lines: Expected %d pages, got %d pages", randomNum, expectedPages, numPages);
		test(expectedPages == numPages);
		delete paginator;
		}
	delete charsPerPage;
	delete screenDevice;
	delete paraFormat;
	Destroy();
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-4015
@SYMTestCaseDesc	    Test for PDEF120239
@SYMTestPriority 	    Normal
@SYMTestActions			Use text consist of "LTR text + ZWJ + RTL Text", format the line, then:
						1) use CTmTextLayout::FindAdjacentChunks() to find chunks around overlapped doc pos.
						2) use CTextView::MoveCursorL to move cursor through out the line.
@SYMTestExpectedResults 1) FindAdjacentChunks() returns correct chunks
						2) Cursor should moves Left to Right and Right to Left correctly
@SYMDEF                 PDEF120239
*/
void CTextViewTest::TestForPDEF120239L()
	{
	test.Next(_L("Testing fix for PDEF120239L"));
	InitializeL();
	
	TCharFormat charFormat = TCharFormat(_L("NewTimes"),10);
	TCharFormatMask charFormatMask;
	charFormatMask.SetAll();
	iCharLayer->SetL(charFormat, charFormatMask);
	iEtext->SetGlobalCharFormat(iCharLayer);
 	
	_LIT(KTestScript,"\x0931\x094d\x200d\x684");	// Test script (LTR text + ZWJ + RTL Text)
	
	iEtext->Reset();
	iEtext->InsertL(0,KTestScript);
	iView->FormatTextL();
	
	// 1) use CTmTextLayout::FindAdjacentChunks()
	CTmTextLayout::TTmChunkDescription left;
	CTmTextLayout::TTmChunkDescription right;
	
	TTmDocPosSpec pos(3, TTmDocPosSpec::ETrailing);

	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 0);
	test(left.iEnd == 3);
	test(!left.iRightToLeft);
	test(right.iStart == 2);
	test(right.iEnd == 4);
	test(right.iRightToLeft);
	
	pos.iPos = 2;
	pos.iType = TTmDocPosSpec::ELeading;
	
	iLayout->TagmaTextLayout().FindAdjacentChunks(pos, left, right);
	test(left.iStart == 2);
	test(left.iEnd == 4);
	test(left.iRightToLeft);
	test(right.iStart == 4);
	test(right.iEnd == 5);
	test(!right.iRightToLeft);
	
	// 2) use CTextView::MoveCursorL to move cursor
	TTmDocPos cursorPos;
	TTmDocPos targetPos1 (0, ETrue);
	TTmDocPos targetPos2 (0, EFalse);
	TCursorPosition::TMovementType move = TCursorPosition::EFRight;
	
	TCursorSelection selection(0,0);
	iView->SetSelectionL(selection);
	iView->GetCursorPos(cursorPos);
	test(cursorPos == targetPos1 || cursorPos == targetPos2);
	
	targetPos1.iPos = 3;
	targetPos1.iLeadingEdge = EFalse;
	targetPos2.iPos = 4;
	targetPos2.iLeadingEdge = EFalse;
	iView->MoveCursorL(move, EFalse);
	iView->GetCursorPos(cursorPos);
	test(cursorPos == targetPos1 || cursorPos == targetPos2);
	
	targetPos1.iPos = 2;
	targetPos1.iLeadingEdge = ETrue;
	targetPos2.iPos = 4;
	targetPos2.iLeadingEdge = ETrue;
	iView->MoveCursorL(move, EFalse);
	iView->GetCursorPos(cursorPos);
	test(cursorPos == targetPos1 || cursorPos == targetPos2);
	
	move = TCursorPosition::EFLeft;
	
	targetPos1.iPos = 3;
	targetPos1.iLeadingEdge = EFalse;
	targetPos2.iPos = 4;
	targetPos2.iLeadingEdge = EFalse;
	iView->MoveCursorL(move, EFalse);
	iView->GetCursorPos(cursorPos);
	test(cursorPos == targetPos1 || cursorPos == targetPos2);
	
	targetPos1.iPos = 0;
	targetPos1.iLeadingEdge = EFalse;
	targetPos2.iPos = 0;
	targetPos2.iLeadingEdge = ETrue;
	iView->MoveCursorL(move, EFalse);
	iView->GetCursorPos(cursorPos);
	test(cursorPos == targetPos1 || cursorPos == targetPos2);

	Destroy();
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-4021
@SYMTestCaseDesc	    Test for DEF124989, to ensure TFormAndEtextEditor::SetStyleHelperL has no 
                        NULL-dereference issue.
@SYMTestPriority 	    Normal
@SYMTestActions  	    Call TFormAndEtextEditor::InsertTextL and pass in a style name which could 
						not be found in styleList.Then a NULL is generated.
@SYMTestExpectedResults No panic(KERN-EXEC 3) raised from this case.
@SYMDEF                 DEF124989
*/
void CTextViewTest::TestForDEF124989L()
	{
	// Initialise CTextViewTest object for next test.
	InitializeL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-4021 Testing fix for DEF124989 "));

	// Insert a one line paragraph into EText object and reformat view.
	_LIT(KTestParagraph, "This is a piece of text to test the character positioning API based in X,Y cocordinates.");
	iEtext->InsertL(0, KTestParagraph);
	
	TFormAndEtextEditor newEditor(*iView,*iEtext);
	
	CStyleList* styleList = CStyleList::NewL();
	CleanupStack::PushL(styleList);
	RParagraphStyleInfo paraStyleInfo(NULL,NULL);

	// Appending the new style to stylelist
	styleList->AppendL(&paraStyleInfo);
	iEtext->SetStyleListExternallyOwned(*styleList);
	
	TPtrC ptr1(_L("Arial3"));
	// Inserting the text and applying the style specified(Arial3) 
	// which is not in syleList.  
	newEditor.InsertTextL(0, _L("Hello World"),&ptr1);
	
	// Clean up test object
	CleanupStack::PopAndDestroy();
	Destroy();
	}

/**
@SYMTestCaseID                      SYSLIB-FORM-UT-4022
@SYMTestCaseDesc	    Test for DEF124975
@SYMTestPriority                       Normal
@SYMTestActions                      1) Call CTextLayout::AdjustVerticalAlignment() when there is no content
                                                   2) Add Some random text and then call CTextLayout::AdjustVerticalAlignment()@SYMTestExpectedResults       There should be no panic during the process
@SYMDEF                                 DEF124975
*/
void CTextViewTest::TestForDEF124975L()
	{
	test.Next(_L(" @SYMTestCaseID: Testing fix for coverity DEF124975 "));
	InitializeL();
	
	const TInt MaxTestCount = 50;
	const TInt MaxStringCount = 100;
	TInt TestCount = GetNumber(1, MaxTestCount);
	TInt StringCount = GetNumber(1, MaxStringCount);
	
	for(TInt i=0; i<TestCount; i++)
		{
		iLayout->AdjustVerticalAlignment(CParaFormat::ECustomAlign);
		}
	
	for(TInt i=0; i<TestCount; i++)
		{
		RBuf testString;
		testString.Create(StringCount);
		for(int i=0; i<testString.MaxLength(); i++)
			{
			TInt c = GetNumber(0, 0xffff);
			while ( IsSurrogate(c) )
			    c = GetNumber(0, 0xffff);
			testString.Append( c );
			}
		AddTextL(testString);
		iLayout->AdjustVerticalAlignment(CParaFormat::ECustomAlign);
		testString.Close();
		}
	
	Destroy();
	}

void CTextViewTest::TestForDEF142286BounceScrollingL()
    {
    test.Next(_L(" Testing fix for DEF142286 which supports bounce scrolling feature "));
    TestScrollDisplayPixelsNoLimitBordersL(10);
    TestScrollDisplayPixelsNoLimitBordersL(50);
    TestScrollDisplayPixelsNoLimitBordersL(100);
    }

void RunTestsL(CCoeEnv& aEnv)
	{
	CTextViewTest* t = new(ELeave) CTextViewTest(aEnv);
	CleanupStack::PushL(t);
	t->ConstructL();
	t->TestL();
	t->Test1L();
	t->TestCancelSelectionL();
	t->TestFinishBackgroundFormattingL();
	t->TestSetCursorVisibilityL();
	t->TestSetSelectionVisibilityL();
	t->TestEnablePictureFrameL();
	t->TestSetCursorWidthTypeL();
	t->TestParagraphRectL();
	t->TestDrawL();
	t->TestFormatTextL();
	t->TestHandleRangeFormatChangeL();
	t->TestHandleInsertDeleteL();
	t->TestHandleGlobalChangeL();
	t->TestHandleGlobalChangeNoRedrawL();
	t->TestScrollDisplayL();
	t->TestScrollDisplayPixelsL();
	t->TestScrollDisplayLinesL();
	t->TestScrollDisplayParagraphsL();
	t->TestMoveCursorL();
	t->TestSetSelectionL();
	t->TestMatchCursorHeightL();
	t->TestCalculateHorizontalExtremesL();
	t->TestXyPosToDocPosL();
	t->TestGetPictureRectangleL();
	t->TestGetPictureRectangle1L();
	//t->TestGetPictureRectangleDefectL();
	t->TestSetDisplayContextL();
	t->TestGetParaFormatL();
	t->TestGetLineRectL();
	t->TestForDEF003426L();
	t->TestForDEF038488L();
	t->FormAndEtextTestL();
	t->TestForINC092725L();
	t->TestForPDEF108443L();
	t->TestForPDEF113755L();
	t->TestForPDEF115165L();
	t->TestForPDEF118443L();
	t->TestForPDEF121798L();
	t->TestForDEF124989L();
	t->TestForPDEF120239L();
	t->TestForDEF124975L();
    t->TestForDEF142286BounceScrollingL();
	CleanupStack::PopAndDestroy(t);
}

TInt E32Main()
	{
	CCoeEnv* env=new CCoeEnv;
	TRAPD(err,
		env->ConstructL();
		RunTestsL(*env);
		);
	return err;
	}

#if defined(__WINS__)
EXPORT_C TInt EntryPoint(TAny*) {return E32Main();}
#endif
