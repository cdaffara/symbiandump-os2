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
// Main test harness for t_tinternetdate
// 
//



// MEMORY LEAK TESTING - Change and recompile to enable memory leak testing
// #define __MEMORY_LEAK_TESTING_ENABLED



#include <e32base.h>
#include "IpuTestUtils.h"

#include "cconverttorfc1123formtest.h"

_LIT(KTestPanic, "TInetDate");
_LIT(KTestTitle, "T_TInternetDate");

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

	testHarness->LogIt(_L ("@SYMTestCaseID IWS-APPPROTOCOLS-INETPROTUTIL-T_TINTERNETDATE-0001 "));

	CConvertToRfc1123FormTest* convertToRfc1123FormTest = CConvertToRfc1123FormTest::NewLC(testHarness);
#ifdef __MEMORY_LEAK_TESTING_ENABLED
	CTestBase::MemoryLeakTestL(*convertToRfc1123FormTest,*testHarness);
#endif
	convertToRfc1123FormTest->DoTestsL();

	CleanupStack::PopAndDestroy(2, testHarness);
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

