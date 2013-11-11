// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// t_main
// Main test harness for t_wspcodec
// 
//



// MEMORY LEAK TESTING - Change and recompile to enable memory leak testing
// #define __MEMORY_LEAK_TESTING_ENABLED



#include <e32base.h>
#include "IpuTestUtils.h"

#include "cwspdecodetest.h"
#include "cwspencodetest.h"

_LIT(KTestPanic, "WSPCodec");
_LIT(KTestTitle, "T_WSPCodec");

GLDEF_C void Panic(TInt aPanic);


GLDEF_C void Panic(TInt aPanic)
	{
	User::Panic(KTestPanic,aPanic);
	}

GLDEF_C void TestL()
	{
	CIpuTestHarness* testHarness = CIpuTestHarness::NewL(KTestTitle);
	CleanupStack::PushL(testHarness);
	testHarness->DoResourceLeakTest(ETrue);

	testHarness->LogIt(_L ("@SYMTestCaseID IWS-APPPROTOCOLS-INETPROTUTIL-T_WSPCODEC-0001 "));

	CWSPDecodeTest* decodeTest = CWSPDecodeTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*decodeTest,*testHarness);
#endif
	decodeTest->DoTestsL();

	CWSPEncodeTest* encodeTest = CWSPEncodeTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*encodeTest,*testHarness);
#endif
	encodeTest->DoTestsL();

	CleanupStack::PopAndDestroy(3, testHarness);
	}

GLDEF_C TInt E32Main()
//
// Main function
	{
	__UHEAP_MARK;
	CTrapCleanup* tc=CTrapCleanup::New();
	TRAPD(err,TestL());
	if (err!=KErrNone)
		User::Panic(_L("Test Fail"),err);
	delete tc;
	__UHEAP_MARKEND;
	return KErrNone;
	}

