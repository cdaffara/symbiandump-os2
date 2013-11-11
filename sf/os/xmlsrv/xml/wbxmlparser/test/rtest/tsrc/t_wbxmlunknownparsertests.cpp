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
_LIT8 (KStringDictionaryFD1, "FD1~0");
_LIT8 (KStringDictionaryFD3, "FD3~0");
_LIT8 (KStringDictionarySML10, "SYNCML:SYNCML1.0");
_LIT8 (KStringDictionarySML11, "SYNCML:SYNCML1.1");
_LIT8 (KStringDictionaryDTDSML10, "-//SYNCML//DTD SyncML 1.0//EN~0");
_LIT8 (KStringDictionaryDTDSML11, "-//SYNCML//DTD SyncML 1.1//EN~0");

_LIT  (KTestDocument, "c:\\system\\XmlTest\\Wml\\Unknown\\UnknownPubId_AllElements.wmlc");
_LIT  (KTestDocument1, "c:\\system\\XmlTest\\SyncML\\Unknown\\DEF078987_3.wbxml");

static RTest test(_L("t_wbxmlunknownparsertests"));


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3721
@SYMTestCaseDesc		    Testing wbxml parser
@SYMTestPriority 		    Medium
@SYMTestActions  		    Parses a doucment and behaviour should be as expected.
@SYMTestExpectedResults 	Test should not fail.
@SYMREQ 		 		 	REQ0000
*/
static void BehaviourTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3721 BehaviourTestsL tests... "));

	__UHEAP_MARK;

	TPassOrFailureSettings aPassOrFailureSettings;

	aPassOrFailureSettings.iDoParseDocument = ETrue;
	aPassOrFailureSettings.iFilenameProvided = ETrue;
	aPassOrFailureSettings.iParseMode = EErrorOnUnrecognisedTags|
										ERawContent;


	CStabilityTestClass* behaviourTest = CStabilityTestClass::NewL(test);
	CleanupStack::PushL(behaviourTest);



	// Test 1

	aPassOrFailureSettings.iStringDictionaryUri = &KBlankStringDictionaryUri;
	aPassOrFailureSettings.iExpectedCode = KErrXmlMissingStringDictionary;

	behaviourTest->TestBehaviour(KTestDocument, aPassOrFailureSettings);


	// Test 2

	// Preload Dictionary
	aPassOrFailureSettings.iStringDictionaryUri = &KStringDictionaryUri;
	aPassOrFailureSettings.iExpectedCode = KErrNone;

	behaviourTest->TestBehaviour(KTestDocument, aPassOrFailureSettings);



	CleanupStack::PopAndDestroy(behaviourTest);

	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3722
@SYMTestCaseDesc		    Testing : Convergence: KErrXmlStringDictionaryPluginNotFound when
							syncing with DM server
@SYMTestPriority 		    High
@SYMTestActions  		    It parse the files DEF078987_3.wbxml a which contain the updated
							unknown publicID
@SYMTestExpectedResults 	Test should not fail
@SYMDEF 		 		 	DEF083044
*/
void Defect_DEF083044_TestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3722 SyncML unknown publicID tests... "));

	__UHEAP_MARK;

	TPassOrFailureSettings aPassOrFailureSettings;
	aPassOrFailureSettings.iDoParseDocument = ETrue;
	aPassOrFailureSettings.iFilenameProvided = ETrue;

	CStabilityTestClass* behaviourTest = CStabilityTestClass::NewL(test);
	CleanupStack::PushL(behaviourTest);

	aPassOrFailureSettings.iParseMode = EErrorOnUnrecognisedTags|
										ERawContent;
	aPassOrFailureSettings.iExpectedCode = KErrNone;

	// Preload Dictionary
	aPassOrFailureSettings.iStringDictionaryUri = &KStringDictionaryFD1;
	behaviourTest->TestBehaviour(KTestDocument1, aPassOrFailureSettings);

	aPassOrFailureSettings.iStringDictionaryUri = &KStringDictionaryFD3;
	behaviourTest->TestBehaviour(KTestDocument1, aPassOrFailureSettings);

	aPassOrFailureSettings.iStringDictionaryUri = &KStringDictionarySML10;
	behaviourTest->TestBehaviour(KTestDocument1, aPassOrFailureSettings);

	aPassOrFailureSettings.iStringDictionaryUri = &KStringDictionarySML11;
	behaviourTest->TestBehaviour(KTestDocument1, aPassOrFailureSettings);

	aPassOrFailureSettings.iStringDictionaryUri = &KStringDictionaryDTDSML10;
	behaviourTest->TestBehaviour(KTestDocument1, aPassOrFailureSettings);

	aPassOrFailureSettings.iStringDictionaryUri = &KStringDictionaryDTDSML11;
	behaviourTest->TestBehaviour(KTestDocument1, aPassOrFailureSettings);

	CleanupStack::PopAndDestroy(behaviourTest);

	__UHEAP_MARKEND;
	}


static void MainL()
	{
	Defect_DEF083044_TestsL();
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

