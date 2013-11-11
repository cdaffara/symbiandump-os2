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
// Performance test implementation for XML API 
// 
//

/**
 @file 
 @internalComponent
*/
#include <e32std.h>
#include <ecom/ecom.h>

#include <xml/matchdata.h>
#include <xml/parser.h>

#include "contenthandlers.h"
#include "tp_perftestbase.h"

#define CR_JROE6C2LB7

//************************************************
// Class definition
class CXmlApiTests : public CPerformanceTests
	{
	public:
		static CXmlApiTests* NewL();
		static CXmlApiTests* NewLC();
	
	public:
		friend void TestParseBeginL();
		friend void TestNewLMimeType();
		friend void TestNewLCMimeType();
		friend void TestNewLMatchData();
		friend void TestNewLCMatchData();
		friend void TestParseBeginLMD();
		
		

	public:
		~CXmlApiTests();
				
		virtual void GetMethodListL(RApiList& aAPIList);
		virtual TInt GetNumberOfRepetition();	
		virtual void PrepareTestsL();	

	private:
		void ConstructL();
		CXmlApiTests();
				
	private:
		Xml::TSimpleContentHandler* iHandler;
	};


//************************************************
// Globals
_LIT8(KXmlTestNewLMimeType, "NewL(mimeType) --- SYSLIB-XML-PT-1413");
_LIT8(KXmlTestNewLCMimeType, "NewLC(mimeType) --- SYSLIB-XML-PT-1414");
_LIT8(KXmlTestParseBeginL, "ParseBeginL(mimeType) --- SYSLIB-XML-PT-1415");

#ifdef CR_JROE6C2LB7
_LIT8(KXmlTestNewLMatchData, "NewL(CMatchData) --- SYSLIB-XML-PT-1416");
_LIT8(KXmlTestNewLCMatchData, "NewLC(CMatchData) --- SYSLIB-XML-PT-1417");
_LIT8(KXmlTestParseBeginLMD, "ParseBeginL(CMatchData)--- SYSLIB-XML-PT-1418");
#endif // CR_JROE6C2LB7

_LIT8(KXmlXmlParserMimeType, "text/xml");
_LIT8(KXmlWbXmlParserMimeType, "text/wbxml");
_LIT(KXmlPerformanceTestName, "Xml Api Performance Tests");

CXmlApiTests* apiTest;


//************************************************
// Test methods


/**
@SYMTestCaseID 			 	SYSLIB-XML-PT-1413
@SYMTestCaseDesc		    Performance test.
@SYMTestPriority 		    Medium
@SYMTestActions  		    
@SYMTestExpectedResults 	
@SYMREQ						REQ0000 		 		 					
*/
void TestNewLMimeType()
	{
	apiTest->ApiTestStart();
	Xml::CParser* parser = Xml::CParser::NewL(KXmlXmlParserMimeType(), *apiTest->iHandler);
	apiTest->ApiTestEnd();
	delete parser;
	}


/**
@SYMTestCaseID 			 	SYSLIB-XML-PT-1414
@SYMTestCaseDesc		    Performance test.
@SYMTestPriority 		    Medium
@SYMTestActions  		    
@SYMTestExpectedResults 	
@SYMREQ						REQ0000 		 		 					
*/
void TestNewLCMimeType()
	{
	apiTest->ApiTestStart();
	Xml::CParser* parser = Xml::CParser::NewLC(KXmlXmlParserMimeType(), *apiTest->iHandler);
	apiTest->ApiTestEnd();
	CleanupStack::Pop(parser);
	delete parser;
	}


/**
@SYMTestCaseID 			 	SYSLIB-XML-PT-1415
@SYMTestCaseDesc		    Performance test.
@SYMTestPriority 		    Medium
@SYMTestActions  		    
@SYMTestExpectedResults 	
@SYMREQ						REQ0000 		 		 					
*/
void TestParseBeginL()
	{
	Xml::CParser* parser = Xml::CParser::NewLC(KXmlXmlParserMimeType(), *apiTest->iHandler);
	apiTest->ApiTestStart();
	parser->ParseBeginL(KXmlWbXmlParserMimeType());
	apiTest->ApiTestEnd();
	CleanupStack::PopAndDestroy(parser);
	}


/**
@SYMTestCaseID 			 	SYSLIB-XML-PT-1416
@SYMTestCaseDesc		    Performance test.
@SYMTestPriority 		    Medium
@SYMTestActions  		    
@SYMTestExpectedResults 	
@SYMREQ						REQ0000 		 		 					
*/
#ifdef CR_JROE6C2LB7
void TestNewLMatchData()
	{
	Xml::CMatchData* matchData = Xml::CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlXmlParserMimeType);
	apiTest->ApiTestStart();
	Xml::CParser* parser = Xml::CParser::NewLC(*matchData, *apiTest->iHandler);
	apiTest->ApiTestEnd();
	CleanupStack::PopAndDestroy(2, matchData);
	}


/**
@SYMTestCaseID 			 	SYSLIB-XML-PT-1417
@SYMTestCaseDesc		    Performance test.
@SYMTestPriority 		    Medium
@SYMTestActions  		    
@SYMTestExpectedResults 	
@SYMREQ						REQ0000 		 		 					
*/
void TestNewLCMatchData()
	{
	apiTest->ApiTestStart();
	Xml::CParser* parser = Xml::CParser::NewLC(KXmlXmlParserMimeType(), *apiTest->iHandler);
	apiTest->ApiTestEnd();
	CleanupStack::Pop(parser);
	delete parser;
	}


/**
@SYMTestCaseID 			 	SYSLIB-XML-PT-1418
@SYMTestCaseDesc		    Performance test.
@SYMTestPriority 		    Medium
@SYMTestActions  		    
@SYMTestExpectedResults 	
@SYMREQ						REQ0000 		 		 					
*/
void TestParseBeginLMD()
	{
	Xml::CMatchData* matchData = Xml::CMatchData::NewLC();
	matchData->SetMimeTypeL(KXmlXmlParserMimeType);
	Xml::CParser* parser = Xml::CParser::NewLC(*matchData, *apiTest->iHandler);
	
	Xml::CMatchData* matchDataNew = Xml::CMatchData::NewLC();
	
	matchDataNew->SetMimeTypeL(KXmlWbXmlParserMimeType);
	apiTest->ApiTestStart();
	parser->ParseBeginL(*matchDataNew);
	apiTest->ApiTestEnd();
	CleanupStack::PopAndDestroy(3, matchData);
	}
#endif // CR_JROE6C2LB7


//*************************************************
// Derivied virtuals method

void CXmlApiTests::PrepareTestsL()
	{
	iHandler = new (ELeave) Xml::TSimpleContentHandler();
	}

void CXmlApiTests::GetMethodListL(RApiList& aApiList)
	{
	ASSERT(aApiList.Count() == 0);
#ifdef _DEBUG
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestNewLMimeType, TestNewLMimeType, 17103000));
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestNewLCMimeType, TestNewLCMimeType, 17142000));
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestParseBeginL, TestParseBeginL, 11874000));
#ifdef CR_JROE6C2LB7
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestNewLMatchData, TestNewLMatchData, 17215000));
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestNewLCMatchData, TestNewLCMatchData, 17321000));
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestParseBeginLMD, TestParseBeginLMD, 12019000));
#endif //CR_JROE6C2LB7

#else

	aApiList.Append( new (ELeave) TApiRecord(KXmlTestNewLMimeType, TestNewLMimeType, 9017000 ));
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestNewLCMimeType, TestNewLCMimeType, 9037000 ));
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestParseBeginL, TestParseBeginL, 6720000));

#ifdef CR_JROE6C2LB7
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestNewLMatchData, TestNewLMatchData, 8986000));
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestNewLCMatchData, TestNewLCMatchData, 9031000));
	aApiList.Append( new (ELeave) TApiRecord(KXmlTestParseBeginLMD, TestParseBeginLMD, 6685000));
#endif //CR_JROE6C2LB7

#endif //_DEBUG
	}
TInt CXmlApiTests::GetNumberOfRepetition()
	{
	return 1000;
	}


//***********************************************************
// Construction / descrution

CXmlApiTests* CXmlApiTests::NewL()
	{
	CXmlApiTests* self = CXmlApiTests::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
CXmlApiTests* CXmlApiTests::NewLC()
	{
	CXmlApiTests* self = new (ELeave) CXmlApiTests();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CXmlApiTests::ConstructL()
	{
	CPerformanceTests::ConstructL(KXmlPerformanceTestName());
	}

CXmlApiTests::CXmlApiTests()
	{

	}

CXmlApiTests::~CXmlApiTests()
	{
	REComSession::FinalClose();
	if (iHandler)
		{
		delete iHandler;
		iHandler = NULL;
		}
	}


//***********************************************************

static void MainL()
	{
	__UHEAP_MARK;
	apiTest = CXmlApiTests::NewLC();
	apiTest->RunTestsL();
	CleanupStack::Pop(apiTest);
	delete apiTest;
	__UHEAP_MARKEND;
	}


TInt E32Main()
	{
	
	CTrapCleanup* c=CTrapCleanup::New();
	
	// start the loader
	RFs fs;
	User::LeaveIfError(fs.Connect()==KErrNone);
	fs.Close();
	
	User::LeaveIfError (c!=0);
	TRAPD(r,MainL());
	User::LeaveIfError(r);
	delete c;
	
	return KErrNone;
	}
