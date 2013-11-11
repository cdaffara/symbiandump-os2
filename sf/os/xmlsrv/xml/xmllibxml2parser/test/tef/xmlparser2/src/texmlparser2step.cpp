// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file texmlparser2step.cpp
 @internalTechnology
*/
 
#include "texmlparser2step.h"
#include "tetestcontenthandler.h"
#include "libxml2_globals_private.h"
#include <e32cmn.h>
#include <xml/parserfeature.h>
#include <xml/matchdata.h>

using namespace Xml;
/**
 * Class CXmlParser2Step implementation
 */
_LIT(KSimpleXmlFile, "c:\\testdata\\xmlexample.xml");
#ifdef _DEBUG
_LIT(KCorruptXm1File, "c:\\testdata\\corruptxml1.xml");
#endif

/**
 * Constructor. Sets the test step name so that it can be 
 * differentiated from within doTestStepL()
 */
CXmlParser2Step::CXmlParser2Step(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

/**
 * TEF invokes doTestStepL interface along with the step name
 * to let appropriate action to be taken for the test step.
 */
TVerdict CXmlParser2Step::doTestStepL(void)
	{
	if (TestStepName() == KXmlParser2SetContentSink)
		{
		INFO_PRINTF1(KXmlParser2SetContentSink);
		SetTestStepResult(TestKXmlParser2SetContentSink());
		}
	else if (TestStepName() == KXmlParser2OOMTests)
		{
		INFO_PRINTF1(KXmlParser2OOMTests);
		SetTestStepResult(TestKXmlParser2OOMTests());
		}
	else if (TestStepName() == KXmlParser2OOMTests2)
		{
		INFO_PRINTF1(KXmlParser2OOMTests2);
		SetTestStepResult(TestKXmlParser2OOMTests2());
		}
	else if (TestStepName() == KXmlParser2OOMTests3)
		{
		INFO_PRINTF1(KXmlParser2OOMTests3);
		SetTestStepResult(TestKXmlParser2OOMTests3());
		}
	else if (TestStepName() == KXmlParser2OOMTests4)
		{
		INFO_PRINTF1(KXmlParser2OOMTests4);
		SetTestStepResult(TestKXmlParser2OOMTests4());
		}
	return TestStepResult();
	}

TVerdict CXmlParser2Step::TestKXmlParser2SetContentSink()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());

	CTestContentHandler *sax = CTestContentHandler::NewL();
	RFile handle;
	TInt err;
	TRAP(err,handle.Open(fs, KSimpleXmlFile, EFileShareReadersOnly));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Error opening test XML file"));
		User::Leave(err);
		}
	CleanupClosePushL(handle);
	CMatchData *matchData = CMatchData::NewL();
	matchData->SetMimeTypeL(_L8("text/xml"));
	matchData->SetVariantL(_L8("libxml2"));
	CParser *parser = CParser::NewL(*matchData, *sax);
	TInt retVal = KErrNone;
	//Valid feature
	retVal = parser->EnableFeature(EReportNamespaceMapping);
	if (retVal != KErrNone)
		{
		INFO_PRINTF1(_L("Error Enabling valid feature"));
		return EFail;
		}
	//Invalid feature
	TInt EInvalidFeatureId = 0x1111;
	retVal = parser->EnableFeature(EInvalidFeatureId);
	if (retVal != KErrNotSupported)
		{
		INFO_PRINTF1(_L("Unexpected error while enabling invalid feature"));
		return EFail;
		}
	//Valid feature
	retVal = parser->DisableFeature(EReportNamespaces);
	if (retVal != KErrNone)
		{
		INFO_PRINTF1(_L("Error Disabling valid feature"));
		return EFail;
		}

	//Invalid feature
	retVal = parser->DisableFeature(EInvalidFeatureId);
	if (retVal != KErrNotSupported)
		{
		INFO_PRINTF1(_L("Unexpected error while disabling invalid feature"));
		return EFail;
		}
	RContentProcessorUids *uid = new (ELeave)RContentProcessorUids;
	const TUid KXmlContProcUid =
		{
		0x20021CE5 //UID3
		};
	uid->Append(KXmlContProcUid);
	TRAP(err, parser->SetProcessorChainL(*uid));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Error Setting up processor chain"));
		return EFail;
		}
	TRAP(err,ParseL(*parser, handle));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Error Parsing"));
		return EFail;
		}
	delete parser;
	fs.Close();
	CleanupStack::Pop(&handle);
	return EPass;
	}
/**
 * This test step should be run only on debug builds owing to the 
 * __UHEAP macros that are defined only in debug builds.
 */
TVerdict CXmlParser2Step::TestKXmlParser2OOMTests()
	{
#ifdef _DEBUG
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CTestContentHandler *sax = CTestContentHandler::NewL();
	RFile handle;
	TInt err;
	CleanupClosePushL(handle);
	TRAP(err,handle.Open(fs, KSimpleXmlFile, EFileShareReadersOnly));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Error opening test XML file"));
		User::Leave(err);
		}

	CMatchData *matchData = CMatchData::NewL();
	matchData->SetMimeTypeL(_L8("text/xml"));
	matchData->SetVariantL(_L8("libxml2"));
	CParser *parser = CParser::NewL(*matchData, *sax);
	CleanupStack::PushL(parser);

	__UHEAP_SETFAIL(RHeap::EDeterministic, 40);
	TRAP(err,ParseL(*parser, handle));
	//it could be a random error
	if (err == KErrNone)
		{
		return EFail;
		}
	INFO_PRINTF1(_L("Tests passed at Heap failure rate :40"));
	parser->ParseBeginL();
	__UHEAP_SETFAIL(RHeap::EDeterministic, 50);
	TRAP(err,ParseL(*parser, handle));
	//it could be a random error
	if (err == KErrNone)
		{
		return EFail;
		}
	INFO_PRINTF1(_L("Tests passed at Heap failure rate :50"));
	parser->ParseBeginL();
	__UHEAP_SETFAIL(RHeap::EDeterministic, 100);
	TRAP(err,ParseL(*parser, handle));
	//it could be a random error
	if (err == KErrNone)
		{
		return EFail;
		}
	INFO_PRINTF1(_L("Tests passed at Heap failure rate :100"));
	parser->ParseBeginL();
	__UHEAP_SETFAIL(RHeap::ENone, 0);
	fs.Close();
	CleanupStack::Pop(parser);
	CleanupStack::Pop(&handle);
#else
	INFO_PRINTF1(_L("This test is meant to be run only in DEBUG builds. Returning EPASS"));
#endif
	return EPass;
	}

/**
 * This test step should be run only on debug builds owing to the 
 * __UHEAP macros that are defined only in debug builds.
 */
TVerdict CXmlParser2Step::TestKXmlParser2OOMTests2()
	{
#ifdef _DEBUG
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CTestContentHandler *sax = CTestContentHandler::NewL();
	RFile handle;
	TInt err;
	CleanupClosePushL(handle);
	TRAP(err,handle.Open(fs, KSimpleXmlFile, EFileShareReadersOnly));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Error opening test XML file"));
		User::Leave(err);
		}

	CMatchData *matchData = CMatchData::NewL();
	matchData->SetMimeTypeL(_L8("text/xml"));
	matchData->SetVariantL(_L8("libxml2"));
	CParser *parser = CParser::NewL(*matchData, *sax);
	CleanupStack::PushL(parser);

	__UHEAP_SETFAIL(RHeap::EDeterministic, 1);
	TRAP(err,ParseL(*parser, handle));
	if (err != KErrNoMemory)
		{
		return EFail;
		}
	INFO_PRINTF1(_L("Tests passed at Heap failure rate :1"));
	__UHEAP_SETFAIL(RHeap::EDeterministic, 33);
	TRAP(err,ParseL(*parser, handle));
	//it could be a random error
	if (err == KErrNone)
		{
		return EFail;
		}
	INFO_PRINTF1(_L("Tests passed at Heap failure rate :33"));
	__UHEAP_SETFAIL(RHeap::ENone, 0);
	
  fs.Close();
	CleanupStack::Pop(parser);
	CleanupStack::Pop(&handle);
#else
	INFO_PRINTF1(_L("This test is meant to be run only in DEBUG builds. Returning EPASS"));
#endif
	return EPass;
	}
/**
 * This test step should be run only on debug builds owing to the 
 * __UHEAP macros that are defined only in debug builds.
 */
TVerdict CXmlParser2Step::TestKXmlParser2OOMTests3()
	{
#ifdef _DEBUG
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CTestContentHandler *sax = CTestContentHandler::NewL();
	RFile handle;
	TInt err;
	CleanupClosePushL(handle);
	TRAP(err,handle.Open(fs, KSimpleXmlFile, EFileShareReadersOnly));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Error opening test XML file"));
		User::Leave(err);
		}

	CMatchData *matchData = CMatchData::NewL();
	matchData->SetMimeTypeL(_L8("text/xml"));
	matchData->SetVariantL(_L8("libxml2"));
	CParser *parser = CParser::NewL(*matchData, *sax);
	CleanupStack::PushL(parser);

	__UHEAP_SETFAIL(RHeap::EDeterministic, 32);
	TRAP(err,ParseL(*parser, handle));
	//it could be a random error
	if (err == KErrNone)
		{
		return EFail;
		}
	INFO_PRINTF1(_L("Tests passed at Heap failure rate :32"));
	__UHEAP_SETFAIL(RHeap::ENone, 0);

	fs.Close();
	CleanupStack::Pop(parser);
	CleanupStack::Pop(&handle);
#else
	INFO_PRINTF1(_L("This test is meant to be run only in DEBUG builds. Returning EPASS"));
#endif
	return EPass;
	}

TVerdict CXmlParser2Step::TestKXmlParser2OOMTests4()
	{
#ifdef _DEBUG
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CTestContentHandler *sax = CTestContentHandler::NewL();
	RFile handle;
	TInt err;
	CleanupClosePushL(handle);
	TRAP(err,handle.Open(fs, KCorruptXm1File, EFileShareReadersOnly));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Error opening test XML file"));
		User::Leave(err);
		}

	CMatchData *matchData = CMatchData::NewL();
	CleanupStack::PushL(matchData);
	matchData->SetMimeTypeL(_L8("text/xml"));
	matchData->SetVariantL(_L8("libxml2"));
	CParser *parser = CParser::NewL(*matchData, *sax);
	CleanupStack::PushL(parser);
	
	__UHEAP_SETFAIL(RHeap::EDeterministic, 72);
	
	TRAP(err,ParseL(*parser, handle););
	TInt EXmlTagMismatch = -993; //copied from xmlparseerrors.h
	if (err != EXmlTagMismatch)
		{
		return EFail;
		}
	__UHEAP_SETFAIL(RHeap::ENone, 0);

	fs.Close();
	CleanupStack::Pop(parser);
	CleanupStack::Pop(matchData);
	CleanupStack::Pop(&handle);
#else
	INFO_PRINTF1(_L("This test is meant to be run only in DEBUG builds. Returning EPASS"));
#endif
	return EPass;
	}
