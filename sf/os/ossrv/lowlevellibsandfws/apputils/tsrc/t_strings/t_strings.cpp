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

#define MY_STRINGPOOL pool

#include <e32std.h>
#include <e32test.h>
#include <f32file.h>
#include <WapTestUtils.h>
#include "http.h"

LOCAL_D CWapTestHarness* testHarness;

_LIT(KTitleStr, "HTTP String Pool Test Harness");
_LIT8(KHello, "hello");
_LIT8(KHello1, "hello");
_LIT8(KGoodbye, "goodbye");
const void* tableContent[] = { (const void*)&KHello,
											  (const void*)&KGoodbye};

const static TStringTable table = { 2,&tableContent[0]};

class CHTTPTest : public CBase
	{
public:
	static CHTTPTest* NewL();
	void DoTestsL();
	};

CHTTPTest* CHTTPTest::NewL()
	{
	return new (ELeave) CHTTPTest;
	}

void CHTTPTest::DoTestsL()
	{

	testHarness->StartTestL(_L("Creating the string pool"));
	RStringPool pool;
	pool.OpenL(table);
	testHarness->EndTest(0);
	testHarness->StartTestL(_L("Creating 'hello'"));
	STRDCI_L(hello, KHello);
	RStringF def;
	testHarness->EndTest(hello == def);
	testHarness->StartTestL(_L("Creating another 'hello'"));
	ASTRDCI_L(hello1, KHello1);
	testHarness->EndTest(!(hello == hello1.iObj));
	testHarness->StartTestL(_L("Testing !="));
	testHarness->EndTest(hello != hello1.iObj);
	testHarness->StartTestL(_L("Creating 'goodbye'"));
	STRDCI_L(goodbye, KGoodbye);
	testHarness->EndTest(hello == goodbye);
	testHarness->StartTestL(_L("Creating string tokens"));
	RStringTokenF h1, h2(hello), g;
	h1 = hello;
	testHarness->EndTest(h1 != h2);
	testHarness->StartTestL(_L("Testing == on string tokens"));
	testHarness->EndTest(!(h1 == h2));
	testHarness->StartTestL(_L("Making a string from a token"));
	hello1.iObj =  pool.StringF(h1);
	testHarness->EndTest(hello != hello1.iObj);
	testHarness->StartTestL(_L("Non-equal tokens"));
	g = goodbye;
	testHarness->EndTest(hello == goodbye);
	testHarness->StartTestL(_L("Operator != on non-equal tokens"));
	testHarness->EndTest(!(hello != goodbye));
	testHarness->StartTestL(_L("Copy function"));
	hello1.iObj = hello.Copy();
	testHarness->EndTest(hello != hello1.iObj);
	testHarness->StartTestL(_L("String extraction (same)"));
	testHarness->EndTest(static_cast<const TDesC8&>(hello).Compare(hello1.iObj));
	testHarness->StartTestL(_L("String extraction (different)"));
	testHarness->EndTest(!static_cast<const TDesC8&>(hello).Compare(goodbye));
	testHarness->StartTestL(_L("String extraction (original)"));
	testHarness->EndTest(static_cast<const TDesC8&>(hello).Compare(KHello));
	testHarness->StartTestL(_L("Closing pool"));
	hello.Copy();
	hello.Close();
	hello.Close();
	hello1.iObj.Close();
	// The copys here have got a bit confused
	hello1.iObj.Close();
	goodbye.Close();
	pool.CloseAll();
	testHarness->EndTest(0);	
	}

GLDEF_C void TestL()
	{
	testHarness = CWapTestHarness::NewLC(KTitleStr);
	// Set resource handle leak test
	testHarness->DoResourceLeakTest(ETrue);
	CHTTPTest* theTest = CHTTPTest::NewL();
	theTest->DoTestsL();
	delete theTest;
	CleanupStack::PopAndDestroy();	//	UrlTest, wapTest
	}

GLDEF_C TInt E32Main()
//
// Main function
	{
	__UHEAP_MARK;
	CTrapCleanup* tc=CTrapCleanup::New();
	TRAPD(err,TestL());
	if (err!=KErrNone)
		User::Panic(_L("TSTRING"),err);
	delete tc;
	__UHEAP_MARKEND;
	return KErrNone;
	}
