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
// This tests the failure of CExpat's internal heap.  It is a separate test executable
// because it is statically linked to the xml parser object code and bypasses Ecom
// 
//

#include <e32test.h>
#include "contenthandlers.h"
#include "xmlparserplugin.h"

#include <xml/plugins/charsetconverter.h>
#include <xml/stringdictionarycollection.h>

using namespace Xml;

GLDEF_D RTest test(_L("Expat heap test"));  // must be called test, as the e32test macros rely on this


TPtrC8 KNamespaceTest =
	_L8("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
	"<!--"
	"	namespace.xml - xml test file with known content for use with t_xmlparser"
	"	Copyright (c) 2003, Symbian Ltd."
	"	All Rights Reserved"
	"-->"
	""
	"<RootElement rabbits:att=\"value\">"
	" <Element xmlns=\"http://www.symbian.com\" att=\"value\">"
	"  <elprefix:Element xmlns:elprefix=\"http://element.uri\" att=\"value\">"
	"   <Element xmlns:attprefix=\"http://attribute.uri\" attprefix:att=\"value\"/>"
	"   <Element attprefix:att=\"value\"/>"
	"  </elprefix:Element>"
	" </Element>"
	"</RootElement>");


// ---------------------------------------------------


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3746
@SYMTestCaseDesc		    Creating OOM conditions and parsing.
@SYMTestPriority 		    Medium
@SYMTestActions  		    In OOM conditions, creating a parser and then parsing chunks.
@SYMTestExpectedResults 	The parser should behave as expected in the OOM conditions.
@SYMPREQ 		 		 	PREQ230
*/
LOCAL_C void ExpatOomTestL()
	{
	//
	// Allocate paraphernalia required for CXmlParserPlugin
	//
	TSimpleContentHandler contentHandler;

	RElementStack elementStack;
	CleanupClosePushL(elementStack);

	CCharSetConverter* charSetConverter = CCharSetConverter::NewL();
	CleanupStack::PushL(charSetConverter);

	RStringDictionaryCollection stringDictionaryCollection;
	stringDictionaryCollection.OpenL();
	CleanupClosePushL(stringDictionaryCollection);

	TParserInitParams initParams;
	initParams.iCharSetConverter = charSetConverter;
	initParams.iContentHandler = &contentHandler;
	initParams.iStringDictionaryCollection = &stringDictionaryCollection;
	initParams.iElementStack = &elementStack;


	/*
	The following fragment forces the internal heap to fail during parser
	construction and checks that this causes no leaks from the user heap
	*/
	TInt error;
	TInt count = 0;
	CXmlParserPlugin* parser = 0;
	User::__DbgMarkStart(RHeap::EUser);

	do
		{
		User::__DbgMarkEnd(RHeap::EUser,0);
		User::__DbgMarkStart(RHeap::EUser);
		count++;
		TRAP(error, parser=CXmlParserPlugin::NewL(&initParams, count))
		} while(error==KErrNoMemory);

	CleanupReleasePushL(*parser);


	/*
	Next we parse the namespace test file and cause the heap to heap to fail.

	Using the namespace test file ensures Expat is allocating prefixes and uri
	bindings internally.
	*/
	count = 0;
	RHeap* heap = parser->GetInternalHeap();
	heap->__DbgMarkStart();

	TInt err=KErrNone;
	do
		{
		contentHandler.iError = KErrNone;

		heap->__DbgMarkEnd(0);
		heap->__DbgSetAllocFail(RHeap::EFailNext, ++count);
		heap->__DbgMarkStart();
		TRAP(err, parser->ParseLastChunkL(KNamespaceTest));
		} while(err==KErrNoMemory);

	test(err==KErrNone);
	heap->__DbgMarkEnd(0);


	/*
	Now set the user heap to fail on the next allocation and parse the
	document again.  This causes the CExpat handler function to leave,
	in turn causing the parser to be de-allocated.

	This is in preparation for the next test.
	*/
	contentHandler.iError = KErrNone;
	heap->__DbgSetAllocFail(RHeap::ENone,1);  // __RHEAP_RESET
	heap->__DbgMarkStart();
	User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, 1);
	TRAP(err, parser->ParseLastChunkL(KNamespaceTest));
	test(err==KErrNoMemory);
	heap->__DbgMarkEnd(0);


	/*
	We have a CExpat in an error state with the internal parser deleted.  Now
	cause systematic heap failure while attempting to reset the parser and
	parse a document.

	This will test reset failure and correct handling in CXmlParserPlugin.
	*/
	count = 0;
	heap->__DbgMarkStart();

	do
		{
		heap->__DbgMarkEnd(0);
		heap->__DbgSetAllocFail(RHeap::EFailNext, ++count);
		heap->__DbgMarkStart();
		parser->EnableFeature(EReportNamespaceMapping);
		TRAP(err, parser->ParseLastChunkL(KNamespaceTest));

		} while(err==KErrNoMemory);

	test(err==KErrNone);
	heap->__DbgMarkEnd(0);

	CleanupStack::PopAndDestroy(4);
	User::__DbgMarkEnd(RHeap::EUser,0);
}


// ---------------------------------------------------
// RunTestL
// MainL
// E32Main
//
// Top-level functions


LOCAL_C void RunTestL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3746 Expat parser Out-of-Memory test "));

// Ridiculous device to get around MSVC6 linker-compiler plot to rule the world
#ifdef _DEBUG
	volatile TInt releaseBuild = 0;
#else
	volatile TInt releaseBuild = 1;
#endif

	if(releaseBuild)
		test.Printf(_L("This test is not supported in UREL builds\n"));
	else
		ExpatOomTestL();

	test.End();
	}

LOCAL_C void MainL()
	{
	CleanupClosePushL(test);

	TRAPD(error, RunTestL());
	test(error==KErrNone);

	CleanupStack::PopAndDestroy(&test);
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(error, MainL());

	delete cleanup;
	__UHEAP_MARKEND;

	return error;
	}
