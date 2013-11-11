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

#include <xml/matchdata.h>
#include <xml/parserfeature.h>
#include <xml/xmlparsererrors.h>

#include "t_xmlparser.h"
#include "unzip.h"

using namespace Xml;

RTest test(_L("XML parser tests"));  // must be called 'test', as the e32test macros rely on this.

// Installed via bld.inf test exports
_LIT(KBigTestFile,       "c:\\system\\data\\xml\\xml\\networking.xml");
_LIT(KRefFilename,       "c:\\system\\data\\xml\\xml\\ref-networking.xml");
_LIT(KSimpleTestFile,    "c:\\system\\data\\xml\\xml\\simple.xml");
_LIT(KInvalidTestFile,   "c:\\system\\data\\xml\\xml\\invalid.xml");
_LIT(KNamespaceTestFile, "c:\\system\\data\\xml\\xml\\namespace.xml");
_LIT(KEntityTestFile,    "c:\\system\\data\\xml\\xml\\entity.xml");
_LIT(KCapsTestFile,      "c:\\system\\data\\xml\\xml\\caps.xml");
_LIT(KEncodingTestFile,  "c:\\system\\data\\xml\\xml\\doc_jp_utf.xml");
_LIT(KEncodingRefFile,   "c:\\system\\data\\xml\\xml\\ref_doc_jp_utf.xml");

_LIT(KZipTestFile, "c:\\system\\data\\xml\\xml.zip");
_LIT(KXmlTestDir, "c:\\system\\data\\xml");

_LIT8(KIncompleteData, "<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\"><container id=\"1\" parentID=\"0\" child ");

_LIT8(KParserDataType, "text/xml");
_LIT8(KParserDataType2, "text/wbxml");
_LIT8(KParserDataType3, "text/unique");
_LIT8(KParserDataType4, "text/same");


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3734
@SYMTestCaseDesc		    Parsing an xml document.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Takes networking.xml, parses it and reconstitutes it as XML. A comparison
							between the output in Epocwind.out and networking.xml can be used to show
							that the parser is doing what it should.
							The test has now been automated to diff the output with ref-networking.xml.
@SYMTestExpectedResults 	The document is parsed without any issues.
@SYMPREQ 		 		 	PREQ230
*/
LOCAL_C void BasicParsingTestL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile result;
	TFileName filename;
	User::LeaveIfError(result.Temp(fs, _L("C:\\"), filename, EFileWrite));
	CleanupClosePushL(result);

	TRebuildingContentHandler contentHandler(result);

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);

	ParseL(*parser, fs, KBigTestFile());
	test(contentHandler.iError==KErrNone);

	CleanupStack::PopAndDestroy(2);

	RFile ref;
	User::LeaveIfError(ref.Open(fs, KRefFilename, EFileRead));
	CleanupClosePushL(ref);
	User::LeaveIfError(result.Open(fs, filename, EFileRead));
	CleanupClosePushL(result);

	TInt resultSize;
	User::LeaveIfError(result.Size(resultSize));
	TInt refSize;
	User::LeaveIfError(ref.Size(refSize));
	test(resultSize==refSize);

	TBuf8<256> refBuf;
	TBuf8<256> resultBuf;

	while(ref.Read(refBuf)==KErrNone && result.Read(resultBuf)==KErrNone && refBuf.Length()>0)
			test(refBuf==resultBuf);

	test(refBuf.Length()==0 && resultBuf.Length()==0);

	CleanupStack::PopAndDestroy(2);
	fs.Delete(filename);
	CleanupStack::PopAndDestroy(&fs);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 				SYSLIB-XML-CT-1578
@SYMTestCaseDesc			SS31 Symbian XML Framework - Symbian SAX Plugin is not returned by default.
@SYMTestPriority 			High
@SYMTestActions  			Create CParser and parse test file. Compare number of elements in output
							to determine which parser is selected. Create CParser and parse test file
							with CMatchData API with default supplied parameters. Test for selected parser.
@SYMTestExpectedResults		Test should not fail. Symbian parser is expected to be selected.
@SYMDEF 					INC073582
*/
LOCAL_C void INC073582L()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TSimpleContentHandler* handler = new (ELeave) TSimpleContentHandler();
	CleanupStack::PushL(handler);

	RFile handle;
	User::LeaveIfError(handle.Open(fs, KSimpleTestFile, EFileShareReadersOnly));
	CleanupClosePushL(handle);

	// Test default behaviour of text/xml parser (multiple parsers return Symbian)

	CParser* parser = CParser::NewLC(KParserDataType, *handler);

	ParseL(*parser, handle);
	//Expected:		handler->iNumElements == 6
	//				Production, Symbian parser.
	test(handler->iNumElements==6);

	CleanupStack::PopAndDestroy(parser);

	// Test default behaviour with CMatchData supplied version of CParser

	CMatchData *matchData = CMatchData::NewLC();

	matchData->SetMimeTypeL(KParserDataType);
	matchData->SetVariantL(_L8(""));

	parser = CParser::NewLC(*matchData, *handler);

	ParseL(*parser, handle);
	//Expected:		handler->iNumElements == 6
	//				Production, Symbian parser.
	test(handler->iNumElements==6);

	CleanupStack::PopAndDestroy(parser);
	CleanupStack::PopAndDestroy(matchData);

	// Test default behaviour of text/wbxml parser (multiple parsers return Symbian-first)

	parser = CParser::NewLC(KParserDataType2, *handler);

	ParseL(*parser, handle);
	//Expected:		handler->iNumElements == 6
	//				Production, Symbian parser.
	test(handler->iNumElements==6);

	CleanupStack::PopAndDestroy(parser);

	// Test default behaviour of text/unique parser (single parser non-Symbian)

	parser = CParser::NewLC(KParserDataType3, *handler);

	ParseL(*parser, handle);
	//Expected:		handler->iNumElements == 1
	//				Test parser1.
	test(handler->iNumElements==1);

	CleanupStack::PopAndDestroy(parser);

	// Test default behaviour of text/same parser (multiple parser non-Symbian, return lowest uid)

	parser = CParser::NewLC(KParserDataType4, *handler);

	ParseL(*parser, handle);
	//Expected:		handler->iNumElements == 1
	//				Test parser1.
	test(handler->iNumElements==1);

	CleanupStack::PopAndDestroy(parser);

	CleanupStack::PopAndDestroy(&handle);
	CleanupStack::PopAndDestroy(handler);
	CleanupStack::PopAndDestroy(&fs);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3735
@SYMTestCaseDesc		    Parsing a document larger than the Expat internal buffer.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Checks that a descriptor larger than Expats internal buffer can be parsed successfully.
@SYMTestExpectedResults 	The xml document is parsed correctly.
@SYMDEF 		 		 	DEF056122
*/
LOCAL_C void DEF056122L()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile result;
	TFileName filename;
	User::LeaveIfError(result.Temp(fs, _L("C:\\"), filename, EFileWrite));
	CleanupClosePushL(result);

	TRebuildingContentHandler contentHandler(result);

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);


	// Read the test file into a descriptor

	RFile file;

	User::LeaveIfError(file.Open (fs, KBigTestFile(), EFileRead|EFileShareReadersOnly));
	CleanupClosePushL (file);

	TInt size = 0;
	file.Size (size);

	HBufC8* rawdocheap = HBufC8::NewLC (size);
	TPtr8 rawdoc(rawdocheap->Des ());

	User::LeaveIfError (file.Read (rawdoc, size));

	// Parse the descriptor

	ParseL(*parser, rawdoc);
	test(contentHandler.iError==KErrNone);

	CleanupStack::PopAndDestroy(4);

	// Test everything worked

	RFile ref;
	User::LeaveIfError(ref.Open(fs, KRefFilename, EFileRead));
	CleanupClosePushL(ref);
	User::LeaveIfError(result.Open(fs, filename, EFileRead));
	CleanupClosePushL(result);

	TInt resultSize;
	User::LeaveIfError(result.Size(resultSize));
	TInt refSize;
	User::LeaveIfError(ref.Size(refSize));
	test(resultSize==refSize);

	TBuf8<256> refBuf;
	TBuf8<256> resultBuf;

	while(ref.Read(refBuf)==KErrNone && result.Read(resultBuf)==KErrNone && refBuf.Length()>0)
			test(refBuf==resultBuf);

	test(refBuf.Length()==0 && resultBuf.Length()==0);

	CleanupStack::PopAndDestroy(2);
	fs.Delete(filename);
	CleanupStack::PopAndDestroy(&fs);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID          SYSLIB-XML-CT-1598
@SYMTestCaseDesc	    Tests to make sure XML parser does not crash when parsing a message that is not complete.
@SYMTestPriority 	    Medium
@SYMTestActions  	    Parses the message that is not complete.
@SYMTestExpectedResults Test must not fail.
@SYMDEF                 INC073797
*/
LOCAL_C void INC073797()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile result;
	TFileName filename;
	User::LeaveIfError(result.Temp(fs, _L("C:\\"), filename, EFileWrite));
	CleanupClosePushL(result);

	TRebuildingContentHandler contentHandler(result);

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);

	parser->ParseL(KIncompleteData());
	test(contentHandler.iError==KErrNone);

	CleanupStack::PopAndDestroy(2);

	fs.Delete(filename);
	CleanupStack::PopAndDestroy(&fs);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3736
@SYMTestCaseDesc		    Parsing an xml file with extended character..
@SYMTestPriority 		    Medium
@SYMTestActions  		    Checks that the parser can deal with extended characters - locales outside of ascii.
							Parses doc_jp_utf.xml, reconstitutes it as xml, and compares with ref_doc_jp_utf.xml.
@SYMTestExpectedResults 	The reconstructed document is the same as the original document.
@SYMDEF 		 		 	DEF051379
*/
LOCAL_C void DEF051379L()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TInt pos = 0;

	//open a temporary results file
	RFile result;
	TFileName filename;
	User::LeaveIfError(result.Temp(fs, _L("C:\\"), filename, EFileWrite));
	CleanupClosePushL(result);

	//reconstitutes the xml from parser callbacks
	TRebuildingContentHandler contentHandler(result);

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);

	//parse the file
	ParseL(*parser, fs, KEncodingTestFile());
	test(contentHandler.iError==KErrNone);

	//open the reference file, and compare with results file
	RFile ref;
	User::LeaveIfError(ref.Open(fs, KEncodingRefFile, EFileRead));
	CleanupClosePushL(ref);

	User::LeaveIfError(result.Seek(ESeekStart,pos));

	TInt resultSize;
	User::LeaveIfError(result.Size(resultSize));
	TInt refSize;
	User::LeaveIfError(ref.Size(refSize));
	test(resultSize==refSize);

	TBuf8<256> refBuf;
	TBuf8<256> resultBuf;

	while(ref.Read(refBuf)==KErrNone && result.Read(resultBuf)==KErrNone && refBuf.Length()>0)
			test(refBuf==resultBuf);

	//check that RFile::Read has read the entire contents of each file,
	//so that length (from cursor position to end of file) == 0
	test(refBuf.Length()==0 && resultBuf.Length()==0);


	CleanupStack::PopAndDestroy(&ref);
	CleanupStack::PopAndDestroy(parser);
	CleanupStack::PopAndDestroy(&result);
	fs.Delete(filename);
	CleanupStack::PopAndDestroy(&fs);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3737
@SYMTestCaseDesc		    Parsing multiple documents in sequence.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Checks that the parser can be reset after parsing one document and used to parse another.
@SYMTestExpectedResults 	The contenthandler is able to deal with the parsing of 2 xml documents in sequence.
@SYMPREQ 		 		 	PREQ230
*/
LOCAL_C void ResetTestL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TSimpleContentHandler contentHandler;

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);

	ParseL(*parser, fs, KSimpleTestFile());

	test(contentHandler.iNumElements==6);
	test(contentHandler.iError==KErrNone);

	ParseL(*parser, fs, KBigTestFile());

	test(contentHandler.iNumElements==832);
	test(contentHandler.iError==KErrNone);

	CleanupStack::PopAndDestroy(2);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3738
@SYMTestCaseDesc		    Parser copes with leaving content handler.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Checks that the parser copes with a leave from a content handler callback.
@SYMTestExpectedResults 	Tests pass.
@SYMPREQ 		 		 	PREQ230
*/
LOCAL_C void CallbackLeaveTestL()
	{
	TSimpleContentHandler contentHandler;
	contentHandler.iLeaveOnStartElement = ETrue;

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TRAPD(err, ParseL(*parser, fs, KSimpleTestFile()));

	User::LeaveIfError(err); // For OOM testing

	test(contentHandler.iNumElements==1);
	test(err==TSimpleContentHandler::KExpectedLeaveCode);

	contentHandler.iLeaveOnStartElement = EFalse;
	ParseL(*parser, fs, KSimpleTestFile());
	test(contentHandler.iNumElements==6);

	CleanupStack::PopAndDestroy(2);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3739
@SYMTestCaseDesc		    Well-formedness errors are propagated.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Checks that well-formedness errors generated by the parser are propagated.
@SYMTestExpectedResults 	content handler reports the error.
@SYMPREQ 		 		 	PREQ230
*/
LOCAL_C void InvalidXmlTestL()
	{
	TSimpleContentHandler contentHandler;

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	ParseL(*parser, fs, KInvalidTestFile());
	if(contentHandler.iError!=EXmlInvalidToken)
		User::Leave(contentHandler.iError);  // For OOM testing

	CleanupStack::PopAndDestroy(2);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3740
@SYMTestCaseDesc		    Reporting namespace uri's and prefixes.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Checks that namespace uri's and prefixes are correctly reported.
@SYMTestExpectedResults 	content handler has no errors.
@SYMPREQ 		 		 	PREQ230
*/
LOCAL_C void NamespaceTestL()
	{
	_LIT8(KDefaultUri, "http://www.symbian.com");
	_LIT8(KElementPrefix, "elprefix");
	_LIT8(KElementUri, "http://element.uri");
	_LIT8(KAttributePrefix, "attprefix");
	_LIT8(KAttributeUri, "http://attribute.uri");

	TNamespaceContentHandler contentHandler(KDefaultUri, KElementPrefix, KElementUri, KAttributePrefix, KAttributeUri);

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	User::LeaveIfError(parser->EnableFeature(EReportNamespaceMapping));

	ParseL(*parser, fs, KNamespaceTestFile());
	User::LeaveIfError(contentHandler.iError); // For OOM testing

	test(contentHandler.iError==KErrNone);

	CleanupStack::PopAndDestroy(2);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3741
@SYMTestCaseDesc		    Converting character set.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Checks that skipped entities are reported.
@SYMTestExpectedResults 	The expected conversion result and the actual conversion result match.
@SYMPREQ 		 		 	PREQ230
*/
LOCAL_C void SkippedEntityTestL()
	{
	TSimpleContentHandler contentHandler;

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	ParseL(*parser, fs, KEntityTestFile());
	User::LeaveIfError(contentHandler.iError); // For OOM testing

	test(contentHandler.iError==KErrNone);
	test(contentHandler.iNumSkippedEntities==2);

	CleanupStack::PopAndDestroy(2);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 			 	SYSLIB-XML-CT-3742
@SYMTestCaseDesc		    Testing the Features functions of CParser, to make sure they work correctly.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Create the parser object and enable and disable features, testing if they are allowed or not.
@SYMTestExpectedResults 	Enabling and Disabling features return the expected result.
@SYMCR			 		 	CR0000
*/
LOCAL_C void ParserFeatureTestL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TCapsContentHandler contentHandler;

	CParser* parser = CParser::NewLC(KParserDataType, contentHandler);

	// Use a non-existing feature
	test(parser->EnableFeature(0xFFFFFFFF) == KErrNotSupported);
	test(parser->DisableFeature(0xFFFFFFFF) == KErrNotSupported);

	// Enable defined but unsupported features
	test(parser->EnableFeature(EErrorOnUnrecognisedTags) == KErrNotSupported);
	test(parser->EnableFeature(EReportUnrecognisedTags) == KErrNotSupported);
	test(parser->EnableFeature(ESendFullContentInOneChunk) == KErrNotSupported);
	test(parser->EnableFeature(ERawContent) == KErrNotSupported);

	// Enable supported features
	test(parser->EnableFeature(EReportNamespaces) == KErrNone);
	test(parser->EnableFeature(EReportNamespacePrefixes) == KErrNone);
	test(parser->EnableFeature(EReportNamespaceMapping) == KErrNone);
	test(parser->EnableFeature(EConvertTagsToLowerCase) == KErrNone);

	// Disable mandatory features
	test(parser->DisableFeature(EReportNamespacePrefixes)==KErrNotSupported);
	test(parser->DisableFeature(EReportNamespaces)==KErrNotSupported);

	// Disable optional features
	test(parser->DisableFeature(EReportNamespaceMapping) == KErrNone);
	test(parser->DisableFeature(EConvertTagsToLowerCase) == KErrNone);

	// Re-enable optional features needed for test
	test(parser->EnableFeature(EConvertTagsToLowerCase | EReportNamespaceMapping)
		== KErrNone);

	ParseL(*parser, fs, KCapsTestFile());

	test(contentHandler.iNumElements == 1);
	test(contentHandler.iNumPrefixMappings == 2);
	test(contentHandler.iNumPrefixUnmappings == 2);
	test(contentHandler.iError == KErrNone);

	TSimpleContentHandler contentHandler2;

	CParser* parser2 = CParser::NewLC(KParserDataType, contentHandler2);

	parser2->EnableFeature(EReportNamespaceMapping);

	ParseL(*parser2, fs, KNamespaceTestFile());

	test(contentHandler2.iNumElements == 5);
	test(contentHandler2.iNumPrefixMappings == 3);
	test(contentHandler2.iNumPrefixUnmappings == 3);
	test(contentHandler2.iError == KErrNone);

	User::LeaveIfError(parser2->DisableFeature(EReportNamespaceMapping));
	test(!parser2->IsFeatureEnabled(EReportNamespaceMapping));

	ParseL(*parser2, fs, KNamespaceTestFile());

	test(contentHandler2.iNumElements == 5);
	test(contentHandler2.iNumPrefixMappings == 0);
	test(contentHandler2.iNumPrefixUnmappings == 0);
	test(contentHandler2.iError == KErrNone);

	CleanupStack::PopAndDestroy(3);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3745
@SYMTestCaseDesc		    Checks that two parsers can co-exist without interfering with each other.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Creates 2 content handlers and parsers. Each parser parses a document and checks for errors.
@SYMTestExpectedResults 	The parsers should not affect each other and no errors should be produced.
@SYMPREQ 		 			PREQ230
*/
LOCAL_C void TwoParserTestL()
	{
	TSimpleContentHandler contentHandler1, contentHandler2;

	CParser* parser1 = CParser::NewLC(KParserDataType, contentHandler1);

	CParser* parser2 = CParser::NewLC(KParserDataType, contentHandler2);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	// Parse some of the big test file
	ParseL(*parser2, fs, KBigTestFile());

	// Check we've read what we expect
	test(contentHandler1.iNumElements==0);
	test(contentHandler1.iError==KErrNone);

	test(contentHandler2.iNumElements==832);
	test(contentHandler2.iError==KErrNone);

	ParseL(*parser1, fs, KSimpleTestFile());

	test(contentHandler1.iNumElements==6);
	test(contentHandler1.iError==KErrNone);

    // Stats for big test file shouldn't have changed as RunL was never called
	test(contentHandler2.iNumElements==832);
	test(contentHandler2.iError==KErrNone);

	CleanupStack::PopAndDestroy(&fs);

	CleanupStack::PopAndDestroy(2);
	}


// ---------------------------------------------------


struct TEndHandler : public MContentHandler
	{
	// From MContentHandler
	void OnStartDocumentL(const RDocumentParameters& , TInt ) {};
	void OnEndDocumentL(TInt) {};
	void OnStartElementL(const RTagInfo& , const RAttributeArray& , TInt ) {};
	void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
		{
		User::LeaveIfError(aErrorCode);

		const TDesC8& localPart8 = aElement.LocalName().DesC();
		const TDesC8& prefix8 = aElement.Prefix().DesC();

		iOut.Append(_L8("</"));

		if(prefix8.Length())
			{
			iOut.Append(prefix8);
			iOut.Append(_L8(":"));
			}
		iOut.Append(localPart8);
		iOut.Append(_L8(">"));
		};
	void OnContentL(const TDesC8& , TInt ) {};
	void OnStartPrefixMappingL(const RString& , const RString& , TInt ) {};
	void OnEndPrefixMappingL(const RString& , TInt ) {};
	void OnIgnorableWhiteSpaceL(const TDesC8& , TInt ) {};
	void OnSkippedEntityL(const RString& , TInt ) {};
	void OnProcessingInstructionL(const TDesC8& , const TDesC8& , TInt ) {};
	void OnExtensionL(const RString& , TInt , TInt ) {};
	void OnError(TInt aErrorCode) { iError = aErrorCode; };
	TAny* GetExtendedInterface(const TInt32 ) {return 0;};

	TBuf8<0x100> iOut;
	TInt iError;
	};


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3743
@SYMTestCaseDesc		    Testing the Expat parser.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Creating a simple content handler and parser. Parsing strings and checking the return error code what is expected.
@SYMTestExpectedResults 	The results returned by the content handler should be those that are expected.
@SYMCR			 		 	CR0000
*/
LOCAL_C void ExpatTestL()
	{
	TSimpleContentHandler handler;

	CParser* parser = CParser::NewLC(KParserDataType, handler);

	// Here the string is truncated because of the NULL.
	ParseL(*parser, _L8("<doc>\0</doc>"));
	test(handler.iError==EXmlNoElements);

	handler.iError = KErrNone;
	ParseL(*parser, _L8("<doc></dic>"));
	test(handler.iError==EXmlTagMismatch);

	handler.iError = KErrNone;
	ParseL(*parser, _L8("\n<?xml version='1.0'?>\n<a/>"));
	test(handler.iError==EXmlMisplacedPi);

	handler.iError = KErrNone;
	ParseL(*parser, _L8("<?xml version=\"1.0\" encoding=\"UTF-38x\"?>\n\r<doc></doc>"));
	test(handler.iError==EXmlUnknownEncoding);

	handler.iError = KErrNone;
	ParseL(*parser, _L8("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\r<doc></doc>"));
	test(handler.iError==KErrNone);

	handler.iError = KErrNone;
	ParseL(*parser, _L8("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\r<doc>hello</doc>"));
	test(handler.iError==KErrNone);

	handler.iError = KErrNone;
	ParseL(*parser, _L8("<?xml version=\"1.0\" encoding=\"\"?>\n\r<doc></doc>"));
	test(handler.iError==EXmlSyntax);

	handler.iError = KErrNone;
	ParseL(*parser, _L8("<?xml version=\"1.0\"?>\n\r<doc></doc>"));
	test(handler.iError==KErrNone);

	TEndHandler end;
	end.iError=KErrNone;
	CleanupStack::PopAndDestroy(parser);
	parser = CParser::NewLC(KParserDataType, end);

	ParseL(*parser, _L8("<a><b><c/></b><d><e/></d></a>"));
	test(end.iError==KErrNone);
	test(end.iOut==_L8("</c></b></e></d></a>"));

	CleanupStack::PopAndDestroy(parser);
	}


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3744
@SYMTestCaseDesc		    Function to convert a test into an OOM test.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Creates a low memory situation and runs the set of tests to make sure the component(s) works in such conditions.
@SYMTestExpectedResults 	Tests continue to pass, even in low memory conditions.
@SYMPREQ 		 		 	PREQ230
*/
LOCAL_C void OomTest(void (*testFuncL)())
	{
	TInt error;
	TInt count = 0;

	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++count);
		User::__DbgMarkStart(RHeap::EUser);
		TRAP(error, (testFuncL)());
		User::__DbgMarkEnd(RHeap::EUser, 0);
		} while(error == KErrNoMemory);

	_LIT(KTestFailed, "Out of memory test failure on iteration %d\n");
	__ASSERT_ALWAYS(error==KErrNone, test.Panic(error, KTestFailed, count));

	User::__DbgSetAllocFail(RHeap::EUser, RHeap::ENone, 1);
	}


// ---------------------------------------------------


// RunTestsL
// MainL
// E32Main
//
// Top-level functions

LOCAL_C void RunTestsL()
	{
	test.Title();

	test.Start(_L("Unziping test xml files"));

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CUnzip* unzip = CUnzip::NewLC(fs, KZipTestFile);
	unzip->ExtractL(KXmlTestDir);
	CleanupStack::PopAndDestroy(unzip);

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3734 Basic XML parsing test "));
	BasicParsingTestL();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3735 DEF056122 descriptors greater than 2048 (Expat internal buffer size) "));
	DEF056122L();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3736 DEF051379 parsing encoded characters "));
	DEF051379L();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1598 INC073797: Stack crashes sometimes in XML parsing in 3.0 platform "));
	INC073797();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3737 Reset mechanism "));
	ResetTestL();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3738 Leave from callback "));
	CallbackLeaveTestL();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3739 Invalid XML handling "));
	InvalidXmlTestL();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3740 Namespace testing "));
	NamespaceTestL();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3741 Skipped entity test "));
	SkippedEntityTestL();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3742 Parser features test "));
	ParserFeatureTestL();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1578 INC073582: SS31 Symbian XML Framework - Symbian SAX Plugin is not returned by default "));
	INC073582L();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3743 "));
	ExpatTestL();

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3744 Out-of-Memory testing "));
	OomTest(CallbackLeaveTestL);
	OomTest(InvalidXmlTestL);
	OomTest(NamespaceTestL);
	OomTest(SkippedEntityTestL);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3745 Two-parser test "));
	TwoParserTestL();


	test.Next(_L("Finished."));

	test.End();

	test.Close();

	CleanupStack::PopAndDestroy(&fs);

	REComSession::FinalClose();
	}




LOCAL_C void Main()
	{
	TRAPD(err, RunTestsL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	Main();

	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}
