/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

// Test code for GetNextVisualCursorPos functionality
#include <e32test.h>

#include "TGraphicsContext.h"
#include <gdi.h>
#include "TAGMA.H"
#include "TmLayoutImp.h"
#include "TMINTERP.H"
#include "tbidicursorpos.h"

namespace LocalToTBidiCursorPos
{
CTBidiCursorPosStep* TestStep;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)

}

using namespace LocalToTBidiCursorPos;

class CTestSource : public CBase, public MTmSource
	{
public:
	static CTestSource* NewLC();
	~CTestSource();
	void ConstructL();
	// from MTmSource
	MGraphicsDeviceMap& FormatDevice() const { return *iScreenDevice; }
	MGraphicsDeviceMap& InterpretDevice() const { return *iScreenDevice; }
	TInt DocumentLength() const
		{
		return iText->Length();
		}
	void GetText(TInt aPos,TPtrC& aText, TTmCharFormat& aFormat) const
		{
		TTmCharFormat f;
		aFormat = f;
		aText.Set(iText->Mid(aPos));
		}
	void GetParagraphFormatL(TInt, RTmParFormat& aFormat) const
		{
		aFormat.CopyL(iParFormat);
		}
	TInt ParagraphStart(TInt) const { return 0; }

	void SetText(HBufC* aText) { iText = aText; }

private:
	CTestSource() {}

public:
	RTmParFormat iParFormat;
private:
	CTestGraphicsDevice* iScreenDevice;
	CWindowGc* iGc;
	HBufC* iText;
	};

// static
CTestSource* CTestSource::NewLC()
	{
	CTestSource* self = new(ELeave) CTestSource();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CTestSource::ConstructL()
	{
	TSize size(100, 100);
	iScreenDevice = CTestGraphicsDevice::NewL(size, 0);
	User::LeaveIfError(iScreenDevice->CreateContext(iGc));
	}

CTestSource::~CTestSource()
	{
	delete iScreenDevice;
	delete iGc;
	}

#define NO_OF_TEST_CASES 8

class TPos
	{
public:
	TInt iPos;
	TBool iLeading;
	};
class TAugmentedPos
	{
public:
	TInt iPos;
	TBool iLeading;
	TBool iOptional;
	};

const TAugmentedPos expectedCursorPos[NO_OF_TEST_CASES][40] = {
	{ {1, 0, 0}, {2, 0, 0}, {3, 0, 0}, {5, 1, 0},
		{4, 1, 0}, {3, 1, 0}, {7, 0, 0}, {8, 0, 0},
		{9, 0, 0} },
	{ {1, 0, 0}, {2, 0, 0}, {10, 1, 0}, {9, 1, 0},
		{8, 1, 0}, {7, 1, 0}, {6, 0, 0}, {7, 0, 0},
		{4, 1, 0}, {3, 1, 0}, {2, 1, 0}, {12, 0, 0},
		{13, 0, 0} },
	{ {1, 0, 0}, {2, 0, 0}, {5, 1, 1}, {4, 1, 0},
		{3, 1, 1}, {2, 1, 1}, {7, 0, 0}, {8, 0, 0},
		{9, 0, 0} },
	{ {1, 0, 0}, {2, 0, 0}, {3, 0, 0}, {5, 1, 0},
		{4, 1, 0}, {3, 1, 0}, {7, 0, 0}, {8, 0, 0},
		{9, 0, 0}, {11, 1, 0}, {10, 1, 0}, {9, 1, 0},
		{13, 0, 0}, {14, 0, 0}, {15, 0, 0} },
	{ {2, 0, 0}, {3, 0, 0}, {4, 0, 0} },
	{ {3, 0, 0}, {4, 0, 0} },
	{ {1, 0, 0}, {2, 0, 0}, {6, 1, 0}, {5, 1, 0},
		{4, 1, 0}, {3, 1, 0}, {2, 1, 0}, {8, 0, 0},
		{9, 0, 0} },
	{ {1, 0, 0}, {2, 0, 0}, {8, 1, 0}, {7, 1, 0},
		{6, 1, 0}, {5, 1, 0}, {4, 1, 0}, {3, 1, 0},
		{2, 1, 0}, {10, 0, 0}, {11, 0, 0} }
	};

const TPos startPos[NO_OF_TEST_CASES] = { {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1} };

const TInt numberOfCursorPositionsToCheck[NO_OF_TEST_CASES] = {9, 13, 9, 15, 3, 2, 9, 11};
const TBool moveToLeft[NO_OF_TEST_CASES] = {EFalse, EFalse, EFalse, ETrue, ETrue, EFalse, ETrue, ETrue};
const TBool rightToLeftPara[NO_OF_TEST_CASES] = {EFalse, EFalse, EFalse, ETrue, EFalse, EFalse, ETrue, ETrue};

void GetNextVisualCursorPosTestsL(CTmTextLayout* aLayout, CTestSource* aSource)
	{
	// Testcase 1
	_LIT(KBidi1, "abc\x644\x644\x644zzz\x2029");

	// Testcase 2 (0x32 and 0x39 changed to 0x6F2 and 0x0F9, see note below)
	_LIT(KBidi2, "ab\x644\x644\x644\x6F2\x6F9\x644\x644\x644\x644zy\x2029");

	// Testcase 3
	_LIT(KBidi3, "ab\x202ejk\x202cjkl\x2029");

	// Testcase 4
	_LIT(KBidi4, "\x644\x644\x644xyz\x644\x644\x644xyz\x644\x644\x644\x2029");

	// Testcase 5
	_LIT(KBidi5, "\x647\x64b\x647\x647\x2029");

	// Testcase 6
	_LIT(KBidi6, "a\x302\x317z\x2029");

	// Testcase 7 (0x31 and 0x32 changed to 0x6F1 and 0x0F2, see note below)
	_LIT(KBidi7, "\x644\x644\x6F1\x6F2\x2e\x33\x34\x644\x644\x2029");

	// Testcase 8 (0x31 - 0x35 changed to 0x6F1 - 0x0F5, see note below)
	_LIT(KBidi8, "\x644\x644\x6F1\x2c\x6F2\x6F3\x2e\x6F4\x6F5\x644\x644\x2029");

// Note: Test cases 2 and 7 have been changed to use \x6F0-\x6F9, Extended Arabic Digits,
// These have the bidirectional class "EN",the same as ASCII digits.
// These have been used in preference to ASCII digits
// to work around a CodeWarrior preprocessor/C-Cover problem.
// For more detils see DEF042186

	HBufC* logicalText[NO_OF_TEST_CASES];
	logicalText[0] = KBidi1().AllocLC();
	logicalText[1] = KBidi2().AllocLC();
	logicalText[2] = KBidi3().AllocLC();
	logicalText[3] = KBidi4().AllocLC();
	logicalText[4] = KBidi5().AllocLC();
	logicalText[5] = KBidi6().AllocLC();
	logicalText[6] = KBidi7().AllocLC();
	logicalText[7] = KBidi8().AllocLC();

	TTmFormatParam format;
	format.iStartChar = 0;
	format.iEndChar = 0;
	format.iLineInPar = 0;

	TTmDocPos pos(0, ETrue);
	TTmPosInfo2 info;
	int charIndex;
	TBuf<40> msg;

	for (TInt ii = 0; ii < NO_OF_TEST_CASES; ii++)
		{
		msg.Format(_L("GetNextVisualCursorPos test case %d\n"), ii + 1);
		TESTPRINT(msg);
		aSource->SetText(logicalText[ii]);
		format.iEndChar = logicalText[ii]->Length();
		if (rightToLeftPara[ii])
			aSource->iParFormat.iFlags |= RTmParFormat::ERightToLeft;
		aLayout->SetTextL(*aSource, format);

		charIndex = 0;
		pos.iPos = startPos[ii].iPos;
		pos.iLeadingEdge = startPos[ii].iLeading;
		while (aLayout->GetNextVisualCursorPos(pos, info, moveToLeft[ii]))
			{
			while (info.iDocPos.iPos != expectedCursorPos[ii][charIndex].iPos
				|| info.iDocPos.iLeadingEdge != expectedCursorPos[ii][charIndex].iLeading)
				{
                TESTPOINT(expectedCursorPos[ii][charIndex].iOptional);
				++charIndex;
				TESTPOINT(charIndex != numberOfCursorPositionsToCheck[ii]);
				}
			pos = info.iDocPos;
			++charIndex;
			TESTPOINT(charIndex <= numberOfCursorPositionsToCheck[ii]);
			}
		TESTPOINT(charIndex == numberOfCursorPositionsToCheck[ii]);
		aSource->iParFormat.iFlags &= ~(RTmParFormat::ERightToLeft); // reset back to default of LeftToRight
		}

	CleanupStack::PopAndDestroy(NO_OF_TEST_CASES); // cleanup all HBufC* stored in logicalText array
	}

/**
@SYMTestCaseID 			SYSLIB-FORM-UT-3610
@SYMTestCaseDesc  		Testing behaviour of r2l text when wrapping occurs and bidirectional text allowed
@SYMTestPriority  		High
@SYMTestActions 		1. Have some arabic text
						2. Set wrap width to 10 chars
						3. Set alignment to NormalBidirectional
						4. Call CTmTextLayout::SetTextL
						5. Retrieve the cursor position
						6. Check the lineinfo to ensure that the innerRect TL.iX parameter is positive.
@SYMTestExpectedResults	The innerRect that holds text top left x position is positive.
@SYMDEF					DEF109737,PDEF110819,PDEF110820,PDEF110821
*/
void DEF109737(CTmTextLayout* aLayout, CTestSource* aSource)
	{
	//R2L paragraph wrapping
	_LIT(KLllHahHah1, "lll\x62D\x62D\x2029\x62D\x62D\x2029");
	HBufC* buf = HBufC::NewLC(KLllHahHah1().Length());
	*buf = KLllHahHah1;
	aSource->SetText(buf);
	aSource->iParFormat.iFlags = 1;
	aSource->iParFormat.iAlignment = RTmParFormat::EAlignNormalBidirectional;
	TTmFormatParam format;
	format.iStartChar = 0;
	format.iEndChar = buf->Length();
	format.iLineInPar = 0;
	format.iWrapWidth = 10;
	aLayout->SetTextL(*aSource, format);
	TTmLineInfo lineInfo;
	TPoint position;
	TInt width;
	TInt ascent;
	TInt descent;
	TInt type = 0;
	TInt pos = 0;
	TTmDocPosSpec posSpec(pos,static_cast<TTmDocPosSpec::TType>(type));
	TBool result = aLayout->GetCursor(posSpec, ECursorVertical,
									lineInfo, position, width, ascent, descent);
	TESTPOINT(result);
	//Test that the LHS is non negative. Because we are allowing bidirectional text the
	//text shouldnt wrap to next line but instead the cursor can scroll left or right to see the text.
	TESTPOINT(lineInfo.iInnerRect.iTl.iX >= 0);
	CleanupStack::PopAndDestroy(buf);
	}

/** INC041367 - Cursor in wrong position when it is one char before the
beginning of RTL text.

Tests that the cursor is always hanging to the right in a left to right
paragraph. This prevents the cursor from being confusingly positioned on top of
a little L, for example.
*/
void INC041367(CTmTextLayout* aLayout, CTestSource* aSource)
	{
	_LIT(KLllHahHah, "lll\x62D\x62D\x2029");

	HBufC* buf = HBufC::NewLC(KLllHahHah().Length());
	*buf = KLllHahHah;
	aSource->SetText(buf);
	aSource->iParFormat.iFlags = 0;
	TTmFormatParam format;
	format.iStartChar = 0;
	format.iEndChar = buf->Length();
	format.iLineInPar = 0;
	aLayout->SetTextL(*aSource, format);
	TTmLineInfo lineInfo;
	for (TInt pos = 0; pos != 6; ++pos)
		{
		for (TInt type = 0; type != 4; ++type)
			{
			TPoint position;
			TInt width;
			TInt ascent;
			TInt descent;
			TTmDocPosSpec posSpec(pos,
				static_cast<TTmDocPosSpec::TType>(type));
			TBool result = aLayout->GetCursor(posSpec, ECursorVertical,
				lineInfo, position, width, ascent, descent);
			TESTPOINT(result);
			TESTPOINT(0 < width);
			}
		}
	CleanupStack::PopAndDestroy(buf);
	}

TVerdict CTBidiCursorPosStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    TESTPRINT(_L("TBidiCursorPos - GetNextVisualCursorPos tests"));
	CTmTextLayout* layout = new(ELeave) CTmTextLayout;
	CleanupStack::PushL(layout);
	CTestSource* source = CTestSource::NewLC();
	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-3610 GetNextVisualCursorPos tests "));
	GetNextVisualCursorPosTestsL(layout, source);
	TESTPRINT(_L("INC041367"));
	INC041367(layout, source);
	TESTPRINT(_L("DEF109737"));
	DEF109737(layout, source);
	
	CleanupStack::PopAndDestroy(source);
	CleanupStack::PopAndDestroy(layout);
	return TestStepResult();
	}

