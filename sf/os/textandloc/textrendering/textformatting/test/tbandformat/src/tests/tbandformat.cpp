/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32test.h>

#include <coemain.h>
#include <coeaui.h>
#include <txtrich.h>

#include "bandtestfileoutput.h"
#include "testenvironment.h"
#include "bandvalidator.h"
#include "bandmaintainer.h"
#include "bandtestdocument.h"
#include "bandtestactions.h"

#include "sampletext.h"
/**
@file
@internalComponent
*/

/*
Constants. If we want to move to a script-based test system,
these should go in an initialisation file
*/
/*
Controls whether or not to show the text box
*/
const TBool KDisplayTextBox = ETrue;
/*
Height and width of the rectangle containing the text.
The text is formatted to fit this
*/
const TInt KRectHeight = 200;
const TInt KRectWidth = 200;
/*
Standard font type and size
*/
_LIT(KStandardFont, "ClearlyU");
const TInt KStandardFontSize = 17;
/*
Controls whether the 'edit points' chosen for tests are selected from the visible
portion of the band only, or the entire formatted range.
It is recommended to use the entire formatted range, but to compare the output with
previous revisions of the code which has a different formatted range but the same
visible range, it may be necessary to test only the visible portion
*/
const TBool KSelectEditPointsFromWholeBand = ETrue;
/*
End of constants
*/

CCoeEnv* cone;
CBandTestEnvironment* testEnv;
CFileOutput* output;

_LIT(KBandFormat, "TBandFormat");
RTest test(KBandFormat);
_LIT(KScroll, "Scrolling Test");


/*
Given a Document this function will scroll through it, for both the test and refernce view,and
after each scroll check the validity of the contents of the test band formatting view against
the non band formatting reference view.

This is done by first scrolling line by line and then page by page in the following order:
-from the beginning to the end
-from the end back to beginning
-from the beginning back to the end

After each scroll the contents are validated.
*/
void ScrollingTestL(CBandMaintainer& aMaintainer, CBandValidator& aValidator)
	{
	TBuf<32> desc;
	desc=KScroll;

	aMaintainer.InitialiseViewL();

//scroll line by line
	User::After(100000);
//scroll down
	TCursorPosition::TMovementType move = TCursorPosition::EFLineDown;
	test.Start(_L("Scrolling down line by line"));
	while (move == TCursorPosition::EFLineDown)
		{
		test(aMaintainer.ScrollL(move));
		test(aValidator.ValidateL(desc));
		User::After(100000);
		}
//scroll up
	move = TCursorPosition::EFLineUp;
	test.Next(_L("Scrolling up line by line"));
	while (move == TCursorPosition::EFLineUp)
		{
		test(aMaintainer.ScrollL(move));
		test(aValidator.ValidateL(desc));
		User::After(100000);
		}
//scroll down
	move = TCursorPosition::EFLineDown;
	test.Next(_L("Scrolling down line by line"));
	while (move == TCursorPosition::EFLineDown)
		{
		test(aMaintainer.ScrollL(move));
		test(aValidator.ValidateL(desc));
		User::After(100000);
		}

	User::After(100000);

//scroll page by page
	aMaintainer.InitialiseViewL();
//scroll down
	move = TCursorPosition::EFPageDown;
	test.Next(_L("Scrolling down page by page"));
	while (move == TCursorPosition::EFPageDown)
		{
		test(aMaintainer.ScrollL(move));
		test(aValidator.ValidateL(desc));
		User::After(100000);
		}
//scroll up
	move = TCursorPosition::EFPageUp;
	test.Next(_L("Scrolling up page by page"));
	while (move == TCursorPosition::EFPageUp)
		{
		test(aMaintainer.ScrollL(move));
		test(aValidator.ValidateL(desc));
		User::After(100000);
		}
//scroll down
	move = TCursorPosition::EFPageDown;
	test.Next(_L("Scrolling down page by page"));
	while (move == TCursorPosition::EFPageDown)
		{
		test(aMaintainer.ScrollL(move));
		test(aValidator.ValidateL(desc));
		User::After(100000);
		}
	test.End();
	}

/**
@SYMTestCaseID SYSLIB-FORMA-UT-1899
@SYMTestCaseDesc This function runs the scrolling tests. It sets up the documents then calls the ScrollTestL function
to scroll through them.
@SYMTestPriority High
@SYMTestActions The following paragraphs are tested by scrolling up and down through them:
A LTR paragraph containing a few lines of RTL text
A LTR paragraph containing less than a line of RTL text
A LTR paragraph containing a portion of RTL text which is greater than the band
A RTL paragraph containing a few lines of LTR text
A RTL paragraph containing less than a line of LTR text
A RTL paragraph containing a portion of LTR text which is greater than the band
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF092140, DEF092139
*/
void RunScrollingTestsL(CRichText& aRichText, CBandMaintainer& aMaintainer, CBandValidator& aValidator)
	{
// set up 3 paragraphs of LTR with runs of RTL embedded in each paragraph
	aRichText.Reset();
	test.Start(_L("Scrolling LTR document with embedded RTL"));
	aMaintainer.InitialiseViewL();
//LTR paragraph containing a few lines of RTL text
	aRichText.InsertL(aRichText.DocumentLength(), KLTRWithRTL);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
//LTR paragraph containing less than a line of RTL text
	aRichText.InsertL(aRichText.DocumentLength(), KLTRWithSmallRTL);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
//LTR paragraph containing a portion of RTL text which is greater than the band
	aRichText.InsertL(aRichText.DocumentLength(), KLTRWithLargeRTL);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
	ScrollingTestL(aMaintainer, aValidator);

// 3 paragraphs of RTL with runs of LTR embedded in each paragraph
	aRichText.Reset();
	test.Next(_L("Scrolling RTL document with embedded LTR"));
	aMaintainer.InitialiseViewL();
//RTL paragraph containing a few lines of LTR text
	aRichText.InsertL(aRichText.DocumentLength(), KRTLWithLTR);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
//RTL paragraph containing less than a line of LTR text
	aRichText.InsertL(aRichText.DocumentLength(), KRTLWithSmallLTR);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
//RTL paragraph containing a portion of LTR text which is greater than the band
	aRichText.InsertL(aRichText.DocumentLength(), KRTLWithLargeLTR);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
	ScrollingTestL(aMaintainer, aValidator);
	test.End();
	}

/**
@SYMTestCaseID SYSLIB-FORMA-UT-1900
@SYMTestCaseDesc This function will run tests to check if the bidirectional state is being
maintained correctly. Documents containing a mix of different directionality are created and
specific edits which are of interest for each document are performed. These edits are location
specific and will depend on the bidirectional state of surrounding text.
@SYMTestPriority High
@SYMTestActions Edits are performed:
changing the base directionality of a paragraph
changing the directionality of text above the edit position
changing the directionality of neutral characters before and after edit position
changing the directionality of mirrored characters such as parenthesis
changing text directinality by adding explicit embedding
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF092140, DEF092139
*/
void RunBidirectionalTestL(CRichText& aRichText, CBandMaintainer& aMaintainer, CBandValidator& aValidator)
	{

	aRichText.Reset();
	aMaintainer.InitialiseViewL();

//set up paragraph of just LTR text
	aRichText.InsertL(0, KBleak2);
	aMaintainer.InitialiseViewL();
	User::After(3000000);

//change paragraph base directionality by inserting a RLM marker at the beginning of the paragraph
	TInsertText insertRLM(aMaintainer, aRichText, aValidator, KRLM());
	insertRLM.ExecuteL(0, 1);
	User::After(3000000);

	aRichText.Reset();
	aMaintainer.InitialiseViewL();

//set up paragraph of LLL-RRR-LLL-NNN-RRR-LLL
	aRichText.InsertL(0, KEnglishLongWords_1);
	aRichText.InsertL(aRichText.DocumentLength(), KArabicLessThanALine);
	aRichText.InsertL(aRichText.DocumentLength(), KEnglishLessThanALine_1);
	aRichText.InsertL(aRichText.DocumentLength(), KPuncs);
	aRichText.InsertL(aRichText.DocumentLength(), KArabicLessThanALine);
	aRichText.InsertL(aRichText.DocumentLength(), KBleak2);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
	aMaintainer.InitialiseViewL();
	User::After(1000000);

//delete the second run of LTR changing the directionality of the neutrals
	TDeleteText deleteText(aMaintainer, aRichText, aValidator);
	TInt editPos = KEnglishLongWords_1().Length()+KArabicLessThanALine().Length();
	deleteText.ExecuteL(editPos,KEnglishLessThanALine_1().Length());
	User::After(1000000);

//delete the second run of RTL characters to check if neutral directionality is maintained
	editPos = KEnglishLongWords_1().Length()+KArabicLessThanALine().Length()+KPuncs().Length();
	deleteText.ExecuteL(editPos,KArabicLessThanALine().Length());
	User::After(1000000);

	aRichText.Reset();
	aMaintainer.InitialiseViewL();

//set up band to be a paragraph of numbers followed by LTR i.e. EN-EN-EN-LLL
	aRichText.InsertL(aRichText.DocumentLength(), KNumbers);
	aRichText.InsertL(aRichText.DocumentLength(), KBleak2);
	aMaintainer.InitialiseViewL();
	User::After(3000000);

//edit to change directionality of numbers above the edit by inserting RTL text before the LTR
//This RLM works perfectly
	TInsertText insertArabic(aMaintainer, aRichText, aValidator, KArabicMoreThanALine());
	insertArabic.ExecuteL(KNumbers().Length(), KArabicMoreThanALine().Length());
	User::After(3000000);

//undo previous edit text will return to previous directionality
	deleteText.ExecuteL(KNumbers().Length(),KArabicMoreThanALine().Length());
	User::After(10000000);

	aRichText.Reset();
	aMaintainer.InitialiseViewL();

// Document containing RRRRR ( LLLL ) LLLL
	aRichText.InsertL(0, KArabicMoreThanALine);
	aRichText.InsertL(aRichText.DocumentLength(), KOpenBracket);
	aRichText.InsertL(aRichText.DocumentLength(), KEnglishLessThanALine_1);
	aRichText.InsertL(aRichText.DocumentLength(), KCloseBracket);
	aRichText.InsertL(aRichText.DocumentLength(), KEnglishLessThanALine_2);
	aRichText.InsertL(aRichText.DocumentLength(), KOpenBracket);
	aRichText.InsertL(aRichText.DocumentLength(), KEnglishLessThanALine_4);
	aRichText.InsertL(aRichText.DocumentLength(), KCloseBracket);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
	aMaintainer.InitialiseViewL();

// Insert LTR after RTL test Doc should now be RRRR LLLL ( LLLL ) LLLL
	TInsertText insertEnglish(aMaintainer, aRichText, aValidator, KEnglishLessThanALine_3());
	insertEnglish.ExecuteL(KArabicMoreThanALine().Length(), KEnglishLessThanALine_3().Length());
// The brackets are now being displayed correctly
// BUT is the neutral English text meant to be displayed as RTL as initially inserted (no RLM)??
	User::After(10000000);

	aRichText.Reset();
	aMaintainer.InitialiseViewL();

// Just an arabic RTL sentence.
	aRichText.InsertL(0, KArabicMoreThanALine);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
	aMaintainer.InitialiseViewL();

// Test formatting when inserting RTL enclosed in parenthesis
	TInsertText insertArabBrackets(aMaintainer, aRichText, aValidator, KArabicInParenthesis());
	insertArabBrackets.ExecuteL(KArabicMoreThanALine().Length(), KArabicInParenthesis().Length());
	User::After(1000000);

	aRichText.Reset();
	aMaintainer.InitialiseViewL();

// Just an arabic RTL sentence.
	aRichText.InsertL(0, KArabicMoreThanALine);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
	aMaintainer.InitialiseViewL();

// Test formatting when inserting open parenthesis
	TInsertText insertOpen(aMaintainer, aRichText, aValidator, KOpenBracket());
	insertOpen.ExecuteL(KArabicMoreThanALine().Length(), KOpenBracket().Length());
	User::After(1000000);

// Test formatting when inserting RTL after opening parenthesis
	TInsertText insertArabicLessLine(aMaintainer, aRichText, aValidator, KArabicLessThanALine());
	insertArabicLessLine.ExecuteL(aRichText.DocumentLength()-1, KArabicLessThanALine().Length());
	User::After(1000000);

// Test formatting when inserting closing parenthesis after RTL text
	TInsertText insertClose(aMaintainer, aRichText, aValidator, KCloseBracket());
	insertClose.ExecuteL(aRichText.DocumentLength()-1, KOpenBracket().Length());
// This is now being displayed correctly
	User::After(10000000);

// Insert RTL text after parenthesis
	insertArabicLessLine.ExecuteL(aRichText.DocumentLength()-1, KArabicLessThanALine().Length());
// This is now being displayed correctly
	User::After(10000000);

	aRichText.Reset();
	aMaintainer.InitialiseViewL();

//Explicit Embedding testing
	aRichText.InsertL(0, KEnglishShortParagraph);
	aRichText.InsertL(aRichText.DocumentLength(), KRLE);
	aRichText.InsertL(aRichText.DocumentLength(), KFirstWordArabic);
	aRichText.InsertL(aRichText.DocumentLength(), KFirstWord);
	aRichText.InsertL(aRichText.DocumentLength(), KSecondWordArabic);
	aRichText.InsertL(aRichText.DocumentLength(), KSecondWord);
	aRichText.InsertL(aRichText.DocumentLength(), KThirdWordArabic);
	aRichText.InsertL(aRichText.DocumentLength(), KPDF);
	aRichText.InsertL(aRichText.DocumentLength(), KEnglishMoreThanALine_1);
	aRichText.InsertL(aRichText.DocumentLength(), KParagraphDelimiter);
	aMaintainer.InitialiseViewL();

	User::After(1000000);
	editPos=KEnglishShortParagraph().Length()+1;
//	TInsertText insertText2(aMaintainer, aRichText, aValidator, KEnglishLessThanALine_1());
	insertEnglish.ExecuteL(editPos,KEnglishLessThanALine_3().Length());
	User::After(1000000);
	}

/*
Given a specification for a document and a view, this function will run the
single-edit tests for that document/view.

The document is represented by a descriptor array in which each descriptor is a paragraph.

The view is represented simply by the TInt aTopOfBand argument, which tells us how
many paragraphs to lose from the top of the band. So if it is zero, then we just initialise
the doc and don't scroll at all; if it is 1 we scroll down until 1 paragraph is removed from
the top of the band, and so on. Currently it's basically only ever set to 1 or 0, and is thus
used as a boolean "top_of_doc!=top_of_band"

So, given the document/view. the function will retrieve the list of useful positions to test,
and then test:
- increasing font size
- deleting
- inserting text
- inserting various other things
...making use of various helper functions to do so
*/
void RunSingleEditTestsOnDocumentL(CRichText& aRichText, CBandMaintainer& aMaintainer,
	CBandValidator& aValidator, const MDesCArray& aTextContents, TInt aTopOfBand)
	{
	TDocumentSpec docSpec(aTextContents, aTopOfBand);
	docSpec.SetupDocumentL(aRichText, aMaintainer);


	RArray<TInt> positions;
	CleanupClosePushL(positions);

	RArray<TTmLineInfo> lines;
	CleanupClosePushL(lines);

	if(KSelectEditPointsFromWholeBand)
		{
		GetLinesInBandL(aMaintainer.TmLayoutUnderTest(), lines);
		}
	else
		{
		GetVisibleLinesL(aMaintainer.LayoutUnderTest(), lines);
		}

	PositionsToTestL(lines, positions);

	output->WriteDocumentAttributesL(aMaintainer.ViewUnderTest());
	output->WriteDocumentPositionsL(positions);

	test.Start(_L("Reformatting: change font"));
	TReformatText changeFont(aMaintainer, aRichText, aValidator, EChangeFont, testEnv->Device());
	ExecuteTestL(aRichText, aMaintainer, changeFont, docSpec, positions);

	test.Next(_L("Reformatting: decrease font"));
	TReformatText decreaseFont(aMaintainer, aRichText, aValidator, EDecreaseFontSize, testEnv->Device());
	ExecuteTestL(aRichText, aMaintainer, decreaseFont, docSpec, positions);

	test.Next(_L("Reformatting: make bold"));
	TReformatText makeBold(aMaintainer, aRichText, aValidator, EMakeBold, testEnv->Device());
	ExecuteTestL(aRichText, aMaintainer, makeBold, docSpec, positions);

	test.Next(_L("Reformatting: increase font"));
	TReformatText increaseFont(aMaintainer, aRichText, aValidator, EIncreaseFontSize, testEnv->Device());
	ExecuteTestL(aRichText, aMaintainer, increaseFont, docSpec, positions);

	test.Next(_L("Deleting"));
	TDeleteText deleteText(aMaintainer, aRichText, aValidator);
	ExecuteTestL(aRichText, aMaintainer, deleteText, docSpec, positions);

	test.Next(_L("Inserting a space"));
	InsertTextL(aRichText, aMaintainer, aValidator, docSpec, KSpace, positions);

	test.Next(_L("Inserting several spaces"));
	InsertTextL(aRichText, aMaintainer, aValidator, docSpec, KSpaces, positions);

	test.Next(_L("Inserting less than a line of text"));
	InsertTextL(aRichText, aMaintainer, aValidator, docSpec, KEnglishLessThanALine_1, positions);

	test.Next(_L("Inserting multi-line text"));
	InsertTextL(aRichText, aMaintainer, aValidator, docSpec, KEnglishMoreThanALine_1, positions);

	test.Next(_L("Inserting RLO marker"));
	InsertTextL(aRichText, aMaintainer, aValidator, docSpec, KRLO, positions);

	test.Next(_L("Inserting paragraph delimiter"));
	InsertTextL(aRichText, aMaintainer, aValidator, docSpec, KParagraphDelimiter, positions);

	test.End();

	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID SYSLIB-FORMA-UT-1901
@SYMTestCaseDesc This function runs all the 'single edit' tests.
@SYMTestPriority High
@SYMTestActions  creates a number of documents, and then calls RunSingleEditTestsOnDocumentL to run
the actual suite of tests on that document.
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF092140, DEF092139
*/
void RunSingleEditTestsL(CRichText& aRichText, CBandMaintainer& aMaintainer, CBandValidator& aValidator)
	{
	CDesCArrayFlat* doc = new(ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(doc);

//document contains one short paragraph, followed by a paragraph bigger than the band
	doc->AppendL(KEnglishMoreThanALine_1);
	doc->AppendL(KEnglishLargeParagraph);
	test.Start(_L("Document 1: no scrolling"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 0);
	test.Next(_L("Document 1: scroll so top paragraph is removed"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 1);
	doc->Reset();

//document contains 5 short paragraphs, long words
	doc->AppendL(KEnglishLongWords_1);
	doc->AppendL(KEnglishLongWords_2);
	doc->AppendL(KEnglishLongWords_3);
	doc->AppendL(KEnglishLongWords_4);
	doc->AppendL(KEnglishLongWords_5);
	test.Next(_L("Document 2: no scrolling"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 0);
	test.Next(_L("Document 2: scroll so top paragraph is removed"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 1);
	doc->Reset();

//document contains 5 short paragraphs, short words
	doc->AppendL(KEnglishShortWords_1);
	doc->AppendL(KEnglishShortWords_2);
	doc->AppendL(KEnglishShortWords_3);
	doc->AppendL(KEnglishShortWords_4);
	doc->AppendL(KEnglishShortWords_5);

	test.Next(_L("Document 3: no scrolling"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 0);
	test.Next(_L("Document 3: scroll so top paragraph is removed"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 1);
	doc->Reset();

//document contains 10 paragraphs, each less than a line
	doc->AppendL(KEnglishLessThanALine_1);
	doc->AppendL(KEnglishLessThanALine_5);
	doc->AppendL(KEnglishLessThanALine_2);
	doc->AppendL(KEnglishLessThanALine_4);
	doc->AppendL(KEnglishLessThanALine_3);
	doc->AppendL(KEnglishLessThanALine_3);
	doc->AppendL(KEnglishLessThanALine_4);
	doc->AppendL(KEnglishLessThanALine_2);
	doc->AppendL(KEnglishLessThanALine_5);
	doc->AppendL(KEnglishLessThanALine_1);

	test.Next(_L("Document 4: no scrolling"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 0);
	test.Next(_L("Document 4: scroll so top paragraph is removed"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 1);
	doc->Reset();

//document contains 1 short paragraph, normal-length words
	doc->AppendL(KEnglishMoreThanALine_1);
	test.Next(_L("Document 5: no scrolling"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 0);
	doc->Reset();

//document contains 5 short paragraphs, normal-length words
	doc->AppendL(KEnglishMoreThanALine_1);
	doc->AppendL(KEnglishMoreThanALine_2);
	doc->AppendL(KEnglishMoreThanALine_3);
	doc->AppendL(KEnglishMoreThanALine_4);
	doc->AppendL(KEnglishMoreThanALine_5);
	test.Next(_L("Document 6: no scrolling"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 0);
	test.Next(_L("Document 6: scroll so top paragraph is removed"));
	RunSingleEditTestsOnDocumentL(aRichText, aMaintainer, aValidator, *doc, 1);
	doc->Reset();


	CleanupStack::PopAndDestroy();

	test.End();
	}

void SetupAndRunTestsL()
	{
//control environment
	cone->ConstructL();
	cone->DisableExitChecks(ETrue);//no idea how Cone cleanup is supposed to be done
//flush commands
	RWsSession& wsSession = cone->WsSession();
	wsSession.SetAutoFlush(ETrue);
//test environment
	testEnv = CBandTestEnvironment::NewLC(*cone, KStandardFont, KStandardFontSize);

	CRichText* richText = CRichText::NewL(testEnv->ParaFormat(), testEnv->CharFormat());
	CleanupStack::PushL(richText);

	TRect screen;
	testEnv->Device().GetDrawRect(screen);

	TPoint origin(screen.iBr.iX - KRectWidth,0);

	TSize size(KRectWidth, KRectHeight);
	TRect rect(origin, size);

	output = CFileOutput::NewL();
	CleanupStack::PushL(output);

	CBandMaintainer* maintainer = CBandMaintainer::NewLC(*richText, rect, KDisplayTextBox);
	CBandValidator* validator = CBandValidator::NewLC(maintainer->ViewUnderTest(), maintainer->ViewReference(), *output);

	test.Start(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1901 Single edit tests "));
//	RunSingleEditTestsL(*richText, *maintainer, *validator);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1900 Bidi tests "));
	RunBidirectionalTestL(*richText, *maintainer, *validator);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1899 Scrolling tests "));;
//	RunScrollingTestsL(*richText, *maintainer, *validator);
	test.End();

	CleanupStack::PopAndDestroy(5);
	}

GLDEF_C TInt E32Main()
	{
	test.Title();
	__UHEAP_MARK;
	TInt error = RFbsSession::Connect();
	if (error == KErrNotFound)
		{
		FbsStartup();
		error = RFbsSession::Connect();
		}
	test(error == KErrNone);

	cone=new CCoeEnv;//we need to do this here because CCoeEnv ctor will create a trap handler
	TRAP(error, SetupAndRunTestsL());
	cone->DestroyEnvironment();
	RFbsSession::Disconnect();
	__UHEAP_MARKEND;
	test.Close();
	return error;
	}
