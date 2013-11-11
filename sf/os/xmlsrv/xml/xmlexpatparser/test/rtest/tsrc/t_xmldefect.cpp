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
//

#include <e32test.h>
#include <ecom/ecom.h>

#include <xml/contentprocessor.h>
#include <xml/documentparameters.h>
#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <xml/stringdictionarycollection.h>
#include <xml/xmlframeworkerrors.h>

#include "unzip.h"

LOCAL_D RTest 				test(_L("t_xmldefect.exe"));

LOCAL_D RFs					TheFs;

LOCAL_D CTrapCleanup* 		TheTrapCleanup 		= NULL;

LOCAL_D TBool				TheCorruptedXmlFileTest = EFalse;

LOCAL_D TInt    			TheCorruptedXmlFileTestErrorCount = 0;

// Installed via bld.inf test exports
_LIT(KSimpleFile, "c:\\system\\data\\xml\\xml\\another_simple.xml");
_LIT(KNsAndBindingFile, "c:\\system\\data\\xml\\xml\\default_namespace_and_unbinding.xml");

_LIT(KZipTestFile, "c:\\system\\data\\xml\\xmldefect.zip");
_LIT(KXmlTestDir, "c:\\system\\data\\xml");


_LIT8(KXmlParserDataType, "text/xml");


typedef void (*TestFunc) (void);


//----------------------------------------------------------------------------
// The Content Handler

using namespace Xml;

class RDefectTests : public MContentHandler
	{
public:
	RDefectTests() {/*do nothing*/};
	virtual ~RDefectTests() {/*do nothing*/};

	void Open() {/*do nothing*/};
	void Close() {/*do nothing*/};

	// From MContentHandler
	void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode);
	void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttribute, TInt aErrorCode);
	void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode) ;
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);

	void OnError(TInt aErrorCode);

	};



// From MContentHandler

// maps to SAX 2.0 startDocument method.
void RDefectTests::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrNone);
	}


	// maps to SAX 2.0 endDocument method.
void RDefectTests::OnEndDocumentL(TInt aErrorCode)
	{
	test(aErrorCode == KErrNone);
	}


// maps to SAX 2.0 startElement method.
void RDefectTests::OnStartElementL(const RTagInfo& /*aElement*/, const RAttributeArray& /*aAttribute*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrNone);
	}


// maps to SAX 2.0 endElement method.
void RDefectTests::OnEndElementL(const RTagInfo& /*aElement*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrNone);
	}


// maps to SAX 2.0 characters method.
void RDefectTests::OnContentL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrNone);
	}


// maps to SAX 2.0 startPrefixMapping method.
void RDefectTests::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt aErrorCode)
	{
	test.Next(_L("Entered OnStartPrefixMappingL"));
	test(aErrorCode == KErrNone);
	}


// maps to SAX 2.0 endPrefixMapping method.
void RDefectTests::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrNone);
	}


// maps to SAX 2.0 ignorableWhitespace method.
void RDefectTests::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrNone);
	}


// maps to SAX 2.0 skippedEntity method.
void RDefectTests::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrNone);
	}


// maps to SAX 2.0 processingInstruction method.
void RDefectTests::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode)
	{
	test(aErrorCode == KErrNone);
	}


// Note that Oom tests between Xml and Wbxml behave differently.
// Xml has a preallocated block of memory allocated upfront that
// it uses to obtain internal memory allocations from.
// In this manner out of memory allocations happen BEFORE parsing.
// Therefore, OnError() is never called due to KErrNoMemory. However,
// the parser will leave and fall into the oom TRAP block.
// With Wbxml there is no such preallocation so an OnError will occur.
void RDefectTests::OnError(TInt aErrorCode)
	{
	//This is done for the test DEF101097L which parses 2 corrupted xml file
	//in a sequence & thus to check if OnError is called for both the files
	//& avoid those errors by checking if the error is not KErrNone.
	if(TheCorruptedXmlFileTest)
	{
		test(aErrorCode != KErrNone);
		++TheCorruptedXmlFileTestErrorCount;
	}
	else
		test (aErrorCode == KErrEof || aErrorCode == KErrNoMemory);
	}


TAny* RDefectTests::GetExtendedInterface(const TInt32 aUid)
/**
This method obtains the interface matching the specified uid.
@return				0 if no interface matching the uid is found.
					Otherwise, the this pointer cast to that interface.
@param				aUid the uid identifying the required interface.
*/
	{
	if (aUid == 0)
		{

		}
	return 0;
	}


//----------------------------------------------------------------------------
// The Defect Tests

LOCAL_C void SetupL()
	{
	test.Printf(_L("Unziping test xml files for defects\n"));

	CUnzip* unzip = CUnzip::NewLC(TheFs, KZipTestFile);
	unzip->ExtractL(KXmlTestDir);
	CleanupStack::PopAndDestroy(unzip);

	//Getch();
	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3750
@SYMTestCaseDesc		    OOM Testing.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Runs a set of test in oom conditions.
@SYMTestExpectedResults 	The component(s) should fucntion as expected and the tests should pass.
@SYMDEF					 	DEF060070
*/
LOCAL_C void DoOomTestL(TestFunc testFuncL, const TDesC& /*aDesc*/)
	{
	test.Printf(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3750    OOM test \n "));

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
		User::Panic(_L("Unexpected leave reason\n"),err);
		}

	test.Printf(_L("- succeeded with rate %i\r\n"), tryCount);
	}


//----------------------------------------------------------------------------


LOCAL_C void DoTestL(TestFunc aTestFuncL, const TDesC& aDesc)
	{
	test.Next(aDesc);

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

	DoOomTestL(aTestFuncL, aDesc);

	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-1571
@SYMTestCaseDesc		    Testing Prop: Symbian XML
							Framework-MContentHandler::OnStartPrefixMappingL()never called
@SYMTestPriority 		    Medium
@SYMTestActions  		    Parsing a xml file and check whether framework calls
							MContentHandler::OnStartPrefixMappingL() function
@SYMTestExpectedResults 	Test output should show  "Entered OnStartPrefixMappingL".
@SYMDEF 		 		 	DEF074797
*/


LOCAL_C void DEF074797L()
	{

	_LIT8 (KXmlNamespace,"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\
			<group xmlns=\"urn:oma:params:xml:ns:list-service\" xmlns:rl=\"urn:ietf:params:xml:ns:resource-lists\" xmlns:cr=\"urn:ietf:params:xml:ns:common-policy\">\
			<list-service uri=\"sip:prearranged3+s60test.user24@ims3.so.noklab.net\">\
			<display-name>prearranged3</display-name>\
			<list>\
			<entry uri=\"sip:s60test.user24@ims3.so.noklab.net\">\
			<rl:display-name>tr</rl:display-name>\
			</entry>\
			</list>\
			<invite-members>true</invite-members>\
			<max-participant-count>10</max-participant-count>\
			<cr:ruleset>\
			<cr:rule id=\"0\">\
			<cr:conditions>\
			<is-list-member>true</is-list-member>\
			</cr:conditions>\
			<cr:actions>\
			<join-handling>allow</join-handling>\
			<allow-anonymity>true</allow-anonymity>\
			<allow-initiate-conference>true</allow-initiate-conference>\
			<allow-conference-state>true</allow-conference-state>\
			</cr:actions>\
			<cr:transformations />\
			</cr:rule>\
			</cr:ruleset>\
			</list-service>\
			</group>");


	TBuf16<128> ucs4_0041;

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

	_LIT8 (KWbxmlParserDataType, "text/wbxml");

	RDefectTests handler;
	CleanupClosePushL (handler);

	CParser* parser = CParser::NewL(KXmlParserDataType, handler);
	CleanupStack::PushL (parser);

	User::LeaveIfError( parser->EnableFeature( EReportNamespaces ) );
	User::LeaveIfError( parser->EnableFeature( EReportNamespaceMapping ) );
	User::LeaveIfError( parser->EnableFeature( EReportNamespacePrefixes ) );

	// Parse the xml document
	ParseL(*parser, KXmlNamespace);

	// Now switch to a different parser (wbxml)
	parser->ParseBeginL (KWbxmlParserDataType);

	TBuf8<256> buf8;
	// Copy will ignore the upper byte if the byte-pair < 256, otherwise the value 1 is used.
	buf8.Copy(ucs4_0041);

	parser->ParseL(buf8);

	// Now switch back to the default parser (xml)
	ParseL(*parser, KXmlNamespace);

	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);

	}


//----------------------------------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3749
@SYMTestCaseDesc		    Swapping parsers back and forth between parsing the same document twice.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Parse an xml document, swap to the wbxml parser, then swap back to the xml parser and parse.
@SYMTestExpectedResults 	The test completes successfully.
@SYMDEF 		 		 	INC063222
*/
LOCAL_C void INC063222L()
	{
	_LIT8 (KXmlNamespace, "<RootElement rabbits:att=\"value\">\
		   <Element xmlns=\"http://www.symbian.com\" att=\"value\">\
		   <elprefix:Element xmlns:elprefix=\"http://element.uri\" att=\"value\">\
		   <Element xmlns:attprefix=\"http://attribute.uri\" attprefix:att=\"value\"/>\
		   <Element attprefix:att=\"value\"/>\
		   </elprefix:Element>\
		   </Element>\
		   </RootElement>");

	TBuf16<128> ucs4_0041;

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

	_LIT8 (KWbxmlParserDataType, "text/wbxml");

	RDefectTests handler;
	CleanupClosePushL (handler);

	CParser* parser = CParser::NewL(KXmlParserDataType, handler);
	CleanupStack::PushL (parser);

	User::LeaveIfError(parser->EnableFeature(EReportNamespaceMapping));

	// Parse the xml document
	ParseL(*parser, KXmlNamespace);

	// Now switch to a different parser (wbxml)
	parser->ParseBeginL (KWbxmlParserDataType);

	TBuf8<256> buf8;
	// Copy will ignore the upper byte if the byte-pair < 256, otherwise the value 1 is used.
	buf8.Copy(ucs4_0041);

	parser->ParseL(buf8);

	// Now switch back to the default parser (xml)
	ParseL(*parser, KXmlNamespace);

	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);
	}


//----------------------------------------------------------------------------

/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3747
@SYMTestCaseDesc		    Parsing a document supplied via a file handle.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Creating a parser and opening file handle to a xml document and then parsing the document.
@SYMTestExpectedResults 	The document is parsed.
@SYMDEF 		 		 	DEF060070
*/
LOCAL_C void DEF060070L()
	{
	RDefectTests handler;
	CleanupClosePushL (handler);

	CParser* parser = CParser::NewLC(KXmlParserDataType, handler);

	RFile handle;
	handle.Open(TheFs, KSimpleFile, EFileShareReadersOnly);
	CleanupClosePushL (handle);

	ParseL(*parser, handle);

	CleanupStack::PopAndDestroy (&handle);
	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);
	}


//----------------------------------------------------------------------------

/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3748
@SYMTestCaseDesc		    Processing namespaces.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Creating an xml parser and enabling namespace mapping reporting, before parsing a document.
@SYMTestExpectedResults 	The document is parsed with the namespace mapping reporting enabled.
@SYMDEF 		 		 	INC059062
*/
LOCAL_C void INC059062L()
	{
	RDefectTests handler;
	CleanupClosePushL (handler);

	CParser* parser = CParser::NewLC(KXmlParserDataType, handler);

	User::LeaveIfError(parser->EnableFeature(EReportNamespaceMapping));

	RFile handle;
	handle.Open(TheFs, KNsAndBindingFile, EFileShareReadersOnly);
	CleanupClosePushL (handle);

	ParseL(*parser, handle);

	CleanupStack::PopAndDestroy (&handle);
	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);
	}


//----------------------------------------------------------------------------


/*
@SYMTestCaseID          SYSLIB-XML-UT-3366
@SYMTestCaseDesc	    DEF101097: The XML parser don't leave twice when it parse several invalid xml files
@SYMTestPriority 	    Low
@SYMTestActions  	    Tests that the xml parser returns an error each time it parses the same corrupted xml file.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF101097
*/
LOCAL_C void DEF101097L()
	{
	TheCorruptedXmlFileTest = ETrue;

	_LIT  (KTestDocument1, "c:\\system\\data\\xml\\xml\\Corruptedbackupreg.xml");

	RDefectTests handler;
	CleanupClosePushL (handler);

	CParser* parser = CParser::NewLC(KXmlParserDataType, handler);

	// Parse the first xml doc.
	ParseL(*parser,TheFs, KTestDocument1);

	// Parse the second xml doc i.e. same data.
	ParseL(*parser,TheFs, KTestDocument1);

	test(TheCorruptedXmlFileTestErrorCount == 2);

	TheCorruptedXmlFileTest = EFalse;
	TheCorruptedXmlFileTestErrorCount = 0;

	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);
	}


//----------------------------------------------------------------------------



/*
@SYMTestCaseID          SYSLIB-XML-UT-4937
@SYMTestCaseDesc	   	Xml::ParseL()  Shouldn't Leave with KErrNoMemory for Non-Well formed Large XML file
@SYMTestPriority 	    Medium
@SYMTestActions  	    Tests that the Xml parser doesn't leave with KErrNoMemory from ParseL each time it parses the same Corrupted Large Xml file.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 INC134125	
*/

LOCAL_C void INC134125L()
	{
		TheCorruptedXmlFileTest = ETrue;
		
		_LIT  (KTestDocument1, "c:\\system\\data\\xml\\xml\\Corruptbigfile.xml");
		RDefectTests handler;
		CleanupClosePushL (handler);
		CParser* parser = CParser::NewLC(KXmlParserDataType, handler);
		// Parse the Corrupt Big Xml file.
		ParseL(*parser,TheFs, KTestDocument1);
		test(TheCorruptedXmlFileTestErrorCount == 1);
		TheCorruptedXmlFileTest = EFalse;
		TheCorruptedXmlFileTestErrorCount = 0;
		CleanupStack::PopAndDestroy (parser);
		CleanupStack::PopAndDestroy (&handler);
	}


//----------------------------------------------------------------------------

LOCAL_C void DoTestsL()
	{
	DoTestL(DEF060070L, _L(" @SYMTestCaseID:SYSLIB-XML-CT-3747 DEF060070 - XML parser cannot parse a file supplied by handle...\n "));
	DoTestL(INC059062L, _L(" @SYMTestCaseID:SYSLIB-XML-CT-3748 INC059062 - XML Parser Integration - namespaces not processing properly...\n "));
	DoTestL(INC063222L, _L(" @SYMTestCaseID:SYSLIB-XML-CT-3749 INC063222 - Symbian XML Framework - MContentHandler::OnStartPrefixMappingL() is never called...\n "));
	DoTestL(DEF074797L, _L(" @SYMTestCaseID:SYSLIB-XML-CT-1571 DEF074797 - Prop: Symbian XML Framework-MContentHandler::OnStartPrefixMappingL()never called...\n "));
	DoTestL(DEF101097L, _L(" @SYMTestCaseID:SYSLIB-XML-UT-3366 DEF101097 - The XML parser don't leave twice when it parse several invalid xml files...\n "));
	DoTestL(INC134125L, _L("INC134125 - Xml::ParseL()  Shouldn't Leave with KErrNoMemory for Non-Well formed Large XML file...\n"));
	}


//----------------------------------------------------------------------------


TInt E32Main()
    {
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(_L("XML Defect tests\n"));

	// Connect the file server instance
	User::LeaveIfError(TheFs.Connect());

	TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, SetupL());
	test(err == KErrNone);

	TRAP(err, DoTestsL());
	test(err == KErrNone);

	delete TheTrapCleanup;

	TheFs.Close();

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return (KErrNone);
    }

