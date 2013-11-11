// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Started by Brendan, January 1996
// test code for RIncrMatcher
// 
//

#include <bamatch.h>
#include <badesca.h>
#include <e32test.h>

RTest test(_L("Testing RIncrMatcher"));

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0421
@SYMTestCaseDesc        RIncrMatcherBase class test
                        Tests for RIncrMatcherBase::MatchText() function
@SYMTestPriority        Medium
@SYMTestActions         Compare two text buffers
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void MatcherManip(RIncrMatcherBase &aMatcher)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0421 Manipulation "));
	aMatcher.SetMatchText(_L("Hello"));
	test(aMatcher.MatchText()==_L("Hello"));
	test(aMatcher.MatchText()!=_L("Hell"));
	aMatcher.AppendChar('w');
	test(aMatcher.MatchText()==_L("Hellow"));
	aMatcher.DeleteLastChar();
	test(aMatcher.MatchText()==_L("Hello"));
	aMatcher.DeleteLastChar();
	test(aMatcher.MatchText()==_L("Hell"));
	aMatcher.Clear();
	test(aMatcher.MatchText()==_L(""));
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0422
@SYMTestCaseDesc        Tests for RIncrMatcherBase::SetMatchText(),RIncrMatcherBase::SetBestMatch()
                        RIncrMatcherBase::SetBestMatchF() functions
@SYMTestPriority        Medium
@SYMTestActions         Attempt to set the best match
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void MatcherBest(RIncrMatcherBase &aMatcher)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0422 SetBest "));
	aMatcher.SetMatchText(_L("Hello"));
	test(aMatcher.MatchText()==_L("Hello"));
	aMatcher.SetBestMatch(_L("Hell"));
	test(aMatcher.MatchText()==_L("Hell"));
	aMatcher.SetBestMatch(_L("HelL"));
	test(aMatcher.MatchText()==_L("Hel"));
	aMatcher.SetBestMatchF(_L("HEL"));
	test(aMatcher.MatchText()==_L("Hel"));
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0423
@SYMTestCaseDesc        Tests for RIncrMatcherBase::FirstMatchingIndex()
                        RIncrMatcherBase::FirstMatchingIndexF(),RIncrMatcherBase::FirstMatchingIndexC() functions
@SYMTestPriority        Medium
@SYMTestActions         Attempt to find correct array entry
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void MatcherArray(RIncrMatcherBase &aMatcher)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0423 "));
	TPtrC KArrayZero(_S("Hel"));
	TPtrC KArrayOne(_S("heLLo"));
	TPtrC KArrayTwo(_S("Hello"));
	TPtrC KArrayThree(_S("Bye"));
//
	CDesCArray* array=new CDesCArrayFlat(5);
	array->Reset();
	TRAPD(trapVal,array->AppendL(KArrayZero));
	test(trapVal==KErrNone);
	TRAP(trapVal,array->AppendL(KArrayOne));
	test(trapVal==KErrNone);
	TRAP(trapVal,array->AppendL(KArrayTwo));
	test(trapVal==KErrNone);
	TRAP(trapVal,array->AppendL(KArrayThree));
	test(trapVal==KErrNone);
//
	aMatcher.SetMatchText(_L("Bye"));
	TInt result;
	test(aMatcher.FirstMatchingIndex(result,*array)==KErrNone);
	test(result==3);
//
	aMatcher.SetMatchText(_L("Hello"));
	test(aMatcher.FirstMatchingIndex(result,*array)==KErrNone);
	test(result==2);
	test(aMatcher.FirstMatchingIndexF(result,*array)==KErrNone);
	test(result==1);
//
	aMatcher.SetMatchText(_L("heLL"));
	test(aMatcher.FirstMatchingIndexC(result,*array)==KErrNone);
	test(result==1);
//
	delete(array);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0424
@SYMTestCaseDesc        Tests for RIncrMatcherBase::IsMatchF() function
@SYMTestPriority        Medium
@SYMTestActions         Tests for maximum length match
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void MatcherMax(RIncrMatcherBase& aMatcher,TInt aMax)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0424 Max length "));
	HBufC* buf=HBufC::NewL(aMax);
	buf->Des().SetLength(aMax);
	buf->Des().Fill('a');
	aMatcher.SetMatchText(buf->Left(aMax-1));
	test(aMatcher.IsMatchF(*buf));
	aMatcher.AppendChar('a');
	test(aMatcher.IsMatchF(*buf));
	aMatcher.DeleteLastChar();
	test(aMatcher.IsMatchF(*buf));
	aMatcher.AppendChar('w');
	test(!aMatcher.IsMatchF(*buf));
	aMatcher.DeleteLastChar();
	test(aMatcher.IsMatchF(*buf));
	delete(buf);
	}

TInt E32Main()
    {
	test.Title();
	test.Start(_L("Incremental Matcher "));
//
	const TInt KMaxMatchLength=10;
	RIncrMatcherBuf<KMaxMatchLength> matcherBuf;
	TBuf<KMaxMatchLength> buf;
	RIncrMatcherPtr matcherPtr;
	matcherPtr.SetMatcherPtr(buf);
	RIncrMatcherTextBuf matcherText;
	matcherText.SetMatcherLengthL(KMaxMatchLength);
	CTrapCleanup *trapCleanup=CTrapCleanup::New();
//
	__UHEAP_MARK;
	MatcherManip(matcherBuf);
	MatcherManip(matcherPtr);
	MatcherManip(matcherText);
	MatcherBest(matcherBuf);
	MatcherBest(matcherPtr);
	MatcherBest(matcherText);
	MatcherArray(matcherBuf);
	MatcherArray(matcherPtr);
	MatcherArray(matcherText);
	MatcherMax(matcherBuf,KMaxMatchLength);
	MatcherMax(matcherPtr,KMaxMatchLength);
	MatcherMax(matcherText,KMaxMatchLength);
	__UHEAP_MARKEND;
//
	delete(trapCleanup);
	test.End();
    return(KErrNone);
    }
