/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* TInterpreter.cpp unit tests for RTmBoundingRectInterpreter
*
*/


#include "TestPicture.h"
#include "TestLayout.h"
#include "TGraphicsContext.h"
#include "TMINTERP.H"
#include "tinterpreter.h"
#include <txtrich.h>
#include <e32test.h>


namespace LocalToTInterpreter
{
_LIT(KLeftToRight1, "abc \x5D0 def abc \x5D0\x5D1\x5D2 \x5D0\x5D1\x5D2 xyz abc a\tb\tc\td\te.");
_LIT(KLeftToRight2, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
_LIT(KContingentBreak, "\xFFFC");

CTInterpreterStep* TestStep = NULL;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)

}
using namespace LocalToTInterpreter;


/**
Checks if one region is a subset of another.
@param aRegion
	The potential superset.
@param aPotentialSubset
	The potential subset.
@return
	ETrue if and only if aPotentialSubset is contained within aRegion.
@internalComponent
*/
TBool IsSubsetL(const TRegion& aRegion, const TRegion& aPotentialSubset)
	{
	RRegion sub;
	sub.Copy(aPotentialSubset);
	sub.SubRegion(aRegion);
	if (sub.CheckError())
		User::Leave(KErrNoMemory);
	return sub.IsEmpty();
	}

/**
Checks if two regions are equal.
@param aA First region.
@param aB Second region.
@return
	ETrue if the regions are equal, false otherwise.
@internalComponent
*/
TBool RegionsEqualL(const TRegion& aA, const TRegion& aB)
	{
	return IsSubsetL(aA, aB) && IsSubsetL(aB, aA);
	}

/**
Gets the region corresponding to the selection. Builds it up by adding
selections together. The selections are at (n * aStartStep, aEnd + n *
aEndStep) for n <- {0, 1, 2...}. The iteration ends when either the start would
be beyond the length of the document or when the start is at 0 and the end is
beyond the length of the document.
@param aRegion Returns the region.
@param aLayout The layout to get the selection of.
@param aStartStep How different the starts of successive selections are.
@param aEnd The end of the first selection.
@param aEndStep How different the ends of the successive selections are.

@internalComponent
*/
void GetSelectionL(TRegion& aRegion, CTestTmTextLayout& aLayout,
	TInt aStartStep, TInt aEnd, TInt aEndStep)
	{
	aRegion.Clear();
	TRect rect;
	TInt documentLength = aLayout.Source().DocumentLength();
	TInt start = 0;
	while (start < documentLength && !(documentLength < aEnd && start == 0))
		{
		TTmInterpreterParam param(aLayout.Layout());
		RTmBoundingRectInterpreter interp(aLayout.Source(), param);
		if (interp.FirstRect(start, aEnd, rect))
			{
			aRegion.AddRect(rect);
			while (interp.NextRect(rect))
				aRegion.AddRect(rect);
			}
		start += aStartStep;
		aEnd += aEndStep;
		interp.Close();
		}
	if (aRegion.CheckError())
		User::Leave(KErrNoMemory);
	}

/**
Tests RTmBoundingRectInterpreter for a particular piece of text.
@internalComponent
*/
void TestTextL(CTestTmTextLayout& aLayout)
	{
	RRegion region1;
	RRegion region2;
	RRegion region3;
	CleanupClosePushL(region1);
	CleanupClosePushL(region2);
	CleanupClosePushL(region3);

	GetSelectionL(region1, aLayout, 1, 1, 1);
	GetSelectionL(region2, aLayout, 0, 1, 1);
	GetSelectionL(region3, aLayout, 0, aLayout.Source().DocumentLength(), 1);

	TESTPOINT(RegionsEqualL(region1, region2));
	TESTPOINT(RegionsEqualL(region1, region3));

	CleanupStack::PopAndDestroy(&region3);
	CleanupStack::PopAndDestroy(&region2);
	CleanupStack::PopAndDestroy(&region1);
	}


void TestsL()
	{
	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);
	CRichText* richText = CRichText::NewL(paraLayer, charLayer);
	CleanupStack::PushL(richText);

	TESTPRINT(_L("RTmBoundingRectInterpreter consistency of coverage"));
	richText->Reset();
	richText->InsertL(0, KLeftToRight1);
	CTestTmTextLayout* text1 = CTestTmTextLayout::NewLC(*richText, 100);
	TSize pictureSize(15, 15);
	CTestPicture* picture = new (ELeave) CTestPicture;
	picture->SetSizeInPixels( pictureSize, text1->Device() );
	TPictureHeader pictureHeader;
	pictureHeader.iPicture = picture;
	pictureHeader.iSize = pictureSize;
	richText->InsertL(19, pictureHeader);
	TTmReformatParam param;
	param.iStartChar = 19;
	param.iOldLength = 0;
	param.iNewLength = 1;
	param.iMaxExtraLines = KMaxTInt;
	param.iParFormatChanged = ETrue;
	param.iParInvalid = EFalse;
	TTmReformatResult out;
	text1->FormatL(param, out);
	TestTextL(*text1);

	//Test for finding text chunks adjoining a given document position
	text1->TestAdjacentChunks();

	CleanupStack::PopAndDestroy(text1);
	CleanupStack::PopAndDestroy(richText);
	CleanupStack::PopAndDestroy(charLayer);
	CleanupStack::PopAndDestroy(paraLayer);
	}


/**
@SYMTestCaseID          SYSLIB-FORM-UT-1591
@SYMTestCaseDesc	    Tests to make sure when contingent line breaks, "\xFFFC", are inserted into richtext
						(with and without a picture attached) and FormatL is called it does not lead to a
						panic in 'IsLegalBreakBeforeL' or 'IsLegalBreakAfterL'.
@SYMTestPriority 	    High
@SYMTestActions  	    Insert some richtext, then insert a TPictureHeader into the text (which inserts a
						contingent line break), then call FormatL. Then insert just a contingent line break,
						with no TPictureHeader associated with it and call FormatL (for IsLegalBreakAfterL).
						Then repeat the process, but making sure the text is scanned in the opposite
						direction, so IsLegalBreakBeforeL is called.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF077884
*/

void Def077884L()
	{
	TInt testStartLength = 52;
	TInt testEndLength = 56;

	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);
	CRichText* richText = CRichText::NewL(paraLayer, charLayer);
	CleanupStack::PushL(richText);

	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-1591 DEF077884: TSourcePictureBreaker crashes when picture not found. "));


	richText->Reset();

	richText->InsertL(0, KLeftToRight2);

	TESTPOINT(testStartLength == richText->DocumentLength());

	CTestTmTextLayout* text1 = CTestTmTextLayout::NewLC(*richText, 100);

	TTmReformatResult out;
	TTmReformatParam param;
	param.iStartChar = 0;
	param.iOldLength = 0;
	param.iNewLength = 1;
	param.iMaxExtraLines = KMaxTInt;
	param.iParFormatChanged = ETrue;
	param.iParInvalid = EFalse;

	richText->InsertL(14, KContingentBreak); // Insert a contingent linebreak with no picture

	text1->FormatL(param, out);	// Scans the text from right to left.


	TSize pictureSize(100, 100);
	CTestPicture* picture = new (ELeave) CTestPicture;
	picture->SetSizeInPixels(pictureSize, text1->Device());

	TPictureHeader pictureHeader;
	pictureHeader.iPicture = picture;
	pictureHeader.iSize = pictureSize;

	richText->InsertL(15, pictureHeader); // Insert a contingent linebreak with picture

	param.iOldLength = 1;
	param.iNewLength = 2;

	text1->FormatL(param, out);	// Scans the text from right to left.


	TTmFormatParam formatParam;
	formatParam.iFlags = 0;
	// iFlags now need to be set to make sure the text is scanned in the opposite direction when
	// FormatL is called.
	formatParam.iFlags = formatParam.iFlags | TTmFormatParamBase::ELegalLineBreaksOnly;
	formatParam.iFlags = formatParam.iFlags | TTmFormatParamBase::EWrap;
 	formatParam.iWrapWidth = 12;
	formatParam.iMaxHeight = 100;
	formatParam.iMaxLines = 10;
	formatParam.iEllipsis = 0xFFFF;
	formatParam.iStartChar = 0;
	formatParam.iEndChar = 50;
	formatParam.iLineInPar = 0;

	param.iOldLength = 2;
	param.iNewLength = 3;

	CTmTextLayout* text2 = new (ELeave) CTmTextLayout;

	text2->SetTextL(text1->Source(),formatParam);

	richText->InsertL(28, KContingentBreak); // Insert a contingent linebreak with no picture

	text2->FormatL(formatParam, param, out); // Scans the text from left to right.


	CTestPicture* picture2 = new (ELeave) CTestPicture;
	picture2->SetSizeInPixels( pictureSize, text1->Device() );

	TPictureHeader pictureHeader2;
	pictureHeader2.iPicture = picture2;
	pictureHeader2.iSize = pictureSize;

	richText->InsertL(34, pictureHeader2); // Insert a contingent linebreak with picture

	param.iOldLength = 3;
	param.iNewLength = 4;

	text2->FormatL(formatParam, param, out); // Scans the text from left to right.


	TestTextL(*text1);

	TESTPOINT(testEndLength == richText->DocumentLength());

	CleanupStack::PopAndDestroy(text1);
	CleanupStack::PopAndDestroy(richText);
	CleanupStack::PopAndDestroy(charLayer);
	CleanupStack::PopAndDestroy(paraLayer);
	}

/**
Tests RTmBoundingRectInterpreter.
@internalComponent
*/
TVerdict CTInterpreterStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    TESTPRINT(_L("TInterpreter unit"));
    TESTPRINT(_L("Start TInterpreter.exe Tests"));
	TestsL();
	Def077884L();
	return TestStepResult();
	}


