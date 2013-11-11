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
#include <e32std.h>
#include <coemain.h>
#include <coeaui.h>
#include <txtrich.h>
#include <s32file.h>
#include <bacline.h>

#include "bandtestfileoutput.h"
#include "testenvironment.h"
#include "bandvalidator.h"
#include "bandmaintainer.h"
#include "bandtestdocument.h"
#include "bandtestactions.h"

#include "randomizedtests.h"
#include "randomizedactions.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

const TInt KMinParagraphs = 1;
const TInt KMaxParagraphs = 5;
const TInt KMaxParagraphLength = 1000;
TInt KNumberOfTestCases = 10;
TInt KEditsPerTestCase = 200;
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
Control whether or not to record all actions
*/
TBool record = EFalse;

CCoeEnv* cone;
CBandTestEnvironment* testEnv;
CFileOutput* output;
CRichText* richText;
CBandMaintainer* maintainer;
CBandValidator* validator;

_LIT(KRandomizedTest, "Randomized Test");
RTest test(KRandomizedTest);
_LIT(KRecord, "record");

RFs fs;
RFile file;

#ifdef __WINS__
	_LIT(KOutputFileName, "c:\\randomizedtests.txt");
	_LIT(KReplayStoreName, "c:\\randomizedtests_replay.dat");
	_LIT(KDocContentsFileName, "c:\\doc_contents.dat");
#else
	_LIT(KOutputFileName, "e:\\randomizedtests.txt");
	_LIT(KReplayStoreName, "e:\\randomizedtests_replay.dat");
	_LIT(KDocContentsFileName, "e:\\doc_contents.dat");
#endif

TInt BoundedRandom(TInt aMin, TInt aMax)
	{
	__ASSERT_ALWAYS(aMin <= aMax, User::Invariant());

	TInt64 seed = Math::Random();
	TReal randomReal = Math::FRand(seed);

	TReal maxReal = randomReal * ((aMax-aMin)+1);
	TReal rounded = 0;
	User::LeaveIfError(Math::Round(rounded, maxReal, 5));

	TInt result = rounded + aMin;
	if(result > aMax)//this doesn't happen very often
		{
		return aMax;
		}
	return result;
	}

void RandomStringLC(TInt aMin, TInt aMax, RBuf& aString)
	{
	aString.CreateL(BoundedRandom(aMin, aMax));
	CleanupClosePushL(aString);
	for(TInt i = 0; i < aString.MaxLength(); i++)
		{
		TChar c = KAvailableCharacters()[BoundedRandom(0, KAvailableCharacters().Length()-1)];
		aString.Append(c);
		}
	}

_LIT8(KCR8, "\r\n");

void WriteViewInfoL(const CTextView& aView)
	{
//info about the view as a whole
	const CTextLayout& layout = *(aView.Layout());
	const CTmTextLayout& tmLayout = layout.TagmaTextLayout();

	TInt sc = tmLayout.StartChar();
	TInt ec = tmLayout.EndChar();
	TInt pCount = tmLayout.Paragraphs();
	TInt lCount = tmLayout.Lines();
	TInt firstLine = 0;
	TInt lastLine = firstLine + lCount;

	TTmDocPos cursorPos;
	aView.GetCursorPos(cursorPos);

	TBuf8<256> info;
	info.Format(_L8("start: %d, end: %d, pars: %d, lines: %d, cursor: %d\r\n"), sc, ec, pCount, lCount, cursorPos.iPos);
	file.Write(info);
	file.Write(KCR8);

//detailed info about the lines it contains

	TInt needed = 0;
	TInt currentLine = firstLine;

	TBuf8<256> lineInfoOutput;

	while(currentLine < lastLine)
		{
		TTmLineInfo currentLineInfo;
		tmLayout.LineNumberToLine(currentLine, currentLineInfo);
		if(currentLineInfo.iFlags & TTmLineInfo::EParStart)
			{
			lineInfoOutput.Format(_L8("<p>"));
			}
		else
			{
			lineInfoOutput.Format(_L8("   "));
			}
		lineInfoOutput.AppendFormat(_L8("%4d: "), currentLineInfo.iStart);
		lineInfoOutput.AppendFormat(_L8("%4d: "), currentLineInfo.iEnd);
		TBuf<64> lineContents;
		tmLayout.GetDisplayedTextL(currentLine, lineContents, needed);
		__ASSERT_ALWAYS(needed <= lineContents.Length(), User::Invariant());


		for(TInt c = 0; c < lineContents.Length(); c++)
			{
			const TUint16 ch = lineContents[c];
			TUint16 mask = 0xFF00;
			if((mask & ch) == 0)
				{
				lineInfoOutput.Append(ch);
				}
			else
				{
				lineInfoOutput.AppendFormat(_L8("<%d>"),ch);
				}
			}

		lineInfoOutput.Append(KCR8);
		file.Write(lineInfoOutput);
		currentLine++;
		}

	file.Write(KCR8);
	}

void WriteDetailedDocInfoL()
	{
	TBuf8<32> view;
	view.Format(_L8("\r\n\r\nview under test\r\n"));
	file.Write(view);
	WriteViewInfoL(maintainer->ViewUnderTest());

	view.Format(_L8("\r\nreference view\r\n"));
	file.Write(view);
	WriteViewInfoL(maintainer->ViewReference());
	file.Write(KCR8);
	}

void SetupDocL(CDesCArray& aParagraphs)
	{
	TInt paragraphCount = BoundedRandom(KMinParagraphs, KMaxParagraphs);

	for (TInt i = 0; i <= paragraphCount; i++)
		{
		RBuf randomString;
		RandomStringLC(1, KMaxParagraphLength, randomString);
		aParagraphs.AppendL(randomString);
		CleanupStack::PopAndDestroy();
		}
	TDocumentSpec docSpec(aParagraphs, 0);
	docSpec.SetupDocumentL(*richText, *maintainer);
	}

void StoreL(const CDesCArray& aParagraphs, const RPointerArray<CRandomizedAction>& aActions)
	{
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs, KReplayStoreName, EFileWrite));
	CleanupClosePushL(stream);

	TInt pCount = aParagraphs.Count();
	stream.WriteInt32L(pCount);
	for(TInt i = 0; i < pCount; i++)
		{
		stream << aParagraphs[i];
		}

	TInt aCount = aActions.Count();
	stream.WriteInt32L(aCount);
	for(TInt i = 0; i < aCount; i++)
		{
		stream << *(aActions[i]);
		}

	stream.CommitL();
	CleanupStack::PopAndDestroy();
	}

void RestoreL(CDesCArray& aParagraphs, RPointerArray<CRandomizedAction>& aActions)
	{
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs, KReplayStoreName, EFileRead));
	CleanupClosePushL(stream);

	TInt pCount = stream.ReadInt32L();

	RFileWriteStream docStream;
	User::LeaveIfError(docStream.Replace(fs, KDocContentsFileName, EFileWrite));
	CleanupClosePushL(docStream);

	for(TInt i = 0; i < pCount; i++)
		{
		HBufC* paragraph = HBufC::NewLC(stream, KMaxParagraphLength);
		TBuf8<2048> b;
		for(TInt c = 0; c < paragraph->Length(); c++)
			{
			const TUint16 ch = paragraph->operator[](c);
			TUint16 mask = 0xFF00;
			if((mask & ch) == 0)
				{
				b.Append(ch);
				}
			else
				{
				b.AppendFormat(_L8("\\x%3x"), ch);
				}
			}
		b.Append(KCR8);
		b.Append(KCR8);
		docStream << b;
		aParagraphs.AppendL(*paragraph);
		CleanupStack::PopAndDestroy(paragraph);//does the array take ownership. or not??
		}
	docStream.CommitL();
	CleanupStack::PopAndDestroy();
	TInt aCount = stream.ReadInt32L();
	for(TInt i = 0; i < aCount; i++)
		{
		CRandomizedAction* action = CRandomizedAction::NewLC(*maintainer, *richText, *validator, testEnv->Device(), file, stream);
		aActions.AppendL(action);
		CleanupStack::Pop(action);
		}

	CleanupStack::PopAndDestroy();
	}

void DestroyActionArray(TAny* aArray)
	{
	RPointerArray<CRandomizedAction>* array = reinterpret_cast<RPointerArray<CRandomizedAction>*>(aArray);
	array->ResetAndDestroy();
	array->Close();
	}

void ReplayL()
	{
	//initialise the maintainer & validator
	TRect screen;
	testEnv->Device().GetDrawRect(screen);

	TPoint origin(screen.iBr.iX - KRectWidth,0);

	TSize size(KRectWidth, KRectHeight);
	TRect rect(origin, size);

	maintainer = CBandMaintainer::NewLC(*richText, rect, KDisplayTextBox);
	validator = CBandValidator::NewLC(maintainer->ViewUnderTest(), maintainer->ViewReference(), *output);

//initialise the actions and initial document
	RPointerArray<CRandomizedAction> actions;
	TCleanupItem cleanupArray(DestroyActionArray, &actions);
	CleanupStack::PushL(cleanupArray);
	CDesCArrayFlat* doc = new(ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(doc);
	RestoreL(*doc, actions);
	TDocumentSpec docSpec(*doc, 0);
 	docSpec.SetupDocumentL(*richText, *maintainer);
//replay the actions
	TInt count = actions.Count();
	for(TInt i = 0; i < count; i++)
		{
		CRandomizedAction* action = actions[i];
		TBool success = action->ExecuteL();
		WriteDetailedDocInfoL();
		__ASSERT_ALWAYS(success, User::Invariant());
		if (action->StopTest())
			break;
		}
	CleanupStack::PopAndDestroy(4);	//doc, cleanupArray
	}


TBool ExecuteRandomActionsL(const CDesCArray& aParagraphs, RPointerArray<CRandomizedAction>& aActions)
	{
	for (TInt i = 0; i < KEditsPerTestCase; i++)
		{
		CRandomizedAction* action = CRandomizedAction::NewLC(*maintainer, *richText, *validator, testEnv->Device(), file);
		User::LeaveIfError(aActions.Append(action));
		CleanupStack::Pop();
		if(record)
			{
			StoreL(aParagraphs, aActions);
			}
		if (!action->ExecuteL())
			{
			return EFalse;
			}
		}
	return ETrue;
	}

/**
@SYMTestCaseID SYSLIB-FORMA-UT-1902
@SYMTestCaseDesc Runs randomized tests which perform scrolls, edits and reformats
@SYMTestPriority High
@SYMTestActions Creates random documents and runs random tests on it, can be configured to
perform different amounts of documents and edits
@SYMTestExpectedResults The test must not fail or panic .
@SYMDEF DEF092140, DEF092139
*/
void RunTestsL()
	{
	TBuf<256> info;
	info.Format(_L(" @SYMTestCaseID:SYSLIB-FORMA-UT-1902 Starting Randomized tests, Number of test cases: %d and Number of Edits per test %d. "),KNumberOfTestCases,KEditsPerTestCase);
	test.Start(info);
	for(TInt i = 0; i < KNumberOfTestCases; i++)
		{
		TRect screen;
		testEnv->Device().GetDrawRect(screen);

		TPoint origin(screen.iBr.iX - KRectWidth,0);

		TSize size(KRectWidth, KRectHeight);
		TRect rect(origin, size);

		maintainer = CBandMaintainer::NewLC(*richText, rect, KDisplayTextBox);
		validator = CBandValidator::NewLC(maintainer->ViewUnderTest(), maintainer->ViewReference(), *output);

		info.Zero();
		info.Format(_L("Test Case %d"),i);
		test.Next(info);
		RPointerArray<CRandomizedAction> actions;
		TCleanupItem cleanupArray(DestroyActionArray, &actions);
		CleanupStack::PushL(cleanupArray);
		CDesCArrayFlat* doc = new(ELeave) CDesCArrayFlat(5);
		CleanupStack::PushL(doc);
		SetupDocL(*doc);
		TBool success = ExecuteRandomActionsL(*doc, actions);
		if(!success)
			{
			StoreL(*doc, actions);
			__ASSERT_ALWAYS(success, User::Invariant());
			break;
			}
		if (actions[actions.Count()-1]->StopTest())
			break;
		CleanupStack::PopAndDestroy(4);	//doc, cleanupArray
		}
	test.End();
	}

void SetupAndRunTestsL()
	{
//fs, file
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(file.Replace(fs, KOutputFileName, EFileWrite));
	CleanupClosePushL(file);
//control environment
	cone->ConstructL();
	cone->DisableExitChecks(ETrue);//no idea how Cone cleanup is supposed to be done
//flush commands
	RWsSession& wsSession = cone->WsSession();
	wsSession.SetAutoFlush(ETrue);
//test environment
	testEnv = CBandTestEnvironment::NewLC(*cone, KStandardFont, KStandardFontSize);

	richText = CRichText::NewL(testEnv->ParaFormat(), testEnv->CharFormat());
	CleanupStack::PushL(richText);

	output = CFileOutput::NewL();
	CleanupStack::PushL(output);

	CCommandLineArguments* command = CCommandLineArguments::NewLC();

	if(command->Count() < 2)
		{
		record = EFalse;
		RunTestsL();
		}
	else
		{
		if(command->Count() == 4)
			{
			TLex numberOfTests(command->Arg(2));
			User::LeaveIfError(numberOfTests.Val(KNumberOfTestCases));
			TLex editsPerTest(command->Arg(3));
			User::LeaveIfError(editsPerTest.Val(KEditsPerTestCase));
			}

		TPtrC arg = command->Arg(1);
		if(arg == KRecord)
			{
			record = ETrue;
			RunTestsL();
			}
		else
			{
			ReplayL();
			}
		}

	CleanupStack::PopAndDestroy(6);//is this a record??
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
	test(error==KErrNone);
	cone->DestroyEnvironment();
	RFbsSession::Disconnect();
	__UHEAP_MARKEND;
	test.Close();
	return error;
	}

