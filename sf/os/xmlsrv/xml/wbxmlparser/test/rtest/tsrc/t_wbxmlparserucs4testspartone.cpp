// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32test.h>
#include <charconv.h>

#include <xml/parserfeature.h>
#include <xml/xmlframeworkerrors.h>

#include "stabilitytestclass.h"
#include "testdocuments.h"

using namespace Xml;

static RTest test(_L("t_wbxmlparserucs4testspartone"));


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3725
@SYMTestCaseDesc		    Parsing unicode characters.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Parsing unicode characters and comparing the expected output.
@SYMTestExpectedResults 	Expected out matches actual output.
@SYMREQ 		 		 	REQ0000
*/
static void ParserTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3725 ParserTestsL tests... "));

	__UHEAP_MARK;


	TBuf16<256> ucs4_0041;

	// <wml>A</wml>
	// 0x01, 0x04, 0x6A, 0x00, 0x7F, 0x02, 0x80, 0x41, 0x01

	ucs4_0041.Append(0x01);
	ucs4_0041.Append(0x04);
	ucs4_0041.Append(0x6A);
	ucs4_0041.Append(0x00);
	ucs4_0041.Append(0x7F);
	ucs4_0041.Append(0x02);
	ucs4_0041.Append(0x80);
	ucs4_0041.Append(0x41);
	ucs4_0041.Append(0x01);

	_LIT (KDataComparison, "<wml xmlns='-//WAPFORUM//DTD WML 1.1//EN'>A</wml>");

	TPassOrFailureSettings aPassOrFailureSettings;

	aPassOrFailureSettings.iStringDictionaryUri = &KBlankStringDictionaryUri;
	aPassOrFailureSettings.iDoParseDocument = ETrue;
	aPassOrFailureSettings.iFilenameProvided = EFalse;
	aPassOrFailureSettings.iParseMode = EErrorOnUnrecognisedTags|
										ERawContent;
	aPassOrFailureSettings.iExpectedCode = KErrNone;


	CStabilityTestClass* behaviourTest = CStabilityTestClass::NewL(test);
	CleanupStack::PushL(behaviourTest);


	behaviourTest->TestBehaviour(ucs4_0041, aPassOrFailureSettings);

	test(KDataComparison() == *behaviourTest->iOutput);
	behaviourTest->iOutput->Zero();


	CleanupStack::PopAndDestroy(behaviourTest);

	__UHEAP_MARKEND;
	}

static void MainL()
	{
	ParserTestsL();
	}

TInt E32Main()
	{

	__UHEAP_MARK;
	test.Title();
	test.Start(_L("initialising"));

	CTrapCleanup* c=CTrapCleanup::New();

	// start the loader
	RFs fs;
	test (fs.Connect()==KErrNone);
	fs.Close();

	test (c!=0);
	TRAPD(r,MainL());
	test (r==KErrNone);
	delete c;
	test.End();
	test.Close();
	__UHEAP_MARKEND;

	return KErrNone;
	}

