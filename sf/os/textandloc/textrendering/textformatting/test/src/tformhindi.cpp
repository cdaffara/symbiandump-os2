/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32test.h>
#include <frmtlay.h>
#include <frmtview.h>
#include <txtlaydc.h>
#include <fbs.h>
#include <w32std.h>
#include <bitdev.h>
#include <txtrich.h>
#include "tformhindi.h"

namespace LocalToTFormHindi {

_LIT(KTformhindi, "tformhindi");
const TInt KDisplayWidth = 100;
const TInt KDisplayHeight = 100;
_LIT(KDevanagariFontName, "Devanagari OT Eval");

CTFormHindiStep* TestStep = NULL;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)

}
using namespace LocalToTFormHindi;

_LIT(KTestDeva, "|\x915\x94d\x92b|\x907|\x920\x94d\x920|");
_LIT(KTestBidi, "|\x915|\x644|\x920|");
_LIT(KTestDeva2, "|\x917|\x91c|\x92f\x93f|\x924\x94d\x932\x940|\x917|");

static const TInt KZeroWidthJoiner = 0x200d;

/** Test that the deletions occur in expected places.
Expected places should be marked by '|' (pipe character).
These will be removed prior to the test.
@internalComponent
*/
void TestDeletePosition(const TDesC& aTestText, CRichText* aRichText,
	CTextView* aTextView)
	{
	TBuf<100> text;
	TInt positions[20];
	TInt positionCount = 0;

	TInt ttlen = aTestText.Length();
	for (TInt i = 0; i != ttlen; ++i)
		{
		TInt c = aTestText[i];
		if (c == '|')
			{
			positions[positionCount] = text.Length();
			++positionCount;
			}
		else
			text.Append(c);
		}

	aRichText->Reset();
	aRichText->InsertL(0, text);
	TCharFormat format(KDevanagariFontName, 200);
	TCharFormatMask mask;
	mask.SetAttrib(EAttFontTypeface);
	aRichText->ApplyCharFormatL(format, mask, 0, text.Length());
	aTextView->FormatTextL();

	for (TInt j = 0; j < positionCount - 1; ++j)
		{
		aTextView->SetDocPosL(positions[j], EFalse);
		TInt pos = aTextView->GetForwardDeletePositionL().HigherPos();
		TESTPOINT(pos == positions[j + 1]);
		aTextView->SetDocPosL(positions[j], ETrue);
		pos = aTextView->GetForwardDeletePositionL().HigherPos();
		TESTPOINT(pos == positions[j + 1]);
		aTextView->SetDocPosL(positions[j + 1], EFalse);
		pos = aTextView->GetBackwardDeletePositionL().LowerPos();
		TESTPOINT(pos == positions[j]);
		aTextView->SetDocPosL(positions[j + 1], ETrue);
		pos = aTextView->GetBackwardDeletePositionL().LowerPos();
		TESTPOINT(pos == positions[j]);
		}

	aRichText->Reset();
	}


// DEF101191: FORM always splits chunks at ZWJ character

class CPDEF_101617_CustomDraw : public MFormCustomDraw
{
public:
	CPDEF_101617_CustomDraw() :
		iLeftChunk(),
		iRightChunk(),
		iChunk(0)
		{
		}
	virtual ~CPDEF_101617_CustomDraw() {}

	void setExpectedChunk(const TDesC& aChunk)
	{
		iLeftChunk = aChunk;
		iRightChunk.Zero();
		iChunk = 0;
	}
	void setExpectedChunks(const TDesC& aLeftChunk, const TDesC& aRightChunk)
	{
		iLeftChunk = aLeftChunk;
		iRightChunk = aRightChunk;
		iChunk = 0;
	}

	// MFormCustomDraw
	virtual void DrawText(const TParam& /*aParam*/, const TLineInfo& /*aLineInfo*/, const TCharFormat& /*aFormat*/,
		const TDesC& aText, const TPoint& /*aTextOrigin*/, TInt /*aExtraPixels*/) const
		{
		TBool containsZWJ = EFalse;

		TBuf<100> text;
		TInt len = aText.Length();
		for (TInt i = 0; i < len; i++)
			{
			// remove 0xffff
			if (0xffff != aText[i])
				{
				text.Append(aText[i]);
				if (KZeroWidthJoiner == aText[i])
					{
					containsZWJ = ETrue;
					}
				}
			}

		// check that the text was splitted as expected
		if (containsZWJ || iChunk > 0)
			{
			// the first chunk with a LZW should be equal to iLeftChunk
			if (0 == iChunk)
				{
                TESTPOINT(text == iLeftChunk);
				iChunk++;
				}
			// the following chunk should be equal to iRightChunk, if it is not-null.
			else if (1 == iChunk && iRightChunk.Size() > 0)
				{
                TESTPOINT(text == iRightChunk);
				iChunk++;
				}
			// just ignore the following chunks
			}
		}

private:
	TBuf<100> iLeftChunk;
	TBuf<100> iRightChunk;
	mutable TInt iChunk; // modified in a const method
};

void PDEF_101617_DefectL(CRichText* aRichText, CTextView* aTextView)
	{
	TCharFormat format(KDevanagariFontName, 200);
	TCharFormatMask mask;
	mask.SetAttrib(EAttFontTypeface);

	CPDEF_101617_CustomDraw* customDrawer = new CPDEF_101617_CustomDraw;
	CleanupStack::PushL(customDrawer);

	CTextLayout* layout = const_cast<CTextLayout*>(aTextView->Layout());
	layout->SetCustomDraw(customDrawer);

	TRect rect(0, 0, 300, 100);

	// 1. test the sequence Sha, Virama, ZWJ, Va,Sha, EndOfParagraph
	_LIT(KTestDevaZWJ1, "\x0936\x094d\x200d\x0935\x0936\x2029");
	TPtrC text1(KTestDevaZWJ1().Ptr(), KTestDevaZWJ1().Length());

	// the text should not be split and should remain a single chunk...
	_LIT(KTestDevaZWJ1_LeftChunk, "\x0936\x094d\x200d\x0935\x0936");

	customDrawer->setExpectedChunk(KTestDevaZWJ1_LeftChunk);

	aRichText->Reset();
	aRichText->InsertL(0, text1);
	aRichText->ApplyCharFormatL(format, mask, 0, text1.Length());
	aTextView->FormatTextL();
	aTextView->DrawL(rect);


	// 2. test the sequence Sha,ZWJ,Virama, Va, EndOfParagraph
	_LIT(KTestDevaZWJ2, "\x0936\x200d\x094d\x0935\x2029");
	TPtrC text2(KTestDevaZWJ2().Ptr(), KTestDevaZWJ2().Length());

	// the text should be split at the bidirectionality change
	_LIT(KTestDevaZWJ2_LeftChunk, "\x0936\x200d\x094d\x0935");

	customDrawer->setExpectedChunk(KTestDevaZWJ2_LeftChunk);

	aRichText->Reset();
	aRichText->InsertL(0, text2);
	aRichText->ApplyCharFormatL(format, mask, 0, text2.Length());
	aTextView->FormatTextL();
	aTextView->DrawL(rect);

	// 3. test the sequence Sha, Virama, ZWJ,Alef(Hebrew) EndOfParagraph
	_LIT(KTestDevaZWJ3, "\x0936\x094d\x200d\x05D0\x2029");
	TPtrC text3(KTestDevaZWJ3().Ptr(), KTestDevaZWJ3().Length());

	// the text should be split keeping ZWJ (0x200d) at the end of the first chunk...
	_LIT(KTestDevaZWJ3_LeftChunk, "\x0936\x094d\x200d");
	// ... and placing another ZWJ at the start of the second chunk (right to left text)
	_LIT(KTestDevaZWJ3_RightChunk, "\x05D0\x200d");

	customDrawer->setExpectedChunks(KTestDevaZWJ3_LeftChunk, KTestDevaZWJ3_RightChunk);

	aRichText->Reset();
	aRichText->InsertL(0, text3);
	aRichText->ApplyCharFormatL(format, mask, 0, text3.Length());
	aTextView->FormatTextL();
	aTextView->DrawL(rect);

	// 4. test the sequence Sha, Virama, ZWJ, Va,Alef(Hebrew) EndOfParagraph
	_LIT(KTestDevaZWJ4, "\x0936\x094d\x200d\x0935\x05D0\x2029");
	TPtrC text4(KTestDevaZWJ4().Ptr(), KTestDevaZWJ4().Length());

	// the text should be split at the bidirectionality change
	_LIT(KTestDevaZWJ4_LeftChunk, "\x0936\x094d\x200d\x0935");
	_LIT(KTestDevaZWJ4_RightChunk, "\x05D0");

	customDrawer->setExpectedChunks(KTestDevaZWJ4_LeftChunk, KTestDevaZWJ4_RightChunk);

	aRichText->Reset();
	aRichText->InsertL(0, text4);
	aRichText->ApplyCharFormatL(format, mask, 0, text4.Length());
	aTextView->FormatTextL();
	aTextView->DrawL(rect);

	aRichText->Reset();

	layout->SetCustomDraw(NULL);
	CleanupStack::PopAndDestroy(customDrawer);
	}


/** @SYMTestCaseID SYSLIB-FORM-UT-1532
@SYMTestCaseDesc
	Test delete-by-syllable within Hindi text.
@SYMTestPriority High
@SYMTestActions
	Format some Devanagari, test result of calling GetForwardDeletePositionL
	and GetBackwardDeletePositionL for various inputs.
@SYMTestExpectedResults
	Success if the font "Devanagari OT Eval" is present, Not Run if it is not.
	This font is licensed to Symbian by Monotype Imaging Ltd. for internal
	testing only and so is not distributed with the SDK. Therefore users
	outside of Symbian will experience Not Run unless they have also
	obtained this font.
@SYMPREQ PREQ18 */
void TestTextViewL(CRichText* aRichText,
	CTextView* aTextView)
	{
	// Test devanagari delete-by-syllable
	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-1532 Test some simple Hindi "));
	TestDeletePosition(KTestDeva, aRichText, aTextView);

	// Test Bidi
	// The Arabic character is not present in this font, not even
	// as a fallback glyph. This allows us to exercise a fixes for
	// a latent defect.
	TESTPRINT(_L("Test with characters not in font"));
	TestDeletePosition(KTestBidi, aRichText, aTextView);

	// Test sample suggested by customer
	TESTPRINT(_L("Test Hindi #2"));
	TestDeletePosition(KTestDeva2, aRichText, aTextView);

	// regression test for PDEF101617: FORM always splits chunks at ZWJ character
	TESTPRINT(_L("Regression test: PDEF101617"));
	PDEF_101617_DefectL(aRichText, aTextView);

	}

void TestL(CFbsScreenDevice* aDevice)
	{
	CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormat);
	CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
	CleanupStack::PushL(charFormat);
	CRichText* text = CRichText::NewL(paraFormat, charFormat);
	CleanupStack::PushL(text);
	TRect displayRect(0, 0, KDisplayWidth, KDisplayHeight);
	CTextLayout* layout = CTextLayout::NewL(text, displayRect.Width());
	CleanupStack::PushL(layout);
	CTextView* view = CTextView::NewL(layout, displayRect,
		aDevice, aDevice, 0, 0, 0);
	CleanupStack::PushL(view);
	TestTextViewL(text, view);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(layout);
	CleanupStack::PopAndDestroy(text);
	CleanupStack::PopAndDestroy(charFormat);
	CleanupStack::PopAndDestroy(paraFormat);
	}

TVerdict CTFormHindiStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    TESTPRINT(KTformhindi);
    
	TInt error = RFbsSession::Connect();
	if (error == KErrNotFound)
		{
		FbsStartup();
		User::LeaveIfError(RFbsSession::Connect());
		}
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
	TFontSpec fs(KDevanagariFontName, 16);
	CFont* devaFont;
	User::LeaveIfError(screenDevice->GetNearestFontInPixels(devaFont, fs));
	TFontSpec fontSpec = devaFont->FontSpecInTwips();
	if(0 != fontSpec.iTypeface.iName.Compare(KDevanagariFontName))
		{
		// Test font not found.
		User::Leave(KErrNotFound);
		}
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// We know that we have everything we need now, so we'll start the test!
	// A failure before this point would show up in the logs as "not run" rather
	// than "failed".
	TESTPRINT(_L("Test forward/backward delete for Hindi"));
	TRAP(error, TestL(screenDevice));
	CleanupStack::PopAndDestroy(scheduler);
	CleanupStack::PopAndDestroy(gc);
	CleanupStack::PopAndDestroy(screenDevice);
	RFbsSession::Disconnect();
	User::LeaveIfError(error);
	return TestStepResult();
	}
