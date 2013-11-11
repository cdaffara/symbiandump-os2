/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TCustomWrap.h"
#include "TGraphicsContext.h"
#include <e32std.h>
#include <e32test.h>
#include <frmtlay.h>
#include <frmtview.h>
#include <txtlaydc.h>
#include <fbs.h>
#include <w32std.h>
#include <txtrich.h>
#include <e32math.h>
#include <f32file.h>
#include <flogger.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

#include "tformbenchmark.h"

namespace LocalToTFormBenchmark
{
CTFormBenchmarkStep* TestStep = NULL;
#define TESTPOINT(p) TestStep->testpoint(p,(TText8*)__FILE__,__LINE__)
#define TESTPRINT(p) TestStep->print(p,(TText8*)__FILE__,__LINE__)
}
using namespace LocalToTFormBenchmark;

RFs fs;
RFile fileTimeStamps;

_LIT(KFormBenchmark, "TFormBenchmark");

#if ((defined (__WINS__)) || (defined (__X86GCC__)))
	_LIT(KLogTimeStampsName, "c:\\formtimestamps.csv");
#else
	_LIT(KLogTimeStampsName, "e:\\formtimestamps.csv");
#endif

_LIT(KEnglish, "Whereas recognition of the inherent dignity and of the equal \
and inalienable rights of all members of the human family is the foundation \
of freedom, justice and peace in the world, ");
const TInt KEnglishRepeats = 119;
_LIT(KArabic, "\x644\x645\x651\x627 \x643\x627\x646 \x627\x644\x627\x639\x62A\
\x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
\x645. ");
const TInt KArabicRepeats = 156;
_LIT(KJapanese, "\x4EBA\x985E\x793E\x4F1A\x306E\x3059\x3079\x3066\x306E\x69CB\
\x6210\x54E1\x306E\x56FA\x6709\x306E\x5C0A\x53B3\x3068\x5E73\x7B49\x3067\x8B72\
\x308B\x3053\x3068\x306E\x3067\x304D\x306A\x3044\x6A29\x5229\x3068\x3092\
\x627F\x8A8D\x3059\x308B\x3053\x3068\x306F\x3001\x4E16\x754C\x306B\x304A\
\x3051\x308B\x81EA\x7531\x3001\x6B63\x7FA9\x53CA\x3073\x5E73\x548C\x306E\
\x57FA\x790E\x3067\x3042\x308B\x306E\x3067\x3001 ");
const TInt KJapaneseRepeats = 357;
const TInt KDisplayWidth = 100;
const TInt KDisplayHeight = 120;

const TInt KInsertDisplayWidth = 200;
const TInt KChangedDisplayWidth = 150;

_LIT(KEnglishToInsert1, "Whereas recognition of the inherent dignity");
_LIT(KEnglishToInsert2, " and of the equal and inalienable rights of all... and so on");

const TInt KEnglishInsertRepeats = 400;
const TInt KEnglishScrollRepeats = 100;

const TInt KDeleteTextInterval = 10;//we measure delete text once for each 10 deletes, to make the output smaller

_LIT8(KDeleteTextFromStart, "Delete text from start\r\n");
_LIT8(KInsertTextAtStart, "Insert text at start\r\n");
_LIT8(KScrollingUp, "Scrolling up\r\n");
_LIT8(KScrollingDown, "Scrolling down\r\n");

struct TDocInfo
	{
	TDocInfo():iDocLength(0), iDocParaCount(0), iLineCount(0){}
	TInt iDocLength;
	TInt iDocParaCount;
	TInt iLineCount;
	};

struct TTFTimeStamp
	{
	TInt64 iTime;
	TInt iIteration;
	};

struct TFormattingObjects
	{
	TRect* iDisplayRect;
	CParaFormatLayer* iParFormat;
	CCharFormatLayer* iCharFormat;
	CRichText* iRichText;
	CTextLayout* iLayout;
	CTestGraphicsDevice* iDevice;
	CTextView* iView;
	};

/*
Functions for writing benchmarking data to the CSV
*/
void WriteSampleDocInfo(TDocInfo aDocInfo, RFile aDest)
	{
	TBuf8<128> buf;
	buf.Format(_L8("Sample text: %d characters: %d paras: %d lines\r\n"), aDocInfo.iDocLength, aDocInfo.iDocParaCount, aDocInfo.iLineCount);
	aDest.Write(buf);
	}

void WriteTimeStampsL(const TDesC8& aTitle, RArray<TTFTimeStamp>& aTimeStamps)
	{
	fileTimeStamps.Write(aTitle);
	for (TInt i = 0; i < aTimeStamps.Count(); i++)
		{
		TTFTimeStamp timestamp = aTimeStamps[i];
		TBuf8<128> buf;
		buf.Format(_L8("%d, %Ld\r\n"), timestamp.iIteration, timestamp.iTime/1000);
		fileTimeStamps.Write(buf);
		}
	}

TDocInfo GetSampleDocInfoL(TFormattingObjects& aFormattingObjects)
	{
	TDocInfo docInfo;
	const CTmTextLayout& layout = aFormattingObjects.iView->Layout()->TagmaTextLayout();
	docInfo.iDocLength = layout.Source()->DocumentLength();
	docInfo.iDocParaCount = aFormattingObjects.iRichText->ParagraphCount();
//very peculiar way to count the lines...
	TCursorPosition::TMovementType move = TCursorPosition::EFLineDown;
	while (move == TCursorPosition::EFLineDown)
		{
		aFormattingObjects.iView->MoveCursorL(move, EFalse);
		}
	move = TCursorPosition::EFLineUp;
	while (move == TCursorPosition::EFLineUp)
		{
		docInfo.iLineCount++;
		aFormattingObjects.iView->MoveCursorL(move, EFalse);
		}
	return docInfo;
	}

/*
Set up the test document
*/
void SetTextL(CRichText& aText, const TDesC& aSample, TInt aRepeats, TInt aRepeatsPerPara)
	{
	TInt repeatsInThisPar = 0;
	aText.Reset();
	for (; aRepeats != 0; --aRepeats)
		{
		aText.InsertL(0, aSample);
		repeatsInThisPar ++;
		if (repeatsInThisPar > aRepeatsPerPara)
			{
			TChar paragraphDelimiter(0x2029);
			aText.InsertL(0, paragraphDelimiter);
			repeatsInThisPar = 0;
			}
		}
	}

_LIT(KOneWord, "blahblahblahblahblahblahblahblah");

void BigBufLC(RBuf& aBigBuf)
	{
	aBigBuf.CreateL(16384);
	aBigBuf.CleanupClosePushL();
	while(aBigBuf.Length()+KOneWord().Length() < aBigBuf.MaxLength())
		{
		aBigBuf.Append(KOneWord);
		}
	}

TTimeIntervalMicroSeconds MeasureOpeningL(CTextView& aView)
	{
	TCursorSelection zero(0, 0);
	aView.SetPendingSelection(zero);
	TTime start;
	TTime end;
	start.UniversalTime();
	aView.HandleGlobalChangeL();
	end.UniversalTime();
	return end.MicroSecondsFrom(start);
	}

TTimeIntervalMicroSeconds MeasureRmsCursorDownL(CTextView& aView)
	{
	TInt mem;
	TInt cells = User::AllocSize(mem);
	TReal totalOfSquares = 0;
	TTime start;
	TTime end;
	TCursorPosition::TMovementType move = TCursorPosition::EFLineDown;
	while (move == TCursorPosition::EFLineDown)
		{
		start.UniversalTime();
		aView.MoveCursorL(move, EFalse);
		end.UniversalTime();
		TReal us = end.MicroSecondsFrom(start).Int64();
		totalOfSquares += us*us;
		}
	TReal rms;
	Math::Sqrt(rms, totalOfSquares);
	cells = User::AllocSize(mem);
	return TTimeIntervalMicroSeconds(static_cast<TInt64>(rms));
	}

TInt NumberOfLines(CTextLayout& aLayout)
	{
	TTmDocPosSpec dp(aLayout.DocumentLength(), TTmDocPosSpec::ETrailing);
	TTmPosInfo2 posInfo;
	TTmLineInfo lineInfo;
	aLayout.FindDocPos(dp, posInfo, &lineInfo);
	return lineInfo.iLineNumber;
	}

TTimeIntervalMicroSeconds MeasureFormattingL(CTextView& aView)
	{
	// This looks to me like a defect.
	// We have to set the actual doc pos to prevent a crash, even
	// though we are setting a pending selection.
	// Probably not too serious, though.
	aView.SetDocPosL(0, ETrue);
	TCursorSelection zero(0, 0);
	aView.SetPendingSelection(zero);
	TTime start;
	TTime end;
	start.UniversalTime();
	aView.HandleGlobalChangeL();
	aView.FinishBackgroundFormattingL();
	end.UniversalTime();
	end.UniversalTime();
	return end.MicroSecondsFrom(start);
	}

void MeasureDeleteTextFromStartL(RArray<TTFTimeStamp>& aTimeStamps,
	TFormattingObjects& aFormattingObjects, TInt64& aSlowest)
	{
	const CTmTextLayout& tmLayout = aFormattingObjects.iLayout->TagmaTextLayout();
	aTimeStamps.Close();
	TTime start;
	TTime end;
	TInt iteration = 0;
	TInt64 total = 0;

	TInt step = 0;
	while(aFormattingObjects.iRichText->DocumentLength() > 0)
		{
		aFormattingObjects.iRichText->DeleteL(0, 1);
		TCursorSelection cursor(0,0);
		start.UniversalTime();
		aFormattingObjects.iView->HandleInsertDeleteL(cursor, 1, EFalse);
		end.UniversalTime();

		TInt64 us = end.MicroSecondsFrom(start).Int64();

		if(us > aSlowest)
			{
			aSlowest = us;
			}

		total += us;
		step++;
		if(step == KDeleteTextInterval)
			{
			TInt64 average = total/KDeleteTextInterval;
			TTFTimeStamp timestamp;
			timestamp.iTime = average;
			timestamp.iIteration = iteration;
			User::LeaveIfError(aTimeStamps.Append(timestamp));
			iteration++;
			step = 0;
			total = 0;
			}
		}
	}

void MeasureInsertTextAtStartL(TInt aRepeats, TInt aRepeatsPerPara, RArray<TTFTimeStamp>& aTimeStamps,
	TFormattingObjects& aFormattingObjects, TInt64& aSlowest)
	{
	aTimeStamps.Close();
	TTime start;
	TTime end;
	TInt repeatsInThisPar = 0;
	TViewRectChanges changes;
	aFormattingObjects.iRichText->Reset();
	for (TInt i = 0; i < aRepeats; i++)
		{
		TBuf<128> sample;
		if ((i%2)==0)
			{
			sample = KEnglishToInsert1;
			}
		else
			{
			sample = KEnglishToInsert2;
			}
		aFormattingObjects.iRichText->InsertL(0, sample);
		TCursorSelection cursor(0, sample.Length());

		start.UniversalTime();
		aFormattingObjects.iLayout->HandleBlockChangeL(cursor, 0, changes, 0);
		end.UniversalTime();

		TInt64 us = end.MicroSecondsFrom(start).Int64();
		repeatsInThisPar ++;
		TTFTimeStamp timestamp;
		timestamp.iTime = us;
		timestamp.iIteration = i;
		User::LeaveIfError(aTimeStamps.Append(timestamp));
		if(us > aSlowest)
			{
			aSlowest = us;
			}
		if (repeatsInThisPar > aRepeatsPerPara)
			{
			TChar paragraphDelimiter(0x2029);
			aFormattingObjects.iRichText->InsertL(0, paragraphDelimiter);
			repeatsInThisPar = 0;
			}
		}
	}

void MeasureCursorL(RArray<TTFTimeStamp>& aTimeStamps, TCursorPosition::TMovementType aType,
	TFormattingObjects& aFormattingObjects, TInt64& aSlowest)
	{
	aTimeStamps.Close();
	TTime start;
	TTime end;
	TInt lines = 0;
	TCursorPosition::TMovementType move = aType;

	while (move == aType)
		{
		lines++;
		start.UniversalTime();
		aFormattingObjects.iView->MoveCursorL(move, EFalse);
		end.UniversalTime();
		TInt64 us = end.MicroSecondsFrom(start).Int64();
		TTFTimeStamp timestamp;
		timestamp.iTime = us;
		timestamp.iIteration = lines;
		User::LeaveIfError(aTimeStamps.Append(timestamp));
		if(us > aSlowest)
			{
			aSlowest = us;
			}
		const CTmTextLayout& tmLayout = aFormattingObjects.iLayout->TagmaTextLayout();
		}
	}

void CreateFormattingObjectsLC(TFormattingObjects& aFormattingObjects)
	{
	TRect* displayRect = new(ELeave) TRect(0, 0, KDisplayWidth, KDisplayHeight);
	CleanupStack::PushL(displayRect);
	CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormat);
	CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
	CleanupStack::PushL(charFormat);
	CRichText* text = CRichText::NewL(paraFormat, charFormat);
	CleanupStack::PushL(text);
	CTextLayout* layout = CTextLayout::NewL(text, displayRect->Width());
	CleanupStack::PushL(layout);
	CTestGraphicsDevice* device = CTestGraphicsDevice::NewL(displayRect->Size(), 0);
	CleanupStack::PushL(device);
	// Prevent the line array from using up the memory
	device->LineArray().Disable();
	CTextView* view = CTextView::NewL(layout, *displayRect,
		device, device, 0, 0, 0);
	CleanupStack::PushL(view);
	// must disable flicker-free redraw or Form will try to use
	// the test font with an off-screen (i.e. real) graphics context,
	// which will panic.
	view->DisableFlickerFreeRedraw();

	aFormattingObjects.iDisplayRect = displayRect;
	aFormattingObjects.iParFormat = paraFormat;
	aFormattingObjects.iCharFormat = charFormat;
	aFormattingObjects.iRichText = text;
	aFormattingObjects.iLayout = layout;
	aFormattingObjects.iDevice = device;
	aFormattingObjects.iView = view;
	}

void DestroyFormattingObjects(TFormattingObjects& aFormattingObjects)
	{
	CleanupStack::PopAndDestroy(aFormattingObjects.iView);
	CleanupStack::PopAndDestroy(aFormattingObjects.iDevice);
	CleanupStack::PopAndDestroy(aFormattingObjects.iLayout);
	CleanupStack::PopAndDestroy(aFormattingObjects.iRichText);
	CleanupStack::PopAndDestroy(aFormattingObjects.iCharFormat);
	CleanupStack::PopAndDestroy(aFormattingObjects.iParFormat);
	CleanupStack::PopAndDestroy(aFormattingObjects.iDisplayRect);
	}

/**
@SYMTestCaseID SYSLIB-FORMA-UT-1897
@SYMTestCaseDesc Benchmarks scrolling text
@SYMTestPriority High
@SYMTestActions creates document and scrolls it
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF092140, DEF092139
*/
void RunScrollingTestL(TInt aRepeats, TInt aRepeatsPerParagraph,
	RArray<TTFTimeStamp>& aTimeStampsDown, RArray<TTFTimeStamp>& aTimeStampsUp,
	TDocInfo& aDocInfo)
	{
    TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1897 "));
	TInt docLength = KEnglish().Length() * aRepeats;
	TInt paragraphs = aRepeats/aRepeatsPerParagraph;

	TFormattingObjects o;
	CreateFormattingObjectsLC(o);
	o.iLayout->SetWrapWidth(KInsertDisplayWidth);
	o.iLayout->SetAmountToFormat(CTextLayout::EFFormatBand);
	SetTextL(*(o.iRichText), KEnglish, aRepeats, aRepeatsPerParagraph);

	TTimeIntervalMicroSeconds opening = MeasureOpeningL(*(o.iView));
	TInt64 slowest = 0;
	MeasureCursorL(aTimeStampsDown, TCursorPosition::EFLineDown, o, slowest);
	MeasureCursorL(aTimeStampsUp, TCursorPosition::EFLineUp, o, slowest);

	TBuf<256> buf;
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_Scrolling_Slowest_Doc_with_%d_characters_%d_paragraphs;microseconds: %Ld"), docLength, paragraphs, slowest);
	TESTPRINT(buf);
	
	aDocInfo = GetSampleDocInfoL(o);
	DestroyFormattingObjects(o);
	}

/**
@SYMTestCaseID SYSLIB-FORMA-UT-1898
@SYMTestCaseDesc Benchmarks inserting, deleting and formatting text
@SYMTestPriority High
@SYMTestActions creates documents in each language and performs edits and reformats
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF092140, DEF092139
*/
void RunEnglishArabicJapaneseBenchmarksL(TFormattingObjects& aFormattingObjects)
	{
    TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1898 "));
	SetTextL(*aFormattingObjects.iRichText, KEnglish, KEnglishRepeats, KEnglishRepeats);
	TTimeIntervalMicroSeconds opening = MeasureOpeningL(*aFormattingObjects.iView);
	TTimeIntervalMicroSeconds rmsCursorDown = MeasureRmsCursorDownL(*aFormattingObjects.iView);
	TTimeIntervalMicroSeconds englishFormatting = MeasureFormattingL(*aFormattingObjects.iView);
	TInt englishLines = NumberOfLines(*aFormattingObjects.iLayout);
	SetTextL(*aFormattingObjects.iRichText, KArabic, KArabicRepeats, KArabicRepeats);
	TTimeIntervalMicroSeconds arabicFormatting = MeasureFormattingL(*aFormattingObjects.iView);
	TInt arabicLines = NumberOfLines(*aFormattingObjects.iLayout);
	SetTextL(*aFormattingObjects.iRichText, KJapanese, KJapaneseRepeats, KJapaneseRepeats);
	TTimeIntervalMicroSeconds japaneseFormatting = MeasureFormattingL(*aFormattingObjects.iView);
	TInt japaneseLines = NumberOfLines(*aFormattingObjects.iLayout);

	// Check that the results are fair
	TInt minLines;
	TInt maxLines;
	if (arabicLines < englishLines)
		{
		minLines = arabicLines;
		maxLines = englishLines;
		}
	else
		{
		minLines = englishLines;
		maxLines = arabicLines;
		}
	if (japaneseLines < minLines)
		minLines = japaneseLines;
	else if (maxLines < japaneseLines)
		maxLines = japaneseLines;

	//Tests that the number of lines in each test is more or less balanced
	TESTPOINT(maxLines * 100 <= minLines * 105);

	TBuf<256> buf;
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_OpeningLargeParagraph;microseconds: %d"),
		static_cast<TInt>(opening.Int64()));
	TESTPRINT(buf);
	buf.Zero();
	
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_RmsCursorDown;microseconds: %d"),
		static_cast<TInt>(rmsCursorDown.Int64()));
	TESTPRINT(buf);
	buf.Zero();
	
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormattingEnglish;microseconds: %d"),
		static_cast<TInt>(englishFormatting.Int64()));
	TESTPRINT(buf);
	buf.Zero();
	
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormattingArabic;microseconds: %d"),
		static_cast<TInt>(arabicFormatting.Int64()));
	TESTPRINT(buf);
	buf.Zero();
	
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormattingJapanese;microseconds: %d"),
		static_cast<TInt>(japaneseFormatting.Int64()));
	TESTPRINT(buf);	
	}

/**
@SYMTestCaseID SYSLIB-FORMA-UT-1896
@SYMTestCaseDesc Benchmarks inserting text from the beginning of a paragraph
@SYMTestPriority High
@SYMTestActions creates a large paragraph containg just one word and inserts text iteratively from
the beginning
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF092140, DEF092139
*/
void RunInsertTextTestsL(TInt aRepeats, TInt aRepeatsPerParagraph,
	RArray<TTFTimeStamp>& aTimeStamps, TDocInfo& aDocInfo)
	{
    TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1896 "));
	TFormattingObjects o;
	CreateFormattingObjectsLC(o);
	TInt64 slowest = 0;
	o.iLayout->SetWrapWidth(KInsertDisplayWidth);
	o.iLayout->SetAmountToFormat(CTextLayout::EFFormatBand);
	MeasureInsertTextAtStartL(aRepeats, aRepeatsPerParagraph, aTimeStamps, o, slowest);
	aDocInfo = GetSampleDocInfoL(o);

	TBuf<256> buf;
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_InsertText_Slowest;microseconds: %Ld"), slowest);
	TESTPRINT(buf);
	
	DestroyFormattingObjects(o);
	}

/**
@SYMTestCaseID SYSLIB-FORMA-UT-1895
@SYMTestCaseDesc Benchmarks deleting text from the beginning of a paragraph
@SYMTestPriority High
@SYMTestActions creates a large paragraph containg just one word and deletes text iteratively from
the beginning
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF092140, DEF092139
*/
void RunDeleteTextFromStartTest(RArray<TTFTimeStamp>& aTimeStamps, TDocInfo& aDocInfo)
	{
    TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1895 "));
	TFormattingObjects o;
	CreateFormattingObjectsLC(o);
	TInt64 slowest = 0;
	o.iLayout->SetWrapWidth(KInsertDisplayWidth);
	o.iLayout->SetAmountToFormat(CTextLayout::EFFormatBand);
	RBuf bigbuf;
	BigBufLC(bigbuf);
	SetTextL(*o.iRichText, bigbuf, 1, 1);
	TTimeIntervalMicroSeconds opening = MeasureOpeningL(*o.iView);

	MeasureDeleteTextFromStartL(aTimeStamps, o, slowest);
	aDocInfo = GetSampleDocInfoL(o);

	TBuf<256> buf;	
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_DeleteText_Slowest;microseconds: %Ld"), slowest);
	TESTPRINT(buf);	

	CleanupStack::PopAndDestroy();//bigbuf
	DestroyFormattingObjects(o);
	}

void GetFormattingBenchmarkL(TInt aNumberOfIterations, TInt& aNumberOfCharacters, TInt64& aNormalisedBenchmark)
	{
    TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1895 "));
	TFormattingObjects o;
	CreateFormattingObjectsLC(o);
	o.iLayout->SetWrapWidth(KInsertDisplayWidth);
	o.iLayout->SetAmountToFormat(CTextLayout::EFFormatBand);

	SetTextL(*(o.iRichText), KEnglish, aNumberOfIterations, aNumberOfIterations);

	TTimeIntervalMicroSeconds ignore = MeasureOpeningL(*(o.iView));
	o.iView->SetDocPosL(o.iRichText->DocumentLength());
	//make a global change
	o.iLayout->SetWrapWidth(KChangedDisplayWidth);
	//measure time to reformat for it
	TTime start;
	TTime end;
	start.UniversalTime();
	o.iView->HandleGlobalChangeL();
	end.UniversalTime();

	TTimeIntervalMicroSeconds formattingTime = end.MicroSecondsFrom(start);

	aNumberOfCharacters = o.iRichText->DocumentLength();
	aNormalisedBenchmark = formattingTime.Int64()/aNumberOfIterations;

	DestroyFormattingObjects(o);
	}

/**
@SYMTestCaseID SYSLIB-FORMA-UT-1894
@SYMTestCaseDesc Benchmarks formatting text
@SYMTestPriority High
@SYMTestActions for a series of documents of increasing size consisting of a single paragraph,
sets the cursor position to the end of the document, makes a global change and formats it.
@SYMTestExpectedResults The test must not fail or panic. The increase in time taken should be
roughly linear with the document size.
@SYMDEF DEF095401
*/
void RunFormattingBenchmarksL()
	{
    TESTPRINT(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1894 "));
	TInt numberOfCharacters = 0;
	TInt numberOfIterations;
	TInt64 normalisedBenchmark = 0;

	TBuf<256> buf;
	
	numberOfIterations = 1;
	GetFormattingBenchmarkL(numberOfIterations, numberOfCharacters, normalisedBenchmark);
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormatText;document contains %d characters: %Ld microseconds per iteration"), numberOfCharacters, normalisedBenchmark);
	TESTPRINT(buf);
	
	buf.Zero();
	numberOfIterations = 5;
	GetFormattingBenchmarkL(numberOfIterations, numberOfCharacters, normalisedBenchmark);
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormatText;document contains %d characters: %Ld microseconds per iteration"), numberOfCharacters, normalisedBenchmark);
	TESTPRINT(buf);
	
	buf.Zero();
	numberOfIterations = 10;
	GetFormattingBenchmarkL(numberOfIterations, numberOfCharacters, normalisedBenchmark);
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormatText;document contains %d characters: %Ld microseconds per iteration"), numberOfCharacters, normalisedBenchmark);
	TESTPRINT(buf);

	buf.Zero();
	numberOfIterations = 50;
	GetFormattingBenchmarkL(numberOfIterations, numberOfCharacters, normalisedBenchmark);
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormatText;document contains %d characters: %Ld microseconds per iteration"), numberOfCharacters, normalisedBenchmark);
	TESTPRINT(buf);

	buf.Zero();
	numberOfIterations = 100;
	GetFormattingBenchmarkL(numberOfIterations, numberOfCharacters, normalisedBenchmark);
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormatText;document contains %d characters: %Ld microseconds per iteration"), numberOfCharacters, normalisedBenchmark);
	TESTPRINT(buf);

	buf.Zero();
	numberOfIterations = 250;
	GetFormattingBenchmarkL(numberOfIterations, numberOfCharacters, normalisedBenchmark);
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormatText;document contains %d characters: %Ld microseconds per iteration"), numberOfCharacters, normalisedBenchmark);
	TESTPRINT(buf);

	buf.Zero();
	numberOfIterations = 500;
	GetFormattingBenchmarkL(numberOfIterations, numberOfCharacters, normalisedBenchmark);
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormatText;document contains %d characters: %Ld microseconds per iteration"), numberOfCharacters, normalisedBenchmark);
	TESTPRINT(buf);
	
	buf.Zero();
	numberOfIterations = 750;
	GetFormattingBenchmarkL(numberOfIterations, numberOfCharacters, normalisedBenchmark);
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormatText;document contains %d characters: %Ld microseconds per iteration"), numberOfCharacters, normalisedBenchmark);
	TESTPRINT(buf);
	
	buf.Zero();
	numberOfIterations = 1000;
	GetFormattingBenchmarkL(numberOfIterations, numberOfCharacters, normalisedBenchmark);
	buf.AppendFormat(_L("PERFORMANCE: Syslibs;Form_FormatText;document contains %d characters: %Ld microseconds per iteration"), numberOfCharacters, normalisedBenchmark);
	TESTPRINT(buf);
	}

void RunBenchmarksL()
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	RunFormattingBenchmarksL();

	TDocInfo docInfo;
	RArray<TTFTimeStamp> timestampsDown;
	RArray<TTFTimeStamp> timestampsUp;
	CleanupClosePushL(timestampsDown);
	CleanupClosePushL(timestampsUp);

	RunDeleteTextFromStartTest(timestampsUp, docInfo);
	WriteSampleDocInfo(docInfo, fileTimeStamps);
	WriteTimeStampsL(KDeleteTextFromStart, timestampsUp);

	RunInsertTextTestsL(KEnglishInsertRepeats, KEnglishInsertRepeats, timestampsUp, docInfo);
	WriteSampleDocInfo(docInfo, fileTimeStamps);
	WriteTimeStampsL(KInsertTextAtStart, timestampsUp);

	RunScrollingTestL(KEnglishScrollRepeats, KEnglishScrollRepeats, timestampsDown, timestampsUp, docInfo);
	WriteSampleDocInfo(docInfo, fileTimeStamps);
	WriteTimeStampsL(KScrollingDown, timestampsDown);
	WriteTimeStampsL(KScrollingUp, timestampsUp);

	RunScrollingTestL(KEnglishScrollRepeats, KEnglishScrollRepeats/2, timestampsDown, timestampsUp, docInfo);
	WriteSampleDocInfo(docInfo, fileTimeStamps);
	WriteTimeStampsL(KScrollingDown, timestampsDown);
	WriteTimeStampsL(KScrollingUp, timestampsUp);

	RunScrollingTestL(KEnglishScrollRepeats, KEnglishScrollRepeats/10, timestampsDown, timestampsUp, docInfo);
	WriteSampleDocInfo(docInfo, fileTimeStamps);
	WriteTimeStampsL(KScrollingDown, timestampsDown);
	WriteTimeStampsL(KScrollingUp, timestampsUp);

	TFormattingObjects o;
	CreateFormattingObjectsLC(o);
	RunEnglishArabicJapaneseBenchmarksL(o);
	DestroyFormattingObjects(o);

	CleanupStack::PopAndDestroy(2);
	CleanupStack::PopAndDestroy(scheduler);

	}

TVerdict CTFormBenchmarkStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    TestStep = this;
    
    TESTPRINT(KFormBenchmark);
    TESTPRINT(_L("Start Font/Bitmap Server"));

	TInt error = RFbsSession::Connect();
	if (error == KErrNotFound)
		{
		FbsStartup();
		error = RFbsSession::Connect();
		}
	// Tests that FBServ actually starts
	TESTPOINT(error == KErrNone);
	error = fs.Connect();
	TESTPOINT(error == KErrNone);

	error = fileTimeStamps.Replace(fs, KLogTimeStampsName, EFileWrite);
	
	TBuf<256> buf;
	buf.AppendFormat(_L("> fileTimeStamps.Replace %d"), error);
	TESTPRINT(buf);
	
	TESTPOINT(error == KErrNone);

	TESTPRINT(_L("Run Benchmarks"));
	TRAP(error, RunBenchmarksL());
	// Tests that the Benchmarks did not run out of memory
	// or otherwise leave
	TESTPOINT(error == KErrNone);

	fileTimeStamps.Close();
	fs.Close();
	RFbsSession::Disconnect();

	return TestStepResult();
	}

