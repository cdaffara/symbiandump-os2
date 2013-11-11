// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CFileUriTest.h"


// Constants and definions
//
_LIT(KUriParserTestPanic, "T_FileUri");
_LIT(KUriTestTitle, "T_FileUri Test Harness");

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

	testHarness->LogIt(_L ("@SYMTestCaseID IWS-APPPROTOCOLS-INETPROTUTIL-T_FILEURIMAIN-0001 "));

	CFileUriTest* fileUriTest = CFileUriTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*fileUriTest,*testHarness);
#endif
	fileUriTest->DoTestsL();

	CleanupStack::PopAndDestroy(2, testHarness);	//testHarness, fileUriTest
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
