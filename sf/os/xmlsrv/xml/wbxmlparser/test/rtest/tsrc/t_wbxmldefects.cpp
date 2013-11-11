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
#include <xml/parser.h>
#include <xml/wbxmlextensionhandler.h>
#include <xml/xmlframeworkerrors.h>

LOCAL_D RTest 				test(_L("t_wbxmldefects.exe"));
LOCAL_D TBool               OOM_TestingActive = EFalse;
LOCAL_D RFs					TheFs;

LOCAL_D CTrapCleanup* 		TheTrapCleanup 		= NULL;

// Installed via bld.inf test exports
_LIT  (KTestDocument1,"c:\\system\\xmltest\\SyncML\\1.2\\Defects\\DEF078987_1.wbxml");
_LIT  (KTestDocument2,"c:\\system\\xmlTest\\SyncML\\1.2\\Defects\\DEF078987_2.wbxml");
_LIT  (KCorruptWAPEmailPushMsg1,"c:\\system\\xmltest\\defects\\00000036.emnc");
_LIT  (KCorruptWAPEmailPushMsg2,"c:\\system\\xmltest\\defects\\00000036b.emnc");
_LIT  (KCorruptWAPEmailPushMsg3,"c:\\system\\xmltest\\defects\\00000060.emnc");
_LIT  (KCorruptWAPEmailPushMsg4,"c:\\system\\xmltest\\defects\\00000104.emnc");


typedef void (*TestFunc) (void);


//----------------------------------------------------------------------------
// The Content Handler

using namespace Xml;

class RDefectTests : public MWbxmlExtensionHandler, public MContentHandler
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

	// From MWbxmlExtensionHandler
	void OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode);
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


// NOTE: SAX 2.0 setDocumentLocator method not supported.

void RDefectTests::OnExtensionL(const RString& /*aData*/, TInt /*aToken*/, TInt aErrorCode)
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
	test (aErrorCode == KErrEof || aErrorCode == KErrNoMemory || aErrorCode == KErrXmlDocumentCorrupt);
	}


TAny* RDefectTests::GetExtendedInterface(const TInt32 aUid)
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


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3730
@SYMTestCaseDesc		    OOM testing.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Runs the set of tests in OOM conditions.
@SYMTestExpectedResults 	parser should be able to deal with OOM conditions.
@SYMDEF 		 		 	PDEF083044
*/
LOCAL_C void DoOomTestL(TestFunc testFuncL, const TDesC& /*aDesc*/)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3730 "));
	test.Printf(_L("    OOM test"));
			
    OOM_TestingActive = ETrue;
	TInt err, tryCount = 0;
	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++tryCount);
		User::__DbgMarkStart(RHeap::EUser);
		TRAP(err, (*testFuncL)());
		User::__DbgMarkEnd(RHeap::EUser, 0);
		} while(err==KErrNoMemory);
    OOM_TestingActive = EFalse;

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
// The Defect Tests


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-1666
@SYMTestCaseDesc		    Testing : Convergence: KErrXmlStringDictionaryPluginNotFound when
							syncing with DM server
@SYMTestPriority 		    High
@SYMTestActions  		    It parse the two files DEF078987_1.wbxml and DEF078987_1.wbxml
							(Attached with the defect) which contain the updated
							publicID FD3 for DTD SyncML 1.1
@SYMTestExpectedResults 	Test should not fail
@SYMDEF 		 		 	DEF083044
*/

LOCAL_C void Defect_DEF083044L()
	{

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-1666 "));
	_LIT8 (KWbxmlParserDataType, "text/wbxml");

	RDefectTests handler;
	CleanupClosePushL (handler);

	CParser* parser = CParser::NewL(KWbxmlParserDataType, handler);
	CleanupStack::PushL (parser);
	ParseL(*parser,TheFs, KTestDocument1);
	CleanupStack::PopAndDestroy (parser);

	// Actually we don't need to delete the parser and create it again to parser another file.
	//But here with WBXML file, it is failing when parsing wbxml file second time.
	//Seems  it is defect in wbxml parser


	parser = CParser::NewL(KWbxmlParserDataType, handler);
	CleanupStack::PushL (parser);
	ParseL(*parser,TheFs, KTestDocument2);


	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);
	}


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3173
@SYMTestCaseDesc		    Testing : XML parser USER 130 panic parsing DRM rights object
@SYMTestPriority 		    High
@SYMTestActions  		    It parse wbxml with extra END tag
@SYMTestExpectedResults 	Test should not fail
@SYMDEF 		 		 	DEF096973
*/


LOCAL_C void Defect_DEF096973L()
	{

	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3173 "));
	TBuf16<128> ucs4_0041;

	ucs4_0041.Append(0x01);
	ucs4_0041.Append(0x04);
	ucs4_0041.Append(0x6A);
	ucs4_0041.Append(0x00);
	ucs4_0041.Append(0x7F);
	ucs4_0041.Append(0x02);
	ucs4_0041.Append(0x80);
	ucs4_0041.Append(0x41);
	ucs4_0041.Append(0x01);
	ucs4_0041.Append(0x01); //Extra END tag

	_LIT8 (KWbxmlParserDataType, "text/wbxml");

	RDefectTests handler;
	CleanupClosePushL (handler);

	CParser* parser = CParser::NewL(KWbxmlParserDataType, handler);
	CleanupStack::PushL (parser);


	// Now switch to a different parser (wbxml)
	parser->ParseBeginL (KWbxmlParserDataType);

	TBuf8<256> buf8;
	// Copy will ignore the upper byte if the byte-pair < 256, otherwise the value 1 is used.
	buf8.Copy(ucs4_0041);

	TRAPD (err,parser->ParseL(buf8));
	test(err == KErrXmlDocumentCorrupt);

	parser->ParseEndL();


	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);
	}


/*
@SYMTestCaseID          SYSLIB-XML-UT-1679
@SYMTestCaseDesc	    DEF083625 - WBXML parser crash when it tries to parse second wbxml file
@SYMTestPriority 	    High
@SYMTestActions  	    Tests that the wbxml parser can handle parsing more than one wbxml file
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF083625
*/
LOCAL_C void DEF083625L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-UT-1679 "));
	_LIT8 (KWbxmlParserDataType, "text/wbxml");
	_LIT  (KTestDocument1, "c:\\system\\XmlTest\\SyncML\\1.1\\add-to-client\\FromServer1.wbxml");
	_LIT  (KTestDocument2, "c:\\system\\XmlTest\\SyncML\\1.1\\add-to-server\\FromServer1.wbxml");

	RDefectTests handler;
	CleanupClosePushL (handler);

	CParser* parser = CParser::NewLC(KWbxmlParserDataType, handler);

	// Parse the first wbxml doc.
	ParseL(*parser,TheFs, KTestDocument1);

	// Parse the second wbxml doc. This will panic if DEF083625 is still present.
	ParseL(*parser,TheFs, KTestDocument2);

	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);
	}

/*
@SYMTestCaseID          BASESRVCS-XML-UT-4011
@SYMTestCaseDesc	    INC127127-Test WAP push email message which contain a corrupt string table length encoding and a negative string table length.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests that the wbxml parser can handle parsing of corrupted emnc file.
@SYMTestExpectedResults WBXML Parser should detect corruption or negative length and  leave with KErrXmlDocumentCorrupt
@SYMDEF                 INC127127	
*/
LOCAL_C void Incident_INC127127L()
	{
	test.Next(_L(" @SYMTestCaseID:BASESRVCS-XML-UT-4011 "));

	_LIT8( KWbxmlParserDataType, "text/wbxml");
	
	RBuf8 aBuf,aBuf1;
	RFs aRfs;
	RFile aRFile,aRFile1;

	
	RDefectTests handler;
	CleanupClosePushL(handler);
	
	CParser* parser = CParser::NewLC( KWbxmlParserDataType, handler );
	parser->ParseBeginL();


	User::LeaveIfError(aRfs.Connect());
	CleanupClosePushL(aRfs);
	
	
	User::LeaveIfError(aRFile.Open(aRfs,KCorruptWAPEmailPushMsg1,EFileRead));
	CleanupClosePushL(aRFile);
	TInt size1;
	User::LeaveIfError(aRFile.Size(size1));
	aBuf.CreateL(size1);
	CleanupClosePushL(aBuf);
	aRFile.Read(aBuf);

	TRAPD(err1,parser->ParseL(aBuf));
	// If inside an OOM test loop and alloc failure then report it back to the caller.
	if (OOM_TestingActive && (err1 == KErrNoMemory))
        User::Leave(err1);
    test(err1 == KErrXmlDocumentCorrupt);
	
	
	User::LeaveIfError(aRFile1.Open(aRfs,KCorruptWAPEmailPushMsg2,EFileRead));
	CleanupClosePushL(aRFile1);
	TInt size2;
	User::LeaveIfError(aRFile1.Size(size2));
	aBuf1.CreateL(size2);
	CleanupClosePushL(aBuf1);
	aRFile1.Read(aBuf1);
		
	TRAPD(err2,parser->ParseL(aBuf1));
	// If inside an OOM test loop and alloc failure then report it back to the caller.
	if (OOM_TestingActive && (err2 == KErrNoMemory))
        User::Leave(err2);
	test(err2 == KErrXmlDocumentCorrupt);


	CleanupStack::PopAndDestroy(5, &aRfs); 

	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);
	}

/*
@SYMTestCaseID          BASESRVCS-XML-UT-4012
@SYMTestCaseDesc	    INC133484-Watchermainthread crashes when receiving corrupted email notification.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests that the wbxml parser can handle parsing of corrupted emnc file.
@SYMTestExpectedResults WBXML Parser should detect corruption or negative length and  leave with KErrXmlDocumentCorrupt
@SYMDEF                 INC133484	
*/
LOCAL_C void Incident_INC133484L()
	{
	_LIT8( KWbxmlParserDataType, "text/wbxml");
	
	RBuf8 aBuf;
	RFs aRfs;
	RFile aRFile;

	
	RDefectTests handler;
	CleanupClosePushL(handler);
	
	CParser* parser = CParser::NewLC( KWbxmlParserDataType, handler );
	parser->ParseBeginL();


	User::LeaveIfError(aRfs.Connect());
	CleanupClosePushL(aRfs);
	
	
	User::LeaveIfError(aRFile.Open(aRfs,KCorruptWAPEmailPushMsg3,EFileRead));
	CleanupClosePushL(aRFile);
	TInt size;
	User::LeaveIfError(aRFile.Size(size));
	aBuf.CreateL(size);
	CleanupClosePushL(aBuf);
	aRFile.Read(aBuf);

	TRAPD(err,parser->ParseL(aBuf));
	// If inside an OOM test loop and alloc failure then report it back to the caller.
	if (OOM_TestingActive && (err == KErrNoMemory))
        User::Leave(err);
    test(err == KErrXmlDocumentCorrupt);
	
	
	CleanupStack::PopAndDestroy(3, &aRfs); 

	CleanupStack::PopAndDestroy (parser);
	CleanupStack::PopAndDestroy (&handler);
	}


/*

PDEF142385-Tests that the wbxml parser can handle parsing of corrupted emnc file.
	
*/
LOCAL_C void Defect_PDEF142385L()
 	{
 	_LIT8( KWbxmlParserDataType, "text/wbxml");
 	
	RBuf8 aBuf;
 	RFs aRfs;
 	RFile aRFile;
 
 	RDefectTests handler;
 	CleanupClosePushL(handler);
 	
 	CParser* parser = CParser::NewLC( KWbxmlParserDataType, handler );
 	parser->ParseBeginL();
 
 
 	User::LeaveIfError(aRfs.Connect());
 	CleanupClosePushL(aRfs);
 	

 	User::LeaveIfError(aRFile.Open(aRfs,KCorruptWAPEmailPushMsg4,EFileRead));
 	CleanupClosePushL(aRFile);
 	TInt size;
 	User::LeaveIfError(aRFile.Size(size));
 	aBuf.CreateL(size);
 	CleanupClosePushL(aBuf);
 	aRFile.Read(aBuf);
 
 	TRAPD(err,parser->ParseL(aBuf));
 	// If inside an OOM test loop and alloc failure then report it back to the caller.
 	if (OOM_TestingActive && (err == KErrNoMemory))
         User::Leave(err);
     // Since the input file is a well formed WBXML, the code should not crash nor should it raise a "KErrXmlDocumentCorrupt" exception
 	test(err == KErrNone);
 	
 	CleanupStack::PopAndDestroy(5); 
 	}
	
		
	
LOCAL_C void DoTestsL()
	{
	//DEF096973: XML parser USER 130 panic parsing DRM rights object
	Defect_DEF096973L();
	
    DoTestL(Incident_INC127127L, _L("INC127127: Test WBXML Parser robustness with corrupt string table length files..."));
    DoTestL(Defect_DEF083044L, _L("DEF083044: P:Convergence: KErrXmlStringDictionaryPluginNotFound when syncing with DM server..."));
	DoTestL(DEF083625L, _L("DEF083625 - WBXML parser crash when it tries to parse second wbxml file..."));
	DoTestL(Incident_INC133484L, _L("INC133484:Watchermainthread crashes when receiving corrupted email notification..."));
	DoTestL(Defect_PDEF142385L, _L("PDEF142385:Watchermainthread crashes when receiving corrupted email ntfcn   ..."));
	}


TInt E32Main()
    {
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(_L("Wbxml XML Defect tests\n"));

	// Connect the file server instance
	User::LeaveIfError(TheFs.Connect());

	TheTrapCleanup = CTrapCleanup::New();

	TInt err;

	TRAP(err, DoTestsL());
	test(err == KErrNone);

	delete TheTrapCleanup;

	TheFs.Close();

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return (KErrNone);
    }

