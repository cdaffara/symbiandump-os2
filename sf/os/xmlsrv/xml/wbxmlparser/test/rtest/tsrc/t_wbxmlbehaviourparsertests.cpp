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

#include <xml/parserfeature.h>
#include <xml/xmlframeworkerrors.h>

#include "stabilitytestclass.h"
#include "testdocuments.h"

using namespace Xml;

_LIT8 (KStringDictionaryUri, "4~0");
_LIT  (KTestDocument, "No document needed for this test.");

static RTest test(_L("_wbxmlbehaviourparsertests"));

/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3732
@SYMTestCaseDesc		    Wbxml behaviour parser tests.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Testing various aspects of the wbxml parser.
@SYMTestExpectedResults 	Tests should pass.
@SYMREQ			 		 	REQ0000
*/
static void BehaviourTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3732 BehaviourTestsL tests... "));

	__UHEAP_MARK;

	TPassOrFailureSettings aPassOrFailureSettings;

	aPassOrFailureSettings.iStringDictionaryUri = &KStringDictionaryUri;
	aPassOrFailureSettings.iDoParseDocument = EFalse;
	aPassOrFailureSettings.iFilenameProvided = ETrue;

	CStabilityTestClass* behaviourTest = CStabilityTestClass::NewL(test);
	CleanupStack::PushL(behaviourTest);



	// Test 1
	aPassOrFailureSettings.iParseMode = EErrorOnUnrecognisedTags|
										ERawContent;
	aPassOrFailureSettings.iExpectedCode = KErrNone;

	behaviourTest->TestBehaviour(KTestDocument, aPassOrFailureSettings);


	// Test 2

	aPassOrFailureSettings.iParseMode = EErrorOnUnrecognisedTags;
	aPassOrFailureSettings.iExpectedCode = KErrNone;

	behaviourTest->TestBehaviour(KTestDocument, aPassOrFailureSettings);


	// Test 3

	//aPassOrFailureSettings.iParseMode = ERawContent;
	//aPassOrFailureSettings.iExpectedCode = KErrNotSupported;

	//behaviourTest->TestBehaviour(KTestDocument, aPassOrFailureSettings);


	// Test 4

	aPassOrFailureSettings.iParseMode = EErrorOnUnrecognisedTags|
										ERawContent |
										EReportUnrecognisedTags;
	aPassOrFailureSettings.iExpectedCode = KErrNotSupported;

	behaviourTest->TestBehaviour(KTestDocument, aPassOrFailureSettings);


	// Test 5

	//aPassOrFailureSettings.iParseMode = ENone;
	//aPassOrFailureSettings.iExpectedCode = KErrNotSupported;

	//behaviourTest->TestBehaviour(KTestDocument, aPassOrFailureSettings);


	// Test 6

	//aPassOrFailureSettings.iParseMode = ESendFullContentInOneChunk;
	//aPassOrFailureSettings.iExpectedCode = KErrNotSupported;

	//behaviourTest->TestBehaviour(KTestDocument, aPassOrFailureSettings);



	CleanupStack::PopAndDestroy(behaviourTest);

	__UHEAP_MARKEND;
	}


static void MainL()
	{
	BehaviourTestsL();
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

