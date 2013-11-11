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

static RTest test(_L("t_wbxmlwmlparserstabilitytests"));


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3720
@SYMTestCaseDesc		    Parsing wml test documents.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Parsing files in the specified directory.
@SYMTestExpectedResults 	Test should pass.
@SYMREQ 		 		 	REQ0000
*/
static void WmlTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3720 WmlTestsL tests... "));

	__UHEAP_MARK;

	CStabilityTestClass* wmlTest = CStabilityTestClass::NewL(test);
	CleanupStack::PushL(wmlTest);
	wmlTest->StabilityTestL(KWml1_1Directory(), KWmlcExt(), &CStabilityTestClass::TestWholeL);
	CleanupStack::PopAndDestroy(wmlTest);

	CStabilityTestClass* wmlTestMid = CStabilityTestClass::NewL(test, EFalse, KMidChunkSize);
	CleanupStack::PushL(wmlTestMid);
	wmlTestMid->StabilityTestL(KWml1_1Directory(), KWmlcExt(), &CStabilityTestClass::TestChunkL);
	CleanupStack::PopAndDestroy(wmlTestMid);

	__UHEAP_MARKEND;
	}


static void MainL()
	{
	WmlTestsL();
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
