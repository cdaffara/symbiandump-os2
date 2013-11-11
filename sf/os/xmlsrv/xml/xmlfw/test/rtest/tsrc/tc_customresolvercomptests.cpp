// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of Component Tests for CR JROE6C2LB7.
// Those tests are based on the parser plugins provided with Symbian OS
// and test parsers written for this purpose:
// 1. XML parser - production code (mime type: text/xml, variant ID: Symbian)
// 2. WBXML parser - production code (mime type: text/wbxml, variant ID: Symbian)
// 3. XML/WBXML parser - test code (mime type: text/xml||text/wbxml, variant ID: SymbianExtended)
// 4. XML parser - test code (mime type: text/xml, variant ID: SymbianExtended)
// 5  XML parser - test code (mime type: text/testparser, variant ID: )
// If the number of Symbian production parsers is changed,
// those test could require necessary updates.
// 
//

/**
 @file
 @internalComponent
*/

#include <e32test.h>
#include <f32file.h>
#include <ecom/ecom.h>

#include <xml/matchdata.h>
#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <xml/xmlframeworkerrors.h>

#include "contenthandlers.h"

using namespace Xml;

_LIT(KXmlTestSimpleTestFile,    "c:\\System\\XmlTest\\Resolver\\simple.xml");
_LIT8(KXmlTestParserMimeType, "text/xml");
_LIT8(KXmlTestParserMimeTypeCS, "TEXT/XML");
_LIT8(KXmlTestWbXmlParserMimeType, "text/wbxml");
_LIT8(KXmlTestSymbianString, "Symbian");
_LIT8(KXmlTestSymbianStringCS, "SYMBIAN");
_LIT8(KXmlTestSymbianExtendedString, "SymbianExtended");
_LIT8(KXmlTestTestParser, "text/testparser");

RTest test(_L("t_XmlCustomResolverTests"));

//=========================================================================
// Class declaration

class CXmlCustomResolverTests : public CBase
{
public:
	CXmlCustomResolverTests();

public:
	// The tests to run
	void Test1L(RFs& aRFs);
	void Test2L();
	void Test3L(RFs& aRFs);
	void Test4L(RFs& aRFs);
	void Test5L();
	void Test6L();
	void Test7L();
	void Test8L();
	void Test9L(RFs& aRFs);
	void Test10L(RFs& aRFs);
	void Test11L();
	void Test12L();
	void Test13L(RFs& aRFs);
	void Test14L(RFs& aRFs);
};



//=========================================================================
// Class constructor
CXmlCustomResolverTests::CXmlCustomResolverTests()
	{
	}

//=========================================================================

/**
Test Xmlparser creation - 1.
MimeType: 			text/xml
Variant: 			NULL
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		FALSE

Expected:			handler->iNumElements > 0
					Any XML parser

@SYMTestCaseID 			 	SYSLIB-XML-CT-1377
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test1L(RFs& aRFs)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1377 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestParserMimeType);

	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	CParser* parser = NULL;
	parser = CParser::NewL(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements>0);
	delete parser; parser = NULL;

	// NewLC test
	handler->iNumElements = 0;
	parser = CParser::NewLC(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements>0);

	delete handler;
	CleanupStack::PopAndDestroy(2, matchData);
	}


/**
Test Xmlparser creation - 2.
MimeType: 			text/xml
Variant: 			NULL
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		TRUE

Expected:			leave, KErrXmlMoreThanOneParserMatched

@SYMTestCaseID 		 		SYSLIB-XML-CT-1378
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1378 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestParserMimeType);
	matchData->SetLeaveOnMany(ETrue);

	// NewL test
	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();
	TRAPD(err, CParser::NewL(*matchData, *handler));
	test(err == KErrXmlMoreThanOneParserMatched);

	// NewLC test
	TRAP(err, CParser::NewLC(*matchData, *handler));
	test(err == KErrXmlMoreThanOneParserMatched);

	delete handler;
	CleanupStack::PopAndDestroy(matchData);
	}

/**
Test Xmlparser creation - 3.
MimeType: 			text/xml
Variant: 			Symbian
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		FALSE

Expected:			handler->iNumElements == 6
					Production, Symbian parser.

@SYMTestCaseID 		 		SYSLIB-XML-CT-1379
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test3L(RFs& aRFs)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1379 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestParserMimeType);
	matchData->SetVariantL(KXmlTestSymbianString);

	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL
	CParser* parser = CParser::NewL(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements==6);
	delete parser; parser = NULL;

	// NewLC
	parser = CParser::NewLC(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements==6);

	delete handler;
	CleanupStack::PopAndDestroy(2, matchData);
	}

/**
Test Xmlparser creation - 4.
MimeType: 			text/xml
Variant: 			SymbianExtended
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		FALSE

Expected:			handler->iNumElements == 1 || handler->iNumElements == 2
					Either of two test parsers.

@SYMTestCaseID 			 	SYSLIB-XML-CT-1380
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test4L(RFs& aRFs)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1380 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestParserMimeType);
	matchData->SetVariantL(KXmlTestSymbianExtendedString);
	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	CParser* parser = CParser::NewL(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements==1 || handler->iNumElements==2 );
	delete parser; parser = NULL;

	// NewLC test
	parser = CParser::NewLC(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements==1 || handler->iNumElements==2 );

	delete handler;
	CleanupStack::PopAndDestroy(2, matchData);
	}

/**
Test Xmlparser createion - 5.
MimeType: 			text/xml
Variant: 			SymbianExtended
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		TRUE

Expected:			leave, KErrXmlMoreThanOneParserMatched

@SYMTestCaseID 			 	SYSLIB-XML-CT-1381
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test5L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1381 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestParserMimeType);
	matchData->SetVariantL(KXmlTestSymbianExtendedString);
	matchData->SetLeaveOnMany(ETrue);

	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	TRAPD(err, CParser::NewL(*matchData, *handler));
	test (err == KErrXmlMoreThanOneParserMatched);

	// NewLC test
	TRAP(err, CParser::NewLC(*matchData, *handler));
	test (err == KErrXmlMoreThanOneParserMatched);

	delete handler;
	CleanupStack::PopAndDestroy(matchData);
	}

/**
Test Xmlparser createion - 6.
MimeType: 			text/wbxml
Variant: 			NULL
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		FALSE

Expected:			any of wbxml parsers

@SYMTestCaseID 			 	SYSLIB-XML-CT-1382
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test6L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1382 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestWbXmlParserMimeType);
	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	CParser* parser = CParser::NewL(*matchData, *handler);
	delete parser; parser = NULL;

	// NewLC test
	parser = CParser::NewLC(*matchData, *handler);

	delete handler;
	CleanupStack::PopAndDestroy(2, matchData);
	}

/**
Test Xmlparser createion - 7.
MimeType: 			text/wbxml
Variant: 			NULL
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		TRUE

Expected:			leave, KErrXmlMoreThanOneParserMatched

@SYMTestCaseID 		 		SYSLIB-XML-CT-1383
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test7L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1383 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestWbXmlParserMimeType);
	matchData->SetLeaveOnMany(ETrue);

	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	TRAPD(err, CParser::NewL(*matchData, *handler));
	test(err == KErrXmlMoreThanOneParserMatched);

	// NewLC test
	TRAP(err, CParser::NewLC(*matchData, *handler));
	test(err == KErrXmlMoreThanOneParserMatched);

	delete handler;
	CleanupStack::PopAndDestroy(matchData);
	}

/**
Test Xmlparser createion - 8.
MimeType: 			TEXT/XML
Variant: 			NULL
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		FALSE

Expected:			leave, KErrXmlParserPluginNotFound

@SYMTestCaseID 		 		SYSLIB-XML-CT-1384
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test8L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1384 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestParserMimeTypeCS);

	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	TRAPD(err, CParser::NewL(*matchData, *handler));
	test(err == KErrXmlParserPluginNotFound);

	// NewLC test
	TRAP(err, CParser::NewLC(*matchData, *handler));
	test(err == KErrXmlParserPluginNotFound);

	delete handler;
	CleanupStack::PopAndDestroy(matchData);
	}

/**
Test Xmlparser createion - 9.
MimeType: 			TEXT/XML
Variant: 			NULL
CaseSensitivity: 	FALSE
Rom-Only: 			FALSE
LeaveOnMany: 		FALSE

Expected:			handler->iNumElements > 0
					Any XML parser

@SYMTestCaseID 			 	SYSLIB-XML-CT-1385
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test9L(RFs& aRFs)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1385 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestParserMimeTypeCS);
	matchData->SetCaseSensitivity(EFalse);

	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	CParser* parser = CParser::NewL(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements > 0);
	delete parser; parser = NULL;

	// NewLC test
	parser = CParser::NewLC(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements > 0);

	delete handler;
	CleanupStack::PopAndDestroy(2, matchData);
	}

/**
Test Xmlparser createion - 10.
MimeType: 			TEXT/XML
Variant: 			SYMBIAN
CaseSensitivity:	FALSE
Rom-Only: 			FALSE
LeaveOnMany: 		FALSE

Expected:			handler->iNumElements > 0
					Any XML parser

@SYMTestCaseID 		 		SYSLIB-XML-CT-1386
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test10L(RFs& aRFs)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1386 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestParserMimeTypeCS);
	matchData->SetVariantL(KXmlTestSymbianStringCS);
	matchData->SetCaseSensitivity(EFalse);

	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	CParser* parser = CParser::NewL(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements == 6);
	delete parser; parser = NULL;

	// NewLC test
	parser = CParser::NewLC(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements == 6);

	delete handler;
	CleanupStack::PopAndDestroy(2, matchData);
	}

/**
Test Xmlparser createion - 11.
MimeType: 			TEXT/XML
Variant: 			SYMBIAN
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		FALSE

Expected:			handler->iNumElements > 0
					Any XML parser

@SYMTestCaseID 		 		SYSLIB-XML-CT-3752
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test11L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3752 "));

	CMatchData* matchData = CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlTestParserMimeTypeCS);
	matchData->SetVariantL(KXmlTestSymbianStringCS);
	matchData->SetCaseSensitivity(ETrue);

	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	TRAPD(err, CParser::NewL(*matchData, *handler));
	test(err == KErrXmlParserPluginNotFound);

	// NewLC test
	TRAP(err, CParser::NewLC(*matchData, *handler));
	test(err == KErrXmlParserPluginNotFound);

	delete handler;
	CleanupStack::PopAndDestroy(matchData);
	}

/**
Test Xmlparser creation - 12.
MimeType: 			NULL
Variant: 			NULL
CaseSensitivity: 	TRUE
Rom-Only: 			FALSE
LeaveOnMany: 		FALSE

Expected:			Leave, KErrArgument - mime type mandatory

@SYMTestCaseID 		 	SYSLIB-XML-CT-3753
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test12L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3753 "));

	CMatchData* matchData = CMatchData::NewLC();
	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// NewL test
	TRAPD(err, CParser::NewL(*matchData, *handler));
	test(err == KErrArgument);

	// NewLC test
	TRAP(err, CParser::NewLC(*matchData, *handler));
	test(err == KErrArgument);

	delete handler;
	CleanupStack::PopAndDestroy(matchData);

	}

/**
Test Xmlparser creation - 13.
MimeType: 		text/testparser
Variant: 			NULL
CaseSensitivity: 	TRUE
Rom-Only: 		FALSE
LeaveOnMany: 	FALSE

Expected:	handler->iNumPrefixMappings should be equal to 1

@SYMTestCaseID 		 		SYSLIB-XML-CT-1411
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test13L(RFs& aRFs)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1411 "));

	CMatchData* matchData = CMatchData::NewLC();
	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	matchData->SetMimeTypeL(KXmlTestTestParser);
	CParser* parser = CParser::NewL(*matchData, *handler);
	TRAPD(err, ParseL(*parser, aRFs, KXmlTestSimpleTestFile));
	test(err == KErrXmlUnsupportedExtInterface);

	delete parser;
	delete handler;
	CleanupStack::PopAndDestroy(matchData);
	}

/**
Test Xmlparser swap test - 14.
This method is testing parser swapping using ParseBeginL method.
Test steps:
 1. Get xml test parser
 	Test for 1 or 2 events generated.

 2. Swap to xml production parser
 	Test for 6 events genrated.

 3. Swap to wbxml production parser
 	Test for no errors.

 4. Swap to xml production parser
 	Test for 6 events gerated.

@SYMTestCaseID 		 		SYSLIB-XML-CT-1412
@SYMTestCaseDesc
@SYMTestPriority 		    Medium
@SYMTestActions
@SYMTestExpectedResults
@SYMCR 		 		 		CR0000
*/
void CXmlCustomResolverTests::Test14L(RFs& aRFs)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1412 "));

	CMatchData* matchData = CMatchData::NewLC();
	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();

	// Get the tests parser of xml
	matchData->SetMimeTypeL(KXmlTestParserMimeType);
	matchData->SetVariantL(KXmlTestSymbianExtendedString);
	CParser* parser = CParser::NewL(*matchData, *handler);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements == 1 || handler->iNumElements == 2);

	// Try to swap parsers to production xml
	matchData->SetVariantL(KXmlTestSymbianString);
	handler->iNumElements = 0;
	parser->ParseBeginL(*matchData);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements == 6);

	// Try to swap parser to wbxml production parser
	matchData->SetMimeTypeL(KXmlTestWbXmlParserMimeType);
	TRAPD(err, parser->ParseBeginL(*matchData));
	test(err == KErrNone);

	// Try to swap again to test xml parser
	matchData->SetMimeTypeL(KXmlTestParserMimeType);
	matchData->SetVariantL(KXmlTestSymbianExtendedString);
	parser->ParseBeginL(*matchData);
	ParseL(*parser, aRFs, KXmlTestSimpleTestFile);
	test(handler->iNumElements == 1 || handler->iNumElements == 2);

	delete parser;
	delete handler;
	CleanupStack::PopAndDestroy(matchData);
	}


//========================================================================

static void XmlCustomResolverTestsL()
	{
	CXmlCustomResolverTests *customResolverTests = new (ELeave) CXmlCustomResolverTests();
	CleanupStack::PushL(customResolverTests);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	customResolverTests->Test1L(fs);
	customResolverTests->Test2L();
	customResolverTests->Test3L(fs);
	customResolverTests->Test4L(fs);
	customResolverTests->Test5L();
	customResolverTests->Test6L();
	customResolverTests->Test7L();
	customResolverTests->Test8L();
	customResolverTests->Test9L(fs);
	customResolverTests->Test10L(fs);
	customResolverTests->Test11L();
	customResolverTests->Test12L();
	customResolverTests->Test13L(fs);
	customResolverTests->Test14L(fs);
	CleanupStack::PopAndDestroy(2, customResolverTests);
	}

static void MainL()
	{
	__UHEAP_MARK;
	XmlCustomResolverTestsL();
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	}

TInt E32Main()
	{
	test.SetLogged(ETrue);
	test.Title();
	test.Start(_L("XML Framework enhancement tests"));

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

	return KErrNone;
	}



