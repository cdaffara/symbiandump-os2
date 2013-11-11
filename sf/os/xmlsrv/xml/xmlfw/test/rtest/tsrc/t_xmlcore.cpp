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
#include <ecom/ecom.h>
#include <xml/contentprocessor.h>
#include <xml/xmlframeworkerrors.h>
#include <xml/wbxmlextensionhandler.h>
#include <xml/parser.h>
#include <xml/stringdictionarycollection.h>
#include <xml/documentparameters.h>

#include "t_testconstants.h"

_LIT8(KDictionaryUri, "-//TestStringDictionary//DTD TestStringDictionary 1.1//EN~0");
_LIT8(KParserMimeType, "text/testparser");

const TUid KValidatorImplUid		= {0x101FAA11};
const TUid KNonExistantValidator	= {0x101FD197};

using namespace Xml;

class RCoreTests : public RTest, public MWbxmlExtensionHandler, public MContentHandler
	{
public:
	RCoreTests();

	void RunTest();

	// From MContentHandler
	void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode);
	void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttribute, TInt aErrorCode);
	void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode) ;
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);

	void OnError(TInt aErrorCode);


	// From MWbxmlExtensionHandler
	void OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode);

private:
	void RunTestL();

	typedef void (*TestFunc) (void);

	void DoTestL(TestFunc testFuncL, const TDesC& aDesc);
	void DoOomTest(TestFunc testFuncL, const TDesC& aDesc);

	};


RCoreTests::RCoreTests():
	RTest(_L("XML Framework Core"))
	{
	}

// Core framework tests
void SimpleL();
void NoProcessorsL();
void OneProcessorL();
void ManyProcessorsL();
void BadDictionaryL();
void BadParserL();
void BadChainL();
void ParserL();
void EmptyChunkL();
void TagInfoL();
void TagInfo2L();
void AttributeL();
void Attribute2L();
void Attribute3L();
void DocumentParamsL();
void DocumentParams2L();

void RCoreTests::RunTestL()
	{
	DoTestL(SimpleL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1255 Simple test... "));
	DoTestL(NoProcessorsL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1256 Chain with no processors... "));
	DoTestL(OneProcessorL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1257 Chain with 1 processor... "));
	DoTestL(ManyProcessorsL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1258 Chain with many processors... "));
	DoTestL(BadDictionaryL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1259 Non-existing dictionary... "));
	DoTestL(BadParserL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1260 Non-existing parser->.. "));
	DoTestL(BadChainL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1261 Non-existing processors... "));
	DoTestL(ParserL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1262 Parser test... "));
	DoTestL(EmptyChunkL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1263 Parsing an empty chunk... "));
	DoTestL(TagInfoL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1264 RTagInfo... "));
	DoTestL(TagInfo2L, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1265 More RTagInfo... "));
	DoTestL(AttributeL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1266 RAttribute... "));
	DoTestL(Attribute2L, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1267 More RAttribute... "));
	DoTestL(Attribute3L, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1268 Even more RAttribute... "));
	DoTestL(DocumentParamsL, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1269 RDocumentParameters... "));
	DoTestL(DocumentParams2L, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1270 More RDocumentParameters... "));
	}

void RCoreTests::RunTest()
	{
	Title();
	Start(_L("XML Framework Core"));

	TRAPD(err,RunTestL());

	if (err != KErrNone)
		User::Panic(_L("Testing failed: "),err);

	//Getch();

	End();
	Close();
	}

RCoreTests test;

void RCoreTests::DoTestL(TestFunc aTestFuncL, const TDesC& aDesc)
	{
	Next(aDesc);

	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	(*aTestFuncL)();

	REComSession::FinalClose(); // Don't want leaks outside the test

	//--------------
	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount == endThreadHandleCount);

	__UHEAP_MARKEND;

	DoOomTest(aTestFuncL, aDesc);

	}


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3733
@SYMTestCaseDesc		    Out of memory testing.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Running the test set in low memory conditions.
@SYMTestExpectedResults 	The tests perform as expected in low memory conditions.
@SYMREQ 		 		 	REQ0000
*/
void RCoreTests::DoOomTest(TestFunc testFuncL, const TDesC& /*aDesc*/)
	{
	Printf(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3733    OOM test "));

	TInt err, tryCount = 0;
	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++tryCount);
		User::__DbgMarkStart(RHeap::EUser);
		TRAP(err, (*testFuncL)());
		User::__DbgMarkEnd(RHeap::EUser, 0);
		} while(err==KErrNoMemory);

	if(err==KErrNone)
		{
		// Reset
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::ENone,1);
		}
	else
		{
		User::Panic(_L("Unexpected leave reason"),err);
		}

	Printf(_L("- succeeded with rate %i\r\n"), tryCount);
	}



// From MContentHandler

// maps to SAX 2.0 startDocument method.
void RCoreTests::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnStartDocument);
	}


	// maps to SAX 2.0 endDocument method.
void RCoreTests::OnEndDocumentL(TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnEndDocument);
	}


// maps to SAX 2.0 startElement method.
void RCoreTests::OnStartElementL(const RTagInfo& /*aElement*/, const RAttributeArray& /*aAttribute*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnStartElement);
	}


// maps to SAX 2.0 endElement method.
void RCoreTests::OnEndElementL(const RTagInfo& /*aElement*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnEndElement);
	}


// maps to SAX 2.0 characters method.
void RCoreTests::OnContentL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnContent);
	}


// maps to SAX 2.0 startPrefixMapping method.
void RCoreTests::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnStartPrefixMapping);
	}


// maps to SAX 2.0 endPrefixMapping method.
void RCoreTests::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnEndPrefixMapping);
	}


// maps to SAX 2.0 ignorableWhitespace method.
void RCoreTests::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnIgnorableWhiteSpace);
	}


// maps to SAX 2.0 skippedEntity method.
void RCoreTests::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnSkippedEntity);
	}


// maps to SAX 2.0 processingInstruction method.
void RCoreTests::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnProcessingInstruction);
	}


// NOTE: SAX 2.0 setDocumentLocator method not supported.

void RCoreTests::OnExtensionL(const RString& /*aData*/, TInt /*aToken*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrorCodeOnExtension);
	}



void RCoreTests::OnError(TInt aErrorCode)
	{
	test (aErrorCode == KErrEof);
	}


TAny* RCoreTests::GetExtendedInterface(const TInt32 aUid)
/**
This method obtains the interface matching the specified uid.
@return				0 if no interface matching the uid is found.
					Otherwise, the this pointer cast to that interface.
@param				aUid the uid identifying the required interface.
*/
	{
	if (aUid == MWbxmlExtensionHandler::EExtInterfaceUid)
		{
		return static_cast<MWbxmlExtensionHandler*>(this);
		}
	return 0;
	}


//----------------------------------------------------------------------------
//Basic Framework Tests

/**
@SYMTestCaseID          SYSLIB-XML-CT-1255
@SYMTestCaseDesc	    Tests for XML parser CParser::AddPreloadedDictionaryL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for loading a string dictionary prior to parsing.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void SimpleL()
	{
	
	CParser* parser = CParser::NewLC(KParserMimeType, test);
	parser->AddPreloadedDictionaryL(KDictionaryUri);
	CleanupStack::PopAndDestroy(parser);
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1256
@SYMTestCaseDesc	    Tests for XML parser CParser::AddPreloadedDictionaryL(),CParser::SetProcessorChainL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for changing the client and plugin chain,loading a string dictionary prior to parsing.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void NoProcessorsL()
	{
	
	CParser* parser = CParser::NewLC(KParserMimeType, test);

	RContentProcessorUids pUids;
	CleanupClosePushL(pUids);

	parser->SetProcessorChainL(pUids);
	parser->AddPreloadedDictionaryL(KDictionaryUri);

	CleanupStack::PopAndDestroy(&pUids);
	CleanupStack::PopAndDestroy(parser);
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1257
@SYMTestCaseDesc	    Tests for XML parser CParser::AddPreloadedDictionaryL,SetProcessorChainL functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for changing the client and plugin chain,loading a string dictionary prior to parsing.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void OneProcessorL()
	{
	
	RContentProcessorUids pUids;
	CleanupClosePushL(pUids);

	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	CParser* parser = CParser::NewLC(KParserMimeType, test);
	parser->SetProcessorChainL(pUids);
	parser->AddPreloadedDictionaryL(KDictionaryUri);

	CleanupStack::PopAndDestroy(parser);
	CleanupStack::PopAndDestroy(&pUids);
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1258
@SYMTestCaseDesc	    Tests for XML parser CParser::AddPreloadedDictionaryL(),CParser::SetProcessorChainL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for changing the client and plugin chain,loading a string dictionary prior to parsing.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void ManyProcessorsL()
	{
	
	RContentProcessorUids pUids;
	CleanupClosePushL(pUids);

	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));

	CParser* parser = CParser::NewLC(KParserMimeType, test);
	parser->SetProcessorChainL(pUids);
	parser->AddPreloadedDictionaryL(KDictionaryUri);

	CleanupStack::PopAndDestroy(parser);
	CleanupStack::PopAndDestroy(&pUids);
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1259
@SYMTestCaseDesc	    Tests for XML parser CParser::AddPreloadedDictionaryL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for loading a bad string dictionary prior to parsing.
                        Tests for memory error and non availability of XML string dictionary plugin
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void BadDictionaryL()
	{
	
	_LIT8(KNonexistantDictionaryUri, "NONEXISTANT:NONEXISTANT1.0");
	CParser* parser = CParser::NewLC(KParserMimeType, test);
	TRAPD(err, parser->AddPreloadedDictionaryL(KNonexistantDictionaryUri));
	if (err == KErrNoMemory)
		{
		User::Leave(err);
		}
	test(err == KErrXmlStringDictionaryPluginNotFound);

	CleanupStack::PopAndDestroy(parser);
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1260
@SYMTestCaseDesc	    Tests for bad XML parser CParser
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for noexistant parser.Check for out of memory and pulgin not found errors.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void BadParserL()
	{
	
	CParser* parser = NULL;

	_LIT8(KNonexistantParserMimeType, "NONEXISTANT PARSER MIME TYPE");
	TRAPD(err, parser = CParser::NewLC(KNonexistantParserMimeType, test));

	// For OOM tests
	if (err == KErrNoMemory)
		{
		User::Leave(err);
		}

	test(err == KErrXmlParserPluginNotFound);

	if(err == KErrNone)
		{
		CleanupStack::PopAndDestroy(parser);
		}
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1261
@SYMTestCaseDesc	    Tests for bad client and plugin chain.CParser::SetProcessorChainL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for out of memory,plugin not found errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void BadChainL()
	{
	
	RContentProcessorUids pUids;
	CleanupClosePushL(pUids);

	User::LeaveIfError(pUids.Append(KNonExistantValidator));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));

	CParser* parser = CParser::NewLC(KParserMimeType, test);
	TRAPD(err, parser->SetProcessorChainL(pUids));

	// For OOM tests
	if (err == KErrNoMemory)
		{
		User::Leave(KErrNoMemory);
		}

	test (err == KErrXmlPluginNotFound);

	CleanupStack::PopAndDestroy(parser);
	CleanupStack::PopAndDestroy(&pUids);
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1262
@SYMTestCaseDesc	    Tests for loading new XML parser .
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for CParser::NewLC() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void ParserL()
	{
	
	CParser* parser = CParser::NewLC(KParserMimeType, test);
	CleanupStack::PopAndDestroy(parser);
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1263
@SYMTestCaseDesc	    Tests for XML parser
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for empty chunk to parse
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void EmptyChunkL()
	{
	
	RContentProcessorUids pUids;
	CleanupClosePushL(pUids);

	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	User::LeaveIfError(pUids.Append(KValidatorImplUid));
	CParser* parser = CParser::NewLC(KParserMimeType, test);
	parser->SetProcessorChainL(pUids);
	parser->AddPreloadedDictionaryL(KDictionaryUri);

	TBuf8<10> nothing;
	ParseL(*parser, nothing);

	CleanupStack::PopAndDestroy(parser);
	CleanupStack::PopAndDestroy(&pUids);
	}

_LIT8 (KTestString1, "This is a test string1.");
_LIT8 (KTestString2, "This is a test string2.");
_LIT8 (KTestString3, "This is a test string3.");
_LIT8 (KTestString4, "This is a test string4.");

/**
@SYMTestCaseID          SYSLIB-XML-CT-1264
@SYMTestCaseDesc	    Tests for RTagInfo::Uri(),RTagInfo::Prefix(),RTagInfo::LocalName() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for creating a tag info with three strings.
                        Read them back and test for integrity of the data.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TagInfoL()
	{
	
	RStringPool sp;
	CleanupClosePushL(sp);
	sp.OpenL();

	RString string1 = sp.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = sp.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = sp.OpenStringL(KTestString3);

	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&sp);

	RTagInfo tagInfo;
	tagInfo.Open(string1, string2, string3);

	const RString& uri = tagInfo.Uri();
	const RString& prefix = tagInfo.Prefix();
	const RString& localName = tagInfo.LocalName();

	test (KTestString1() == uri.DesC());
	test (KTestString2() == prefix.DesC());
	test (KTestString3() == localName.DesC());

	tagInfo.Close();
	sp.Close();
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1265
@SYMTestCaseDesc	    Tests for RTagInfo's constructor functions
@SYMTestPriority 	    High
@SYMTestActions  	    Create three RTagInfo objects using assignment operator,copy constructor,and Copy function with the same information that describes an element.
                        Tests for the integrity of the data of all the three objects.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TagInfo2L()
	{
	
	RStringPool sp;
	CleanupClosePushL(sp);
	sp.OpenL();

	RString string1 = sp.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = sp.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = sp.OpenStringL(KTestString3);

	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&sp);

	RTagInfo tagInfo;
	tagInfo.Open(string1, string2, string3);
	RTagInfo tagInfo2 = tagInfo;
	RTagInfo tagInfo3 (tagInfo);
	RTagInfo tagInfo4 = tagInfo.Copy();

	const RString& uri = tagInfo.Uri();
	const RString& prefix = tagInfo.Prefix();
	const RString& localName = tagInfo.LocalName();

	test (KTestString1() == uri.DesC());
	test (KTestString2() == prefix.DesC());
	test (KTestString3() == localName.DesC());

	test (KTestString1() == tagInfo2.Uri().DesC());
	test (KTestString2() == tagInfo2.Prefix().DesC());
	test (KTestString3() == tagInfo2.LocalName().DesC());

	test (KTestString1() == tagInfo3.Uri().DesC());
	test (KTestString2() == tagInfo3.Prefix().DesC());
	test (KTestString3() == tagInfo3.LocalName().DesC());

	test (KTestString1() == tagInfo4.Uri().DesC());
	test (KTestString2() == tagInfo4.Prefix().DesC());
	test (KTestString3() == tagInfo4.LocalName().DesC());

	tagInfo.Close();
	tagInfo4.Close();
	sp.Close();
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1266
@SYMTestCaseDesc	    Tests for RTagInfo created from RAttribute class
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the information of RTagInfo objects
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void AttributeL()
	{
	
	RStringPool sp;
	sp.OpenL();
	CleanupClosePushL(sp);

	RString string1 = sp.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = sp.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = sp.OpenStringL(KTestString3);
	CleanupClosePushL(string3);

	RString string4 = sp.OpenStringL(KTestString4);

	CleanupStack::Pop(&string3);
	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&sp);

	RAttribute attribute;
	attribute.Open(string1, string2, string3);
	attribute.SetValue(string4);

	const RTagInfo& tagInfo = attribute.Attribute();
	const RString& value = attribute.Value();
	TAttributeType type = attribute.Type();

	test (KTestString1() == tagInfo.Uri().DesC());
	test (KTestString2() == tagInfo.Prefix().DesC());
	test (KTestString3() == tagInfo.LocalName().DesC());
	test (KTestString4() == value.DesC());
	test (EAttributeType_NONE == type);

	attribute.Close();
	sp.Close();
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1267
@SYMTestCaseDesc	    Tests for RTagInfo created from RAttribute::Uri(),Prefix(),LocalName() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Create three RAttribute objects using copy constructor,assignment operator and Copy function.Test for information of RAttribute objects
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Attribute2L()
	{
	
	RStringPool sp;
	sp.OpenL();
	CleanupClosePushL(sp);

	RString string1 = sp.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = sp.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = sp.OpenStringL(KTestString3);
	CleanupClosePushL(string3);

	RString string4 = sp.OpenStringL(KTestString4);

	CleanupStack::Pop(&string3);
	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&sp);

	RAttribute attribute;
	attribute.Open(string1, string2, string3, string4);
	RAttribute attribute2 = attribute;
	RAttribute attribute3 (attribute);
	RAttribute attribute4 = attribute.Copy();

	const RTagInfo& tagInfo = attribute.Attribute();
	const RString& value = attribute.Value();
	TAttributeType type = attribute.Type();

	test (KTestString1() == tagInfo.Uri().DesC());
	test (KTestString2() == tagInfo.Prefix().DesC());
	test (KTestString3() == tagInfo.LocalName().DesC());
	test (KTestString4() == value.DesC());
	test (EAttributeType_NONE == type);

	test (KTestString1() == attribute2.Attribute().Uri().DesC());
	test (KTestString2() == attribute2.Attribute().Prefix().DesC());
	test (KTestString3() == attribute2.Attribute().LocalName().DesC());
	test (KTestString4() == attribute2.Value().DesC());
	test (EAttributeType_NONE == attribute2.Type());

	test (KTestString1() == attribute3.Attribute().Uri().DesC());
	test (KTestString2() == attribute3.Attribute().Prefix().DesC());
	test (KTestString3() == attribute3.Attribute().LocalName().DesC());
	test (KTestString4() == attribute3.Value().DesC());
	test (EAttributeType_NONE == attribute3.Type());

	test (KTestString1() == attribute4.Attribute().Uri().DesC());
	test (KTestString2() == attribute4.Attribute().Prefix().DesC());
	test (KTestString3() == attribute4.Attribute().LocalName().DesC());
	test (KTestString4() == attribute4.Value().DesC());
	test (EAttributeType_NONE == attribute4.Type());

	attribute.Close();
	attribute4.Close();
	sp.Close();
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1268
@SYMTestCaseDesc	    Tests for RTagInfo created from RAttribute::Uri,Prefix,LocalName functions
@SYMTestPriority 	    High
@SYMTestActions  	    Create RAttribute Test for information of RAttribute objects
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Attribute3L()
	{
	
	RStringPool sp;
	sp.OpenL();
	CleanupClosePushL(sp);

	RString string1 = sp.OpenStringL(KTestString1);
	CleanupClosePushL(string1);

	RString string2 = sp.OpenStringL(KTestString2);
	CleanupClosePushL(string2);

	RString string3 = sp.OpenStringL(KTestString3);
	CleanupClosePushL(string3);

	RString string4 = sp.OpenStringL(KTestString4);

	CleanupStack::Pop(&string3);
	CleanupStack::Pop(&string2);
	CleanupStack::Pop(&string1);
	CleanupStack::Pop(&sp);

	RAttribute attribute;
	attribute.Open(string1, string2, string3, string4);

	const RTagInfo& tagInfo = attribute.Attribute();
	const RString& value = attribute.Value();
	TAttributeType type = attribute.Type();

	test (KTestString1() == tagInfo.Uri().DesC());
	test (KTestString2() == tagInfo.Prefix().DesC());
	test (KTestString3() == tagInfo.LocalName().DesC());
	test (KTestString4() == value.DesC());
	test (EAttributeType_NONE == type);

	attribute.Close();
	sp.Close();
	}

_LIT8 (KCharSetName, "Test Charset name.");

/**
@SYMTestCaseID          SYSLIB-XML-CT-1269
@SYMTestCaseDesc	    Tests for information of the document to be passed to client RDocumentParameters::CharacterSetName() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the character set name of document
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DocumentParamsL()
	{
	
	RStringPool sp;
	sp.OpenL();
	CleanupClosePushL(sp);

	RString characterSetName = sp.OpenStringL(KCharSetName);

	CleanupStack::Pop(&sp);

	RDocumentParameters docParam;
	docParam.Open(characterSetName);

	test (KCharSetName() == docParam.CharacterSetName().DesC());

	docParam.Close();
	sp.Close();
	}

/**
@SYMTestCaseID          SYSLIB-XML-CT-1270
@SYMTestCaseDesc	    Tests for information of the document to be passed to client RDocumentParameters::CharacterSetName() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the character set name of document
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DocumentParams2L()
	{
	
	RStringPool sp;
	sp.OpenL();
	CleanupClosePushL(sp);

	RString characterSetName = sp.OpenStringL(KCharSetName);
	CleanupStack::Pop(&sp);

	RDocumentParameters docParam;
	docParam.Open(characterSetName);

	test (KCharSetName() == docParam.CharacterSetName().DesC());

	docParam.Close();
	sp.Close();
	}




TInt E32Main()
    {
	CTrapCleanup* tc=CTrapCleanup::New();

	__UHEAP_MARK;
	test.RunTest();
	__UHEAP_MARKEND;

	delete tc;
	User::Heap().Check();
	return 0;
    }

