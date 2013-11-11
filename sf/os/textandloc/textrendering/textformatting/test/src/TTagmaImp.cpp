/*
* Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* TTagmaImp.cpp test file for Tagma classes
*
*/


#include "TGraphicsContext.h"
#include <e32test.h>
#include <e32std.h>
#include <coemain.h>
#include <gdi.h>
#include <txtlaydc.h>
#include "TAGMA.H"
#include "TMSTD.H"
#include "TmLayoutImp.h"
#include "TMINTERP.H"
#include "TmText.h"
#include "InlineText.h"
#include "ttagmaimp.h"

namespace LocalToTTagmaImp
{
enum TTagmaImpPanic { EAccessOutsideText = 1 };
void Panic(TTagmaImpPanic)
	{
	User::Panic(_L("TTagmaImp"), EAccessOutsideText);
	}

CTTagmaImpStep* TestStep = NULL;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)

const TInt KPictureCharacter = 0xFFFC;

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
class TDocModel : public MLayDoc
	{
public:
	TDocModel(const TDesC& aDes)
		: iDes(&aDes), iParagraphFormat(0) {}
	void SetParagraphFormat(CParaFormat* a)
		{
		iParagraphFormat = a;
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
	TBool SelectParagraphLabel(TInt) { return EFalse; }
	void CancelSelectLabel() {}
private:
	const TDesC* iDes;
	CParaFormat* iParagraphFormat;
	};

class THandleTester
	{
	TInt iProcessHandles;
	TInt iThreadHandles;
public:
	THandleTester()
		{
		RThread().HandleCount(iProcessHandles, iThreadHandles);
		}
	~THandleTester()
		{
		TInt p;
		TInt t;
		RThread().HandleCount(p, t);
// this seems to break at random...
//		TestStep->test(p == iProcessHandles);
		TESTPOINT(t == iThreadHandles);
		}
	};

class CFallableDeviceMap : public CBase, public MGraphicsDeviceMap
	{
	MGraphicsDeviceMap& iDeviceMap;
public:
	CFallableDeviceMap(MGraphicsDeviceMap& aDeviceMap) : iDeviceMap(aDeviceMap) {}
	TInt HorizontalTwipsToPixels(TInt aTwips) const
		{
		return iDeviceMap.HorizontalTwipsToPixels(aTwips);
		}
	TInt VerticalTwipsToPixels(TInt aTwips) const
		{
		return iDeviceMap.VerticalTwipsToPixels(aTwips);
		}
	TInt HorizontalPixelsToTwips(TInt aPixels) const
		{
		return iDeviceMap.HorizontalPixelsToTwips(aPixels);
		}
	TInt VerticalPixelsToTwips(TInt aPixels) const
		{
		return iDeviceMap.VerticalPixelsToTwips(aPixels);
		}
	TInt GetNearestFontInTwips(CFont*& aFont,const TFontSpec& aFontSpec)
		{
		TInt *i = new TInt;
		if (i)
			{
			delete i;
			return iDeviceMap.GetNearestFontInTwips(aFont, aFontSpec);
			}
		return KErrNoMemory;
		}
	void ReleaseFont(CFont* aFont)
		{
		iDeviceMap.ReleaseFont(aFont);
		}
	};

class CTestSource : public CBase, public MTmSource
	{
	MGraphicsDeviceMap* iDeviceMap;
	TPtrC iText;
	TTmCharFormat iDefaultCharFormat;
	TTmCharFormat iSpecialCharFormat;
	TInt iBeginSpecialCharFormat;
	TInt iEndSpecialCharFormat;
public:
	~CTestSource()
		{
		delete iDeviceMap;
		}
	CTestSource(const TDesC& aText,
		TTmCharFormat aDefault, TTmCharFormat aSpecial,
		TInt aBeginSpecialCharFormat, TInt aEndSpecialCharFormat)
		: iText(aText),
		iDefaultCharFormat(aDefault), iSpecialCharFormat(aSpecial),
		iBeginSpecialCharFormat(aBeginSpecialCharFormat),
		iEndSpecialCharFormat(aEndSpecialCharFormat)
		{
		}
	void ConstructL(MGraphicsDeviceMap& aDeviceMap)
		{
		iDeviceMap = new(ELeave) CFallableDeviceMap(aDeviceMap);
		}
	MGraphicsDeviceMap& FormatDevice() const { return *iDeviceMap; }
	MGraphicsDeviceMap& InterpretDevice() const { return *iDeviceMap; }
	TInt DocumentLength() const { return iText.Length(); }
	void SetText(const TDesC& aStaticText)
		{
		iText.Set(aStaticText);
		}
	void GetText(TInt aPos, TPtrC& aText, TTmCharFormat& aFormat) const
		{
		TInt length = iText.Length()+1;
		if (aPos < iBeginSpecialCharFormat)
			{
			length = iBeginSpecialCharFormat;
			aFormat = iDefaultCharFormat;
			}
		else if (aPos < iEndSpecialCharFormat)
			{
			length = iEndSpecialCharFormat;
			aFormat = iSpecialCharFormat;
			}
		else
			{
			aFormat = iDefaultCharFormat;
			}
		if (aPos == length)
			aText.Set(0, 0);
		else
			aText.Set(&iText[aPos], length - aPos);
		}

	void GetParagraphFormatL(TInt, RTmParFormat& aFormat) const
		{
		RTmParFormat defParFormat;
		CleanupClosePushL(defParFormat);
		aFormat.CopyL(defParFormat);
		CleanupStack::PopAndDestroy();
		}
	TInt ParagraphStart(TInt aPos) const
		{
		if (aPos <= iText.Length())
			aPos = iText.Length() - 1;
		TPtrC p(&iText[0], aPos + 1);
		TInt delim = p.LocateReverse(TChar(CEditableText::EParagraphDelimiter));
		return delim < 0? 0 : delim + 1;
		}
	CPicture* PictureL(TInt aPos) const
		{
		// There are no pictures, but we need to check that code never tries to
		// take one from beyond the bounds of the text.
		__ASSERT_ALWAYS(0 <= aPos, Panic(EAccessOutsideText));
		__ASSERT_ALWAYS(aPos < iText.Length(), Panic(EAccessOutsideText));
		return 0;
		}
	};

}
using namespace LocalToTTagmaImp;


class CTagmaImpTest : public CBase
	{
public:
	CTagmaImpTest(CTTagmaImpStep* aStep) : iDevice(0), iGc(0)
		{
        TestStep = aStep;
		}
	void ConstructL()
		{
		TSize size(100, 100);
		iDevice = CTestGraphicsDevice::NewL(size, 0);
		iDevice->CreateContext(iGc);
 		}
 	~CTagmaImpTest()
 		{
 		delete iDevice;
 		}
	void ExerciseCTmCodeL();
	void AddSome1L(CTmCode& aCode, TInt aStartValue);
	TInt AddLotsL(CTmCode& aCode, TInt aAmountToAdd);
	TInt InsertSomeL(CTmCode& aCode, TInt aStartValue, TInt aIndex);
	TInt InsertLotsL(CTmCode& aCode, TInt aAmountToAdd, TInt aIndex);
	TInt CheckLots(CTmCode& aCode, TInt aIndex, TInt aStartValue, TInt aCount);
	void CTmCodeOOML();
	CTestSource* NewTestSourceLC();
	CTmCode* NewFormattedCTmCodeLC(MTmSource&);
	void ExerciseFormatL();
	void FormatOOML();
	void ExerciseRTmTextCacheWidthL();
	void RTmTextCacheWidthOOM();
	void ExerciseRTmGeneralInterpreterGetDisplayedTextL();
	void RTmGeneralInterpreterGetDisplayedTextOOM();
	void CTmTextImp_RRunArrayL();
	void WEP_55BHBF_DefectL();
	void BUR_58FGE8_DefectL();
	void EXT_5ATF8D_DefectL();
	void INC_044969_DefectL();
	void DEF_073838_DefectL();
	void PDEF_101464_DefectL();
	void DEF101994_DefectL();
	void CTmTextImp_BidirectionalAlignmentL();
	void ExerciseCopyL();
	void ExerciseGetCParaFormatL();
	void CustomFormattingL();
	void TestChunkContext();
	void TestBytecodeLineContext();
	void TestBytecodeChunkContext();
	void TestL();

	void GetIndices(TDes8& aBuf, const CTmTextImp::RRunArray& aRunArray);
	// Function to test the API's of CTmTextImp
	void TestAPIL();
	private:
	CTestGraphicsDevice* iDevice;
	CGraphicsContext* iGc;
	};

TInt CTagmaImpTest::CheckLots(CTmCode& aCode, TInt aIndex,
	TInt aStartValue, TInt aCount)
	{
	TTmCodeReader reader(aCode, aIndex, aCode.Size());
	for (; aCount; --aCount, aStartValue += 6)
		{
		TUint8 byte = reader.ReadByte();
		TInt num = reader.ReadNumber();
		TRect rect = reader.ReadRect();
		TRect testRect(aStartValue + 2, aStartValue + 3, aStartValue + 4, aStartValue + 5);
		TESTPOINT(byte == static_cast<TUint8>(aStartValue));
		TESTPOINT(num == aStartValue + 1);
		TESTPOINT(rect == testRect);
		}
	return reader.CodePos();
	}

void CTagmaImpTest::AddSome1L(CTmCode& aCode, TInt aStartValue)
	{
	aCode.AppendByteL(static_cast<TUint8>(aStartValue));
	aCode.AppendNumberL(aStartValue + 1);
	TRect rect(aStartValue + 2, aStartValue + 3, aStartValue + 4, aStartValue + 5);
	aCode.AppendRectL(rect);
	}

TInt CTagmaImpTest::AddLotsL(CTmCode& aCode, TInt aAmountToAdd)
	{
	TInt finalSize = aAmountToAdd + aCode.Size();
	TInt count = 0;
	for (TInt i = 0; aCode.Size() < finalSize; i += 6)
		{
		AddSome1L(aCode, i);
		++count;
		}
	return count;
	}

TInt CTagmaImpTest::InsertSomeL(CTmCode& aCode, TInt aStartValue, TInt aIndex)
	{
	TInt offset = aIndex - aCode.Size();
	aCode.InsertByteL(static_cast<TUint8>(aStartValue), aIndex);
	aCode.InsertNumberL(aStartValue + 1, aCode.Size() + offset);
	TRect rect(aStartValue + 2, aStartValue + 3, aStartValue + 4, aStartValue + 5);
	aCode.InsertRectL(rect, aCode.Size() + offset);
	return aCode.Size() + offset;
	}

TInt CTagmaImpTest::InsertLotsL(CTmCode& aCode, TInt aAmountToAdd, TInt aIndex)
	{
	TInt finalSize = aAmountToAdd + aCode.Size();
	TInt count = 0;
	for (TInt i = 0; aCode.Size() < finalSize; i += 6)
		{
		aIndex = InsertSomeL(aCode, i, aIndex);
		++count;
		}
	return count;
	}

void CTagmaImpTest::ExerciseCTmCodeL()
	{
	CTmCode* code = new (ELeave) CTmCode;
	CleanupStack::PushL(code);
	code->CreateBufferL();
	TInt count1 = AddLotsL(*code, 400);
	TInt size1 = code->Size();
	TInt count2 = AddLotsL(*code, 2000);
	TInt size2 = code->Size() - size1;
	TInt count3 = InsertLotsL(*code, 997, size1);
	TInt size3 = code->Size() - size1 - size2;
	CheckLots(*code, 0, 0, count1);
	CheckLots(*code, size1, 0, count3);
	CheckLots(*code, size1 + size3, 0, count2);
	CTmCode* code2 = new (ELeave) CTmCode;
	CleanupStack::PushL(code2);
	code2->CreateBufferL();
	TInt count4 = AddLotsL(*code2, 3000);
	TInt size4 = code2->Size();
	code->ChangeL(size1, size1 + size3, *code2);
	CheckLots(*code, 0, 0, count1);
	CheckLots(*code, size1, 0, count4);
	CheckLots(*code, size1 + size4, 0, count2);
	code->Delete(size1, size4);
	CheckLots(*code, 0, 0, count1);
	CheckLots(*code, size1, 0, count2);
	TESTPOINT(code2->Size() == 0);
	CleanupStack::PopAndDestroy(code2);
	CleanupStack::PopAndDestroy(code);
	}

void CTagmaImpTest::CTmCodeOOML()
	{
	ExerciseCTmCodeL();
	TInt err;
	TInt failAt = 1;
	do {
		THandleTester h();

		__UHEAP_MARK;
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		TRAP(err, ExerciseCTmCodeL());
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);

		++failAt;
		} while (err == KErrNoMemory);
	TESTPOINT(err == KErrNone);
	}

CTestSource* CTagmaImpTest::NewTestSourceLC()
	{
	_LIT(KTestText, "Some text for you to format for me.\x2029");
	TTmCharFormat defCharFormat(_L("SwissA"), 60);
	TTmCharFormat specialCharFormat(_L("Arial"), 50);
	CTestSource* source = new(ELeave) CTestSource(KTestText,
		defCharFormat, specialCharFormat, 5, 9);
	CleanupStack::PushL(source);
	source->ConstructL(*iDevice);
	return source;
	}

CTmCode* CTagmaImpTest::NewFormattedCTmCodeLC(MTmSource& aSource)
	{
	CTmCode* code = new(ELeave) CTmCode;
	CleanupStack::PushL(code);
	TTmFormatParam formatParam;
	formatParam.iStartChar = 0;
	formatParam.iEndChar = aSource.DocumentLength();
	formatParam.iLineInPar = 0;
	formatParam.iWrapWidth = 50;
	formatParam.iMaxHeight = KMaxTInt;
	formatParam.iMaxLines = KMaxTInt;
	formatParam.iFlags = TTmFormatParam::EWrap;
	CTmFormatContext::TInfo info;
	CTmTextLayout *layout = new(ELeave) CTmTextLayout;
	CleanupStack::PushL(layout);
	CTmFormatContext::FormatL(aSource, formatParam, *code, info, layout);
	CleanupStack::PopAndDestroy(layout);
	return code;
	}

void CTagmaImpTest::ExerciseFormatL()
	{
	CTestSource* source = NewTestSourceLC();
	CTmCode* code = NewFormattedCTmCodeLC(*source);
	CleanupStack::PopAndDestroy(code);
	CleanupStack::PopAndDestroy(source);
	}

void CTagmaImpTest::FormatOOML()
	{
	// dry run- this will help the typeface store to settle down.
	ExerciseFormatL();
	TInt err;
	TInt failAt = 1;
	do {
		THandleTester h();

		__UHEAP_MARK;
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		TRAP(err, ExerciseFormatL());
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);

		++failAt;
		} while (err == KErrNoMemory);
	TESTPOINT(err == KErrNone);
	}

void CTagmaImpTest::ExerciseRTmTextCacheWidthL()
	{
	_LIT(KOnions, "How easily happiness starts, slicing onions.\x2029");
	TTmCharFormat defCharFormat(_L("SwissA"), 60);
	TTmCharFormat specialCharFormat(_L("Arial"), 50);
	CTestSource* source = new (ELeave) CTestSource(KOnions,
		defCharFormat, specialCharFormat, 11, 20);
	CleanupStack::PushL(source);
	source->ConstructL(*iDevice);
	RTmTextCache textCache(*source, source->FormatDevice());
	CleanupClosePushL(textCache);
	textCache.TotalWidthL(0, KOnions().Length(), EFalse);
	textCache.AdvanceWidthL(0, KOnions().Length(), EFalse);
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy(source);
	}

void CTagmaImpTest::RTmTextCacheWidthOOM()
	{
	TInt err;
	TInt failAt = 1;
	do {
		THandleTester h();

		__UHEAP_MARK;
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		TRAP(err, ExerciseRTmTextCacheWidthL());
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);

		++failAt;
		} while (err == KErrNoMemory);
	TESTPOINT(err == KErrNone);
	}

void CTagmaImpTest::ExerciseRTmGeneralInterpreterGetDisplayedTextL()
	{
	CTestSource* source = NewTestSourceLC();
	CTmCode* code = NewFormattedCTmCodeLC(*source);
	TTmInterpreterParam param(*code);
	param.iCodeStart = 0;
	param.iCodeEnd = code->Size();
	param.iTextStart = 0;
	param.iWidth = 50;
	RTmGeneralInterpreter interp(*source, param);
	CleanupClosePushL(interp);
	TInt needed;
	TBuf<50> buf;
	interp.GetDisplayedTextL(0, buf,0, needed);
	CleanupStack::PopAndDestroy();		// interp.Close()
	RTmGeneralInterpreter interp2(*source, param);
	CleanupClosePushL(interp2);
	interp2.GetDisplayedTextL(1, buf, 0,needed);
	CleanupStack::PopAndDestroy();		// interp2.Close()
	CleanupStack::PopAndDestroy(code);
	CleanupStack::PopAndDestroy(source);
	}

void CTagmaImpTest::RTmGeneralInterpreterGetDisplayedTextOOM()
	{
	TInt err;
	TInt failAt = 1;
	do {
		THandleTester h();

		__UHEAP_MARK;
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		TRAP(err, ExerciseRTmGeneralInterpreterGetDisplayedTextL());
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		__UHEAP_MARKENDC(0);

		++failAt;
		} while (err == KErrNoMemory);
	TESTPOINT(err == KErrNone);
	}

void CTagmaImpTest::WEP_55BHBF_DefectL()
	{
	__UHEAP_MARK;
	// defect WEP-55BHBF: EikLabels index out of bounds!!!!
	// There is a picture character at the end to trick GetLineBreakL
	// into looking outside the text for a picture.
	_LIT(KHello, "hello\xFFFC");
	TPtrC helloSeg(KHello().Ptr(), 5);
	CTestSource* source = NewTestSourceLC();

	TInt bp, hc, bpas;
	source->SetText(helloSeg);
	source->GetLineBreakL(helloSeg, 0, 0, 5, ETrue, bp, hc, bpas);
	CleanupStack::PopAndDestroy();
	__UHEAP_MARKEND;
	}

/** test code for INC044969 - Word app: Missing words on right of page

	When Word is formating a block of text which consists of 8 repeats of
	"this is a test " and the line break occurs just after "a", the defect caused the "a"
	to disappear, so the line ended with "this is" and the next line started with
	"test".
 */
void CTagmaImpTest::INC_044969_DefectL()
	{

	// create a CTestSource containing the test data
	_LIT(KTest, " this is a test this is a test this is a test this is a test this is a test this is a test this is a test this is a test.");
	TPtrC testSeg(KTest().Ptr(), KTest().Length() );
	CTestSource* source = NewTestSourceLC();
	source->SetText(testSeg);

	CTmCode* code = new(ELeave) CTmCode;
	CleanupStack::PushL(code);

	// format the text, a wrap width of 100 should produce
	// the required break position of 10 (just after the a of " this is a")
	CTmFormatContext::TInfo info;
	TTmFormatParam formatParam;
	formatParam.iStartChar = 0;
	formatParam.iEndChar = source->DocumentLength();
	formatParam.iLineInPar = 0;
	formatParam.iMaxHeight = KMaxTInt;
	formatParam.iMaxLines = KMaxTInt;
	formatParam.iFlags = TTmFormatParam::EWrap;
	formatParam.iWrapWidth = 100;
	CTmTextLayout* layout = new(ELeave) CTmTextLayout;
	CleanupStack::PushL(layout);
	CTmFormatContext::FormatL(*source, formatParam, *code, info, layout);

	// check that format has found the break at 11 characters
	// This is at the end of "this is a "
	TESTPOINT(info.iFirstLineEndChar == 11 );

	// get the text that would be displayed into displayBuffer
	TTmInterpreterParam interpreter_param(*code);
	interpreter_param.iCodeStart = 0;
	interpreter_param.iCodeEnd = code->Size();
	interpreter_param.iTextStart = 0;
	interpreter_param.iWidth = 50;
	RTmGeneralInterpreter interpreter(*source, interpreter_param);
	TInt noCharsToDisplay =0;
	TBuf<200> displayBuffer;
	interpreter.GetDisplayedTextL(0, displayBuffer,0, noCharsToDisplay);

	// now check the content of the buffer returned
	// The defect caused this buffer to be missing the final 'a'
	// from "...this is a" at the end of the line
	// When this defect happens noCharsToDisplay != info.iFirstLineEndChar
	TESTPOINT( info.iFirstLineEndChar == noCharsToDisplay );
	TESTPOINT( displayBuffer[9] == 'a' );

	interpreter.Close();
	CleanupStack::PopAndDestroy(layout);
	CleanupStack::PopAndDestroy(code);   // code 
	CleanupStack::PopAndDestroy(source); // source

	}

/** test code for DEF073838 - Line break problem with WORD

	When appending a space to the last line of a Word document, the defect
	caused the last word of the line to be moved to a new line as if the space
	were part of the word itself. So, if a paragraph ended with the words
	"this is a test " and the final space was after the right margin, the line
	was broken at the position of the 't' of "test".
 */
void CTagmaImpTest::DEF_073838_DefectL()
	{
	CTmCode* code = new(ELeave) CTmCode;
	CleanupStack::PushL(code);


	// 1. format the text, a wrap width of 520 should produce no line breaks.
	// 	0x2029 is EParagraphDelimiter
	CTestSource* source1 = NewTestSourceLC();

	_LIT(KTest1, "Quo usque tandem, Catilina, abutere patientia nostra \x2029");
	TPtrC testSeg1(KTest1().Ptr(), KTest1().Length() );
	source1->SetText(testSeg1);

	CTmFormatContext::TInfo info1;
	TTmFormatParam formatParam1;
	formatParam1.iStartChar = 0;
	formatParam1.iEndChar = source1->DocumentLength();
	formatParam1.iLineInPar = 0;
	formatParam1.iMaxHeight = KMaxTInt;
	formatParam1.iMaxLines = KMaxTInt;
	formatParam1.iFlags = TTmFormatParam::EWrap;
	formatParam1.iWrapWidth = 520;
	CTmTextLayout* layout = new(ELeave) CTmTextLayout;
	CleanupStack::PushL(layout);
	CTmFormatContext::FormatL(*source1, formatParam1, *code, info1, layout);

	// check that format didn't found any breaks
	TESTPOINT( info1.iFirstLineEndChar == KTest1().Length());
	TESTPOINT( info1.iLastLineStartChar == 0 );
	TESTPOINT( info1.iHeight == 12 ); // just 1 line



	// 2. now append more than one space and verify that the line is not broken
	CTestSource* source2 = NewTestSourceLC();

	_LIT(KTest2, "Quo usque tandem, Catilina, abutere patientia nostra        \x2029");
	TPtrC testSeg2(KTest2().Ptr(), KTest2().Length() );
	source2->SetText(testSeg2);

	CTmFormatContext::TInfo info2;
	TTmFormatParam formatParam2;
	formatParam2.iStartChar = 0;
	formatParam2.iEndChar = source2->DocumentLength();
	formatParam2.iLineInPar = 0;
	formatParam2.iMaxHeight = KMaxTInt;
	formatParam2.iMaxLines = KMaxTInt;
	formatParam2.iFlags = TTmFormatParam::EWrap;
	formatParam2.iWrapWidth = 520;
	CTmFormatContext::FormatL(*source2, formatParam2, *code, info2, layout);

	// check that format didn't found any breaks
	TESTPOINT( info2.iFirstLineEndChar == KTest2().Length());
	TESTPOINT( info2.iLastLineStartChar == 0 );
	TESTPOINT( info2.iHeight == 12 ); // just 1 line



	// 3. now test with page breaks (0x000C) and line breaks (0x2028)
	CTestSource* source3 = NewTestSourceLC();

	_LIT(KTest3, "Quo usque tandem, Catilina, abutere patientia nostra \x2028Quo usque tandem, Catilina, abutere patientia nostra \x000CQuo usque tandem, Catilina, abutere patientia nostra \x2029");
	TPtrC testSeg3(KTest3().Ptr(), KTest3().Length() );
	source3->SetText(testSeg3);

	CTmFormatContext::TInfo info3;
	TTmFormatParam formatParam3;
	formatParam3.iStartChar = 0;
	formatParam3.iEndChar = source3->DocumentLength();
	formatParam3.iLineInPar = 0;
	formatParam3.iMaxHeight = KMaxTInt;
	formatParam3.iMaxLines = KMaxTInt;
	formatParam3.iFlags = TTmFormatParam::EWrap;
	formatParam3.iWrapWidth = 520;
	CTmFormatContext::FormatL(*source3, formatParam3, *code, info3, layout);

	// check that format didn't found any breaks
	TESTPOINT( info3.iFirstLineEndChar == 54 );
	TESTPOINT( info3.iLastLineStartChar == 108 );
	TESTPOINT( info3.iHeight == 36 ); // 3 lines

	CleanupStack::PopAndDestroy(source3);
	CleanupStack::PopAndDestroy(source2);
	CleanupStack::PopAndDestroy(layout); 
	CleanupStack::PopAndDestroy(source1);
	CleanupStack::PopAndDestroy(code);
	}

class TTestParLabelSource : public MTmSource, public MFormLabelApi
	{
	MGraphicsDeviceMap* iDevice;
	TPtrC iBody;
	TPtrC iLabel;
	TPtrC* iCurrent;
public:
	TTestParLabelSource(MGraphicsDeviceMap* aDevice,
		const TDesC& aBody, const TDesC& aLabel)
		: iDevice(aDevice)
		{
		iBody.Set(aBody.Ptr(), aBody.Length());
		iLabel.Set(aLabel.Ptr(), aLabel.Length());
		iCurrent = &iBody;
		}
	void Close()
		{
		}
	TInt DocumentLength() const { return iCurrent->Length(); }
	MGraphicsDeviceMap& FormatDevice() const
		{
		return *iDevice;
		}
	TAny* GetExtendedInterface(const TUid& aInterfaceId)
		{
		if (aInterfaceId == KFormLabelApiExtensionUid)
			{
			return static_cast<MFormLabelApi*>(this);
			}
		else
			{
			// In this instance, calling the parent class will always return NULL
			// but the pattern should be followed by all implementors for safety
			return MTmSource::GetExtendedInterface(aInterfaceId);
			}
		}
	void LabelMetrics(TLabelType /*aType*/, TSize& aLabelSize, TInt& aMarginSize) const
		{
		aLabelSize.iWidth = 100;
		aLabelSize.iHeight = 10;
		aMarginSize = 10;
		}
	void GetParagraphFormatL(TInt aPos, RTmParFormat& aFormat) const
		{
		__ASSERT_ALWAYS(0 <= aPos, User::Invariant());
		__ASSERT_ALWAYS(aPos <= iCurrent->Length(), User::Invariant());
		const RTmParFormat f;
		aFormat.CopyL(f);
		}
	void GetText(TInt aPos,TPtrC& aText,TTmCharFormat& aFormat) const
		{
		ASSERT(0 <= aPos);
		ASSERT(aPos <= iCurrent->Length());
		aText.Set(iCurrent->Ptr() + aPos, iCurrent->Length()+1 - aPos);
		TTmCharFormat f;
		aFormat = f;
		}
	MGraphicsDeviceMap& InterpretDevice() const
		{
		return *iDevice;
		}
	TInt ParagraphStart(TInt) const { return 0; }
	TBool LabelModeSelect(TLabelType,TInt)
		{
		if (iCurrent == &iLabel)
			return EFalse;
		iCurrent = &iLabel;
		return ETrue;
		}
	void LabelModeCancel() { iCurrent = &iBody; }
	TRgb SystemColor(TUint aColorIndex,TRgb aDefaultColor) const
		{
		if (aColorIndex == TLogicalRgb::ESystemSelectionForegroundIndex)
			return KRgbWhite;
		if (aColorIndex == TLogicalRgb::ESystemSelectionBackgroundIndex)
			return KRgbRed;
		return MTmSource::SystemColor(aColorIndex, aDefaultColor);
		}
	};

void CTagmaImpTest::BUR_58FGE8_DefectL()
	{
	// Int: Application panic when selecting, Select All from the edit menu
	_LIT(KBody, "small\x2029");
	_LIT(KLabel, "longer than the body\x2029");
	TTestParLabelSource s(iDevice, KBody, KLabel);
	CTmTextLayout* lay = new(ELeave) CTmTextLayout;
	TTmFormatParam fp;
	fp.iWrapWidth = 200;
	fp.iEndChar = KBody().Length();
	lay->SetTextL(s, fp);
	TPoint zero;
	TRect clip(0, 0, 200, 200);
	lay->HighlightSection(*iGc, zero, 0, KBody().Length(), clip);
	s.Close();
	}

/**
 * EXT-5ATF8D: "TAGMA - different character formats" The complaint is that
 * CTmText does not seem to display differently formatted characters in their
 * different formats. The defect is due to the fact that the function to get
 * the text from the buffer ignores the length of run of characters, so the
 * rendering assumes that all the characters are in the same format.
 * @internalComponent
 */
void CTagmaImpTest::EXT_5ATF8D_DefectL()
	{
	TTmFormatParamBase formatParam;
	CTmTextImp* text = new(ELeave)
		CTmTextImp(*iDevice, formatParam);
	CleanupStack::PushL(text);

	TTmCharFormat cf1;
	cf1.iEffects |= TTmCharFormat::EUnderline;
	TTmCharFormat cf2;
	cf2.iEffects &= ~TTmCharFormat::EUnderline;
	RTmParFormat pf1;
	CleanupClosePushL(pf1);
	pf1.iSpaceAbove = 1;
	RTmParFormat pf2;
	CleanupClosePushL(pf2);
	pf2.iSpaceAbove = 2;
	text->InsertL(0, _L("first line\x2029second."), &cf1, &pf1, 0, 0);
	text->InsertL(14, _L("t p"), &cf2, &pf2, 0, 0);

	TPtrC testText;
	TTmCharFormat cft;
	RTmParFormat pft;
	CleanupClosePushL(pft);

	text->GetText(0, testText, cft);
	text->GetParagraphFormatL(0, pft);
	TESTPOINT(testText.Length() == 14);
	TESTPOINT(testText.Compare(_L("first line\x2029sec")) == 0);
	TESTPOINT(cft == cf1);
	TESTPOINT(pft == pf1);

	text->GetText(11, testText, cft);
	text->GetParagraphFormatL(11, pft);
	TESTPOINT(testText.Length() == 3);
	TESTPOINT(testText.Compare(_L("sec")) == 0);
	TESTPOINT(cft == cf1);
	TESTPOINT(pft == pf2);

	text->GetText(14, testText, cft);
	text->GetParagraphFormatL(14, pft);
	TESTPOINT(testText.Length() == 3);
	TESTPOINT(testText.Compare(_L("t p")) == 0);
	TESTPOINT(cft == cf2);
	TESTPOINT(pft == pf2);

	text->GetText(17, testText, cft);
	text->GetParagraphFormatL(17, pft);
	TESTPOINT(cft == cf1);
	TESTPOINT(pft == pf2);

	text->GetText(21, testText, cft);
	text->GetParagraphFormatL(21, pft);
	TESTPOINT(pft == pf2);
	CleanupStack::PopAndDestroy(&pft);
	CleanupStack::PopAndDestroy(&pf2);
	CleanupStack::PopAndDestroy(&pf1);
	CleanupStack::PopAndDestroy(text);
	}

void CTagmaImpTest::GetIndices(TDes8& aBuf, const CTmTextImp::RRunArray& aRunArray)
	{
    TESTPOINT(aRunArray.Index(0) == aRunArray.Index(1));
	aBuf.Zero();
	TInt index;
	for (TInt i = 1; 0 <= (index = aRunArray.Index(i)); ++i)
		{
		aBuf.Append('0' + index);
		}
	}

/**
@SYMTestCaseID          SYSLIB-FORM-UT-1886
@SYMTestCaseDesc        Testing the API's of CTmTextImp: ChangeFormatL, MemoryUsed, SystemColor, CustomizeL
@SYMTestPriority        Low
@SYMTestActions         Tests by changing the format, checking the memory usage, setting the system colour and Changing the custom formatting
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void CTagmaImpTest::TestAPIL()
	{
	TTmFormatParam formatParam;
	CTmTextImp* text = new(ELeave)
		CTmTextImp(*iDevice, formatParam);
	CleanupStack::PushL(text);
	TTmCharFormat cf1;
	cf1.iEffects |= TTmCharFormat::EUnderline;
	RTmParFormat pf1;
	CleanupClosePushL(pf1);
	pf1.iSpaceAbove = 1;
	text->InsertL(0, _L("Some text"), &cf1, &pf1, 0, 0);

	TInt memUsed = text->MemoryUsed();
	text->ChangeFormatL(formatParam);
	TESTPOINT(text->MemoryUsed()==memUsed);

	TTmFormatParam formatParam1;
	formatParam1.iStartChar = 0;
	formatParam1.iEndChar = 15;
	formatParam1.iLineInPar = 0;
	formatParam1.iWrapWidth = 50;
	formatParam1.iMaxHeight = KMaxTInt;
	formatParam1.iMaxLines = KMaxTInt;
	formatParam1.iFlags = TTmFormatParam::EWrap;
	//Setting the format of the text
	text->ChangeFormatL(formatParam1);
	//After changing the format, memory used by the text differs from the initial value
	TESTPOINT(text->MemoryUsed()!=memUsed);
	TRgb color1(100,10,20);
	TRgb color2;
	//Setting the system colour with an index representing the system background colour
	color2 = text->SystemColor(TLogicalRgb::ESystemBackgroundIndex,color1);
	TESTPOINT(color2==color1);

	_LIT(KBody, "Body");
	_LIT(KLabel, "Label, Longer than Body");
	TTestParLabelSource sourceLabel(iDevice, KBody, KLabel);
	CTmTextLayout* iLayout = new (ELeave) CTmTextLayout;
	iLayout->SetTextL(sourceLabel, formatParam);
	//Changing the custom formatting
	text->CustomizeL(iLayout->Source());

	//Setting the system colour with an index representing the system foreground colour
	color2 = text->SystemColor(TLogicalRgb::ESystemSelectionForegroundIndex,color1);
	TESTPOINT(color2!=color1);
	CleanupStack::PopAndDestroy(&pf1);
	CleanupStack::PopAndDestroy(text);
	}

void CTagmaImpTest::CTmTextImp_RRunArrayL()
	{
	TBuf8<50> indexBuf;
	CTmTextImp::RRunArray ra;
	CleanupClosePushL(ra);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("")));
	ra.Insert(0, 10, 0);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("0000000000")));
	ra.Insert(5, 10, 1);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("00000111111111100000")));
	ra.Delete(13, 4);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("0000011111111000")));
	ra.Delete(2, 12);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("0000")));
	ra.Delete(0, 4);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("")));
	ra.Insert(0, 20, 0);
	ra.Set(5, 10, 1);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("00000111111111100000")));
	ra.Set(6, 4, 2);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("00000122221111100000")));
	ra.Set(10, 4, 3);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("00000122223333100000")));
	ra.Set(9, 2, 1);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("00000122211333100000")));
	ra.Set(6, 1, 1);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("00000112211333100000")));
	ra.Set(8, 1, 1);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("00000112111333100000")));
	ra.Set(7, 1, 1);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("00000111111333100000")));
	ra.Set(7, 2, 2);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("00000112211333100000")));
	ra.Set(1, 19, 1);
	GetIndices(indexBuf, ra);
	TESTPOINT(0 == indexBuf.Compare(_L8("01111111111111111111")));
	CleanupStack::PopAndDestroy(&ra);
	}


void CTagmaImpTest::CTmTextImp_BidirectionalAlignmentL()
	{
	ExerciseCopyL();
	ExerciseGetCParaFormatL();
	}

void CTagmaImpTest::ExerciseCopyL()
	{
	CParaFormat* pF = CParaFormat::NewL();
	CleanupStack::PushL(pF);
	RTmParFormat rPF;

	TESTPOINT(RTmParFormat::EAlignNormalBidirectional == rPF.iAlignment);

	rPF.CopyL(*pF);
	TESTPOINT(RTmParFormat::EAlignNormal == rPF.iAlignment);

	pF->iHorizontalAlignment = CParaFormat::ELeftAlign;
	rPF.CopyL(*pF);
	TESTPOINT(RTmParFormat::EAlignNormal == rPF.iAlignment);

	pF->iHorizontalAlignment = CParaFormat::ECenterAlign;
	rPF.CopyL(*pF);
	TESTPOINT(RTmParFormat::EAlignCenter == rPF.iAlignment);

	pF->iHorizontalAlignment = CParaFormat::ERightAlign;
	rPF.CopyL(*pF);
	TESTPOINT(RTmParFormat::EAlignReverse == rPF.iAlignment);

	pF->iHorizontalAlignment = CParaFormat::EJustifiedAlign;
	rPF.CopyL(*pF);
	TESTPOINT(RTmParFormat::EAlignJustify == rPF.iAlignment);

	pF->iHorizontalAlignment = CParaFormat::EAbsoluteLeftAlign;
	rPF.CopyL(*pF);
	TESTPOINT(RTmParFormat::EAlignAbsoluteLeft == rPF.iAlignment);

	pF->iHorizontalAlignment = CParaFormat::EAbsoluteRightAlign;
	rPF.CopyL(*pF);
	TESTPOINT(RTmParFormat::EAlignAbsoluteRight == rPF.iAlignment);

	rPF.Close();
	CleanupStack::PopAndDestroy(); //pF
	}

void CTagmaImpTest::ExerciseGetCParaFormatL()
	{
	CParaFormat* pF = CParaFormat::NewL();
	CleanupStack::PushL(pF);
	RTmParFormat rPF;

	TESTPOINT(pF->iHorizontalAlignment == CParaFormat::ELeftAlign);

	rPF.iAlignment = RTmParFormat::EAlignNormal;
	rPF.GetCParaFormatL(*pF);
	TESTPOINT(CParaFormat::ELeftAlign == pF->iHorizontalAlignment);

	rPF.iAlignment = RTmParFormat::EAlignCenter;
	rPF.GetCParaFormatL(*pF);
	TESTPOINT(CParaFormat::ECenterAlign == pF->iHorizontalAlignment);

	rPF.iAlignment = RTmParFormat::EAlignReverse;
	rPF.GetCParaFormatL(*pF);
	TESTPOINT(CParaFormat::ERightAlign == pF->iHorizontalAlignment);

	rPF.iAlignment = RTmParFormat::EAlignJustify;
	rPF.GetCParaFormatL(*pF);
	TESTPOINT(CParaFormat::EJustifiedAlign == pF->iHorizontalAlignment);

	rPF.iAlignment = RTmParFormat::EAlignAbsoluteLeft;
	rPF.GetCParaFormatL(*pF);
	TESTPOINT(CParaFormat::EAbsoluteLeftAlign == pF->iHorizontalAlignment);

	rPF.iAlignment = RTmParFormat::EAlignAbsoluteRight;
	rPF.GetCParaFormatL(*pF);
	TESTPOINT(CParaFormat::EAbsoluteRightAlign == pF->iHorizontalAlignment);

	rPF.Close();
	CleanupStack::PopAndDestroy(); // pF
	}

class TTestCustomFormattingSource : public MTmSource, public MTmInlineTextSource
	{
	MGraphicsDeviceMap* iDevice;
	TInt iCustomFormatType;
	TPtrC iBody;
	TPtrC* iCurrent;
	TPtrC iNullText;
public:
	TTestCustomFormattingSource(MGraphicsDeviceMap* aDevice,
		const TDesC& aBody, TInt aCustomFormatType)
		: iDevice(aDevice)
        {
		iCustomFormatType = aCustomFormatType;
		iBody.Set(aBody.Ptr(), aBody.Length());
		iCurrent = &iBody;
		}
	void Close()
		{
		}
	TInt DocumentLength() const { return iCurrent->Length(); }
	MGraphicsDeviceMap& FormatDevice() const
		{
		return *iDevice;
		}
	TAny* GetExtendedInterface(const TUid& aInterfaceId)
		{
		if (aInterfaceId == KInlineTextApiExtensionUid)
			{
			return static_cast<MTmInlineTextSource*>(this);
			}
		else
			{
			// In this instance, calling the parent class will always return NULL
			// but the pattern should be followed by all implementors for safety
			return MTmSource::GetExtendedInterface(aInterfaceId);
			}
		}
	void GetParagraphFormatL(TInt aPos, RTmParFormat& aFormat) const
		{
		__ASSERT_ALWAYS(0 <= aPos, User::Invariant());
		__ASSERT_ALWAYS(aPos <= iCurrent->Length(), User::Invariant());
		const RTmParFormat f;
		aFormat.CopyL(f);
		}
	void GetText(TInt aPos,TPtrC& aText,TTmCharFormat& aFormat) const
		{
		ASSERT(0 <= aPos);
		ASSERT(aPos <= iCurrent->Length());
		aText.Set(iCurrent->Ptr() + aPos, iCurrent->Length() - aPos);
		TTmCharFormat f;
		aFormat = f;
		}
	MGraphicsDeviceMap& InterpretDevice() const
		{
		return *iDevice;
		}
	TInt ParagraphStart(TInt) const { return 0; }
	TRgb SystemColor(TUint aColorIndex,TRgb aDefaultColor) const
		{
		if (aColorIndex == TLogicalRgb::ESystemSelectionForegroundIndex)
			return KRgbWhite;
		if (aColorIndex == TLogicalRgb::ESystemSelectionBackgroundIndex)
			return KRgbRed;
		return MTmSource::SystemColor(aColorIndex, aDefaultColor);
		}
	TInt GetNextInlineTextPosition(const TTmDocPos& aFrom, TInt aMaxLength, TTmDocPos& aNext)
		{
		if (iCustomFormatType == 0)
			{
			return KErrNotFound;
			}
		else if (iCustomFormatType == 1)
			{
			_LIT(KCFS1, "EPOC");
			TInt from = aFrom.iPos;
			if (from < 4)
				from = 0;
			else
				{ // let's move back to look for the target string
				from -= (aFrom.iLeadingEdge ? 3 : 4); // adjustment takes care of not returning trailing edge if at start pos
				}
			TInt pos = iBody.Mid(from).Find(KCFS1);
			if (pos != KErrNotFound)
				{
				pos += from;
				aNext.iPos = pos + 4; // Adjust by length of string "EPOC"
				aNext.iLeadingEdge = EFalse;
				if (aNext.iPos - aFrom.iPos < aMaxLength + (aNext.iLeadingEdge ? 0 : 1))
					return KErrNotFound;
				TBuf<256> buf;
				buf.AppendFormat(_L("GetPos-InlineTextAt-%d %c-From-%d\n"),aNext.iPos, aNext.iLeadingEdge ? 'L' : 'T', aFrom.iPos);
				TESTPRINT(buf);
				return KErrNone;
				}
			return KErrNotFound;
			}
		else if (iCustomFormatType == 2)
			{
			_LIT(KCFS2, "(ABCDEFGHI)");
			TInt from = aFrom.iPos;
			if (from < 7)
				from = 0;
			else
				{ // let's move back to look for the target string
				from -= (aFrom.iLeadingEdge ? 6 : 7); // adjustment takes care of not returning trailing edge if at start pos
				}
			TInt pos = iBody.Mid(from).Find(KCFS2);
			if (pos != KErrNotFound)
				{
				pos += from;
				if (pos + 4 >= aFrom.iPos)
					{ // search position is before the first break space
					aNext.iPos = pos + 4; // adjust to position of first space
					aNext.iLeadingEdge = ETrue;
					if (aNext.iPos - aFrom.iPos < aMaxLength + (aNext.iLeadingEdge ? 0 : 1))
						return KErrNotFound;
					TBuf<256> buf;
					buf.AppendFormat(_L("GetPos-InlineTextAt-%d %c-From-%d\n"), aNext.iPos, aNext.iLeadingEdge ? 'L' : 'T', aFrom.iPos);
					TESTPRINT(buf);
					return KErrNone;
					}
				// if it gets here search position is after the first break space
				if (pos + 7 >= aFrom.iPos)
					{ // but before the second
					aNext.iPos = pos + 7; // adjust to position of second space
					aNext.iLeadingEdge = EFalse;
					if (aNext.iPos - aFrom.iPos < aMaxLength + (aNext.iLeadingEdge ? 0 : 1))
						return KErrNotFound;
					TBuf<256> buf;
					buf.AppendFormat(_L("GetPos-InlineTextAt-%d %c-From-%d\n"), aNext.iPos, aNext.iLeadingEdge ? 'L' : 'T', aFrom.iPos);
					TESTPRINT(buf);
					return KErrNone;
					}
				}
			return KErrNotFound;
			}
		return KErrNotFound;
		}
	TPtrC GetInlineText(const TTmDocPos& aAt)
		{
	    TBuf<256> buf;
	    buf.AppendFormat(_L("QueryTextAt-%d %c\n"), aAt.iPos, aAt.iLeadingEdge ? 'L' : 'T');
	    TESTPRINT(buf);
		if (iCustomFormatType == 0)
			{
			return iNullText;
			}
		else if (iCustomFormatType == 1)
			{
			_LIT(KCFS1, "EPOC");
			TInt at = aAt.iPos;
			if (at < 4)
				at = 0;
			else
				at -= 4; // let's move back to look for the target string
			TInt pos = iBody.Mid(at).Find(KCFS1);
			if (pos != KErrNotFound)
				{
				pos += at;
				if ((pos + 4 == aAt.iPos) && !aAt.iLeadingEdge)
					{
					_LIT(KCFS3, "?");
					TPtrC tPtrC(KCFS3);
					TBuf<256> buf;
					buf.AppendFormat(_L("GetText-QuestionMark-%d\n"), aAt.iPos);
					TESTPRINT(buf);
					return tPtrC;
					}
				}
			return iNullText;
			}
		else if (iCustomFormatType == 2)
			{
			_LIT(KCFS2, "(ABCDEFGHI)");
			TInt at = aAt.iPos;
			if (at < 7)
				at = 0;
			else
				at -= 7; // let's move back to look for the target string
			TInt pos = iBody.Mid(at).Find(KCFS2);
			if (pos != KErrNotFound)
				{
				pos += at;
				if ((pos + 4 == aAt.iPos) && aAt.iLeadingEdge)
//				if ((pos + 4 == aAt.iPos) && !aAt.iLeadingEdge)
					{ // search position is on the first break space
					_LIT(KCFS4, " ");
					TPtrC tPtrC(KCFS4);
					
                    TBuf<256> buf;
                    buf.AppendFormat(_L("GetText-Space-%d-L\n"), aAt.iPos);
                    TESTPRINT(buf);
					return tPtrC;
					}
				// if it gets here search position is not on the first break space
				if ((pos + 7 == aAt.iPos) && !aAt.iLeadingEdge)
					{ // but it is on the second
					_LIT(KCFS4, " ");
					TPtrC tPtrC(KCFS4);
					TBuf<256> buf;
					buf.AppendFormat(_L("GetText-Space-%d-T\n"), aAt.iPos);
					TESTPRINT(buf);
					return tPtrC;
					}
				}
			return iNullText;
			}
		return iNullText;
		}
	};

void CTagmaImpTest::CustomFormattingL()
	{
	// Test implementation of CustomFormatting
	// ========
	// Test one
	// ========
	// First test is where inline text is enabled but where there is none to insert
	_LIT(KBody0, "This is a bunch of boring plain text that doesn't get any inline text inserted.\x2029"); 
	TTestCustomFormattingSource s0(iDevice, KBody0, 0);	// No inline text
	CTmTextLayout* lay0 = new(ELeave) CTmTextLayout;
	TTmFormatParam fp0;
	fp0.iWrapWidth = 200;
	fp0.iEndChar = KBody0().Length();
	lay0->SetTextL(s0, fp0);
	s0.Close();
	// ========
	// Test two
	// ========
	// Second test is where a single inline question mark will be inserted after the word EPOC
	_LIT(KBody1, "With luck the word EPOC will be followed by a question mark\x2029");
	TTestCustomFormattingSource s1(iDevice, KBody1, 1);	// No inline text
	CTmTextLayout* lay1 = new(ELeave) CTmTextLayout;
	TTmFormatParam fp1;
	fp1.iWrapWidth = 200;
	fp1.iEndChar = KBody1().Length();
	lay1->SetTextL(s1, fp1);
	s1.Close();
	// ==========
	// Test three
	// ==========
	// Third test looks for a pair of brackets separated by 9 chars and inserts spaces to
	// split them into three blocks of three chars
	_LIT(KBody2, "Take 9 chars inside brackets (ABCDEFGHI) and split by spaces into 3 x 3.\x2029");
	TTestCustomFormattingSource s2(iDevice, KBody2, 2);	// No inline text
	CTmTextLayout* lay2 = new(ELeave) CTmTextLayout;
	TTmFormatParam fp2;
	fp2.iWrapWidth = 200;
	fp2.iEndChar = KBody2().Length();
	lay2->SetTextL(s2, fp2);
	s2.Close();
	}


/**
@SYMTestCaseID 			SYSLIB-FORM-CT-3353
@SYMTestCaseDesc		Test Handling of ZWJ characters in latin text
@SYMTestPriority 		High
@SYMTestActions			Format some Latin strings with ZWJ characters and
						verify the formatted string and length of text.
@SYMTestExpectedResults	The formatted string should exclude the ZWJ character
						and the length of the string should equal the
						number of characters in the original string + 1.
						for the terminator.
						The formatting of the string must not cause a panic.
@SYMDEF PDEF101464
 */
void CTagmaImpTest::PDEF_101464_DefectL()
{
	//Latin ZWJ example strings
	_LIT(KLatinZWJ1,"\x200D\x0061\x0062\x0063");
	_LIT(KLatinZWJ2,"\x0061\x0062\x200D\x0063");
	_LIT(KLatinZWJ3,"\x0061\x0062\x0063\x200D");

	//Expected displayed result from Latin strings - ZJW has no effect
	_LIT(KDisplayedLatinZWJ,"\x0061\x0062\x0063");
	//Expected length is four characters plus terminator
	TInt KExpectedTextLength = 5;

	CTmText *text = CTmText::NewL(*iDevice,KMaxTInt,FALSE);
	CleanupStack::PushL(text);

	TBuf<32> buffer;
	TInt needed;
	TInt textLength;

	//Test the sample text with ZWJ at the beginning of the string
	text->InsertL(0,KLatinZWJ1);
	text->GetDisplayedText(0,buffer,needed);

	//Verify that the text is formatted correctly
	TESTPOINT(buffer == KDisplayedLatinZWJ);

	//Verify that the length of the string is as expected
	textLength = (text->EndChar()) - (text->StartChar());
	TESTPOINT(textLength == KExpectedTextLength);
	text->Clear();

	//Test the sample text with ZWJ in the middle of the string
	text->InsertL(0,KLatinZWJ2);
	text->GetDisplayedText(0,buffer,needed);

	//Verify that the text is formatted correctly
	TESTPOINT(buffer == KDisplayedLatinZWJ);

	//Verify that the length of the string is as expected
	textLength = (text->EndChar()) - (text->StartChar());
	TESTPOINT(textLength == KExpectedTextLength);
	text->Clear();

	//Test the sample text with ZWJ at the end of the string
	text->InsertL(0,KLatinZWJ3);
	text->GetDisplayedText(0,buffer,needed);

	//Verify that the text is formatted correctly
	TESTPOINT(buffer == KDisplayedLatinZWJ);

	//Verify that the length of the string is as expected
	textLength = (text->EndChar()) - (text->StartChar());
	TESTPOINT(textLength == KExpectedTextLength);
	text->Clear();

	CleanupStack::PopAndDestroy(); // text
}



/**
@SYMTestCaseID 			SYSLIB-FORM-CT-3398
@SYMTestCaseDesc		Test Mapping Of Post Unicode 2.0 Characters
@SYMTestPriority 		High
@SYMTestActions			Format some Unicdoe 5.0 characters and test if they have
						been formatted
@SYMTestExpectedResults	The formatted string should include the Unicode 5.0 characters
@SYMDEF DEF101994
 */
void CTagmaImpTest::DEF101994_DefectL()
{
	// Kannada and Tamil characters
	_LIT(KKannadaTamil,"\x0CBC\x0CBD\x0BB6\x0BE6\x0BF3\x0BF4\x0BF5\x0BF6\x0BF7\x0BF8\x0BF9\x0BFA");
	//Expected displayed result - all characters should be mapped and displayed
	_LIT(KDisplayedKannadaTamil,"\x0CBC\x0CBD\x0BB6\x0BE6\x0BF3\x0BF4\x0BF5\x0BF6\x0BF7\x0BF8\x0BF9\x0BFA");
	//Expected length is twelve characters plus terminator
	TInt KExpectedTextLength = 13;

	CTmText *text = CTmText::NewL(*iDevice,KMaxTInt,FALSE);
	CleanupStack::PushL(text);

	TBuf<32> buffer;
	TInt needed;
	TInt textLength;

	//Test the sample text the Kananda and Tamil characters
	text->InsertL(0,KKannadaTamil);
	text->GetDisplayedText(0,buffer,needed);

	//Verify that the text is formatted correctly
	TESTPOINT(buffer == KDisplayedKannadaTamil);

	//Verify that the length of the string is as expected
	textLength = (text->EndChar()) - (text->StartChar());
	TESTPOINT(textLength == KExpectedTextLength);
	text->Clear();

	CleanupStack::PopAndDestroy(); // text
}

	/**
	@SYMTestCaseID 				SYSLIB-FORM-UT-4010
	@SYMTestCaseDesc 			Automated Form testing for Drawing Text with Context
	@SYMTestPriority 			High
	@SYMTestActions  			Format text and check the line context stored in chunks.
	@SYMTestExpectedResults 	Each chunk should contain the correct context based on the script used in the source text.
	@SYMPREQ 					1766: Support Bengali, Tamil and Telugu Scripts
	@SYMREQ 					854: Text I18N Drawing Latin Punctuation along the Indic text baseline
	*/
	void CTagmaImpTest::TestChunkContext()
		{
		TTmChunk chunk;
		TTmChunk::TInfo chunkInfo;
		
		// create a CTestSource containing the test data
		_LIT(KTest, "\x0915\x094d\x0915...abc.");
		TPtrC testSeg(KTest().Ptr(), KTest().Length() );
		CTestSource* source = NewTestSourceLC();
		source->SetText(testSeg);
		
		CTmCode* code = new(ELeave) CTmCode;
		CleanupStack::PushL(code);
		
		// format the text
		TTmFormatParam formatParam;
		formatParam.iStartChar = 0;
		formatParam.iEndChar = source->DocumentLength();
		formatParam.iLineInPar = 0;
		formatParam.iMaxHeight = KMaxTInt;
		formatParam.iMaxLines = KMaxTInt;
		formatParam.iFlags = TTmFormatParam::EWrap;
		formatParam.iWrapWidth = 533;
		CTmTextLayout* layout = new(ELeave) CTmTextLayout;
		CleanupStack::PushL(layout);
		
		CTmFormatContext* formatContext = new CTmFormatContext(*source, formatParam, *code, layout);
		CleanupStack::PushL(formatContext);
		
		chunk.SetL(*formatContext,0,0,10,533,0,chunkInfo);
		TUint context = chunk.iContextCharInByteCode;
		// Test that the first chunk's context has been recognised as not needing supplied context.
		TESTPOINT(0 == context);
		chunk.SetL(*formatContext,3,0,10,533,0,chunkInfo);
		context = chunk.iContextCharInByteCode;
		// Test that the next chunk retains the Hindi context.
		TESTPOINT(2325 == context);
		chunk.SetL(*formatContext,4,0,10,533,0,chunkInfo);
		context = chunk.iContextCharInByteCode;
		// Test that the next chunk retains the Hindi context.
		TESTPOINT(2325 == context);
		chunk.SetL(*formatContext,5,0,10,533,0,chunkInfo);
		context = chunk.iContextCharInByteCode;
		// Test that the next chunk retains the Hindi context.
		TESTPOINT(2325 == context);
		chunk.SetL(*formatContext,6,0,10,533,0,chunkInfo);
		context = chunk.iContextCharInByteCode;
		// Test that the next chunk has been recognised as not needing supplied context.
		TESTPOINT(0 == context);
		chunk.SetL(*formatContext,9,0,10,533,0,chunkInfo);
		context = chunk.iContextCharInByteCode;
		// Test that the next chunk retains the Latin context.
		TESTPOINT(99 == context);
		
		
		CleanupStack::PopAndDestroy(formatContext); // formatContext
		CleanupStack::PopAndDestroy(layout); // layout
		CleanupStack::PopAndDestroy(code);   // code
		CleanupStack::PopAndDestroy(source); // source
		}
	
	/**
	@SYMTestCaseID 				SYSLIB-FORM-UT-4011
	@SYMTestCaseDesc 			Automated Form testing for Drawing Text with Context
	@SYMTestPriority 			High
	@SYMTestActions  			Format text and check the line context from the generated bytecode.
	@SYMTestExpectedResults 	The bytecode should contain the correct context based on the script used in the source text.
	@SYMPREQ 					1766: Support Bengali, Tamil and Telugu Scripts
	@SYMREQ 					854: Text I18N Drawing Latin Punctuation along the Indic text baseline
	*/
	void CTagmaImpTest::TestBytecodeLineContext()
		{
		// create a CTestSource containing the test data
		_LIT(KTest, "abcdefghij..........\x0915\x0915\x0915\x0915\x0915\x0915\x0915\x0915\x0915\x0915..........");
		
		TPtrC testSeg(KTest().Ptr(), KTest().Length());
		CTestSource* source = NewTestSourceLC();
		source->SetText(testSeg);
		CTmCode* code = new(ELeave) CTmCode;
		CleanupStack::PushL(code);
		
		// Format the text.
		CTmFormatContext::TInfo info;
		TTmFormatParam formatParam;
		formatParam.iStartChar = 0;
		formatParam.iEndChar = source->DocumentLength();
		formatParam.iLineInPar = 0;
		formatParam.iMaxHeight = KMaxTInt;
		formatParam.iMaxLines = KMaxTInt;
		formatParam.iFlags = TTmFormatParam::EWrap;
		formatParam.iWrapWidth = 100;
		CTmTextLayout* layout = new(ELeave) CTmTextLayout;
		CleanupStack::PushL(layout);
		CTmFormatContext::FormatL(*source, formatParam, *code, info, layout);
		
		// Set up the interpreter.
		TTmInterpreterParam interpreter_param(*code);
		interpreter_param.iCodeStart = 0;
		interpreter_param.iCodeEnd = code->Size();
		interpreter_param.iTextStart = 0;
		interpreter_param.iWidth = 50;
		RTmGeneralInterpreter interpreter(*source, interpreter_param);
		
		// Expected context per line.
		TInt expectedResults[] = {106, 106, 2325, 2325};
		TInt lines = 0;
		
		// Iterate through lines in bytecode and check the context.
		while (interpreter.Next())
			{
			if (interpreter.Op() == TTmInterpreter::EOpLine)
				{
				TESTPOINT(interpreter.LineContextCharChar() == expectedResults[lines++]);
				}
			interpreter.Skip();
			}
		
		// Test the correct amount of lines have been tested.
		TESTPOINT(4 == lines);
		
		// Close the interpreter and clean up the heap.
		interpreter.Close();
		CleanupStack::PopAndDestroy(layout); // layout
		CleanupStack::PopAndDestroy(code);   // code
		CleanupStack::PopAndDestroy(source); // source
		}
		
	
	/**
	@SYMTestCaseID 				SYSLIB-FORM-UT-4012
	@SYMTestCaseDesc 			Automated Form testing for Drawing Text with Context
	@SYMTestPriority 			High
	@SYMTestActions  			Format text and check the chunk context from the generated bytecode.
	@SYMTestExpectedResults 	Chunks within the bytecode should contain the correct context based on the script used in the source text.
	@SYMPREQ 					1766: Support Bengali, Tamil and Telugu Scripts
	@SYMREQ 					854: Text I18N Drawing Latin Punctuation along the Indic text baseline
	*/
	void CTagmaImpTest::TestBytecodeChunkContext()
		{
		// create a CTestSource containing the test data.
		_LIT(KTest, "a. b\x915.c. \x916.d \x917. chunk");
		TPtrC testSeg(KTest().Ptr(), KTest().Length());
		CTestSource* source = NewTestSourceLC();
		source->SetText(testSeg);
		
		// Create a bytecode object.
		CTmCode* code = new(ELeave) CTmCode;
		CleanupStack::PushL(code);
		
		// Format the text.
		CTmFormatContext::TInfo info;
		TTmFormatParam formatParam;
		formatParam.iStartChar = 0;
		formatParam.iEndChar = source->DocumentLength();
		formatParam.iLineInPar = 0;
		formatParam.iMaxHeight = KMaxTInt;
		formatParam.iMaxLines = KMaxTInt;
		formatParam.iFlags = TTmFormatParam::EWrap;
		formatParam.iWrapWidth = KMaxTInt;
		CTmTextLayout* layout = new(ELeave) CTmTextLayout;
		CleanupStack::PushL(layout);
		CTmFormatContext::FormatL(*source, formatParam, *code, info, layout);
		
		// Set up the interpreter.
		TTmInterpreterParam interpreter_param(*code);
		interpreter_param.iCodeStart = 0;
		interpreter_param.iCodeEnd = code->Size();
		interpreter_param.iTextStart = 0;
		interpreter_param.iWidth = KMaxTInt;
		RTmGeneralInterpreter interpreter(*source, interpreter_param);		
		
		// Expected context per chunk.
		TInt expectedResults[] = {0, 2325, 0};
		TInt chunks = 0;

		// Make sure we're starting at the begining.
		TBool found = interpreter.LineNumberToLine(0);
		
		// Iterate through the chunks, testing context.
		while (interpreter.Next() && interpreter.Op() != TTmInterpreter::EOpLine)
			{
			TUint32 op = interpreter.Op();
			if (interpreter.Op() == TTmInterpreter::EOpText || interpreter.Op() == TTmInterpreter::EOpSpecialChar)
				{
				TESTPOINT(expectedResults[chunks++] == interpreter.ContextCharChar());
				}
			else
				{
				interpreter.Skip();
				}
			}
		
		// Test that the expected number of chunks were found in the bytecode.
		TESTPOINT(3 == chunks);
		
		// Close the interpreter and clean up the heap.
		interpreter.Close();
		CleanupStack::PopAndDestroy(layout); // layout
		CleanupStack::PopAndDestroy(code);   // code
		CleanupStack::PopAndDestroy(source); // source
		}

void CTagmaImpTest::TestL()
	{
//	__UHEAP_MARK;
    
	TESTPRINT(_L("Regression test: DEF073838")); // Line break problem with WORD
	DEF_073838_DefectL();

	TESTPRINT(_L("Regression test: INC044969"));
	INC_044969_DefectL();

	TESTPRINT(_L("CTmTextImp::RRunArray tests"));
	CTmTextImp_RRunArrayL();

	TESTPRINT(_L("Regression test: BUR-58FGE8"));
	BUR_58FGE8_DefectL();

	TESTPRINT(_L("Regression test: WEP-55BHBF"));
	WEP_55BHBF_DefectL();

	TESTPRINT(_L("Regression test: EXT-5ATF8D"));
	EXT_5ATF8D_DefectL();

	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-1886 Test for CTmTextImp API's "));
	TestAPIL();

	TESTPRINT(_L("CTmCode tests"));
	CTmCodeOOML();

	TESTPRINT(_L("CTmFormatContext::FormatL tests"));
	FormatOOML();

	TESTPRINT(_L("RTmTextCache::Width tests"));
	RTmTextCacheWidthOOM();

	TESTPRINT(_L("RTmGeneralInterpreter::GetDisplayedText tests"));
	RTmGeneralInterpreterGetDisplayedTextOOM();

	TESTPRINT(_L("Bidirectional alignment tests"));
	CTmTextImp_BidirectionalAlignmentL();

	TESTPRINT(_L("Custom formatting tests"));
	CustomFormattingL();

	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-3353 Regression test: PDEF101464 "));
	PDEF_101464_DefectL();

	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-CT-3398 Regression test: DEF101994 "));
	DEF101994_DefectL();
	
	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-4010 Drawing Text within context: Testing chunk context."));
	TestChunkContext();
	
	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-4011 Drawing Text within context: Testing line context within bytecode."));
	TestBytecodeLineContext();
	
	TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORM-UT-4012 Drawing Text within context: Testing chunk context within bytecode."));
	TestBytecodeChunkContext();	
	
//	__UHEAP_MARKEND;
	}

CTTagmaImpStep::CTTagmaImpStep()
    {
    
    }

TVerdict CTTagmaImpStep::doTestStepL()
    {    
    SetTestStepResult(EPass);
    TestStep = this;
    
    TESTPRINT(_L("Tagma internals tests"));
    CTagmaImpTest* t = new(ELeave) CTagmaImpTest(this);
    CleanupStack::PushL(t);
    t->ConstructL();
    t->TestL();
    CleanupStack::PopAndDestroy(t);
    
    return TestStepResult();
    }


