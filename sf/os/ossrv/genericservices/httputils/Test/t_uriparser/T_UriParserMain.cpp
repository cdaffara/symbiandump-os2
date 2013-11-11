// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// MEMORY LEAK TESTING - Change and recompile to enable memory leak testing
// #define __MEMORY_LEAK_TESTING_ENABLED


// System includes
//
#include <e32base.h>

// Local include
//
#include "IpuTestUtils.h"
#include "CUriAndAuthorityTest.h"
#include "CDelimitedPathTest.h"
#include "CDelimitedPathSegmentTest.h"
#include "CDelimitedQueryTest.h"
#include "CEscapeUtilsTest.h"
#include "CTextUtilsTest.h"
#include "CUriUtilsTest.h"
#include "CSipUriAndAuthorityTest.h"


// Constants and definions
//
_LIT(KUriParserTestPanic, "T_Uri");
_LIT(KUriTestTitle, "T_Uri Test Harness");

GLDEF_C void Panic(TInt aPanic);

//	Local/Global function implementations
//
GLDEF_C void Panic(TInt aPanic)
//
//	Panic the client program.
	{
	User::Panic(KUriParserTestPanic,aPanic);
	}

GLDEF_C void TestL()
	{
	CIpuTestHarness* testHarness = CIpuTestHarness::NewL(KUriTestTitle);
	CleanupStack::PushL(testHarness);
	testHarness->DoResourceLeakTest(ETrue);

	testHarness->LogIt(_L ("@SYMTestCaseID IWS-APPPROTOCOLS-INETPROTUTIL-T_URIPARSERMAIN-0001 "));

	CEscapeUtilsTest* escapeUtilsTest = CEscapeUtilsTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*escapeUtilsTest,*testHarness);
#endif
	escapeUtilsTest->DoTestsL();

	CTextUtilsTest* textUtilsTest = CTextUtilsTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*textUtilsTest,*testHarness);
#endif
	textUtilsTest->DoTestsL();

	CUriUtilsTest* uriUtilsTest = CUriUtilsTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*uriUtilsTest,*testHarness);
#endif
	uriUtilsTest->DoTestsL();

	CUriAndAuthorityTest* uriAndAuthorityTest = CUriAndAuthorityTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*uriAndAuthorityTest,*testHarness);
#endif
	uriAndAuthorityTest->DoTestsL();
	
	CDelimitedPathTest* delimitedPathTest = CDelimitedPathTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*delimitedPathTest,*testHarness);
#endif
	delimitedPathTest->DoTestsL();

	CDelimitedPathSegmentTest* delimitedPathSegmentTest = CDelimitedPathSegmentTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*delimitedPathSegmentTest,*testHarness);
#endif
	delimitedPathSegmentTest->DoTestsL();

	CDelimitedQueryTest* delimitedQueryTest = CDelimitedQueryTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*delimitedQueryTest,*testHarness);
#endif
	delimitedQueryTest->DoTestsL();

	CSipUriAndAuthorityTest* sipUriAndAuthorityTest = CSipUriAndAuthorityTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*sipUriAndAuthorityTest,*testHarness);
#endif
	sipUriAndAuthorityTest->DoTestsL();

	CleanupStack::PopAndDestroy(9, testHarness);	//	lots!
	}

GLDEF_C TInt E32Main()
//
// Main function
	{
	__UHEAP_MARK;
	CTrapCleanup* tc=CTrapCleanup::New();
	TRAPD(err,TestL());
	if (err!=KErrNone)
		User::Panic(_L("Failed to complete test"),err);
	delete tc;
	__UHEAP_MARKEND;
	return KErrNone;
	}
