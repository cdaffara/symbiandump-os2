/*
* Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* TPARSE.CPP
*
*/


#include <txtrich.h>
#include <e32math.h>
#include "T_parse.h"
#include "T_PARSE_for_TEF.h"

LOCAL_D CTestStep *pTestStep = NULL;
#define test(cond)											\
	{														\
	TBool __bb = (cond);									\
	pTestStep->TEST(__bb);									\
	if (!__bb)												\
		{													\
		pTestStep->ERR_PRINTF1(_L("ERROR: Test Failed"));	\
		User::Leave(1);										\
		}													\
	}
#undef INFO_PRINTF1
#undef INFO_PRINTF2
#undef INFO_PRINTF3
// copy from tefexportconst.h
#define INFO_PRINTF1(p1)            pTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1))
#define INFO_PRINTF2(p1, p2)        pTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2))
#define INFO_PRINTF3(p1, p2, p3)    pTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3))


LOCAL_D CTrapCleanup* TrapCleanup;
LOCAL_D const TInt KTestCleanupStack=0x200;


CTestParser* CTestParser::NewL()
	{
	CTestParser* self = new (ELeave) CTestParser;
	CleanupStack::PushL(self);
	self->iDoItText = HBufC::NewL(128);
	*(self->iDoItText) = _L("Or is it just undead?");
	CleanupStack::Pop();
	return self;
	}


CTestParser::CTestParser()
	{
	}


CTestParser::~CTestParser()
	{
	delete iDoItText;
	}


TBool CTestParser::ParseThisText(const CRichText& aTextObj, TBool aAllowBack,
									TInt aStartScan, TInt aScanLength,
									TInt& aStartTag, TInt& aTagLength)
	{
	// Very simple - looking for the phrase "the world"
	_LIT(targetText, "TARGET");
	TInt length = 6;
	// Consider that the start of the scan may be part way through
	// the item that we are looking for, so see if we need to move back
	TInt start = aStartScan;
	if (aAllowBack)
		{
		if (start >= length - 1)
			start -= length - 1;
		else
			start = 0;
		}
	// And it might go beyond the end of the scan
	TInt end = aStartScan + aScanLength;
	if (end + length - 1 <= aTextObj.DocumentLength())
		end += length - 1;
	else
		end = aTextObj.DocumentLength();
	while (end - start >= length)
		{
		TPtrC buffer = aTextObj.Read(start, end - start);
		TInt segmentLength= buffer.Length();

		if (segmentLength >= length)	// There's enough text to bother searching
			{
			TInt result = buffer.FindF(targetText);
			if (result != KErrNotFound)	// We found it
				{
				aStartTag = start + result;
				aTagLength = length;
				return ETrue;
				}
			}
		if (end == start + segmentLength)	// There's no more text at all
			break;
		// The buffer is segmented and there's another segment 

		// It could be over the segment boundary
		TInt start2 = start + segmentLength;
		TInt end2 = start2;
		if (segmentLength >= length - 1)
			start2 -= length - 1;
		else start2 = start;
		if (end >= end2 + length - 1)
			end2 += length -1;
		else
			end2 = end;
		if (end2 - start2 >= length)
			{
			// Create a buffer with the end of one and the start of the other
			TBuf<10> bridgeBuffer;
			aTextObj.Extract(bridgeBuffer, start2, end2 - start2);
			TInt result = bridgeBuffer.FindF(targetText);
			if (result != KErrNotFound)	// We found it
				{
				aStartTag = start2 + result;
				aTagLength = length;
				return ETrue;
				}
			}

		// Move start up for next segment
		start += segmentLength;
		}
		return EFalse;	// Not enough text left in buffer
	}


const TDesC& CTestParser::CreateDoItText(const CRichText& /* aTextObj */,
							TInt /* aStartText */, TInt /* aLength */)
	{
	return *iDoItText;
	}


void CTestParser::ActivateThisTextL(const CRichText& /* aTextObj */,
					  TInt /* aStartText */, TInt /* aLength */)
	{
	// Do something?

	}


void CTestParser::GetRecogniseFormat(TCharFormat& aFormat)
	{
	aFormat.iFontPresentation.iTextColor = KRgbRed;
	aFormat.iFontPresentation.iUnderline = EUnderlineOn;
	}


void CTestParser::GetRolloverFormat(TCharFormat& aFormat)
	{
	aFormat.iFontPresentation.iTextColor = KRgbRed;
	aFormat.iFontPresentation.iUnderline = EUnderlineOn;
	aFormat.iFontPresentation.iHighlightColor = KRgbDarkRed;
	aFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightRounded;
	}


void CTestParser::Release()
	{
	delete this;
	}

//--------------------------------------------

CTestParser2* CTestParser2::NewL()
	{
	CTestParser2* self = new (ELeave) CTestParser2;
	CleanupStack::PushL(self);
	self->iDoItText = HBufC::NewL(128);
	*(self->iDoItText) = _L("Or is it just undead?");
	CleanupStack::Pop();
	return self;
	}


CTestParser2::CTestParser2()
	{
	}


CTestParser2::~CTestParser2()
	{
	delete iDoItText;
	}


TBool CTestParser2::ParseThisText(const CRichText& aTextObj, TBool aAllowBack,
									TInt aStartScan, TInt aScanLength,
									TInt& aStartTag, TInt& aTagLength)
	{
	// Very simple - looking for the phrase "ARG"
	_LIT(targetText, "ARG");
	TInt length = 3;
	// Consider that the start of the scan may be part way through
	// the item that we are looking for, so see if we need to move back
	TInt start = aStartScan;
	if (aAllowBack)
		{
		if (start >= length - 1)
			start -= length - 1;
		else
			start = 0;
		}
	// And it might go beyond the end of the scan
	TInt end = aStartScan + aScanLength;
	if (end + length - 1 <= aTextObj.DocumentLength())
		end += length - 1;
	else
		end = aTextObj.DocumentLength();
	while (end - start >= length)
		{
		TPtrC buffer = aTextObj.Read(start, end - start);
		TInt segmentLength= buffer.Length();

		if (segmentLength >= length)	// There's enough text to bother searching
			{
			TInt result = buffer.FindF(targetText);
			if (result != KErrNotFound)	// We found it
				{
				aStartTag = start + result;
				aTagLength = length;
				return ETrue;
				}
			}
		if (end == start + segmentLength)	// There's no more text at all
			break;
		// The buffer is segmented and there's another segment 

		// It could be over the segment boundary
		TInt start2 = start + segmentLength;
		TInt end2 = start2;
		if (segmentLength >= length - 1)
			start2 -= length - 1;
		else start2 = start;
		if (end >= end2 + length - 1)
			end2 += length -1;
		else
			end2 = end;
		if (end2 - start2 >= length)
			{
			// Create a buffer with the end of one and the start of the other
			TBuf<10> bridgeBuffer;
			aTextObj.Extract(bridgeBuffer, start2, end2 - start2);
			TInt result = bridgeBuffer.FindF(targetText);
			if (result != KErrNotFound)	// We found it
				{
				aStartTag = start2 + result;
				aTagLength = length;
				return ETrue;
				}
			}

		// Move start up for next segment
		start += segmentLength;
		}
		return EFalse;	// Not enough text left in buffer
	}


const TDesC& CTestParser2::CreateDoItText(const CRichText& /* aTextObj */,
							TInt /* aStartText */, TInt /* aLength */)
	{
	return *iDoItText;
	}


void CTestParser2::ActivateThisTextL(const CRichText& /* aTextObj */,
					  TInt /* aStartText */, TInt /* aLength */)
	{
	// Do something?

	}


void CTestParser2::GetRecogniseFormat(TCharFormat& aFormat)
	{
	aFormat.iFontPresentation.iTextColor = KRgbRed;
	aFormat.iFontPresentation.iUnderline = EUnderlineOn;
	}


void CTestParser2::GetRolloverFormat(TCharFormat& aFormat)
	{
	aFormat.iFontPresentation.iTextColor = KRgbRed;
	aFormat.iFontPresentation.iUnderline = EUnderlineOn;
	aFormat.iFontPresentation.iHighlightColor = KRgbDarkRed;
	aFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightRounded;
	}


void CTestParser2::Release()
	{
	delete this;
	}


//--------------------------------------------


CEditObserver::CEditObserver()
	{
	iStart = 0;
	iExtent = 0;
	}


CEditObserver::~CEditObserver()
	{
	}


void CEditObserver::EditObserver(TInt aStart, TInt aExtent)
	{
	iStart = aStart;
	iExtent = aExtent;
	}


void Test1()
	{
	INFO_PRINTF1(_L("Install 5, deinstall in reverse order"));
	CTestParser* parser1 = CTestParser::NewL();
	CTestParser* parser2 = CTestParser::NewL();
	CTestParser* parser3 = CTestParser::NewL();
	CTestParser* parser4 = CTestParser::NewL();
	CTestParser* parser5 = CTestParser::NewL();
	CRichText::ActivateParserL(parser1);	// List 1
	CRichText::ActivateParserL(parser2);	// List 1, 2
	CRichText::ActivateParserL(parser3);	// List 1, 2, 3
	CRichText::ActivateParserL(parser4);	// List 1, 2, 3, 4
	CRichText::ActivateParserL(parser5);	// List 1, 2, 3, 4, 5
	CRichText::DeactivateParser(parser5);	// List 1, 2, 3, 4
	CRichText::DeactivateParser(parser4);	// List 1, 2, 3
	CRichText::DeactivateParser(parser3);	// List 1, 2
	CRichText::DeactivateParser(parser2);	// List 1
	CRichText::DeactivateParser(parser1);	// List empty
	delete parser5;
	delete parser4;
	delete parser3;
	delete parser2;
	delete parser1;
	CRichText::DeactivateParserDefaults();
	}


void Test2()
	{
	INFO_PRINTF1(_L("Install, deinstall in interleaved order"));
	CTestParser* parser1 = CTestParser::NewL();
	CRichText::ActivateParserL(parser1);	// List 1
	CTestParser* parser2 = CTestParser::NewL();
	CRichText::ActivateParserL(parser2);	// List 1, 2
	// Remove first in list
	CRichText::DeactivateParser(parser1);	// List 2
	delete parser1;
	CTestParser* parser3 = CTestParser::NewL();
	CRichText::ActivateParserL(parser3);	// List 2, 3
	// Remove last in list
	CRichText::DeactivateParser(parser3);	// List 2
	delete parser3;
	CTestParser* parser4 = CTestParser::NewL();
	CRichText::ActivateParserL(parser4);	// List 2, 4
	CTestParser* parser5 = CTestParser::NewL();
	CRichText::ActivateParserL(parser5);	// List 2, 4, 5
	// Remove middle in list
	CRichText::DeactivateParser(parser4);	// List 2, 5
	delete parser4;
	// Empty list
	CRichText::DeactivateParser(parser5);	// List 2
	delete parser5;
	CRichText::DeactivateParser(parser2);	// List empty
	delete parser2;
	CRichText::DeactivateParserDefaults();
	}


void Test3()
	{
	INFO_PRINTF1(_L("Testing memory with OOM"));

	TInt i;
	TInt ret;
	TInt count = 0;
	TInt success = 0;

	for (i = 0; i < 20; i++)
		{
		CTestParser* parser1 = NULL;
		CTestParser* parser2 = NULL;
		CTestParser* parser3 = NULL;
		CTestParser* parser4 = NULL;
		CTestParser* parser5 = NULL;
		// Switch on memory problems, varying fail rate from 20 to 1
		__UHEAP_SETFAIL(RHeap::EDeterministic, 20 - i);
		__UHEAP_MARK;
		count++;
		TRAP(ret, parser1 = CTestParser::NewL());
		if (ret != KErrNone)
			{
			parser1 = NULL;
			}
		else
			{
			TRAP(ret, CRichText::ActivateParserL(parser1));
			if (ret != KErrNone)
				{
				delete parser1;
				parser1 = NULL;
				}
			}
		TRAP(ret, parser2 = CTestParser::NewL());
		if (ret != KErrNone)
			{
			parser2 = NULL;
			}
		else
			{
			TRAP(ret, CRichText::ActivateParserL(parser2));
			if (ret != KErrNone)
				{
				delete parser2;
				parser2 = NULL;
				}
			}
		TRAP(ret, parser3 = CTestParser::NewL());
		if (ret != KErrNone)
			{
			parser3 = NULL;
			}
		else
			{
			TRAP(ret, CRichText::ActivateParserL(parser3));
			if (ret != KErrNone)
				{
				delete parser3;
				parser3 = NULL;
				}
			}
		TRAP(ret, parser4 = CTestParser::NewL());
		if (ret != KErrNone)
			{
			parser4 = NULL;
			}
		else
			{
			TRAP(ret, CRichText::ActivateParserL(parser4));
			if (ret != KErrNone)
				{
				delete parser4;
				parser4 = NULL;
				}
			}
		TRAP(ret, parser5 = CTestParser::NewL());
		if (ret != KErrNone)
			{
			parser5 = NULL;
			}
		else
			{
			TRAP(ret, CRichText::ActivateParserL(parser5));
			if (ret != KErrNone)
				{
				delete parser5;
				parser5 = NULL;
				}
			}
		if (parser1)
			{
			success++;
			CRichText::DeactivateParser(parser1);
			delete parser1;
			}
		if (parser2)
			{
			success++;
			CRichText::DeactivateParser(parser2);
			delete parser2;
			}
		if (parser3)
			{
			success++;
			CRichText::DeactivateParser(parser3);
			delete parser3;
			}
		if (parser4)
			{
			success++;
			CRichText::DeactivateParser(parser4);
			delete parser4;
			}
		if (parser5)
			{
			success++;
			CRichText::DeactivateParser(parser5);
			delete parser5;
			}
		CRichText::DeactivateParserDefaults();
		// Switch off memory problems
		__UHEAP_MARKEND;
		__UHEAP_RESET;
		}

	INFO_PRINTF3(_L("%d attempted activations, %d successful\n"), 5 * count, success);
	}


void Test4()
	{
	// Create a block of 1000 chars
	// Randomly insert a target string and check:
	// - Can't find target that is not complete
	// - Does find complete target in right place
	// - Once target is removed, can't find it
	// repeat x 100
	INFO_PRINTF1(_L("Testing EText behaviour with active parsers and single target"));
	// Create and activate a parser
	CTestParser* parser1 = CTestParser::NewL();
	CRichText::ActivateParserL(parser1);
	// Create a CRichText
	CParaFormatLayer* GlobalParaFormatLayer = CParaFormatLayer::NewL();
	CCharFormatLayer* GlobalCharFormatLayer = CCharFormatLayer::NewL();
	CRichText* richText = CRichText::NewL(GlobalParaFormatLayer, GlobalCharFormatLayer, CEditableText::ESegmentedStorage, CEditableText::EDefaultTextGranularity);
	richText->SetPictureFactory(NULL, NULL);  // forces index generation
	// Switch on parsers for this CRichText
	CEditObserver* editObserver = new (ELeave) CEditObserver;
	richText->SetEditObserver(editObserver);

	// Insert 1000 chars (repeated string)
	TInt i;
	for (i = 0; i < 100; i++)
		richText->InsertL(richText->DocumentLength(), _L("abcdefghij"));
	TInt64 seed = 314159;
	TInt startTags;
	TInt lengthTags;
	for (i = 0; i < 100; i++)
		{
#if 0
		INFO_PRINTF2(_L("i=%d"), i);
#endif
		// Get a random in range 0-999
		TInt random = Math::Rand(seed) % 1000;
		// "Randomly" insert target string that is not complete
		richText->InsertL(random, _L("TARGE"));
		test(!richText->ParseText(startTags, lengthTags, EFalse));
		test(!richText->ParseText(startTags, lengthTags, ETrue));
		// Complete target string and check we find single target where we expect
		richText->InsertL(random + 5, 'T');
#if 1
		test(richText->ParseText(startTags, lengthTags, EFalse));
#else
		TBool b = richText->ParseText(startTags, lengthTags, EFalse);
		INFO_PRINTF2(_L("    b=%d"), b);
		test(b);
#endif
		test((startTags == random) && (lengthTags == 6));
		test(richText->ParseText(startTags, lengthTags, ETrue));
		test((startTags == random) && (lengthTags == 6));
		// Completely remove target string
		richText->DeleteL(random, 6);
		test(!richText->ParseText(startTags, lengthTags, EFalse));
		test(!richText->ParseText(startTags, lengthTags, ETrue));
		}

	// Clean up

	delete richText;
	delete GlobalCharFormatLayer;
	delete GlobalParaFormatLayer;
	CRichText::DeactivateParser(parser1);
	delete parser1;
	CRichText::DeactivateParserDefaults();
	delete editObserver;
	}


void Test5()
	{
	// Create a block of 1000 chars
	// Randomly insert two target strings and check:
	// - Can't find targets that are not complete
	// - Does find complete targets with exact range covered
	// - Once targets are removed, can't find it
	// repeat x 100
	INFO_PRINTF1(_L("Testing EText behaviour with active parsers and double target"));
	// Create and activate a parser
	CTestParser* parser1 = CTestParser::NewL();
	CRichText::ActivateParserL(parser1);
	// Create a CRichText
	CParaFormatLayer* GlobalParaFormatLayer = CParaFormatLayer::NewL();
	CCharFormatLayer* GlobalCharFormatLayer = CCharFormatLayer::NewL();
	CRichText* richText = CRichText::NewL(GlobalParaFormatLayer, GlobalCharFormatLayer, CEditableText::ESegmentedStorage, CEditableText::EDefaultTextGranularity);
	richText->SetPictureFactory(NULL, NULL);  // forces index generation
	// Switch on parsers for this CRichText
	CEditObserver* editObserver = new (ELeave) CEditObserver;
	richText->SetEditObserver(editObserver);

	// Insert 1000 chars (repeated string)
	TInt i;
	for (i = 0; i < 100; i++)
		richText->InsertL(richText->DocumentLength(), _L("abcdefghij"));
	TInt64 seed1 = 314159;
	TInt startTags;
	TInt lengthTags;
	for (i = 0; i < 100; i++)
		{
#if 0
		INFO_PRINTF2(_L("i=%d"), i);
#endif
		// Get a random in range 0-999
		TInt random1 = Math::Rand(seed1) % 1000;
		TInt random2 = Math::Rand(seed1) % 1000;
		TInt rlow = (random1 < random2) ? random1 : random2;
		TInt rhigh = (random1 > random2) ? random1 : random2;
		if (rlow + 7 > rhigh)
			{ // Too close, spread them out
			if (rhigh + 7 <= richText->DocumentLength())
				rhigh += 7;
			else
				rlow -= 7;
			}
		// "Randomly" insert target strings that are not complete
		richText->InsertL(rlow, _L("TARGE"));
		richText->InsertL(rhigh - 1, _L("TARGE"));
		test(!richText->ParseText(startTags, lengthTags, EFalse));
		test(!richText->ParseText(startTags, lengthTags, ETrue));
		// Complete target string and check we find single target where we expect
		richText->InsertL(rlow + 5, 'T');
		richText->InsertL(rhigh + 5, 'T');
#if 1
		test(richText->ParseText(startTags, lengthTags, EFalse));
#else
		TBool bb = richText->ParseText(startTags, lengthTags, EFalse);
		test(bb);
#endif
		test((startTags == rlow) && (lengthTags == rhigh + 6 - rlow));
		test(richText->ParseText(startTags, lengthTags, ETrue));
		test((startTags == rlow) && (lengthTags == rhigh + 6 - rlow));
		// Completely remove target string
		richText->DeleteL(rhigh, 6);
		richText->DeleteL(rlow, 6);
		test(!richText->ParseText(startTags, lengthTags, EFalse));
		test(!richText->ParseText(startTags, lengthTags, ETrue));
		}

	// Clean up

	delete richText;
	delete GlobalCharFormatLayer;
	delete GlobalParaFormatLayer;
	CRichText::DeactivateParser(parser1);
	delete parser1;
	CRichText::DeactivateParserDefaults();
	delete editObserver;
	}

/**
@SYMTestCaseID          SYSLIB-ETEXT-UT-3405
@SYMTestCaseDesc	    Test for the fix for when CRichText crashes when parsers are active
@SYMTestPriority 	    Medium
@SYMTestActions  	    Calls PositionOfNextTag on all available document positions to validate fix.
@SYMTestExpectedResults Test must be able to call PositionOfNextTag on all document positions
@SYMDEF                 PDEF102494
*/	

void Test6()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-UT-3405 Calling PositionOfNextTag on an empty document "));
	
	// Create and activate a parser
	CTestParser* parser1 = CTestParser::NewL();
	CRichText::ActivateParserL(parser1);
	// Create a CRichText
	CParaFormatLayer* GlobalParaFormatLayer = CParaFormatLayer::NewL();
	CCharFormatLayer* GlobalCharFormatLayer = CCharFormatLayer::NewL();
	CRichText* richText = CRichText::NewL(GlobalParaFormatLayer, GlobalCharFormatLayer, CEditableText::ESegmentedStorage, CEditableText::EDefaultTextGranularity);
	richText->SetPictureFactory(NULL, NULL);  // forces index generation
	// Switch on parsers for this CRichText
	CEditObserver* editObserver = new (ELeave) CEditObserver;
	richText->SetEditObserver(editObserver);

	//insert and format some text
	richText->InsertL(richText->DocumentLength(), _L("abcdTARGET"));

	TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	richText->ApplyCharFormatL(charFormat, charFormatMask, 0, richText->DocumentLength());

	TInt startTags;
	TInt lengthTags;
	richText->ParseText(startTags, lengthTags, ETrue);

    // Call position of next tag on all document positions
	for (TInt i = 0; i <= richText->DocumentLength(); i++)
		{
		richText->PositionOfNextTag(i, parser1);
		}
	
	// Clean up

	delete richText;
	delete GlobalCharFormatLayer;
	delete GlobalParaFormatLayer;
	CRichText::DeactivateParser(parser1);
	delete parser1;
	CRichText::DeactivateParserDefaults();
	delete editObserver;
	}

/**
@SYMTestCaseID          SYSLIB-ETEXT-UT-3406
@SYMTestCaseDesc	    Test for the fix for when CRichText crashes when parsers are active
@SYMTestPriority 	    Medium
@SYMTestActions  	    Initialize two parsers. First parser has a target string of "TARGET",
                        and the second parser has a target string of "ARG". By parsing the
                        two overlapping (ARG is contained in TARGET) target strings, the changes
						for this defect have full coverage.
@SYMTestExpectedResults Test must be able to call PositionOfNextTag on the end of document positions
@SYMDEF                 PDEF102494
*/
void Test7()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-UT-3406 Testing EText behaviour with active parsers and single target "));
	
	// Create and activate a parser
	CTestParser* parser1 = CTestParser::NewL();
	CRichText::ActivateParserL(parser1);
	CTestParser2* parser2 = CTestParser2::NewL();
	CRichText::ActivateParserL(parser2);
	// Create a CRichText
	CParaFormatLayer* GlobalParaFormatLayer = CParaFormatLayer::NewL();
	CCharFormatLayer* GlobalCharFormatLayer = CCharFormatLayer::NewL();
	CRichText* richText = CRichText::NewL(GlobalParaFormatLayer, GlobalCharFormatLayer, CEditableText::ESegmentedStorage, CEditableText::EDefaultTextGranularity);
	richText->SetPictureFactory(NULL, NULL);  // forces index generation
	// Switch on parsers for this CRichText
	CEditObserver* editObserver = new (ELeave) CEditObserver;
	richText->SetEditObserver(editObserver);

	// Insert 1000 chars (repeated string)
	TInt i;
	for (i = 0; i < 100; i++)
		richText->InsertL(richText->DocumentLength(), _L("abcdefghij"));

	TInt startTags;
	TInt lengthTags;
	TInt startPosition = 0;
	
	// Run test twice, once at start of string, once at end.
	for (TInt i=0;i < 2;i++)
		{
		//Insert target string that is not complete (for code coverage purposes).
		richText->InsertL(startPosition, _L("TARGE"));
		// Parse range provided only. Parser two will set it's parser tags on "ARG".
		test(richText->ParseText(startTags, lengthTags, EFalse));
		// Ensure this call does not fail at end of document (anymore). This gets
		// the position of the next tag at the end of document position. Should
		// be none found at end of document.
		test((richText->PositionOfNextTag(richText->DocumentLength(), parser1)) == -1);
		
		// Parse all text (for coverage purposes). This will first clear all tags,
		// and then re-applies the tags again on "ARG"
		test(richText->ParseText(startTags, lengthTags, ETrue));
		test((richText->PositionOfNextTag(richText->DocumentLength(), parser1)) == -1);
		
		// Complete target string. The purpose of completing the target string
		// here is for code coverage. The text has changed and the code path in 
		// parselst.cpp will change.
		richText->InsertL(startPosition + 5, 'T');

		// Parse range provided only. The tags on "ARG" are considered
		// lower precendence, so this will clear the parser tags on "ARG" and 
		// will apply the parser tags on "TARGET". 
		test(richText->ParseText(startTags, lengthTags, EFalse));
		test((richText->PositionOfNextTag(richText->DocumentLength(), parser1)) == -1);
				
		// Completely remove target string
		richText->DeleteL(startPosition, 6);
		test(!richText->ParseText(startTags, lengthTags, EFalse));
		test(!richText->ParseText(startTags, lengthTags, ETrue));
		
		startPosition = richText->DocumentLength();
		}

	// Clean up

	delete richText;
	delete GlobalCharFormatLayer;
	delete GlobalParaFormatLayer;
	CRichText::DeactivateParser(parser1);
	delete parser1;
	CRichText::DeactivateParser(parser2);
	delete parser2;
	CRichText::DeactivateParserDefaults();
	delete editObserver;
	}

CT_PARSE::CT_PARSE()
    {
    SetTestStepName(KTestStep_T_PARSE);
    pTestStep = this;
    }

TVerdict CT_PARSE::doTestStepL()
    {
    SetTestStepResult(EFail);

    TrapCleanup = CTrapCleanup::New();
    TRAPD(r,\
        {\
        for (TInt i=KTestCleanupStack;i>0;i--)\
            CleanupStack::PushL((TAny*)1);\
        test(r==KErrNone);\
        CleanupStack::Pop(KTestCleanupStack);\
        });

    INFO_PRINTF1(_L("Testing EText parser system"));

    INFO_PRINTF1(_L("Testing memory under normal conditions"));

    __UHEAP_MARK;
    TRAPD(ret1, Test1());
    __UHEAP_MARKEND;

    __UHEAP_MARK;
    TRAPD(ret2, Test2());
    __UHEAP_MARKEND;

    __UHEAP_MARK;
    TRAPD(ret3, Test3());
    __UHEAP_MARKEND;

    __UHEAP_MARK;
    TRAPD(ret4, Test4());
    __UHEAP_MARKEND;

    __UHEAP_MARK;
    TRAPD(ret5, Test5());
    __UHEAP_MARKEND;

    __UHEAP_MARK;
    TRAPD(ret6, Test6());
    __UHEAP_MARKEND;
    
    __UHEAP_MARK;
    TRAPD(ret7, Test7());
    __UHEAP_MARKEND;
    
    delete TrapCleanup;

    if (r == KErrNone && ret1 == KErrNone && ret2 == KErrNone && ret3 == KErrNone && ret4 == KErrNone && ret5 == KErrNone && ret6 == KErrNone && ret7 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
