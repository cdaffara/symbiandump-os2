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

_LIT8 (KWmlStringDictionaryUri,"4~0");
_LIT8 (KSiStringDictionaryUri, "5~0");

_LIT  (KTestDocument1, "c:\\system\\XmlTest\\Wml\\Corrupt\\CorruptTag_AllElements.wmlc");
_LIT  (KTestDocument2, "c:\\system\\XmlTest\\Wml\\Corrupt\\CorruptAttr_mob.wmlc");
_LIT  (KTestDocument3, "c:\\system\\XmlTest\\Wml\\Corrupt\\CorruptAttrVal_mob.wmlc");
_LIT  (KTestDocument4, "c:\\system\\XmlTest\\Wml\\Corrupt\\NonNullTermInlineStr.wmlc");

_LIT  (KTestDocument20, "c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_inv_act_undf.bdy");
_LIT  (KTestDocument21, "c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_inv_cd_token.bdy");
_LIT  (KTestDocument22, "c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_inv_cd_type.bdy");
_LIT  (KTestDocument23, "c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_inv_char.bdy");
_LIT  (KTestDocument24, "c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_inv_msg_ter.bdy");
_LIT  (KTestDocument25, "c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_val_wbx_ver_14.bdy");
_LIT  (KTestDocument26,"c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_val_wbx_ver_30.bdy");
_LIT  (KTestDocument27,"c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_val_wbx_ver_FF.bdy");
_LIT  (KTestDocument28,"c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_inv_hrf_ter.bdy");
_LIT  (KTestDocument29,"c:\\system\\XmlTest\\ServiceIndication\\Corrupt\\si_inv_msg_ter2.bdy");


static RTest test(_L("t_WbxmlCorruptParserTests"));


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3731
@SYMTestCaseDesc		    Parsing corrupt files.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Testing the wbxml parser with various corrupt files.
@SYMTestExpectedResults 	Parser should be able to cope with the corrupt files.
@SYMREQ			 		 	REQ0000
*/
static void BehaviourTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3731 BehaviourTestsL tests... "));

	__UHEAP_MARK;

	TPassOrFailureSettings aPassOrFailureSettings;

	aPassOrFailureSettings.iDoParseDocument = ETrue;
	aPassOrFailureSettings.iFilenameProvided = ETrue;
	aPassOrFailureSettings.iParseMode = EErrorOnUnrecognisedTags|
										ERawContent;


	CStabilityTestClass* behaviourTest = CStabilityTestClass::NewL(test);
	CleanupStack::PushL(behaviourTest);



	// Wml Tests

	aPassOrFailureSettings.iStringDictionaryUri = &KWmlStringDictionaryUri;

	aPassOrFailureSettings.iExpectedCode = KErrXmlUnsupportedElement;
	behaviourTest->TestBehaviour(KTestDocument1, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlUnsupportedAttribute;
	behaviourTest->TestBehaviour(KTestDocument2, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlUnsupportedAttributeValue;
	behaviourTest->TestBehaviour(KTestDocument3, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrNone;
	behaviourTest->TestBehaviour(KTestDocument4, aPassOrFailureSettings);


	// Si Tests
	//

	aPassOrFailureSettings.iStringDictionaryUri = &KSiStringDictionaryUri;

	aPassOrFailureSettings.iExpectedCode = KErrXmlDocumentCorrupt;
	behaviourTest->TestBehaviour(KTestDocument20, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlUnsupportedAttributeValue;
	behaviourTest->TestBehaviour(KTestDocument21, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlDocumentCorrupt;
	behaviourTest->TestBehaviour(KTestDocument22, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlUnsupportedCharacterSet;
	behaviourTest->TestBehaviour(KTestDocument23, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlDocumentCorrupt; // truncated doc.
	behaviourTest->TestBehaviour(KTestDocument24, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlUnsupportedDocumentVersion;
	behaviourTest->TestBehaviour(KTestDocument25, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlUnsupportedDocumentVersion;
	behaviourTest->TestBehaviour(KTestDocument26, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlUnsupportedDocumentVersion;
	behaviourTest->TestBehaviour(KTestDocument27, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlDocumentCorrupt; // truncated doc.
	behaviourTest->TestBehaviour(KTestDocument28, aPassOrFailureSettings);


	//

	aPassOrFailureSettings.iExpectedCode = KErrXmlDocumentCorrupt; // truncated doc.
	behaviourTest->TestBehaviour(KTestDocument29, aPassOrFailureSettings);



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


