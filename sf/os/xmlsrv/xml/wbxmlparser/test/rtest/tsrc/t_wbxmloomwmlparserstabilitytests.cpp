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

#include "stabilitytestclass.h"
#include "testdocuments.h"

static RTest test(_L("t_wbxmloomwmlparserstabilitytests"));


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3727
@SYMTestCaseDesc		    OOM Testing.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Parsing wml documents.
@SYMTestExpectedResults 	Parser should be able to cope with parsing.
@SYMREQ 		 		 	REQ0000
*/
static void WmlOomTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3727 WmlOomTestsL tests... "));

	__UHEAP_MARK;

	CStabilityTestClass* wmlTest = CStabilityTestClass::NewL(test, ETrue);
	CleanupStack::PushL(wmlTest);
	wmlTest->StabilityTestL(KWml1_1Directory(), KWmlcExt(), &CStabilityTestClass::TestWholeL);
	CleanupStack::PopAndDestroy(wmlTest);

	CStabilityTestClass* wmlTestMid = CStabilityTestClass::NewL(test, ETrue, KMidChunkSize);
	CleanupStack::PushL(wmlTestMid);
	wmlTestMid->StabilityTestL(KWml1_1Directory(), KWmlcExt(), &CStabilityTestClass::TestChunkL);
	CleanupStack::PopAndDestroy(wmlTestMid);

	__UHEAP_MARKEND;
	}


static void MainL()
	{
	WmlOomTestsL();
	}


TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("initialising"));

	CTrapCleanup* c=CTrapCleanup::New();

	// start the loader
	RFs fs;
	test (fs.Connect()==KErrNone);
	fs.Close();

	test (c!=0);
	TRAPD(r,MainL());
	test (r==KErrNone);
	delete c;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
	}
