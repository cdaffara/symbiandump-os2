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
*
*/

#include <e32std.h>
#include <e32test.h>
#include <frmtlay.h>
#include <frmtview.h>
#include <frmconst.h>
#include <txtlaydc.h>
#include <txtetext.h>
#include <w32std.h>
#include "TGraphicsContext.h"
#include "tcustomcharmapping.h"

namespace LocalToTCustomCharMapping
{
class CPinkSquare : public CPicture
	{
public:
	// Size of square in twips.
	// 600 is 15 pixels using the standard test graphics device at
	// its default resolution.
	enum { KWidth = 600, KHeight = 600 };
	CPinkSquare()
		{}
	void Draw(CGraphicsContext& aGc, const TPoint& aTopLeft, const TRect& aClipRect, MGraphicsDeviceMap* aMap) const
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
	void ExternalizeL(RWriteStream&) const
		{}
	void GetOriginalSizeInTwips(TSize& a) const
		{
		a.iWidth = CPinkSquare::KWidth;
		a.iHeight = CPinkSquare::KHeight;
		}
	};

_LIT(KEnd, "\x2029");
class TDocModel : public MLayDoc
	{
public:
	TDocModel(const TDesC& aDes)
	 :	iDes(&aDes), iParagraphFormat(0)
		{}
	void SetParagraphFormat(CParaFormat* a)
		{
		iParagraphFormat = a;
		}
	// From MLayDoc
	TInt LdDocumentLength() const
		{ return iDes->Length(); }
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
		if (aStartPos == LdDocumentLength())
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
	TBool SelectParagraphLabel(TInt)
		{ return EFalse; }
	void CancelSelectLabel()
		{}
private:
	const TDesC* iDes;
	CParaFormat* iParagraphFormat;
	};
}
using namespace LocalToTCustomCharMapping;

class CTestTextView	// slightly naughty
	{
public:
	static void SetContextForFlickerFreeRedraw(CTextView* aView, CBitmapContext* aContext)
		{
		aView->iOffScreenContext = aContext;
		}
	};


static const TInt KTestCases = 5;
static const TInt KVariants = 2;

// For tests 0 to 3 the source string consists of:
// 'Y' <hard space> 'Z' <normal space> <non-breaking hyphen> <zero width space>
static const TPtrC KTestStrings[KTestCases][KVariants] =
	{
		{	// Test remapping with no custom remapper and flags set to invisible
			// The non-breaking hyphen is turned into a normal hyphen
			// The zero width space disappears (0xFFFF) and the hard space becomes a normal space
			// All done by MTmCustom::Map()
		_S("\x0059\x00A0\x005A\x0020\x2011\x200B"),
		_S("\xFFFF\x0059\x0020\x005A\x0020\x002D\xFFFF\xFFFF"),
		},
		{	// Test remapping with no custom remapper and flags set to visible
			// The non-breaking hyphen becomes a tilde
			// The normal space and the zero width space become visible middle dots
			// The hard (non-breaking) space becomes a degree sign
			// The paragraph sign becomes a pilcrow
			// All done by MTmCustom::Map()
		_S("\x0059\x00A0\x005A\x0020\x2011\x200B"),
		_S("\xFFFF\x0059\x00B0\x005A\x00B7\x007E\x00B7\x00B6\xFFFF"),
		},
		{	// Test remapping with custom remapper and flags set to invisible
			// The non-breaking hyphen is turned into a normal hyphen
			// The zero width space disappears (0xFFFF) and the hard space becomes a normal space
			// All done by MTmCustom::Map()
			// The hard (non-breaking) space becomes a caret
			// This is done by the custom remapper
		_S("\x0059\x00A0\x005A\x0020\x2011\x200B"),
		_S("\xFFFF\x0059\x005E\x005A\x0020\x002D\xFFFF\xFFFF"),
		},
		{	// Test remapping with custom remapper and flags set to visible
			// The non-breaking hyphen becomes a tilde
			// The zero width space become visible middle dots
			// All done by MTmCustom::Map()
			// The normal space remains a normal (invisible) space because default behaviour is overidden
			// The hard (non-breaking) space becomes a caret
			// The paragraph sign becomes a capital P
			// All done by the custom remapper
		_S("\x0059\x00A0\x005A\x0020\x2011\x200B"),
		_S("\xFFFF\x0059\x005E\x005A\x0020\x007E\x00B7\x0050\xFFFF"),
		},
		{
			// Test remapping with no custom remapper and some flags explicitly set to invisible
			// The non-breaking hyphen is turned into a normal hyphen
			// The zero width space disappears (0xFFFF) and the hard space becomes a normal space
			// All done by MTmCustom::Map()
		_S("\x0059\x00A0\x005A\x0020\x2011\x200B"),
		_S("\xFFFF\x0059\x0020\x005A\x0020\x002D\xFFFF\xFFFF"),
		}
	};


class CCustomRemapper : public MFormCustomInvisibleCharacterRemapper
	{
public:
	static CCustomRemapper* NewL();
	~CCustomRemapper();
	TUint Remap(TUint aChar, const TNonPrintingCharVisibility aNonPrintingCharVisibility, const TLayDocTextSource& aLayDoc);
private:
	CCustomRemapper();
	};

	CCustomRemapper* CCustomRemapper::NewL()
		{
		CCustomRemapper* me = new(ELeave) CCustomRemapper;
		return me;
		}

	CCustomRemapper::~CCustomRemapper()
		{
		}

	TUint CCustomRemapper::Remap(TUint aChar, const TNonPrintingCharVisibility aNonPrintingCharVisibility, const TLayDocTextSource& aLayDoc)
		{
		switch (aChar)
			{
			case CEditableText::EParagraphDelimiter:
				if (aNonPrintingCharVisibility.ParagraphDelimitersVisible())
					return 0x0050; // capital P - override when visible
				break;

			case CEditableText::ESpace:
				return aChar; // don't remap but don't pass it on - override default
				// break statement was removed.

			case CEditableText::ENonBreakingSpace:
				return 0x005E; // Caret - override always - visible or not
				// break statement was removed.
			default:
				break; // do nothing
			}

		// If not mapping special characters, or not mapping this particular character, use the default mapping.
		return DefaultMapping(aChar, aNonPrintingCharVisibility, aLayDoc);
		}

	CCustomRemapper::CCustomRemapper()
		{
		}

void CTCustomCharMappingStep::DoTestL(TDes& aText, CTextLayout* /*aLayout*/, CTestGraphicsDevice* aDevice, CTextView* aView, TTestNum aTestNum)
	{
	aText = KTestStrings[aTestNum][0];
	aDevice->LineArray().ResetLineArray();
	aView->HandleGlobalChangeL();
	const TTestGCDisplayLine* line1 = &(aDevice->LineArray().Line(0));
	const TTestGCDisplayLine* line2 = aDevice->LineArray().Find(KTestStrings[aTestNum][1]);
	TEST(0 != line1);
	TEST(0 != line2);
	// Can't always do a direct comparison of lines because same string
	// may appear in more than one line, so compare contents
	TEST(line1->iLineData.Compare(line2->iLineData) == 0);
	}


/**
@SYMTestCaseID SYSLIB-FORM-CT-0147
@SYMTestCaseDesc Test installation and deinstallation of custom remapper
@SYMTestPriority High
@SYMTestActions  Test installation and deinstallation of custom remapper
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 1128 Placeholders for invisible characers in rich text shall be customizable
*/
void CTCustomCharMappingStep::RunInstallationTestsL()
	{
	// Note: If you need to move these heap checks any further "in" to focus
	// on a specific test then you will have to move all the setup code in as
	// well - and still preserve the two different display widths in use
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-0147 "));
	__UHEAP_MARK;
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	TBuf<100> text;
	TDocModel docModel(text);
	TRect displayRect(0, 0, KDisplayWidth, KDisplayHeight);
	CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTextView* view = CTextView::NewL(layout, displayRect, device, device, 0, 0, 0);
	CleanupStack::PushL(view);
	// This is used to force the use of CTestGraphicsContext instead of a normal one
	CWindowGc* offScreenContext;
	User::LeaveIfError(device->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetContextForFlickerFreeRedraw(view, offScreenContext);

	// OK, let's get down to testing
	MFormCustomInvisibleCharacterRemapper* remapper;
	// read what the ptr is set to - check it is null
	remapper = layout->GetCustomInvisibleCharacterRemapper();
	INFO_PRINTF1(_L("Test uninstalled"));
	TEST(remapper == NULL);

	// install a custom remapper - get the ptr - check it is set
	MFormCustomInvisibleCharacterRemapper* customRemapper = CCustomRemapper::NewL();
	layout->SetCustomInvisibleCharacterRemapper(customRemapper);
	remapper = layout->GetCustomInvisibleCharacterRemapper();
	INFO_PRINTF1(_L("Test installed"));
	TEST(remapper == customRemapper);

	// set the ptr back to null (deinstall) - get the ptr - check it is null
	layout->SetCustomInvisibleCharacterRemapper(NULL);
	remapper = layout->GetCustomInvisibleCharacterRemapper();
	INFO_PRINTF1(_L("Test uninstalled again"));
	TEST(remapper == NULL);

	delete customRemapper;

	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(layout);
	CleanupStack::PopAndDestroy(scheduler);
	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID SYSLIB-FORM-CT-0148
@SYMTestCaseDesc Test behaviour without custom remapper installed
@SYMTestPriority High
@SYMTestActions  Test behaviour without custom remapper installed
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 1128 Placeholders for invisible characers in rich text shall be customizable
*/
void CTCustomCharMappingStep::RunDefaultBehaviourTestsL()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-0148 "));
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	TBuf<100> text;
	TDocModel docModel(text);
	TRect displayRect(0, 0, KDisplayWidth, KDisplayHeight);
	CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTextView* view = CTextView::NewL(layout, displayRect, device, device, 0, 0, 0);
	CleanupStack::PushL(view);
	// This is used to force the use of CTestGraphicsContext instead of a normal one
	CWindowGc* offScreenContext;
	User::LeaveIfError(device->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetContextForFlickerFreeRedraw(view, offScreenContext);

	// Start by making sure no custom remapper is installed
	MFormCustomInvisibleCharacterRemapper* remapper;
	remapper = layout->GetCustomInvisibleCharacterRemapper();
	TEST(remapper == NULL);

	// Set all invisible characters to be invisible
	TNonPrintingCharVisibility visibility;
	visibility.SetNoneVisible();
	layout->SetNonPrintingCharsVisibility(visibility);

	// Test remapping with no custom remapper and flags set to invisible
	INFO_PRINTF1(_L("Test uninstalled behaviour - flags invisible"));
	DoTestL(text, layout, device, view, EDefaultBehaviourInvisible);

	// Now set all invisible characters to be visible
	visibility.SetAllVisible();
	layout->SetNonPrintingCharsVisibility(visibility);

	// Test remapping with no custom remapper and flags set to visible
	INFO_PRINTF1(_L("Test uninstalled behaviour - flags visible"));
	DoTestL(text, layout, device, view, EDefaultBehaviourVisible);

	// Test remapping with no custom remapper and some flags explicitly set to invisible
	INFO_PRINTF1(_L("Test uninstalled behaviour - some flags invisible"));
	//Set all invisible characters to be visible
	visibility.SetAllVisible();
	//Set some attributes explicitly to be invisible
	visibility.SetSpacesVisible(EFalse);
	visibility.SetTabsVisible(EFalse);
	visibility.SetPotentialHyphensVisible(EFalse);
	visibility.SetParagraphDelimitersVisible(EFalse);
	visibility.SetPageBreaksVisible(EFalse);
	visibility.SetNonBreakingSpacesVisible(EFalse);
	visibility.SetNonBreakingHyphensVisible(EFalse);
	visibility.SetLineBreaksVisible(EFalse);
	layout->SetNonPrintingCharsVisibility(visibility);
	//Test if the visibility is set accordingly
	DoTestL(text, layout, device, view, ENewTest);

	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(layout);
	CleanupStack::PopAndDestroy(scheduler);
	}


/**
@SYMTestCaseID SYSLIB-FORM-CT-0149
@SYMTestCaseDesc Test behaviour with custom remapper installed
@SYMTestPriority High
@SYMTestActions  Test behaviour with custom remapper installed
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 1128 Placeholders for invisible characers in rich text shall be customizable
*/
void CTCustomCharMappingStep::RunCustomBehaviourTestsL()
	{
	// Note: If you need to move these heap checks any further "in" to focus
	// on a specific test then you will have to move all the setup code in as
	// well - and still preserve the two different display widths in use
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-0149 "));
	__UHEAP_MARK;
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	TBuf<100> text;
	TDocModel docModel(text);
	TRect displayRect(0, 0, KDisplayWidth, KDisplayHeight);
	CTextLayout* layout = CTextLayout::NewL(&docModel, displayRect.Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect.Size(), 0);
	CleanupStack::PushL(device);
	CTextView* view = CTextView::NewL(layout, displayRect, device, device, 0, 0, 0);
	CleanupStack::PushL(view);
	// This is used to force the use of CTestGraphicsContext instead of a normal one
	CWindowGc* offScreenContext;
	User::LeaveIfError(device->CreateContext(offScreenContext));
	CleanupStack::PushL(offScreenContext);
	CTestTextView::SetContextForFlickerFreeRedraw(view, offScreenContext);

	// We need to install a custom remapper
	MFormCustomInvisibleCharacterRemapper* remapper;
	MFormCustomInvisibleCharacterRemapper* customRemapper = CCustomRemapper::NewL();
	layout->SetCustomInvisibleCharacterRemapper(customRemapper);
	remapper = layout->GetCustomInvisibleCharacterRemapper();
	TEST(remapper == customRemapper);

	// Set all invisible characters to be invisible
	TNonPrintingCharVisibility visibility;
	visibility.SetNoneVisible();
	layout->SetNonPrintingCharsVisibility(visibility);

	// Test remapping with custom remapper and flags set to invisible
	INFO_PRINTF1(_L("Test installed behaviour - flags invisible"));
	DoTestL(text, layout, device, view, ECustomRemappingInvisible);

	// Now set all invisible characters to be visible
	visibility.SetAllVisible();
	layout->SetNonPrintingCharsVisibility(visibility);

	// Test remapping with custom remapper and flags set to visible
	INFO_PRINTF1(_L("Test installed behaviour - flags visible"));
	DoTestL(text, layout, device, view, ECustomRemappingVisible);

	// Now we are finished deinstall and delete it
	layout->SetCustomInvisibleCharacterRemapper(NULL);
	remapper = layout->GetCustomInvisibleCharacterRemapper();
	TEST(remapper == NULL);
	delete customRemapper;

	CleanupStack::PopAndDestroy(offScreenContext);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(device);
	CleanupStack::PopAndDestroy(layout);
	CleanupStack::PopAndDestroy(scheduler);
	__UHEAP_MARKEND;
	}

CTCustomCharMappingStep::CTCustomCharMappingStep()
    {
    
    }

TVerdict CTCustomCharMappingStep::doTestStepL()
    {
    SetTestStepResult(EPass);
    INFO_PRINTF1(_L("Test installation/deinstallatiion"));
    TInt error = RFbsSession::Connect();
    if (error == KErrNotFound)
        {
        FbsStartup();
        error = RFbsSession::Connect();
        }
    TEST(error == KErrNone);
    TRAP(error, RunInstallationTestsL());
    TEST(error == KErrNone);
    INFO_PRINTF1(_L("Test uninstalled behaviour"));
    TRAP(error, RunDefaultBehaviourTestsL());
    TEST(error == KErrNone);
    INFO_PRINTF1(_L("Test behaviour with custom remapper installed"));
    TRAP(error, RunCustomBehaviourTestsL());
    TEST(error == KErrNone);
    RFbsSession::Disconnect();
    return TestStepResult();
    }
